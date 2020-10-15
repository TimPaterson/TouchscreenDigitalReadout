//****************************************************************************
// Class PosSensor
// PosSensor.h
//
// Created 10/13/2020 12:57:40 PM by Tim
//
//****************************************************************************

#pragma once


class PosSensor
{
public:
	void InputChange(uint uSignal);

public:
	int GetPos()	{ return m_iCurPos; }

protected:
	static const sbyte s_arbQuadDecode[16];
protected:
	int		m_iCurPos;
	byte	m_bPrevSig;
};

extern PosSensor Xpos;
extern PosSensor Ypos;
extern PosSensor Zpos;
extern PosSensor Qpos;
