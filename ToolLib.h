//****************************************************************************
// ToolLib.h
//
// Created 1/30/2021 2:45:39 PM by Tim
//
//****************************************************************************

#pragma once

#include "ListScroll.h"
#include "HotspotList.h"


class ToolLib
{
	class ToolScroll : public ListScroll
	{
	public:
		ToolScroll() : ListScroll(ToolListWidth, ToolListHeight, ToolRowHeight, Color16bpp) {}

	protected:
		virtual bool FillLine(int lineNum, Area *pArea)
		{
			m_text.SetArea(pArea);
			m_text.ClearArea();
			if (lineNum >= 100)
				return false;

			m_text.printf("%i", lineNum);
			return true;
		}

		TextField m_text{this, NULL, FID_Calculator, ToolLibraryForeground, ToolLibraryBackground};
	};

	struct ToolListInfo
	{
		ushort	number;
		ushort	flutes;
		float	diameter;
		float	length;
	};

public:
	static void Init()
	{
		s_scroll.Init();
	}

	static void ShowToolLib()
	{
		s_scroll.Invalidate();
		s_scroll.SetTotalLines(100);
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

	static void ToolAction(uint spot)
	{
		switch (spot)
		{
		case ToolsDone:
			ScreenMgr::DisablePip1();
			ScreenMgr::DisablePip2();
			break;
		}
	}

	static void DisplayToolLine(NumberLineBlankZ text, ToolListInfo pTool)
	{
	}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static ToolScroll	s_scroll;
	inline static ushort		s_toolCount;
	inline static ushort		s_arToolSort[20];
	inline static ToolListInfo	s_toolDisplay;
	inline static ToolListInfo	s_arToolInfo[20];
};
