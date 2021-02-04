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
	double	val;
	uint	tool;
	uint	line;

	if (spot == ToolNumber)
	{
		val = Actions::GetCalcValue();
		tool = (uint)std::min(val, 999.0);

		// Switching tools, save current tool if modified
		// It's already in the sort list
		SaveTool();
		line = FindTool(tool);

		if (line == NoCurrentLine)
		{
			// Tool number not present
			if (IsLibShown())
			{
				// Start entering new tool. If we're just clearing
				// the tool number, start with the last tool.
				if (s_curLineNum != NoCurrentLine && tool == 0)
				{
					if (s_arSortList[s_curLineNum] != ToolBufIndex)
						s_bufTool = *PtrFromLine(s_curLineNum);
				}
				else
					s_bufTool.ClearData();

				s_bufTool.number = tool;
				s_curLineNum = NoCurrentLine;
				goto InsertIfValid;
			}
			else
			{
				// Didn't find tool, display error
				s_textMain.SetArea(&MainScreen_Areas.ToolNumber);
				s_textMain.ClearArea();
				s_textMain.WriteString("???");
				return;		// don't show tool info
			}
		}
		ShowToolInfo();
		return;
	}

	if (spot < ToolLastValue)
	{
		if (Actions::HasCalcValue())
		{
			val = Actions::GetCalcValue();

			if (spot < ToolInfoLast)
			{
				// Copy the tool to the buffer if it's not already there.
				if (!IsToolBuffered())
				{
					s_bufTool = *PtrFromLine(s_curLineNum);
					if (IsLibShown())
					{
						// We're in the tool library
						s_modToolIndex = s_arSortList[s_curLineNum];
						s_arSortList[s_curLineNum] = ToolBufIndex;
					}
					else
					{
						// If changing something, tool number no longer valid.
						s_bufTool.number = 0;
						s_curLineNum = NoCurrentLine;
					}
				}
			}

			// Set the entry
			switch (spot)
			{
			case ToolFlutes:
				val = std::min(val, 99.0);
				s_bufTool.flutes = (int)val;
				break;

			case ToolDiameter:
				val = LimitVal(val, 9.999);
				s_bufTool.diameter = val;
				break;

			case ToolLength:
				val = LimitVal(val, 99.999);
				s_bufTool.length = val;
				break;

			case ToolDesc:
				// UNDONE: enter description
				break;

			case ToolChipLoad:
				val = LimitVal(val, 0.9999);
				Eeprom.Data.ChipLoad = val;
				break;

			case ToolSfm:
				val = CheckMetricSurface(std::min(val, 9999.0), false);
				Eeprom.Data.Sfm = val;
				break;
			}

			// See if setting tool info made it "valid"
			if (s_curLineNum == NoCurrentLine)
			{
InsertIfValid:
				if (s_bufTool.IsValid())
				{
					// Tool now valid, insert it
					line = InsertTool(ToolBufIndex);
					s_scroll.InvalidateLines(line, s_toolCount - 1);
					s_scroll.ScrollToLine(line);
				}
			}
			else
				s_scroll.InvalidateLines(s_curLineNum, s_curLineNum);

			ShowToolInfo();
		}
		else
		{
			// Just reading the entry
			switch (spot)
			{
			case ToolFlutes:
				val = s_bufTool.flutes;
				break;

			case ToolDiameter:
				val = CheckMetric(s_bufTool.diameter);
				break;

			case ToolLength:
				val = CheckMetric(s_bufTool.length);
				break;

			case ToolChipLoad:
				val = CheckMetric(Eeprom.Data.ChipLoad);
				break;

			case ToolSfm:
				val = CheckMetricSurface(Eeprom.Data.Sfm);
				break;

			default:
				return;
			}
			Actions::SetCalcValue(val);
		}
		return;
	}

	switch (spot)
	{
	case ToolsDone:
		ScreenMgr::DisablePip1();
		ScreenMgr::DisablePip2();
		SaveTool();
		return;
	}
}


void ToolLib::ShowToolInfo()
{
	double	val;
	uint	sides;
	ulong	color;
	ToolLibInfo	*pInfo;

	// Tool info
	if (s_curLineNum == NoCurrentLine)
		pInfo = &s_bufTool;
	else
		pInfo = PtrFromLine(s_curLineNum);

	s_textMain.DisplayLine(pInfo);
	s_textLib.DisplayLine(pInfo);

	// SFM
	s_textMain.PrintDbl(
		"%5.0f",
		CheckMetricSurface(Eeprom.Data.Sfm), 
		&MainScreen_Areas.Sfm);

	// Chip load
	s_textMain.PrintDbl(
		IsMetric() ? "%6.3f" : "%6.4f",
		CheckMetric(Eeprom.Data.ChipLoad),
		&MainScreen_Areas.ChipLoad);

	// Compute and display RPM
	if (pInfo->diameter != 0 && Eeprom.Data.Sfm != 0)
	{
		val = Eeprom.Data.Sfm / (pInfo->diameter * M_PI);
		val *= Eeprom.Data.fToolLibMetric ? 1000 : 12;
		val = std::min(val, (double)Eeprom.Data.MaxRpm);
	}
	else
		val = 0;

	s_textMain.PrintUint("%5u", (uint)val, &MainScreen_Areas.Rpm);

	// Compute and display feed rate
	val *= CheckMetric(Eeprom.Data.ChipLoad) * pInfo->flutes;
	s_textMain.PrintUint("%5u", (uint)val, &MainScreen_Areas.FeedRate);

	// Update cutter radius offset
	PrepareDrawTool();
	sides = s_toolSides;
	DrawTool(sides & ToolLeftBit,  ToolLeft_X,  ToolLeft_Y);
	DrawTool(sides & ToolRightBit, ToolRight_X, ToolRight_Y);
	DrawTool(sides & ToolBackBit,  ToolBack_X,  ToolBack_Y);
	DrawTool(sides & ToolFrontBit, ToolFront_X, ToolFront_Y);

	val = CheckMetric(pInfo->diameter / 2);
	Xaxis.SetOffset(sides & ToolLeftBit ? val : (sides & ToolRightBit ? -val : 0));
	if (Eeprom.Data.fCncCoordinates)
		val = -val;
	Yaxis.SetOffset(sides & ToolFrontBit ? -val : (sides & ToolBackBit ? val : 0));
	val = CheckMetric(pInfo->length);
	if (Eeprom.Data.fCncCoordinates)
		val = -val;
	Zaxis.SetOffset(Eeprom.Data.fToolLenAffectsZ ? val : 0);

	color = Eeprom.Data.fHighlightOffset && pInfo->diameter != 0 ? ToolColor : AxisForeColor;
	Xaxis.SetForeColor(sides & (ToolLeftBit | ToolRightBit) ? color : AxisForeColor);
	Yaxis.SetForeColor(sides & (ToolBackBit | ToolFrontBit) ? color : AxisForeColor);
}
