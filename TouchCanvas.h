//****************************************************************************
// TouchCanvas.h
//
// Created 11/29/2020 4:25:06 PM by Tim
//
//****************************************************************************

#pragma once

#include "ScreenMgr.h"
#include "Hotspot.h"


class TouchCanvas : public Canvas
{
public:
	TouchCanvas(ulong addr, ushort width, byte depth, HotspotList *list) :
		Canvas(addr, width, depth),  m_pSpots{list} {}

public:
	HotspotData *TestHit(int x, int y)
	{
		int		i;

		x -= m_viewPosX;
		y -= m_viewPosY;
		if (x < 0 || y < 0)
			return NULL;

		for (i = 0; i < m_pSpots->count; i++)
		{
			if (x >= m_pSpots->HotspotList[i].X1 && x <= m_pSpots->HotspotList[i].X2 &&
				y >= m_pSpots->HotspotList[i].Y1 && y <= m_pSpots->HotspotList[i].Y2)
				return &m_pSpots->HotspotList[i].data;
		}

		return NULL;
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	HotspotList	*m_pSpots;
};
