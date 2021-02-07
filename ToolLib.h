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
#include <FatFile/FatFileConst.h>


class ToolLib
{
	static constexpr int ToolEntrySize = 64;
	static constexpr int MaxToolCount = 100;
	static constexpr ushort NoCurrentLine = 0xFFFF;		// occurs in s_curLineNum
	static constexpr ushort ToolBufIndex = 0xFFFF;		// occurs in s_arSortList[]
	static constexpr ushort ToolNotModified = 0xFFFF;	// occurs in s_modToolIndex

	//*********************************************************************
	// Types
	//*********************************************************************

	enum ToolButtonImages
	{
		TOOL_IMAGE_NotModified,
		TOOL_IMAGE_ConfirmDelete,
		TOOL_IMAGE_IsModified,
	};

	struct ToolLibBase
	{
		ushort	number;
		ushort	flutes;
		double	diameter;
		double	length;
	};

	static constexpr int ToolDescSize = ToolEntrySize - sizeof(ToolLibBase);

	struct ToolLibInfo : public ToolLibBase
	{
		char	arDesc[ToolDescSize];

		bool HasData()
		{
			return diameter != 0 || length != 0 || flutes != 0 || arDesc[0] != 0;
		}

		bool IsValid()
		{
			if (number == 0)
				return false;

			return HasData();
		}

		ToolLibInfo & operator= (const ToolLibInfo &src)
		{
			*(ToolLibBase *)this = *(ToolLibBase *)&src;
			strcpy(arDesc, src.arDesc);
			return *this;
		}

		void ClearData()
		{
			number = 0;
			flutes = 0;
			diameter = 0;
			length = 0;
			arDesc[0] = 0;
		}
	};

	class ToolScroll : public ListScroll
	{
	public:
		ToolScroll() : ListScroll(ToolListWidth, ToolListHeight, ToolRowHeight, Color16bpp) {}

	protected:
		virtual void FillLine(int lineNum, Area *pArea)
		{
			if (lineNum < s_toolCount)
			{
				if (lineNum == s_curLineNum)
					s_textList.SetForeColor(ToolLibSelected);
				s_textList.DisplayLine(PtrFromLine(lineNum));
				s_textList.SetForeColor(ToolLibForeground);
				ScreenMgr::CopyRect(this, pArea, &ToolRow);
			}
			else
			{
				ScreenMgr::FillRect(this, pArea, ToolLibBackground);
				if (lineNum == s_toolCount)
				{
					// Draw bottom line of last grid entry.
					// Note that most destination graphics registers are already set up
					WriteReg16(BTE_HIG0, 1);	// one pixel height
					SetForeColor(ToolInfoForeground);
					ScreenMgr::SetBteSrc0(&ToolRow);
					WriteRegXY(S0_X0, 0, 0);
					WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeMemoryCopyWithRop | BTE_CTRL1_RopS0);
					WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
					WaitWhileBusy();
				}
			}
		}

		virtual void LineSelected(int lineNum)
		{
			SelectLine(lineNum);
			ShowToolInfo();
		}
	};

	class ToolDisplay : public NumberLineBlankZ
	{
	public:
		ToolDisplay(Canvas *pCanvas, const Area *pArea, ulong foreColor, ulong backColor) :
			NumberLineBlankZ(pCanvas, pArea, FID_CalcSmall, foreColor, backColor), arItemAreas{pArea}
			{}

	public:
		void DisplayLine(ToolLibInfo *pTool)
		{
			uint	widthSpace;

			PrintDbl(IsMetric() ? "%5.2f" : "%5.3f", CheckMetric(pTool->diameter), &arItemAreas[ToolDiameter]);
			PrintDbl(IsMetric() ? "%6.2f" : "%6.3f", CheckMetric(pTool->length), &arItemAreas[ToolLength]);

			PrintUint("%3u", pTool->number, &arItemAreas[ToolNumber]);
			PrintUint("%3u", pTool->flutes, &arItemAreas[ToolFlutes]);

			// Set the space width to normal for the description field
			SetArea(&arItemAreas[ToolDesc]);
			ClearArea();
			widthSpace = GetSpaceWidth();
			SetSpaceWidth();
			WriteString(pTool->arDesc);
			SetSpaceWidth(widthSpace);
		}

	protected:
		const Area *arItemAreas;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	// .cpp file
	static void ToolAction(uint spot);
	static void ShowToolInfo();
	static int ImportTools(char *pb, uint cb, uint cbWrap);
	static int ImportTool(char *pchBuf);
	static void ImportDone();

public:
	static void Init()
	{
		s_scroll.Init();

		s_modToolIndex = ToolNotModified;
		if (s_toolCount == 0)
			s_curLineNum = NoCurrentLine;

		s_scroll.SetTotalLines(s_toolCount);
		s_scroll.ScrollToLine(s_curLineNum);
	}

	static void ShowToolLib()
	{
		ScreenMgr::EnablePip1(&ToolLibrary, 0, 0);
		ScreenMgr::EnablePip2(&s_scroll, 0, ToolListTop);
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

	static void ChangeUnits()
	{
		s_scroll.InvalidateLines(0, s_toolCount - 1);
		ShowToolInfo();
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	static ToolLibInfo *PtrFromIndex(uint index)
	{
		if (index == ToolBufIndex)
			return &s_bufTool;

		return &s_arToolInfo[index];
	}

	static ToolLibInfo *PtrFromLine(uint line)
	{
		return PtrFromIndex(s_arSortList[line]);
	}

	static ToolLibInfo *PtrCurrentTool()
	{
		if (s_curLineNum == NoCurrentLine)
			return &s_bufTool;
		return PtrFromLine(s_curLineNum);
	}

	static bool IsToolBuffered()
	{
		return s_curLineNum == NoCurrentLine || s_arSortList[s_curLineNum] == ToolBufIndex;
	}

	static void SelectLine(uint line)
	{
		uint	oldLine;

		oldLine = s_curLineNum;
		s_curLineNum = line;
		if (oldLine != NoCurrentLine)
			s_scroll.InvalidateLine(oldLine);
		if (line != NoCurrentLine)
		{
			s_scroll.InvalidateLine(line);
			s_scroll.ScrollToLine(line);
		}
	}

	static uint FindTool(uint num)
	{
		uint	line;

		for (line = 0; line < s_toolCount; line++)
		{
			if (PtrFromLine(line)->number == num)
			{
				SelectLine(line);
				if (s_modToolIndex != ToolNotModified)
					DEBUG_PRINT("Find replacing modified tool\n");
				s_modToolIndex = ToolNotModified;
				return line;
			}
		}
		return NoCurrentLine;
	}

	static void SaveTool()
	{
		if (s_curLineNum == NoCurrentLine)
			return;

		if (s_modToolIndex != ToolNotModified)
		{
			// Update existing tool
			s_arToolInfo[s_modToolIndex] = s_bufTool;
			s_arSortList[s_curLineNum] = s_modToolIndex;
			s_modToolIndex = ToolNotModified;
			SetToolButtonImage(TOOL_IMAGE_NotModified);
		}
		else if (s_arSortList[s_curLineNum] == ToolBufIndex)
		{
			// New tool
			if (s_freeToolIndex >= MaxToolCount)
			{
				// UNDONE: garbage collect tool list
				return;
			}

			s_arSortList[s_curLineNum] = s_freeToolIndex;
			s_arToolInfo[s_freeToolIndex++] = s_bufTool;
		}
	}

	static uint InsertTool(uint bufIndex)
	{
		uint	line;
		uint	num;

		if (s_toolCount >= MaxToolCount)
			return NoCurrentLine;

		num = PtrFromIndex(bufIndex)->number;

		for (line = 0; line < s_toolCount && PtrFromLine(line)->number < num; line++);

		// We do not update the display in case this is a mass import
		memmove(&s_arSortList[line + 1], &s_arSortList[line], (s_toolCount - line) * sizeof s_arSortList[0]);
		s_arSortList[line] = bufIndex;
		if (s_modToolIndex != ToolNotModified)
			DEBUG_PRINT("Inserting  modified tool\n");
		s_modToolIndex = ToolNotModified;
		s_toolCount++;
		s_scroll.SetTotalLines(s_toolCount);
		return line;
	}

	static void DeleteTool()
	{
		uint	line;

		// We should only be here when displaying an unmodified tool
		line = s_curLineNum;

		// Overwrite tool data -- UNDONE: handle for flash
		s_arToolInfo[s_arSortList[line]].ClearData();

		// Remove from sort list
		s_toolCount--;
		memmove(&s_arSortList[line], &s_arSortList[line + 1], (s_toolCount - line) * sizeof s_arSortList[0]);
		if (line >= s_toolCount)
		{
			line--;
			SelectLine(line);
		}
		s_scroll.SetTotalLines(s_toolCount);
		s_scroll.InvalidateLines(line, s_toolCount + 1);
		ShowToolInfo();
	}

	static void SetToolButtonImage(ToolButtonImages image)
	{
		ScreenMgr::SelectImage(&ToolLibrary, &ToolLibrary_Areas.ToolButtons, &ToolButtons, image);
	}

protected:
	static bool IsMetric()
	{
		return Eeprom.Data.fIsMetric;
	}

	static bool IsLibShown()
	{
		return ScreenMgr::GetPip1()->pImage == &ToolLibrary;
	}

	static double LimitVal(double val, double max)
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
	// const (flash) data
	//*********************************************************************
protected:
	inline static const char s_archImportHead[] = "Tool,Diameter,Length,Flutes,Description";

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static ToolDisplay	s_textMain {&MainScreen, &MainScreen_Areas.ToolNumber,
		ScreenForeColor, ScreenBackColor};
	inline static ToolDisplay	s_textInfo  {&ToolLibrary, &ToolLibrary_Areas.ToolNumber,
		ToolInfoForeground, ToolInfoBackground};
	inline static ToolDisplay	s_textList  {&ToolRow, &ToolRow_Areas.ToolNumber,
		ToolLibForeground, ToolLibBackground};
	inline static ToolScroll	s_scroll;
	inline static ushort		s_toolCount;
	inline static ushort		s_curLineNum;
	inline static ushort		s_modToolIndex;
	inline static ushort		s_freeToolIndex;
	inline static byte			s_toolSides;
	inline static bool			s_fConfirmDelete;
	inline static ToolLibInfo	s_bufTool;
	inline static FILE			s_fileImport;
	inline static ushort		s_arSortList[MaxToolCount];
	inline static ToolLibInfo	s_arToolInfo[MaxToolCount];
public:
	inline static byte			s_arImportBuf[2][FAT_SECT_SIZE] ALIGNED_ATTR(uint32_t);
};
