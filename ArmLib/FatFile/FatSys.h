//****************************************************************************
// FatSys.h
//
// Created 11/11/2020 11:08:07 AM by Tim
//
//****************************************************************************

#pragma once

// Note that the following symbols must be defined at this point:
//
// FAT_SECT_BUF_CNT	- number of sector buffers
// FAT_MAX_HANDLES	- max number of file handles
// FAT_NUM_DRIVES	- number of drives
//
// This would typically be done in FatFileDef.h

#include <FatFile/FatDrive.h>


#define FAT_DRIVES_LIST(...) FatDrive *const FatSys::m_arDrives[FAT_NUM_DRIVES] = {__VA_ARGS__};

static constexpr int FAT_NO_NAME_LEN = -1;

class FatSys
{
	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	static int HandleOfDrive(int drv)	{return -drv;}
	static bool IsError(int err)		{return FatDrive::IsError(err);}
	static bool IsErrorNotBusy(int err)	{return FatDrive::IsErrorNotBusy(err);}
	static bool IsFolder(uint handle)	{return HandleToPointer(handle)->IsFolder();}
	static FatDateTime GetFatDate(uint handle)	
		{return DriveToPointer(HandleToPointer(handle)->GetDrive())->m_state.DateTime;}
	static byte *GetDataBuf()			{ return FatDrive::s_pvDataBuf; }
	static bool IsDriveMounted(int drv)	{ return m_arDrives[drv]->IsMounted(); }
	static void SetStatusNotify(FatDrive::StatusChange *pfn) { FatDrive::SetStatusNotify(pfn); }
	static uint DriveFromHandle(uint h)	{ return HandleToPointer(h)->GetDrive(); }

	//static byte SetDate(byte handle, ulong dwTime) {return SetFatDate(handle, ToFatTime(dwTime));}
	/*
	static byte LockBuffer() {return LockBuffer();}
	static void WriteLockedBuffer(byte idBuf) {WriteLockedBuffer(idBuf);}
	static void WriteCurrentBuffer() {WriteCurrentBuffer();}
	static void UnlockBufferMakeCurrent(byte idBuf) {UnlockBufferMakeCurrent(idBuf);}
	static void UnlockBuffer(byte idBuf) {UnlockBuffer(idBuf);}
	*/

public:
	//*********************************************************************
	// Set the callback for operation complete

	void SetCompletionNotify(uint drive, OpCompletePv *pfn, void *pv = NULL)
	{
		DriveToPointer(drive)->SetCompletionNotify(pfn, pv);
	}

	void SetCompletionNotify(uint drive, OpCompleteInt *pfn, int arg = 0)
	{
		DriveToPointer(drive)->SetCompletionNotify((OpCompletePv *)pfn, (void *)arg);
	}

	void SetCompletionNotify(uint drive, OpCompleteUint *pfn, uint arg = 0)
	{
		DriveToPointer(drive)->SetCompletionNotify((OpCompletePv *)pfn, (void *)arg);
	}

	template<class T>
	void SetCompletionNotifyMethod(uint drive, void (T::*pM)(int), T *pObj)
	{
		union 
		{
			void (T::*pM)(int); 
			OpCompletePv *pfn;
		} u;
		u.pM = pM;
		DriveToPointer(drive)->SetCompletionNotify(u.pfn, pObj);
	}

	//****************************************************************************

	static void Process()
	{
		// Perform pending operation for each drive. If operation completes,
		// the completion notification will be called if set.
		for (uint drvCur = 0; drvCur < FAT_NUM_DRIVES; drvCur++)
			DriveToPointer(drvCur)->ProcessDrive();
	}

	//****************************************************************************

	static int GetDriveStatus(uint drive)
	{
		FatDrive	*pDrive;

		// Perform pending operation for each drive
		for (uint drvCur = 0; drvCur < FAT_NUM_DRIVES; drvCur++)
		{
			pDrive = DriveToPointer(drvCur);
			pDrive->PerformOp();
		}

		pDrive = DriveToPointer(drive);
		return pDrive->Status();
	}

	//****************************************************************************

	static int GetStatus(uint handle)
	{
		uint	drive;

		if (handle == 0)
			drive = 0;
		else
			drive = HandleToPointer(handle)->GetDrive();

		return GetDriveStatus(drive);
	}

	//****************************************************************************

	static int Init()
	{
		int err;

		FatBuffer::InvalidateAll();
		FatDrive::InvalidateHandles();

		for (int i = 0; i < FAT_NUM_DRIVES; i++)
		{
			err = m_arDrives[i]->Init(i);
			if (err != FATERR_None)
				return err;
		}
		return err;
	}

	//****************************************************************************

	static int StartMount(uint drive)
	{
		FatDrive	*pDrive;

		pDrive = DriveToPointer(drive);
		return pDrive->Mount();
	}

	//****************************************************************************

	static void Dismount(byte drive)
	{
		DriveToPointer(drive)->Dismount();
	}

	//****************************************************************************

	static int StartOpen(const char *pchName, int hFolder = 0, 
		uint flags = OPENFLAG_File | OPENFLAG_Folder, 
		int cchName = FAT_NO_NAME_LEN)
	{
		FatFile		*pf;
		FatDrive	*pDrive;
		uint		drive;
		int			hFile;
		int			err;

		hFile = GetHandle(hFolder);
		pf = HandleToPointer(hFile);

		if (IsError(hFile))
			return hFile;

		if (cchName == FAT_NO_NAME_LEN)
			cchName = strlen(pchName);

		// Check for drive specifier
		if (FAT_NUM_DRIVES > 1)
		{
			if (cchName >= 2 && *(pchName + 1) == ':')
			{
				drive = *pchName - 'A';
				if (drive > 'Z' - 'A')
					drive -= 'a' - 'A';
				if (drive >= FAT_NUM_DRIVES)
				{
					err = FATERR_InvalidDrive;
					goto CloseErr;
				}

				cchName -= 2;
				pchName += 2;
				pDrive = DriveToPointer(drive);
				pDrive->InitRootSearch(pf);	// ignore passed-in root
			}
			else
				pDrive = DriveToPointer(pf->GetDrive());
		}
		else
			pDrive = DriveToPointer(0);

		if (pDrive->m_state.op != FATOP_None)
		{
			err = FATERR_Busy;
			goto CloseErr;
		}

		if (!pDrive->IsMounted())
		{
			err = STERR_NotMounted;
			goto CloseErr;
		}

		pDrive->m_state.cchName = cchName;
		pDrive->m_state.pchName = (char *)pchName;
		pDrive->m_state.handle = hFile;
		pDrive->m_state.info.OpenFlags = flags;

		pDrive->m_state.cchFolderName = pDrive->ParseFolder();
		if (pDrive->m_state.cchFolderName == 0)
		{
			// If no name, can't create, delete, rename
			if (flags & OPENFLAG_DeleteBit || (flags & OPENFLAG_CreateBits) == OPENFLAG_CreateNew)
			{
				err = FATERR_InvalidFileName;
				goto CloseErr;
			}

			// If no file name, return dup of hFolder
			pDrive->m_state.status = FATERR_None;
			pDrive->m_state.op = FATOP_Status;
			return hFile;
		}

		pDrive->m_state.op = FATOP_Open;
		err = pDrive->StartOpen(pf);
		if (err != FATERR_Busy)
		{
			pDrive->m_state.op = FATOP_None;
	CloseErr:
			pf->Close();
			return err;
		}

		return hFile;
	}

	//****************************************************************************
	// Use Close() if file was not written to

	static int Close(uint handle) NO_INLINE_ATTR
	{
		FatFile	*pf;

		if (handle != 0)
		{
			if (handle > FAT_MAX_HANDLES)
				return FATERR_InvalidHandle;

			pf = HandleToPointer(handle);
			if (pf->IsDirty())
				return FATERR_DirtyNotClosed;

			pf->Close();
		}

		return FATERR_None;
	}

	//****************************************************************************
	// Use StartClose() if file could have been written to

	static int StartClose(uint handle) NO_INLINE_ATTR
	{
		int	err;

		if (handle != 0)
		{
			if (handle > FAT_MAX_HANDLES)
				return FATERR_InvalidHandle;

			err = StartFlush(handle);
			if (IsError(err))
				return err;

			HandleToPointer(handle)->Close();
		}

		return FATERR_None;
	}

	//****************************************************************************

	static int StartFlush(uint handle) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatDrive	*pDrive;

		if (handle != 0)
		{
			pf = HandleToPointer(handle);
			if (pf->IsDirty())
			{
				pDrive = DriveToPointer(pf->GetDrive());
				if (pDrive->m_state.op != FATOP_None)
					return FATERR_Busy;

				pDrive->m_state.handle = handle;
				pDrive->m_state.status = 0;
				pDrive->m_state.op = FATOP_Close;
				return pDrive->StartBuf(pf->m_DirSec);
			}

			FatBuffer::ClearPriority(handle);
		}

		return FATERR_None;
	}

	//****************************************************************************

	static int StartFlushAll(uint drive) NO_INLINE_ATTR
	{
		FatDrive	*pDrive;

		pDrive = DriveToPointer(drive);
		if (pDrive->m_state.op != FATOP_None)
			return FATERR_Busy;

		pDrive->m_state.status = 0;
		pDrive->m_state.op = FATOP_FlushAll;
		return FATERR_Busy;
	}

	//****************************************************************************

	static int EnumBegin(uint handle, uint flags = OPENFLAG_File | OPENFLAG_Folder) NO_INLINE_ATTR
	{
		int			h;
		FatFile		*pf;

		// Start new enumeration
		h = GetHandle(handle);
		if (IsError(h))
			return h;
		pf = HandleToPointer(h);
		pf->m_flags.fNextClus = 1;
		pf->m_flags.OpenType = OPENTYPE_Enum;
		pf->m_OpenFlags = flags;
		return h;
	}

	//****************************************************************************

	static int StartEnumNext(uint hParent, char *pchNameBuf, int cbBuf) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatFile		*pfParent;
		FatDrive	*pDrive;
		int			hFile;
		int			err;

		if (hParent == 0)
			return FATERR_InvalidHandle;

		pfParent = HandleToPointer(hParent);
		if (pfParent->m_flags.OpenType != OPENTYPE_Enum)
			return FATERR_InvalidHandle;

		if (cbBuf < FAT_MIN_NAME_BUF)
			return FATERR_InvalidArgument;

		pDrive = DriveToPointer(pfParent->GetDrive());
		if (pDrive->m_state.op != FATOP_None)
			return FATERR_Busy;

		// Get the handle to return if found
		hFile = GetHandle(hParent);
		if (IsError(hFile))
			return hFile;
		pf = HandleToPointer(hFile);

		// Initialize search location
		pfParent->CopySearchLoc(pf);

		pDrive->m_state.cchName = cbBuf - 1;// allow room for null terminator
		pDrive->m_state.pchName = pchNameBuf;
		pDrive->m_state.handle = hFile;
		pDrive->m_state.info.hParent = hParent;	// remember parent
		pDrive->m_state.info.OpenFlags = pfParent->m_OpenFlags;
		pDrive->m_state.op = FATOP_Enum;

		if (pfParent->m_flags.fNextClus)
		{
			// Initialize to read first entry
			pfParent->m_flags.fNextClus = 0;
			err = pDrive->ReadFirstDir(pf);
			if (err != FATERR_Busy)
			{
				pf->Close();
				return err;
			}
		}
		else
			pf->SearchNext();

		return hFile;
	}

	//****************************************************************************

	static int StartDelete(uint handle) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatDrive	*pDrive;
		int			err;

		pf = HandleToPointer(handle);
		if (pf->m_flags.OpenType != OPENTYPE_Delete)
			return FATERR_InvalidHandle;

		pDrive = DriveToPointer(pf->GetDrive());
		if (pDrive->m_state.op != FATOP_None)
			return FATERR_Busy;

		err = pDrive->StartBuf(pf->m_DirSec);
		if (err != FATERR_Busy)
			return err;
		pDrive->m_state.op = FATOP_Delete;
		return FATERR_None;
	}

	//****************************************************************************

	static int StartRename(const char *pchName, int hFolder, uint hSrc, int cchName = FAT_NO_NAME_LEN) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatDrive	*pDrive;
		int			err;

		pf = HandleToPointer(hSrc);
		if (pf->m_flags.OpenType != OPENTYPE_Delete)
			return FATERR_InvalidHandle;

		pDrive = DriveToPointer(pf->GetDrive());
		if (pDrive->m_state.op != FATOP_None)
			return FATERR_Busy;

		pDrive->m_state.info.hParent = hSrc;
		err = StartOpen(pchName, hFolder, OPENFLAG_CreateNew, cchName);
		if (IsError(err))
			return err;

		pDrive->m_state.op = FATOP_RenameOpen;
		return FATERR_None;
	}

	//****************************************************************************

	static int StartRead(uint handle, void *pv, uint cb)
	{
		return ReadWrite(handle, pv, cb, FATOP_Read);
	}

	//****************************************************************************

	static int StartWrite(uint handle, void *pv, uint cb)
	{
		return ReadWrite(handle, pv, cb, FATOP_Write);
	}

	//****************************************************************************

	static ulong StartSeek(uint handle, ulong ulPos, int origin = FAT_SEEK_SET) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatDrive	*pDrive;
		ulong		ulTmp;

		pf = HandleToPointer(handle);
		if (!pf->IsOpen() || pf->IsFolder())
			return -1;

		// Make sure we don't read past EOF
		if (origin != FAT_SEEK_SET)
		{
			if (origin == FAT_SEEK_END)
				ulTmp = pf->Length();
			else if (origin == FAT_SEEK_CUR)
				ulTmp = pf->CurPos();
			else
				return -1;

			// Check for seek before start of file
			if ((long)ulPos < 0 && -ulPos > ulTmp)
				ulPos = 0;
			else
				ulPos += ulTmp;
		}

		pDrive = DriveToPointer(pf->GetDrive());
		if (pDrive->m_state.op != FATOP_None)
			return -1;

		if (ulPos > pf->Length())
			return -1;

		pDrive->m_state.handle = handle;
		pDrive->m_state.op = FATOP_Seek;
		pDrive->m_state.dwSeekPos = ulPos;

		return ulPos;
	}

	//****************************************************************************

	static ulong GetPosition(uint handle) NO_INLINE_ATTR
	{
		FatFile		*pf;

		pf = HandleToPointer(handle);
		if (!pf->IsOpen() || pf->IsFolder())
			return -1;

		return pf->CurPos();
	}

	//****************************************************************************

	static ulong GetSize(uint handle) NO_INLINE_ATTR
	{
		FatFile		*pf;

		pf = HandleToPointer(handle);
		if (!pf->IsOpen() || pf->IsFolder())
			return -1;

		return pf->Length();
	}

	//****************************************************************************

	static int StartGetDate(uint handle) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatDrive	*pDrive;
		int			err;

		pf = HandleToPointer(handle);
		if (!pf->IsOpen())
			return FATERR_InvalidHandle;

		pDrive = DriveToPointer(pf->GetDrive());
		err = pDrive->StartBuf(pf->m_DirSec);
		if (err != FATERR_Busy)
			return err;
		pDrive->m_state.op = FATOP_GetDate;
		return FATERR_None;
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	static FatDrive *DriveToPointer(uint drive)	{ return m_arDrives[drive]; }
	static FatFile *HandleToPointer(int h)	{return FatDrive::HandleToPointer(h);}

	//****************************************************************************

	static int ReadWrite(uint handle, void *pv, uint cb, byte op) NO_INLINE_ATTR
	{
		FatFile		*pf;
		FatDrive	*pDrive;
		ulong		ulTmp;

		pf = HandleToPointer(handle);
		if (pf->m_flags.OpenType != OPENTYPE_Normal || pf->IsFolder())
			return FATERR_InvalidHandle;

		pDrive = DriveToPointer(pf->GetDrive());
		if (pDrive->m_state.op != FATOP_None)
			return FATERR_Busy;

		pDrive->m_state.pb = (byte *)pv;
		pDrive->m_state.handle = handle;
		pDrive->m_state.status = 0;

		if (op == FATOP_Read)
		{
			// Make sure we don't read past EOF
			if (pf->Length() <= pf->CurPos())
				cb = 0;
			else
			{
				ulTmp = pf->Length() - pf->CurPos();
				if (ulTmp < cb)
					cb = ulTmp;
			}
		}
		else
			pf->m_flags.fDirty = 1;

		if (cb > 0xFFFF)
			cb = 0xFFFF;
		pDrive->m_state.cb = cb;
		if (cb == 0)
			op = FATOP_Status;

		pDrive->m_state.op = op;
		return FATERR_None;
	}

	//****************************************************************************

	static int GetHandle(int hFolder) NO_INLINE_ATTR
	{
		FatFile		*pfFolder;
		FatFile		*pf;
		uint		hFile;

 		pf = FatDrive::GetHandleList();

		// Make sure we have an unused handle available
		for (hFile = 1; ;)
		{
			if (!pf->IsOpen())
				break;
			pf++;
			hFile++;

			if (hFile > FAT_MAX_HANDLES)
				return FATERR_NoHandles;
		}

		if (hFolder > 0)
		{
			pfFolder = HandleToPointer(hFolder);
			if (!pfFolder->IsOpen() || !pfFolder->IsFolder())
				return FATERR_InvalidHandle;

			HandleToPointer(hFolder)->DupFolder(pf);
		}
		else
		{
			// hFolder is negative of drive number to search from root
			if (hFolder <= -FAT_NUM_DRIVES)
				return FATERR_InvalidHandle;

			DriveToPointer(-hFolder)->InitRootSearch(pf);
		}

		return hFile;
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

protected:
	static FatDrive	*const m_arDrives[FAT_NUM_DRIVES];
};
