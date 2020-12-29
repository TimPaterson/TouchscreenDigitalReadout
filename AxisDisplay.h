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


class AxisDisplay : public PosSensor, public NumberLine
{
public:
	AxisDisplay(AxisInfo *pInfo, const Area *pArea) :
		PosSensor(pInfo), NumberLine(&MainScreen, pArea, FID_DigitDisplay, 0, 0xFFFF00)
		{}

	void UpdateDisplay()
	{
		double	pos;

		pos = GetPosition();

		if (IsMetric())
			PrintNum("%8.2f\n", pos);
		else
			PrintNum("%8.4f\n", pos);
	}

public:
	static void UpdateAll()
	{
		Xpos.UpdateDisplay();
		Ypos.UpdateDisplay();
		Zpos.UpdateDisplay();
	}
};
