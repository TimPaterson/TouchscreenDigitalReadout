//****************************************************************************
// FatDrive.h
//
// Created 11/10/2020 10:22:38 AM by Tim
//
// Note that the following symbols must be defined before including this file:
//
// FAT_SECT_BUF_CNT	- number of sector buffers
// FAT_MAX_HANDLES	- max number of file handles
// FAT_NUM_DRIVES	- number of drives
//
// This would typically be done in FatFileDef.h
//
//****************************************************************************

#pragma once

#include <FatFile\FatFile.h>


// Invalid file name characters
#define	SHORT_NAME_BAD_CHARS	"\"*./:<>?\\|"
#define	LONG_NAME_BAD_CHARS		"+,;=[]"

// Values returned by CompDir()
#define	COMP_Match		0
#define	COMP_Empty		1
#define	COMP_NoMatch	2
#define COMP_StartLong	3

#define LONG_NAME_MATCH	0xFF	// Assigned to NamePos

// Values returned by CreateShortName()
#define SHORTNAME_IsChanged		0x01
#define SHORTNAME_HasUpper		0x02
#define SHORTNAME_HasLower		0x04
#define SHORTNAME_CaseBits		(CASE_BASE_LOWER | CASE_EXT_LOWER)	// 0x08 | 0x10
#define SHORTNAME_CaseChange	0x20
#define SHORTNAME_CantCreate	0x40

#define FAT_MIN_NAME_BUF	(CHAR_PER_SHORT_NAME + 2)	// allow for '.' and null terminator

FatDateTime GetFatTime();


//*************************************************************************
// Drive operation state

enum FatOp
{
	FATOP_None,
	FATOP_Status,
	FATOP_NewFolder,
	FATOP_Open,
	FATOP_RenameOpen,
	FATOP_Create,
	FATOP_RenameCreate,
	FATOP_Delete,
	FATOP_RenameDelete,
	FATOP_BlockRead,
	FATOP_Read,
	FATOP_Write,
	FATOP_Enum,
	FATOP_Seek,
	FATOP_MountDev,
	FATOP_Mount,
	FATOP_Close,
	FATOP_CloseAll,
	FATOP_Flush,
	FATOP_FlushAll,
	FATOP_FreeChain,
	FATOP_FreeChainFlush,
	FATOP_GetDate,
	FATOP_SetParentClus,
};

struct FatOpState
{
	// Data for the current operation
	// 32-bit aligned
	ulong		dwParentClus;	// Field needed for Rename/Move
	union
	{
		struct
		{
			char	*pchName;
			byte	cchName;
			byte	cchFolderName;
			byte	bSecCnt;
			byte	handle;
		};
		struct
		{
			byte	*pb;
			ushort	cb;
		};
		ulong		dwSeekPos;
		ulong		dwClusFree;
		FatDateTime	DateTime;
	};
	union
	{
		short	status;
		struct
		{
			byte	OpenFlags;
			byte	hParent;
			byte	NamePos;	// long filename position
			byte	Checksum;	// for long filename dir entry
		} info;
	};

	// 16-bit aligned
	struct
	{
		ushort	op:8;
		ushort	action:6;
		ushort	fStartRead:1;
	};

	// 8-bit aligned
	byte	cFree;
	byte	CreateChecksum;
};

//*************************************************************************
// Operation complete notification can have several flavors

typedef void OpCompletePv(void *pv, int status);
typedef void OpCompleteInt(int arg, int status);
typedef void OpCompleteUint(uint arg, int status);

//*************************************************************************

class FatDrive : protected Storage
{
	//*********************************************************************
	// Types
	//*********************************************************************

	static constexpr int DirEntPerSect = FAT_SECT_SIZE / sizeof(FatDirEnt);

	typedef void StatusChange(int drive, int status);

public:
	enum FatDriveState
	{
		FDS_Dismounted,
		FDS_Mounted,
	};

protected:
	enum FatAction
	{
		FATACT_None,
		FATACT_ReadFat,
		FATACT_AllocNewClus,
		FATACT_LinkNewClus,
		FATACT_ZeroDirClus,
		FATACT_CloseFolder,
	};

	// Data from BPB
	struct FatLocalBpb
	{
		ulong	FatStartSec;
		ulong	MaxClus;
		ulong	FatSize;
		ulong	DataStartSec;
		ulong	FreeClus;
		byte	SecPerClus;
		byte	SecPerClusShift;
		byte	NumFat;
		byte	RootSecCnt;
		byte	RootClus;
		union
		{
			struct
			{
				byte	fFat16:1;
				byte	fFat32:1;
				byte	fSearchEnd:1;
			};
			byte	bFlags;
		};

		ushort BytesPerClus() {return FAT_SECT_SIZE * SecPerClus;}
	};

	// This is use by the short name generator
	struct FatShortName
	{
		char	Base[8];
		char	Ext[3];
		byte	Attr;
		byte	CaseFlags;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	static byte *GetDataBuf()		{ return s_pvDataBuf; }
	static void InvalidateHandles()	{memset(s_arHandles, 0, sizeof s_arHandles);}
	static void SetStatusNotify(StatusChange *pfn)	{ s_pfnStatusChange = pfn; }

public:
	uint Init(uint drive)
	{
		m_drive = drive;
		return InitDev();
	}

	int Status()
	{
		switch (m_state.op)
		{
		case FATOP_None:
			return FATERR_None;

		case FATOP_Status:
			m_state.op = FATOP_None;
			return m_state.status;
		}

		return FATERR_Busy;
	}

	void ProcessDrive()
	{
		PerformOp();
		if (m_state.op == FATOP_Status && m_pfnOpComplete != NULL)
		{
			m_state.op = FATOP_None;
			m_pfnOpComplete(m_pvOpCompleteArg, m_state.status);
		}
	}

	uint GetDrive()		{ return m_drive; }

	//*********************************************************************
	// Set the callback for operation complete

	void SetCompletionNotify(OpCompletePv *pfn, void *pv)
	{
		m_pfnOpComplete = pfn;
		m_pvOpCompleteArg = pv;
	}

	void SetCompletionNotify(OpCompleteInt *pfn, int arg)
	{
		SetCompletionNotify((OpCompletePv *)pfn, (void *)arg);
	}

	void SetCompletionNotify(OpCompleteUint *pfn, uint arg)
	{
		SetCompletionNotify((OpCompletePv *)pfn, (void *)arg);
	}

	template<class T>
	void SetCompletionNotifyMethod(void (T::*pM)(int), T *pObj)
	{
		union 
		{
			void (T::*pM)(int); 
			OpCompletePv *pfn;
		} u;
		u.pM = pM;
		SetCompletionNotify(u.pfn, pObj);
	}

	//*********************************************************************

	int Mount()
	{
		int		err;

		Dismount();
		err = MountDev();
		if (IsError(err))
			return err;

		m_state.op = FATOP_MountDev;
		return FATERR_None;
	}

	void Dismount()		
	{
		DismountDev();
		m_BPB.bFlags = 0;
		if (s_pfnStatusChange != NULL)
			s_pfnStatusChange(m_drive, FDS_Dismounted);
		InvalidateAllBuffers();
	}

	bool IsMounted()	{return m_BPB.fFat16 || m_BPB.fFat32;}

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	bool IsFat32()			{return m_BPB.fFat32;}
	ulong Fat32DirStart()	{return m_BPB.RootClus;}

protected:
	static FatFile *GetHandleList()	{return &s_arHandles[0];}

	static FatFile *HandleToPointer(int handle)
		{ return &s_arHandles[handle - 1]; }

	//*********************************************************************
	// Main dispatcher

	void PerformOp()
	{
		int			err;
		FatCluster	clus;
		ulong		ulTmp;
		uint		uTmp;
		uint		uOff;
		uint		cb;
		FatFile		*pf;
		FatShortDirEnt	*pDir;
		FatDateTime	daytime;
		byte		*pbBuf;
		byte		*pb;
		FatBufDesc	*pDesc;

		if (m_state.op == FATOP_None || m_state.op == FATOP_Status)
			return;

		err = GetStatus();
		if (err == FATERR_Busy)
			return;

		if (IsError(err))
		{
			if (m_state.fStartRead)
				goto Invalidate;

			goto Finished;
		}

		if (m_state.fStartRead)
		{
			m_state.fStartRead = 0;
			err = ReadData(CurBufBlock(), CurBuf());
			if (IsError(err))
			{
Invalidate:
				m_state.fStartRead = 0;
				m_state.action = FATACT_None;
				CurBufDesc()->InvalidateBuf();
				goto Finished;
			}
			return;
		}

		pf = HandleToPointer(m_state.handle);

		switch (m_state.action)
		{
		case FATACT_ReadFat:
			// Data is ready
			err = ReadFat(pf);
			break;

		case FATACT_AllocNewClus:
			ulTmp = m_BPB.FreeClus;
			for (;;)
			{
				clus = ReadFatEntry(ulTmp);
				if (clus.IsError())
				{
					err = clus.ul;
					goto ActionErrChk;
				}
				if (clus.Cluster == 0)
				{
					// Found a free entry
					WriteFatEntry(ulTmp, FAT32_EOC);
					pf->m_flags.fNextClus = 0;
					pf->m_flags.fIsAll = 0;

					if (pf->m_FirstClus.ul == 0)
					{
						pf->m_flags.fDirty = 1;	// important for folders
						pf->m_FirstClus.ul = ulTmp;
						pf->m_CurClus.ul = ulTmp;

						if (pf->IsFolder())
							goto ClusLinked;

						err = FATERR_None;
					}
					else
					{
						err = StartFatRead(pf->m_CurClus.Cluster);
						m_state.action = FATACT_LinkNewClus;
					}
					goto ActionErrChk;
				}
				ulTmp++;
				if (ulTmp == m_BPB.MaxClus)
				{
					if (m_BPB.fSearchEnd)
					{
						err = FATERR_DiskFull;
						goto Finished;
					}
					m_BPB.fSearchEnd = 1;
					m_BPB.FreeClus = 2;
					break;
				}
				m_BPB.FreeClus = ulTmp;
				if (clus.fLastSect)
					break;
			}
			err = StartAllocClus();
			break;

		case FATACT_LinkNewClus:
			err = WriteFatEntry(pf->m_CurClus.Cluster, m_BPB.FreeClus);
			if (pf->IsFolder())
			{
	ClusLinked:
				// Need to zero out the new cluster
				m_state.action = FATACT_ZeroDirClus;
				// Sectors will be zeroed in reverse order.
				m_state.bSecCnt = m_BPB.SecPerClus - 1;
				pf->m_Length = SectFromClus(m_BPB.FreeClus) + m_state.bSecCnt;
				err = GetFreeBuf();
			}
			else
				pf->m_CurClus.ul = m_BPB.FreeClus;
			break;

		case FATACT_ZeroDirClus:
			// Zero out the cluster allocated to a folder, starting
			// with the last sector and moving forward. Once we get
			// to the first sector, we need to add the . and .. entries.
			ulTmp = pf->m_Length;
			uTmp = m_state.bSecCnt;
			if (uTmp == (byte)(m_BPB.SecPerClus - 1))
				memset(CurBuf(), 0, FAT_SECT_SIZE); // First time through

			pDesc = CurBufDesc();
			pDesc->SetBlock(ulTmp, m_drive);
			pDesc->SetFlagsDirty();
			if (uTmp == 0)
			{
				pf->m_Length = 0;	// folders have length zero
				m_state.action = FATACT_None;	// might be done

				// Is this first cluster allocated to directory?
				// Folder can only be dirty if first cluster was set.
				if (pf->m_flags.fDirty)
				{
					FatShortDirEnt	*pDir;
					FatDateTime		daytime;

					// Create '.' and '..' entries
					pDir = (FatShortDirEnt *)CurBuf();
					daytime = GetFatTime();
					MakeDotName(pDir, pf->m_FirstClus.ul, daytime);
					pDir++;
					MakeDotName(pDir, m_state.dwParentClus, daytime);
					pDir->Name[1] = '.';

					// Did we just create an intermediate folder?
					if (m_state.op != FATOP_NewFolder)
						m_state.action = FATACT_CloseFolder;

					// Flush the new folder
					err = StartBuf(pf->m_DirSec);
				}
				else
				{
					// Adding a cluster to an existing directory. If their
					// were any free entries in the previous cluster, reload
					// the directory sector with those free entries.
					if (m_state.cFree != 0)
						err = StartBuf(pf->m_DirSec);
				}
			}
			else
			{
				err = WriteBuf(CurBufIndex());
				pf->m_Length = ulTmp - 1;
				m_state.bSecCnt = uTmp - 1;
			}
			break;

		case FATACT_CloseFolder:
			// Come here when we created an intermediate folder. Write out info
			// for this folder, move on to next name.
			SetFirstClus(pf);	// Sets buffer dirty, file clean
			m_state.action = FATACT_None;
			err = StartOpen(pf);
			break;
		}

	ActionErrChk:
		if (err == FATERR_Busy)
			return;

		m_state.action = FATACT_None;

		if (IsError(err))
			goto Finished;

		switch (m_state.op)
		{
		case FATOP_None:
			return;

		case FATOP_NewFolder:
			SetFirstClus(pf);	// Sets buffer dirty, file clean
			break;

		case FATOP_Open:
		case FATOP_RenameOpen:
			// Data is ready
			err = SearchDir(pf);
			if (err == FATERR_Busy)
				return;

			if (err == FATERR_FileNotFound)
			{
				uTmp = m_state.info.OpenFlags & OPENFLAG_CreateBits;
				if (uTmp == OPENFLAG_OpenExisting || uTmp & OPENFLAG_DeleteBit)
					break;

				err = StartCreate(pf);
			}
			break;

		case FATOP_Create:
		case FATOP_RenameCreate:
			err = Create(pf);
			break;

		case FATOP_BlockRead:
		case FATOP_Read:
		case FATOP_Write:
			if (pf->m_flags.fNextClus)
				goto NextClus;

			// Convert current position to sector within cluster
			uTmp = (pf->m_CurPos / FAT_SECT_SIZE) & (m_BPB.SecPerClus - 1);

			if (m_state.op == FATOP_BlockRead)
			{
				cb = FAT_SECT_SIZE;
				m_state.op = FATOP_Read;
				goto UpdatePos;
			}

			// Compute sector from cluster
	GetClus:
			ulTmp = SectFromClus(pf->m_CurClus.Cluster) + uTmp;

			// Offset in sector
			uOff = pf->m_CurPos & (FAT_SECT_SIZE - 1);

			// See if we're already in this sector
			if (FindBuffer(ulTmp))
			{
				cb = FAT_SECT_SIZE - uOff;	// Bytes left in sector
				if (cb > m_state.cb)
					cb = m_state.cb;
				pbBuf = CurBuf() + uOff;
				pb = m_state.pb;
				if (pb == NULL)
				{
					s_pvDataBuf = pbBuf;
					m_state.cb = 0;
					if (m_state.op == FATOP_Write)
						CurBufDesc()->SetFlagsDirty();
					goto UpdateNoPtr;
				}

				if (m_state.op == FATOP_Read)
					memcpy(pb, pbBuf, cb);
				else
				{
					CurBufDesc()->SetFlagsDirty();
					memcpy(pbBuf, pb, cb);
				}
	UpdatePos:
				m_state.pb += cb;
				m_state.cb -= cb;
	UpdateNoPtr:
				m_state.status += cb;
				pf->m_CurPos += cb;
				uOff = pf->m_CurPos & (FAT_SECT_SIZE - 1);
				if (m_state.op == FATOP_Write && pf->m_CurPos > pf->m_Length)
					pf->m_Length = pf->m_CurPos;

				pDesc = CurBufDesc();
				if (uOff == 0)
				{
					// The buffer management scheme gives priority to partially
					// read/written buffers. Once we reach the end of a buffer,
					// it's no longer higher priority.
					pDesc->ClearPriority();

					uTmp++;		// Next sector in cluster
					if (uTmp >= m_BPB.SecPerClus)
					{
						// Moving on to next cluster
						uTmp = pf->m_CurClus.Cnt;
						if (uTmp == 0)
						{
							// No more clusters in this string
							if (m_state.cb == 0)
								pf->m_flags.fNextClus = 1;
							else
							{
	NextClus:
								if (pf->m_flags.fIsAll)
								{
									// We're at end of cluster chain.
									if (m_state.op == FATOP_Read)
										goto ReadFinished;

									// Allocate another cluster
									err = StartAllocClus();
									break;
								}
								err = StartFatRead(pf->m_CurClus.ul);
								break;
							}
						}
						else
						{
							// Next cluster in this string
							pf->m_CurClus.Cnt = uTmp - 1;
							pf->m_CurClus.Cluster++;
						}
						uTmp = 0;	// sector 0 in cluster
					}
				}
				else
				{
					// The buffer management scheme gives priority to partially
					// read/written buffers. Since we're in the middle of this
					// buffer, it qualifies for higher priority.
					pDesc->SetPriority(m_state.handle);
				}

				if (m_state.cb == 0)
					goto ReadFinished;	// All done

				goto GetClus;
			}

			// Not buffered - check for full sector
			if (uOff == 0)
			{
				if (m_state.cb >= FAT_SECT_SIZE)
				{
					if (m_state.pb != NULL)
					{
						if (m_state.op == FATOP_Write)
						{
							err = GetStatus();	// in case of 2 full sector writes
							if (IsError(err))
								break;
							err = WriteData(ulTmp, m_state.pb);
							if (IsError(err))
								break;
							cb = FAT_SECT_SIZE;
							goto UpdatePos;
						}
						m_state.op = FATOP_BlockRead;
						err = ReadData(ulTmp, m_state.pb);
						if (IsError(err))
							break;	// finished, with error
						return;
					}
					else
						goto WriteSectorCheck;
				}
				else if (pf->m_CurPos >= pf->m_Length)
				{
	WriteSectorCheck:
					if (m_state.op == FATOP_Write)
					{
						// Writing past end (or whole sector), don't bother to read in sector
						err = GetFreeBuf();
						if (err != FATERR_Busy)
							break;
						CurBufDesc()->SetBlock(ulTmp, m_drive);	// pretend its already in buffer
						break;
					}
				}
			}

			// Partial sector, need to buffer it
			err = StartBuf(ulTmp);
			break;

		case FATOP_Enum:
			err = Enum(pf);
			if (IsError(err))
			{
				if (err != FATERR_Busy)
					pf->Close();
				if (err != FATERR_FileNotFound)
					break;
				m_state.status = 0;	// signal end with zero-length name
			}
			else
				m_state.status = m_state.cchFolderName;	// length of name
			goto ReadFinished;

		case FATOP_Seek:
			err = Seek(pf);
			break;

		case FATOP_FlushAll:
			// We start at the beginning each time, stopping at the first
			// file that's dirty.
			pf = GetHandleList();
			for (uTmp = 0; uTmp < FAT_MAX_HANDLES; uTmp++)
			{
				if (pf->IsDirty() && pf->m_Drive == m_drive)
				{
					// Close this file, then repeat the search
					m_state.handle = uTmp + 1;
					m_state.op = FATOP_CloseAll;
					err = StartBuf(pf->m_DirSec);
					goto StillDirty;
				}
				pf++;
			}
			// None were dirty, so flush the buffers
			goto StartFlush;

		case FATOP_Close:
		case FATOP_CloseAll:
			// We only enter here if FatFile is dirty
			// Compute position within directory sector
			pDir = SetFirstClus(pf);	// Sets buffer dirty, file clean
			daytime = GetFatTime();
			pDir->WriteTime = daytime.time;
			pDir->WriteDate = daytime.date;
			pDir->FileSize = pf->m_Length;
			if (m_state.op == FATOP_CloseAll)
			{
				// Don't bother flushing buffers yet, continue closing files
				m_state.op = FATOP_FlushAll;
				break;
			}
	StartFlushClose:
			pf->Close();
	StartFlush:
			m_state.op = FATOP_Flush;
			//
			// Fall into Flush
			//
		case FATOP_Flush:
			// We start at the beginning each time, stopping at the first
			// buffer that's dirty.
			for (uTmp = 0; uTmp < FAT_SECT_BUF_CNT; uTmp++)
			{
				pDesc= BufDescFromIndex(uTmp);
				if (pDesc->IsDirty() && pDesc->GetDrive() == m_drive)
				{
					// Flush this buffer, then repeat the search
					err = WriteBuf(uTmp);
					goto StillDirty;
				}
			}
	StillDirty:
			break;

		case FATOP_SetParentClus:
			// We have moved/renamed a folder. We need to make sure its
			// parent (..) entry is correct.
			pDir = (FatShortDirEnt *)CurBuf();
			pDir++;	// second entry should be ".."
			if (pDir->Name[0] == '.' && pDir->Name[1] == '.')
			{
				clus.ul = m_state.dwParentClus;	// Parent folder
				if (pDir->FirstClusLo != clus.uLo ||
					pDir->FirstClusHi != clus.uHi)
				{
					pDir->FirstClusLo = clus.uLo;
					pDir->FirstClusHi = clus.uHi;
					CurBufDesc()->SetFlagsDirty();
				}
				goto StartFlushClose;
			}
			err = FATERR_FormatError;
			break;

		case FATOP_Delete:
		case FATOP_RenameDelete:
			// Compute position within directory sector
			pDir = &((FatShortDirEnt *)CurBuf())[pf->m_DirEnt];
			CurBufDesc()->SetFlagsDirty();
 			// SetMediaDirty() not called because we will flush all changes

			for(;;)
			{
				pDir->Name[0] = (char)EMPTY_DIR_ENTRY;
				if ((pDir->Attr & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME)
				{
					// In long name, advance to next entry
					pDir++;
					if (!AtEndBuf(pDir))
						continue;

					err = ReadNextDir(pf);
					break;
				}
				else
				{
					// Finished deleting name entry
					if (m_state.op == FATOP_RenameDelete)
					{
						if (!pf->IsFolder())
							goto StartFlushClose;

						// Renamed/moved a folder. Reset the .. pointer.
						m_state.op = FATOP_SetParentClus;
						err = StartBuf(SectFromClus(pf->m_FirstClus.ul));
						break;
					}

					// Free cluster chain
					clus.ul = pf->m_FirstClus.Cluster;
					if (clus.ul == 0)
						goto StartFlushClose;
					m_state.op = FATOP_FreeChainFlush;
					goto FreeChainReadFat;
				}
			}
			break;

		case FATOP_FreeChain:
		case FATOP_FreeChainFlush:
			// FAT sector in buffer
			ulTmp = m_state.dwClusFree;
			clus = ReadFatEntry(ulTmp);
			if (clus.IsError())
			{
				err = clus.ul;
				break;
			}
			clus.ul = clus.Cluster;
			if (clus.ul == 0)
				goto EndChain;	// Bad chain, ignore

			WriteFatEntry(ulTmp, 0);	// Mark as free
			if (clus.ul >= FAT32_EOC_MIN)
			{
	EndChain:
				if (m_state.op == FATOP_FreeChainFlush)
					goto StartFlushClose;
				break;
			}
	FreeChainReadFat:
			m_state.dwClusFree = clus.ul;
			err = StartBuf(FatSectFromClus(clus.ul));
			break;

		case FATOP_MountDev:
			m_state.op = FATOP_Mount;
			err = StartBuf(0);	// Read boot sector to start mount
			break;

		case FATOP_Mount:
			err = MountDrive();
			if (err == FATERR_None && s_pfnStatusChange != NULL)
				s_pfnStatusChange(m_drive, FDS_Mounted);
			break;

		case FATOP_GetDate:
			// Compute position within directory sector
			pDir = &((FatShortDirEnt *)CurBuf())[pf->m_DirEnt];
			m_state.DateTime.time = pDir->WriteTime;
			m_state.DateTime.date = pDir->WriteDate;
			break;

		default:
			return;
		}

		if (err == FATERR_Busy)
			return;

	Finished:
		// Finished, good or bad
		m_state.status = err;
	ReadFinished:
		m_state.op = FATOP_Status;
		return;
	}

	//*********************************************************************
	// Mount drive

	int MountDrive()
	{
		int		iSect;
		int		iClus;
		byte	*pb;
		FatBootSect		*pBoot;
		FatBootSecTail	*pTail;
		FatPartitionEnt	*pPartition;

		pb = CurBuf();
		if (*(ushort *)(pb + FAT_SIG_OFFSET) != FAT_SIGNATURE)
			return FATERR_CantMount;

		pBoot = (FatBootSect *)pb;

		if (pBoot->Bpb.BytsPerSec != FAT_SECT_SIZE)
			goto BadBpb;

		m_BPB.SecPerClus = pBoot->Bpb.SecPerClus;
		m_BPB.SecPerClusShift = __builtin_ctzl(m_BPB.SecPerClus);

		// Compute no. of root directory sectors
		iSect = pBoot->Bpb.RootEntCnt / DirEntPerSect;
		if (iSect >= 0x100)
			goto BadBpb;
		m_BPB.RootSecCnt = iSect;
		m_BPB.NumFat = pBoot->Bpb.NumFATs;

		m_BPB.FatSize = pBoot->Bpb.FATSz16 == 0 ? pBoot->Bpb32.FATSz32 : pBoot->Bpb.FATSz16;
		iSect += m_BPB.FatSize * m_BPB.NumFat;

		m_BPB.FatStartSec = pBoot->Bpb.RsvdSecCnt + CurBufBlock();
		m_BPB.DataStartSec = m_BPB.FatStartSec + iSect;

		// Compute maximum cluster number
		iSect += pBoot->Bpb.RsvdSecCnt;
		iSect = (pBoot->Bpb.TotSec16 != 0 ? pBoot->Bpb.TotSec16 : pBoot->Bpb.TotSec32) - iSect;
		iClus = iSect >> m_BPB.SecPerClusShift;	// Number of clusters

		if (iClus < FAT16_MAX_CLUS)
		{
			if (iClus < FAT12_MAX_CLUS)
				goto BadBpb;

			m_BPB.fFat16 = 1;
			pTail = &pBoot->Tail16;
			m_BPB.RootClus = 0;
		}
		else
		{
			m_BPB.fFat32 = 1;
			if (pBoot->Bpb32.RootClus >= 0x100 || pBoot->Bpb32.ExtFlags != 0)
				goto BadBpb;

			pTail = &pBoot->Tail32;
			m_BPB.RootClus = (byte)pBoot->Bpb32.RootClus;
		}

		m_BPB.MaxClus = iClus + 1;
		m_BPB.FreeClus = 2;	// UNDONE: for FAT32 read FSInfo

		if (pTail->FilSysType[0] != 'F' ||
			pTail->FilSysType[1] != 'A' ||
			pTail->FilSysType[2] != 'T')
		{
	BadBpb:
			m_BPB.bFlags = 0;

			// If we're not reading sector zero, this is our second try
			if (CurBufBlock() != 0)
				return FATERR_CantMount;

			// Maybe it was a partition table
			pPartition = (FatPartitionEnt *)(CurBuf() + FAT_PARTITION_START);
			for (int i = 4; i > 0; i--)
			{
				if (pPartition->FileSystem != 0)
					return StartBuf(pPartition->StartSector);
				pPartition++;
			}
			return FATERR_CantMount;
		}

		return FATERR_None;
	}

	//*********************************************************************
	// Buffer management

	static byte *BufFromIndex(byte iBuf)	{return FatBuffer::BufFromIndex(iBuf);}
	static byte CurBufIndex()				{return FatBuffer::CurBufIndex();}
	static byte *CurBuf()					{return FatBuffer::CurBuf();}
	static byte *EndCurBuf()				{return FatBuffer::EndCurBuf();}
	static bool AtEndBuf(void *pv)			{return FatBuffer::AtEndBuf(pv);}
	static FatBufDesc *CurBufDesc()			{return FatBuffer::CurBufDesc(); }
	static ulong CurBufBlock()				{ return CurBufDesc()->GetBlock(); }
	static FatBufDesc *BufDescFromIndex(byte iBuf)	{return FatBuffer::BufDescFromIndex(iBuf);}

	void InvalidateAllBuffers()
	{
		FatBuffer::InvalidateAll(m_drive);
	}

	byte *FindBuffer(ulong ulSect)
	{
		return FatBuffer::FindBuffer(ulSect, m_drive);
	}

	int GetFreeBuf()
	{
		return WriteBuf(FatBuffer::GetFreeBuf(m_drive));
	}

	//****************************************************************************

	int StartBuf(ulong ulSect) NO_INLINE_ATTR
	{
		int		err;

		if (FindBuffer(ulSect))
			return FATERR_Busy;

		err = GetFreeBuf();
		if (err != FATERR_Busy)
			return err;
		CurBufDesc()->SetBlock(ulSect, m_drive);
		m_state.fStartRead = 1;
		return FATERR_Busy;
	}

	//*********************************************************************

	int WriteBuf(uint buf) NO_INLINE_ATTR
	{
		int			err;
		FatBufDesc *pDesc;

		pDesc = BufDescFromIndex(buf);
		if (pDesc->IsDirty())
		{
			err = WriteData(pDesc->GetBlock(), BufFromIndex(buf));
			if (IsError(err))
				return err;
		}
		pDesc->SetFlagsDirty(false);
		pDesc->ClearPriority();
		return FATERR_Busy;
	}

	//*********************************************************************
	// File I/O

	typedef ALIGNED_ATTR(byte) ushort unaligned_ushort;

	static int FillLongName(unaligned_ushort *pwName, uint cch1, char *pchLongName, int cch)
	{
		ushort		wch;

		for (; cch1 != 0; cch1--, cch--)
		{
			if (cch > 0)
				wch = *pchLongName++;
			else if (cch == 0)
				wch = 0;
			else
				wch = 0xFFFF;

			// Byte at a time as some fields are not aligned
			*pwName++ = wch;
		}

		return cch;
	}

	//****************************************************************************

	int StartOpen(FatFile *pf, int bTail = 1) NO_INLINE_ATTR
	{
		int				err;
		FatShortName	ShortName;

		err = CreateShortName(&ShortName, bTail);
		if (IsError(err))
			return err;
		if (err == 0)
			bTail = 0;
		pf->m_ShortNameNumTail = bTail;	// Remember tail number if long name

		// If this is our 2nd time here and the short name matches the long name,
		// don't search for another available short name.
		if (bTail >= 2 && !(err & SHORTNAME_IsChanged))
			return FATERR_None;

		m_state.CreateChecksum = ShortNameChecksum(ShortName.Base);
		return ReadFirstDir(pf);
	}

	//****************************************************************************

	int StartAllocClus() NO_INLINE_ATTR
	{
		int	err;

		err = StartFatRead(m_BPB.FreeClus);
		if (err == FATERR_Busy)
			m_state.action = FATACT_AllocNewClus;
		return err;
	}

	//****************************************************************************

	int StartCreate(FatFile *pf) NO_INLINE_ATTR
	{
		int		cLongDir;
		int		cFree;
		int		cDirPerSec;
		int		bTmp;

		cLongDir = pf->m_ShortNameNumTail;	// flag indicating long name needed
		if (cLongDir)
		{
			if (m_state.info.OpenFlags & OPENFLAG_ShortConflict)
			{
				if (cLongDir == 0xFF)
					return FATERR_InvalidFileName;

				m_state.info.OpenFlags &= ~OPENFLAG_ShortConflict;
				bTmp = StartOpen(pf, cLongDir + 1);
				if (IsError(bTmp))
					return bTmp;
			}

			cLongDir = (ushort)(m_state.cchFolderName + CHAR_PER_LONG_NAME - 1) / CHAR_PER_LONG_NAME;
			m_state.info.NamePos = cLongDir | LAST_LONG_ENTRY;
		}

		// Need to create the file
		m_state.op += FATOP_Create - FATOP_Open;
		cFree = m_state.cFree;
		if (cFree == 0)
			goto GrowDirectory;

		if (cFree == (byte)-1)
		{
			cFree = 1;
			pf->m_DirSec = CurBufBlock();
		}
		else if (cFree > 0)
		{
			// Have location to start filename
			pf->RestoreDirPos();
		}

		if (cFree <= cLongDir)
		{
			// See if there's more room
			if (pf->IsRoot())
				bTmp = m_BPB.RootSecCnt - pf->m_DirSecInClus;
			else
				bTmp = m_BPB.SecPerClus - pf->m_DirSecInClus;

			if (bTmp < 3)
			{
				cDirPerSec = FAT_SECT_SIZE / sizeof(FatDirEnt);
				cFree += cDirPerSec - pf->m_DirEnt - 1;
				if (bTmp == 2)
					cFree += cDirPerSec;

				if (cFree <= cLongDir)
				{
	GrowDirectory:
					// Directory was full, try to allocate more
					if (pf->IsRoot())
						return FATERR_RootDirFull;
					return StartAllocClus();
				}
			}
		}
		return StartBuf(pf->m_DirSec);
	}

	//****************************************************************************

	int Create(FatFile *pf) NO_INLINE_ATTR
	{
		int			bTmp;
		int			bDirPos;
		int			bPos;
		int			bDirEnt;
		int			cch;
		uint		wTmp;
		ulong		dwTmp;
		FatDirEnt	*pDir;
		char		*pchLongName;
		FatDateTime	daytime;

		// Compute position within directory sector
		bDirEnt = pf->m_DirEnt;
		wTmp = bDirEnt * sizeof(FatDirEnt);
		if (wTmp == FAT_SECT_SIZE)
			return ReadNextDir(pf);

		pDir = (FatDirEnt *)(CurBuf() + wTmp);
		bDirPos = m_state.info.NamePos;

		for (;;)
		{
			bTmp = pDir->Short.Name[0];
			if (bTmp != EMPTY_DIR_ENTRY && bTmp != END_DIR_ENTRY)
				return FATERR_InternalError;	// should have been free

			CurBufDesc()->SetFlagsDirty();
			if (bDirPos == 0)
				break;

			// Long name entry
			pDir->Long.Order = bDirPos;
			pDir->Long.Attr = ATTR_LONG_NAME;
			pDir->Long.Type = 0;
			pDir->Long.FirstClusLo = 0;
			pDir->Long.Checksum = m_state.CreateChecksum;

			bDirPos = (bDirPos & ~LAST_LONG_ENTRY) - 1;
			bPos = bDirPos * CHAR_PER_LONG_NAME;
			pchLongName = m_state.pchName + bPos;
			cch = m_state.cchFolderName - bPos;

			cch = FillLongName(pDir->Long.Name1, _countof(pDir->Long.Name1), pchLongName, cch);
			pchLongName += _countof(pDir->Long.Name1);

			cch = FillLongName(pDir->Long.Name2, _countof(pDir->Long.Name2), pchLongName, cch);
			pchLongName += _countof(pDir->Long.Name2);

			cch = FillLongName(pDir->Long.Name3, _countof(pDir->Long.Name3), pchLongName, cch);
			pchLongName += _countof(pDir->Long.Name3);

			bDirEnt++;
			pDir++;
			if (AtEndBuf(pDir))
			{
				// Need next dir sector
				m_state.info.NamePos = bDirPos;
				pf->m_DirEnt = bDirEnt;
				return ReadNextDir(pf);
			}
		}

		// Final entry
		bTmp = CreateShortName((FatShortName *)pDir, pf->m_ShortNameNumTail);
		if (bTmp & SHORTNAME_CantCreate)
			return FATERR_InvalidFileName;
		pDir->Short.Attr = ATTR_ARCHIVE;
		memset(&pDir->Short.CrtTimTenth, 0, sizeof *pDir - offsetof(FatShortDirEnt, CrtTimTenth));
		daytime = GetFatTime();
		pDir->Short.WriteTime = daytime.time;
		pDir->Short.WriteDate = daytime.date;

		// Set up open file
		dwTmp = pf->m_FirstClus.ul;	// Save parent dir cluster
		if (pf->IsRoot() || dwTmp == Fat32DirStart())
			dwTmp = 0;
		m_state.dwParentClus = dwTmp;	// Save parent dir cluster
		memset(pf, 0, sizeof *pf);

		// Set up non-zero fields of open file
		pf->SetDrive(m_drive);
		pf->m_DirEnt = bDirEnt;
		pf->m_DirSec = CurBufBlock();
		pf->m_flags.OpenType = OPENTYPE_Normal;
		pf->m_flags.fNextClus = 1;
		pf->m_flags.fIsAll = 1;

		// See if this if there is more in the pathname
		m_state.pchName += m_state.cchFolderName;
		m_state.cchFolderName = ParseFolder();

		if (m_state.cchFolderName != 0)
		{
			// Need to create next file/folder in pathname
			m_state.op += FATOP_Open - FATOP_Create;
			goto SetFolder;
		}

		if (m_state.op == FATOP_RenameCreate)
		{
			byte		hParent;
			FatFile		*pfParent;

			pf->Close();
			hParent = m_state.info.hParent;
			pfParent = HandleToPointer(hParent);
			pDir->Short.FirstClusHi = pfParent->m_FirstClus.uHi;
			pDir->Short.FirstClusLo = pfParent->m_FirstClus.uLo;
			pDir->Short.FileSize = pfParent->m_Length;
			pDir->Short.WriteDate = pfParent->m_DateTime.date;
			pDir->Short.WriteTime = pfParent->m_DateTime.time;

			// Restore m_DirSecInClus for ReadNextDir
			bTmp = (byte)pfParent->m_DirSec;
			bTmp -= (byte)m_BPB.DataStartSec;
			if (pfParent->IsRoot())
				bTmp += m_BPB.RootSecCnt;
			else
				bTmp &= m_BPB.SecPerClus - 1;
			pfParent->m_DirSecInClus = bTmp;

			if (pfParent->m_flags.fIsFolder)
				pDir->Short.Attr = ATTR_DIRECTORY | ATTR_ARCHIVE;
			CurBufDesc()->SetFlagsDirty();

			// Set up m_state to delete original file name
			m_state.handle = hParent;
			m_state.op = FATOP_RenameDelete;
			return StartBuf(pfParent->m_DirSec);
		}

		if (m_state.info.OpenFlags & OPENFLAG_Folder)
		{
			m_state.op = FATOP_NewFolder;
	SetFolder:
			pf->m_flags.fIsFolder = 1;
			pDir->Short.Attr = ATTR_DIRECTORY | ATTR_ARCHIVE;
			// Allocate first cluster to the new directory
			return StartAllocClus();
		}

		return FATERR_None;
	}

	//****************************************************************************

	int Seek(FatFile *pf) NO_INLINE_ATTR
	{
		uint		bCnt;
		ulong		ulClusNew;
		ulong		ulClusCur;
		ulong		ulTmp;

		// Find out new location relative current cluster
		ulClusNew = m_state.dwSeekPos;
		ulClusCur = pf->CurPos();
		if (pf->m_flags.fNextClus)
		{
			ulClusCur--;	// Position was in next cluster, pull it back
			pf->m_flags.fNextClus = 0;
		}

		ulClusNew /= FAT_SECT_SIZE;
		ulClusCur /= FAT_SECT_SIZE;

		ulClusNew >>= m_BPB.SecPerClusShift;
		ulClusCur >>= m_BPB.SecPerClusShift;

		// If seeking backward, start at the beginning
		if (ulClusNew < ulClusCur)
		{
			ulClusCur = 0;
			pf->m_CurClus = pf->m_FirstClus;
			pf->m_CurPos = 0;
			pf->m_flags.fIsAll = 0;
		}

		if (ulClusCur == ulClusNew)
		{
			pf->m_CurPos = m_state.dwSeekPos;
			return FATERR_None;
		}

		// See if we have more consecutive clusters
		bCnt = pf->m_CurClus.Cnt;
		ulTmp = ulClusNew - ulClusCur;
		if (bCnt >= ulTmp)
		{
			// Have it cached
			bCnt -= ulTmp;
			pf->m_CurClus.Cnt = bCnt;
			pf->m_CurClus.Cluster += ulTmp;
			pf->m_CurPos = m_state.dwSeekPos;
			return FATERR_None;
		}

		// Bump position to start of next cluster
		ulTmp = m_BPB.BytesPerClus() * (bCnt + 1);
		ulTmp += pf->m_CurPos;			// Bump position
		ulTmp &= ~(FAT_SECT_SIZE - 1);	// back up to start of cluster
		pf->m_CurPos = ulTmp;
		pf->m_CurClus.ul = pf->m_CurClus.Cluster + bCnt;	// Set Cnt to zero too
		pf->m_flags.fNextClus = 1;
		if (pf->IsAll())
			return FATERR_None;
		return StartFatRead(pf->m_CurClus.ul);
	}

	//****************************************************************************

	int Enum(FatFile *pf) NO_INLINE_ATTR
	{
		uint		oDir;

		// Compute position within directory sector
		oDir = pf->m_DirEnt * sizeof(FatDirEnt);
		if (oDir == FAT_SECT_SIZE)
			return ReadNextDir(pf);

		return SearchDir(pf, oDir, true);
	}

	//*********************************************************************
	// FAT management

	int StartFatRead(ulong dwCluster) NO_INLINE_ATTR
	{
		int	err;

		err = StartBuf(FatSectFromClus(dwCluster));
		if (err == FATERR_Busy)
			m_state.action = FATACT_ReadFat;
		return err;
	}

	//****************************************************************************

	int ReadFat(FatFile *pf) NO_INLINE_ATTR
	{
		int			err;
		int			cnt;
		ulong		dwTmp;
		ulong		dwLast;
		FatCluster	clus;

		cnt = 0;
		dwLast = pf->m_CurClus.Cluster;

		// Read ahead in FAT sector to accumulate consecutive clusters
		do
		{
			clus = ReadFatEntry(dwLast);
			if (clus.IsError())
			{
				err = clus.ul;
				goto Exit;
			}
			dwTmp = clus.Cluster;

			// Check for case of error in FAT
			if (dwTmp == 0)
				return FATERR_FormatError;

			if (dwTmp >= FAT32_EOC_MIN)
			{
				pf->m_flags.fIsAll = 1;
				break;
			}

			if (cnt == 0)
			{
				pf->m_CurClus.ul = dwTmp;	// zero count too
				pf->m_flags.fNextClus = 0;
				dwLast = dwTmp;
			}
			else
			{
				dwLast++;
				if (dwTmp != dwLast)
					break;
			}

		} while (++cnt <= CLUS_COUNT_MAX && !clus.fLastSect && 
			FatSectFromClus(dwLast) == CurBufBlock());

		// We finished counting consecutive clusters
		if (cnt > 0)
			cnt--;
		err = FATERR_None;

	Exit:
		pf->m_CurClus.Cnt = cnt;
		return err;
	}

	//****************************************************************************

	FatShortDirEnt *SetFirstClus(FatFile *pf) NO_INLINE_ATTR
	{
		FatShortDirEnt	*pDir;
		LONG_BYTES		dwb;

		// Compute position within directory sector
		pDir = &((FatShortDirEnt *)CurBuf())[pf->m_DirEnt];
		pf->m_flags.fDirty = 0;
		dwb.ul = pf->m_FirstClus.Cluster;
		pDir->FirstClusHi = dwb.uHi16;
		pDir->FirstClusLo = dwb.uLo16;
		CurBufDesc()->SetFlagsDirty();
		return pDir;
	}

	//****************************************************************************

	ulong SectFromClus(ulong dwCluster) NO_INLINE_ATTR
	{
		dwCluster -= 2;
		dwCluster <<= m_BPB.SecPerClusShift;
		dwCluster += m_BPB.DataStartSec;
		return dwCluster;
	}

	//****************************************************************************

	ulong ShiftClusByFatSize(ulong dwCluster)
	{
		dwCluster <<= 1;
		if (IsFat32())
			dwCluster <<= 1;

		return dwCluster;
	}

	//****************************************************************************

	ulong FatSectFromClus(ulong dwCluster) NO_INLINE_ATTR
	{
		dwCluster = ShiftClusByFatSize(dwCluster);

		// Divide by sector size to get sector number
		dwCluster /= FAT_SECT_SIZE;
		dwCluster += m_BPB.FatStartSec;
		return dwCluster;
	}

	//****************************************************************************

	int WriteFatEntry(ulong ulClus, ulong ulValue) NO_INLINE_ATTR
	{
		uint		wTmp;
		byte		*pbFat;

		wTmp = ShiftClusByFatSize(ulClus);

		// Skip to correct position in this FAT sector
		wTmp &= FAT_SECT_SIZE - 1;
		pbFat = CurBuf() + wTmp;
		if (m_BPB.fFat16)
			*(ushort *)pbFat = (ushort)ulValue;
		else
			((FatCluster *)pbFat)->Cluster = ulValue;

		CurBufDesc()->SetFlagsDirty();
		return FATERR_None;
	}
	//****************************************************************************

	FatCluster ReadFatEntry(ulong dwClus) NO_INLINE_ATTR
	{
		uint		uTmp;
		FatCluster	clus;
		union
		{
			byte	*pb;
			ushort	*pus;
			ulong	*pul;
		} pFat;

		uTmp = ShiftClusByFatSize(dwClus);

		// Skip to correct position in this FAT sector
		uTmp &= FAT_SECT_SIZE - 1;

		pFat.pb = CurBuf() + uTmp;
		if (m_BPB.fFat16)
		{
			clus.ul = *pFat.pus++;
			if ((ushort)clus.ul >= FAT16_EOC_MIN)
			{
				clus.ul = FAT32_EOC;
				goto EndCheck;
			}
		}
		else
		{
			clus.ul = *pFat.pul++;
			clus.ul = clus.Cluster;
			if (clus.ul >= FAT32_EOC_MIN)
				goto EndCheck;
		}

		// Perform sanity check
		if (clus.ul > m_BPB.MaxClus || clus.ul == 1)
			clus.ul = FATERR_FormatError;
		else
		{
	EndCheck:
			if (AtEndBuf(pFat.pb))
				clus.fLastSect = true;
		}

		return clus;
	}

	//*********************************************************************
	// Directory search

	static int CompareNamePart(const char *pchName, const ushort *pwDir, int cch, int cch1)
	{
		byte	ch1, ch2;
		const byte	*pbDir;

		// We ignore upper byte of Unicode characters in name
		pbDir = (const byte *)pwDir;

		if (cch1 > cch)
			cch1 = -1;	// This will be a final match
		else
			cch = cch1;	// Limit to directory field length

		while (cch)
		{
			ch1 = *pbDir;
			pbDir += 2;
			ch2 = *pchName++;
			// Flush to upper case for case insensitivity
			if (ch1 >= 'a' && ch1 <= 'z')
				ch1 -= 'a' - 'A';
			if (ch2 >= 'a' && ch2 <= 'z')
				ch2 -= 'a' - 'A';
			if (ch1 != ch2)
				return 0;

			cch--;
		}

		// If directory entry is longer, look for null terminator
		if (cch1 == -1 && *pbDir != 0)
			return 0;
		return cch1;
	}

	//****************************************************************************

	static int CompareLongName(const char *pchName, FatDirEnt *pDir, uint cch)
	{
		int	cDif;

		cDif = CompareNamePart(pchName, pDir->Long.Name1, cch, _countof(pDir->Long.Name1));
		if (cDif <= 0)
			return cDif;

		cch -= cDif;
		pchName += cDif;

		cDif = CompareNamePart(pchName, pDir->Long.Name2, cch, _countof(pDir->Long.Name2));
		if (cDif <= 0)
			return cDif;

		cch -= cDif;
		pchName += cDif;

		cDif = CompareNamePart(pchName, pDir->Long.Name3, cch, _countof(pDir->Long.Name3));
		return cDif;
	}

	//****************************************************************************

	static uint CopyLongName(char *pchName, FatDirEnt *pDir, uint cch)
	{
		byte	ch;
		uint	cbTotal;
		uint	cch1;
		byte	*pbDir;

		cbTotal = 0;

		// We ignore upper byte of Unicode characters in name
		pbDir = (byte *)pDir->Long.Name1;
		for (cch1 = _countof(pDir->Long.Name1); cch1 != 0; cch1--)
		{
			ch = *pbDir;
			if (ch == 0)
				goto EndDir;
			if (cch)
			{
				*pchName++ = ch;
				cch--;
			}
			pbDir += 2;
			cbTotal++;
		}

		pbDir = (byte *)pDir->Long.Name2;
		for (cch1 = _countof(pDir->Long.Name2); cch1 != 0; cch1--)
		{
			ch = *pbDir;
			if (ch == 0)
				goto EndDir;
			if (cch)
			{
				*pchName++ = ch;
				cch--;
			}
			pbDir += 2;
			cbTotal++;
		}

		pbDir = (byte *)pDir->Long.Name3;
		for (cch1 = _countof(pDir->Long.Name3); cch1 != 0; cch1--)
		{
			ch = *pbDir;
			if (ch == 0)
				goto EndDir;
			if (cch)
			{
				*pchName++ = ch;
				cch--;
			}
			pbDir += 2;
			cbTotal++;
		}

	EndDir:
		return cbTotal;
	}

	//****************************************************************************

	static uint CopyShortPart(char *pchDest, const char *pchSrc, uint cch, byte fLower)
	{
		const char	*pchCur;
		uint		cTotal;
		char		ch;

		pchCur = pchSrc + cch;
		do
		{
			if (*--pchCur != ' ')
				break;
		} while (--cch != 0);

		cTotal = cch;
		while (cch)
		{
			ch = *pchSrc++;
			if (fLower && ch >= 'A' && ch <= 'Z')
				ch += 'a' - 'A';
			*pchDest++ = ch;
			cch--;
		}
		return cTotal;
	}

	//****************************************************************************

	static byte RotateRight(byte b)
	{
		return ((b & 1) ? 0x80 : 0) + (b >> 1);
	}

	//****************************************************************************

	static byte ShortNameChecksum(char *pchName)
	{
		byte	cch;
		byte	bTmp;

		bTmp = 0;
		for (cch = sizeof(((FatShortDirEnt *)0)->Name); cch > 0; cch--)
			bTmp = RotateRight(bTmp) + *pchName++;
		return bTmp;
	}

	//****************************************************************************

	static void MakeDotName(FatShortDirEnt *pDir, ulong dwClus, FatDateTime daytime)
	{
		pDir->Name[0] = '.';
		memset(&pDir->Name[1], ' ', (sizeof pDir->Name) - 1);
		pDir->FirstClusHi = dwClus >> 16;
		pDir->FirstClusLo = (ushort)dwClus;
		pDir->WriteTime = daytime.time;
		pDir->WriteDate = daytime.date;
		pDir->Attr = ATTR_DIRECTORY;
	}

	//****************************************************************************

	void InitRootSearch(FatFile *pf)
	{
		pf->m_flags.b = 0;
		pf->m_flags.fIsFolder = 1;
		pf->m_flags.fIsRoot = 1;
		pf->m_flags.OpenType = OPENTYPE_Normal;
		pf->SetDrive(m_drive);
		if (IsFat32())
		{
			pf->m_FirstClus.ul = Fat32DirStart();
			pf->m_flags.fIsRoot = 0;
		}
	}

	//****************************************************************************

	int CompDir(FatDirEnt *pDir, byte fFindAll) NO_INLINE_ATTR
	{
		int		bTmp;
		int		pos;
		int		cch;
		int		retval;
		char	*pchDir;
		char	*pchName;
		char	ch1;
		char	ch2;

		bTmp = pDir->Long.Order;
		if (bTmp == EMPTY_DIR_ENTRY)
		{
			pos = 0;
			retval = COMP_Empty;
			goto Exit;
		}

		if (bTmp == END_DIR_ENTRY)
		{
			if (!(m_state.info.OpenFlags & OPENFLAG_FoundFree))
			{
				if (m_state.cFree == 0)
					m_state.cFree = -1;
				else
					m_state.cFree++;
			}
			return FATERR_FileNotFound;
		}

		// First check for long file name
		if ((pDir->Short.Attr & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME)
		{
			bTmp = pDir->Long.Order;
			pos = bTmp & ~LAST_LONG_ENTRY;
			pos = (pos - 1) * CHAR_PER_LONG_NAME;
			cch = m_state.cchFolderName - pos;

			if (bTmp & LAST_LONG_ENTRY)
			{
				// Have a new long file name
				if (fFindAll)
					m_state.cchFolderName = 0;
				else if (cch == 0 || cch > CHAR_PER_LONG_NAME)
					goto NoMatch;
				m_state.info.Checksum = pDir->Long.Checksum;
			}
			else
			{
				// In the middle of a long file name
				if (m_state.info.Checksum != pDir->Long.Checksum)
					goto NoMatch;
				if (pos != m_state.info.NamePos - CHAR_PER_LONG_NAME)
					goto NoMatch;
			}

			// Name length within range
			if (fFindAll)
			{
				if (pos < m_state.cchName)
					cch = m_state.cchName - pos;
				else
					cch = 0;
				m_state.cchFolderName += CopyLongName(m_state.pchName + pos, pDir, cch);
			}
			else if (!CompareLongName(m_state.pchName + pos, pDir, cch))
				goto NoMatch;

			// Tail end of name matched
			if (pos == 0)
				pos = LONG_NAME_MATCH;

			if (pDir->Long.Order & LAST_LONG_ENTRY)
			{
				retval = COMP_StartLong;
				goto Exit;
			}
			goto InLongName;
		}
		else
		{
			if (m_state.info.NamePos == LONG_NAME_MATCH)
			{
				bTmp = ShortNameChecksum(pDir->Short.Name);
				// Long names matched. Verify checksum on short name
				if (bTmp != m_state.info.Checksum)
					goto CopyShortName;	// Compare short names

				if (fFindAll)
				{
					if (m_state.cchFolderName <= m_state.cchName)
						m_state.pchName[m_state.cchFolderName] = '\0';
					else
					{
						m_state.pchName[m_state.cchName] = '\0';

						if (m_state.cchName < FAT_MIN_NAME_BUF)
							goto CopyShortName;
					}
				}
			}
			else
			{
	CopyShortName:
				pchName = m_state.pchName;
				if (fFindAll)
				{
					cch = CopyShortPart(pchName, pDir->Short.Name, 8, pDir->Short.CaseFlags & CASE_BASE_LOWER);
					pchName += cch;
					*pchName++ = '.';
					bTmp = CopyShortPart(pchName, &pDir->Short.Name[8], 3, pDir->Short.CaseFlags & CASE_EXT_LOWER);
					if (bTmp == 0)
						bTmp--;	// Overwrite '.' with null terminator
					pchName += (sbyte)bTmp;
					*pchName = '\0';
					m_state.cchFolderName = cch + bTmp + 1;
				}
				else
				{
					// Compare short names
					cch = m_state.cchFolderName;
					pchDir = pDir->Short.Name;
					bTmp = 8;
	CompShortName:
					do
					{
						ch1 = *pchDir++;	// get char from dir entry
						if (cch)
						{
							ch2 = *pchName;
							if (ch2 >= 'a' && ch2 <= 'z')
								ch2 -= 'a' - 'A';

							// Fill with blanks once we see the dot
							// But treat one or two leading dots as normal characters
							if (ch2 != '.' || bTmp == 8 || (bTmp == 7 && *(pchName - 1) == '.'))
							{
								cch--;
								pchName++;
							}
							else
								ch2 = ' ';
						}
						else
							ch2 = ' ';	// Fill with blanks

						if (ch1 != ch2)
							goto NoShortMatch;

					} while (--bTmp);

					if (pchDir == &pDir->Short.Name[8])
					{
						if (cch)
						{
							if (*pchName != '.')
								goto NoShortMatch;
							cch--;
							pchName++;
						}
						bTmp = 3;
						goto CompShortName;
					}
					else if (cch)
					{
	NoShortMatch:
						// Have a short name that didn't match. See if it might
						// match the short name we would create from the long name.
						// If the first 4 chars match, compare with checksum.
						if (bTmp <= 4 && ShortNameChecksum(pDir->Short.Name) == m_state.CreateChecksum)
							m_state.info.OpenFlags |= OPENFLAG_ShortConflict;
	NoMatch:
						pos = 0;
	InLongName:
						retval = COMP_NoMatch;
	Exit:
						m_state.info.NamePos = pos;
						return retval;
					}
				}
			}
		}

		// Success
		return COMP_Match;
	}

	//****************************************************************************

	int SearchDir(FatFile *pf, uint oDir = 0, bool fFindAll = false) NO_INLINE_ATTR
	{
		FatFile		*pfParent;
		FatDirEnt	*pDir;
		ulong		sect;
		int			err;
		int			cFree;
		int			bTmp;
		uint		wHasClus;
		FileFlags	flags;
		FileFlags	flagRoot;

		// Make sure directory sector is in buffer
		if (pf->IsRoot())
			sect = m_BPB.DataStartSec - m_BPB.RootSecCnt;
		else
			sect = SectFromClus(pf->m_CurClus.Cluster);

		sect += pf->m_DirSecInClus;
		if (!FindBuffer(sect))
			return StartBuf(sect);

		pDir = (FatDirEnt *)(CurBuf() + oDir);

		do
		{
			err = CompDir(pDir, fFindAll);
			if (IsError(err))
				return err;

			if (err == COMP_Match)
			{
				// Ignore Hidden, System, and Volume ID entries unless Hidden flag set
				err = m_state.info.OpenFlags;
				bTmp = pDir->Short.Attr & (ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID);
				if (bTmp && !(err & OPENFLAG_Hidden))
					goto NextEntry;

				if (fFindAll)
				{
					// Just skip if not type (file or folder) we want
					if (pDir->Short.Attr & ATTR_DIRECTORY)
					{
						// For directory, must have Folder bit set
						if (!(err & OPENFLAG_Folder))
							goto NextEntry;
					}
					else if (pDir->Short.Attr & ATTR_VOLUME_ID)
					{
						// For volume ID, must NOT have Folder or File bits set
						bTmp = err & (OPENFLAG_File | OPENFLAG_Folder);
						if (bTmp)
							goto NextEntry;
					}
					else
					{
						// For file, must have File bit set
						if (!(err & OPENFLAG_File))
							goto NextEntry;
					}

					// Save current search position in parent
					pfParent = HandleToPointer(m_state.info.hParent);
					pf->CopySearchLoc(pfParent);
				}

				// Have a match, fill in handle
				s_pvDataBuf = (byte *)pDir;	// return pointer to dir entry
				pf->m_FirstClus.uLo = pf->m_CurClus.uLo = pDir->Short.FirstClusLo;
				pf->m_FirstClus.uHi = pf->m_CurClus.uHi = pDir->Short.FirstClusHi;
				flagRoot = pf->m_flags;	// preserve if Delete/Rename
				flags = flagRoot;
				flags.fIsFolder = 0;
				flags.fIsRoot = 0;
				flags.fIsAll = 0;
				flags.fNextClus = 0;
				wHasClus = pf->m_FirstClus.uHi | pf->m_FirstClus.uLo;
				if (wHasClus == 0)
				{
					flags.fIsAll = 1;
					flags.fNextClus = 1;
				}
				pf->m_flags = flags;

				if (pDir->Short.Attr & ATTR_DIRECTORY)
				{
					// Have a folder
					flags.fIsFolder = 1;
					pf->m_flags = flags;
					if (wHasClus == 0)
						InitRootSearch(pf);

					if (!fFindAll)
					{
						// Need to find subfolder?
						m_state.pchName += m_state.cchFolderName;
						m_state.cchFolderName = ParseFolder();
						if (m_state.cchFolderName != 0)
							return StartOpen(pf);
					}

					if (!(err & OPENFLAG_Folder))
						return FATERR_FileNotFound;
				}
				else if (pDir->Short.Attr & ATTR_VOLUME_ID)
				{
					// For volume ID, must NOT have Folder or File bits set
					bTmp = err & (OPENFLAG_File | OPENFLAG_Folder);
					if (bTmp)
						return FATERR_FileNotFound;
				}
				else
				{
					// Have a file
					if (!fFindAll && m_state.cchName != 0)
						return FATERR_FileNotFound;

					if (!(err & OPENFLAG_File))
						return FATERR_FileNotFound;
				}

				bTmp = err & OPENFLAG_CreateBits;
				if (bTmp == OPENFLAG_CreateNew)
					return FATERR_FileExists;

				if (err & OPENFLAG_DeleteBit)
				{
					flags = pf->m_flags;
					flags.OpenType = OPENTYPE_Delete;
					// Restore original IsRoot flag
					if (flagRoot.fIsRoot)
						flags.fIsRoot = 1;
					pf->m_flags = flags;

					bTmp = m_state.info.NamePos;
					if (bTmp == LONG_NAME_MATCH)
						pf->RestoreDirPos();

					pf->m_DateTime.date = pDir->Short.WriteDate;
					pf->m_DateTime.time = pDir->Short.WriteTime;

					if (bTmp != LONG_NAME_MATCH)
						goto SetDirSec;
				}
				else
				{
					pf->m_CurPos = 0;
	SetDirSec:
					pf->m_DirSec = CurBufBlock();
				}
				pf->m_Length = pDir->Short.FileSize;

				// Truncate file if Create Always
				if ((m_state.info.OpenFlags & OPENFLAG_CreateBits) == OPENFLAG_CreateAlways
					&& pf->m_FirstClus.ul != 0)
				{
					ulong	dwTmp = pf->m_FirstClus.ul;
					m_state.op = FATOP_FreeChain;
					pf->m_flags.fIsAll = 1;
					pf->m_flags.fNextClus = 1;
					pf->m_flags.fDirty = 1;
					pf->m_FirstClus.ul = 0;
					pf->m_CurClus.ul = 0;
					pf->m_Length = 0;
					m_state.dwClusFree = dwTmp;
					return StartBuf(FatSectFromClus(dwTmp));
				}
				return FATERR_None;
			} // end of match

			cFree = m_state.cFree;
			if (m_state.info.OpenFlags & OPENFLAG_DeleteBit)
			{
				if (err == COMP_StartLong)
					goto SavePosition;
			}
			else if (!(m_state.info.OpenFlags & OPENFLAG_FoundFree))
			{
				// We are still looking for a free entry
				if (err == COMP_Empty)
				{
					if (cFree == 0)
					{
	SavePosition:
						// Copy this position.
						pf->SaveDirPos();
					}
					else if (cFree * CHAR_PER_LONG_NAME >= m_state.cchFolderName)
						m_state.info.OpenFlags |= OPENFLAG_FoundFree;

					cFree++;
				}
				else
					cFree = 0;
				m_state.cFree = cFree;
			}

	NextEntry:
			pf->m_DirEnt++;
			pDir++;
		} while (!AtEndBuf(pDir));

		// Didn't fine a match in this sector, move to next
		return ReadNextDir(pf);
	}

	//****************************************************************************

	int ReadFirstDir(FatFile *pf) NO_INLINE_ATTR
	{
		ulong	dwTmp;

		pf->m_flags.fIsAll = 0;
		if (pf->IsRoot())
			dwTmp = m_BPB.DataStartSec - m_BPB.RootSecCnt;
		else
		{
			// First cluster always has zero in Cnt field
			dwTmp = pf->m_CurClus.ul = pf->m_FirstClus.ul;
			if (dwTmp == 0)
				return FATERR_FormatError;	// should never happen

			// Compute sector from cluster
			dwTmp = SectFromClus(dwTmp);
		}
		m_state.info.NamePos = 0;	// position in long name
		m_state.cFree = 0;
		m_state.info.OpenFlags &= ~OPENFLAG_FoundFree;
		pf->m_DirSecInClus = 0;
		pf->m_DirEnt = 0;

		return StartBuf(dwTmp);
	}

	//****************************************************************************

	int ReadNextDir(FatFile *pf) NO_INLINE_ATTR
	{
		int		bPos;
		int		bCnt;
		int		err;
		ulong	dwTmp;

		// Convert current position to sector within cluster
		bPos = pf->m_DirSecInClus + 1;
		if (pf->IsRoot())
		{
			if (bPos >= m_BPB.RootSecCnt)
				return FATERR_FileNotFound;

			dwTmp = m_BPB.DataStartSec - m_BPB.RootSecCnt;
		}
		else
		{
			dwTmp = pf->m_CurClus.Cluster;

			if (bPos >= m_BPB.SecPerClus)
			{
				// We've run past the end of the cluster
				bPos = 0;
				bCnt = pf->m_CurClus.Cnt;
				if (bCnt == 0)
				{
					if (pf->IsAll())
						return FATERR_FileNotFound;

					// Need to go to the FAT
					err = StartFatRead(dwTmp);
					goto NewClus;
				}
				else
				{
					// Just read next consecutive sector
					dwTmp++;
					pf->m_CurClus.Cnt = bCnt - 1;
					pf->m_CurClus.Cluster = dwTmp;
				}
			}

			// Compute sector from cluster
			dwTmp = SectFromClus(dwTmp);
		}

		dwTmp += bPos;
		err = StartBuf(dwTmp);
NewClus:
		// Bump our position to match new sector
		pf->m_DirSecInClus = bPos;
		pf->m_DirEnt = 0;

		return err;
	}

	//****************************************************************************

	int ParseFolder()
	{
		char		ch;
		char		chFirst;
		char		*pchName;
		int			cchName;
		int			cch;

		cchName = m_state.cchName;
		pchName = m_state.pchName;
		do	// eat '.'
		{
			if (cchName == 0)
				return 0;

			// First eat any number of leading folder delimiters
			for (;;)
			{
				chFirst = *pchName;
				if (chFirst != '\\' && chFirst != '/')
					break;
				pchName++;
				cchName--;
				if (cchName == 0)
					return 0;
			}

			// Save pointer to start of folder name
			m_state.pchName = pchName;
			cch = cchName;

			// Now skip to next delimiter or end
			for (;;)
			{
				cchName--;
				if (cchName == 0)
					break;
				ch = *++pchName;
				if (ch == '\\' || ch == '/')
					break;
			}

			m_state.cchName = cchName;
			cch -= cchName;
		} while (cch == 1 && chFirst == '.');
		return cch;
	}

	//****************************************************************************

	int CreateShortName(FatShortName *pRes, uint num)
	{
		char	*pchLongCur;
		char	*pchShortCur;
		char	*pchBadChar;
		int		cchLong;
		int		cchShort;
		int		flags;
		char	ch;

		pchLongCur = m_state.pchName;
		cchLong = m_state.cchFolderName;
		pchShortCur = &pRes->Base[0];
		flags = 0;
		cchShort = 8;

		// Pre-fill result
		memset(pchShortCur, ' ', 11);

		ch = *pchLongCur;
		if (ch == ' ')
			return FATERR_InvalidFileName;
		if (ch == EMPTY_DIR_ENTRY)
		{
			ch = KANJI_LEAD_BYTE;
			goto SaveShort;
		}

		// Strip leading dots
		for (;;)
		{
			if (ch != '.')
				break;
			flags |= SHORTNAME_IsChanged;
			pchLongCur++;
			cchLong--;
			if (cchLong == 0)
				return SHORTNAME_CantCreate;
			ch = *pchLongCur;
		}

		// Copy characters to short name, doing case conversion
		do
		{
			ch = *pchLongCur++;

			// Check for dot, ending name portion
			if (ch == '.')
			{
				// See if we're already in extension
				if (pchShortCur + cchShort > &pRes->Ext[0])
				{
					// Fill out with blanks to cover previous extension
					pRes->Ext[0] = ' ';
					pRes->Ext[1] = ' ';
					pRes->Ext[2] = ' ';
					flags |= SHORTNAME_IsChanged;
				}
				else
				{
					// See if we can use case flag on base name
					if (flags & SHORTNAME_HasLower)
					{
						if (flags & SHORTNAME_HasUpper)
							flags |= SHORTNAME_CaseChange;
						else
							flags |= CASE_BASE_LOWER;
					}
				}
				flags &= ~(SHORTNAME_HasLower | SHORTNAME_HasUpper);

				// Set up for extension (will overwrite previous ones)
				pchShortCur = &pRes->Ext[0];
				cchShort = 3;
				goto NextLong;
			}

			// Don't allow control characters
			if (ch < ' ')
				return FATERR_InvalidFileName;

			// Remove embedded blanks from short name
			if (ch == ' ')
			{
				flags |= SHORTNAME_IsChanged;
				goto NextLong;
			}

			// Convert to upper case -- this does not change the name
			if (ch >= 'a' && ch <= 'z')
			{
				ch -= 'a' - 'A';
				flags |= SHORTNAME_HasLower;
			}
			else if (ch >= 'A' && ch <= 'Z')
			{
				flags |= SHORTNAME_HasUpper;
			}
			else
			{
				// Look for symbols not allowed in short name
				pchBadChar = strchr(archBadChars, ch);
				if (pchBadChar != NULL)
				{
					// See if symbol is allowed in long name
					if (pchBadChar >= archBadChars + sizeof(SHORT_NAME_BAD_CHARS) - 1)
						return FATERR_InvalidFileName;
					ch = '_';
					flags |= SHORTNAME_IsChanged;
				}
			}
			if (cchShort == 0)
				flags |= SHORTNAME_IsChanged;
			else
			{
	SaveShort:
				*pchShortCur++ = ch;
				cchShort--;
			}
	NextLong:
			cchLong--;
		} while (cchLong);

		// See if we can use case flags
		if (flags & SHORTNAME_HasLower)
		{
			if (flags & SHORTNAME_HasUpper)
				flags |= SHORTNAME_CaseChange;
			else
			{
				// In extension?
				if (pchShortCur + cchShort > &pRes->Ext[0])
					flags |= CASE_EXT_LOWER;
				else
					flags |= CASE_BASE_LOWER;
			}
		}

		// Numeric-tail generation if characters in name are different
		if (flags & SHORTNAME_IsChanged)
		{
			pchShortCur = &pRes->Base[6];
			if (num >= 10)
				pchShortCur--;
			if (num >= 100)
				pchShortCur--;

			*pchShortCur++ = '~';
			ch = pRes->Ext[0];
			itoa(num, pchShortCur, 10);
			pRes->Ext[0] = ch;
			flags = SHORTNAME_IsChanged;
		}

		pRes->CaseFlags = flags & SHORTNAME_CaseBits;

		return flags & (SHORTNAME_CaseChange | SHORTNAME_IsChanged);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

	static inline const char archBadChars[] = SHORT_NAME_BAD_CHARS LONG_NAME_BAD_CHARS;

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

protected:
	FatOpState m_state;
	FatLocalBpb m_BPB;
	OpCompletePv *m_pfnOpComplete;
	void *m_pvOpCompleteArg;
	byte	m_drive;

protected:
	inline static StatusChange *s_pfnStatusChange;
	inline static byte *s_pvDataBuf;
	inline static FatFile s_arHandles[FAT_MAX_HANDLES];

	friend class FatSys;
};
