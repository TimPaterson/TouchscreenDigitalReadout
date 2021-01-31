//****************************************************************************
// Actions.h
//
// Created 12/20/2020 12:51:43 PM by Tim
//
//****************************************************************************

#pragma once

#include "ScreenMgr.h"
#include "AxisDisplay.h"
#include "ToolLib.h"


class Actions
{
	static constexpr int InBufSize = 12;
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

	//*********************************************************************
	// Local types
	//*********************************************************************
protected:
	class CalcMemory : public NumberLineBlankZ
	{
	public:
		CalcMemory(const Area *pArea, ulong backColor) :
			NumberLineBlankZ(&MainScreen, pArea, FID_CalcSmall, 0, backColor)
		{}

	public:
		double GetVal()	{ return m_val; }

		void SetVal(double val)
		{
			m_val = val;
			PrintDbl("%.8g\n", val);
		}

	protected:
		double	m_val;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************
public:
	static void Init()
	{
		ShowAbsInc();
		ShowInchMetric();
		ShowToolInfo();
		ToolLib::Init();
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
				pPipInfo = ScreenMgr::GetPip(s_pCapture);
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
		char		*pStr;
		bool		*pToggle;

		pSpot = ScreenMgr::TestHit(x, y);
		if (pSpot == NULL)
			return;

		group = pSpot->group;
		spot = pSpot->id;

		if (group < HOTSPOT_GROUP_CaptureEnd)
		{
			switch (group)
			{
			case HOTSPOT_GROUP_ToolDisplay:
				s_pCapture = ToolLib::ListCapture(x, y, (ScrollAreas)spot);
				break;

			default:
				return;
			}
			return;
		}

		switch (group)
		{

		//*****************************************************************
		// Dispatch to other handlers
		//

		case HOTSPOT_GROUP_ToolLib:
			ToolLib::ToolAction(spot);
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
			pSensor = s_arSensor[spot];

			if (s_state == AS_Empty)
				ToValueState(pSensor->GetPosition());
			else
				pSensor->SetPosition(ToValueState());
			break;

		//*****************************************************************
		// Press one of the axis zero buttons.
		//

		case HOTSPOT_GROUP_AxisButton:
			s_arSensor[spot]->SetPosition(0);
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
		// Press a tool info key
		//

		case HOTSPOT_GROUP_ToolInfo:
			if (s_state == AS_Empty)
			{
				// Just reading the value
				switch (spot)
				{
					case ToolDiameter:
						val = Eeprom.Data.ToolDiameter;
						break;

					case ToolLength:
						val = Eeprom.Data.ToolLength;
						break;

					case ToolFlutes:
						val = Eeprom.Data.ToolFlutes;
						break;

					case ToolSfm:
						val = Eeprom.Data.Sfm;
						break;

					case ToolChipLoad:
						val = Eeprom.Data.ChipLoad;
						break;

					default:	// ToolNumber
						val = Eeprom.Data.ToolNumber;
						break;
				}
				if (val != 0)
					ToValueState(val);
				break;
			}
			else
			{
				// Setting the value
				val = ToValueState();
				// Negative values never allowed
				if (val < 0)
					val = -val;

				switch (spot)
				{
					case ToolDiameter:
						Eeprom.Data.ToolDiameter = LimitVal(val, 9.999);
						break;

					case ToolLength:
						Eeprom.Data.ToolLength = LimitVal(val, 99.999);
						break;

					case ToolChipLoad:
						Eeprom.Data.ChipLoad = LimitVal(val, 0.9999);
						break;

					case ToolFlutes:
						val = std::min(val, 99.0);
						Eeprom.Data.ToolFlutes = (byte)val;
						break;

					case ToolSfm:
						val = std::min(val, 9999.0);
						val = Eeprom.Data.Sfm = (ushort)val;
						break;

					default:	// ToolNumber
						val = std::min(val, 511.0);
						val = Eeprom.Data.ToolNumber = (ushort)val;
						break;
				}
				UpdateEeprom();
				ShowToolInfo();
			}
			return;

		//*****************************************************************
		// Press a tool cutting side key
		//

		case HOTSPOT_GROUP_ToolSide:
			s_toolSides = (s_toolSides & ~(spot >> ToolMaskShift)) ^ spot;
			ShowToolInfo();
			return;

		//*****************************************************************
		// Settings screen
		//

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
			ShowToolInfo();
			return;

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
			case ToolMenu:
				ToolLib::ShowToolLib();
				break;

			case InchMetric:
				Eeprom.Data.fIsMetric ^= true;
				ConvertToolValues(Eeprom.Data.fIsMetric);
				UpdateEeprom();
				ShowInchMetric();
				break;

			case AbsInc:
				Eeprom.Data.OriginNum ^= 1;
				UpdateEeprom();
				ShowAbsInc();
				break;

			case Settings:
				if (!ScreenMgr::GetPip2()->IsEnabled())
				{
					ScreenMgr::EnablePip2(&SettingsScreen, 0, 0);
					ShowSettingsInfo();
				}
				else
				{
					ScreenMgr::DisablePip2();
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

public:
	static void ShowInchMetric()
	{
		SelectImage(&MainScreen_Areas.InchMetric, &InchMetricBtn, IsMetric());
		SelectImage(&MainScreen_Areas.SpeedDisplay, &SpeedDisplay, IsMetric());

		ShowToolInfo();
	}

public:
	static void ShowAbsInc()
	{
		SelectImage(&MainScreen_Areas.AbsInc, &Coord, Eeprom.Data.OriginNum);
	}

public:
	static void ShowToolInfo()
	{
		double	val;
		uint	sides;
		ulong	color;

		s_ToolDisplay.PrintDbl(
			IsMetric() ? "%5.2f" : "%5.3f",
			Eeprom.Data.ToolDiameter,
			&MainScreen_Areas.ToolDiameter);

		s_ToolDisplay.PrintDbl(
			IsMetric() ? "%6.2f" : "%6.3f",
			Eeprom.Data.ToolLength,
			&MainScreen_Areas.ToolLength);

		s_ToolDisplay.PrintDbl(
			IsMetric() ? "%6.3f" : "%6.4f",
			Eeprom.Data.ChipLoad,
			&MainScreen_Areas.ChipLoad);

		s_ToolDisplay.PrintUint("%3u", Eeprom.Data.ToolFlutes, &MainScreen_Areas.ToolFlutes);

		s_ToolDisplay.PrintUint("%5u", Eeprom.Data.Sfm, &MainScreen_Areas.Sfm);

		// Compute and display RPM
		if (Eeprom.Data.ToolDiameter != 0 && Eeprom.Data.Sfm != 0)
		{
			val = Eeprom.Data.Sfm / (Eeprom.Data.ToolDiameter * M_PI);
			val *= IsMetric() ? 1000 : 12;
			val = std::min(val, (double)Eeprom.Data.MaxRpm);
		}
		else
			val = 0;

		s_ToolDisplay.PrintUint("%5u", (uint)val, &MainScreen_Areas.Rpm);

		// Compute and display feed rate
		val *= Eeprom.Data.ChipLoad * Eeprom.Data.ToolFlutes;
		s_ToolDisplay.PrintUint("%5u", (uint)val, &MainScreen_Areas.FeedRate);

		// Update cutter radius offset
		PrepareDrawTool();
		sides = s_toolSides;
		DrawTool(sides & ToolLeftBit,  ToolLeft_X,  ToolLeft_Y);
		DrawTool(sides & ToolRightBit, ToolRight_X, ToolRight_Y);
		DrawTool(sides & ToolBackBit,  ToolBack_X,  ToolBack_Y);
		DrawTool(sides & ToolFrontBit, ToolFront_X, ToolFront_Y);

		val = Eeprom.Data.ToolDiameter / 2;
		Xaxis.SetOffset(sides & ToolLeftBit ? val : (sides & ToolRightBit ? -val : 0));
		if (Eeprom.Data.fCncCoordinates)
			val = -val;
		Yaxis.SetOffset(sides & ToolFrontBit ? -val : (sides & ToolBackBit ? val : 0));
		val = Eeprom.Data.ToolLength;
		if (Eeprom.Data.fCncCoordinates)
			val = -val;
		Zaxis.SetOffset(Eeprom.Data.fToolLenAffectsZ ? val : 0);

		color = Eeprom.Data.fHighlightOffset && Eeprom.Data.ToolDiameter != 0 ? ToolColor : AxisForeColor;
		Xaxis.SetForeColor(sides & (ToolLeftBit | ToolRightBit) ? color : AxisForeColor);
		Yaxis.SetForeColor(sides & (ToolBackBit | ToolFrontBit) ? color : AxisForeColor);
	}

	//*********************************************************************
	// Helper functions - Main screen
	//*********************************************************************
protected:
	static void SelectImage(const Area *pAreaDst, const ColorImage *pSrc, uint index)
	{
		ScreenMgr::SelectImage(&MainScreen, pAreaDst, pSrc, index);
	}

	static bool IsMetric()
	{
		return Eeprom.Data.fIsMetric;
	}

	static void UpdateEeprom()
	{
		//Eeprom.StartSave(); // UNDONE: enable auto save to EEPROM
	}

	static double LimitVal(double val, double max) NO_INLINE_ATTR
	{
		if (IsMetric())
			max *= 10.0;
		return std::min(val, max);
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

	static void ConvertToolValues(bool fToMetric)
	{
		double	factor;

		factor = fToMetric ? MmPerInch : 1.0 / MmPerInch;

		Eeprom.Data.ToolDiameter *= factor;
		Eeprom.Data.ToolLength *= factor;
		Eeprom.Data.ChipLoad *= factor;

		factor = MmPerInch * 12.0 / 1000.0;	// meters / foot
		if (!fToMetric)
			factor = 1 / factor;
		Eeprom.Data.Sfm = lround(Eeprom.Data.Sfm * factor);
	}

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
	// Helper functions - Settings screen
	//*********************************************************************
protected:
	static void SettingsCheckBox(const Area *pAreaDst, bool f)
	{
		ScreenMgr::SelectImage(&SettingsScreen, pAreaDst, &CheckBox, f);
	}

	static void ShowAxisInfo(AxisInfo axis, const Area *pRes, const Area *pCorrect, const Area *pInvert)
	{
		double	val;

		SettingsCheckBox(pInvert, axis.Direction);

		s_SettingDisplay.PrintUint("%i", (uint)axis.Resolution, pRes);

		val = (axis.Correction - 1.0) * 1E6;
		s_SettingDisplay.PrintDbl("%+6.1f", val, pCorrect);
	}

	static void ShowSettingsInfo()
	{
		ShowAxisInfo(Eeprom.Data.XaxisInfo, &SettingsScreen_Areas.Xresolution,
			&SettingsScreen_Areas.Xcorrection, &SettingsScreen_Areas.Xinvert);

		ShowAxisInfo(Eeprom.Data.YaxisInfo, &SettingsScreen_Areas.Yresolution,
			&SettingsScreen_Areas.Ycorrection, &SettingsScreen_Areas.Yinvert);

		ShowAxisInfo(Eeprom.Data.ZaxisInfo, &SettingsScreen_Areas.Zresolution,
			&SettingsScreen_Areas.Zcorrection, &SettingsScreen_Areas.Zinvert);

		ShowAxisInfo(Eeprom.Data.QaxisInfo, &SettingsScreen_Areas.Qresolution,
			&SettingsScreen_Areas.Qcorrection, &SettingsScreen_Areas.Qinvert);

		SettingsCheckBox(&SettingsScreen_Areas.HighlightXY, Eeprom.Data.fHighlightOffset);
		SettingsCheckBox(&SettingsScreen_Areas.OffsetZ, Eeprom.Data.fToolLenAffectsZ);
		SettingsCheckBox(&SettingsScreen_Areas.CncCoordinates, Eeprom.Data.fCncCoordinates);

		s_SettingDisplay.PrintUint("%5i", Eeprom.Data.MaxRpm, &SettingsScreen_Areas.MaxRpm);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

	inline static PosSensor	*const s_arSensor[4] = { &Xaxis, &Yaxis, &Zaxis, &Qpos };

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static double	s_arg1;
	inline static ListScroll	*s_pCapture;

	inline static CalcMemory s_memories[MemoryCount]
	{
		{&MainScreen_Areas.Mem1, MemColorOdd},
		{&MainScreen_Areas.Mem2, MemColorEven},
		{&MainScreen_Areas.Mem3, MemColorOdd},
		{&MainScreen_Areas.Mem4, MemColorEven},
	};
	inline static NumberLine s_CalcDisplay{&MainScreen, &MainScreen_Areas.CalcDisplay, FID_Calculator, ScreenForeColor, CalcBackColor};
	inline static TextLine	s_CalcText{&MainScreen, &MainScreen_Areas.CalcText, FID_CalcSmall, ScreenForeColor, CalcBackColor};
	inline static NumberLineBlankZ s_ToolDisplay{&MainScreen, NULL, FID_CalcSmall, ScreenForeColor, ScreenBackColor};
	inline static NumberLineBlankZ s_SettingDisplay{&SettingsScreen, NULL, FID_SettingsFont, SettingForeColor, SettingBackColor};
	inline static char		s_arEntryBuf[InBufSize] = "\n ";
	inline static byte		s_indBuf = 2;
	inline static byte		s_op = OP_none;
	inline static byte		s_state;
	inline static bool		s_fHaveDp;
	inline static byte		s_toolSides;
};
