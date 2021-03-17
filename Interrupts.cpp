//****************************************************************************
// Interrupts.cpp
//
// Created 10/13/2020 12:32:39 PM by Tim
//
//****************************************************************************

#include <standard.h>
#include "Dro.h"
#include "AxisDisplay.h"


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
	EIC->INTFLAG.reg = uIntFlags;	// reset all flags

	// Is it a position sensor?
	if (uIntFlags & PosSensorIrqMask)
	{
		uPortVal = PORTA->IN.ul;
		Xdisplay.InputChange(uPortVal >> XposA_BIT);
		Ydisplay.InputChange(uPortVal >> YposA_BIT);
		Zdisplay.InputChange(uPortVal >> ZposA_BIT);
		Qpos.InputChange(uPortVal >> QposA_BIT);
	}
}

//****************************************************************************
// WDT

#ifdef DEBUG

EXTERN_C void WdtHelper(uint uAddr)
{
	WDT->INTFLAG.reg = WDT_INTFLAG_EW;	// Clear interrupt
	DEBUG_PRINT("\nWDT interrupt from %x\n", uAddr);
}

void NAKED_ATTR WDT_Handler()
{
	asm volatile (
		"ldr	r0, [sp, #0x18] \n\t"
		"b		WdtHelper \n\t"
	);
}

#endif

//****************************************************************************
// Hard Fault

#ifdef DEBUG

EXTERN_C void WaitLoop()
{
	while (1);
}

EXTERN_C void HardFaultHelper(void * *ppv)
{
	void	*pv;
	
	pv = *ppv;
	*ppv = (void *)WaitLoop;
	DEBUG_PRINT("\nHard Fault at %p\n", pv);
}

void NAKED_ATTR HardFault_Handler()
{
	asm volatile (
		"mov	r0, sp \n\t"
		"add	r0, #0x18 \n\t"
		"b		HardFaultHelper \n\t"
	);
}

#endif
