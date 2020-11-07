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

enum KeypadHotspots
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
	Key_Sign,
	Key_Decimal,
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