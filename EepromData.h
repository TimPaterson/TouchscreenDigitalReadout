//****************************************************************************
// EepromData.h
//
// Created 11/19/2020 10:44:04 AM by Tim
//
//****************************************************************************

// Null out any macros we aren't using
#ifndef EepromData
#define EepromData(typ, name, ...)
#endif

//*********************************************************************
// EEPROM layout
//
// The size and order of these items should never change so the
// EEPROM in a device remains valid.
//
// EeepromData(typ, name, ...)
//
// typ - standard C++ type
// name - member name in struct
// ... - initial value when EEPROM is not programmed
//
//*********************************************************************

EepromData(TouchInfo, TouchInit, { { \
	{TouchPointInvalid, TouchPointInvalid}, \
	{TouchPointInvalid, TouchPointInvalid} }, \
	TouchDefaultMinZ, TouchUpdateRate, TouchInitialDiscard, TouchAverageShift})
EepromData(ulong, Brightness, LcdBacklightPwmMax)
EepromData(AxisInfo, XaxisInfo, { 1.0, 5, false, false })
EepromData(AxisInfo, YaxisInfo, { 1.0, 5, false, false })
EepromData(AxisInfo, ZaxisInfo, { 1.0, 5, false, false })
EepromData(AxisInfo, QaxisInfo, { 1.0, 5, false, true })	// default to disabled
// 32-bit aligned here
EepromData(bool, fIsMetric, false)
EepromData(byte, OriginNum, 0)
EepromData(bool, fHighlightOffset, true)
EepromData(bool, fToolLenAffectsZ, true)
// 32-bit aligned here
EepromData(double, ChipLoad, 0)
EepromData(double, Sfm, 0)
// 32-bit aligned here
EepromData(ushort, MaxRpm, 10000)
EepromData(bool, fCncCoordinates, false)
EepromData(bool, fToolLibMetric, false)
// 32-bit aligned here

// Undefine all the macros now
#undef	EepromData
