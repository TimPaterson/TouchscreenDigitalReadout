//****************************************************************************
// RA8876const.h
//
// Created 10/17/2020 11:27:38 AM by Tim
//
//****************************************************************************

#pragma once


namespace RA8876const
{
	//*********************************************************************
	// Chip hardware constants

	enum Registers
	{
		SRR,		// Software Reset 
		CCR,		// Chip Configuration 
		MACR,		// Memory Access Control 
		ICR,		// Input Control 
		MRWDP,		// Memory Data Read/Write Port

		// PLL
		SPPLLC1,	// SCLK (panel scan clock) PLL Control 1
		SPPLLC2,	// SCLK PLL Control 2
		MPPLLC1,	// MCLK (SDRAM clock) PLL Control 1
		MPPLLC2,	// MCLK PLL Control 2
		CPPLLC1,	// CCLK (core clock) PLL Control 1
		CPPLLC2,	// CCLK PLL Control 2

		// Interrupt
		INTEN,		// Interrupt Enable 
		INTF,		// Interrupt Event Flag 
		MINTFR,		// Mask Interrupt Flag 

		// GPIO
		PUENR,		// Pull-up Control 
		PSFSR,		// PIO Select/Function Select 

		// LCD
		MPWCTR,		// Main/PIP Window Control 
		PIPCDEP,	// PIP Window Color Depth
		DPCR,		// Display Configuration
		PCSR,		// Panel Scan Clock and Data Setting
		HDWR,		// Horizontal Display Width
		HDWFTR,		// Horizontal Display Width Fine Tune
		HNDR,		// Horizontal Non-Display Period
		HNDFTR,
		HSTR,
		HPWR,
		VDHR0,
		VDHR1,
		VNDR0,
		VNDR1,
		VSTR,
		VPWR,
		MISA0,
		MISA1,
		MISA2,
		MISA3,
		MIW0,
		MIW1,
		MWULX0,
		MWULX1,
		MWULY0,
		MWULY1,
		PWDULX0,
		PWDULX1,
		PWDULY0,
		PWDULY1,
		PISA0,
		PISA1,
		PISA2,
		PISA3,
		PIW0,
		PIW1,
		PWIULX0,
		PWIULX1,
		PWIULY0,
		PWIULY1,
		PWW0,
		PWW1,
		PWH0,
		PWH1,
		GTCCR,
		BTCR,
		CURHS,
		CURVS,
		GCHP0,
		GCHP1,
		GCVP0,
		GCVP1,
		GCC0,
		GCC1,

		// Geometric Engine
		CVSSA0 = 0x50,
		CVSSA1,
		CVSSA2,
		CVSSA3,
		CVS_IMWTH0,
		CVS_IMWTH1,
		AWUL_X0,
		AWUL_X1,
		AWUL_Y0,
		AWUL_Y1,
		AW_WTH0,
		AW_WTH1,
		AW_HT0,
		AW_HT1,
		AW_COLOR,
		CURH0,
		CURH1,
		CURV0,
		CURV1,
		F_CURX0,
		F_CURX1,
		F_CURY0,
		F_CURY1,
		DCR0,
		DLHSR0,
		DLHSR1,
		DLVSR0,
		DLVSR1,
		DLHER0,
		DLHER1,
		DLVER0,
		DLVER1,
		DTPH0,
		DTPH1,
		DTPV0,
		DTPV1,
		DCR1 = 0x76,
		ELL_A0,
		ELL_A1,
		ELL_B0,
		ELL_B1,
		DEHR0,
		DEHR1,
		DEVR0,
		DEVR1,

		// PWM
		PSCLR = 0x84,
		PMUXR,
		PCFGR,
		DZ_LENGTH,
		TCMPB0L,
		TCMPB0H,
		TCNTB0L,
		TCNTB0H,
		TCMPB1L,
		TCMPB1H,
		TCNTB1L,
		TCNTB1H,

		// BTE
		BTE_CTRL0,
		BTE_CTRL1,
		BTE_COLR,
		S0_STR0,
		S0_STR1,
		S0_STR2,
		SO_STR3,
		S0_WTH0,
		S0_WTH1,
		S0_X0,
		S0_X1,
		S0_Y0,
		S0_Y1,
		S1_STR0,
		S1_RED = S1_STR0,
		S1_STR1,
		S1_GREEN = S1_STR1,
		S1_STR2,
		S1_BLUE = S1_STR2,
		S1_STR3,
		S1_WTH0,
		S1_WTH1,
		S1_X0,
		S1_X1,
		S1_Y0,
		S1_Y1,
		DT_STR0,
		DT_STR1,
		DT_STR2,
		DT_STR3,
		DT_WTH0,
		DT_WTH1,
		DT_X0,
		DT_X1,
		DT_Y0,
		DT_Y1,
		BTE_WTH0,
		BTE_WTH1,
		BTE_HIG0,
		BTE_HIG1,
		APB_CTRL,

		// SPI
		DMA_CTRL,
		SFL_CTRL,
		SPIDR,
		SPIMCR,
		SPIMSR,
		SPI_DIVSOR,
		DMA_SSTR0,
		DMA_SSTR1,
		DMA_SSTR2,
		DMA_SSTR3,
		DMA_DX0,
		DMA_DX1,
		DMA_DY0,
		DMA_DY1,
		DMAW_WTH0 = 0xC6,
		DMAW_WTH1,
		DMAW_HIGH0,
		DMAW_HIGH1,
		DMA_SWTH0,
		DMA_SWTH1,

		// Text Engine
		CCR0,
		CCR1,
		GTFNT_SEL,
		GTFNT_CR,
		FLDR,
		F2FSSR,
		FGCR,
		FGCG,
		FGCB,
		BGCR,
		BGCG,
		BGCB,
		CGRAM_STR0 = 0xDB,
		CGRAM_STR1,
		CGRAM_STR2,
		CGRAM_STR3,

		// Power
		PMU,

		// SDRAM Control
		SDRAR,
		SDRMD,
		SDR_REF_ITVL0,
		SDR_REF_ITVL1,
		SDRCR,

		// I2C
		I2CMCPR0,
		I2CMCPR1,
		I2CMTXR,
		IC2MRXR,
		I2CMCMDR,
		I2CMSTUR,

		// GPIO
		GPIOAD,
		GPIOA,
		GPIOB,
		GPIOCD,
		GPIOC,
		GPIODD,
		GPIOD,
		GPIOED,
		GPIOE,
		GPIOFD,
		GPIOF,

		// Key scan
		KSCR1,
		KSCR2,
		KSDR0,
		KSDR1,
		KSDR2,

	};

	//************************************************************************
	// Status Register

	enum STATUS_Bits
	{
		STATUS_Irq = 0x01,
		STATUS_InhibitOperation = 0x02,
		STATUS_SdramReady = 0x04,
		STATUS_CoreBusy = 0x08,
		STATUS_ReadFifoEmpty = 0x10,
		STATUS_ReadFifoFull = 0x20,
		STATUS_WriteFifoEmpty = 0x40,
		STATUS_WriteFifoFull = 0x80
	};

	//************************************************************************
	// Chip Configuration Registers

	// Software Reset Register
	enum SRR_Bits
	{
		SRR_Reset = 0x01,	// Write-only
		SRR_Warning = 0x01,	// Read-only mask
	};

	// Chip Configuration Register
	enum CCR_Bits
	{
		// Choose one from each group
		CCR_BusWidth8 = 0x00,
		CCR_BusWidth16 = 0x01,

		CCR_SpiDisable = 0x00,
		CCR_SpiEnable = 0x02,

		CCR_I2Cdisable = 0x00,
		CCR_I2Cenable = 0x04,

		CCR_LcdWidth16 = 0x10,
		CCR_LcdWidth18 = 0x08,
		CCR_LcdWidth24 = 0x00,

		CCR_KeyScanDisable = 0x00,
		CCR_KeyScanEnable = 0x20,

		CCR_WaitMaskOff = 0x00,
		CCR_WaitMaskOn = 0x40,

		CCR_PllReconfigure = 0x80
	};

	// Memory Access Control Register
	enum MACR_Bits
	{
		// Set read & write to same order
		MACR_LeftRightTopBottom = 0x00,
		MACR_RightLeftTopBottom = 0x12,	// Mirror
		MACR_TopBottomLeftRight = 0x24,	// Rotate right & mirror
		MACR_BottomTopLeftRight = 0x36,	// Rotate left

		MACR_MaskNone = 0x00,
		MACR_MaskHigh = 0x80,
		MACR_MaskHighEven = 0xC0,
	};

	// Input Control Register
	enum ICR_Bits
	{
		ICR_MemPort_Mask = 0x03,
		ICR_MemPortSdram = 0x00,
		ICR_MemPortGamma = 0x01,
		ICR_MemPortCursor = 0x02,
		ICR_MemPortPalette = 0x03,

		ICR_TextGraphicsMode_Mask = 0x04,
		ICR_GraphicsMode = 0x00,
		ICR_TextMode = 0x04,

		ICR_IrqLevelLow = 0x00,
		ICR_IrqLevelHigh = 0x80,
	};

	//************************************************************************
	// PLL Control

	// PLL control for SCLK, MCLK, and CCLK PLLs
	enum PLLC1_Bits
	{
		PLL_Prescale1 = 0x00,
		PLL_Prescale2 = 0x01,

		PLL_Postscale1 = 0x00,
		PLL_Postscale2 = 0x02,
		PLL_Postscale4 = 0x04,
		PLL_Postscale8 = 0x06,

		// These apply to SCLK PLL only
		SPLL_ExtraDiv1 = 0x00,
		SPLL_ExtraDiv2 = 0x10,
		SPLL_ExtraDiv4 = 0x20,
		SPLL_ExtraDiv8 = 0x30,
		SPLL_ExtraDiv16 = 0x08,
	};

	//************************************************************************
	// LCD Display Control Registers

	// Main/PIP Window Control Register
	enum MPWCTR_Bits
	{
		MPWCTR_SyncIdle = 0x01,
		MPWCTR_SyncEnable = 0x00,

		MPWCTR_MainImageColor_Mask = 0x0C,
		MPWCTR_MainImageColor_Shft = 2,
		MPWCTR_MainImageColor8 = 0x00,
		MPWCTR_MainImageColor16 = 0x04,
		MPWCTR_MainImageColor24 = 0x08,

		MPWCTR_ConfigurePip_Mask = 0x10,
		MPWCTR_ConfigurePip1 = 0x00,
		MPWCTR_ConfigurePip2 = 0x10,

		MPWCTR_Pip2_Mask = 0x40,
		MPWCTR_Pip2Disable = 0x00,
		MPWCTR_Pip2Enable = 0x40,

		MPWCTR_Pip1_Mask = 0x80,
		MPWCTR_Pip1Disable = 0x00,
		MPWCTR_Pip1Enable = 0x80,
	};

	// PIP Window Color Depth Setting
	enum PIPCDEP_Bits
	{
		PIPCDEP_Pip2Color_Mask = 0x03,
		PIPCDEP_Pip2Color_Shft = 0,
		PIPCDEP_Pip2Color8 = 0x00,
		PIPCDEP_Pip2Color16 = 0x01,
		PIPCDEP_Pip2Color24 = 0x02,

		PIPCDEP_Pip1Color_Mask = 0x0C,
		PIPCDEP_Pip1Color_Shft = 2,
		PIPCDEP_Pip1Color8 = 0x00,
		PIPCDEP_Pip1Color16 = 0x04,
		PIPCDEP_Pip1Color24 = 0x08,
	};

	// Display Configuration Register
	enum DPCR_Bits
	{
		// Choose one from each group
		DPCR_OutputSequenceRGB = 0x00,
		DPCR_OutputSequenceRBG = 0x01,
		DPCR_OutputSequenceGRB = 0x02,
		DPCR_OutputSequenceGBR = 0x03,
		DPCR_OutputSequenceBRG = 0x04,
		DPCR_OutputSequenceBGR = 0x05,
		DPCR_OutputSequenceGray = 0x06,
		DPCR_OutputSequenceIdle = 0x07,

		DPCR_VertScanTopToBottom = 0x00,
		DPCR_VertScanBottomToTop = 0x04,

		DPCR_DisplayTestBar = 0x20,

		DPCR_DisplayOff = 0x00,
		DPCR_DisplayOn = 0x40,

		DPCR_PclkEdgeRising = 0x00,
		DPCR_PclkEdgeFalling = 0x80,
	};

	// Panel scan Clock and Data Setting Register
	enum PCSR_Bits
	{
		// Choose one from each group
		PCSR_VsyncIdleLow = 0x00,
		PCSR_VsyncIdleHigh = 0x01,

		PCSR_HsyncIdleLow = 0x00,
		PCSR_HsyncIdleHigh = 0x02,

		PCSR_DataIdleLow = 0x00,
		PCSR_DataIdleHight = 0x04,

		PCSR_ClockIdleLow = 0x00,
		PCSR_ClockIdleHigh = 0x08,

		PCSR_DataEnableIdleLow  = 0x00,
		PCSR_DataEnableIdleHigh = 0x10,

		PCSR_DataEnableActiveLow = 0x20,
		PCSR_DataEnableActiveHigh = 0x00,

		PCSR_VsyncActiveLow = 0x00,
		PCSR_VsyncActiveHigh = 0x40,

		PCSR_HsyncActiveLow = 0x00,
		PCSR_HsyncActiveHigh = 0x80,
	};

	// Graphic / Text Cursor Control Register
	enum GTCCR_Bits
	{
		GTCCR_TextCursorBlink = 0x01,

		GTCCR_TextCursorEnable = 0x02,

		GTCCR_GraphicCursorSelect_Mask = 0x0C,
		GTCCR_GraphicCursorSelect1 = 0x00,
		GTCCR_GraphicCursorSelect2 = 0x04,
		GTCCR_GraphicCursorSelect3 = 0x08,
		GTCCR_GraphicCursorSelect4 = 0x0C,

		GTCCR_GraphicsCursorEnable = 0x10,

		GTCCR_GammaTableSelect_Mask = 0x60,
		GTCCR_GammaTableSelectBlue = 0x00,
		GTCCR_GammaTableSelectGreen = 0x20,
		GTCCR_GammaTableSelectRed = 0x40,

		GTCCR_GammaCorrectionEnable = 0x80,
	};

	//************************************************************************
	// Geometric Engine Control Registers

	// Color Depth of Canvas & Active Window
	enum AW_COLOR_Bits
	{
		AW_COLOR_CanvasColor_Mask = 0x03,
		AW_COLOR_CanvasColor8 = 0x00,
		AW_COLOR_CanvasColor16 = 0x01,
		AW_COLOR_CanvasColor24 = 0x02,
		AW_COLOR_DataWidth8 = 0x00,
		AW_COLOR_DataWidth16 = 0x01,

		AW_COLOR_AddrMode_Mask = 0x04,
		AW_COLOR_AddrModeXY = 0x00,
		AW_COLOR_AddrModeLinear = 0x04,
	};

	// Draw Line / Triangle Control Register 0
	enum DCR0_Bits
	{
		DCR0_DrawLine = 0x00,
		DCR0_DrawTriangle = 0x02,

		DCR0_FillOff = 0x00,
		DCR0_FillOn = 0x020,

		DCR0_DrawActive = 0x80,
	};

	// Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1
	enum DCR1_Bits
	{
		DCR1_DrawEllipse = 0x00,
		DCR1_DrawCurve = 0x10,
		DCR1_DrawRect = 0x20,
		DCR1_DrawRoundedRect = 0x30,

		DCR1_FillOff = 0x00,
		DCR1_FillOn = 0x040,

		DCR1_DrawActive = 0x80,
	};

	//************************************************************************
	// Block Transfer Engine (BTE) Control Registers

	enum BTE_CTRL0_Bits
	{
		BTE_CTRL0_Pattern8x8 = 0x00,
		BTE_CTRL0_Pattern16x16 = 0x01,

		BTE_CTRL0_Enable = 0x10,
	};

	enum BTE_CTRL1_Bits
	{
		BTE_CTRL1_OpcodeMpuWriteWithRop = 0x00,
		BTE_CTRL1_OpcodeMpuWriteChromaKey = 0x04,
		BTE_CTRL1_OpcodeMpuWriteExpandMono = 0x08,
		BTE_CTRL1_OpcodeMpuWriteExpandMonoTranparent = 0x09,
		BTE_CTRL1_OpcodeMemoryCopyWithRop = 0x02,
		BTE_CTRL1_OpcodeMemoryCopyChromaKey = 0x05,
		BTE_CTRL1_OpcodeMemoryCopyWithOpacity = 0x0B,
		BTE_CTRL1_OpcodeMemoryCopyExpandMono = 0x0E,
		BTE_CTRL1_OpcodeMemoryCopyExpandMonoTransparent  = 0x0F,
		BTE_CTRL1_OpcodePatternFillWithRop = 0x06,
		BTE_CTRL1_OpcodePatternFillChromaKey = 0x07,
		BTE_CTRL1_OpcodeSolidFill = 0x0C,

		BTE_CTRL1_RopBlack = 0x00,
		BTE_CTRL1_RopNotS0andNotS1 = 0x01,
		BTE_CTRL1_RopNotS0andS1 = 0x02,
		BTE_CTRL1_RopNotS0 = 0x03,
		BTE_CTRL1_RopS0andNotS1 = 0x04,
		BTE_CTRL1_RopNotS1 = 0x05,
		BTE_CTRL1_RopS0xorS1 = 0x06,
		BTE_CTRL1_RopNotS0orNotS1 = 0x07,
		BTE_CTRL1_RopS0andS1 = 0x08,
		BTE_CTRL1_RopNotS0xorS1 = 0x09,
		BTE_CTRL1_RopS1 = 0x0A,
		BTE_CTRL1_RopNotS0orS1 = 0x0B,
		BTE_CTRL1_RopS0 = 0x0C,
		BTE_CTRL1_RopS0orNotS1  = 0x0D,
		BTE_CTRL1_RopS0orS1 = 0x0E,
		BTE_CTRL1_RopWhite = 0x0f,

		BTE_CTRL1_BitStartShift = 4,
	};

	enum BTE_COLR_Bits
	{
		BTE_COLR_DestColor_Mask = 0x03,
		BTE_COLR_DestColor_Shft = 0,
		BTE_COLR_DestColor8 = 0x00,
		BTE_COLR_DestColor16 = 0x01,
		BTE_COLR_DestColor24 = 0x02,

		BTE_COLR_Src1Color_Mask = 0x1C,
		BTE_COLR_Src1Color_Shft = 2,
		BTE_COLR_Src1Color8 = 0x00,
		BTE_COLR_Src1Color16 = 0x04,
		BTE_COLR_Src1Color24 = 0x08,
		BTE_COLR_Src1ColorConst = 0x0C,
		BTE_COLR_Src1ColorAlpha8 = 0x10,
		BTE_COLR_Src1ColorAlpha16 = 0x14,

		BTE_COLR_Src0Color_Mask = 0x60,
		BTE_COLR_Src0Color_Shft = 5,
		BTE_COLR_Src0Color8 = 0x00,
		BTE_COLR_Src0Color16 = 0x20,
		BTE_COLR_Src0Color24 = 0x40,
	};

	//************************************************************************
	// Serial Flash & SPI Master Control Registers

	// Serial flash DMA Controller REG
	enum DMA_CTRL_Bits
	{
		DMA_CTRL_Start = 0x01,
	};

	// Serial Flash/ROM Controller Register
	enum SFL_CTRL_Bits
	{
		SFL_CTRL_ReadCommand03 = 0x00,
		SFL_CTRL_ReadCommand0B = 0x04,
		SFL_CTRL_ReadCommand1B = 0x08,
		SFL_CTRL_ReadCommand3B = 0x02,
		SFL_CTRL_ReadCommandBB = 0x03,

		SFL_CTRL_AddrBits24 = 0x00,
		SFL_CTRL_AddrBits32 = 0x20,

		SFL_CTRL_ModeFont = 0x00,
		SFL_CTRL_ModeDma = 0x40,

		SFL_CTRL_Select0 = 0x00,
		SFL_CTRL_Select1 = 0x80,
	};

	// SPI master Control Register
	enum SPIMCR_Bits
	{
		SPIMCR_SpiMode0 = 0x00,
		SPIMCR_SpiMode1 = 0x01,
		SPIMCR_SpiMode2 = 0x02,
		SPIMCR_SpiMode3 = 0x03,

		SPIMCR_EmtIrqUnmasked = 0x00,
		SPIMCR_EmtIrqMasked = 0x04,

		SPIMCR_OvfIrqUnmasked = 0x00,
		SPIMCR_OvfIrqMasked = 0x08,

		SPIMCR_SlaveSelectInactive = 0x00,
		SPIMCR_SlaveSelectActive = 0x10,

		SPIMCR_SlaveSelectCs0 = 0x00,
		SPIMCR_SlaveSelectCs1 = 0x20,

		SPIMCR_IrqEnable = 0x40,
		SPIMCR_IrqDisable = 0x00,
	};

	// SPI master Status Register
	enum SPIMSR_Bits
	{
		SPIMSR_Idle = 0x04,
		SPIMSR_Overflow = 0x08,
		SPIMSR_RxFifoFull = 0x10,
		SPIMSR_RxFifoEmpty = 0x20,
		SPIMSR_TxFifoFull = 0x40,
		SPIMSR_TxFifoEmpty = 0x80,
	};

	//************************************************************************
	// Text Engine

	// Character Control Register 0
	enum CCR0_Bits
	{
		CCR0_CharSet_Mask = 0x03,
		CCR0_CharSet8859_1 = 0x00,
		CCR0_CharSet8859_2 = 0x01,
		CCR0_CharSet8859_4 = 0x02,
		CCR0_CharSet8859_5 = 0x03,

		CCR0_CharHeight_Mask = 0x30,
		CCR0_CharHeight16 = 0x00,
		CCR0_CharHeight24 = 0x10,
		CCR0_CharHeight32 = 0x20,

		CCR0_CharSource_Mask = 0xC0,
		CCR0_CharSourceInternal = 0x00,
		CCR0_CharSourceExternal = 0x40,
		CCR0_CharSourceRam = 0x80,
	};

	// Character Control Register 1
	enum CCR1_Bits
	{
		CCR1_CharHeightX_Mask = 0x03,
		CCR1_CharHeightX1 = 0x00,
		CCR1_CharHeightX2 = 0x01,
		CCR1_CharHeightX3 = 0x02,
		CCR1_CharHeightX4 = 0x03,

		CCR1_CharWidthX_Mask = 0x0C,
		CCR1_CharWidthX1 = 0x00,
		CCR1_CharWidthX2 = 0x04,
		CCR1_CharWidthX3 = 0x08,
		CCR1_CharWidthX4 = 0x0C,

		CCR1_CharRotation_Mask = 0x01,
		CCR1_CharRotationNone = 0x00,
		CCR1_CharRotation90 = 0x01,

		CCR1_CharBackground_Mask = 0x40,
		CCR1_CharBackgroundSet = 0x00,
		CCR1_CharBackgroundTransparent = 0x40,
	};

	// GT Character ROM Select
	enum GTFNT_SEL_Bits
	{
		GTFNT_SEL_GT21L16T1W = 0x00,
		GTFNT_SEL_GT30L16U2W = 0x20,
		GTFNT_SEL_GT30L24T3Y = 0x40,
		GTFNT_SEL_GT30L24M1Z = 0x60,
		GTFNT_SEL_GT30L32S4W = 0x80,
		GTFNT_SEL_GT21L34S1W = 0xC0,
	};

	// GT Character ROM Control register
	enum GTFNT_CR
	{
		GTENT_CR_CharWidthFixed = 0x00,
		GTENT_CR_CharWidthArial = 0x01,
		GTENT_CR_CharWidthRoman = 0x02,
		GTENT_CR_CharWidthBold = 0x03,

		GTFNT_CR_Ascii = 0x20,
		// There's more, but we don't care
	};

	//************************************************************************
	// SDRAM Control

	// SDRAM attribute register
	enum SDRAR_Bits
	{
		// Choose one from each group
		SDRAR_ColumnBits8 = 0x00,
		SDRAR_ColumnBits9 = 0x01,
		SDRAR_ColumnBits10 = 0x02,
		SDRAR_ColumnBits11 = 0x03,
		SDRAR_ColumnBits12 = 0x04,

		SDRAR_RowBits11 = 0x00,
		SDRAR_RowBits12 = 0x08,
		SDRAR_RowBits13 = 0x10,

		SDRAR_Banks2 = 0x00,
		SDRAR_Banks4 = 0x20,

		SDRAR_TypeMobile = 0x40,

		SDRAR_RefreshPowerDown = 0x80
	};

	// SDRAM mode register & extended mode register
	enum SDRMD_Bits
	{
		// Choose one from each group
		SDRMD_CasClocks2 = 0x02,
		SDRMD_CasClocks3 = 0x03,

		SDRMD_DriverFull = 0x00,
		SDRMD_DriverHalf = 0x08,
		SDRMD_DriverQuarter = 0x10,
		SDRMD_DriverEighth = 0x18,

		SDRMD_ArrayFull = 0x00,
		SDRMD_ArrayHalf = 0x20,
		SDRMD_ArrayQuarter = 0x40,
		SDRMD_ArrayEighth = 0xA0,
		SDRMD_ArraySixteenth = 0xC0,
	};

	// SDRAM Control register
	enum SDRCR_Bits
	{
		// Choose one from each group
		SDRCR_InitDone = 0x01,

		SDRCR_PowerSave = 0x02,

		SDRCR_EnableTiming = 0x04,

		SDRCR_ClearWarning = 0x00,
		SDRCR_EnableWarning = 0x08,

		SDRCR_ClockEnabled = 0x10,

		SDRCR_BusWidth16 = 0x00,
		SDRCR_BusWidth32 = 0x20,

		SDRCR_BurstSize256 = 0x00,
		SDRCR_BurstSize128 = 0x40,
		SDRCR_BurstSize64 = 0x80,
		SDRCR_BurstSize32 = 0xC0,
	};
};
