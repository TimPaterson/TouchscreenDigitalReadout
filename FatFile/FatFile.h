//****************************************************************************
// FatFile.h
//
// Created 11/10/2020 9:35:58 AM by Tim
//
//****************************************************************************

#pragma once

#include <FatFile\Fat.h>
#include <FatFile\FatBuffer.h>


union FatCluster
{
	struct
	{
		ulong	Cluster:28;
		ulong	Cnt:4;
	};
	struct
	{
		ushort	uLo;
		ushort	uHi;
	};
	struct
	{
		ulong	:30;
		ulong	fLastSect:1;
		ulong	fError:1;
	};
	ulong	ul;

	bool IsError()	{ return fError; }
};

#define	CLUS_COUNT_MAX	15	// max count in 4 bits of Cnt


union DirEntPos
{
	struct
	{
		ulong	DirEnt:4;
		ulong	DirSec:28;
	};
	ulong	ul;
};

union FileFlags
{
	struct
	{
		byte	OpenType:2;
		byte	fNextClus:1;	// Current position is first byte of next cluster
		byte	fIsAll:1;
		byte	fIsFolder:1;
		byte	fIsRoot:1;
		byte	fDirty:1;
	};
	byte	b;
};

#define OPENTYPE_Free	0
#define OPENTYPE_Normal	1
#define OPENTYPE_Enum	2
#define OPENTYPE_Delete	3


//*************************************************************************
// class FatFile
//
// This keeps all the information for an open file. It has a fixed
// static array of these, indexed by file handle.
//
//*************************************************************************

class FatFile
{
	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	void RestoreDirPos()
	{
		m_CurClus.ul = m_Length;
		m_DirEnt = m_SavedDirEnt;
		m_DirSecInClus = m_SavedDirSecInClus;
		//m_DirSec = SectFromClus(m_CurClus.Cluster) + m_DirSecInClus;
	}

	void SaveDirPos()
	{
		m_Length = m_CurClus.ul;
		m_SavedDirEnt = m_DirEnt;
		m_SavedDirSecInClus = m_DirSecInClus;
		m_DirSec = FatBuffer::CurBufDesc()->GetBlock();
	}

	void DupFolder(FatFile *pf)
	{
		pf->m_flags = m_flags;
		pf->m_FirstClus = m_FirstClus;
		pf->m_Drive = m_Drive;
	}

	void CopySearchLoc(FatFile *pf)
	{
		pf->m_CurClus = m_CurClus;
		pf->m_DirEnt = m_DirEnt;
		pf->m_DirSecInClus = m_DirSecInClus;
	}

	byte IsOpen()		{return m_flags.OpenType != OPENTYPE_Free;}
	void Close()		{m_flags.b = 0;}
	byte IsFolder()		{return m_flags.fIsFolder;}
	byte IsRoot()		{return m_flags.fIsRoot;}
	byte IsAll()		{return m_flags.fIsAll;}
	byte IsDirty()		{return m_flags.fDirty;}
	ulong Length()		{return m_Length;}
	ulong CurPos()		{return m_CurPos;}
	void SearchNext()	{m_DirEnt++;}
	byte GetDrive()		{return m_Drive;}
	void SetDrive(byte d)	{m_Drive = d;}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

public:
	FatCluster	m_FirstClus;
	FatCluster	m_CurClus;	// Current position
	ulong		m_Length;
	DirEntPos	m_DirEntPos;
	union
	{
		ulong	m_CurPos;	// For files
		struct				// For folders
		{
			byte	m_SavedDirEnt;
			byte	m_SavedDirSecInClus;
			union
			{
				byte	m_ShortNameNumTail;
				byte	m_OpenFlags;	// for enumeration
			};
		};
		FatDateTime	m_DateTime;		// for rename
	};

	FileFlags	m_flags;
	byte		m_Drive;

	// Directory entry location
	byte		m_DirEnt;
	byte		m_DirSecInClus;
	ulong		m_DirSec;
};
