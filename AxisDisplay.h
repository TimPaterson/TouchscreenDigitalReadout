//****************************************************************************
// AxisDisplay.h
//
// Created 12/19/2020 1:57:03 PM by Tim
//
//****************************************************************************

#pragma once

#include "PosSensor.h"
#include "LcdDef.h"
#include "TextField.h"
#include "HotspotList.h"


class AxisDisplay;

extern AxisDisplay	Xpos;
extern AxisDisplay	Ypos;
extern AxisDisplay	Zpos;
extern PosSensor	Qpos;


class AxisDisplay : public PosSensor, public TextLine
{
public:
	AxisDisplay(AxisInfo *pInfo, uint X, uint Y) :
		PosSensor(pInfo), TextLine(&MainScreen, X, Y)
	{
		SetForeColor(0);
		SetBackColor(0xFFFF00);
		SetFont(FID_DigitDisplay);
		SetSpaceWidth(GetCharWidth('0'));
	}

	void UpdateDisplay()
	{
		double	pos;

		pos = GetPosition();
		if (pos < 0)
			MoveXposition(GetCharWidth(' ') - GetCharWidth('-'));

		if (IsMetric())
			printf("%8.2f\n", pos);
		else
			printf("%8.4f\n", pos);
	}

public:
	static void UpdateAll()
	{
		Xpos.UpdateDisplay();
		Ypos.UpdateDisplay();
		Zpos.UpdateDisplay();
	}
};
