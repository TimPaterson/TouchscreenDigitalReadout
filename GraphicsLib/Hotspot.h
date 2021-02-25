//****************************************************************************
// Hotspot.h
//
// Created 11/6/2020 4:56:31 PM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// These are compatible with C so we can use it's superior initialization

typedef struct  
{
	byte	id;
	byte	group;
} HotspotData;

typedef struct
{
	ushort	X1;
	ushort	Y1;
	ushort	X2;
	ushort	Y2;
	HotspotData	data;
} Hotspot;

typedef struct
{
	ushort	count;
	Hotspot	HotspotList[];
} HotspotList;
