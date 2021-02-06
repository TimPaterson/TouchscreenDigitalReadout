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


void FileOperations::WriteFileToFlash(const char *psz, ulong addr)
{
	flash.addr = addr;
	hFile = StartOpen(psz, 0, OPENFLAG_OpenExisting | OPENFLAG_File);
	TO_STATE(flash, open);
}

void FileOperations::FileDirectory(char *psz, int cb)
{
	dir.psz = psz;
	dir.cb = cb;
	dir.cnt = 0;
	hFile = EnumBegin(0);
	StartEnumNext(hFile, psz, cb);
	TO_STATE(dir, enumNext);
}

void FileOperations::Mount(int drv)
{
	drive = drv;
	StartMount(drv);
	TO_STATE(single, ready);
}

void FileOperations::ToolImport(const char *psz)
{
	hFile = StartOpen(psz, 0, OPENFLAG_OpenExisting | OPENFLAG_File);
	TO_STATE(import, open);
}

void FileOperations::Process()
{
	int		status;
	int		cb;

	if (state == ST_Idle)
		return;

	if (hFile != 0)
		status = GetStatus(hFile);
	else
		status = GetDriveStatus(drive);

	if (status == FATERR_Busy)
		return;

	if (IsError(status) && status != FATERR_FileNotFound)
		DEBUG_PRINT("File error %i\n", status);
	else
	{
		switch (state)
		{
			//*************************************************************
			// WriteFileToFlash

			OP_STATE(flash, open)
				cb = GetSize(hFile);
				// Round up to full block size for erasure
				cb = (cb + SerialFlashBlockSize - 1) & ~(SerialFlashBlockSize - 1);
				StartRead(hFile, NULL, FAT_SECT_SIZE);
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
					StartRead(hFile, NULL, FAT_SECT_SIZE);
				}
				else
				{
					Close(hFile);
					DEBUG_PRINT("complete\n");
					OP_DONE;
				}
			END_STATE

			//*************************************************************
			// FileDirectory

			OP_STATE(dir, enumNext)
				// status is handle of next file
				int h = status;

				if (IsError(h))
				{
					Close(hFile);
					DEBUG_PRINT("%i files\n", dir.cnt);
					OP_DONE;
				}
				DEBUG_PRINT("%-20s", dir.psz);
				if (!IsFolder(h))
					DEBUG_PRINT(" %6li\n", GetSize(h));
				else
					DEBUG_PRINT("\n");
				Close(h);
				dir.cnt++;
				StartEnumNext(hFile, dir.psz, dir.cb);
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
				StartRead(hFile, ToolLib::s_arImportBuf[0], FAT_SECT_SIZE);
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
						cb = ToolLib::ImportTools((char *)&ToolLib::s_arImportBuf[2] - import.cbLeft, import.cbLeft, cb);
					else
						cb = ToolLib::ImportTools((char *)&ToolLib::s_arImportBuf[0], cb, 0);

ImportNext:
					if (cb < 0)
						goto ImportClose;	// UNDONE: error handling

					import.cbLeft = cb;
					StartRead(hFile, ToolLib::s_arImportBuf[1], FAT_SECT_SIZE);
					TO_STATE(import, read1);
				}
				else
				{
ImportClose:
					Close(hFile);
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
					cb = ToolLib::ImportTools((char *)&ToolLib::s_arImportBuf[1] - import.cbLeft, import.cbLeft + cb, 0);
					if (cb < 0)
						goto ImportClose;	// UNDONE: error handling

					import.cbLeft = cb;
					StartRead(hFile, ToolLib::s_arImportBuf[0], FAT_SECT_SIZE);
					TO_STATE(import, read0);
				}
				else
					goto ImportClose;
			END_STATE
		}
	}

	// Executed only when operation completed
	state = ST_Idle;
	hFile = 0;
}
