//****************************************************************************
// SerialMem.h
//
// Created 12/15/2020 11:12:29 AM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// Serial memory constants. Must be C-compatible.

static const int SerialFlashPageSize = 256;
static const int SerialFlashSectorSize = 0x1000;
static const int SerialFlashBlockSize = 0x8000;

#define FONT_FILE_LENGTH(val)	static const int FontFileLength = val;
#include "Fonts/Fonts.h"

#define SCREEN_FILE_LENGTH(val)	static const int ScreenFileLength = val;
#include "Images/Screen.h"

static const int MegaByte = 0x100000;
static const int FlashScreenStart = SerialFlashBlockSize;
static const int FlashFontStart = 14 * MegaByte;
static const int RamScreenStart = 0;
static const int RamFontStart = RamScreenStart + ScreenFileLength;
static const int RamFreeStart = RamFontStart + FontFileLength;
