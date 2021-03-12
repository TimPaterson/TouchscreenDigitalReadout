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
		return m_pfnError(err);
	m_hFile = err;
	TO_STATE(flash, open);
	return FATERR_None;
}

int FileOperations::Mount(int drv)
{
	int		err;

	err = StartMount(drv);
	if (IsErrorNotBusy(err))
		return m_pfnError(err);
	m_drive = drv;
	TO_STATE(single, ready);
	return FATERR_None;
}

int FileOperations::ToolImport(const char *psz, int drive)
{
	int		err;

	err = StartOpen(psz, HandleOfDrive(drive), OPENFLAG_OpenExisting | OPENFLAG_File);
	if (IsError(err))
		return m_pfnError(err);
	m_hFile = err;
	TO_STATE(import, open);
	return FATERR_None;
}

int FileOperations::ToolExport(const char *psz, int drive)
{
	int		err;

	err = StartOpen(psz, HandleOfDrive(drive), OPENFLAG_CreateAlways | OPENFLAG_File);
	if (IsError(err))
		return m_pfnError(err);
	m_hFile = err;
	TO_STATE(Export, open);
	return FATERR_None;
}

int FileOperations::FolderEnum(const char *pFilename, int drive, int cchName, bool fCreate)
{
	int		err;
	uint	flags;

	// See if we should create the folder if it doesn't exist
	flags = fCreate ? OPENFLAG_OpenAlways | OPENFLAG_Folder : OPENFLAG_OpenExisting | OPENFLAG_Folder;
	err = StartOpen(pFilename, HandleOfDrive(drive), flags, cchName);
	if (IsError(err))
		return m_pfnError(err);
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
	{
		OpDone();
		m_pfnError(status);
	}
	else
	{
		switch (m_state)
		{
			//*************************************************************
			// WriteFileToFlash

			OP_STATE(flash, open)
				StartRead(m_hFile, g_FileBuf[0], FAT_SECT_SIZE);
				flash.iBuf = 0;
				TO_STATE(flash, read);
				flash.erased = RA8876::SerialMemEraseStart(flash.addr, SerialFlashBlockSize, 1);
			END_STATE

			OP_STATE(flash, read)
				// status is no. of bytes read
				flash.cb = status;
				flash.oBuf = 0;
				TO_STATE(flash, wait);
			END_STATE

			OP_STATE(flash, wait)
				if (RA8876::IsSerialMemBusy())
					EXIT_STATE
				if (flash.cb == 0)
				{
					Close(m_hFile);
					DEBUG_PRINT("complete\n");
					OP_DONE;
				}
				else
				{
					if (flash.erased == 0)
					{
						flash.erased = RA8876::SerialMemEraseStart(flash.addr, SerialFlashBlockSize, 1);
						EXIT_STATE
					}
					// Write next page
					cb = RA8876::SerialMemWriteStart(flash.addr, flash.cb, &g_FileBuf[flash.iBuf][flash.oBuf], 1);
					flash.addr += cb;
					flash.oBuf += cb;
					flash.cb -= cb;
					flash.erased -= cb;
					if (flash.cb == 0)
					{
						// Read next buffer
						flash.iBuf ^= 1;
						StartRead(m_hFile, &g_FileBuf[flash.iBuf], FAT_SECT_SIZE);
						TO_STATE(flash, read);
					}
				}
			END_STATE

			//*************************************************************
			// Single operation (Mount)

			OP_STATE(single, ready)
				DEBUG_PRINT("Complete\n");
				OpDone();
				Files.DriveMountComplete(m_drive);
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
			// ToolExport

			OP_STATE(Export, open)
				Export.pBuf = ToolLib::ExportStart();
				Export.iTool = 0;
				Export.iBuf = 0;
				TO_STATE(Export, write);
			END_STATE

			OP_STATE(Export, write)
				char	*pBuf, *pBufRes;
				int		cb;

				pBuf = Export.pBuf;
				if (Export.iBuf == FileBufSectors - 1)
				{
					// Wrapped into last buffer, go back to start
					cb = pBuf - (char *)g_FileBuf[Export.iBuf];
					// Copy data from last buffer up to first
					memcpy(g_FileBuf, pBuf, cb);
					pBuf = (char *)g_FileBuf[0] + cb;
					Export.iBuf = 0;
				}
				do 
				{
					pBufRes = ToolLib::ExportTool(pBuf, Export.iTool++);
					if (pBufRes == NULL)
					{
						// Reached end, flush last buffer
						cb = pBuf - (char *)g_FileBuf[Export.iBuf];
						StartWrite(m_hFile, g_FileBuf[Export.iBuf], cb);
						TO_STATE(Export, flush);
						EXIT_STATE
					}
					pBuf = pBufRes;
				} while (pBuf < (char *)g_FileBuf[Export.iBuf + 1]);

				Export.pBuf = pBuf;
				StartWrite(m_hFile, g_FileBuf[Export.iBuf], FAT_SECT_SIZE);
				Export.iBuf++;
			END_STATE

			OP_STATE(Export, flush)
				StartClose(m_hFile);
				TO_STATE(Export, close);
			END_STATE

			OP_STATE(Export, close)
				OpDone();
				ToolLib::ExportDone();	// start folder enumeration
			END_STATE

			//*************************************************************
			// Folder enumeration

			OP_STATE(folder, open)
				int		h;

				h = EnumBegin(m_hFile);
				// May have created new folder, so can't use Close()
				StartClose(m_hFile);
				m_hFile = h;
				folder.cnt = 0;
				folder.pInfo = (FileEnumInfo *)g_FileBuf;
				TO_STATE(folder, close);
			END_STATE

			OP_STATE(folder, close)
				m_drive = StartEnumNext(m_hFile, folder.pInfo->Name, MAX_PATH + 1);
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
	OpDone();
}
