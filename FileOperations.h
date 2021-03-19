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
#define FLASH_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, seek) OP_STATE(op, read) OP_STATE(op, wait)
#define MOUNT_OP_STATES(op) OP_STATE(op, ready)
#define IMPORT_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, readStart) OP_STATE(op, erase) OP_STATE(op, read0) OP_STATE(op, read1)
#define EXPORT_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, write) OP_STATE(op, flush) OP_STATE(op, close)
#define FOLDER_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, close) OP_STATE(op, name) OP_STATE(op, date)
#define HEADER_OP_STATES(op) OP_STATE(op, open) OP_STATE(op, read)
#define UPDATE_OP_STATES(op) OP_STATE(op, seek) OP_STATE(op, read)

#define STATE(op, st)		ST_##op##_##st
#define OP_STATE(op, st)	STATE(op, st),

enum
{
	ST_Idle,
	FLASH_OP_STATES(flash)
	MOUNT_OP_STATES(mount)
	IMPORT_OP_STATES(import)
	EXPORT_OP_STATES(Export)
	FOLDER_OP_STATES(folder)
	HEADER_OP_STATES(header)
	UPDATE_OP_STATES(update)
};

#undef OP_STATE
#define OP_STATE(op, st)	case STATE(op, st): {
#define END_STATE			} return;
#define EXIT_STATE			return;
#define TO_STATE(op, st)	m_state = STATE(op, st)
#define OP_DONE				break


enum FileInfoType
{
	INFO_File,
	INFO_Folder,
	INFO_Parent,
	INFO_Error,
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
	typedef int ErrorHandler(int err);

public:
	void Process();
	int Mount(int drv);
	int FolderEnum(const char *pFilename, int drive, int cchName = FAT_NO_NAME_LEN, bool fCreate = false);

public:
	int WriteFileToFlash(const char *psz, ulong addr, int drive = 0)
	{
		flash.addr = addr;
		return Open(psz, drive, OPENFLAG_OpenExisting | OPENFLAG_File, STATE(flash, open));
	}

	int WriteToFlash(ulong addr, ulong cb, ulong offset, uint hFile)
	{
		flash.addr = addr;
		flash.cbTotal = cb;
		return Seek(offset, hFile, STATE(flash, seek));
	}

	int ToolImport(const char *psz, int drive)
	{
		return Open(psz, drive, OPENFLAG_OpenExisting | OPENFLAG_File, STATE(import, open));
	}

	int ToolExport(const char *psz, int drive)
	{
		return Open(psz, drive, OPENFLAG_CreateAlways | OPENFLAG_File, STATE(Export, open));
	}

	int ReadUpdateHeader(const char *psz, int drive)
	{
		return Open(psz, drive, OPENFLAG_OpenExisting | OPENFLAG_File, STATE(header, open));
	}

	int ReadFirmware(ulong addr, uint cb, ulong offset, uint hFile)
	{
		update.addr = addr;
		update.cb = cb;
		return Seek(offset, hFile, STATE(update, seek));
	}

public:
	int Open(const char *psz, int drive, uint flags, uint state)
	{
		int		err;

		err = StartOpen(psz, HandleOfDrive(drive), flags);
		if (IsError(err))
			return m_pfnError(err);
		m_state = state;
		m_hFile = err;
		return FATERR_None;
	}

	int Seek(ulong offset, uint hFile, uint state)
	{
		int		err;

		m_hFile = hFile;
		err = StartSeek(hFile, offset);
		if (IsError(err))
			return m_pfnError(err);
		m_state = state;
		return FATERR_None;
	}

public:
	bool IsBusy()		{ return m_state != ST_Idle; }
	void OpDone()
	{
		m_state = ST_Idle;
		m_hFile = 0;
	}

	void SetErrorHandler(ErrorHandler *pfn = NULL)	
	{ 
		if (pfn != NULL)
			m_pfnError = pfn;
		else
			m_pfnError = NoErrorHandler;
	}

protected:
	static int NoErrorHandler(int err) 
	{ 
		DEBUG_PRINT("File error %i\n", err); 
		return err;
	}

protected:
	ErrorHandler	*m_pfnError = NoErrorHandler;

	union
	{
		// WriteFileToFlash
		struct  
		{
			ulong	addr;
			long	cbTotal;
			ushort	cb;
			ushort	erased;
			ushort	oBuf;
			byte	iBuf;
		} flash;

		// ToolImport
		struct  
		{
			void	*pErase;
			char	*pBuf;
			ushort	cbLeft;
		} import;

		// ToolExport
		struct  
		{
			char	*pBuf;
			ushort	iTool;
			ushort	iBuf;
		} Export;

		// FolderEnum
		struct  
		{
			FileEnumInfo *pInfo;
			ushort	cbName;
			ushort	cnt;
		} folder;

		// ReadFirmware
		struct  
		{
			ulong	addr;
			int		cb;
		} update;
	};
	byte	m_state;
	byte	m_hFile;
	byte	m_drive;
};

extern FileOperations FileOp;
