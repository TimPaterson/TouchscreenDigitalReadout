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

extern AxisDisplay	Xaxis;
extern AxisDisplay	Yaxis;
extern AxisDisplay	Zaxis;
extern PosSensor	Qpos;


class AxisDisplay : public PosSensor, public NumberLine
{
public:
	AxisDisplay(AxisInfo *pInfo, const Area &area) :
		PosSensor(pInfo), NumberLine(MainScreen, area, FID_DigitDisplay, AxisForeColor, AxisBackColor)
		{}

	void UpdateDisplay()
	{
		double	pos;

		if (m_pInfo->Disable)
		{
			ClearArea();
			return;
		}

		pos = GetPosition();

		if (IsMetric())
			PrintNum("%8.2f\n", pos);
		else
			PrintNum("%8.4f\n", pos);
	}

public:
	static void UpdateAll()
	{
		Xaxis.UpdateDisplay();
		Yaxis.UpdateDisplay();
		Zaxis.UpdateDisplay();
	}
};
