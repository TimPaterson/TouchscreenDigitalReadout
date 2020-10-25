//****************************************************************************
// Class RA8876
// RA8876.cpp
//
// Created 10/11/2020 4:57:27 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "LcdDef.h"
#include "RA8876.h"


using namespace RA8876const;

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

	// SPI Initialization
	// SPI Serial character ROM at CS0

	// SPI Serial flash at CS1
	SPIMCR_Init = SPIMCR_SpiMode0 | SPIMCR_EmtIrqMasked | 
		SPIMCR_OvfIrqMasked | SPIMCR_SlaveSelectCs1 | SPIMCR_IrqEnable,
};

//*********************************************************************
// Helpers

inline INLINE_ATTR void ToggleEnable()
{
	SetLcdPin(LcdE);	// toggle E
	Timer::ShortDelay_clocks(1);
	ClearLcdPin(LcdE);
}

// When the RA8876 is first powered on, it runs on the crystal clock
// at only 10 MHz. Make sure it's ready by checking the WAIT line.
inline void WriteRegSlow(uint addr, uint val)
{
	while (GetLcdWait() == 0);
	RA8876::WriteAddr(addr);
	Timer::ShortDelay_clocks(1);	// Pause for WAIT to assert
	while (GetLcdWait() == 0);
	RA8876::WriteData(val);
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
	RA8876::WriteAddr(addr);
	Timer::ShortDelay_clocks(1);	// Pause for WAIT to assert
	while (GetLcdWait() == 0);
	return RA8876::ReadData();
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
	GTFNT_SEL, GTFNT_SEL_GT30L32S4W,
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

void RA8876::WriteAddr(uint addr)
{
	// Write address
	ClearLcdPin(LcdRW | LcdCs | LcdCD);
	PORTB->OUT.Lcd8 = addr;	// Address
	ToggleEnable();
	SetLcdPin(LcdCD | LcdCs);
}

void RA8876::WriteData(uint val)
{
	// Write data
	SetLcdPin(LcdCD);
	ClearLcdPin(LcdRW | LcdCs);
	PORTB->OUT.Lcd8 = val;	// Data
	ToggleEnable();
	SetLcdPin(LcdCs);
}

uint RA8876::ReadData()
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
