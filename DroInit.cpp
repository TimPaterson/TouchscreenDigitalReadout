//****************************************************************************
// DroInit.cpp
//
// Created 10/1/2020 3:39:26 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "RA8876.h"

//*********************************************************************
// DFLL closed loop mode configuration

#define CONF_CLOCK_DFLL_MULTIPLY_FACTOR         DIV_UINT_RND(48000000, 32768)
#define CONF_CLOCK_DFLL_MAX_COARSE			    0x1f
#define CONF_CLOCK_DFLL_MAX_FINE				0xff
#define CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE    (CONF_CLOCK_DFLL_MAX_COARSE / 4)
#define CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE      (CONF_CLOCK_DFLL_MAX_FINE / 4)

void StartClock()
{
	SYSCTRL_DFLLVAL_Type		dfllVal;
	SYSCTRL_DFLLMUL_Type		dfflMul;
	
	// Start the 32kHz crystal oscillator, allow 32 clocks startup time
	SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP(1) | SYSCTRL_XOSC32K_XTALEN | 
		SYSCTRL_XOSC32K_ENABLE | SYSCTRL_XOSC32K_EN32K;
	// Wait for it to be ready
	while (!SYSCTRL->PCLKSR.bit.XOSC32KRDY);
	
	// Initialize clock generator 1 sourced by XOSC32K
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);
	GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_ID(1);

	// Clock DFLL from 32kHz clock generator 1
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_DFLL48 | GCLK_CLKCTRL_CLKEN |
		GCLK_CLKCTRL_GEN(1);
	ClearPinsB(LcdCs_PIN);
	
    // Initialize 48MHz clock
	// Errata: set ONDEMAND bit to zero first & enable
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
	// Set up calibration values
	dfllVal.reg = 0;
	dfllVal.bit.COARSE = NVM_SOFTWARE_CAL->DFLL48M_COARSE;
	dfllVal.bit.FINE = NVM_SOFTWARE_CAL->DFLL48M_FINE;
	SYSCTRL->DFLLVAL.reg = dfllVal.reg;
	// Multiplier
	dfflMul.reg = 0;
	dfflMul.bit.MUL = CONF_CLOCK_DFLL_MULTIPLY_FACTOR;
	dfflMul.bit.CSTEP = CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE;
	dfflMul.bit.FSTEP = CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE;
	SYSCTRL->DFLLMUL.reg = dfflMul.reg;
	// Set up control register in closed loop mode
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE | SYSCTRL_DFLLCTRL_MODE |
		SYSCTRL_DFLLCTRL_RUNSTDBY;
	// Wait for it to be ready
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY);
	while (!SYSCTRL->PCLKSR.bit.DFLLLCKC);	// Wait for coarse lock
	
	// Initialize clock generator 0 
	NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_RWS(1) | NVMCTRL_CTRLB_MANW;	// add a wait state for flash
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(1);
	GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_GENEN | 
		GCLK_GENCTRL_OE | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_ID(0);
}

//*********************************************************************

void Init()
{
	PM->APBCMASK.reg = PM_APBCMASK_AC | PM_APBAMASK_EIC;
	
	// Set port pin output levels first
	// RtpIrq_PIN input set high to pull-up
	SetPinsA(SdCs_PIN | RtpCs_PIN);
	SetPinsB(LcdCs_PIN | RtpIrq_PIN);
	// Set port pin direction (1 = output)
	DirWritePinsA(SdCs_PIN | RtpCs_PIN);
	DirWritePinsB(LcdData_PIN | LcdE_PIN | LcdCD_PIN | LcdRW_PIN | LcdCs_PIN);
	// Set all inputs to continuously sample
	PORT->Group[0].CTRL.reg = 0xFFFFFFFF;
	PORT->Group[1].CTRL.reg = 0xFFFFFFFF;
	// Turn on pull-up
	SetPortConfigB(PORT_WRCONFIG_INEN | PORT_WRCONFIG_PULLEN, RtpIrq_PIN);
	// Turn on input buffers
	SetPortConfigB(PORT_WRCONFIG_INEN, LcdData_PIN);

	//************************************************************************
	// Port pin multiplexers

	// Set up PA08 as NMI
	// Set up LCD WAIT, LCD IRQ, RTP IRQ, and SD card detect as interrupts or events
	// Set up all the position sensor inputs as external interrupts
	// This is on MUX channel A
	SetPortMuxA(PORT_MUX_A, Nmi_PIN | LcdWait_PIN | LcdIrq_PIN | QposA_PIN | QposB_PIN | 
		ZposA_PIN | ZposB_PIN | YposA_PIN | YposB_PIN | XposA_PIN | XposB_PIN);
	SetPortMuxB(PORT_MUX_A, MicroSdCd_PIN);

	// Set up Analog Comparator input on PA04
	// This in on MUX channel B
	SetPortMuxA(PORT_MUX_B, AcIn_PIN);

	// Set up SERCOM1 on PA16 (MOSI, pad0), PA17 (SCK, pad 1), PA19 (MISO, pad 3)
	// This is on MUX channel C
	SetPortMuxA(PORT_MUX_C, RtpMosi_PIN | RtpSck_PIN | RtpMiso_PIN);

	// Set up SERCOM0 on PA05 (RX, pad 1) and PA06 (TX, pad 2)
	// Set up SERCOM2 on PA09 (MISO, pad1), PA10 (MOSI, pad 2), PA11 (SCK, pad 3)
	// This is on MUX channel D
	SetPortMuxA(PORT_MUX_D, ConsoleRx_PIN | ConsoleTx_PIN | SdMiso_PIN | SdMosi_PIN | SdSck_PIN);

	// Set up TCC1 output W0[1] on PA07
	// This is on MUX channel E
	SetPortMuxA(PORT_MUX_E, BacklightPwm_PIN);
	
	// Set up USB on PA24 and PA25
	// This is on MUX channel G
	SetPortMuxA(PORT_MUX_G, UsbDm_BIT | UsbDp_BIT);

	// Set up Analog Comparator output on PA12
	// This is on MUX channel H
	SetPortMuxA(PORT_MUX_H, AcOut_PIN);

	//************************************************************************
	// Peripheral initialization

	// Set up analog comparator to monitor power supply
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_AC_DIG;
	AC->COMPCTRL[0].reg = AC_COMPCTRL_OUT_ASYNC | AC_COMPCTRL_MUXPOS_PIN0 |AC_COMPCTRL_MUXNEG_BANDGAP | 
		AC_COMPCTRL_SPEED_HIGH | AC_COMPCTRL_INTSEL_FALLING | AC_COMPCTRL_ENABLE;
	AC->CTRLA.reg = AC_CTRLA_LPMUX | AC_CTRLA_RUNSTDBY(1) | AC_CTRLA_ENABLE;
	// UNDONE: For testing, use regular interrupt on AC
	AC->INTENSET.reg = AC_INTENSET_COMP0;
	NVIC_EnableIRQ(AC_IRQn);

	// Set up EIC
	// GCLK only needed in case we select filtering
	// "To use NMI, GCLK_EIC must be enabled after EIC configuration (NMICTRL)"
	// UNDONE: EIC->NMICTRL.reg = EIC_NMICTRL_NMISENSE_FALL | EIC_NMICTRL_NMIFILTEN;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_EIC;
	EIC->INTENSET.reg = PosSensorIrqMask;

#define IRQCONFIG(irq, sense, filter)	EicConfig[irq > 7].reg |= (sense | filter) << (irq > 7 ? irq - 7 : irq) * 4
	EIC_CONFIG_Type	EicConfig[2];
	EicConfig[0].reg = 0;
	EicConfig[1].reg = 0;
	// Set all position sensor inputs to both edges, no filter
	IRQCONFIG(EIBIT_QposA, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_QposB, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_XposA, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_XposB, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_YposA, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_YposB, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_ZposA, EIC_CONFIG_SENSE0_BOTH, 0);
	IRQCONFIG(EIBIT_ZposB, EIC_CONFIG_SENSE0_BOTH, 0);
	// Resistive touch panel has active-low interrupt
	IRQCONFIG(EIBIT_Rtp, EIC_CONFIG_SENSE0_FALL, 0);
	// LCD panel has active-low interrupt
	// Use WAIT as event, requiring level detection
	IRQCONFIG(EIBIT_LcdIrq, EIC_CONFIG_SENSE0_FALL, 0);
	IRQCONFIG(EIBIT_LcdIrq, EIC_CONFIG_SENSE0_HIGH, 0);
	// Possibly use MicroSD MISO as interrupt for write complete
	// Possibly use MicroSD card detect as interrupt UNDONE: edge unknown
	IRQCONFIG(EIBIT_SdMiso, EIC_CONFIG_SENSE0_RISE, 0);
	IRQCONFIG(EIBIT_SdCd, EIC_CONFIG_SENSE0_RISE, EIC_CONFIG_FILTEN0);

	EIC->CONFIG[0].reg = EicConfig[0].reg;
	EIC->CONFIG[1].reg = EicConfig[1].reg;
	EIC->CTRL.reg = EIC_CTRL_ENABLE;
	NVIC_EnableIRQ(EIC_IRQn);
}
