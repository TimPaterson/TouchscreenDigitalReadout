//****************************************************************************
// TouchCanvas.h
//
// Created 11/29/2020 4:25:06 PM by Tim
//
//****************************************************************************

#pragma once

#include "Hotspot.h"
#include "FontInfo.h"	// for definition of Image struct


enum ColorDepths
{
	Color8bpp,
	Color16bpp,
	Color24bpp
};


class Image
{
public:
	Image(ulong addr, ushort width) : 
		m_imageAddress{addr}, m_imageWidth{width} {}

protected:
	ulong		m_imageAddress;
	ushort		m_imageWidth;
};


class Canvas : Image
{
public:
	Canvas(ulong addr, ushort width, ushort height, byte depth) : 
		Image(addr, width), m_viewWidth{width}, 
		m_viewHeight{height}, m_colorDepth{depth} {}

public:
	byte GetColorDepth()	{ return m_colorDepth; }

	void SetWindowSize(uint width, uint height)
	{
		m_viewWidth = width;
		m_viewHeight = height;
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	ushort		m_viewPosX;
	ushort		m_viewPosY;
	ushort		m_viewWidth;
	ushort		m_viewHeight;
	byte		m_colorDepth;
};

// Canvas registers all fall into the same sequence:
// 4 - Image start
// 2 - Image width
// 2 - Viewport X
// 2 - Viewport Y
//
// The sequence applies to:
// - Main image (MISA0)
// - PIP image (PISA0)
// - Canvas (CVSSA0)
// - BTE source 0 (S0_STR0)
// - BTE source 1 (S1_STR0)
// - BTE destination (DT_STR0)
//
// Sometimes the size of the view window is also needed, tacked on to the sequence:
// 10 - Canvas registers
// 2 - Viewport width
// 2 - Viewport height
//
// The sequence applies to:
// - PIP image (PISA0)
// - Canvas (CVSSA0)
// - BTE destination (DT_STR0)

static constexpr int ImageRegCount = 4 + 2;
static constexpr int CanvasRegCount = ImageRegCount + 2 + 2;
static constexpr int CanvasViewRegCount = CanvasRegCount + 2 + 2;


class TouchCanvas : public Canvas
{
public:
	TouchCanvas(ulong addr, ushort width, ushort height, byte depth, HotspotList *list) :
		Canvas(addr, width, height, depth),  m_pSpots{list} {}

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
