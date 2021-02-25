//****************************************************************************
// Dro.h
//
// Created 10/1/2020 12:36:41 PM by Tim
//
//****************************************************************************

#pragma once

#include <IoBuf/UsartBuf.h>
#include <Util/TimerLib.h>
#include <Nvm/EepromMgr.h>
#include "Spi.h"
#include "SerialMem.h"
#include "FatFileDef.h"


#define VERSION	0

static constexpr double MmPerInch = 25.4;

// externs
void ChangeScreenBrightness(int change);
void DumpCanvas(uint addr);

//*********************************************************************
// Define port pins
//
// PA00 - xtal
// PA01 - xtal
// PA02 - in, Q position A (EXINT 2)
// PA03 - in, Q position B (EXINT 3)
// PA04 - analog in, supply voltage (AC AIN[0])
// PA05 - in, console serial RX data (SERCOM 0 pad 1)
// PA06 - out, console serial TX data (SERCOM 0 pad 2)
// PA07 - out, backlight PWM (TCC1 WO[1])
// PA08 - in, supply low (NMI)
// PA09 - in, MicroSD DOUT/MISO (SERCOM 2 pad 1 & EXINT 9)
// PA10 - out, MicroSD DIN/MOSI (SERCOM 2 pad 2)
// PA11 - out, MicroSD SCK (SERCOM 2 pad 3)
// PA12 - out, supply low (analog comparator CMP[0])
// PA13 - in, Z position A (EXINT 13)
// PA14 - in, Z position B (EXINT 14)
// PA15 - out, MicroSD CS
// PA16 - out, RTP DIN/MOSI (SERCOM 1 pad 0)
// PA17 - out, RTP SCK (SERCOM 1 pad 1)
// PA18 - out, RTP CS
// PA19 - in, RTP DOUT/MISO (SERCOM 1 pad 3)
// PA20 - in, Y position A (EXINT 4)
// PA21 - in, Y position B (EXINT 5)
// PA22 - in, X position A (EXINT 6)
// PA23 - in, X position B (EXINT 7)
// PA24 - USB D-
// PA25 - USB D+
// PA27 - in, RA8876 WAIT (EXINT 15)
// PA28 - in, RA8876 INT (EXINT 8)
//
// PB00 - in/out, RA8876 DB0
// PB01 - in/out, RA8876 DB1
// PB02 - in/out, RA8876 DB2
// PB03 - in/out, RA8876 DB3
// PB04 - in/out, RA8876 DB4
// PB05 - in/out, RA8876 DB5
// PB06 - in/out, RA8876 DB6
// PB07 - in/out, RA8876 DB7
// PB08 - in/out, RA8876 DB8
// PB09 - in/out, RA8876 DB9
// PB10 - in/out, RA8876 DB10
// PB11 - in/out, RA8876 DB11
// PB12 - in/out, RA8876 DB12
// PB13 - in/out, RA8876 DB13
// PB14 - in/out, RA8876 DB14
// PB15 - in/out, RA8876 DB15
// PB16 - in w/pull-up, RTP IRQ (EXINT 0)
// PB17 - in, MicroSD card detect (EXINT 1)
// PB22 - out, RA8876 E (TC7 WO[0])
// PB23 - out, RA8876 C/D
// PB30 - out, RA8877 R/W
// PB31 - out, RA9977 CS

enum PortAbitMap
{
	QposA_BIT = 2,
	QposB_BIT,
	AcIn_BIT,
	ConsoleRx_BIT,
	ConsoleTx_BIT,
	BacklightPwm_BIT,

	Nmi_BIT,
	SdMiso_BIT,
	SdMosi_BIT,
	SdSck_BIT,
	AcOut_BIT,
	ZposA_BIT,
	ZposB_BIT,
	SdCs_BIT,

	RtpMosi_BIT,
	RtpSck_BIT,
	RtpCs_BIT,
	RtpMiso_BIT,
	YposA_BIT,
	YposB_BIT,
	XposA_BIT,
	XposB_BIT,

	UsbDm_BIT,
	UsbDp_BIT,
	LcdWait_BIT = 27,
	LcdIrq_BIT = 28
};

enum PortApins
{
	QposA_PIN =			(1 << QposA_BIT),
	QposB_PIN =			(1 << QposB_BIT),
	AcIn_PIN =			(1 << AcIn_BIT),
	ConsoleRx_PIN =		(1 << ConsoleRx_BIT),
	ConsoleTx_PIN =		(1 << ConsoleTx_BIT),
	BacklightPwm_PIN =	(1 << BacklightPwm_BIT),

	Nmi_PIN =			(1 << Nmi_BIT),
	SdMiso_PIN =		(1 << SdMiso_BIT),
	SdMosi_PIN =		(1 << SdMosi_BIT),
	SdSck_PIN =			(1 << SdSck_BIT),
	AcOut_PIN =			(1 << AcOut_BIT),
	ZposA_PIN =			(1 << ZposA_BIT),
	ZposB_PIN =			(1 << ZposB_BIT),
	SdCs_PIN =			(1 << SdCs_BIT),

	RtpMosi_PIN =		(1 << RtpMosi_BIT),
	RtpSck_PIN =		(1 << RtpSck_BIT),
	RtpCs_PIN =			(1 << RtpCs_BIT),
	RtpMiso_PIN =		(1 << RtpMiso_BIT),
	YposA_PIN =			(1 << YposA_BIT),
	YposB_PIN =			(1 << YposB_BIT),
	XposA_PIN =			(1 << XposA_BIT),
	XposB_PIN =			(1 << XposB_BIT),

	UsbDm_PIN =			(1 << UsbDm_BIT),
	UsbDp_PIN =			(1 << UsbDp_BIT),
	LcdWait_PIN =		(1 << LcdWait_BIT),
	LcdIrq_PIN =		(1 << LcdIrq_BIT),
};

enum PortBbitMap
{
	RtpIrq_BIT = 16,
	MicroSdCd_BIT,
	LcdE_BIT = 22,
	LcdCD_BIT,
	LcdRW_BIT = 30,
	LcdCs_BIT
};

enum PortBpins
{
	LcdData_PIN =	0xFFFF,
	RtpIrq_PIN =	(1 << RtpIrq_BIT),
	MicroSdCd_PIN =	(1 << MicroSdCd_BIT),
	LcdE_PIN =		(1 << LcdE_BIT),
	LcdCD_PIN =		(1 << LcdCD_BIT),
	LcdRW_PIN =		(1 << LcdRW_BIT),
	LcdCs_PIN =		(1 << LcdCs_BIT),
};

template <typename T>
struct PortIoGroup
{
	volatile T	DIR;
	volatile T	DIRCLR;
	volatile T	DIRSET;
	volatile T	DIRTGL;
	volatile T	OUT;
	volatile T	OUTCLR;
	volatile T	OUTSET;
	volatile T	OUTTGL;
	volatile T	IN;
};

//*********************************************************************
// Define struct for PORTA allowing byte access per pin assignments

union PortAreg
{
	ulong	ul;
	struct 
	{
		byte	b0;
		byte	Sd;
		byte	Rtp;
		byte	Lcd;
	};
};

static constexpr int PortASdPos = offsetof(PortAreg, Sd) * 8;
static constexpr int PortARtpPos = offsetof(PortAreg, Rtp) * 8;
static constexpr int PortALcdPos = offsetof(PortAreg, Lcd) * 8;

typedef PortIoGroup<PortAreg> PortIoA;

enum  PortAregMap
{
	SdCs =		(1 << (SdCs_BIT - PortASdPos)),
	RtpCs =		(1 << (RtpCs_BIT - PortARtpPos)),
	LcdWait =	(1 << (LcdWait_BIT - PortALcdPos)),
	LcdIrq =	(1 << (LcdIrq_BIT - PortALcdPos)),
};

#define PORTA	((volatile PortIoA *)&PORT_IOBUS->Group[0])

inline void SetSdCs() { PORTA->OUTSET.Sd = SdCs; }
inline void ClearSdCs() { PORTA->OUTCLR.Sd = SdCs; }

inline void SetRtpCs() { PORTA->OUTSET.Rtp = RtpCs; }
inline void ClearRtpCs() { PORTA->OUTCLR.Rtp = RtpCs; }

inline int GetLcdWait() { return PORTA->IN.Lcd & LcdWait; }
inline int GetLcdIrq() { return PORTA->IN.Lcd & LcdIrq; }

//*********************************************************************
// Define struct for PORTB allowing byte access per pin assignments

union PortBreg
{
	ulong	ul;
	byte	Lcd8;
	struct
	{
		ushort	Lcd16;
		union
		{
			ushort	Ctrl;
			struct
			{
				byte	CtrlLo;
				byte	CtrlHi;
			};			
		};
	};
};

static constexpr int PortBCtrlPos = offsetof(PortBreg, Ctrl) * 8;

typedef PortIoGroup<PortBreg> PortIoB;

enum  PortBregMap
{
	LcdData8 = 0xFF,
	LcdData16 = 0xFFFF,

	RtpIrq =	(1 << (RtpIrq_BIT - PortBCtrlPos)),
	MicroSdCd =	(1 << (MicroSdCd_BIT - PortBCtrlPos)),
	LcdE =		(1 << (LcdE_BIT - PortBCtrlPos)),
	LcdCD =		(1 << (LcdCD_BIT - PortBCtrlPos)),
	LcdRW =		(1 << (LcdRW_BIT - PortBCtrlPos)),
	LcdCs =		(1 << (LcdCs_BIT - PortBCtrlPos)),
};

#define PORTB	((volatile PortIoB *)&PORT_IOBUS->Group[1])

inline int GetSdCd() { return PORTB->IN.Ctrl & MicroSdCd; }

inline void SetLcdPin(ushort pins)
{
	if ((pins > 0xFF) && (pins & 0xFF))
	{
		PORTB->OUTSET.Ctrl = pins;
		return;
	}

	if (pins & 0xFF)
		PORTB->OUTSET.CtrlLo = pins;

	if (pins > 0xFF)
		PORTB->OUTSET.CtrlHi = pins >> 8;
}

inline void ClearLcdPin(ushort pins)
{
	if ((pins > 0xFF) && (pins & 0xFF))
	{
		PORTB->OUTCLR.Ctrl = pins;
		return;
	}

	if (pins & 0xFF)
		PORTB->OUTCLR.CtrlLo = pins;

	if (pins > 0xFF)
		PORTB->OUTCLR.CtrlHi = pins >> 8;
}

//*********************************************************************
// External Interrupt summary
//
// EXINT 0 - RTP IRQ
// EXINT 1 - MicroSD card detect
// EXINT 2 - Q position A
// EXINT 3 - Q position B
// EXINT 4 - Y position A
// EXINT 5 - Y position B
// EXINT 6 - X position A
// EXINT 7 - X position B
// EXINT 8 - RA8876 INT
// EXINT 9 - MicroSD MISO
// EXINT 10 -
// EXINT 11 -
// EXINT 12 -
// EXINT 13 - Z position A
// EXINT 14 - Z position B
// EXINT 15 - RA8876 WAIT

enum ExtIrqBit
{
	EIBIT_Rtp,
	EIBIT_SdCd,
	EIBIT_QposA,
	EIBIT_QposB,
	EIBIT_YposA,
	EIBIT_YposB,
	EIBIT_XposA,
	EIBIT_XposB,
	EIBIT_LcdIrq,
	EIBIT_SdMiso,
	EIBIT_ZposA = 13,
	EIBIT_ZposB,
	EIBIT_LcdWait,
};

enum ExtIrq
{
	EI_Rtp =		(1 << EIBIT_Rtp),
	EI_SdCd =		(1 << EIBIT_SdCd),
	EI_QposA =		(1 << EIBIT_QposA),
	EI_QposB =		(1 << EIBIT_QposB),
	EI_YposA =		(1 << EIBIT_YposA),
	EI_YposB =		(1 << EIBIT_YposB),
	EI_XposA =		(1 << EIBIT_XposA),
	EI_XposB =		(1 << EIBIT_XposB),
	EI_LcdIrq =		(1 << EIBIT_LcdIrq),
	EI_SdMiso =		(1 << EIBIT_SdMiso),
	EI_ZposA =		(1 << EIBIT_ZposA),
	EI_ZposB =		(1 << EIBIT_ZposB),
	EI_LcdWait =	(1 << EIBIT_LcdWait),
};

//*********************************************************************
// Timer on TC3. 
//
// With prescale of 1024, resolution is 1 / (48MHz / 1024) = 21.3us, 
// and max delay is 1.4 seconds.

typedef DECLARE_TIMER(TC3, 1024) Timer;

//*********************************************************************
// LCD backlight LED PWM on TCC1 

static constexpr int LcdBacklightPwmFreq = 200;
static constexpr int LcdBacklightPwmMax = F_CPU / LcdBacklightPwmFreq -1;

//*********************************************************************
// Console (debug) serial port

static constexpr int CONSOLE_BAUD_RATE = 500000;

typedef DECLARE_USART(SERCOM0, 100, 100)	Console_t;

#ifdef DEBUG
#define DEBUG_PRINT	::printf
#else
#define DEBUG_PRINT(...)
#endif

extern Console_t Console;
extern FILE Console_FILE;

//*********************************************************************
// Position sensors

static constexpr uint PosSensorIrqMask = EI_QposA | EI_QposB | 
	EI_YposA | EI_YposB | EI_ZposA | EI_ZposB | EI_XposA | EI_XposB;

//*********************************************************************
// Brown-out detector
//
// When voltage drops, the flash can't keep up. Keep it in reset when
// the voltage is below 2.7V.

// This number comes from the data sheet, and is different for some
// silicon revisions. We're using Rev G because it has the EEPROM
// emulation flash in the 128K version.
//
static constexpr byte BOD_LEVEL_2p7_REVG = 39;

//*********************************************************************
// Real-time clock

template<int gclk = 2, int baseYear = 2020>
class RtcTimeBase
{
public:
	static constexpr int BASE_YEAR = baseYear & ~3;	// must be leap year
	static constexpr int MAX_YEAR = BASE_YEAR + 63;

protected:
	// This constant is applied after shifting the time right 1 bit
	static constexpr int FatTimeConversion = (BASE_YEAR - 1980) << (RTC_MODE2_CLOCK_YEAR_Pos - 1);

public:
	RtcTimeBase() {}
	RtcTimeBase(bool fInit)	{ rtcTime.reg = 0; }
public:
	bool operator == (RtcTimeBase op) { return rtcTime.reg == op.rtcTime.reg; }
	bool operator != (RtcTimeBase op) { return !(rtcTime.reg == op.rtcTime.reg); }

public:
	RtcTimeBase ReadClock()	{ rtcTime.reg = RTC->MODE2.CLOCK.reg; return *this;}
	bool IsSet()		{ return rtcTime.reg != 0; }
	void SetClock()		{ RTC->MODE2.CLOCK.reg = rtcTime.reg; }
	uint Second()		{ return rtcTime.bit.SECOND; }
	uint Minute()		{ return rtcTime.bit.MINUTE; }
	uint Hour()			{ return rtcTime.bit.HOUR; }
	uint Day()			{ return rtcTime.bit.DAY; }
	uint Month()		{ return rtcTime.bit.MONTH; }
	uint Year()			{ return rtcTime.bit.YEAR + BASE_YEAR; }

	void SetSecond(uint sec)	{ rtcTime.bit.SECOND = sec; }
	void SetMinute(uint min)	{ rtcTime.bit.MINUTE = min; }
	void SetHour(uint hour)		{ rtcTime.bit.HOUR = hour; }
	void SetDay(uint day)		{ rtcTime.bit.DAY = day; }
	void SetMonth(uint month)	{ rtcTime.bit.MONTH = month; }
	void SetYear(uint year)		{ rtcTime.bit.YEAR = year - BASE_YEAR; }

	// 12-hour clock
	uint Hour12()
	{
		uint hour = rtcTime.bit.HOUR;
		return hour > 12 ? hour - 12 : (hour == 0 ? 12 : hour);
	}

	bool AmPm()		{ return rtcTime.bit.HOUR >= 12; }

	void SetHour(uint hour, bool pm)		
	{
		if (pm)
			rtcTime.bit.HOUR = hour >= 12 ? hour : hour + 12;
		else
			rtcTime.bit.HOUR = hour == 12 ? 0 : hour;
	}

	// Conversion to FAT format
	ulong GetFatTime()	
	{
		// Add 1 to day and month, and adjust year for different base
		return (rtcTime.reg >> 1) + FatTimeConversion;
	}

public:
	static void Init()
	{
		// Initialize clock generator to 1.024 kHz for RTC 
		// Use binary divider of 32 on 32.768 kHz xtal
		GCLK->GENDIV.reg = GCLK_GENDIV_ID(gclk) | GCLK_GENDIV_DIV(LOG2(32) - 1);
		GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_GENEN |
			GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(gclk);
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gclk) | GCLK_CLKCTRL_ID_RTC;

		RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_PRESCALER_DIV1024 | RTC_MODE2_CTRL_MODE_CLOCK | RTC_MODE2_CTRL_ENABLE;
		RTC->MODE2.READREQ.reg = RTC_READREQ_RREQ | RTC_READREQ_RCONT | RTC_READREQ_ADDR(RTC_MODE2_CLOCK_OFFSET);
		RTC->MODE2.DBGCTRL.reg = RTC_DBGCTRL_DBGRUN;
	}

	static void SetClock(uint month, uint day, uint year, uint hour, uint minute, uint second)
	{
		RTC_MODE2_CLOCK_Type	time;

		time.bit.SECOND = second;
		time.bit.MINUTE = minute;
		time.bit.HOUR = hour;
		time.bit.DAY = day;
		time.bit.MONTH = month;
		time.bit.YEAR = year - BASE_YEAR;

		RTC->MODE2.CLOCK.reg = time.reg;
	}

protected:
	RTC_MODE2_CLOCK_Type	rtcTime;
};

typedef RtcTimeBase<> RtcTime;

//*********************************************************************
// EEPROM data definition
// Actually managed in flash my EepromMgr

struct AxisInfo
{
	double	Correction;
	byte	Resolution;		// microns (typically 5)
	bool	Direction;
};

#include "Xtp2046.h"

// The first two rows are reserved to store position at shutdown
static constexpr int ReservedEepromRows = 2;

#define EepromData(typ, name, ...)	typ name;

struct Eeprom_t
{
	#include "EepromData.h"
};

extern const Eeprom_t RwwData;	// initial EEPROM data
typedef EepromMgr<Eeprom_t, &RwwData, ReservedEepromRows> EepromMgr_t;

extern EepromMgr_t Eeprom;
