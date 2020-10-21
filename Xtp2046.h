//****************************************************************************
// Class Xtp2046
// Xtp2046.h
//
// Created 10/13/2020 4:35:49 PM by Tim
//
//****************************************************************************

#pragma once

#include "Spi.h"


class Xtp2046 : public DECLARE_SPI(SERCOM1, RtpCs_PIN, 0)
{
	// Types
	enum ControlByte
	{
		// Analog input selections
		// These are for differential mode
		ADDR_Ypos_Val = 1,
		ADDR_Z1_Val = 3,
		ADDR_Z2_Val = 4,
		ADDR_Xpos_Val = 5,

		ADDR_pos = 4,	// bit position in control byte

		ADDR_Ypos =		(ADDR_Ypos_Val << ADDR_pos),
		ADDR_Z1 =		(ADDR_Z1_Val << ADDR_pos),
		ADDR_Z2 =		(ADDR_Z2_Val << ADDR_pos),
		ADDR_Xpos =		(ADDR_Xpos_Val << ADDR_pos),

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
	};

public:
	static uint ReadX()
	{
		return ReadValue(RTP_Start | ADDR_Xpos | MODE_12Bit | REF_Dif | PWR_On);
	}

	static uint ReadY()
	{
		return ReadValue(RTP_Start | ADDR_Ypos | MODE_12Bit | REF_Dif | PWR_On);
	}

protected:
	static uint ReadValue(byte bControl) NO_INLINE_ATTR
	{
		uint	val;

		Select();
		SpiByte(bControl);
		val = SpiByte() << 4;
		val |= SpiByte() >> 4;
		Deselect();
		return val;
	}
};

extern Xtp2046	Touch;
