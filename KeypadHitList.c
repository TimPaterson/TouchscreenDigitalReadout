//****************************************************************************
// KeypadHitList.c
//
// Created 11/6/2020 4:35:23 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "KeypadHit.h"


//****************************************************************************
// Read in the hotspot coordinates

#define DEFINE_HOTSPOT(name, group, x1, y1, x2, y2)	[name] = {x1, y1, x2, y2},

const HotspotList KeypadList = 
{
	KeypadSpotCount,
	{
	#include "Images/Keypad.h"
	}
};
