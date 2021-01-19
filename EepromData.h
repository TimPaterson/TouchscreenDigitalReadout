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
// 32-bit aligned here
EepromData(bool, fIsMetric, false)
EepromData(byte, OriginNum, 0)
EepromData(bool, fHighlightOffset, true)
EepromData(bool, fToolLenAffectsZ, true)
// 32-bit aligned here
EepromData(double, ChipLoad, 0)
EepromData(ushort, Sfm, 0)
EepromData(ushort, MaxRpm, 10000)
// 32-bit aligned here
EepromData(double, ToolDiameter, 0)
EepromData(double, ToolLength, 0)
// 32-bit aligned here
EepromData(ushort, ToolNumber, 0)
EepromData(byte, ToolFlutes, 0)

// Undefine all the macros now
#undef	EepromData
