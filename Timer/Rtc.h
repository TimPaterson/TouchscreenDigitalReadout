//****************************************************************************
// Rtc.h
//
// Created 2/25/2021 10:31:03 AM by Tim
//
//****************************************************************************

#pragma once


template<int baseYear = 2020, int gclk = 2>
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
	RtcTimeBase operator = (RTC_MODE2_CLOCK_Type op) { rtcTime.reg = op.reg; return *this; }

public:
	RtcTimeBase ReadClock()	{ rtcTime.reg = RTC->MODE2.CLOCK.reg; return *this;}
	bool IsSet()			{ return rtcTime.reg != 0; }
	void SetClock()			{ SetClock(rtcTime); }
	uint Second()			{ return rtcTime.bit.SECOND; }
	uint Minute()			{ return rtcTime.bit.MINUTE; }
	uint Hour()				{ return rtcTime.bit.HOUR; }
	uint Day()				{ return rtcTime.bit.DAY; }
	uint Month()			{ return rtcTime.bit.MONTH; }
	uint Year()				{ return rtcTime.bit.YEAR + BASE_YEAR; }

	void SetSecond(uint sec)	{ rtcTime.bit.SECOND = sec; }
	void SetMinute(uint min)	{ rtcTime.bit.MINUTE = min; }
	void SetHour(uint hour)		{ rtcTime.bit.HOUR = hour; }
	void SetDay(uint day)		{ rtcTime.bit.DAY = day; }
	void SetMonth(uint month)	{ rtcTime.bit.MONTH = month; }
	void SetYear(uint year)		{ rtcTime.bit.YEAR = year - BASE_YEAR; }

	void SetClock(RTC_MODE2_CLOCK_Type time)
	{
		RTC->MODE2.CLOCK.reg = time.reg;
		// The write disabled automatic read synchronization. Turn it back on.
		while (RTC->MODE2.STATUS.bit.SYNCBUSY);	// wait for write to finish
		RTC->MODE2.READREQ.reg = RTC_READREQ_RREQ | RTC_READREQ_RCONT | RTC_READREQ_ADDR(RTC_MODE2_CLOCK_OFFSET);
	}

	void SetTime(uint month, uint day, uint year, uint hour, uint minute, uint second)
	{
		rtcTime.reg = MakeTimeVal(month, day, year, hour, minute, second).reg;
	}

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

	RTC_MODE2_CLOCK_Type GetTimeVal()	{ return rtcTime; }

public:
	static void Init()
	{
#if defined(__SAMC__)
		// This code has not been tested. It requires a SAMC with 32 kHz xtal.
#error	"SAMC RTC initialization not yet tested"
		RTC->MODE2.CTRLA.reg = RTC_MODE2_CTRLA_CLOCKSYNC | RTC_MODE2_CTRLA_PRESCALER_DIV1024 | 
			RTC_MODE2_CTRLA_MODE_CLOCK | RTC_MODE2_CTRLA_ENABLE;
		RTC->MODE2.DBGCTRL.reg = RTC_DBGCTRL_DBGRUN;
#elif defined(__SAMD__)
		// Initialize clock generator to 1.024 kHz for RTC 
		// Use binary divider of 32 on 32.768 kHz xtal
		GCLK->GENDIV.reg = GCLK_GENDIV_ID(gclk) | GCLK_GENDIV_DIV(LOG2(32) - 1);
		GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_GENEN |
			GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(gclk);
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gclk) | GCLK_CLKCTRL_ID_RTC;

		RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_PRESCALER_DIV1024 | RTC_MODE2_CTRL_MODE_CLOCK | RTC_MODE2_CTRL_ENABLE;
		RTC->MODE2.READREQ.reg = RTC_READREQ_RREQ | RTC_READREQ_RCONT | RTC_READREQ_ADDR(RTC_MODE2_CLOCK_OFFSET);
		RTC->MODE2.DBGCTRL.reg = RTC_DBGCTRL_DBGRUN;
#else
#error	"SAM not defined for RTC"
#endif
	}

	static void SetClock(uint month, uint day, uint year, uint hour, uint minute, uint second)
	{
		SetClock(MakeTimeVal(month, day, year, hour, minute, second));
	}

	static RTC_MODE2_CLOCK_Type MakeTimeVal(uint month, uint day, uint year, uint hour, uint minute, uint second)
	{
		RTC_MODE2_CLOCK_Type	time;

		time.bit.SECOND = second;
		time.bit.MINUTE = minute;
		time.bit.HOUR = hour;
		time.bit.DAY = day;
		time.bit.MONTH = month;
		time.bit.YEAR = year - BASE_YEAR;

		return time;
	}

protected:
	RTC_MODE2_CLOCK_Type	rtcTime;
};
