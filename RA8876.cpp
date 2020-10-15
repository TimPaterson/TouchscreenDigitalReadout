//****************************************************************************
// Class RA8876
// RA8876.cpp
//
// Created 10/11/2020 4:57:27 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "RA8876.h"


void RA8876::WriteReg(int addr, int val)
{
	// Write address
	ClearLcdPin(LcdRW | LcdCs | LcdCD);
	PORTB->OUT.Lcd8 = addr;	// Address
	SetLcdPin(LcdE);	// toggle E
	ClearLcdPin(LcdE);

	// Write data
	SetLcdPin(LcdCD);
	PORTB->OUT.Lcd8 = val;	// Data
	SetLcdPin(LcdE);	// toggle E
	ClearLcdPin(LcdE);
	SetLcdPin(LcdCs);
}

