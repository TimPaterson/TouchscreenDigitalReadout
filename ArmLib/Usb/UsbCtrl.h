//****************************************************************************
// UsbCtrl.h
//
// Created 8/17/2020 4:49:07 PM by Tim
//
//****************************************************************************

#pragma once

#include "Usb.h"


class UsbCtrl
{
protected:
	static constexpr int MaxUsbTransferSize = (1 << 14) - 1;	// 14-bit field

public:
	static void Init()
	{
		USB_PADCAL_Type		padCal;

		// Turn on the clocks
		PM->APBBMASK.reg |= PM_APBBMASK_USB;
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;

		// Mux the port pins to the USB module on Mux G
		SetPortMuxA(PORT_MUX_G, PORT_PA24 | PORT_PA25);

		// Pad calibration
		padCal.reg = 0;
		padCal.bit.TRANSP = NVM_SOFTWARE_CAL->USB_TRANSP;
		padCal.bit.TRANSN = NVM_SOFTWARE_CAL->USB_TRANSN;
		padCal.bit.TRIM = NVM_SOFTWARE_CAL->USB_TRIM;
		USB->HOST.PADCAL.reg = padCal.reg;

		// Increase SRAM quality of service for low latency
		USB->HOST.QOSCTRL.reg = USB_QOSCTRL_CQOS_MEDIUM | USB_QOSCTRL_DQOS_MEDIUM;

		// Lower interrupt priority because we could spend some time 
		// servicing USB interrupts. Priority 0 = highest, 3 = lowest.
		NVIC_SetPriority(USB_IRQn, 2);
		// Enable interrupts
		NVIC_EnableIRQ(USB_IRQn);
	};
};
