//****************************************************************************
// Actions.h
//
// Created 12/20/2020 12:51:43 PM by Tim
//
//****************************************************************************

#pragma once

#include "LcdDef.h"
#include "ScreenMgr.h"
#include "AxisDisplay.h"
#include "ToolLib.h"
#include "TouchCalibrate.h"
#include "UpdateMgr.h"


class Actions
{
	static constexpr int InBufSize = 11;
	static constexpr int MemoryCount = 4;

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

	// Each axis has an array of settings. This indexes into the array.
	enum SettingsAreas
	{
		AREA_Disable,
		AREA_Resolution,
		AREA_Invert,
		AREA_Correction,
	};

	//*********************************************************************
	// Local types
	//*********************************************************************
protected:
	class CalcMemory : public NumberLine
	{
	public:
		CalcMemory(const Area &area, ulong backColor) :
			NumberLine(MainScreen, area, FONT_CalcSmall, ScreenForeColor, backColor)
		{}

	public:
		double GetVal()	{ return m_val; }

		void SetVal(double val)
		{
			m_val = val;

			ClearArea();
			if (val == 0)
				return;
			FormatValue(val);
			WriteString(s_arFormatBuf);
		}

	protected:
		double	m_val;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************
public:
	static bool HasCalcValue()  {return s_state != AS_Empty; }

public:
	static void Init()
	{
		ShowAbsInc();
		ShowInchMetric();
		Tools.Init();
		Files.Init();
	}

	static void TakeAction(int x, int y, uint flags)
	{
		PipInfo		*pPipInfo;

		if (flags & TOUCH_Start)
			TakeAction(x, y);
		else if (s_pCapture != NULL)
		{
			if (flags & TOUCH_End)
			{
				s_pCapture->EndCapture();
				s_pCapture = NULL;
			}
			else
			{
				pPipInfo = Lcd.GetPip(s_pCapture);
				s_pCapture->NewPosition(x - pPipInfo->x, y - pPipInfo->y);
			}
		}
	}

	static void TakeAction(int x, int y)
	{
		HotspotData	*pSpot;
		uint		group;
		uint		spot;
		double		val;
		PosSensor	*pSensor;
		AxisDisplay	*pDisplay;
		char		*pStr;
		bool		*pToggle;

		pSpot = Lcd.ScreenTestHit(x, y);
		if (pSpot == NULL)
			return;

		group = pSpot->group;
		spot = pSpot->id;

		switch (group)
		{

		//*****************************************************************
		// Dispatch to other handlers
		//

		case HOTSPOT_GROUP_ToolDisplay:
			s_pCapture = Tools.ListCapture(x, y, (ScrollAreas)spot);
			return;

		case HOTSPOT_GROUP_FileDisplay:
			s_pCapture = Files.ListCapture(x, y, (ScrollAreas)spot);
			return;

		case HOTSPOT_GROUP_ToolLib:
			Tools.ToolAction(spot, x, y);
			return;

		case HOTSPOT_GROUP_Update:
			UpdateMgr::UpdateAction(spot, x, y);
			return;

		case HOTSPOT_GROUP_TimeSet:
			Tools.SetTime(spot);
			return;

		case HOTSPOT_GROUP_Keyboard:
			KeyboardMgr::KeyHit(spot);
			return;

		//*****************************************************************
		// Press a numeric entry key
		//

		case HOTSPOT_GROUP_Digit:
			if (s_state == AS_Value)
			{
				if (spot == Key_sign)
				{
					// Treat as operator, change sign
					ToValueState(-s_arg1);
					break;
				}
				// Otherwise start new entry
				ClearEntry();
			}

			s_state = AS_Entering;
			if (spot == Key_sign)
			{
				if (s_arEntryBuf[0] == '-')
				{
					s_arEntryBuf[0] = ' ';
					if (s_indBuf == 1)
						s_state =  AS_Empty;
				}
				else
					s_arEntryBuf[0] = '-';
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
			s_CalcDisplay.ResetPosition();
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
						if (s_indBuf > 1)
						{
							s_indBuf--;
							if (s_fHaveDp && s_arEntryBuf[s_indBuf] == '.')
								s_fHaveDp = false;
							s_arEntryBuf[s_indBuf] = '\0';
							if (s_indBuf == 1 && s_arEntryBuf[0] == ' ')
								s_state = AS_Empty;
						}
						else if (s_arEntryBuf[0] == '-')
						{
							s_arEntryBuf[0] = ' ';
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
			pDisplay = s_arDisplay[spot];

			if (s_state == AS_Empty)
				ToValueState(pDisplay->GetPosition());
			else
				pDisplay->SetPosition(ToValueState());
			break;

		//*****************************************************************
		// Press one of the axis zero buttons.
		//

		case HOTSPOT_GROUP_AxisButton:
			s_arDisplay[spot]->SetPosition(0);
			break;

		//*****************************************************************
		// Press Undo list for an axis.
		//

		case HOTSPOT_GROUP_Undo:
			s_arDisplay[spot]->Undo();
			break;

		//*****************************************************************
		// Press a memory display
		//
		// If the calculator display is empty, this copies the memory value
		// to the calculator. Otherwise, it copies the calculator value
		// to the memory.

		case HOTSPOT_GROUP_Memory:
			spot -= Mem1;

			if (s_state == AS_Empty)
			{
				val = s_memories[spot].GetVal();
				if (val != 0)
					ToValueState(val);
			}
			else
				s_memories[spot].SetVal(ToValueState());
			break;

		//*****************************************************************
		// Press a tool cutting side key
		//

		case HOTSPOT_GROUP_ToolSide:
			Tools.SetToolSide(spot);
			return;

		//*****************************************************************
		// Settings screen
		//

		case HOTSPOT_GROUP_Disable:
			pSensor = s_arSensor[spot];
			pSensor->SetDisable(pSensor->IsDisabled() ^ true);
			ShowSettingsInfo();
			return;

		case HOTSPOT_GROUP_Resolution:
			pSensor = s_arSensor[spot];
			if (s_state == AS_Empty)
			{
				// Just reading the value
				val = pSensor->GetResolution();
				ToValueState(val);
				break;
			}
			else
			{
				// Setting the value
				val = ToValueState();
				if (val > 10 || val < 1)
					;// UNDONE: Display error
				else
				{
					pSensor->SetResolution((uint)val);
					ShowSettingsInfo();
				}
				return;
			}

		case HOTSPOT_GROUP_Correction:
			pSensor = s_arSensor[spot];
			if (s_state == AS_Empty)
			{
				// Just reading the value
				val = pSensor->GetCorrectionPpm();
				ToValueState(val);
				break;
			}
			else
			{
				// Setting the value
				val = ToValueState();
				if (!pSensor->SetCorrectionPpm(val))
					;	// UNDONE: Display error
				ShowSettingsInfo();
				return;
			}

		case HOTSPOT_GROUP_Invert:
			pSensor = s_arSensor[spot];
			pSensor->SetDirection(pSensor->GetDirection() ^ true);
			ShowSettingsInfo();
			return;

		case HOTSPOT_GROUP_SettingToggle:
			switch (spot)
			{
			case HighlightXY:
				pToggle = &Eeprom.Data.fHighlightOffset;
				break;

			case CncCoordinates:
				pToggle = &Eeprom.Data.fCncCoordinates;
				break;

			default:	// OffsetZ
				pToggle = &Eeprom.Data.fToolLenAffectsZ;
				break;
			}
			*pToggle ^= true;
			ShowSettingsInfo();
			Tools.ShowToolInfo();
			return;

		//*****************************************************************
		// Press a button not related to the calculator
		//

		default:
			switch (spot)
			{
			case ToolMenu:
				Tools.ShowToolLib();
				break;

			case InchMetric:
				Eeprom.Data.fIsMetric ^= true;
				ShowInchMetric();
				Tools.ChangeUnits();
				AxisDisplay::UpdateUndo();
				UpdateEeprom();
				break;

			case AbsInc:
				Eeprom.Data.OriginNum ^= 1;
				ShowAbsInc();
				AxisDisplay::UpdateUndo();
				UpdateEeprom();
				break;

			case Settings:
				if (!Lcd.GetPip2()->IsEnabled())
				{
					Lcd.EnablePip2(&SettingsScreen, 0, 0);
					ShowSettingsInfo();
				}
				else
				{
					Lcd.DisablePip2();
					Eeprom.StartSave();	// save all changes
				}
				break;

			case BrightUp:
				ChangeScreenBrightness(10);
				UpdateEeprom();
				break;

			case BrightDown:
				ChangeScreenBrightness(-10);
				UpdateEeprom();
				break;

			case MaxRpm:
				if (s_state == AS_Empty)
				{
					// Just reading the value
					ToValueState(Eeprom.Data.MaxRpm);
				}
				else
				{
					// Setting the value
					val = ToValueState();
					if (val < 60000 && val >= 100)
					{
						Eeprom.Data.MaxRpm = lround(val);
						ShowSettingsInfo();
					}
				}
				break;

			case TouchCal:
				// touch calibrations save changes to EEPROM
				TouchCalibrate::Open(true);
				break;

			case FirmwareUpdate:
				Eeprom.StartSave();	// save all changes
				UpdateMgr::Open();
				break;
			}
			return;
		}

		//*****************************************************************
		// Display full expression

		s_CalcText.ClearArea();
		pStr = s_arEntryBuf;
		if (s_arEntryBuf[0] == ' ')
			pStr++;

		if (s_op == OP_none)
			s_CalcText.WriteString(pStr);
		else
		{
			FormatValue(s_arg1);
			s_CalcText.printf("%s %c %s", s_arFormatBuf, s_op, pStr);
		}
	}

public:
	static double GetCalcValue()
	{
		double	val;

		val = ToValueState();
		// Negative values never allowed
		if (val < 0)
			val = -val;
		return val;
	}

	static void SetCalcValue(double val)
	{
		if (val != 0)
			ToValueState(val);
	}

	//*********************************************************************
	// Helper functions - Main screen
	//*********************************************************************
protected:
	static void SelectImage(const Area *pAreaDst, const ColorImage *pSrc, uint index)
	{
		Lcd.SelectImage(&MainScreen, pAreaDst, pSrc, index);
	}

	static void UpdateEeprom()
	{
		Eeprom.StartSave();
	}

	static void ToValueState(double val)
	{
		int		cch;

		cch = FormatValue(val);
		if (s_arFormatBuf[0] != '-')
		{
			s_arEntryBuf[0] = ' ';
			memcpy(&s_arEntryBuf[1], s_arFormatBuf, cch + 1);
		}
		else
			memcpy(&s_arEntryBuf[0], s_arFormatBuf, cch + 1);

		s_CalcDisplay.ClearArea();
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

		val = atof(s_arEntryBuf);
		if (s_op == OP_none)
			s_arg1 = val;
		s_state = AS_Value;
		return val;
	}

	static int FormatValue(double val) NO_INLINE_ATTR
	{
		int		cch;
		const char *pFmt;
		double	absVal;

		absVal = fabs(val);
		if (absVal > 99999999.0 || absVal < 0.00001)
		{
			if (absVal == 0.0)
			{
				s_arFormatBuf[0] = '0';
				cch = 1;
				goto SetNull;
			}
			pFmt = "%.2E";	// 3 significant digits (plus 5 char for exponent)
		}
		else if (absVal >= 1)
			pFmt = "%.8G";
		else
		{
			cch = snprintf(s_arFormatBuf, sizeof s_arFormatBuf, "%.7F", val);
			// strip trailing zeros
			while (s_arFormatBuf[cch - 1] == '0')
				cch--;
SetNull:
			s_arFormatBuf[cch] = '\0';
			return cch;
		}

		return snprintf(s_arFormatBuf, sizeof s_arFormatBuf, pFmt, val);
	}

	static void ClearEntry()
	{
		s_CalcDisplay.ClearArea();
		s_state = AS_Empty;
		s_fHaveDp = false;
		s_arEntryBuf[0] = ' ';
		s_arEntryBuf[1] = '\0';
		s_indBuf = 1;
	}

	static void ShowInchMetric()
	{
		SelectImage(&MainScreen_Areas.InchMetric, &InchMetricBtn, Eeprom.Data.fIsMetric);
		SelectImage(&MainScreen_Areas.SpeedDisplay, &SpeedDisplay, Eeprom.Data.fIsMetric);
	}

	static void ShowAbsInc()
	{
		SelectImage(&MainScreen_Areas.AbsInc, &Coord, Eeprom.Data.OriginNum);
	}

	//*********************************************************************
	// Helper functions - Settings screen
	//*********************************************************************
protected:
	static void SettingsCheckBox(const Area &pAreaDst, bool f)
	{
		Lcd.SelectImage(&SettingsScreen, &pAreaDst, &CheckBox, f);
	}

	static void ShowAxisInfo(SensorInfo axis, const Area arArea[])
	{
		double	val;

		SettingsCheckBox(arArea[AREA_Disable], axis.Disable);
		SettingsCheckBox(arArea[AREA_Invert], axis.Direction);

		s_SettingDisplay.PrintUint("%i", (uint)axis.Resolution, arArea[AREA_Resolution]);

		val = (axis.Correction - 1.0) * 1E6;
		s_SettingDisplay.PrintDbl("%+6.1f", val, arArea[AREA_Correction]);
	}

	static void ShowSettingsInfo()
	{
		ShowAxisInfo(Eeprom.Data.XaxisInfo, &SettingsScreen_Areas.Xdisable);
		ShowAxisInfo(Eeprom.Data.YaxisInfo, &SettingsScreen_Areas.Ydisable);
		ShowAxisInfo(Eeprom.Data.ZaxisInfo, &SettingsScreen_Areas.Zdisable);
		ShowAxisInfo(Eeprom.Data.QaxisInfo, &SettingsScreen_Areas.Qdisable);

		SettingsCheckBox(SettingsScreen_Areas.HighlightXY, Eeprom.Data.fHighlightOffset);
		SettingsCheckBox(SettingsScreen_Areas.OffsetZ, Eeprom.Data.fToolLenAffectsZ);
		SettingsCheckBox(SettingsScreen_Areas.CncCoordinates, Eeprom.Data.fCncCoordinates);

		s_SettingDisplay.PrintUint("%5i", Eeprom.Data.MaxRpm, SettingsScreen_Areas.MaxRpm);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

	inline static AxisDisplay*const s_arDisplay[3] = { &Xdisplay, &Ydisplay, &Zdisplay };

	inline static PosSensor	*const s_arSensor[4] = { &Xpos, &Ypos, &Zpos, &Qpos };

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static double	s_arg1;
	inline static ListScroll	*s_pCapture;

	inline static CalcMemory s_memories[MemoryCount]
	{
		{MainScreen_Areas.Mem1, MemColorOdd},
		{MainScreen_Areas.Mem2, MemColorEven},
		{MainScreen_Areas.Mem3, MemColorOdd},
		{MainScreen_Areas.Mem4, MemColorEven},
	};
	inline static NumberLine s_CalcDisplay{MainScreen, MainScreen_Areas.CalcDisplay, 
		FONT_Calculator, ScreenForeColor, CalcBackColor};
	inline static TextLine	s_CalcText{MainScreen, MainScreen_Areas.CalcText, 
		FONT_CalcSmall, ScreenForeColor, CalcBackColor};
	inline static NumberLineBlankZ s_SettingDisplay{SettingsScreen, SettingsScreen_Areas.MaxRpm, 
		FONT_SettingsFont, SettingForeColor, SettingBackColor};
	inline static char		s_arEntryBuf[InBufSize] = " ";
	inline static char		s_arFormatBuf[InBufSize];
	inline static byte		s_indBuf = 1;
	inline static byte		s_op = OP_none;
	inline static byte		s_state;
	inline static bool		s_fHaveDp;
};
