//****************************************************************************
// TouchCalibrate.h
//
// Created 3/5/2021 3:00:10 PM by Tim
//
//****************************************************************************

#pragma once

#include "ScreenMgr.h"


class TouchCalibrate_t : TouchCanvas, ScreenMgr
{
	// Calibration target position
	static constexpr int TouchEdgeOffsetX = ScreenWidth / 10;
	static constexpr int TouchEdgeOffsetY = ScreenHeight / 10;
	static constexpr int RightTarget = ScreenWidth - TouchEdgeOffsetX - 1;
	static constexpr int BottomTarget = ScreenHeight - TouchEdgeOffsetY - 1;
	static constexpr int MiddleTargetX = ScreenWidth / 2;
	static constexpr int MiddleTargetY = ScreenHeight / 2;

	// Calibration
	static constexpr int AbortFlag = -1;
	static constexpr int TouchTimeMs = 500;

	// Colors
	static constexpr ulong TargetColor = 0;
	static constexpr ulong ButtonColor = 0x0000FF;
	static constexpr ulong ButtonTextColor = 0xFFFFFF;
	static constexpr ulong BackColor = ScreenBackColor;

	// Define the two big buttons to repeat or quite
	static constexpr int ButtonHeight = ScreenHeight / 5;
	static constexpr int ButtonWidth = ScreenWidth / 3;
	static constexpr int ButtonLeft = ButtonWidth;
	static constexpr int RepeatBtnTop = ButtonHeight;
	static constexpr int DoneBtnTop = ButtonHeight * 3;
	// 16 x 32 font is used with x2 multiplier
	#define REPEAT_BTN_LABEL	"Calibrate"
	#define DONE_BTN_LABEL		"Done"
	static constexpr int TextHeight = 32 * 2;
	static constexpr int TextWidth = 16 * 2;	// per character
	// Positions within button
	static constexpr int TextTop = (ButtonHeight - TextHeight) / 2;
	static constexpr int RepeatTextLeft = (ButtonWidth - STRLEN(REPEAT_BTN_LABEL) * TextWidth) / 2;
	static constexpr int DoneTextLeft = (ButtonWidth - STRLEN(DONE_BTN_LABEL) * TextWidth) / 2;

	struct Target
	{
		// For initializing
		#define TARGET(x, y) {{x, 0, 1, ScreenHeight}/*vert*/, {0, y, ScreenWidth, 1}/*horz*/}

		Area	vert;
		Area	horz;

		uint GetX()		{ return vert.Xpos; }
		uint GetY()		{ return horz.Ypos; }
	};

	#define TargX(n)	s_Targets[n].vert.Xpos
	#define TargY(n)	s_Targets[n].horz.Ypos

	struct TouchPoint
	{
		int		x;
		int		y;
	};

	// Imitate HotpostList
	struct CalibrateHotspotList
	{
		ushort	count;
		Hotspot	list[2];
	};

	enum CalibrateHotspots
	{
		HOTSPOT_Repeat,
		HOTSPOT_Done,
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************
public:
	TouchCalibrate_t() : TouchCanvas(0, ScreenWidth, ScreenHeight, ScreenWidth, Color16bpp, NULL) {}

public:
	void Open(bool fShowButtons = false)
	{
		int		flags;
		HotspotData	*pSpot;

		if (AllocIfNeeded(ScreenHeight))
		{
			// New screen, build image
			FillRect(this, GetViewArea(), BackColor);
			SetHitList((HotspotList *)&s_hotSpots);
		}
		m_oldImage = GetMainImage();
		SetMainImage(this);
		DisablePip1();
		DisablePip2();

		if (fShowButtons)
			goto ShowButtons;

		for (;;) 
		{

			if (StartCalibration())
				break;

ShowButtons:
			DrawButtons();
			for (;;) 
			{
				flags = GetTouch(true);
				if (flags == AbortFlag)
					goto Abort;

				if (flags & TOUCH_Start)
				{
					pSpot = this->TestHit(Touch.GetX(), Touch.GetY());
					if (pSpot != NULL && pSpot != NOT_ON_CANVAS)
					{
						if (pSpot->id == HOTSPOT_Done)
							goto Abort;
						else
						{
							DrawButtons(true);
							break;	// repeat
						}
					}
				}
			}
		} // for(;;) repeat calibration

Abort:
		DisableGraphicsCursor();				
		Eeprom.StartSave();
		DrawButtons(true);
		SetMainImage(m_oldImage);
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	void DrawButtons(bool fErase = false)
	{
		ulong color = fErase ? BackColor : ButtonColor;
		FillRect(this, &s_areaRepeat, color);
		FillRect(this, &s_areaDone, color);

		if (!fErase)
		{
			// Write the labels
			SetDrawCanvas(this);
			SetForeColor(ButtonTextColor);
			InternalFont(CCR0_CharHeight32, CCR0_CharSet8859_1);
			WriteReg(CCR1, CCR1_CharHeightX2 | CCR1_CharWidthX2 | CCR1_CharBackgroundTransparent);
			SetTextPosition(ButtonLeft + RepeatTextLeft, RepeatBtnTop + TextTop);
			WriteString(s_archRepeat);
			SetTextPosition(ButtonLeft + DoneTextLeft, DoneBtnTop + TextTop);
			WriteString(s_archDone);
		}
	}

	void DrawTarget(const Target &target, bool fErase = false)
	{
		ulong color = fErase ? ScreenBackColor : TargetColor;
		FillRect(this, &target.horz, color);
		FillRect(this, &target.vert, color);
	}

	bool CalibrateTarget(const Target &target, TouchPoint &point)
	{
		Timer	tmr;
		int		flags;

		DrawTarget(target);

Restart:
		do 
		{
			flags = GetTouch();
			if (flags == AbortFlag)
			{
				DrawTarget(target, true);	// Erase it
				return true;
			}
		} while (!(flags & TOUCH_Start));

		// Screen touched, make 'em hold it
		tmr.Start();
		do 
		{
			flags = GetTouch();
			if (flags == AbortFlag)
				return true;
			if (!(flags & TOUCH_Touched))
				goto Restart;
		} while (!tmr.CheckDelay_ms(TouchTimeMs));

		// Held on target for required time, use final value
		point.x = Touch.GetRawX();
		point.y = Touch.GetRawY();

		// Now erase the target
		DrawTarget(target, true);
		return false;
	}

	static long DivLongLong(long long num, long den) NO_INLINE_ATTR
	{
		return ShiftIntRnd(num * 2 / den, 1);
	}

	static long DivScale(long num, long den)
	{
		return DivLongLong((long long)num << TouchShift, den);
	}

	bool StartCalibration()
	{
		int		val, k;
		long long	c1, c2, c3, baseX, baseY;

		if (CalibrateTarget(s_Targets[0], m_points[0]))
			return true;
		if (CalibrateTarget(s_Targets[1], m_points[1]))
			return true;
		if (CalibrateTarget(s_Targets[2], m_points[2]))
			return true;

		// Equations for 3-point touch calibration were found at
		// https://www.embedded.com/how-to-calibrate-touch-screens/
		//
		// A .pdf from Texas Instruments called "Calibration in touch-screen systems"
		// also has these equations with an explanation of how to extend it
		// to more points, with an example of 5 points.

		k = (m_points[0].x - m_points[2].x) * (m_points[1].y - m_points[2].y) -
			(m_points[1].x - m_points[2].x) * (m_points[0].y - m_points[2].y);

		val = (TargX(0) - TargX(2)) * (m_points[1].y - m_points[2].y) -
			(TargX(1) - TargX(2)) * (m_points[0].y - m_points[2].y);
		Eeprom.Data.TouchInit.scaleX.aScale = DivScale(val, k);

		val = (TargX(1) - TargX(2)) * (m_points[0].x - m_points[2].x) -
			(TargX(0) - TargX(2)) * (m_points[1].x - m_points[2].x);
		Eeprom.Data.TouchInit.scaleX.bScale = DivScale(val, k);

		val = (TargY(1) - TargY(2)) * (m_points[0].x - m_points[2].x) -
			(TargY(0) - TargY(2)) * (m_points[1].x - m_points[2].x);
		Eeprom.Data.TouchInit.scaleY.aScale = DivScale(val, k);

		val = (TargY(0) - TargY(2)) * (m_points[1].y - m_points[2].y) -
			(TargY(1) - TargY(2)) * (m_points[0].y - m_points[2].y);
		Eeprom.Data.TouchInit.scaleY.bScale = DivScale(val, k);

		// Final calculation exceeds int, use long long
		c1 = m_points[1].x * m_points[2].y - m_points[2].x * m_points[1].y;
		c2 = m_points[2].x * m_points[0].y - m_points[0].x * m_points[2].y;
		c3 = m_points[0].x * m_points[1].y - m_points[1].x * m_points[0].y;
		baseX = c1 * TargX(0) + c2 * TargX(1) + c3 * TargX(2);
		baseY = c1 * TargY(0) + c2 * TargY(1) + c3 * TargY(2);

		// compute rounded 32-bit result of division
		val = DivLongLong(baseX, k);
		Eeprom.Data.TouchInit.scaleX.base = val;

		val = DivLongLong(baseY, k);
		Eeprom.Data.TouchInit.scaleY.base = val;

		return false;
	}

	static int GetTouch(bool fShow = false)
	{
		uint	flags;

		while (!(Touch.Process()))
		{
			wdt_reset();
			if (Console.IsByteReady())
			{
				Console.DiscardReadBuf();
				return AbortFlag;
			}
		}
		flags = Touch.GetTouch();

		// show cursor at touch position
		if (fShow && flags & TOUCH_Touched)
		{
			EnableGraphicsCursor(GTCCR_GraphicCursorSelect2);
			SetGraphicsCursorPosition(Touch.GetX() - 16, Touch.GetY() - 16);
		}
		else
			DisableGraphicsCursor();				

		return flags;
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	TouchCanvas *m_oldImage;
	TouchPoint	m_points[3];

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const char s_archRepeat[] = REPEAT_BTN_LABEL;
	inline static const char s_archDone[] = DONE_BTN_LABEL;
	inline static const ScaleMatrix s_calibMatrix = { TouchScale, 0, 0 };

	inline static const Target s_Targets[3] =
	{
		TARGET(TouchEdgeOffsetX, BottomTarget),
		TARGET(MiddleTargetX, TouchEdgeOffsetY),
		TARGET(RightTarget, MiddleTargetY),
	};

	inline static const Area s_areaRepeat = { ButtonLeft, RepeatBtnTop, ButtonWidth, ButtonHeight };
	inline static const Area s_areaDone = { ButtonLeft, DoneBtnTop, ButtonWidth, ButtonHeight };

	inline static const CalibrateHotspotList s_hotSpots = { 2, {
		// Array of Hotspots: the display area, and the scroll thumb
		{ButtonWidth, ButtonHeight, ButtonWidth * 2 - 1, ButtonHeight * 2 - 1,
			{HOTSPOT_Repeat, 0}},
		{ButtonWidth, ButtonHeight * 3, ButtonWidth * 2 - 1, ButtonHeight * 4 - 1,
			{HOTSPOT_Done, 0}}
	}};
};


// Create a single static instance of the above class
class TouchCalibrate
{
public:
	static void Open(bool fShowButtons = false)		{ s_calibrate.Open(fShowButtons); }
protected:
	inline static TouchCalibrate_t	s_calibrate;
};
