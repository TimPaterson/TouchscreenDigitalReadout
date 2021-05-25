//****************************************************************************
// Class Xtp2046
// Xtp2046.h
//
// Created 10/13/2020 4:35:49 PM by Tim
//
//****************************************************************************

#pragma once

#include <Com/Spi.h>
#include "ResTouch.h"


struct TouchInfo
{
	ScaleMatrix	scaleX;
	ScaleMatrix	scaleY;
	ushort	minZtouch;
	byte	updateRate;
	byte	sampleDiscard;
	byte	averageShift;
	byte	reserved[3];	// round up to multiple of 32 bits
};

//****************************************************************************
// Default EEPROM values

static constexpr ushort TouchDefaultMinZ = 200;
static constexpr int TouchUpdateRate = 30;	// full touch scans per second
static constexpr int TouchInitialDiscard = 4;
static constexpr int TouchAverageShift = 3;


class Xtp2046 : public ResTouch, public DECLARE_SPI(SERCOM1, RtpCs_PIN)
{
public:
	static constexpr int BaudRate = 1000000;

public:
	// Types
	enum ControlByte
	{
		// Analog input selections
		// These are for differential mode
		ADDR_X_Val = 5,
		ADDR_Y_Val = 1,
		ADDR_Z1_Val = 3,
		ADDR_Z2_Val = 4,

		ADDR_pos = 4,	// bit position in control byte

		ADDR_X =	(ADDR_X_Val << ADDR_pos),
		ADDR_Y =	(ADDR_Y_Val << ADDR_pos),
		ADDR_Z1 =	(ADDR_Z1_Val << ADDR_pos),
		ADDR_Z2 =	(ADDR_Z2_Val << ADDR_pos),

		// Mode: 12-bit or 8-bit
		MODE_12Bit_Val = 0,
		MODE_8Bit_Val = 1,

		MODE_pos = 3,

		MODE_12Bit =	(MODE_12Bit_Val << MODE_pos),
		MODE_8Bit =		(MODE_8Bit_Val << MODE_pos),

		// Reference: single ended or differential
		REF_Dif_Val = 0,
		REF_Sngl_Val = 1,

		REF_pos = 2,

		REF_Dif =		(REF_Dif_Val << REF_pos),
		REF_Sngl =		(REF_Sngl_Val  << REF_pos),

		// Power down mode
		PWR_Save_Val = 0,
		PWR_RefOffAdcOn_Val = 1,
		PWR_RefOnAdcOff_Val = 2,
		PWR_On_Val = 3,

		PWR_pos = 0,

		PWR_Save =			(PWR_Save_Val << PWR_pos),
		PWR_RefOffAdcOn =	(PWR_RefOffAdcOn_Val << PWR_pos),
		PWR_RefOnAdcOff =	(PWR_RefOnAdcOff_Val << PWR_pos),
		PWR_On =			(PWR_On_Val << PWR_pos),

		// Start bit
		RTP_Start = 0x80,

		// Combined values
		RTP_ReadX = RTP_Start | MODE_12Bit | REF_Dif | PWR_Save | ADDR_X,
		RTP_ReadY = RTP_Start | MODE_12Bit | REF_Dif | PWR_Save | ADDR_Y,
		RTP_ReadZ1 = RTP_Start | MODE_12Bit | REF_Dif | PWR_Save | ADDR_Z1,
		RTP_ReadZ2 = RTP_Start | MODE_12Bit | REF_Dif | PWR_Save | ADDR_Z2,
	};


public:
	void Init(SpiInPad padMiso, SpiOutPad padMosi, TouchInfo *pInfo, uint width, uint height)
	{
		SpiInit(padMiso, padMosi, SPIMODE_0);
		SetBaudRateConst(BaudRate);

		// Set scaling values
		SetMax(width, height);
		SetMatrix(pInfo->scaleX, pInfo->scaleY);
		m_minZtouch = pInfo->minZtouch;
		m_avgShift = pInfo->averageShift;
		m_discardCnt = pInfo->sampleDiscard;
		m_sampleCnt = (1 << m_avgShift) + m_discardCnt;
		m_scanTicks = Timer::TicksFromFreq(pInfo->updateRate * m_sampleCnt);

		m_tmr.Start();
	}

	ushort GetRawX()	{ return m_rawX; }
	ushort GetRawY()	{ return m_rawY; }
	ushort GetRawZ()	{ return m_rawZ; }

	bool Process() NO_INLINE_ATTR
	{
		if (!m_tmr.CheckInterval_ticks(m_scanTicks))
			return false;

		//if (GetRtpPenIrq() == 0)
		m_rawZ = ReadValue(RTP_ReadZ1);
		if (m_rawZ >= m_minZtouch)
		{
			// Touching
			if (!m_fPrevTouch)
			{
				// Just starting contact
				m_sumX = 0;
				m_sumY = 0;
				m_cAvg = 0;
			}
			m_fPrevTouch = true;
			if (++m_cAvg <= m_discardCnt)
				return false;

			m_sumX += ReadValue(RTP_ReadX);
			m_sumY += ReadValue(RTP_ReadY);

			if (m_cAvg < m_sampleCnt)
				return false;

			m_rawX = m_sumX >> m_avgShift;
			m_rawY = m_sumY >> m_avgShift;

			ProcessRaw(m_rawX, m_rawY);
			IsTouched(true);
			m_sumX = 0;
			m_sumY = 0;
			m_cAvg = 0;
		}
		else
		{
			// Not touching
			if (m_fPrevTouch)
				m_cAvg = 0;	// Just ending contact

			m_fPrevTouch = false;
			if (++m_cAvg < m_sampleCnt)
				return false;

			IsTouched(false);
			m_cAvg = 0;
		}

		return true;
	}


protected:
	static uint ReadValue(byte bControl) NO_INLINE_ATTR
	{
		uint	val;

		Select();
		// Read twice to get stable values
		SpiByte(bControl);
		SpiByte();
		SpiByte(bControl);
		val = SpiByte() << 5;
		val |= SpiByte() >> 3;
		Deselect();
		return val;
	}

protected:
	Timer	m_tmr;
	ushort	m_sumX;
	ushort	m_sumY;
	ushort	m_rawX;
	ushort	m_rawY;
	ushort	m_rawZ;

	ushort	m_minZtouch;
	ushort	m_scanTicks;
	byte	m_avgShift;
	byte	m_sampleCnt;
	byte	m_discardCnt;
	byte	m_cAvg;
	bool	m_fPrevTouch;
};

extern Xtp2046	Touch;
