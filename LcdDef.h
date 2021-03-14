//****************************************************************************
// LcdDef.h
//
// Created 10/24/2020 12:18:34 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876const.h"


using namespace RA8876const;

// Define this symbol only if the RA8876 has a 16-bit MCU interface
#define RA8876_16BIT_BUS


//*************************************************************************
// This class is required as a base for the generic RA8876 class. It
// defines the hardware interface and initialization. Any member declared 
// protected or public is required for the derived classes.
//

class RA8876_Base
{
	//*********************************************************************
	// Types

	struct RegValue
	{
		byte	addr;
		byte	val;
	};

	//*********************************************************************
	// Initialization values for East Rising ER-TFTM101-1
	//*********************************************************************
public:
	static constexpr int ScreenWidth = 1024;
	static constexpr int ScreenHeight = 600;

protected:
	// Hardware settings
	static constexpr long CoreFreq = 100'000'000;	// RA8876 core clock - CCLK
private:
	static constexpr long OscFreq = 10'000'000;		// Crystal clock
	static constexpr long SdramFreq = 100'000'000;	// SDRAM clock - MCLK
	static constexpr int SdramRefresh = 64;			// SDRAM refresh interval; ms
	static constexpr int SdramRowSize = 4096;		// SDRAM row size (for refresh)
	static constexpr int SdramRefInterval = SdramFreq / 1000 * SdramRefresh / SdramRowSize - 2;

	// LCD parameters
	static constexpr long LcdScanFreq = 50'000'000;	// LCD scan clock - SCLK
	// HSync
	static constexpr int LcdHsyncWidthPx = 70;
	static constexpr int LcdHsyncFrontPorchPx = 160;
	static constexpr int LcdHsyncBackPorchPx = 160;
	// VSync
	static constexpr int LcdVsyncWidthLn = 10;
	static constexpr int LcdVsyncFrontPorchLn = 12;
	static constexpr int LcdVsyncBackPorchLn = 23;

	// Serial flash/ROM SPI settings
	// Unit 0: character generator ROM
	static constexpr long MaxSpiClock0 = 30'000'000;	// when using Fast Read

	// Unit 1: serial flash
	static constexpr long MaxSpiClock1 = 30'000'000;

	enum RA8876_InitValues
	{
		// Chip Configuration Registers
		CCR_Init = CCR_BusWidth16 | CCR_SpiEnable | CCR_I2Cdisable | CCR_LcdWidth24 |
			CCR_KeyScanDisable | CCR_WaitMaskOff | CCR_PllReconfigure,

		// PLL Initialization values
		// VCO has range 100 - 600 MHz. Dividing output by 4 will give
		// range 25 - 150 MHz, which covers full MCLK and CCLK range.
		// Using SCLK extra divider of 2 makes it 12.5 - 75 MHz.
		MPLLC1_Init = PLL_Prescale1 | PLL_Postscale4,
		MPLLC2_Init = SdramFreq * 4 / OscFreq - 1,

		CPLLC1_Init = PLL_Prescale1 | PLL_Postscale4,
		CPLLC2_Init = CoreFreq * 4 / OscFreq - 1,

		SPLLC1_Init = PLL_Prescale1 | PLL_Postscale4 | SPLL_ExtraDiv2,
		SPLLC2_Init = LcdScanFreq * 4 * 2 / OscFreq - 1,

		// SDRAM Initialization values
		SDRAR_Init = SDRAR_ColumnBits9 | SDRAR_RowBits12 | SDRAR_Banks4,
		SDRMD_Init = SDRMD_CasClocks3 | SDRMD_DriverFull | SDRMD_ArrayFull,
		SDRCR_Init = SDRCR_InitDone | SDRCR_ClearWarning | SDRCR_BusWidth16 | SDRCR_BurstSize256,

		// Display Initialization
		MACR_Init = MACR_LeftRightTopBottom | MACR_MaskNone,
		ICR_Init = ICR_MemPortSdram | ICR_IrqLevelHigh | ICR_GraphicsMode,
		DPCR_Init = DPCR_OutputSequenceRGB | DPCR_VertScanTopToBottom | DPCR_PclkEdgeFalling,
		PCSR_Init = PCSR_VsyncIdleHigh | PCSR_HsyncIdleHigh | PCSR_DataIdleLow |
			PCSR_ClockIdleLow | PCSR_DataEnableIdleLow | PCSR_DataEnableActiveHigh |
			PCSR_VsyncActiveHigh | PCSR_HsyncActiveHigh,
		HDWR_Init = ScreenWidth / 8 - 1,
		HDWFTR_Init = ScreenWidth % 8,
		HNDR_Init = LcdHsyncBackPorchPx / 8 - 1,
		HNDFTR_Init = LcdHsyncBackPorchPx % 8,
		HSTR_Init = LcdHsyncFrontPorchPx / 8 - 1,
		HPWR_Init = (LcdHsyncWidthPx - 1) / 8,
		VDHR0_Init = (ScreenHeight - 1) & 0xFF,
		VDHR1_Init = (ScreenHeight - 1) >> 8,
		VNDR0_Init = (LcdVsyncBackPorchLn - 1) & 0xFF,
		VNDR1_Init = (LcdVsyncBackPorchLn - 1) >> 8,
		VSTR_Init = LcdVsyncFrontPorchLn - 1,
		VPWR_Init = LcdVsyncWidthLn - 1,
		MPWCTR_Init = MPWCTR_SyncEnable,

		// SPI Initialization
		// SPI Serial character ROM at CS0

		// SPI Serial flash at CS1
		SPIMCR_Init = SPIMCR_SpiMode0 | SPIMCR_EmtIrqMasked |
			SPIMCR_OvfIrqMasked | SPIMCR_SlaveSelectCs1 | SPIMCR_IrqEnable,
	};

	//*********************************************************************
	// Serial memory setup needed in class RA8876
	//*********************************************************************
protected:
	static constexpr long SpiClock0 = std::min(MaxSpiClock0, CoreFreq / 2);
	static constexpr int SFL_CTRL_Init0 = SFL_CTRL_Select0 | SFL_CTRL_ReadCommand0B | SFL_CTRL_AddrBits24;

	static constexpr long SpiClock1 = std::min(MaxSpiClock1, CoreFreq / 2);
	static constexpr int SFL_CTRL_Init1 = SFL_CTRL_Select1 | SFL_CTRL_ReadCommand3B | SFL_CTRL_AddrBits24;

	//*********************************************************************
	// Hardware-specific I/O needed in class RA8876
	//*********************************************************************
protected:
	static uint GetStatus()
	{
		uint	status;

		PORTB->DIR.Lcd16 = 0;	// Switch to inputs
		SetLcdPin(LcdRW);
		ClearLcdPin(LcdCs | LcdCD);
		SetLcdPin(LcdE);	// toggle E
		Timer::ShortDelay_clocks(2);
		status = PORTB->IN.Lcd8;
		ClearLcdPin(LcdE);
		SetLcdPin(LcdCs);
		PORTB->DIR.Lcd16 = LcdData16;	// Switch back to outputs
		return status;
	}

	static void WriteAddrInline(uint addr) INLINE_ATTR
	{
		// Write address
		ClearLcdPin(LcdRW | LcdCs | LcdCD);
		PORTB->OUT.Lcd8 = addr;	// Address
		ToggleEnable();
		SetLcdPin(LcdCD | LcdCs);
	}

	static void WriteDataInline(uint val) INLINE_ATTR
	{
		// Write data
		SetLcdPin(LcdCD);
		ClearLcdPin(LcdRW | LcdCs);
		PORTB->OUT.Lcd8 = val;	// Data
		ToggleEnable();
		SetLcdPin(LcdCs);
	}

	static uint ReadDataInline() INLINE_ATTR
	{
		uint	val;

		// Read data
		PORTB->DIR.Lcd16 = 0;	// Switch to inputs
		ClearLcdPin(LcdCs);
		SetLcdPin(LcdCD | LcdRW);
		SetLcdPin(LcdE);	// toggle E
		Timer::ShortDelay_clocks(2);
		val = PORTB->IN.Lcd8;
		ClearLcdPin(LcdE);
		SetLcdPin(LcdCs);
		PORTB->DIR.Lcd16 = LcdData16;	// Switch back to outputs
		return val;
	}

#ifdef RA8876_16BIT_BUS

	static void WriteData16Inline(uint val) INLINE_ATTR
	{
		// Write data
		SetLcdPin(LcdCD);
		ClearLcdPin(LcdRW | LcdCs);
		PORTB->OUT.Lcd16 = val;	// Data
		ToggleEnable();
		SetLcdPin(LcdCs);
	}

	static uint ReadData16Inline() INLINE_ATTR
	{
		uint	val;

		// Read data
		PORTB->DIR.Lcd16 = 0;	// Switch to inputs
		ClearLcdPin(LcdCs);
		SetLcdPin(LcdCD | LcdRW);
		SetLcdPin(LcdE);	// toggle E
		Timer::ShortDelay_clocks(2);
		val = PORTB->IN.Lcd16;
		ClearLcdPin(LcdE);
		SetLcdPin(LcdCs);
		PORTB->DIR.Lcd16 = LcdData16;	// Switch back to outputs
		return val;
	}

#endif	// RA8876_16BIT_BUS

	//*********************************************************************

public:
	static void WriteReg(uint addr, uint val)
	{
		WriteAddrInline(addr);
		WriteDataInline(val);
	}

	static uint ReadReg(uint addr)
	{
		WriteAddrInline(addr);
		return ReadDataInline();
	}

	//*********************************************************************
	// Hardware-specific RA8876 I/O
	//*********************************************************************
private:
	static void ToggleEnable() INLINE_ATTR
	{
		SetLcdPin(LcdE);	// toggle E
		Timer::ShortDelay_clocks(1);
		ClearLcdPin(LcdE);
	}

	// When the RA8876 is first powered on, it runs on the crystal clock
	// at only 10 MHz. Make sure it's ready by checking the WAIT line.
	static void WriteRegSlow(uint addr, uint val)
	{
		while (GetLcdWait() == 0);
		WriteAddrInline(addr);
		Timer::ShortDelay_clocks(1);	// Pause for WAIT to assert
		while (GetLcdWait() == 0);
		WriteDataInline(val);
	}

	static void WriteRegListSlow(const RegValue *pList, int iLen)
	{
		do
		{
			WriteRegSlow(pList->addr, pList->val);
			pList++;
		} while (--iLen > 0);
	}

	static uint ReadRegSlow(uint addr)
	{
		while (GetLcdWait() == 0);
		WriteAddrInline(addr);
		Timer::ShortDelay_clocks(1);	// Pause for WAIT to assert
		while (GetLcdWait() == 0);
		return ReadDataInline();
	}

	static void WriteRegList(const RegValue *pList, int iLen)
	{
		do
		{
			WriteReg(pList->addr, pList->val);
			pList++;
		} while (--iLen > 0);
	}

	//*********************************************************************

public:
	static void Init()
	{
		// Make sure we're ready to accept commands
		while (GetStatus() & STATUS_InhibitOperation);

		// Software reset
		WriteRegSlow(SRR, SRR_Reset);
		// Wait for reset to end
		while (GetStatus() & STATUS_InhibitOperation);

		// Initialize PLL
		WriteRegListSlow(s_arPllInitList, _countof(s_arPllInitList));
		// Wait for PLL to stabilize
		while((ReadRegSlow(CCR) & CCR_PllReconfigure) == 0);

		// Initialize SDRAM
		WriteRegList(s_arSdramInitList, _countof(s_arSdramInitList));
		// Wait for SDRAM to be ready
		while ((GetStatus() & STATUS_SdramReady) == 0);

		// Initialize LCD
		WriteRegList(s_arInitList, _countof(s_arInitList));
	}

	//*********************************************************************
	// Function-specific handlers

	static void TestPattern()
	{
		WriteReg(DPCR, DPCR_Init | DPCR_DisplayOn | DPCR_DisplayTestBar);
	}

	static void DisplayOn()
	{
		WriteReg(DPCR, DPCR_Init | DPCR_DisplayOn);
	}

	static void DisplayOff()
	{
		WriteReg(DPCR, DPCR_Init | DPCR_DisplayOff);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const RegValue s_arPllInitList[] = {
		// Initialize PLL
		SPPLLC1, SPLLC1_Init,
		SPPLLC2, SPLLC2_Init,
		MPPLLC1, MPLLC1_Init,
		MPPLLC2, MPLLC2_Init,
		CPPLLC1, CPLLC1_Init,
		CPPLLC2, CPLLC2_Init,
		// Also write to CCR to reconfigure PLL
		CCR, CCR_Init & ~CCR_PllReconfigure,
		CCR, CCR_Init,
	};

	inline static const RegValue s_arSdramInitList[] = {
		// Initialize SDRAM
		SDRAR, SDRAR_Init,
		SDRMD, SDRMD_Init,
		SDR_REF_ITVL0, (byte)SdramRefInterval,
		SDR_REF_ITVL1, SdramRefInterval >> 8,
		SDRCR, SDRCR_Init,
	};

	inline static const RegValue s_arInitList[] = {
		// Initialize Display
		MACR,	MACR_Init,
		ICR,	ICR_Init,
		PCSR,	PCSR_Init,
		HDWR,	HDWR_Init,
		HDWFTR,	HDWFTR_Init,
		HNDR,	HNDR_Init,
		HNDFTR,	HNDFTR_Init,
		HSTR,	HSTR_Init,
		HPWR,	HPWR_Init,
		VDHR0,	VDHR0_Init,
		VDHR1,	VDHR1_Init,
		VNDR0,	VNDR0_Init,
		VNDR1,	VNDR1_Init,
		VSTR,	VSTR_Init,
		VPWR,	VPWR_Init,
		MPWCTR,	MPWCTR_Init,
		GTFNT_SEL, GTFNT_SEL_GT30L32S4W,
		DPCR,	DPCR_Init,
	};
};
