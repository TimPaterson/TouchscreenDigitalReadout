//****************************************************************************
// FileOperations.cpp
//
// Created 2/5/2021 11:20:03 AM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "FileOperations.h"
#include "ToolLib.h"


// Global buffer for file operations
byte g_FileBuf[FileBufSectors][FAT_SECT_SIZE] ALIGNED_ATTR(uint32_t);


int FileOperations::WriteFileToFlash(const char *psz, ulong addr)
{
	int		err;

	flash.addr = addr;
	err = StartOpen(psz, 0, OPENFLAG_OpenExisting | OPENFLAG_File);
	if (IsError(err))
		return err;
	m_hFile = err;
	TO_STATE(flash, open);
	return FATERR_None;
}

int FileOperations::Mount(int drv)
{
	int		err;

	err = StartMount(drv);
	if (IsErrorNotBusy(err))
		return err;
	m_drive = drv;
	TO_STATE(single, ready);
	return FATERR_None;
}

int FileOperations::ToolImport(const char *psz)
{
	int		err;

	err = StartOpen(psz, 0, OPENFLAG_OpenExisting | OPENFLAG_File);
	if (IsError(err))
		return err;
	m_hFile = err;
	TO_STATE(import, open);
	return FATERR_None;
}

int FileOperations::FolderEnum(const char *pFilename, int cchName)
{
	int		err;

	err = StartOpen(pFilename, 0, OPENFLAG_OpenExisting | OPENFLAG_Folder, cchName);
	if (IsError(err))
		return err;
	m_hFile = err;
	TO_STATE(folder, open);
	return FATERR_None;
}

void FileOperations::Process()
{
	int		status;
	int		cb;

	if (m_state == ST_Idle)
		return;

	if (m_hFile != 0)
		status = GetStatus(m_hFile);
	else
		status = GetDriveStatus(m_drive);

	if (status == FATERR_Busy)
		return;

	if (IsError(status))
		DEBUG_PRINT("File error %i\n", status);
	else
	{
		switch (m_state)
		{
			//*************************************************************
			// WriteFileToFlash

			OP_STATE(flash, open)
				cb = GetSize(m_hFile);
				// Round up to full block size for erasure
				cb = (cb + SerialFlashBlockSize - 1) & ~(SerialFlashBlockSize - 1);
				StartRead(m_hFile, NULL, FAT_SECT_SIZE);
				TO_STATE(flash, read);

				WDT->CTRL.reg = 0;	// disable watchdog during long process
				RA8876::SerialMemErase(flash.addr, cb, 1);
				WDT->CTRL.reg = WDT_CTRL_ENABLE;
			END_STATE

			OP_STATE(flash, read)
				// status is no. of bytes read
				cb = status;
				if (cb > 0)
				{
					RA8876::SerialMemWrite(flash.addr, cb, GetDataBuf(), 1);
					flash.addr += cb;
					StartRead(m_hFile, NULL, FAT_SECT_SIZE);
				}
				else
				{
					Close(m_hFile);
					DEBUG_PRINT("complete\n");
					OP_DONE;
				}
			END_STATE

			//*************************************************************
			// Single operation (Mount)

			OP_STATE(single, ready)
				DEBUG_PRINT("Complete\n");
				OP_DONE;
			END_STATE

			//*************************************************************
			// ToolImport

			OP_STATE(import, open)
				StartRead(m_hFile, g_FileBuf[0], FAT_SECT_SIZE);
				import.cbLeft = 0;
				TO_STATE(import, readStart);
			END_STATE

			OP_STATE(import, readStart)
				cb = status;
				if (cb > 0)
				{
					cb = ToolLib::ImportTools(NULL, cb, 0);
					goto ImportNext;
				}
				else
					goto ImportClose;
			END_STATE

			OP_STATE(import, read0)
				// status is no. of bytes read
				cb = status;
				if (cb > 0)
				{
					if (import.cbLeft != 0)
						cb = ToolLib::ImportTools((char *)&g_FileBuf[2] - import.cbLeft, import.cbLeft, cb);
					else
						cb = ToolLib::ImportTools((char *)&g_FileBuf[0], cb, 0);

ImportNext:
					if (cb < 0)
						goto ImportClose;	// UNDONE: error handling

					import.cbLeft = cb;
					StartRead(m_hFile, g_FileBuf[1], FAT_SECT_SIZE);
					TO_STATE(import, read1);
				}
				else
				{
ImportClose:
					Close(m_hFile);
					ToolLib::ImportDone();
					DEBUG_PRINT("complete\n");
					OP_DONE;
				}
			END_STATE

			OP_STATE(import, read1)
				// status is no. of bytes read
				cb = status;
				if (cb > 0)
				{
					cb = ToolLib::ImportTools((char *)&g_FileBuf[1] - import.cbLeft, import.cbLeft + cb, 0);
					if (cb < 0)
						goto ImportClose;	// UNDONE: error handling

					import.cbLeft = cb;
					StartRead(m_hFile, g_FileBuf[0], FAT_SECT_SIZE);
					TO_STATE(import, read0);
				}
				else
					goto ImportClose;
			END_STATE

			//*************************************************************
			// Folder enumeration

			OP_STATE(folder, open)
				int		h;

				h = EnumBegin(m_hFile);
				Close(m_hFile);
				m_hFile = h;
				folder.cnt = 0;
				folder.pInfo = (FileEnumInfo *)g_FileBuf;
				m_drive = StartEnumNext(h, folder.pInfo->Name, MAX_PATH + 1);
				TO_STATE(folder, name);
			END_STATE

			OP_STATE(folder, name)
				if (status == 0)
				{
					// Zero-length name means we're done
EndEnum:
					Close(m_hFile);
					Files.FolderEnumDone(folder.cnt);
					OP_DONE;
				}
				folder.cbName = status;	// remember length
				folder.pInfo->Size = GetSize(m_drive);
				StartGetDate(m_drive);
				TO_STATE(folder, date);
			END_STATE

			OP_STATE(folder, date)
				ushort	offset;
				ushort	*pList;

				folder.pInfo->DateTime = GetFatDate(m_drive);

				// Get buffer position
				offset = (byte *)folder.pInfo - (byte *)g_FileBuf;
				pList = ((ushort *)FILE_BUF_END) - 1 - folder.cnt;

				if (IsFolder(m_drive))
				{
					if (folder.pInfo->Name[0] == '.')
					{
						if (folder.pInfo->Name[1] == '\0')
							goto NextFolder;	// skip folder '.'
						// UNDONE: handle ..
					}
					if (folder.pInfo->Name[1] == '.' && folder.pInfo->Name[2] == '\0')
						folder.pInfo->Type = INFO_Parent;
					else
						folder.pInfo->Type = INFO_Folder;
				}
				else
					folder.pInfo->Type = INFO_File;
					
				// Add to list at end of buffer
				*pList-- = offset;
				folder.cnt++;
				// Get offset for next FileEnumInfo
				// Note the name length does not include null terminator.
				folder.pInfo = folder.pInfo->Next(folder.cbName + 1);
NextFolder:
				Close(m_drive);
				// How much room is left in buffer?
				cb = (byte *)pList - (byte *)folder.pInfo - sizeof(FileEnumInfo);
				if (cb < FAT_MIN_NAME_BUF)
				{
					DEBUG_PRINT("Quit enum with %i bytes free:\n", cb);
					goto EndEnum;	// out of room, we're done
				}
				// Start next
				m_drive = StartEnumNext(m_hFile, folder.pInfo->Name, cb);
				TO_STATE(folder, name);
			END_STATE
		}
	}

	// Executed only when operation completed
	m_state = ST_Idle;
	m_hFile = 0;
}
