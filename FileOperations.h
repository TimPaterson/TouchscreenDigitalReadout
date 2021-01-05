//****************************************************************************
// FileOperations.h
//
// Created 1/2/2021 4:23:07 PM by Tim
//
//****************************************************************************

#pragma once

#include "FatFileDef.h"
#include <FatFile/FatDrive.h>


// Use macros for state definitions
#define FLASH_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, read)
#define DIR_OP_STATES(op) OP_STATE(op, enumNext)
#define SINGLE_OP_STATES(op) OP_STATE(op, ready)

#define OP_STATE(op, st)	ST_##op##_##st,

enum
{
	ST_Idle,
	FLASH_OP_STATES(flash)
	DIR_OP_STATES(dir)
	SINGLE_OP_STATES(single)
};

#undef OP_STATE
#define OP_STATE(op, st)	case ST_##op##_##st: {
#define END_STATE			} return;
#define TO_STATE(op, st)	state = ST_##op##_##st
#define OP_DONE				break


class FileOperations : public FatSys
{
public:
	void WriteFileToFlash(const char *psz, ulong addr)
	{
		flash.addr = addr;
		hFile = StartOpen(psz, 0, OPENFLAG_OpenExisting | OPENFLAG_File);
		TO_STATE(flash, open);
	}

	void FileDirectory(char *psz, int cb)
	{
		dir.psz = psz;
		dir.cb = cb;
		dir.cnt = 0;
		hFile = EnumBegin(0);
		StartEnumNext(hFile, psz, cb);
		TO_STATE(dir, enumNext);
	}

	void Mount(int drv)
	{
		drive = drv;
		StartMount(drv);
		TO_STATE(single, ready);
	}

public:
	bool IsBusy()	{ return state != ST_Idle; }

public:
	void Process()
	{
		int		status;

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
					int cb = GetSize(hFile);
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
					int cb = status;
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
			}
		}

		// Executed only when operation completed
		state = ST_Idle;
		hFile = 0;
	}

protected:
	union
	{
		// WriteFileToFlash
		struct  
		{
			ulong	addr;
		} flash;

		// FileDirectory
		struct  
		{
			char	*psz;
			ushort	cb;
			ushort	cnt;
		} dir;
	};
	byte	state;
	byte	hFile;
	byte	drive;
};
