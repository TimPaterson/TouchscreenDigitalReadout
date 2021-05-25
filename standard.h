//****************************************************************************
// standard.h
//
// Created 3/7/2016 7:31:18 AM by Tim
//
//****************************************************************************

#pragma once
#pragma pack(4)

#include <sam.h>
#include <sam_spec.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Standard data types
typedef	uint8_t			byte;
typedef	int8_t			sbyte;
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

// Common macros
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif
#define STRLEN(str)		(sizeof str - 1)	// characters in string literal
#define ADDOFFSET(p, o)	(((byte *)(p)) + (o))
#define NO_INLINE_ATTR	__attribute__((noinline))
#define INLINE_ATTR		__attribute__((always_inline))
#define DEPRECATED_ATTR	__attribute__((deprecated))
#define PACKED_ATTR		__attribute__((packed))
#define ALIGNED_ATTR(typ)	__attribute__((aligned(sizeof(typ))))
#define RAMFUNC_ATTR	__attribute__ ((section(".ramfunc")))
#define NAKED_ATTR		__attribute__ ((naked))
#define VERSION_INFO	__attribute__ ((section(".version_info")))
#define LOBYTE(w)       ((byte)(w))
#define HIBYTE(w)       ((byte)((ushort)(w) >> 8))
#define DIV_UINT_RND(x, y)	(((x) + (y) / 2) / (y))	// use this macro in constants
#define CONCAT_(x,y)	x##y
#define CONCAT(x,y)		CONCAT_(x,y)
#define CAT3_(x,y,z)	x##y##z
#define CAT3(x,y,z)		CAT3_(x,y,z)
#define STRINGIFY_(x)	#x
#define STRINGIFY(x)	STRINGIFY_(x)
#define LOG2(x)			(31 - __builtin_clz(x))

inline bool CompSign(int s1, int s2)		{ return (s1 ^ s2) >= 0; }

// Rounded integer division/shifting
inline int ShiftIntRnd(int n, int s)		{ return ((n >> (s - 1)) + 1) >> 1; }
inline uint ShiftUintRnd(uint n, int s)		{ return ((n >> (s - 1)) + 1) >> 1; }
inline uint DivUintRnd(uint n, uint d)		{ return DIV_UINT_RND(n, d); }
inline int DivIntByUintRnd(int n, uint d)
{ 
	int sgn = n >> (sizeof(n)*8-1);	// 0 or -1
	return (n + (int)(((d / 2) ^ sgn) - sgn)) / (int)d; 
}
inline int DivIntRnd(int n, int d)		
{ 
	int rnd = d / 2;
	return (n + ((n ^ d) < 0 ? -rnd : rnd)) / d; 
}

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#ifdef WDT_STATUS_SYNCBUSY
inline void wdt_reset() {if (!WDT->STATUS.bit.SYNCBUSY) WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;}
#else
inline void wdt_reset() {if (!WDT->SYNCBUSY.bit.CLEAR) WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;}
#endif

typedef union
{
	ulong	ul;
	long	l;
	float	flt;
	byte	arb[4];
	ushort	arus[2];
	struct
	{
		ushort	uLo16;
		ushort	uHi16;
	};
	struct
	{
		byte	bLo;
		byte	bMidLo;
		byte	bMidHi;
		byte	bHi;
	};
} LONG_BYTES;

#ifdef __cplusplus

//*********************************************************************
// Bit I/O helpers

static constexpr ulong ALL_PORT_PINS = 0xFFFFFFFF;

// For any port using port number (0 = PORTA, etc.)
// Output
inline void SetPins(uint pins, int iPort)		{ PORT_IOBUS->Group[iPort].OUTSET.reg = pins; }
inline void ClearPins(uint pins, int iPort)		{ PORT_IOBUS->Group[iPort].OUTCLR.reg = pins; }
inline void TogglePins(uint pins, int iPort)	{ PORT_IOBUS->Group[iPort].OUTTGL.reg = pins; }
inline void WritePins(uint pins, int iPort)		{ PORT_IOBUS->Group[iPort].OUT.reg = pins; }
inline uint GetOutPins(int iPort)				{ return PORT_IOBUS->Group[iPort].OUT.reg; }

// Direction
inline void DirOutPins(uint pins, int iPort)	{ PORT_IOBUS->Group[iPort].DIRSET.reg = pins; }
inline void DirInPins(uint pins, int iPort)		{ PORT_IOBUS->Group[iPort].DIRCLR.reg = pins; }
inline void DirTglPins(uint pins, int iPort)	{ PORT_IOBUS->Group[iPort].DIRTGL.reg = pins; }
inline void DirWritePins(uint pins, int iPort)	{ PORT_IOBUS->Group[iPort].DIR.reg = pins; }
inline uint GetDirPins(int iPort)				{ return PORT_IOBUS->Group[iPort].DIR.reg; }

// Input
inline uint GetPins(int iPort)					{ return PORT_IOBUS->Group[iPort].IN.reg; }
inline uint GetPins(uint pins, int iPort)		{ return PORT_IOBUS->Group[iPort].IN.reg & pins; }

// For PORTA and PORTB explicitly
// Output
inline void SetPinsA(uint pins)			{ SetPins(pins, 0); }
inline void SetPinsB(uint pins)			{ SetPins(pins, 1); }
inline void ClearPinsA(uint pins)		{ ClearPins(pins, 0); }
inline void ClearPinsB(uint pins)		{ ClearPins(pins, 1); }
inline void TogglePinsA(uint pins)		{ TogglePins(pins, 0); }
inline void TogglePinsB(uint pins)		{ TogglePins(pins, 1); }
inline void WritePinsA(uint pins)		{ WritePins(pins, 0); }
inline void WritePinsB(uint pins)		{ WritePins(pins, 1); }
inline uint GetOutPinsA()				{ return GetOutPins(0); }
inline uint GetOutPinsB()				{ return GetOutPins(1); }

// Access as type LONG_BYTES above for individual byte access
#define PortSetA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].OUTSET.reg)
#define PortSetB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].OUTSET.reg)
#define PortClearA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].OUTCLR.reg)
#define PortClearB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].OUTCLR.reg)
#define PortToggleA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].OUTTGL.reg)
#define PortToggleB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].OUTTGL.reg)
#define PortWriteA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].OUT.reg)
#define PortWriteB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].OUT.reg)

// Direction
inline void DirOutPinsA(uint pins)		{ DirOutPins(pins, 0); }
inline void DirOutPinsB(uint pins)		{ DirOutPins(pins, 1); }
inline void DirInPinsA(uint pins)		{ DirInPins(pins, 0); }
inline void DirInPinsB(uint pins)		{ DirInPins(pins, 1); }
inline void DirTglPinsA(uint pins)		{ DirTglPins(pins, 0); }
inline void DirTglPinsB(uint pins)		{ DirTglPins(pins, 1); }
inline void DirWritePinsA(uint pins)	{ DirWritePins(pins, 0); }
inline void DirWritePinsB(uint pins)	{ DirWritePins(pins, 1); }
inline uint GetDirPinsA()				{ return GetDirPins(0); }
inline uint GetDirPinsB()				{ return GetDirPins(1); }

// Access as type LONG_BYTES above for individual byte access
#define PortDirOutA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].DIRSET.reg)
#define PortDirOutB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].DIRSET.reg)
#define PortDirInA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].DIRCLR.reg)
#define PortDirInB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].DIRCLR.reg)
#define PortDirTglA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].DIRTGL.reg)
#define PortDirTglB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].DIRTGL.reg)
#define PortDirWriteA	(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].DIR.reg)
#define PortDirWriteB	(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].DIR.reg)

// Input
inline uint GetPinsA(uint pins)			{ return GetPins(pins, 0); }
inline uint GetPinsB(uint pins)			{ return GetPins(pins, 1); }
inline uint GetPinsA()					{ return GetPins(0); }
inline uint GetPinsB()					{ return GetPins(1); }

// Access as type LONG_BYTES above for individual byte access
#define PortInA		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[0].IN.reg)
#define PortInB		(*(volatile LONG_BYTES *)&PORT_IOBUS->Group[1].IN.reg)

//*********************************************************************
// Helper to use 8-bit access

inline void WriteByteOfReg32(volatile void *pv, uint val, ulong mask = 0)
{
	int		pos;

	mask |= val;
	pos = __builtin_ctzl(mask) / 8;
	if (__builtin_clzl(mask) / 8 == 3 - pos)
		((volatile byte *)pv)[pos] = val >> pos * 8;
	else
		*(volatile ulong *)pv = val;
}

//*********************************************************************
// Helpers to set up port configuration

inline void SetPortConfig(uint uConfig, uint uPins, int iPort)
{
	if (uPins & 0xFFFF)
	{
		// Set pins in lo 16 bits
		PORT->Group[iPort].WRCONFIG.reg =
			uConfig | 
			PORT_WRCONFIG_WRPINCFG | 
			PORT_WRCONFIG_PINMASK(uPins & 0xFFFF);
	}
	
	if (uPins > 0xFFFF)
	{
		// Set pins in hi 16 bits
		PORT->Group[iPort].WRCONFIG.reg =
			uConfig |
			PORT_WRCONFIG_WRPINCFG | 
			PORT_WRCONFIG_PINMASK(uPins >> 16) |
			PORT_WRCONFIG_HWSEL;
	}
}

inline void SetPortConfigA(uint uConfig, uint uPins) { SetPortConfig(uConfig, uPins, 0); }
inline void SetPortConfigB(uint uConfig, uint uPins) { SetPortConfig(uConfig, uPins, 1); }

inline void SetPortMux(uint uMux, uint uPins, int iPort)
{
	SetPortConfig(		
		PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(uMux) |
		PORT_WRCONFIG_INEN |
		PORT_WRCONFIG_PMUXEN,
		uPins,
		iPort
	);
}

inline void SetPortMuxA(uint uMux, uint uPins)	{ SetPortMux(uMux, uPins, 0); }
inline void SetPortMuxB(uint uMux, uint uPins)	{ SetPortMux(uMux, uPins, 1); }

inline void SetPortMuxPin(uint uMux, uint uPin)
{
	SetPortConfig(		
		PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(uMux) |
		PORT_WRCONFIG_INEN |
		PORT_WRCONFIG_PMUXEN,
		1 << (uPin & 0x1F),
		uPin >> 5
	);
}
#endif

enum PORT_MUX
{
	PORT_MUX_A,
	PORT_MUX_B,
	PORT_MUX_C,
	PORT_MUX_D,
	PORT_MUX_E,
	PORT_MUX_F,
	PORT_MUX_G,
	PORT_MUX_H,
	PORT_MUX_I
};