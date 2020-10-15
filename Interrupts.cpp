//****************************************************************************
// Interrupts.cpp
//
// Created 10/13/2020 12:32:39 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "PosSensor.h"


//****************************************************************************
// USART

DEFINE_USART_ISR(SERCOM0, Console)

//****************************************************************************
// Analog Comparator UNDONE: testing only

void AC_Handler()
{
	Console.WriteString("Shutting down\n");
	while(1);
}

//****************************************************************************
// External Interrupts

void EIC_Handler()
{
	uint	uIntFlags;
	uint	uPortVal;

	uIntFlags = EIC->INTFLAG.reg;

	// Is it a position sensor?
	if (uIntFlags & PosSensorIrqMask)
	{
		EIC->INTFLAG.reg = PosSensorIrqMask;	// reset all flags
		uPortVal = PORTA->IN.ul;
		Xpos.InputChange(uPortVal >> XposA_BIT);
		Ypos.InputChange(uPortVal >> YposA_BIT);
		Zpos.InputChange(uPortVal >> ZposA_BIT);
		Qpos.InputChange(uPortVal >> QposA_BIT);
	}
}
