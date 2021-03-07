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
	static constexpr int BottomPosition = LcdHeightPx - TouchEdgeOffset - 1;
	static constexpr int RightPosition = LcdWidthPx - TouchEdgeOffset - 1;

	// Calibration
	static constexpr int AbortFlag = -1;
	static constexpr int TouchTimeMs = 500;

	// Colors
	static constexpr ulong TargetColor = 0;
	static constexpr ulong ButtonColor = 0x0000FF;
	static constexpr ulong ButtonTextColor = 0xFFFFFF;

	// Define the two big buttons to repeat or quite
	static constexpr int ButtonHeight = LcdHeightPx / 5;
	static constexpr int ButtonWidth = LcdWidthPx / 3;
	static constexpr int ButtonLeft = ButtonWidth;
	static constexpr int RepeatBtnTop = ButtonHeight;
	static constexpr int DoneBtnTop = ButtonHeight * 3;
	// 16 x 32 font is used with x2 multiplier
	#define REPEAT_BTN_LABEL	"Repeat"
	#define DONE_BTN_LABEL		"Done"
	static constexpr int TextHeight = 32 * 2;
	static constexpr int TextWidth = 16 * 2;	// per character
	// Positions within button
	static constexpr int TextTop = (ButtonHeight - TextHeight) / 2;
	static constexpr int RepeatTextLeft = (ButtonWidth - STRLEN(REPEAT_BTN_LABEL) * TextWidth) / 2;
	static constexpr int DoneTextLeft = (ButtonWidth - STRLEN(DONE_BTN_LABEL) * TextWidth) / 2;

	struct Target
	{
		Area	horz;
		Area	vert;

		uint GetX()		{ return vert.Xpos; }
		uint GetY()		{ return horz.Ypos; }
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
	TouchCalibrate_t() : TouchCanvas(0, LcdWidthPx, LcdHeightPx, LcdWidthPx, Color16bpp, NULL) {}

public:
	void Open()
	{
		int		flags;
		HotspotData	*pSpot;

		if (AllocIfNeeded(LcdHeightPx))
		{
			// New screen, build image
			FillRect(this, GetViewArea(), ScreenBackColor);
			SetHitList((HotspotList *)&s_hotSpots);
		}
		m_oldImage = GetMainImage();
		SetMainImage(this);
		DisablePip1();
		DisablePip2();

		for (;;) 
		{

			if (StartCalibration())
				break;

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
		ulong color = fErase ? ScreenBackColor : ButtonColor;
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
				return true;
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

		if (Touch.IsTouchDataValid(&Eeprom.Data.TouchInit))
			Touch.CalcScales(&Eeprom.Data.TouchInit, LcdWidthPx, LcdHeightPx);

		// Now erase the target
		DrawTarget(target, true);
		return false;
	}

	bool StartCalibration()
	{
		if (CalibrateTarget(s_targetTopLeft, Eeprom.Data.TouchInit.topLeft))
			return true;	// abort
		return CalibrateTarget(s_targetBottomRight, Eeprom.Data.TouchInit.bottomRight);
	}

	static int GetTouch(bool fShow = false)
	{
		uint	flags;

		while (!(Touch.Process()))
		{
			wdt_reset();
			if (Console.IsByteReady())
				return AbortFlag;
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

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const char s_archRepeat[] = REPEAT_BTN_LABEL;
	inline static const char s_archDone[] = DONE_BTN_LABEL;

	inline static const Target s_targetTopLeft =
	{
		{ 0, TouchEdgeOffset, LcdWidthPx, 1 },
		{ TouchEdgeOffset, 0, 1, LcdHeightPx }
	};

	inline static const Target s_targetBottomRight =
	{
		{ 0, BottomPosition, LcdWidthPx, 1 },
		{ RightPosition, 0, 1, LcdHeightPx }
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
	static void Open()		{ s_calibrate.Open(); }
protected:
	inline static TouchCalibrate_t	s_calibrate;
};
