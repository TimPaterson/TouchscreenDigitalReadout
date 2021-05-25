//****************************************************************************
// KeyboardDef.h
//
// Created 2/8/2021 9:56:21 AM by Tim
//
//****************************************************************************

#pragma once

// These bits correspond to the indicator lights
enum KeyLocks
{
	KL_NumLock = 0x01,
	KL_CapsLock = 0x02,
	KL_ScrollLock = 0x04,
};

enum KeyModifiers
{
	KM_Lctl = 0x01,
	KM_Lshift = 0x02,
	KM_Lalt = 0x04,
	KM_Lgui = 0x08,
	KM_Rctl = 0x10,
	KM_Rshift = 0x20,
	KM_Ralt = 0x40,
	KM_Rgui = 0x80,

	KM_Ctl = KM_Lctl | KM_Rctl,
	KM_Shift = KM_Lshift | KM_Rshift,
	KM_Alt = KM_Lalt | KM_Ralt,
	KM_Gui = KM_Lgui | KM_Rgui,
};

enum KeySource
{
	KS_Keyboard = 0,	// Main keyboard
	KS_Keypad = 0x100,	// keypad
};

enum ControlKeys
{
	NoKey = 0xFF,
	CD32 = 0,	// code 32 not used on US keyboards
	CR = 0x0D,
	ESC = 0x1B,
	BS = 0x08,
	TAB = 0x09,
	// Function keys are consecutive
	F1 = 0x81,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	PrtSc = 0xA0,
	Pause,
	Ins,
	Home,
	PgUp,
	Del,
	End,
	PgDn,
	Up,
	Dn,
	Lf,
	Rt,

	LockPrefix = 0xF0,
	CapsLk = LockPrefix | KL_CapsLock,
	ScrLk = LockPrefix | KL_ScrollLock,
	NumLk = LockPrefix | KL_NumLock,
};

