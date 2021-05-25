//****************************************************************************
// SdCard.h
//
// Created 11/11/2020 2:31:57 PM by Tim
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
#include <FatFile/SdCard/SdConst.h>


static constexpr int SDCARD_MAX_READ_WAIT	= 1000;
static constexpr int SDCARD_MAX_WRITE_WAIT	= 50000;
static constexpr int SDCARD_MAX_MOUNT_WAIT	= 10000;

#define	ErrGoTo(e, loc)		do {err = e; goto loc;} while (0)
#define	ErrGoDeselect(e)	ErrGoTo(e, Deselect)

template<class T>
class SdCard : public FatDrive, public T
{
	//*********************************************************************
	// Types
	//*********************************************************************

	enum SdOpState
	{
		SDOP_None,
		SDOP_Read,
		SDOP_Write,
		SDOP_Mount,
	};

	enum SdMountFlags
	{
		SDMOUNT_NotMounted,
		SDMOUNT_LoCap,
		SDMOUNT_HiCap
	};

	//*********************************************************************
	// Implementation of Storage class in FatDrive
	//*********************************************************************
public:
	virtual int GetStatus()
	{
		int		err;
		int		res;
		uint	cBlock;

		if (m_state == SDOP_None)
			return STERR_None;

		Select();
		switch (m_state)
		{
		case SDOP_Read:
			err = CheckReadStatus();
			if (err == STERR_Busy)
			{
				if (--m_cRetry == 0)
					err = STERR_TimeOut;
				break;
			}

			if (err == STERR_None)
			{
				T::ReadBytes(m_pData, SDCARD_BLOCK_SIZE);

				// Discard CRC bytes
				SpiRead();
				SpiRead();

				m_state = SDOP_None;
				Deselect();

				// Start next block
				cBlock = m_cBlock - 1;
				if (cBlock > 0)
				{
					err = ReadData(++m_Lba, m_pData + SDCARD_BLOCK_SIZE, cBlock);
					if (err == STERR_None)
						return STERR_Busy;
				}
				return err;
			}
			break;

		case SDOP_Write:
			err = SpiRead();
			if (err == 0)
			{
				if (--m_cRetry == 0)
					err = STERR_TimeOut;
				else
					err = STERR_Busy;
			}
			else
			{
				err = GetCardStatus();
				if (err == STERR_None)
				{
					m_state = SDOP_None;
					Deselect();

					// Start next block
					cBlock = m_cBlock - 1;
					if (cBlock > 0)
					{
						err = WriteData(++m_Lba, m_pData + SDCARD_BLOCK_SIZE, cBlock);
						if (err == STERR_None)
							return STERR_Busy;
					}
					return err;
				}
			}
			break;

		case SDOP_Mount:
			err = SendCommand(SDCARD_APP_CMD);
			err = SendCommand(SDCARD_APP_SEND_OP_COND, SDCARD_OP_COND_HCS_ARG);
			res = SDMOUNT_NotMounted;

			if (err == SDCARD_R1_READY)
			{
				// See if hi capacity
				err = SendCommand(SDCARD_READ_OCR);
				res = SDMOUNT_LoCap;
				if (err == SDCARD_R1_READY)
				{
					// Get last 4 bytes of response R3
					err = SpiRead();
					if (err & SDCARD_R3_MSB_CCS)
						res = SDMOUNT_HiCap;

					for (int j = 3; j > 0; j--)
						err = SpiRead();
				}
				err = res;
			}
			else
			{
				if (--m_cRetry == 0)
					err = STERR_TimeOut;
				else
					err = STERR_Busy;
			}

			// Crank the speed up
			m_fMount = res;
			T::SetClockFast();
			break;

		default:
			err = STERR_None;
			break;
		}

		Deselect();
		if (err != STERR_Busy)
			m_state = SDOP_None;
		return err;
	}

	virtual int InitDev()
	{
		return STERR_None;
	}

	virtual int MountDev()
	{
		int		i;
		int		err;

		m_fMount = SDMOUNT_NotMounted;

		if (!T::SdCardPresent())
			return STERR_NoMedium;

		T::SetClockSlow();

		for (i = 10; i > 0; i--)
			SpiRead();

		Select();

		for (i = SDCARD_BLOCK_SIZE - 1; i > 0; i--)
			SpiRead();

		err = SendCommand(SDCARD_GO_IDLE_STATE);
		if (err != SDCARD_R1_IDLE)
			ErrGoDeselect(STERR_NoMedium);

		// See if v.2 card
		err = SendCommand(SDCARD_SEND_IF_COND, SDCARD_IF_COND_VHS_ARG);
		if (err == SDCARD_R1_IDLE)
		{
			// v.2 card. Get last 4 bytes of response R7
			for (i = 3; i > 0; i--)
			{
				err = SpiRead();
			}
			if (err !=	SDCARD_VHS)
				ErrGoDeselect(STERR_DevFail);

			err = SpiRead();
			if (err !=	SDCARD_CHECK_PATTERN)
				ErrGoDeselect(STERR_DevFail);
		}

		m_state = SDOP_Mount;
		m_cRetry = SDCARD_MAX_MOUNT_WAIT;
		err = STERR_None;

	Deselect:
		Deselect();
		return err;
	}

	virtual int ReadData(ulong Lba, void *pv, uint cBlock)
	{
		int		err;

		if (m_state != SDOP_None)
			return STERR_Busy;

		m_Lba = Lba;
		m_pData = (byte *)pv;
		m_cBlock = cBlock;

		// Convert 512-byte block to byte address if not hi cap.
		if (m_fMount == SDMOUNT_LoCap)
			Lba <<= 9;

		Select();
		err = SendCommand(SDCARD_READ_BLOCK, Lba);
		Deselect();
		if (err != SDCARD_R1_READY)
			return MapR1Err(err);

		// Access has started
		m_state = SDOP_Read;
		m_cRetry = SDCARD_MAX_READ_WAIT;

		return STERR_None;
	}

	virtual int WriteData(ulong Lba, void *pv, uint cBlock)
	{
		int		err;

		if (m_state != SDOP_None)
			return STERR_Busy;

		m_Lba = Lba;
		m_pData = (byte *)pv;
		m_cBlock = cBlock;

		// Convert 512-byte block to byte address if not hi cap.
		if (m_fMount == SDMOUNT_LoCap)
			Lba <<= 9;

		Select();
		err = SendCommand(SDCARD_WRITE_BLOCK, Lba);
		if (err != SDCARD_R1_READY)
			ErrGoDeselect(MapR1Err(err));

		// Send data token
		SpiWrite(SDCARD_START_TOKEN);
		T::WriteBytes((byte *)pv, SDCARD_BLOCK_SIZE);

		// Get data response
		for (int i = 0; i < 10; i++)
		{
			err = SpiRead();
			if (err != 0xFF)
				break;
		}

		err &= SDCARD_DATRESP_MASK;
		if (err != SDCARD_DATRESP_ACCEPTED)
		{
			if (err == SDCARD_DATRESP_WRITE_ERR)
				ErrGoDeselect(GetCardStatus());
			ErrGoDeselect(STERR_DevFail);
		}

		// No error, set up time-out on write
		m_state = SDOP_Write;
		m_cRetry = SDCARD_MAX_WRITE_WAIT;
		err = STERR_None;

	Deselect:
		Deselect();
		return err;
	}

	virtual int DismountDev()
	{
		return STERR_None;
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	void SpiWrite(byte b)	{ T::SpiWrite(b); }
	byte SpiRead()			{ return T::SpiRead(); }
	void Select()			{ T::Select(); }
	void Deselect()			{ T::Deselect(); }

	uint SendCommand(uint cmd, ulong ulArg = 0)
	{
		uint		bTmp;
		LONG_BYTES	lbArg;

		lbArg.ul = ulArg;

		// SPI must be ready
		SpiRead();
		SpiWrite(cmd | 0x40);

		SpiWrite(lbArg.bHi);
		SpiWrite(lbArg.bMidHi);
		SpiWrite(lbArg.bMidLo);
		SpiWrite(lbArg.bLo);

		if (cmd == SDCARD_SEND_IF_COND)
			bTmp = SDCARD_IF_COND_CRC;
		else
			bTmp = SDCARD_GO_IDLE_CRC;

		SpiWrite(bTmp);
		SpiRead();				// Skip first return byte

		// Get response
		for (int i = 0; i < 10; i++)
		{
			bTmp = SpiRead();
			if (bTmp != 0xFF)
				break;
		}

		return bTmp;
	}

	//****************************************************************************

	int MapR1Err(uint err)
	{
		if (err == SDCARD_R1_READY)
			return STERR_None;

		if (err & SDCARD_R1_ADDR_ERR)
			return STERR_InvalidAddr;

		if (err & SDCARD_R1_BAD_CMD)
			return STERR_BadCmd;

		if (err & SDCARD_R1_CRC_ERR)
			return STERR_BadBlock;

		return STERR_DevFail;
	}

	//****************************************************************************

	int CheckReadStatus()
	{
		uint	bTmp;

		// Check for data
		bTmp = SpiRead();
		if (bTmp == SDCARD_START_TOKEN)
			return STERR_None;

		if (bTmp == 0xFF)
			return STERR_Busy;	// Still waiting for data

		// Data read error token
		if (bTmp & SDCARD_ERRTOK_BAD_DATA)
			return STERR_BadBlock;

		if (bTmp & SDCARD_ERRTOK_BAD_ADDR)
			return STERR_InvalidAddr;

		return STERR_DevFail;
	}

	//****************************************************************************

	int GetCardStatus() NO_INLINE_ATTR
	{
		uint	b1;
		uint	b2;

		b1 = SendCommand(SDCARD_SEND_STATUS);
		b2 = SpiRead();	// 2nd byte of status
		if (b2 != 0)
		{
			if (b2 & (SDCARD_R2_WRITE_PROT | SDCARD_R2_CARD_LOCKED))
				return STERR_WriteProtect;
			return STERR_DevFail;
		}

		return MapR1Err(b1);
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************

	byte	*m_pData;
	ulong	m_Lba;
	ushort	m_cRetry;
	ushort	m_cBlock;
	byte	m_state;
	byte	m_fMount;
};
