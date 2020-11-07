//****************************************************************************
// Class Xtp2046
// Xtp2046.h
//
// Created 10/13/2020 4:35:49 PM by Tim
//
//****************************************************************************

#pragma once

#include "Spi.h"
#include "ResTouch.h"


class Xtp2046 : public ResTouch, public DECLARE_SPI(SERCOM1, RtpCs_PIN, 0)
{
public:
	static constexpr int BaudRate = 1000000;
	static constexpr int AverageShift = 4;
	static constexpr int AverageCount = 1 << AverageShift;
	static constexpr int ScanRate = 100 * AverageCount;

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
		RTP_ReadX = RTP_Start | MODE_12Bit | REF_Dif | PWR_On | ADDR_X,
		RTP_ReadY = RTP_Start | MODE_12Bit | REF_Dif | PWR_On | ADDR_Y,
		RTP_ReadZ1 = RTP_Start | MODE_12Bit | REF_Dif | PWR_On | ADDR_Z1,
		RTP_ReadZ2 = RTP_Start | MODE_12Bit | REF_Dif | PWR_On | ADDR_Z2,
	};


public:
	void Init(SpiInPad padMiso, SpiOutPad padMosi)
	{
		Spi::Init(padMiso, padMosi, SPIMODE_0);
		Spi::SetBaudRateConst(BaudRate);

		// Set scaling values
		InitScale(&scaleX, &scaleY);

		m_tmr.Start();
	}

	bool Process() NO_INLINE_ATTR
	{
		if (!m_tmr.CheckInterval_rate(ScanRate))
			return false;

		if (ReadValue(RTP_ReadZ1) >= m_minZtouch)
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
			m_sumX += ReadValue(RTP_ReadX);
			m_sumY += ReadValue(RTP_ReadY);
			if (++m_cAvg < AverageCount)
				return false;

			ProcessRaw(m_sumX >> AverageShift, m_sumY >> AverageShift);
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
			if (++m_cAvg < AverageCount)
				return false;

			IsTouched(false);
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
	ushort	m_minZtouch = 200;
	byte	m_cAvg;
	bool	m_fPrevTouch;

protected:
	// Default scaling values
	inline static const TouchScreenScale scaleX =
	{
		200, 18000, 1023, true
	};

	inline static const TouchScreenScale scaleY =
	{
		300, 11000, 599, true
	};
};

extern Xtp2046	Touch;
