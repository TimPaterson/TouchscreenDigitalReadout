//****************************************************************************
// Actions.h
//
// Created 12/20/2020 12:51:43 PM by Tim
//
//****************************************************************************

#pragma once


#include "ScreenMgr.h"
#include "AxisDisplay.h"


class Actions
{
	static constexpr int InBufSize = 12;
	static constexpr int MemoryCount = 5;

	enum ActionState
	{
		AS_Empty,		// display clear, ready to accept entry
		AS_Entering,	// numeric entry in process, no operation
		AS_Value,		// display has completed value
	};

	enum Operators
	{
		OP_none = Key_equal,
		OP_plus = Key_plus,
		OP_minus = Key_minus,
		OP_mult = Key_mult,
		OP_divide = Key_divide,
	};

	class CalcMemory : public TextLine
	{
	public:
		// Setting the TextLine backcolor to -1 causes transparent mode
		CalcMemory(const Area *pArea, ulong backColor) :
			TextLine(&MainScreen, pArea, FID_CalcSmall, 0, -1) 
		{
			m_fillColor = backColor;
		}

	public:
		double GetVal()	{ return m_val; }

		void SetVal(double val)
		{
			m_val = val;
			// The RA8876 has a bug when performing color expansion
			// It does not correctly fill in the 16-bit background color.
			// We work around that by erasing the previous text and using
			// transparent background fill.
			FillArea(m_fillColor);	// Clear existing value
			if (val != 0)
				printf("%.8g\n", val);
		}

	protected:
		double	m_val;
		ulong	m_fillColor;
	};

public:
	static void TakeAction(int X, int Y)
	{
		HotspotData	*pSpot;
		uint		group;
		uint		spot;
		double		val;
		AxisDisplay	*pAxis;
		char		*pStr;

		pSpot = ScreenMgr::TestHit(X, Y);
		if (pSpot == NULL)
			return;

		group = pSpot->group;
		spot = pSpot->id;

		switch (group)
		{

		//*****************************************************************
		// Press a numeric entry key
		//

		case HOTSPOT_GROUP_Digit:
			if (s_state == AS_Value)
				ClearEntry();

			s_state = AS_Entering;
			if (spot == Key_sign)
			{
				if (s_arEntryBuf[1] == '-')
				{
					s_arEntryBuf[1] = ' ';
					if (s_indBuf == 2)
						s_state =  AS_Empty;
				}
				else
					s_arEntryBuf[1] = '-';
			}
			else
			{
				if (s_indBuf >= InBufSize - 1)
					return;

				if (spot == Key_decimal)
				{
					if (s_fHaveDp)
						return;
					s_fHaveDp = true;
				}

				s_arEntryBuf[s_indBuf++] = spot;
				s_arEntryBuf[s_indBuf] = '\0';
			}
			s_CalcDisplay.WriteString(s_arEntryBuf);
			break;

		//*****************************************************************
		// Press an operator key
		//

		case HOTSPOT_GROUP_Operator:
			if (s_state == AS_Empty)
				break;

			val = ToValueState();
			if (s_op != OP_none)
			{
				// Perform previous operation
				switch (s_op)
				{
				case OP_plus:
					val += s_arg1;
					break;

				case OP_minus:
					val = s_arg1 - val;
					break;

				case OP_mult:
					val *= s_arg1;
					break;

				default:	// OP_divide
					val = s_arg1 / val;
					break;
				}
				s_arg1 = val;
			}
			s_op = spot;
			if (spot == OP_none)
				ToValueState(val);
			else
				ClearEntry();
			break;

		//*****************************************************************
		// Press a special key
		//

		case HOTSPOT_GROUP_Edit:
			switch (spot)
			{
				case Key_pi:
					ToValueState(M_PI);
					break;

				case Key_backSpace:
					switch (s_state)
					{
					case AS_Entering:
						if (s_indBuf > 2)
						{
							s_indBuf--;
							if (s_fHaveDp && s_arEntryBuf[s_indBuf] == '.')
								s_fHaveDp = false;
							s_arEntryBuf[s_indBuf] = '\0';
							if (s_indBuf == 2 && s_arEntryBuf[1] == ' ')
								s_state = AS_Empty;
						}
						else if (s_arEntryBuf[1] == '-')
						{
							s_arEntryBuf[1] = ' ';
							s_state = AS_Empty;
						}
						break;

					default:
						if (s_op != OP_none)
						{
							s_op = OP_none;
							ToValueState(s_arg1);
						}
						break;
					}
					s_CalcDisplay.ClearArea();
					s_CalcDisplay.WriteString(s_arEntryBuf);
					break;

				default:	// Key_clear
					if (s_state == AS_Empty)
					{
						if (s_op != OP_none)
						{
							s_op = OP_none;
							ToValueState(s_arg1);
						}
					}
					else
						ClearEntry();
					break;
			}
			break;

		//*****************************************************************
		// Press one of the axis displays.
		//
		// If the calculator display is empty, this copies the axis value
		// to the calculator. Otherwise, it copies the calculator value
		// to the axis.

		case HOTSPOT_GROUP_Axis:
			switch (spot)
			{
			case Xdisplay:
				pAxis = &Xpos;
				break;

			case Ydisplay:
				pAxis = &Ypos;
				break;

			default:	// Zdisplay
				pAxis = &Zpos;
				break;
			}

			switch (s_state)
			{
			case AS_Empty:
				val = pAxis->GetPosition();
				ToValueState(val);
				break;

			default:
				pAxis->SetPosition(ToValueState());
				break;
			}
			break;

		//*****************************************************************
		// Press a memory display
		//
		// If the calculator display is empty, this copies the memory value
		// to the calculator. Otherwise, it copies the calculator value
		// to the memory.

		case HOTSPOT_GROUP_Memory:
			spot -= Mem1;

			switch (s_state)
			{
			case AS_Empty:
				val = s_memories[spot].GetVal();
				if (val != 0)
					ToValueState(val);
				break;

			default:
				s_memories[spot].SetVal(ToValueState());
				break;
			}
			break;

		//*****************************************************************
		// Press a PIP keyboard key
		//

		case HOTSPOT_GROUP_Keyboard:
			// UNDONE: keyboard keys
			return;

		//*****************************************************************
		// Press a button not related to the calculator
		//

		default:
			switch (spot)
			{
			case Xbutton:
				Xpos.SetPosition(0);
				break;

			case Ybutton:
				Ypos.SetPosition(0);
				break;

			case Zbutton:
				Zpos.SetPosition(0);
				break;

			case InchMetric:
				Eeprom.Data.fIsMetric ^= true;
				ShowInchMetric();
				break;

			case AbsInc:
				Eeprom.Data.OriginNum ^= 1;
				ShowAbsInc();
				break;
			}
			return;
		}

		//*****************************************************************
		// Display full expression

		s_CalcText.ClearArea();
		pStr = &s_arEntryBuf[1];
		if (s_arEntryBuf[1] == ' ')
			pStr++;

		if (s_op == OP_none)
			s_CalcText.printf("\n%s", pStr);
		else
			s_CalcText.printf("\n%.8g %c %s", s_arg1, s_op, pStr);
	}

	static void ShowInchMetric()
	{
		ScreenMgr::CopyRect(Eeprom.Data.fIsMetric ? &Metric : &Inch, 
			0, 0, &MainScreen, &MainScreen_Areas.InchMetric);
	}

	static void ShowAbsInc()
	{
		ScreenMgr::CopyRect(Eeprom.Data.OriginNum ?  &IncCoord : &AbsCoord, 
			0, 0, &MainScreen, &MainScreen_Areas.AbsInc);
	}

protected:
	static void ToValueState(double val)
	{
		s_CalcDisplay.ClearArea();
		snprintf(&s_arEntryBuf[1], InBufSize - 1, "% .8g", val);
		s_CalcDisplay.WriteString(s_arEntryBuf);
		if (s_op == OP_none)
			s_arg1 = val;
		s_state = AS_Value;
	}

	static double ToValueState()
	{
		double val;

		if (s_state == AS_Value && s_op == OP_none)
			return s_arg1;

		val = atof(&s_arEntryBuf[1]);
		if (s_op == OP_none)
			s_arg1 = val;
		s_state = AS_Value;
		return val;
	}

	static void ClearEntry()
	{
		s_CalcDisplay.ClearArea();
		s_state = AS_Empty;
		s_fHaveDp = false;
		s_arEntryBuf[1] = ' ';
		s_arEntryBuf[2] = '\0';
		s_indBuf = 2;
	}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static double	s_arg1;
	inline static CalcMemory s_memories[MemoryCount]
	{
		{&MainScreen_Areas.Mem1, MemColorOdd},
		{&MainScreen_Areas.Mem2, MemColorEven},
		{&MainScreen_Areas.Mem3, MemColorOdd},
		{&MainScreen_Areas.Mem4, MemColorEven},
		{&MainScreen_Areas.Mem5, MemColorOdd}
	};
	inline static NumberLine s_CalcDisplay{&MainScreen, &MainScreen_Areas.CalcDisplay, FID_Calculator, 0, CalcBackColor};
	inline static TextLine	s_CalcText{&MainScreen, &MainScreen_Areas.CalcText, FID_CalcSmall, 0, CalcBackColor};
	inline static char		s_arEntryBuf[InBufSize] = "\n ";
	inline static byte		s_indBuf = 2;
	inline static byte		s_op = OP_none;
	inline static byte		s_state;
	inline static bool		s_fHaveDp;
};
