//****************************************************************************
// TouchCanvas.h
//
// Created 11/29/2020 4:25:06 PM by Tim
//
//****************************************************************************

#pragma once

#include "Hotspot.h"


enum ColorDepths
{
	Color8bpp,
	Color16bpp,
	Color24bpp
};


struct Image
{
	ulong		m_imageAddress;
	ushort		m_imageWidth;
};

struct ColorImage
{
	byte GetColorDepth() const	{ return m_colorDepth; }

	ulong		m_imageAddress;
	ushort		m_imageWidth;
	byte		m_colorDepth;
};

struct Area
{
	ushort	Xpos;
	ushort	Ypos;
	ushort	Width;
	ushort	Height;
};


class Canvas
{
public:
	Canvas(ulong addr, ushort width, ushort height, byte depth) : 
		m_imageAddress{addr}, m_imageWidth{width}, m_viewWidth{width},
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
	ulong		m_imageAddress;
	ushort		m_imageWidth;
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
//
// The Canvas registers (CVSSA0) also have color depth at the end, and the
// m_colorDepth member can be used directly.

static constexpr int ImageRegCount = 4 + 2;
static constexpr int CanvasRegCount = ImageRegCount + 2 + 2;
static constexpr int CanvasViewRegCount = CanvasRegCount + 2 + 2;
static constexpr int CanvasViewDepthRegCount = CanvasViewRegCount + 1;


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

	bool OnCanvas(int x, int y)
	{
		x -= m_viewPosX;
		y -= m_viewPosY;
		if (x < 0 || y < 0 || x >= m_viewWidth || y >= m_viewHeight)
			return false;
		return true;
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	HotspotList	*m_pSpots;
};
