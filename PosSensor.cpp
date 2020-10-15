//****************************************************************************
// Class PosSensor
// PosSensor.cpp
//
// Created 10/13/2020 12:57:40 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "PosSensor.h"


//********************************************************************
// Define the four sensors

PosSensor Xpos;
PosSensor Ypos;
PosSensor Zpos;
PosSensor Qpos;


//********************************************************************
// Implementation of PostSensor

// This table maps which bit changed and the current bit state to
// determine the direction of rotation. It is invalid for both or
// neither input to be changed.
//
const sbyte PosSensor::s_arbQuadDecode[16] =
{
//			Bprev	Aprev	Bcur	Acur
	 0, //	 0		 0		 0		 0		no change
	+1, //	 0		 0		 0		 1		A rise, B lo
	-1, //	 0		 0		 1		 0		B rise, A lo
	 0, //	 0		 0		 1		 1		both change, invalid
	-1, //	 0		 1		 0		 0		A fall, B lo
	 0, //	 0		 1		 0		 1		no change
	 0, //	 0		 1		 1		 0		both change, invalid
	+1, //	 0		 1		 1		 1		B rise, A hi
	+1, //	 1		 0		 0		 0		B fall, A lo
	 0, //	 1		 0		 0		 1		both change, invalid
	 0, //	 1		 0		 1		 0		no change
	-1, //	 1		 0		 1		 1		A rise, B hi
	 0, //	 1		 1		 0		 0		both change, invalid
	-1, //	 1		 1		 0		 1		B fall, A hi
	+1, //	 1		 1		 1		 0		A fall, B hi
	 0, //	 1		 1		 1		 1		no change
};

void PosSensor::InputChange(uint uSignal)
{
	uSignal &= 3;	// low two bits - A and B signals
	m_iCurPos += s_arbQuadDecode[(m_bPrevSig << 2) + uSignal];
	m_bPrevSig = uSignal;
}

