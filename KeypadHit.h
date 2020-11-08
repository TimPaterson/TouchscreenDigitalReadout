//****************************************************************************
// KeypadHit.h
//
// Created 11/7/2020 12:38:35 PM by Tim
//
//****************************************************************************

#pragma once

#include "HitTest.h"

//****************************************************************************
// Enumerate hotspots.

enum DigitHotspots
{
	Key_0,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,
};


enum KeypadHotspots
{
	StartKey = Key_9,

#define DEFINE_HOTSPOT_Keypad(name, group, x1, y1, x2, y2)	name,
#include "Images/Keypad.h"

	KeypadSpotCount
};

#ifdef __cplusplus

extern "C" HotspotList KeypadList;

class KeypadHit : public HitTest
{
public:
	void Init(uint x, uint y)
	{
		SetOrigin(x, y);
		SetList(&KeypadList);
	}
};

extern KeypadHit KeyHit;

#endif	// __cplusplus