//****************************************************************************
// ToolLib.h
//
// Created 1/30/2021 2:45:39 PM by Tim
//
//****************************************************************************

#pragma once

#include "HotspotList.h"
#include "ListScroll.h"
#include "AxisDisplay.h"


class ToolLib
{
	//*********************************************************************
	// Types
	//*********************************************************************

	struct ToolLibInfo
	{
		ushort	number;
		ushort	flutes;
		double	diameter;
		double	length;
	};

	class ToolScroll : public ListScroll
	{
	public:
		ToolScroll() : ListScroll(ToolListWidth, ToolListHeight, ToolRowHeight, Color16bpp) {}

	protected:
		virtual bool FillLine(int lineNum, Area *pArea)
		{
			if (lineNum < s_toolCount)
			{
				s_textList.DisplayLine(&s_arToolInfo[lineNum]);
			}
			else
			{
				s_textList.DisplayLine(&s_toolBlank);
			}
			ScreenMgr::CopyRect(this, pArea, &ToolRow);
			return true;
		}
	};

	class ToolDisplay : public NumberLineBlankZ
	{
	public:
		ToolDisplay(Canvas *pCanvas, const Area *pArea, ulong foreColor, ulong backColor) :
			NumberLineBlankZ(pCanvas, NULL, FID_CalcSmall, foreColor, backColor), arItemAreas{pArea}
			{}

	public:
		void DisplayLine(ToolLibInfo *pTool)
		{
			PrintDbl(IsMetric() ? "%5.2f" : "%5.3f", CheckMetric(pTool->diameter), &arItemAreas[ToolDiameter]);
			PrintDbl(IsMetric() ? "%6.2f" : "%6.3f", CheckMetric(pTool->length), &arItemAreas[ToolLength]);

			PrintUint("%3u", pTool->number, &arItemAreas[ToolNumber]);
			PrintUint("%3u", pTool->flutes, &arItemAreas[ToolFlutes]);
		}

	protected:
		const Area *arItemAreas;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	// .cpp file
	static void ToolAction(uint spot);

public:
	static void Init()
	{
		s_scroll.Init();
	}

	static void ShowToolLib()
	{
		s_scroll.SetTotalLines(20);
		s_scroll.SetScrollPosition(0);
		ScreenMgr::EnablePip2(&s_scroll, 0, ToolListTop);
		ScreenMgr::EnablePip1(&ToolLibrary, 0, 0);
	}

	static ListScroll *ListCapture(int x, int y, ScrollAreas spot)
	{
		if (s_scroll.StartCapture(x, y - ToolListTop, spot))
			return &s_scroll;
		return NULL;
	}

	static void SetToolSide(uint side)
	{
		s_toolSides = (s_toolSides & ~(side >> ToolMaskShift)) ^ side;
		ShowToolInfo();
	}

	static void ShowToolInfo()
	{
		double	val;
		uint	sides;
		ulong	color;

		// Tool info
		s_textMain.DisplayLine(&s_toolDisplay);

		// SFM
		s_textMain.PrintDbl(
			IsMetric() ? "%5.1f" : "%5.0f",
			CheckMetricSurface(Eeprom.Data.Sfm), 
			&MainScreen_Areas.Sfm);

		// Chip load
		s_textMain.PrintDbl(
			IsMetric() ? "%6.3f" : "%6.4f",
			CheckMetric(Eeprom.Data.ChipLoad),
			&MainScreen_Areas.ChipLoad);

		// Compute and display RPM
		if (s_toolDisplay.diameter != 0 && Eeprom.Data.Sfm != 0)
		{
			val = Eeprom.Data.Sfm / (s_toolDisplay.diameter * M_PI);
			val *= Eeprom.Data.fToolLibMetric ? 1000 : 12;
			val = std::min(val, (double)Eeprom.Data.MaxRpm);
		}
		else
			val = 0;

		s_textMain.PrintUint("%5u", (uint)val, &MainScreen_Areas.Rpm);

		// Compute and display feed rate
		val *= CheckMetric(Eeprom.Data.ChipLoad) * s_toolDisplay.flutes;
		s_textMain.PrintUint("%5u", (uint)val, &MainScreen_Areas.FeedRate);

		// Update cutter radius offset
		PrepareDrawTool();
		sides = s_toolSides;
		DrawTool(sides & ToolLeftBit,  ToolLeft_X,  ToolLeft_Y);
		DrawTool(sides & ToolRightBit, ToolRight_X, ToolRight_Y);
		DrawTool(sides & ToolBackBit,  ToolBack_X,  ToolBack_Y);
		DrawTool(sides & ToolFrontBit, ToolFront_X, ToolFront_Y);

		val = CheckMetric(s_toolDisplay.diameter / 2);
		Xaxis.SetOffset(sides & ToolLeftBit ? val : (sides & ToolRightBit ? -val : 0));
		if (Eeprom.Data.fCncCoordinates)
			val = -val;
		Yaxis.SetOffset(sides & ToolFrontBit ? -val : (sides & ToolBackBit ? val : 0));
		val = CheckMetric(s_toolDisplay.length);
		if (Eeprom.Data.fCncCoordinates)
			val = -val;
		Zaxis.SetOffset(Eeprom.Data.fToolLenAffectsZ ? val : 0);

		color = Eeprom.Data.fHighlightOffset && s_toolDisplay.diameter != 0 ? ToolColor : AxisForeColor;
		Xaxis.SetForeColor(sides & (ToolLeftBit | ToolRightBit) ? color : AxisForeColor);
		Yaxis.SetForeColor(sides & (ToolBackBit | ToolFrontBit) ? color : AxisForeColor);
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	static bool IsMetric()
	{
		return Eeprom.Data.fIsMetric;
	}

	static double LimitVal(double val, double max) NO_INLINE_ATTR
	{
		if (IsMetric())
			max *= 10.0;
		return CheckMetric(std::min(val, max), false);
	}

	static void PrepareDrawTool()
	{
		ScreenMgr::SetDrawCanvas(&MainScreen);
		ScreenMgr::WriteRegXY(ELL_A0, ToolImageRadius, ToolImageRadius);
	}

	static void DrawTool(bool fEnable, uint x, uint y)
	{
		ScreenMgr::WriteRegXY(DEHR0, x, y);
		ScreenMgr::SetForeColor(fEnable ? ToolColor : NoToolColor);
		ScreenMgr::WriteReg(DCR1, DCR1_DrawEllipse | DCR1_FillOn | DCR1_DrawActive);
		ScreenMgr::WaitWhileBusy();
	}

	static double CheckMetric(double val, bool fToDisplay = true)
	{
		double	factor;

		if (Eeprom.Data.fIsMetric == Eeprom.Data.fToolLibMetric)
			return val;

		factor = (fToDisplay ^ Eeprom.Data.fToolLibMetric) ? MmPerInch : 1.0 / MmPerInch;
		return val * factor;
	}

	static double CheckMetricSurface(double val, bool fToDisplay = true)
	{
		double	factor;

		if (Eeprom.Data.fIsMetric == Eeprom.Data.fToolLibMetric)
			return val;

		factor = MmPerInch * 12.0 / 1000.0;	// meters / foot
		if (!(fToDisplay ^ Eeprom.Data.fToolLibMetric))
			factor = 1 / factor;
		return val * factor;
	}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static ToolDisplay	s_textMain {&MainScreen, &MainScreen_Areas.ToolNumber,
		ScreenForeColor, ScreenBackColor};
	inline static ToolDisplay	s_textLib  {&ToolLibrary, &ToolLibrary_Areas.ToolNumber,
		ToolLibraryForeground, ToolLibraryBackground};
	inline static ToolDisplay	s_textList  {&ToolRow, &ToolRow_Areas.ToolNumber,
		ToolLibraryForeground, ToolLibraryBackground};
	inline static ToolScroll	s_scroll;
	inline static ushort		s_toolCount;
	inline static ushort		s_arToolSort[20];
	inline static ToolLibInfo	s_toolDisplay;
	inline static ToolLibInfo	s_toolBlank;
	inline static ToolLibInfo	s_arToolInfo[20];
	inline static byte			s_toolSides;
};
