//****************************************************************************
// Class FatBuffer
// FatBuffer.h
//
// Created 11/9/2020 4:46:37 PM by Tim
//
//****************************************************************************

#pragma once

#include <FatFile\FatFileConst.h>


#ifndef FAT_SECT_BUF_CNT
#define FAT_SECT_BUF_CNT	4
#endif

static constexpr ulong INVALID_BUFFER = 0xFFFFFFFF;

class FatBuffer;

class FatBufDesc
{
public:
	void InvalidateBuf()		{ m_block = INVALID_BUFFER; }
	void SetBlock(ulong block, byte drive)	{ m_block = block; m_drive = drive; }
	ulong GetBlock()			{ return m_block; }
	byte GetDrive()				{ return m_drive; }
	bool IsDirty()				{ return m_fIsDirty; }
	void SetFlagsDirty(bool f = true)	{m_fIsDirty = f;}
	bool IsPriority()			{return m_handle != 0;}
	void ClearPriority()		{m_handle = 0;}
	void SetPriority(byte h)	{m_handle = h;}

protected:
	ulong	m_block;
	bool	m_fIsDirty;
	byte	m_drive;
	byte	m_handle;	// file that owns this buffer when it has priority

	friend FatBuffer;
};

class FatBuffer
{
	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	static byte *BufFromIndex(byte iBuf)	{return (byte *)&s_arSectBuf[iBuf];}
	static byte CurBufIndex()				{return s_nextBuf;}
	static byte *CurBuf()					{return BufFromIndex(CurBufIndex());}
	static byte *EndCurBuf()				{return BufFromIndex(CurBufIndex()) + FAT_SECT_SIZE;}
	static bool AtEndBuf(void *pv)
		{return ((uint)pv & (FAT_SECT_SIZE - 1)) == ((uint)s_arSectBuf & (FAT_SECT_SIZE - 1));}
	static FatBufDesc *BufDescFromIndex(byte iBuf)	{return &s_desc[iBuf];}
	static FatBufDesc *CurBufDesc()					{ return s_pDesc; }

public:
	static void InvalidateAll()
	{
		for (int i = 0; i < FAT_SECT_BUF_CNT; i++)
			s_desc[i].m_block = INVALID_BUFFER;
	}

	static void InvalidateAll(uint drive)
	{
		for (int i = 0; i < FAT_SECT_BUF_CNT; i++)
		{
			if (s_desc[i].m_drive == drive)
				s_desc[i].m_block = INVALID_BUFFER;
		}
	}

	static void ClearPriority(uint handle)
	{
		for (int i = 0; i < FAT_SECT_BUF_CNT; i++)
		{
			if (s_desc[i].m_handle == handle)
				s_desc[i].ClearPriority();
		}
	}

	static byte *FindBuffer(ulong dwBlock, uint drive)
	{
		int			buf;
		FatBufDesc *pDesc;

		for (buf = 0; buf < FAT_SECT_BUF_CNT; buf++)
		{
			pDesc = BufDescFromIndex(buf);
			if (pDesc->m_block == dwBlock && pDesc->m_drive == drive)
			{
				s_nextBuf = buf;
				s_pDesc = pDesc;
				return BufFromIndex(buf);
			}
		}
		return NULL;
	}

	static uint GetFreeBuf(byte drive)
	{
		int			i;
		uint		uCur;
		FatBufDesc	*pDesc;

		// Scan for a buffer that doesn't have priority. If all buffers
		// have priority, the loop will scan around to one past current.
		// Ignore dirty buffer on another drive.
		uCur = s_nextBuf;
		i = FAT_SECT_BUF_CNT;
		do
		{
			uCur++;
			i--;
			if (uCur >= FAT_SECT_BUF_CNT)
				uCur = 0;
			pDesc = BufDescFromIndex(uCur);
		} while ((pDesc->IsDirty() && pDesc->GetDrive() != drive) || (pDesc->IsPriority() && i >= 0));

		s_nextBuf = uCur;
		s_pDesc = pDesc;

		return uCur;
	}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

protected:
	inline static byte			s_nextBuf;
	inline static FatBufDesc	*s_pDesc;
	inline static FatBufDesc	s_desc[FAT_SECT_BUF_CNT];
	// Ensure buffers are aligned on 32-bit boundary
	inline static byte			s_arSectBuf[FAT_SECT_BUF_CNT][FAT_SECT_SIZE] ALIGNED_ATTR(uint32_t);
};
