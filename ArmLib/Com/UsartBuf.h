//****************************************************************************
// UsartBuf.h
//
// Description:
//
// These classes provide interrupt-driven buffered I/O for the USART (SERCOM)
// modules. You specify the transmit and receive buffer sizes, which can be any
// size. There is also an option for half-duplex operation using user-provided
// routines to turn external drivers on and off.
//
// *************
// DECLARATION
// *************
// The first step is to use a macro to declare buffered USART class based
// on a SERCOM:
//
// DECLARE_USART(sercom, inbuf, outbuf)
// - sercom: one of SERCOM0 - SERCOMn
// - inbuf: size of the receive buffer
// - output: size of the transmit buffer
//
// The macro actually parses the text of the sercom argument, so it can't
// be a macro itself. 
//
// DECLARE_USART must be used in a C++ statement that uses the resulting 
// type. In the end you need to declare one variable of this type or of a 
// derived type.
//
// Examples:
//
// DECLARE_USART(SERCOM1, 50, 50)	SerialVar;	// variable declaration
// typedef DECLARE_USART(SERCOM0, 10, 100) SerialType;	// type declaration
// class DerivedSerial : public DECLARE_USART(SERCOM4, 25, 25) {...}	// class declaration
//
// You may create as many buffered USART classes as the MCU has SERCOM modules.
//
// *************
// INTIALIZATION
// *************
// You must set up the PORT mux to assign the physical serial I/O pins to the 
// SERCOM module per the data sheet section "I/O Multiplexing and Considerations".
// This is done with this function:
//
// void SetPortMux(uint uMux, uint uPins, int iPort)
// - uMux: mux identifier, one of PORT_MUX_A, PORT_MUX_B, etc
// - uPins: a bit set for each pin to be affected
// - iPort: optional port number (0 = PORTA, 1 = PORTB, etc.)
//
// Example:
//
// SetPortMuxA(PORT_MUX_C, SerialTxPin | SerialRxPin, 0);
//
// Typically the SERCOM modules are on mux C and mux D. Each SERCOM module
// has four pads, and a given combination of mux and pin assign that pin 
// to a specific pad. Pads can be assigned to different functions. For 
// example, any pad can be assigned as receive data, but only some can be 
// assigned transmit data. Assignment is done with this member function:
//
// void Init(RxPad padRx, TxPad padTx)
// - padRx: one of RXPAD_Pad0, RXPAD_Pad1, RXPAD_Pad2, RXPAD_Pad3
// - padTx: one of TXPAD_Pad0, TXPAD_Pad2, TXPAD_Pad0_RTS_Pad2_CTS_Pad3, TXPAD_Pad0_TE_Pad2
//
// This function enables the SERCOM clock and assigns the functions of the
// pads. TXPAD_Pad0_RTS_Pad2_CTS_Pad3 enables hardware handshaking using
// RTS and CTS. TXPAD_Pad0_TE_Pad2 enables hardware control of an output
// driver for half-duplex applications (TE = "transmit enable"); this
// function is not available on all devices (use DECLARE_USART_HALF in 
// this case, see below).
//
// This function also sets up standard asynchronous serial parameters
// of LSB first, 8 data bits, 1 stop bit, and no parity.
//
// Example:
//
// SerialVar.Init(RXPAD_Pad3, TXPAD_Pad2);
//
// There are several member functions for setting baud rate:
//
//	void SetBaudRate(uint rate, uint clock)
//	void SetBaudRate(uint rate)
// 	void SetBaudRateConst(uint rate, uint clock)
//	void SetBaudRateConst(uint rate)
// - rate: desired baud rate in bits per second
// - clock: CPU clock rate
//
// Functions without the 'clock' parameter assume the presence of the symbol
// F_CPU which is defined to be the CPU clock speed. If used, it must be
// defined before this header file is included.
//
// Example:
//
// #define F_CPU 48000000	// 48 MHz clock
//
// Or better still, define it in a command-line preprocessor symbol.
//
// SetBaudRateConst() should ONLY be used with a constant for the baud rate.
// The calculations use 64-bit integers which disappear with compiler
// constant folding.
//
// SetBaudRate() is used with a variable baud rate. It includes its own
// optimized math to avoid bringing in runtime routines.
//
// Examples:
//
// SerialVar.SetBaudRateConst(19200);	// set to 19,200 baud - F_CPU set
// SerialVar.SetBaudRate(baud, 2000000);	// variable baud, 2MHz CPU clock
// 
// Finally, to enable the USART use this member function:
//
// void Enable()
//
// Example:
//
// SerialVar.Enable();
//
// At this point the USART is fully operational. Baud rate cannot be changed
// while the USART is enabled.
//
// **********
// INTERRUPTS
// **********
// Each buffered USART must use interrupts. A macro is provided to define the
// interrupt service routine (ISR):
//
// DEFINE_USART_ISR(sercom, var)
// - sercom: The same SERCOM used in DECLARE_USART
// - var: a variable declared as a buffered USART
//
// Example:
//
// DEFINE_USART_ISR(SERCOM1, SerialVar)
//
// ***********
// HALF_DUPLEX
// ***********
// If your SAM MCU supports hardware transmitter enable, then it will control 
// the transmitter driver in hardware using the correct pad assignment.
//
// If your SAM MCU does NOT support hardware transmitter enable, then an
// alternate macro is available: DECLARE_USART_HALF. It requires that
// two functions be defined that enable and disable the transmitter driver.
// The function names are fixed as SERCOMn_DriverOn() and SERCOMn_DriverOff(),
// where the 'n' is replaced with the SERCOM number. These functions must
// be defined before the macro is called.
//
// Example:
//
// inline void SERCOM1_DriverOn() {	SetPins(Driver_PIN); }
// inline void SERCOM1_DriverOff(){	ClearPins(Driver_PIN); }
// DECLARE_USART_HALF(SERCOM1, 50, 50)	SerialHalfDuplex;
// 
// Otherwise everything else about DECLARE_USART applies to DECLARE_USART_HALF.
//
// *********
// INTERFACE
// *********
// void Enable()
// void Disable()
// void WriteByte(byte b)			
// void WriteBytes(void *pv, int cb)
// void WriteString(const char *psz)
// bool IsByteReady()
// byte ReadByte()					
// void ReadBytes(void *pv, int cb)	
// byte ReadByteWdr()				
// byte PeekByte()					
// byte PeekByte(int off)			
// int BytesCanWrite()				
// int BytesCanRead()				
// bool CanWriteByte()				
// void DiscardReadBuf()
// void DiscardReadBuf(int cnt)
//
//****************************************************************************

#pragma once

#include <Com/IoBuf.h>
#include <stdarg.h>

typedef void (*UsartHalfDuplexDriver_t)();

#define SERCOM_ASYNC_BAUD(baud, clock)	((uint16_t)-(((uint32_t)(((uint64_t)baud * (65536 * 16 * 2)) / clock) + 1) / 2))

const int SERCOM_SIZE = (byte *)SERCOM1 - (byte *)SERCOM0;

// Declare full-duplex version
// Read the SERCOM number as the last character of the name string (SERCOMn)
#define DECLARE_USART(usart, inbuf, outbuf) \
	UsartBuf<#usart[6] - '0', inbuf, outbuf>

// Declare half-duplex version.
// These add calls to turn output driver on and off in the ISR
#define DECLARE_USART_HALF(usart, inbuf, outbuf) \
	UsartBufHalf<#usart[6] - '0', inbuf, outbuf, &usart##_DriverOff, &usart##_DriverOn>

// Define ISR
#define DEFINE_USART_ISR(usart, var) \
	void usart##_Handler() {var.UsartIsr();}
		
//****************************************************************************

enum RxPad
{
	RXPAD_Pad0,
	RXPAD_Pad1,
	RXPAD_Pad2,
	RXPAD_Pad3
};

enum TxPad
{
	TXPAD_Pad0,
	TXPAD_Pad2,
	TXPAD_Pad0_RTS_Pad2_CTS_Pad3,
	TXPAD_Pad0_TE_Pad2	// SAM C only
};

//****************************************************************************

class UsartBuf_t : public IoBuf
{
public:
	//************************************************************************
	// These methods override methods in IoBuf.h

protected:
	// We need our WriteByte to enable interrupts
	void WriteByteInline(byte b)
	{
		IoBuf::WriteByteInline(b);
		// Enable interrupts
		GetUsart()->INTENSET.reg = SERCOM_USART_INTENCLR_DRE;
	}

public:
	// We need our WriteByte to enable interrupts
	void WriteByte(byte b) NO_INLINE_ATTR
	{
		WriteByteInline(b);
	}

	void PutByte(byte b)	{ WriteByte(b); }

	// We need our own WriteString to use our own WriteByte
	void WriteString(const char *psz) NO_INLINE_ATTR
	{
		char	ch;

		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			if (ch == '\n')
				WriteByte('\r');
			WriteByte(ch);
		}
	}

	// We need our own WriteBytes to use our own WriteByte
	void WriteBytes(void *pv, int cb) NO_INLINE_ATTR
	{
		byte	*pb;

		for (pb = (byte *)pv; cb > 0; cb--, pb++)
			WriteByte(*pb);
	}

	void WriteBytes(int cb, ...) NO_INLINE_ATTR
	{
		va_list	args;
		va_start(args, cb);
		for ( ; cb > 0; cb--)
			WriteByte(va_arg(args, int));
		va_end(args);
	}

	//************************************************************************
	// These are additional methods

protected:
	void Init(RxPad padRx, TxPad padTx, int iUsart)
	{
		SERCOM_USART_CTRLA_Type	serCtrlA;

#if	defined(GCLK_PCHCTRL_GEN_GCLK0)
		int		iGclkId;

		// Enable clock
		MCLK->APBCMASK.reg |= 1 << (MCLK_APBCMASK_SERCOM0_Pos + iUsart);

		// Clock it with GCLK0
#ifdef SERCOM5_GCLK_ID_CORE
		iGclkId  = iUsart == 5 ? SERCOM5_GCLK_ID_CORE : SERCOM0_GCLK_ID_CORE + iUsart;
#else
		iGclkId  = SERCOM0_GCLK_ID_CORE + iUsart;
#endif
		GCLK->PCHCTRL[iGclkId].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
#else
		// Enable clock
		PM->APBCMASK.reg |= 1 << (PM_APBCMASK_SERCOM0_Pos + iUsart);

		// Clock it with GCLK0
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
			(GCLK_CLKCTRL_ID_SERCOM0_CORE + iUsart);
#endif

		// standard 8,N,1 parameters
		serCtrlA.reg = 0;
		serCtrlA.bit.DORD = 1;		// LSB first
		serCtrlA.bit.MODE = 1;		// internal clock
		serCtrlA.bit.RXPO = padRx;
		serCtrlA.bit.TXPO = padTx;
		GetUsart()->CTRLA.reg = serCtrlA.reg;
		GetUsart()->CTRLB.reg = SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_RXEN;
	}

	void Enable(int iUsart)
	{
		GetUsart()->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
		GetUsart()->CTRLA.bit.ENABLE = 1;
		NVIC_EnableIRQ((IRQn)(SERCOM0_IRQn + iUsart));
	}

	void Disable(int iUsart)
	{
		NVIC_DisableIRQ((IRQn)(SERCOM0_IRQn + iUsart));
		GetUsart()->CTRLA.bit.ENABLE = 0;
		GetUsart()->INTENCLR.reg = SERCOM_USART_INTFLAG_RXC | SERCOM_USART_INTFLAG_DRE | SERCOM_USART_INTFLAG_TXC;
	}

public:
	uint32_t IsEnabled()
	{
		return GetUsart()->CTRLA.reg & SERCOM_USART_CTRLA_ENABLE;
	}

	void SetBaudReg(uint16_t rate)
	{
		GetUsart()->BAUD.reg = rate;
	}

	void SetBaudRate(uint32_t rate, uint32_t clock) NO_INLINE_ATTR
	{
		uint32_t	quo;
		uint32_t	quoBit;

		rate *= 16;		// actual clock frequency
		// Need 17-bit result of rate / clock
		for (quo = 0, quoBit = 1 << 16; quoBit != 0; quoBit >>= 1)
		{
			if (rate >= clock)
			{
				rate -= clock;
				quo |= quoBit;
			}
			rate <<= 1;
		}
		// Round
		if (rate >= clock)
			quo++;
		SetBaudReg((uint16_t)-quo);
	}

#ifdef F_CPU
	void SetBaudRate(uint32_t rate)
	{
		SetBaudRate(rate, F_CPU);
	}

	void SetBaudRateConst(uint32_t rate)
	{
		SetBaudRateConst(rate, F_CPU);
	}
#endif

	void SetBaudRateConst(uint32_t rate, uint32_t clock)
	{
		SetBaudReg(SERCOM_ASYNC_BAUD(rate, clock));
	}

	byte IsXmitInProgress()
	{
		return GetUsart()->INTENCLR.reg & SERCOM_USART_INTFLAG_DRE;
	}

	void StreamInit(FILE *pFile)
	{
		union {void (UsartBuf_t::*mf)(byte); _fdev_put_t *p;} u = {&UsartBuf_t::WriteByte};
		fdev_setup_stream(pFile, u.p, NULL, _FDEV_SETUP_WRITE | _FDEV_SETUP_CRLF);
		fdev_set_udata(pFile, this);
	}

	// Write directly to the USART data port. Very fast output,
	// useful for debugging.
	void DirectWriteByte(byte b)
	{
		GetUsart()->DATA.reg = b;
	}

	void WriteByteNoBuf(byte b)
	{
		while (!GetUsart()->INTFLAG.bit.DRE);
		DirectWriteByte(b);
	}

	void WriteStringNoBuf(const char *psz)
	{
		char	ch;

		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			WriteByteNoBuf(ch);
		}
	}

	SercomUsart *GetUsart()		{return (SercomUsart *)m_pvIO;}
};

//****************************************************************************

template <int iUsart, int cbRcvBuf, int cbXmitBuf> 
class UsartBuf : public UsartBuf_t
{
public:
	static const int RcvBufSize = cbRcvBuf - 1;
	static const int XmitBufSize = cbXmitBuf - 1;

public:
	UsartBuf()
	{
		IoBuf::Init(cbRcvBuf, cbXmitBuf);
		m_pvIO = (SercomUsart *)((byte *)SERCOM0 + iUsart * SERCOM_SIZE);
	}

	void Init(RxPad padRx, TxPad padTx)
	{
		UsartBuf_t::Init(padRx, padTx, iUsart);
	}

	void Enable()
	{
		UsartBuf_t::Enable(iUsart);
	}

	void Disable()
	{
		UsartBuf_t::Disable(iUsart);
	}

	//************************************************************************
	// Interrupt service routine

public:
	void UsartIsr()
	{
		SercomUsart *pUsart = GetUsart();

		if (pUsart->INTFLAG.bit.RXC)
			ReceiveByte(pUsart->DATA.reg);

		if (pUsart->INTFLAG.bit.DRE && pUsart->INTENCLR.bit.DRE)
		{
			if (IsByteToSend())
				pUsart->DATA.reg = SendByte();
			else
				pUsart->INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
		}
	}

protected:
	byte	reserved[cbRcvBuf];
	byte	m_arbXmitBuf[cbXmitBuf];
};

//****************************************************************************
// Std I/O

template <class Base, int cbBuf> 
class StdIo : public Base
{
public:
	void printf(const char *__fmt, ...)
	{
		va_list	args;
		va_start(args, __fmt);
		vsnprintf(m_archPrintBuf, cbBuf - 1, __fmt, args);
		va_end(args);
		this->WriteString(m_archPrintBuf);
	}

protected:
	char m_archPrintBuf[cbBuf];
};

//****************************************************************************
// Half-duplex version

template <int iUsart, int cbRcvBuf, int cbXmitBuf, UsartHalfDuplexDriver_t driverOff,
	UsartHalfDuplexDriver_t driverOn> 
class UsartBufHalf : public UsartBuf_t
{
public:
	UsartBufHalf()
	{
		IoBuf::Init(cbRcvBuf, cbXmitBuf);
		m_pvIO = (SercomUsart *)((byte *)SERCOM0 + iUsart * SERCOM_SIZE);
	}

public:
	void Init(RxPad padRx, TxPad padTx)
	{
		UsartBuf_t::Init(padRx, padTx, iUsart);
	}

	void Enable()
	{
		UsartBuf_t::GetUsart()->INTENSET.reg = SERCOM_USART_INTFLAG_TXC;
		UsartBuf_t::Enable(iUsart);
	}

	void Disable()
	{
		UsartBuf_t::Disable(iUsart);
	}

	//************************************************************************
	// Interrupt service routine

public:
	void UsartIsr()
	{
		SercomUsart *pUsart = UsartBuf_t::GetUsart();

		if (pUsart->INTFLAG.bit.RXC)
			UsartBuf_t::ReceiveByte(pUsart->DATA.reg);

		if (pUsart->INTFLAG.bit.DRE && pUsart->INTENCLR.bit.DRE)
		{
			if (UsartBuf_t::IsByteToSend())
			{
				driverOn();
				pUsart->DATA.reg = UsartBuf_t::SendByte();
			}
			else
				pUsart->INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
		}

		if (pUsart->INTFLAG.bit.TXC)
		{
			driverOff();
			pUsart->INTFLAG.reg = SERCOM_USART_INTFLAG_TXC;	//clear TXC
		}
	}

protected:
	byte	reserved[cbRcvBuf];
	byte	m_arbXmitBuf[cbXmitBuf];
};
