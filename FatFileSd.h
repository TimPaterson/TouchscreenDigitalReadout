//****************************************************************************
// FatFileSd.h
//
// Created 11/11/2020 4:34:45 PM by Tim
//
//****************************************************************************

#pragma once

#include <FatFile/FatSysWait.h>
#include <FatFile/SdCard/SdCard.h>
#include "Spi.h"


//****************************************************************************
// This is generic glue for using a standard SERCOM SPI with a FAT SD card

#define	SD_CARD_SLOW_BAUDRATE	400000
#define	SD_CARD_FAST_BAUDRATE	(F_CPU / 4)

// Declare SPI port
//
// Read the SERCOM number as the last character of the name string (SERCOMn)
#define DECLARE_SPI_FAT(usart, ...) SpiFat<#usart[6] - '0', __VA_ARGS__>

template <int iUsart, uint uSsPin, uint uSsPort = 0>
class SpiFat : public Spi<iUsart, uSsPin, uSsPort, 0xFF>
{
public:
	typedef Spi<iUsart, uSsPin, uSsPort, 0xFF>	Base;

	static void SpiInit(SpiInPad padMiso, SpiOutPad padMosi, SpiMode modeSpi = SPIMODE_0)
		{ Base::SpiInit(padMiso, padMosi, modeSpi); }
	static byte SpiRead()			{ return Base::SpiByte(); }
	static void SpiWrite(byte b)	{ Base::SpiByte(b); }
	static void SetClockSlow()		
		{ Base::SetBaudRateConstEnabled(SD_CARD_SLOW_BAUDRATE); }
	static void SetClockFast()		
		{ Base::SetBaudRateConstEnabled(SD_CARD_FAST_BAUDRATE); }
};

//****************************************************************************
// This is specific to the touch screen project

class SpiBase : public DECLARE_SPI_FAT(SERCOM2, SdCs_PIN)
{
public:
	static bool SdCardPresent()		{ return true; }
};

typedef SdCard<SpiBase> FatSd;
