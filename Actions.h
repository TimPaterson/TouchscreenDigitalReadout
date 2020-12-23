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
public:
	static void TakeAction(int X, int Y)
	{
		HotspotData	*pSpot;
		uint		group;
		uint		spot;

		pSpot = ScreenMgr::TestHit(X, Y);
		if (pSpot == NULL)
			return;

		group = pSpot->group;
		spot = pSpot->id;

		if (group != HOTSPOT_GROUP_Keyboard)
		{
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
		}
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
};
