//****************************************************************************
// LcdDef.h
//
// Created 10/24/2020 12:18:34 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876const.h"


using namespace RA8876const;

//*********************************************************************
// Initialization values for East Rising ER-TFTM101-1

// Hardware settings
static constexpr long OscFreq = 10'000'000;		// Crystal clock
static constexpr long CoreFreq = 100'000'000;	// RA8876 core clock - CCLK
static constexpr long SdramFreq = 100'000'000;	// SDRAM clock - MCLK
static constexpr int SdramRefresh = 64;			// SDRAM refresh interval; ms
static constexpr int SdramRowSize = 4096;		// SDRAM row size (for refresh)
static constexpr int SdramRefInterval = SdramFreq / 1000 * SdramRefresh / SdramRowSize - 2;

// LCD parameters
static constexpr long LcdScanFreq = 50'000'000;	// LCD scan clock - SCLK
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

// Serial flash/ROM SPI settings
// Unit 0: character generator ROM
static constexpr long MaxSpiClock0 = 30'000'000;	// when using Fast Read
static constexpr long SpiClock0 = std::min(MaxSpiClock0, CoreFreq / 2);
static constexpr int SpiDivisor0 = (CoreFreq / 2 + SpiClock0 - 1) / SpiClock0 - 1;
static constexpr int SFL_CTRL_Init0 = SFL_CTRL_Select0 | SFL_CTRL_ReadCommand0B | SFL_CTRL_AddrBits24;

// Unit 1: serial flash
static constexpr long MaxSpiClock1 = 30'000'000;
static constexpr long SpiClock1 = std::min(MaxSpiClock1, CoreFreq / 2);
static constexpr int SpiDivisor1 = (CoreFreq / 2 + SpiClock1 - 1) / SpiClock1 - 1;
static constexpr int SFL_CTRL_Init1 = SFL_CTRL_Select1 | SFL_CTRL_ReadCommand3B | SFL_CTRL_AddrBits24;
