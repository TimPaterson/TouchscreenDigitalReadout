//****************************************************************************
// Spi.h
//
// Created 6/18/2019 1:37:43 PM by Tim
//
//****************************************************************************

#pragma once


// Declare SPI port
//
// Read the SERCOM number as the last character of the name string (SERCOMn)
#define DECLARE_SPI(usart, pin, port) Spi<#usart[6] - '0', pin, port>

#define pSpi(i)	((SercomSpi *)((byte *)SERCOM0 + ((byte *)SERCOM1 - (byte *)SERCOM0) * i))

//****************************************************************************

enum SpiInPad
{
	SPIMISOPAD_Pad0,
	SPIMISOPAD_Pad1,
	SPIMISOPAD_Pad2,
	SPIMISOPAD_Pad3
};

enum SpiOutPad
{
	SPIOUTPAD_Pad0_MOSI_Pad1_SCK,
	SPIOUTPAD_Pad2_MOSI_Pad3_SCK,
	SPIOUTPAD_Pad3_MOSI_Pad1_SCK,
	SPIOUTPAD_Pad0_MOSI_Pad3_SCK
};

enum SpiMode
{
	SPIMODE_0,	// CPOL = 0, CPHA = 0
	SPIMODE_1,	// CPOL = 0, CPHA = 1
	SPIMODE_2,	// CPOL = 1, CPHA = 0
	SPIMODE_3,	// CPOL = 1, CPHA = 1
};

//****************************************************************************

template <int iUsart, uint uSsPin, uint uSsPort>
class Spi
{
	// Types
	template <typename T>
	struct PortIoGroup
	{
		volatile T	DIR;
		volatile T	DIRCLR;
		volatile T	DIRSET;
		volatile T	DIRTGL;
		volatile T	OUT;
		volatile T	OUTCLR;
		volatile T	OUTSET;
		volatile T	OUTTGL;
		volatile T	IN;
	};

	typedef PortIoGroup<LONG_BYTES>	PortSs;
	#define SsPort	((PortSs *)&PORT_IOBUS->Group[uSsPort])

public:
	static void Init(SpiInPad padMiso, SpiOutPad padMosi, SpiMode modeSpi)
	{
		SERCOM_SPI_CTRLA_Type	spiCtrlA;

#if	defined(GCLK_PCHCTRL_GEN_GCLK0)
		// Enable clock
		MCLK->APBCMASK.reg |= 1 << (MCLK_APBCMASK_SERCOM0_Pos + iUsart);

		// Clock it with GCLK0
		GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + iUsart].reg = GCLK_PCHCTRL_GEN_GCLK0 |
			GCLK_PCHCTRL_CHEN;
#else
		// Enable clock
		PM->APBCMASK.reg |= 1 << (PM_APBCMASK_SERCOM0_Pos + iUsart);

		// Clock it with GCLK0
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
			(GCLK_CLKCTRL_ID_SERCOM0_CORE + iUsart);
#endif

		// standard 8-bit, MSB first
		spiCtrlA.reg = 0;
		spiCtrlA.bit.MODE = 3;		// SPI master mode
		spiCtrlA.bit.DOPO = padMosi;
		spiCtrlA.bit.DIPO = padMiso;
		spiCtrlA.bit.CPHA = modeSpi & 1;
		spiCtrlA.bit.CPOL = modeSpi & 2;
		spiCtrlA.bit.IBON = 1;
		pSpi(iUsart)->CTRLA.reg = spiCtrlA.reg;
		pSpi(iUsart)->CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
	}

	static void Enable()
	{
		pSpi(iUsart)->CTRLA.bit.ENABLE = 1;
	}

	static void Disable()
	{
		pSpi(iUsart)->CTRLA.bit.ENABLE = 0;
	}

#ifdef F_CPU
	static void SetBaudRateConst(uint32_t rate)
	{
		SetBaudRateConst(rate, F_CPU);
	}
#endif

	static void SetBaudRateConst(uint32_t rate, uint32_t clock)
	{
		pSpi(iUsart)->BAUD.reg = DIV_UINT_RND(clock, rate * 2) - 1;
	}

	static void Select(bool fSelect)	{ if (fSelect) Select(); else Deselect(); }

	static byte SpiByte(byte b = 0) NO_INLINE_ATTR
	{
		WriteByte(b);
		while (!IsByteReady());
		return ReadByte();
	}

	static void Select()	{ ClearPins(uSsPin, uSsPort); }
	static void Deselect()	{ SetPins(uSsPin, uSsPort); }

protected:
	static byte ReadByte()
	{
		return pSpi(iUsart)->DATA.reg;
	}

	static void WriteByte(byte b = 0)
	{
		pSpi(iUsart)->DATA.reg = b;
	}

	static bool IsByteReady()
	{
		return pSpi(iUsart)->INTFLAG.bit.RXC;
	}

	static bool CanSendByte()
	{
		return pSpi(iUsart)->INTFLAG.bit.DRE;
	}

	static bool IsRxOverflow()
	{
		return pSpi(iUsart)->INTFLAG.bit.ERROR;
	}

	static void ClearOverflow()
	{
		pSpi(iUsart)->INTFLAG.reg = SERCOM_SPI_INTFLAG_ERROR;
	}
};
