//****************************************************************************
// FontInfo.h
//
// Created 11/29/2020 10:03:52 AM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// This is compatible with C so we can use it's superior initialization

typedef struct
{
	ulong	FontStart;
	ushort	CharsetWidth;
	ushort	Height;
	byte	FirstChar;
	byte	LastChar;
	byte	CharStride;
	byte	arWidths[];
} FontInfo;
