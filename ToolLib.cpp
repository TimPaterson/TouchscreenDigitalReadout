//****************************************************************************
// ToolLib.cpp
//
// Created 1/31/2021 5:23:39 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "ToolLib.h"
#include "Actions.h"


void ToolLib::ToolAction(uint spot)
{
	double	val, newVal;

	switch (spot)
	{
	case ToolsDone:
		ScreenMgr::DisablePip1();
		ScreenMgr::DisablePip2();
		return;

	case ToolNumber:
		val = s_toolDisplay.number;
		newVal = Actions::GetSetValue(val);
		newVal = std::min(newVal, 999.0);
		s_toolDisplay.number = (int)newVal;
		break;

	case ToolFlutes:
		val = s_toolDisplay.flutes;
		newVal = Actions::GetSetValue(val);
		newVal = std::min(newVal, 99.0);
		s_toolDisplay.flutes = (int)newVal;
		break;

	case ToolDiameter:
		val = CheckMetric(s_toolDisplay.diameter);
		newVal = Actions::GetSetValue(val);
		if (val == newVal)
			return;
		newVal = LimitVal(newVal, 9.999);
		s_toolDisplay.diameter = newVal;
		break;

	case ToolLength:
		val = CheckMetric(s_toolDisplay.length);
		newVal = Actions::GetSetValue(val);
		if (val == newVal)
			return;
		newVal = LimitVal(newVal, 99.999);
		s_toolDisplay.length = newVal;
		break;

	case ToolChipLoad:
		val = CheckMetric(Eeprom.Data.ChipLoad);
		newVal = Actions::GetSetValue(val);
		if (val == newVal)
			return;
		newVal = LimitVal(newVal, 0.9999);
		Eeprom.Data.ChipLoad = newVal;
		break;

	case ToolSfm:
		val = CheckMetricSurface(Eeprom.Data.Sfm);
		newVal = Actions::GetSetValue(val);
		if (val == newVal)
			return;
		newVal = CheckMetricSurface(std::min(newVal, 9999.0), false);
		Eeprom.Data.Sfm = newVal;
		break;

	default:
		return;
	}

	if (val != newVal)
	{
		// Changed tool value
		ShowToolInfo();
		s_textLib.DisplayLine(&s_toolDisplay);
	}
}

