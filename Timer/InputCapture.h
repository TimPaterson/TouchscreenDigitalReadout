//****************************************************************************
// InputCapture.h
//
// Created 6/4/2019 10:12:36 AM by Tim
//
// Description:
//
// Input capture is a feature of SAM MCUs that allows a timer to measure
// the period and pulse width on an input pin. This class encapsulates the
// initialization of this feature, which is complicated because it involves
// the External Interrupt Controller and Event System in addition to the
// timer.
//
// *************
// DECLARATION
// *************
// The first step is to use a macro to declare an input capture class based
// on a specific counter:
//
// DECLARE_CAPTURE(counter, prescale)
// - counter: one of TCC0 - TCC2 or TC0 - TC4
// - prescale: a valid prescale divider: 1,2,4,8,16,64,256,1024
//
// The macro actually parses the text of the counter argument, so it can't
// be a macro itself. The prescale argument can be a macro but must resolve
// directly to one of the numeric values -- it can't be an expression.
//
// DECLARE_CAPTURE must be used in a C++ statement that uses the resulting 
// type. In the end you need to declare one variable of this type or of a 
// derived type.
//
// Examples:
//
// DECLARE_CAPTURE(TC1, 4)	CaptureVar;	// variable declaration
// typedef DECLARE_CAPTURE(TCC0, 1) CaptureType;	// type declaration
// class Tachometer : public DECLARE_CAPTURE(TC4, 64) {...}	// class declaration
//
// You may create as many input capture classes as you have timers. Each
// timer is dedicated to the input capture function only.
//
// *************
// INTIALIZATION
// *************
// There are two steps to initializing input capture. The first call is to
// the static function StartClocks(). 
//
// Examples:
//
// InputCapture::StartClocks(); // use base class
// CaptureVar.StartClocks(); // use variable of derived class
//
// This static function is called only once regardless of the number of
// input capture timers used.
//
// For each input capture timer, you call this static member function:
//
// void Init(int iPin, int iExInt, int iEvChan, bool fInvert = false)
// - iPin: port pin number, 0-31 for Port A, 32-63 for Port B, etc.
// - iExInt: external interrupt for that port pin
// - iEvChan: event channel, 0 - 5 or more depending on device
// - fInvert: true to invert the pin value, giving negative pulse width
//
// Note that there are limited external interrupts. Some pins have the same
// one as other pins, and some pins don't have one at all. You get this from
// the "I/O Multiplexing and Considerations" table in the data sheet.
//
// Just pick any existing event channel not used by anything else. It is
// dedicated to that capture pin & counter.
//
// Examples:
//
// CaptureVar.Init(24, 12, 0); // PA24 uses interrupt 12
// CaptureType::Init(0, 0, 1, true); // PA00 uses interrupt 0, invert signal
//
// **********
// INTERRUPTS
// **********
// Input capture must use interrupts. A macro is provided to define the
// interrupt service routine (ISR):
//
// DEFINE_CAPTURE_ISR(ctr, handler)
// - ctr: The same counter used in DECLARE_CAPTURE
// - handler: code or function for ISR body
//
// Inside the ISR, you use the member functions IsrReadPeriod() and 
// IsrReadWidth() to get the results. The interrupt is triggered by the 
// period, so the ISR MUST call IsrReadPeriod() to clear the interrupt 
// even if it doesn't use the value.
//
// Examples:
//
// DEFINE_CAPTURE_ISR(TC4, Tach.CaptureIsr()) // handle in derived member fcn
// DEFINE_CAPTURE_ISR(TC1, CaptVal = CaptureVar.IsrReadPeriod()) // just save in variable
//
//****************************************************************************

#pragma once

#define GET_PRESCALE(fIsTcc, div)	(fIsTcc ? TCC_CTRLA_PRESCALER_DIV##div : TC_CTRLA_PRESCALER_DIV##div)

#define DECLARE_CAPTURE(ctr, div) \
	InputCapture_t<#ctr[2] == 'C', (#ctr[2] == 'C' ? #ctr[3] : #ctr[2]) - '0', div, GET_PRESCALE(#ctr[2] == 'C', div)>

#define DEFINE_CAPTURE_ISR(ctr, ...)	void ctr##_Handler() { __VA_ARGS__; }

class InputCapture
{
public:
	static void StartClocks()
	{
		MCLK->APBCMASK.reg |= MCLK_APBCMASK_EVSYS;
		GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
	}

	static void InitEvent(int iPin, int iExInt, int iEvChan)
	{
		// Send input pin to EIC so it can become event
		SetPortMuxPin(PORT_MUX_A, iPin);
	
		// Set up External Interrupt Controller to get input on EXTINT[iExInt]
		EIC->CTRLA.reg = 0;	// Ensure not enabled so we can make changes
		EIC->CONFIG[iExInt >> 3].reg |= (EIC_CONFIG_SENSE0_HIGH | EIC_CONFIG_FILTEN0) << ((iExInt & 7) * 4);
		EIC->EVCTRL.reg |= (1 << iExInt);
		EIC->CTRLA.reg = EIC_CTRLA_ENABLE;
		
		// Pipe the event on EXINT[iExInt] to event channel iEvChan
		// No clock needed for asynchronous operation
		EVSYS->CHANNEL[iEvChan].reg = EVSYS_CHANNEL_PATH_ASYNCHRONOUS | EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_0 + iExInt);
	}
};

template<bool fIsTcc, int iTc, int iPrescale, int iPrescaleVal>
class InputCapture_t : public InputCapture
{
#define TCC_PTR(i)		((Tcc *)((byte *)TCC0 + ((byte *)TCC1 - (byte *)TCC0) * i))
#define TC_PTR(i)		((TcCount16 *)((byte *)TC0 + ((byte *)TC1 - (byte *)TC0) * i))

#ifdef F_CPU
public:
	static constexpr int CounterFreq = F_CPU / iPrescale;
#endif

public:
	static void Init(int iPin, int iExInt, int iEvChan, bool fInvert = false)
	{
		InitEvent(iPin, iExInt, iEvChan);
		
		// Set up counter
		if (fIsTcc)
		{
			int iEvUser = iTc == 0 ? EVSYS_ID_USER_TCC0_EV_1 : (iTc == 1 ? EVSYS_ID_USER_TCC1_EV_1 : EVSYS_ID_USER_TCC2_EV_1);
			EVSYS->USER[iEvUser].reg = iEvChan + 1;	// channel n-1 selected
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC0 << iTc;
			GCLK->PCHCTRL[TCC0_GCLK_ID + iTc / 2].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
			TCC_PTR(iTc)->EVCTRL.reg = TCC_EVCTRL_TCEI1 | TCC_EVCTRL_EVACT1_PPW | TCC_EVCTRL_MCEI0 | (fInvert ? TCC_EVCTRL_TCINV1 : 0);
			TCC_PTR(iTc)->INTENSET.reg = TCC_INTENSET_MC0;
			TCC_PTR(iTc)->CTRLA.reg = iPrescaleVal | TCC_CTRLA_CPTEN0 | TCC_CTRLA_CPTEN1 | TCC_CTRLA_PRESCSYNC_PRESC | TCC_CTRLA_ENABLE;
			NVIC_EnableIRQ((IRQn)(TCC0_IRQn + iTc));
		}
		else
		{
			EVSYS->USER[EVSYS_ID_USER_TC0_EVU + iTc].reg = iEvChan + 1;	// channel n-1 selected
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0 << iTc;
			GCLK->PCHCTRL[TC0_GCLK_ID + iTc / 2].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
			TC_PTR(iTc)->EVCTRL.reg = TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_PPW | (fInvert ? TC_EVCTRL_TCINV : 0);
			TC_PTR(iTc)->INTENSET.reg = TC_INTENSET_MC0;
			TC_PTR(iTc)->CTRLA.reg = iPrescaleVal | TC_CTRLA_PRESCSYNC_PRESC | TC_CTRLA_CAPTEN0 | TC_CTRLA_CAPTEN1 | TC_CTRLA_ENABLE;
			NVIC_EnableIRQ((IRQn)(TC0_IRQn + iTc));
		}
	}
	
	static uint IsrReadPeriod()
	{
		return fIsTcc ? TCC_PTR(iTc)->CC[0].reg : TC_PTR(iTc)->CC[0].reg;
	}
	
	static uint IsrReadWidth()
	{
		return fIsTcc ? TCC_PTR(iTc)->CC[1].reg : TC_PTR(iTc)->CC[1].reg;
	}
};
