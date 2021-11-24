//****************************************************************************
// AxisDisplay.h
//
// Created 12/19/2020 1:57:03 PM by Tim
//
//****************************************************************************

#pragma once

#include "AxisPos.h"
#include "LcdDef.h"
#include "TextField.h"
#include "HotspotList.h"


class AxisDisplay;

extern AxisDisplay	Xdisplay;
extern AxisDisplay	Ydisplay;
extern AxisDisplay	Zdisplay;
extern AxisPos		Xpos;
extern AxisPos		Ypos;
extern AxisPos		Zpos;
extern PosSensor	Qpos;


class AxisDisplay
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
	AxisDisplay(AxisPos *pAxisPos, const Area &axisArea, const Area &undoArea) : 
		m_pAxisPos(pAxisPos), m_pAxisArea{&axisArea}, m_pUndoArea{&undoArea}
		{}

	void UpdateDisplay()
	{
		if (m_pAxisPos->IsDisabled())
		{
			s_Display.ClearArea();
			return;
		}

		s_Display.SetTextColor(m_textColor);
		s_Display.SetArea(*m_pAxisArea);
		s_Display.PrintNum(Eeprom.Data.fIsMetric ? "%8.2f" : "%8.4f", m_pAxisPos->GetPosition());
	}

	void SetTextColor(ulong color)
	{
		m_textColor = color;
	}

	double GetPosition()
	{
		return m_pAxisPos->GetPosition();
	}

	void SetPosition(double pos)
	{
		long		posUndo;
		UndoInfo	&undo = m_arUndoInfo[Eeprom.Data.OriginNum];

		posUndo = m_pAxisPos->SetPosition(pos);
		if (posUndo != 0)
		{
			++undo.cur %= UndoLevels;
			undo.value[undo.cur] = posUndo;
			if (undo.count < UndoLevels)
				undo.count++;
			DisplayUndo();
		}
	}

	void Undo()
	{
		UndoInfo	&undo = m_arUndoInfo[Eeprom.Data.OriginNum];

		if (undo.count == 0)
			return;

		m_pAxisPos->AdjustOrigin(undo.value[undo.cur]);
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
			s_UndoDisplay.PrintDbl(Eeprom.Data.fIsMetric ? "%8.2f" : "%8.4f", 
				m_pAxisPos->GetDistance(undo.value[cur]), m_pUndoArea[i]);
			--cur %= UndoLevels;
		}
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	AxisPos		*m_pAxisPos;
	ulong		m_textColor;
	const Area	*m_pAxisArea;
	const Area	*m_pUndoArea;
	UndoInfo	m_arUndoInfo[PosSensor::MaxOrigins];

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static NumberLine		s_Display{MainScreen, MainScreen_Areas.Xdisplay, 
		FONT_DigitDisplay, AxisForeColor, AxisBackColor};

	inline static NumberLineBlankZ	s_UndoDisplay{MainScreen, MainScreen_Areas.UndoX1, 
		FONT_CalcSmall, UndoTextColor, UndoBackColor};
};
