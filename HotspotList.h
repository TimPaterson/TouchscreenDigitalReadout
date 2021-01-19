//****************************************************************************
// HotspotList.h
//
// Created 11/7/2020 12:38:35 PM by Tim
//
//****************************************************************************

#pragma once

#include "Hotspot.h"


//*************************************************************************
// Define the tacked-on characters in each CharSet

#define START_CHARSET(name)			enum CharValues_name {
#define DEFINE_CHAR(name, chr, val)	chr = val,
#define END_CHARSET(name)			};

#include "Fonts/Fonts.h"

//*************************************************************************
// Define the hotspot values for action keys on main screen. 

enum ActionHotspots
{
	Mem1,
	Mem2,
	Mem3,
	Mem4,
	Mem5,
	Key_sign,
	Key_backSpace,
	Key_clear,
	Key_pi,
	ToolNumber,
	ToolFlutes,
	ToolSfm,
	ToolDiameter,
	ToolLength,
	ToolChipLoad,
	HighlightXY,
	OffsetZ,

	Key_mult = CH_Multiply,
	Key_divide = CH_Divide,
};

// Index into array of PosSensor
enum SensorIndex
{
	Xsensor,
	Ysensor,
	Zsensor,
	Qsensor
};

enum ToolSides
{
	ToolMaskShift = 4,
	// Bit flags
	ToolSidesNone,
	ToolLeftBit	= 1,
	ToolRightBit= 2,
	ToolBackBit	= 4,
	ToolFrontBit= 8,

	ToolLeftMask	= ToolLeftBit << ToolMaskShift,
	ToolRightMask	= ToolRightBit << ToolMaskShift,
	ToolBackMask	= ToolBackBit << ToolMaskShift,
	ToolFrontMask	= ToolFrontBit << ToolMaskShift,


	ToolLeft	= ToolLeftBit | ToolRightMask,
	ToolRight	= ToolRightBit | ToolLeftMask,
	ToolBack	= ToolBackBit | ToolFrontMask,
	ToolFront	= ToolFrontBit | ToolBackMask,
};

//*************************************************************************
// Define the hotspot values for keyboard keys, just assigning them
// their character code.

enum KeyboardHotspots
{
	Key_1 = '1',
	Key_2 = '2',
	Key_3 = '3',
	Key_4 = '4',
	Key_5 = '5',
	Key_6 = '6',
	Key_7 = '7',
	Key_8 = '8',
	Key_9 = '9',
	Key_0 = '0',

	Key_q = 'q',
	Key_w = 'w',
	Key_e = 'e',
	Key_r = 'r',
	Key_t = 't',
	Key_y = 'y',
	Key_u = 'u',
	Key_i = 'i',
	Key_o = 'o',
	Key_p = 'p',
	Key_a = 'a',
	Key_s = 's',
	Key_d = 'd',
	Key_f = 'f',
	Key_g = 'g',
	Key_h = 'h',
	Key_j = 'j',
	Key_k = 'k',
	Key_l = 'l',
	Key_z = 'z',
	Key_x = 'x',
	Key_c = 'c',
	Key_v = 'v',
	Key_b = 'b',
	Key_n = 'n',
	Key_m = 'm',

	Key_Q = 'Q',
	Key_W = 'W',
	Key_E = 'E',
	Key_R = 'R',
	Key_T = 'T',
	Key_Y = 'Y',
	Key_U = 'U',
	Key_I = 'I',
	Key_O = 'O',
	Key_P = 'P',
	Key_A = 'A',
	Key_S = 'S',
	Key_D = 'D',
	Key_F = 'F',
	Key_G = 'G',
	Key_H = 'H',
	Key_J = 'J',
	Key_K = 'K',
	Key_L = 'L',
	Key_Z = 'Z',
	Key_X = 'X',
	Key_C = 'C',
	Key_V = 'V',
	Key_B = 'B',
	Key_N = 'N',
	Key_M = 'M',

	Key_Space = ' ',
	Key_semi = ';',
	Key_comma = ',',
	Key_period = '.',
	Key_decimal = Key_period,
	Key_plus = '+',
	Key_minus = '-',
	Key_equal = '=',

	Key_shift = -1,
};

//*************************************************************************
// Define the hotspot groups

#define START_GROUP(group)	HOTSPOT_GROUP_##group,

enum HotspotGroups
{
	#include "Images/Screen.h"
};

//*************************************************************************
// Define the hotspot values for all keys not in the above groups

#define GROUP_HOTSPOT_(name, group, x1, y1, x2, y2)	name,

enum Hotspots
{
	#include "Images/Screen.h"
};

//*************************************************************************
// Hotspot lists will be defined in C due to its superior initialization
// of flexible arrays. Declare the external references.

#ifdef __cplusplus
extern "C"
{

#define START_HOTSPOTS(name) extern HotspotList name##HotspotList;

#include "Images/Screen.h"

//*************************************************************************
// Declare the screens & areas

#include "TouchCanvas.h"

// Define a struct of areas for each screen
#define START_AREAS(name)					struct name##_Areas_t {
#define DEFINE_AREA(name, x1, y1, x2, y2)	Area name;
#define END_AREAS(name)						};

#include "Images/Screen.h"

// Define the externs
#define START_SCREEN(name)			extern TouchCanvas name;
#define START_SCREEN_Overlay(name)	extern const ColorImage name;
#define START_AREAS(name)			extern const name##_Areas_t name##_Areas;

#include "Images/Screen.h"

}
#endif	// __cplusplus

//*************************************************************************
// Define the list of locations

#define DEFINE_LOCATION(name, x, y)	name##_X = x, name##_Y = y,

enum Locations
{
	#include "Images/Screen.h"
};

//*************************************************************************
// Define colors

#define DEFINE_COLOR(name, color)	name = color,

enum Colors
{
	#include "Images/Screen.h"
};

//*************************************************************************
// Define values

#define DEFINE_VALUE(name, val)	static const int name = val;

#include "Images/Screen.h"
