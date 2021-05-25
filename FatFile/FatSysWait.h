//****************************************************************************
// FatSysWait.h
//
// Created 11/11/2020 1:37:06 PM by Tim
//
//****************************************************************************

#pragma once

#include <FatFile\FatSys.h>


typedef void (*WaitLoopFunction_t)();


template<WaitLoopFunction_t WaitLoopFunction>
class FatSysWait : public FatSys
{
public:
	static int WaitResult(uint handle) NO_INLINE_ATTR
	{
		int	status;

		do
		{
			WaitLoopFunction();
			status = GetStatus(handle);
		} while (status == FATERR_Busy);

		return status;
	}

	//****************************************************************************

	static int OpenWait(const char *pchName, int hFolder = 0, uint flags = OPENFLAG_File | OPENFLAG_Folder, int cchName = 0) NO_INLINE_ATTR
	{
		int		err;

		err = StartOpen(pchName, hFolder, flags, cchName);
		if (IsError(err))
			return err;

		hFolder = err;
		err = WaitResult(hFolder);
		if (IsError(err))
		{
			Close(hFolder);
			return err;
		}
		return hFolder;
	}

	//****************************************************************************

	static int CloseWait(uint handle) NO_INLINE_ATTR
	{
		int		err;

		err = StartClose(handle);
		if (err == FATERR_Busy)
			err = WaitResult(handle);
		return err;
	}

	//****************************************************************************

	static int ReadWait(uint handle, void *pv, int cb) NO_INLINE_ATTR
	{
		int		err;

		err = StartRead(handle, pv, cb);
		if (IsError(err))
			return err;

		return WaitResult(handle);
	}

	//****************************************************************************

	static int WriteWait(uint handle, void *pv, int cb) NO_INLINE_ATTR
	{
		int		err;

		err = StartWrite(handle, pv, cb);
		if (IsError(err))
			return err;

		return WaitResult(handle);
	}

	//*********************************************************************

	static int DeleteWait(const char *pchName, int hFolder = 0, uint flags = OPENFLAG_File | OPENFLAG_Folder, int cchName = 0) NO_INLINE_ATTR
	{
		uint	hFile;
		int		err;

		err = StartOpen(pchName, hFolder, flags | OPENFLAG_Delete, cchName);
		if (IsError(err))
			return err;

		hFile = err;
		err = WaitResult(hFile);
		if (IsError(err))
			return err;

		err = StartDelete(hFile);
		if (!(IsError(err)))
			err = WaitResult(hFile);

		return err;
	}

	//*********************************************************************

	static int EnumNextWait(uint handle, char *pch, int cbMax) NO_INLINE_ATTR
	{
		uint	hFile;
		int		err;

		err = StartEnumNext(handle, pch, cbMax);
		if (IsError(err))
			return err;

		hFile = err;
		err = WaitResult(hFile);
		if (IsError(err))
		{
			Close(hFile);
			return err;
		}
		return hFile;
	}

	//*********************************************************************

	static FatDateTime GetDateWait(uint handle) NO_INLINE_ATTR
	{
		FatDateTime		date;

		if (IsError(StartGetDate(handle)) || IsError(WaitResult(handle)))
		{
			date.ul = (uint)-1;
			return date;
		}

		return GetFatDate(handle);
	}

	//*********************************************************************

	static int RenameWait(const char *pchName, int hFolder, uint hFileSrc, int cchName = 0) NO_INLINE_ATTR
	{
		int		err;

		err = StartRename(pchName, hFolder, hFileSrc, cchName);
		if (IsError(err))
			return err;

		return WaitResult(hFileSrc);
	}

	//*********************************************************************

	static ulong SeekWait(uint handle, ulong ulPos, int origin = FAT_SEEK_SET) NO_INLINE_ATTR
	{
		ulong	ulCurPos;
		int		err;

		ulCurPos = StartSeek(handle, ulPos, origin);
		if (ulCurPos != (ulong)-1)
		{
			err = WaitResult(handle);
			if (IsError(err))
				return -1;
		}
		return ulCurPos;
	}

	//*********************************************************************

	static int MountWait(uint drive) NO_INLINE_ATTR
	{
		int		err;

		err = StartMount(drive);
		while (err == FATERR_Busy)
		{
			WaitLoopFunction();
			err = GetDriveStatus(drive);
		}

		return err;
	}
};
