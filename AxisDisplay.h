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

extern AxisDisplay	Xdisplay;
extern AxisDisplay	Ydisplay;
extern AxisDisplay	Zdisplay;
extern PosSensor	Qpos;


class AxisDisplay : public PosSensor
{
	static constexpr int UndoLevels = 8;	// Should be power of 2
	static constexpr int UndoDisplays = 3;

	struct UndoInfo
	{
		ushort	count;
		ushort	cur;
		long	value[UndoLevels];
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	AxisDisplay(AxisInfo *pInfo, const Area &axisArea, const Area &undoArea) : 
		PosSensor(pInfo), m_pAxisArea{&axisArea}, m_pUndoArea{&undoArea}
		{}

	void UpdateDisplay()
	{
		if (m_pInfo->Disable)
		{
			s_Display.ClearArea();
			return;
		}

		s_Display.SetTextColor(m_textColor);
		s_Display.SetArea(*m_pAxisArea);
		s_Display.PrintNum(IsMetric() ? "%8.2f" : "%8.4f", GetPosition());
	}

	void SetTextColor(ulong color)
	{
		m_textColor = color;
	}

	// This hides the method in the base classs
	void SetPosition(double pos)
	{
		long		posNew;
		long		posUndo;
		UndoInfo	&undo = m_arUndoInfo[Eeprom.Data.OriginNum];


		posNew = ConvertPosToInt(pos);
		posUndo = m_arOrigins[Eeprom.Data.OriginNum] - posNew;
		if (posUndo != 0)
		{
			++undo.cur %= UndoLevels;
			undo.value[undo.cur] = posUndo;
			if (undo.count < UndoLevels)
				undo.count++;
			DisplayUndo();
		}
		m_arOrigins[Eeprom.Data.OriginNum] = posNew;
	}

	void Undo()
	{
		UndoInfo	&undo = m_arUndoInfo[Eeprom.Data.OriginNum];

		if (undo.count == 0)
			return;

		m_arOrigins[Eeprom.Data.OriginNum] += undo.value[undo.cur];
		undo.value[undo.cur] = 0;
		--undo.cur %= UndoLevels;
		undo.count--;
		DisplayUndo();
	}

public:
	static void UpdateAll()
	{
		Xdisplay.UpdateDisplay();
		Ydisplay.UpdateDisplay();
		Zdisplay.UpdateDisplay();
	}

	static void UpdateUndo()
	{
		Xdisplay.DisplayUndo();
		Ydisplay.DisplayUndo();
		Zdisplay.DisplayUndo();
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	void DisplayUndo()
	{
		uint		cur;
		UndoInfo	&undo = m_arUndoInfo[Eeprom.Data.OriginNum];

		cur = undo.cur;
		for (uint i = 0; i < UndoDisplays; i++)
		{
			s_UndoDisplay.PrintDbl(IsMetric() ? "%8.2f" : "%8.4f", 
				GetDistance(undo.value[cur]), m_pUndoArea[i]);
			--cur %= UndoLevels;
		}
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	ulong		m_textColor;
	const Area	*m_pAxisArea;
	const Area	*m_pUndoArea;
	UndoInfo	m_arUndoInfo[MaxOrigins];

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static NumberLine		s_Display{MainScreen, MainScreen_Areas.Xdisplay, 
		FID_DigitDisplay, AxisForeColor, AxisBackColor};

	inline static NumberLineBlankZ	s_UndoDisplay{MainScreen, MainScreen_Areas.UndoX1, 
		FID_CalcSmall, UndoTextColor, UndoBackColor};
};
