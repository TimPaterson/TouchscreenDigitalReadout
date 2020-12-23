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

EepromData(TouchScale_t, TouchScale, { {200, 18000, 1023, true}, {300, 11000, 599, true}, {200, 0xFFFF} })
EepromData(ulong, Brightness, LcdBacklightPwmMax)
EepromData(AxisInfo, XaxisInfo, { 1.0, 5, 0 })
EepromData(AxisInfo, YaxisInfo, { 1.0, 5, 0 })
EepromData(AxisInfo, ZaxisInfo, { 1.0, 5, 0 })
EepromData(AxisInfo, QaxisInfo, { 1.0, 5, 0 })
EepromData(bool, fIsMetric, false)
EepromData(byte, OriginNum, 0)

// Undefine all the macros now
#undef	EepromData
