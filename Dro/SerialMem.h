//****************************************************************************
// SerialMem.h
//
// Created 12/15/2020 11:12:29 AM by Tim
//
//****************************************************************************

#pragma once

#include <sam.h>


//****************************************************************************
// Serial memory constants. Must be C-compatible.

static const int SerialFlashPageSize = 256;
static const int SerialFlashSectorSize = 0x1000;
static const int SerialFlashBlockSize = 0x8000;

#define FONT_FILE_LENGTH(val)	static const int FontFileLength = val;
#include "Fonts/Fonts.h"

#define SCREEN_FILE_LENGTH(val)	static const int ScreenFileLength = val;
#include "Images/Screen.h"

// The first chunk of screen memory is reserved for the binary image
// of the firmware during an update.
static const int MaxProgramSize = FLASH_SIZE;

static const int MegaByte = 0x100000;
// The first block of serial flash is unused, available
// for RA8876 startup code
static const int FlashScreenStart = SerialFlashBlockSize;
static const int FlashFontStart = 14 * MegaByte;

static const int RamUpdateStart = 0;
static const int RamScreenStart = RamUpdateStart + MaxProgramSize;
static const int RamFontStart = RamScreenStart + ScreenFileLength;
static const int RamFreeStart = RamFontStart + FontFileLength;
