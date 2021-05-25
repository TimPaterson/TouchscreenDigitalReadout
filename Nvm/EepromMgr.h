//****************************************************************************
// Class EepromMgr
// EepromMgr.h
//
// Created 2/19/2019 4:45:39 PM by Tim
//
//****************************************************************************

#pragma once


#include <Nvm\Nvm.h>
#include <algorithm>	// for min()

#ifndef NVMCTRL_RWW_EEPROM_ADDR
#define NVMCTRL_RWW_EEPROM_ADDR	(FLASH_SIZE - NVMCTRL_RWWEE_PAGES * FLASH_PAGE_SIZE)
#endif

enum NvmWriteState
{
	ST_None,
	ST_Start,
	ST_WaitErase,
	ST_WaitFill,
};


template <class T, const T *pInit, int cReservedRows = 0> class EepromMgr
{
#define	ALIGN32(x)	(((x) + 3) & ~3)

protected:
	struct RowDesc
	{
		ushort	usVersion;
		ushort	usSize;
		ulong	ulCheckSum;
	};

	static constexpr int FlashRowSize = FLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES;
	static constexpr int ManagedEepromStart = NVMCTRL_RWW_EEPROM_ADDR + cReservedRows * FlashRowSize;
	static constexpr ulong Unprogrammed = 0xFFFFFFFF;
	static constexpr ushort InvalidVersion = (ushort)Unprogrammed;
	static constexpr int RowDataSize = FlashRowSize - sizeof(RowDesc);
	static constexpr int MaxEepromDataSize = NVMCTRL_RWWEE_PAGES / NVMCTRL_ROW_PAGES * RowDataSize;
	static constexpr int PageHeadDataSize = FLASH_PAGE_SIZE - sizeof(RowDesc);
	static constexpr int AlignedSize = ALIGN32(sizeof(T));
	static constexpr int iRowCount = (AlignedSize + RowDataSize - 1) / RowDataSize;
	
// public interface
public:
	void Reset()
	{
		memcpy(m_arbPaddedData, pInit , sizeof(T));
	}

public:
	void NO_INLINE_ATTR Init()
	{
		uint	iCount;
		
		iCount = Load();
		if (iCount < sizeof(T))
		{
			// Copy in exact amount of data. Padding will retain its zero initialization.
			memcpy(m_arbPaddedData + iCount, ADDOFFSET(pInit, iCount) , sizeof(T) - iCount);
		}
	}

	void StartSave()
	{
		m_iCurRow = 0;
		m_state = ST_Start;
	}

	void NO_INLINE_ATTR Save()
	{
		StartSave();
		while (m_state != ST_None)
			ProcessSave();
	}
	
	void Process()
	{
		if (m_state == ST_None)
			return;
		ProcessSave();
	}

	//*********************************************************************

// Protected methods
protected:

	void NO_INLINE_ATTR ProcessSave()
	{
		int		iRow;
		int		iCount;
		int		cbRow;
		int		cbPage;
		ushort	usVer;
		ulong	ulCheck;
		byte	*pbPos;
		RowDesc	*pRowLo;
		RowDesc	*pRow;
		RowDesc	*pRowCur;

		if (!Nvm::IsReady())
			return;

		iRow = m_iCurRow;
		if (iRow >= iRowCount)
		{
			m_state = ST_None;
			return;
		}

		pRowLo = (RowDesc *)(ManagedEepromStart + iRow * FlashRowSize * 2);
		pbPos = m_arbPaddedData + iRow * FlashRowSize;
		iCount = AlignedSize - iRow * FlashRowSize;
		pRow = m_arpRow[iRow];
		pRowCur = (RowDesc *)ADDOFFSET(pRowLo, FlashRowSize);
		if (pRowLo == pRow)
		{
			// Using Lo now, rewrite Hi
			pRow = pRowCur;		// row to write, if needed
			pRowCur = pRowLo;	// original
		}
		else
			pRow = pRowLo;

		usVer = pRowCur->usVersion + 1;
		if (usVer == InvalidVersion)
			usVer = 0;

		switch (m_state)
		{
			case ST_None:
				return;

			case ST_Start:
				// See if any changes have been made to the original row
				cbRow = std::min(iCount, RowDataSize);
				ulCheck = CalcChecksum(pbPos, cbRow);
				if (pRowCur->usSize == sizeof(T) && 
					ulCheck == pRowCur->ulCheckSum && 
					memcmp(pRowCur + 1, pbPos, cbRow) == 0)
				{
					m_iCurRow++;
					return;
				}
				m_ulCheck = ulCheck;
				m_state = ST_WaitErase;
				//
				// Fall into ST_WaitErase state
				//
			case ST_WaitErase:
				// Clear the row we're using
				Nvm::EraseRwweeRowReady(pRow);
				m_iCurPage = 1;	// Skip first page with row header for now
				m_state = ST_WaitFill;
				return;

			case ST_WaitFill:
				if (m_iCurPage < NVMCTRL_ROW_PAGES)
				{
					// Write to the page buffer. These are the pages after
					// the first, with no header.
					cbRow = m_iCurPage * FLASH_PAGE_SIZE;	// bytes to skip over (w/header)
					pRowCur = (RowDesc *)ADDOFFSET(pRow, cbRow);
					cbRow -= sizeof(RowDesc);	// actual data bytes skipped
					cbPage = iCount - cbRow;
					if (cbPage > 0)
					{
						if (cbPage > FLASH_PAGE_SIZE)
							cbPage = FLASH_PAGE_SIZE;
						pbPos += cbRow;
						Nvm::memcpy32(pRowCur, pbPos, cbPage);
						Nvm::WriteRwweePageReady();
						m_iCurPage++;
						return;		// come back here to WaitFill
					}
				}

				// First page has descriptor
				pRow->usVersion = usVer;
				pRow->usSize = sizeof(T);
				pRow->ulCheckSum = m_ulCheck;

				cbPage = std::min(iCount, PageHeadDataSize);
				Nvm::memcpy32(pRow + 1, pbPos, cbPage);
				Nvm::WriteRwweePageReady();
				m_arpRow[m_iCurRow++] = pRow;	// update row we are using

				m_state = ST_Start;
				return;
		} // switch
	}

	//*********************************************************************

	int Load()
	{
		int		iCount;
		int		cbLoad;
		int		cbRow;
		RowDesc	*pRowLo;
		RowDesc	*pRowHi;
		RowDesc	*pRow;
		RowDesc	**ppRow;

		cbLoad = 0;
		pRowLo = (RowDesc *)ManagedEepromStart;
		ppRow = &m_arpRow[0];
		do
		{
			pRowHi = (RowDesc *)ADDOFFSET(pRowLo, FlashRowSize);
			if (pRowHi->usVersion == InvalidVersion)
			{
				if (pRowLo->usVersion == InvalidVersion)
					return cbLoad;
				pRow = pRowLo;
			}
			else if (pRowLo->usVersion == InvalidVersion)
			{
				pRow = pRowHi;
			}
			else
			{
				// Both rows have valid version numbers, pick biggest.
				// Note version no. can wrap around zero, so we 
				// subtract them instead of just comparing.
				if ((short)(pRowLo->usVersion - pRowHi->usVersion) > 0)
					pRow = pRowLo;
				else
					pRow = pRowHi;
			}
			
			// Use count from first row
			if (cbLoad == 0)
				iCount = pRow->usSize;
				
			// See if row is valid
			if (pRow->usSize != iCount || 
				iCount > MaxEepromDataSize || 
				CalcChecksumRow(pRow, iCount - cbLoad) != pRow->ulCheckSum)
			{
				// Bad checksum, see if other row is OK
				pRow = pRow == pRowLo ? pRowHi : pRowLo;
				
				// Use count from first row
				if (cbLoad == 0)
					iCount = pRow->usSize;
				
				// See if row is valid
				if (pRow->usVersion == InvalidVersion || 
					pRow->usSize != iCount || 
					iCount > MaxEepromDataSize || 
					CalcChecksumRow(pRow, iCount - cbLoad) != pRow->ulCheckSum)
				{
					return cbLoad;
				}
			}

			// Picked our row, copy data
			*ppRow++ = pRow;

			cbRow = std::min(std::min(ALIGN32(iCount), AlignedSize) - cbLoad, RowDataSize);
			Nvm::memcpy32(m_arbPaddedData + cbLoad, pRow + 1, cbRow);
			cbLoad += cbRow;
			pRowLo = (RowDesc *)ADDOFFSET(pRowLo, 2 * FlashRowSize);
			
		} while (cbLoad < iCount);
		
		return iCount;
	}

	//*********************************************************************
	// Static functions

protected:
	static ulong NO_INLINE_ATTR CalcChecksumRow(RowDesc *pRow, int cb)
	{
		cb = ALIGN32(cb);
		return CalcChecksum(pRow + 1, std::min(cb, RowDataSize));
	}

	static ulong NO_INLINE_ATTR CalcChecksum(void *pRow, int cb)
	{
		int		sum;
		
#if	0	// Use CRC
		PAC->WRCTRL.reg = PAC_WRCTRL_KEY_CLR | PAC_WRCTRL_PERID(ID_DSU);
		DSU->ADDR.reg = (ulong)pRow;
		DSU->LENGTH.reg = cb;
		DSU->DATA.reg = 0xFFFFFFFF;
		DSU->CTRL.reg = DSU_CTRL_CRC;
		while (!DSU->STATUSA.bit.DONE);
		DSU->STATUSA.reg = DSU_STATUSA_DONE;	// reset bit
		sum = DSU->DATA.reg;
#else
		int		i;
		uint	*puData;
	
		sum = cb;
		puData = (uint *)pRow;
		for (i = 0; i < (int)(cb / sizeof *puData); i++)
		{
			sum += *puData++;
		}
#endif

		return sum;
	}

	//*********************************************************************
	// Data

public:
	union
	{
		T		Data;
		byte	m_arbPaddedData[AlignedSize];
	};

protected:
	RowDesc	*m_arpRow[iRowCount];
	// State needed for non-blocking operation
	ulong	m_ulCheck;
	byte	m_state;
	byte	m_iCurRow;
	byte	m_iCurPage;
};
