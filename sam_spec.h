/*
 * sam_spec.h
 *
 * Created: 8/18/2018 12:13:08 PM
 *  Author: Tim
 */ 

#pragma once

#if		defined(_SYSTEM_SAMC20_H_INCLUDED_)
	
#define __SAMC20__

#elif	defined(_SYSTEM_SAMC21_H_INCLUDED_)

#define __SAMC21__

#elif	defined(_SYSTEM_SAMD10_H_INCLUDED_)

#define __SAMD10__

#elif	defined(_SYSTEM_SAMD11_H_INCLUDED_)

#define __SAMD11__

#elif	defined(_SYSTEM_SAMD20_H_INCLUDED_)

#define __SAMD20__

#elif	defined(_SYSTEM_SAMD21_H_INCLUDED_)

#define __SAMD21__

#endif


#if		defined(__SAMC20__) || defined(__SAMC21__)

#define __SAMC__

#elif	defined(__SAMD10__) || defined(__SAMD11__) || defined(__SAMD20__) || defined(__SAMD21__)

#define __SAMD__

#endif

//*********************************************************************
// Define calibration row

#if		defined(__SAMC__)

struct NvmSofwareCal_t
{
	uint64_t	ADC0_BIASREFBUF:3;
	uint64_t	ADC0_BIASCOMP:3;
	uint64_t	ADC1_LINEARITY:3;
	uint64_t	ADC1_BIAS:3;
	uint64_t	OSC32K:7;
	uint64_t	CAL48M_5V:22;
	uint64_t	CAL48M_3V3:22;
};

#define NVM_SOFTWARE_CAL	((NvmSofwareCal_t *)NVMCTRL_OTP5)

struct NvmUserRow_t
{
	uint64_t	BOOTPROT:3;
	uint64_t	:1;
	uint64_t	EEPROM:3;
	uint64_t	:1;
	uint64_t	BODVDD_LEVEL:6;
	uint64_t	BODVDD_ENABLE:1;
	uint64_t	BODVDD_ACTION:2;
	uint64_t	:9;
	uint64_t	WDT_ENABLE:1;
	uint64_t	WDT_ALWAYSON:1;
	uint64_t	WDT_PERIOD:4;
	uint64_t	WDT_WINDOW:4;
	uint64_t	WDT_EWOFFSET:4;
	uint64_t	WDT_WEN:1;
	uint64_t	BODVDD_HYSTERESIS:1;
	uint64_t	:7;
	uint64_t	LOCK:16;
};

#define NVM_USER_ROW	((NvmUserRow_t *)NVMCTRL_USER)

#elif	defined(__SAMD__)

struct NvmSofwareCal_t
{
	uint64_t	:27;
	uint64_t	ADC_LINEARITY:8;
	uint64_t	ADC_BIAS:3;
	uint64_t	OSC32K:7;
	
	// USB fields apply only to D21
	uint64_t	USB_TRANSN:5;
	uint64_t	USB_TRANSP:5;
	uint64_t	USB_TRIM:3;
	
	uint64_t	DFLL48M_COARSE:6;	// not set in Rev C
	uint64_t	DFLL48M_FINE:10;	// not set in Rev C
	uint64_t	:54;
};

#define NVM_SOFTWARE_CAL	((NvmSofwareCal_t *)NVMCTRL_OTP4)

struct NvmUserRow_t
{
	uint64_t	BOOTPROT:3;
	uint64_t	:1;
	uint64_t	EEPROM:3;
	uint64_t	:1;
	uint64_t	BOD33_LEVEL:6;
	uint64_t	BOD33_ENABLE:1;
	uint64_t	BOD33_ACTION:2;
	uint64_t	:8;
	uint64_t	WDT_ENABLE:1;
	uint64_t	WDT_ALWAYSON:1;
	uint64_t	WDT_PERIOD:4;
	uint64_t	WDT_WINDOW:4;
	uint64_t	WDT_EWOFFSET:4;
	uint64_t	WDT_WEN:1;
	uint64_t	BOD33_HYSTERESIS:1;
	uint64_t	:8;
	uint64_t	LOCK:16;
};

#define NVM_USER_ROW	((NvmUserRow_t *)NVMCTRL_USER)

#endif

//*********************************************************************
// Define serial number location

#define	NVM_SERIAL_NUMBER_0		((uint32_t *)0x0080A00C)
#define	NVM_SERIAL_NUMBER_1		((uint32_t *)0x0080A040)
#define	NVM_SERIAL_NUMBER_2		((uint32_t *)0x0080A044)
#define	NVM_SERIAL_NUMBER_3		((uint32_t *)0x0080A048)

