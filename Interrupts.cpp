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

void NonMaskableInt_Handler()
{
	Console.WriteStringNoBuf("Shutting down\r\n");
	// Disable all I/O pins. Make them all inputs and
	// disable mux, pull-up, input buffer
	DirWritePinsA(0);
	SetPortConfigA(0, ALL_PORT_PINS);	
	DirWritePinsB(0);
	SetPortConfigB(0, ALL_PORT_PINS);

	// Turn off some clocks to save power
	PM->APBCMASK.reg = 0;	// all peripherals off
	PM->APBBMASK.reg = PM_APBBMASK_NVMCTRL | PM_APBBMASK_DSU;
	PM->AHBMASK.reg = PM_AHBMASK_NVMCTRL | PM_APBBMASK_DSU | PM_AHBMASK_HPB2 | 
		PM_AHBMASK_HPB1 | PM_AHBMASK_HPB0;
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
