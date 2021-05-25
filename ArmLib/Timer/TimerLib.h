//****************************************************************************
// TimerLib.h
//
// Created 4/5/2017 12:11:28 PM by Tim
//
//****************************************************************************

#pragma once

#ifndef	Timer_t
#define Timer_t	uint16_t
#endif


//****************************************************************************
// This macro uses the IntervalTimer template below to create a timer
// Example:
//
// typedef DECLARE_TIMER(TC3, 16) Timer;
// Timer::Init();	// initialized the hardware
// 
// First parameter is a TC or TCC, second is prescale
//
#define DECLARE_TIMER(ctr, prescale) \
	IntervalTimer<#ctr[2] == 'C', (#ctr[2] == 'C' ? #ctr[3] : #ctr[2]) - '0', prescale, \
	#ctr[2] == 'C' ? CONCAT(TCC_CTRLA_PRESCALER_DIV, prescale) : CONCAT(TC_CTRLA_PRESCALER_DIV, prescale)>


//****************************************************************************
// These helpers are UNDEFed at the end
#define TCC_PTR(i)		((Tcc *)((byte *)TCC0 + ((byte *)TCC1 - (byte *)TCC0) * i))
#ifdef TC0
#define TC_PTR(i)		((TcCount16 *)((byte *)TC0 + ((byte *)TC1 - (byte *)TC0) * i))
#else
#define TC_PTR(i)		((TcCount16 *)((byte *)TC3 + ((byte *)TC4 - (byte *)TC3) * (i - 3)))
#endif


//****************************************************************************
// Define_Timer template
//
// Assumes a timer/counter has been initialized and a function provided to
// get the current value.
//
// Template parameters:
//
// TimerClockFreq	- clock rate of the timer
// GetTickCount		- function returning Timer_t of tick count
// CpuFreq			- CPU frequency, defaults to F_CPU (for asm loops)

template <ulong TimerClockFreq, Timer_t (*GetTickCount)(), ulong CpuFreq
#ifdef F_CPU
 = F_CPU
 #endif
 >
class Define_Timer
{
public:
	// Assuming Timer_t is uint16_t, this converts ticks back to time
	static constexpr double SecondsPerTick = 1.0 / TimerClockFreq;
	static constexpr double MicrosecPerTick = SecondsPerTick * 1E6;
	static constexpr double MaxDelaySeconds = 65535.0 * SecondsPerTick;
	static constexpr int TickShift = LOG2(65535 / MicrosecPerTick);
	static constexpr uint TickScale = 1 << TickShift;
	static constexpr uint MicrosecPerTickScale = lround(MicrosecPerTick * TickScale);

protected:
	// For assembly-language delay loops
	static constexpr uint ClocksPerLoop = 3;
	static constexpr uint LoopBaseClocks = 3;

public:
	INLINE_ATTR static void Delay(double sec)
	{
		Define_Timer	tmr;
		
		tmr.Start();
		while (!tmr.CheckDelay(sec));
	}
	
	INLINE_ATTR static void Delay_us(double us)
	{
		Define_Timer	tmr;
		
		tmr.Start();
		while (!tmr.CheckDelay_us(us));
	}
	
	INLINE_ATTR static void Delay_ms(double ms)
	{
		Define_Timer	tmr;
		
		tmr.Start();
		while (!tmr.CheckDelay_ms(ms));
	}

public:
	INLINE_ATTR static void ShortDelay_ns(double ns)	{ ShortDelay_clocks(ClocksFromNs(ns)); }
	INLINE_ATTR static void ShortDelay_us(double us)	{ ShortDelay_clocks(ClocksFromUs(us)); }
	
public:
	INLINE_ATTR Timer_t Start()						{ return m_uLastTime = GetTickCount(); }
	INLINE_ATTR Timer_t Start(Timer_t uVal)			{ return m_uLastTime = uVal; }
	INLINE_ATTR Timer_t GetStartTime()				{ return m_uLastTime; }

	INLINE_ATTR bool CheckDelay(double sec)			{ return CheckDelay_ticks(TicksFromSec(sec)); }
	INLINE_ATTR bool CheckDelay_us(double us)		{ return CheckDelay_ticks(TicksFromUs(us)); }
	INLINE_ATTR bool CheckDelay_ms(double ms)		{ return CheckDelay_ticks(TicksFromMs(ms)); }
	INLINE_ATTR bool CheckDelay_rate(double f)		{ return CheckDelay_ticks(TicksFromFreq(f)); }

	INLINE_ATTR bool CheckInterval(double sec)		{ return CheckInterval_ticks(TicksFromSec(sec)); }
	INLINE_ATTR bool CheckInterval_us(double us)	{ return CheckInterval_ticks(TicksFromUs(us)); }
	INLINE_ATTR bool CheckInterval_ms(double ms)	{ return CheckInterval_ticks(TicksFromMs(ms)); }
	INLINE_ATTR bool CheckInterval_rate(double f)	{ return CheckInterval_ticks(TicksFromFreq(f)); }

	INLINE_ATTR uint GetInterval_us()				{ return UsFromTicks(GetIntervalTicks()); }
	INLINE_ATTR uint GetIntervalReset_us()			{ return UsFromTicks(GetIntervalTicksReset()); }
		
	// These are for action triggered by a missing event, like
	// a watchdog timer. fForceRestart signals event occurred.
	// Timer is not reset until event occurs.
	INLINE_ATTR bool CheckDelay(double sec, bool fForceRestart)
		{ return CheckDelay_ticks(TicksFromSec(sec), fForceRestart); }
	INLINE_ATTR bool CheckDelay_us(double us, bool fForceRestart)
		{ return CheckDelay_ticks(TicksFromUs(us), fForceRestart); }
	INLINE_ATTR bool CheckDelay_ms(double ms, bool fForceRestart)
		{ return CheckDelay_ticks(TicksFromMs(ms), fForceRestart); }
	INLINE_ATTR bool CheckDelay_rate(double f, bool fForceRestart)
		{ return CheckDelay_ticks(TicksFromFreq(f), fForceRestart); }

	// These are for action triggered by an event or a timeout.
	// fForceRestart signals if the event occurred.
	// Example: Receiver sends response after receiving a packet.
	// If incoming packet is late, response is sent anyway.
	// fForceRestart = did receive packet.
	INLINE_ATTR bool CheckInterval(double sec, bool fForceRestart)
		{ return CheckInterval_ticks(TicksFromSec(sec), fForceRestart); }
	INLINE_ATTR bool CheckInterval_us(double us, bool fForceRestart)
		{ return CheckInterval_ticks(TicksFromUs(us), fForceRestart); }
	INLINE_ATTR bool CheckInterval_ms(double ms, bool fForceRestart)
		{ return CheckInterval_ticks(TicksFromMs(ms), fForceRestart); }
	INLINE_ATTR bool CheckInterval_rate(double f, bool fForceRestart)
		{ return CheckInterval_ticks(TicksFromFreq(f), fForceRestart); }
			
public:
	// All of the above are now duplicated with the timer count passed in.
	// This helps when the timer count was also needed in the caller.
		
	INLINE_ATTR bool CheckDelay(double sec, Timer_t time)		{ return CheckDelay_ticks(TicksFromSec(sec), time); }
	INLINE_ATTR bool CheckDelay_us(double us, Timer_t time)		{ return CheckDelay_ticks(TicksFromUs(us), time); }
	INLINE_ATTR bool CheckDelay_ms(double ms, Timer_t time)		{ return CheckDelay_ticks(TicksFromMs(ms), time); }
	INLINE_ATTR bool CheckDelay_rate(double f, Timer_t time)	{ return CheckDelay_ticks(TicksFromFreq(f), time); }

	INLINE_ATTR bool CheckInterval(double sec, Timer_t time)	{ return CheckInterval_ticks(TicksFromSec(sec), time); }
	INLINE_ATTR bool CheckInterval_us(double us, Timer_t time)	{ return CheckInterval_ticks(TicksFromUs(us), time); }
	INLINE_ATTR bool CheckInterval_ms(double ms, Timer_t time)	{ return CheckInterval_ticks(TicksFromMs(ms), time); }
	INLINE_ATTR bool CheckInterval_rate(double f, Timer_t time){ return CheckInterval_ticks(TicksFromFreq(f), time); }
		
	// These are for action triggered by a missing event, like
	// a watchdog timer. fForceRestart signals event occurred.
	// Timer is not reset until event occurs.
	INLINE_ATTR bool CheckDelay(double sec, bool fForceRestart, Timer_t time)
		{ return CheckDelay_ticks(TicksFromSec(sec), fForceRestart, time); }
	INLINE_ATTR bool CheckDelay_us(double us, bool fForceRestart, Timer_t time)
		{ return CheckDelay_ticks(TicksFromUs(us), fForceRestart, time); }
	INLINE_ATTR bool CheckDelay_ms(double ms, bool fForceRestart, Timer_t time)
		{ return CheckDelay_ticks(TicksFromMs(ms), fForceRestart, time); }
	INLINE_ATTR bool CheckDelay_rate(double f, bool fForceRestart, Timer_t time)
		{ return CheckDelay_ticks(TicksFromFreq(f), fForceRestart, time); }

	// These are for action triggered by an event or a timeout.
	// fForceRestart signals if the event occurred.
	// Example: Receiver sends response after receiving a packet.
	// If incoming packet is late, response is sent anyway.
	// fForceRestart = did receive packet.
	INLINE_ATTR bool CheckInterval(double sec, bool fForceRestart, Timer_t time)
		{ return CheckInterval_ticks(TicksFromSec(sec), fForceRestart, time); }
	INLINE_ATTR bool CheckInterval_us(double us, bool fForceRestart, Timer_t time)
		{ return CheckInterval_ticks(TicksFromUs(us), fForceRestart, time); }
	INLINE_ATTR bool CheckInterval_ms(double ms, bool fForceRestart, Timer_t time)
		{ return CheckInterval_ticks(TicksFromMs(ms), fForceRestart, time); }
	INLINE_ATTR bool CheckInterval_rate(double f, bool fForceRestart, Timer_t time)
		{ return CheckInterval_ticks(TicksFromFreq(f), fForceRestart, time); }

	INLINE_ATTR uint GetInterval_us(Timer_t time)		{ return UsFromTicks(GetIntervalTicks(time)); }
	INLINE_ATTR uint GetIntervalReset_us(Timer_t time)	{ return UsFromTicks(GetIntervalTicksReset(time)); }
			
public:
	// Calculate tick count from interval or rate
	INLINE_ATTR static Timer_t TicksFromUs(double us)	{ return lround(us * TimerClockFreq / 1000000); }
	INLINE_ATTR static Timer_t TicksFromMs(double ms)	{ return lround(ms * TimerClockFreq / 1000); }
	INLINE_ATTR static Timer_t TicksFromSec(double sec)	{ return lround(sec * TimerClockFreq); }
	INLINE_ATTR static Timer_t TicksFromFreq(double f)	{ return lround(TimerClockFreq / f); }

	// Integer versions suitable for use at runtime
	INLINE_ATTR static Timer_t TicksFromUs(uint us)		{ return DIV_UINT_RND(us * TimerClockFreq, 1000000); }
	INLINE_ATTR static Timer_t TicksFromUs(int us)		{ return TicksFromUs((uint)us); }
	INLINE_ATTR static Timer_t TicksFromMs(uint ms)		{ return DIV_UINT_RND(ms * TimerClockFreq, 1000); }
	INLINE_ATTR static Timer_t TicksFromMs(int ms)		{ return TicksFromMs((uint)ms); }
	INLINE_ATTR static Timer_t TicksFromFreq(uint f)	{ return DIV_UINT_RND(TimerClockFreq, f); }
	INLINE_ATTR static Timer_t TicksFromFreq(int f)		{ return TicksFromFreq((uint)f); }

public:
	// Calculate loop count for short delays
	INLINE_ATTR static uint ClocksFromNs(double ns)
	{
		// Take ceiling on clock counts
		return (uint)((ns * CpuFreq + 1E9 - 1) / 1E9);
	}
		
	INLINE_ATTR static uint ClocksFromUs(double us)
	{
		// Take ceiling on clock counts
		return (uint)((us * CpuFreq + 1E6 - 1) / 1E6);
	}

	INLINE_ATTR static uint LoopsFromClocks(uint loops)
	{
		// Take ceiling on loop counts
		return (loops - LoopBaseClocks + ClocksPerLoop - 1) / ClocksPerLoop;
	}

	INLINE_ATTR static void ShortDelay_clocks(uint clocks)
	{
		if (clocks >= LoopBaseClocks + ClocksPerLoop)
			DelayLoop(LoopsFromClocks(clocks));
		else
		{
			// Expecting this loop to be unrolled by optimizations
			while (clocks >= 2)
			{
				asm volatile
				(
					"b	1f\n\t"
				"1:\n\t"
				);
				clocks -= 2;
			}
			if (clocks == 1)
				asm volatile ("nop\n\t");
		}
	}
		
public:
	// Get the interval so far
	INLINE_ATTR Timer_t GetIntervalTicks()
	{
		return GetIntervalTicks(GetTickCount());
	}

	INLINE_ATTR Timer_t GetIntervalTicks(Timer_t time)
	{
		return (Timer_t)(time - m_uLastTime);
	}

	INLINE_ATTR Timer_t GetIntervalTicksReset()
	{
		return GetIntervalTicksReset(GetTickCount());
	}

	INLINE_ATTR Timer_t GetIntervalTicksReset(Timer_t time)
	{
		Timer_t		res;

		res = (Timer_t)(time - m_uLastTime);
		m_uLastTime = time;
		return res;
	}

	INLINE_ATTR uint UsFromTicks(Timer_t time)
	{
		return ShiftUintRnd((uint)time * MicrosecPerTickScale, TickShift);
	}

public:
	INLINE_ATTR bool CheckDelay_ticks(Timer_t ticks)
	{
		return CheckDelay_ticks(ticks, GetTickCount());
	}
	
	INLINE_ATTR bool CheckDelay_ticks(Timer_t ticks, Timer_t time)
	{
		return (Timer_t)(time - m_uLastTime) >= ticks;
	}
	
	INLINE_ATTR bool CheckDelay_ticks(Timer_t ticks, bool fForceRestart)
	{
		return CheckDelay_ticks(ticks, fForceRestart, GetTickCount());
	}
	
	INLINE_ATTR bool CheckDelay_ticks(Timer_t ticks, bool fForceRestart, Timer_t time)
	{
		if (fForceRestart)
		{
			m_uLastTime = time;
			return false;
		}
		return (Timer_t)(time - m_uLastTime) >= ticks;
	}
	
	INLINE_ATTR bool CheckInterval_ticks(Timer_t ticks)
	{
		return CheckInterval_ticks(ticks, GetTickCount());
	}
	
	INLINE_ATTR bool CheckInterval_ticks(Timer_t ticks, Timer_t time)
	{
		if ((Timer_t)(time - m_uLastTime) >= ticks)
		{
			m_uLastTime += ticks;
			return true;
		}
		return false;
	}
	
	INLINE_ATTR bool CheckInterval_ticks(Timer_t ticks, bool fForceRestart)
	{
		return CheckInterval_ticks(ticks, fForceRestart, GetTickCount());
	}
	
	INLINE_ATTR bool CheckInterval_ticks(Timer_t ticks, bool fForceRestart, Timer_t time)
	{
		if (fForceRestart)
		{
			m_uLastTime = time;
			return true;
		}
		else if ((Timer_t)(time - m_uLastTime) >= ticks)
		{
			m_uLastTime += ticks;
			return true;
		}
		return false;
	}
	
protected:
	INLINE_ATTR static void DelayLoop(ulong loops)
	{
		if (loops != 0)
		{
			asm volatile
			(
			"1:	 sub	%[cnt], #1\n\t"
				"bne	1b"
			: [cnt] "+l" (loops) );
		}
	}

protected:
	Timer_t	m_uLastTime;	
};


//****************************************************************************
// IntervalTimer_t template
//
// Provides the code to initialize a timer and read its value. Used as a
// stepping stone to IntervalTimer, which combines IntervaTimer_t with
// Define_Timer.

template<bool fIsTcc, int iTc, int iPrescale, int iPrescaleVal>
class IntervalTimer_t
{
public:
	static void Init()
	{
		// Clocking setup. SAM C and SAM D are different
		//
#if	defined(GCLK_PCHCTRL_GEN_GCLK0)	// SAM C
		if (fIsTcc)
		{
			// Enable clock
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC0 << iTc;
			// Clock it with GCLK0
			GCLK->PCHCTRL[TCC0_GCLK_ID + iTc / 2].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
		}
		else
		{
			// Enable clock
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0 << iTc;
			// Clock it with GCLK0
			GCLK->PCHCTRL[TC0_GCLK_ID + iTc / 2].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
		}
#else	// SAM D
		// Enable clock
		PM->APBCMASK.reg |= 1 << (PM_APBCMASK_TCC0_Pos + iTc);

		// Clock it with GCLK0
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
			(GCLK_CLKCTRL_ID_TCC0_TCC1 + iTc / 2);
#endif

		// TC/TCC setup
		//
		if (fIsTcc)
		{
			// Set up counter
			TCC_PTR(iTc)->CTRLA.reg = iPrescaleVal | TCC_CTRLA_PRESCSYNC_PRESC | TCC_CTRLA_ENABLE;
		}
		else
		{
#ifdef TC_READREQ_RCONT	// SAM D TC only
			// Set up automatic synchronization
			TC_PTR(iTc)->READREQ.reg = TC_READREQ_RCONT | offsetof(TcCount16, COUNT);
#endif
			// Set up counter
			TC_PTR(iTc)->CTRLA.reg = iPrescaleVal | TC_CTRLA_PRESCSYNC_PRESC | TC_CTRLA_ENABLE;
		}
	}

	//************************************************************************

	static Timer_t GetTickCount()
#ifndef TC_READREQ_RCONT	// inline it on SAM D TC
	NO_INLINE_ATTR
#endif
	{
		if (fIsTcc)
		{
			// Must synchronize first
			TCC_PTR(iTc)->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
			while (TCC_PTR(iTc)->SYNCBUSY.reg & (TCC_SYNCBUSY_CTRLB | TCC_SYNCBUSY_COUNT));
			return TCC_PTR(iTc)->COUNT.reg;
		}
		else
		{
#ifndef TC_READREQ_RCONT
			// Must synchronize first
			TC_PTR(iTc)->CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
			while (TC_PTR(iTc)->SYNCBUSY.reg & (TC_SYNCBUSY_CTRLB | TC_SYNCBUSY_COUNT));
#endif
			return TC_PTR(iTc)->COUNT.reg;
		}
	}
};


//****************************************************************************
// IntervalTimer template
//
// Combines Define_Timer and IntervalTimer_t to provide the complete timer.
//
// Use the DECLARE_TIMER macro above to create one.

template<bool fIsTcc, int iTc, int iPrescale, int iPrescaleVal>
class IntervalTimer :
	public IntervalTimer_t<fIsTcc, iTc, iPrescale, iPrescaleVal>, 
	public Define_Timer<F_CPU / iPrescale, IntervalTimer_t<fIsTcc, iTc, iPrescale, iPrescaleVal>::GetTickCount>
{
};

#undef TC_PTR
#undef TCC_PTR
