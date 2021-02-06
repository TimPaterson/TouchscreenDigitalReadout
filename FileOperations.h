//****************************************************************************
// FileOperations.h
//
// Created 1/2/2021 4:23:07 PM by Tim
//
//****************************************************************************

#pragma once

#include "FatFileDef.h"
#include <FatFile/FatSys.h>


// Use macros for state definitions
#define FLASH_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, read)
#define DIR_OP_STATES(op) OP_STATE(op, enumNext)
#define SINGLE_OP_STATES(op) OP_STATE(op, ready)
#define IMPORT_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, readStart) OP_STATE(op, read0) OP_STATE(op, read1)

#define OP_STATE(op, st)	ST_##op##_##st,

enum
{
	ST_Idle,
	FLASH_OP_STATES(flash)
	DIR_OP_STATES(dir)
	SINGLE_OP_STATES(single)
	IMPORT_OP_STATES(import)
};

#undef OP_STATE
#define OP_STATE(op, st)	case ST_##op##_##st: {
#define END_STATE			} return;
#define TO_STATE(op, st)	state = ST_##op##_##st
#define OP_DONE				break


class FileOperations : public FatSys
{
public:
	void Process();
	void WriteFileToFlash(const char *psz, ulong addr);
	void FileDirectory(char *psz, int cb);
	void Mount(int drv);
	void ToolImport(const char *psz);

public:
	bool IsBusy()	{ return state != ST_Idle; }

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

		struct  
		{
			ushort	cbLeft;
		} import;
	};
	byte	state;
	byte	hFile;
	byte	drive;
};
