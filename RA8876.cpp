//****************************************************************************
// Class RA8876
// RA8876.cpp
//
// Created 10/11/2020 4:57:27 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "RA8876.h"


using namespace RA8876const;

//*********************************************************************
// Initialization values for East Rising ER-TFTM101-1

// Hardware settings
static constexpr long OscFreq = 10000000;	// Crystal clock
static constexpr long CoreFreq = 100000000;	// RA8876 core clock - CCLK
static constexpr long SdramFreq = 100000000;	// SDRAM clock - MCLK
static constexpr int SdramRefresh = 64;		// SDRAM refresh interval; ms
static constexpr int SdramRowSize = 4096;	// SDRAM row size (for refresh)
static constexpr int SdramRefInterval = SdramFreq / 1000 * SdramRefresh / SdramRowSize - 2;

// LCD parameters
static constexpr long LcdScanFreq = 50000000;	// LCD scan clock - SCLK
static constexpr int LcdWidthPx = 1024;
static constexpr int LcdHeightPx = 600;
// HSync
static constexpr int LcdHsyncWidthPx = 70;
static constexpr int LcdHsyncFrontPorchPx = 160;
static constexpr int LcdHsyncBackPorchPx = 160;
// VSync
static constexpr int LcdVsyncWidthLn = 10;
static constexpr int LcdVsyncFrontPorchLn = 12;
static constexpr int LcdVsyncBackPorchLn = 23;

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
	DPCR_Init = DPCR_OutputSequenceRGB | DPCR_VertScanTopToBottom | DPCR_PclkEdgeFalling,
	PCSR_Init = PCSR_VsyncIdleHigh | PCSR_HsyncIdleHigh | PCSR_DataIdleLow | 
		PCSR_ClockIdleLow | PCSR_DataEnableIdleLow | PCSR_DataEnableActiveHigh | 
		PCSR_VsyncActiveHigh | PCSR_HsyncActiveHigh,
	HDWR_Init = LcdWidthPx / 8 - 1,
	HDWFTR_Init = LcdWidthPx % 8,
	HNDR_Init = LcdHsyncBackPorchPx / 8 - 1,
	HNDFTR_Init = LcdHsyncBackPorchPx % 8,
	HSTR_Init = LcdHsyncFrontPorchPx / 8 - 1,
	HPWR_Init = (LcdHsyncWidthPx - 1) / 8,
	VDHR0_Init = (LcdHeightPx - 1) & 0xFF,
	VDHR1_Init = (LcdHeightPx - 1) >> 8,
	VNDR0_Init = (LcdVsyncBackPorchLn - 1) & 0xFF,
	VNDR1_Init = (LcdVsyncBackPorchLn - 1) >> 8,
	VSTR_Init = LcdVsyncFrontPorchLn - 1,
	VPWR_Init = LcdVsyncWidthLn - 1,
};

//*********************************************************************
// Helpers

inline INLINE_ATTR void ToggleEnable()
{
	SetLcdPin(LcdE);	// toggle E
	Timer::ShortDelay_clocks(1);
	ClearLcdPin(LcdE);
}

inline INLINE_ATTR void WriteAddr(uint addr)
{
	// Write address
	ClearLcdPin(LcdRW | LcdCs | LcdCD);
	PORTB->OUT.Lcd8 = addr;	// Address
	ToggleEnable();
}

inline INLINE_ATTR void WriteData(uint val)
{
	// Write data
	SetLcdPin(LcdCD);
	PORTB->OUT.Lcd8 = val;	// Data
	ToggleEnable();
	SetLcdPin(LcdCs);
}

inline INLINE_ATTR uint ReadData()
{
	uint	val;

	// Read data
	PORTB->DIR.Lcd16 = 0;	// Switch to inputs
	SetLcdPin(LcdCD | LcdRW);
	SetLcdPin(LcdE);	// toggle E
	Timer::ShortDelay_clocks(2);
	val = PORTB->IN.Lcd8;
	ClearLcdPin(LcdE);
	SetLcdPin(LcdCs);
	PORTB->DIR.Lcd16 = LcdData16;	// Switch back to outputs
	return val;
}

// When the RA8876 is first powered on, it runs on the crystal clock
// at only 10 MHz. Make sure it's ready by checking the WAIT line.
inline void WriteRegSlow(uint addr, uint val)
{
	while (GetLcdWait() == 0);
	WriteAddr(addr);
	Timer::ShortDelay_clocks(1);	// Pause for WAIT to assert
	while (GetLcdWait() == 0);
	WriteData(val);
}

static void WriteRegListSlow(const RegValue *pList, int iLen)
{
	do 
	{
		WriteRegSlow(pList->addr, pList->val);
		pList++;
	} while (--iLen > 0);
}

uint ReadRegSlow(uint addr)
{
	while (GetLcdWait() == 0);
	WriteAddr(addr);
	Timer::ShortDelay_clocks(1);	// Pause for WAIT to assert
	while (GetLcdWait() == 0);
	return ReadData();
}

//*********************************************************************
// Initialization data

const RegValue s_arPllInitList[] = {
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

const RegValue s_arSdramInitList[] = {
	// Initialize SDRAM
	SDRAR, SDRAR_Init,
	SDRMD, SDRMD_Init,
	SDR_REF_ITVL0, (byte)SdramRefInterval,
	SDR_REF_ITVL1, SdramRefInterval >> 8,
	SDRCR, SDRCR_Init,
};

const RegValue s_arInitList[] = {
	// Initialize Display
	MACR,	MACR_Init,
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
	// Turn display on once parameters are set
	DPCR,	DPCR_Init,
};

//*********************************************************************

void RA8876::Init()
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
	SetMainImage(0, LcdWidthPx);
	SetActiveWindowSize(LcdWidthPx, LcdHeightPx);
}

uint RA8876::GetStatus()
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

void RA8876::WriteReg(uint addr, uint val)
{
	WriteAddr(addr);
	WriteData(val);
}

uint RA8876::ReadReg(uint addr)
{
	WriteAddr(addr);
	return ReadData();
}

//*********************************************************************
// Function-specific handlers

void RA8876::TestPattern()
{
	WriteReg(DPCR, DPCR_Init | DPCR_DisplayOn | DPCR_DisplayTestBar);
}

void RA8876::DisplayOn()
{
	WriteReg(DPCR, DPCR_Init | DPCR_DisplayOn);
}

void RA8876::DisplayOff()
{
	WriteReg(DPCR, DPCR_Init | DPCR_DisplayOff);
}
