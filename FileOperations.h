//****************************************************************************
// FileOperations.h
//
// Created 1/2/2021 4:23:07 PM by Tim
//
//****************************************************************************

#pragma once

#include "FatFileDef.h"
#include <FatFile/FatSys.h>


static constexpr int FileBufSectors = 8;
extern byte g_FileBuf[FileBufSectors][FAT_SECT_SIZE] ALIGNED_ATTR(uint32_t);
#define FILE_BUF_END ((byte *)g_FileBuf[FileBufSectors])

// Use macros for state definitions
#define FLASH_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, read)
#define SINGLE_OP_STATES(op) OP_STATE(op, ready)
#define IMPORT_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, readStart) OP_STATE(op, read0) OP_STATE(op, read1)
#define FOLDER_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, name) OP_STATE(op, date)

#define OP_STATE(op, st)	ST_##op##_##st,

enum
{
	ST_Idle,
	FLASH_OP_STATES(flash)
	SINGLE_OP_STATES(single)
	IMPORT_OP_STATES(import)
	FOLDER_OP_STATES(folder)
};

#undef OP_STATE
#define OP_STATE(op, st)	case ST_##op##_##st: {
#define END_STATE			} return;
#define TO_STATE(op, st)	m_state = ST_##op##_##st
#define OP_DONE				break


enum FileInfoType
{
	INFO_File,
	INFO_Folder,
	INFO_Parent
};

struct FileEnumInfo
{
	ulong			Size;
	FatDateTime		DateTime;
	FileInfoType	Type;
	char			Name[];

	FileEnumInfo *Next(int cbName)
	{
		int offset = offsetof(FileEnumInfo, Name) + cbName;
		offset = (offset + 3) & ~3;	// 32-bit align
		return (FileEnumInfo *)ADDOFFSET(this, offset);
	}
};

#define MAX_PATH	255


class FileOperations : public FatSys
{
public:
	void Process();
	int WriteFileToFlash(const char *psz, ulong addr);
	int Mount(int drv);
	int ToolImport(const char *psz);
	int FolderEnum(const char *pFilename, int cchName = 0);

public:
	bool IsBusy()	{ return m_state != ST_Idle; }

protected:
	union
	{
		// WriteFileToFlash
		struct  
		{
			ulong	addr;
		} flash;

		// ToolImport
		struct  
		{
			ushort	cbLeft;
		} import;

		// FolderEnum
		struct  
		{
			FileEnumInfo *pInfo;
			ushort	cbName;
			ushort	cnt;
		} folder;
	};
	byte	m_state;
	byte	m_hFile;
	byte	m_drive;
};

extern FileOperations FileOp;
