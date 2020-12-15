//****************************************************************************
// Canvas.h
//
// Created 11/29/2020 9:58:29 AM by Tim
//
//****************************************************************************

#pragma once


enum ColorDepths
{
	Color8bpp,
	Color16bpp,
	Color24bpp
};


class Canvas : public RA8876
{
public:
	Canvas() {}
	Canvas(ulong addr, ushort width, byte depth) : 
		m_imageAddress{addr}, m_imageWidth{width}, m_colorDepth{depth} {}

	void Init(ulong address, uint width, uint height, ColorDepths colorDepth)
	{
		m_imageAddress = address;
		m_imageWidth = width;
		m_colorDepth = colorDepth;
	}

	void SetViewport(uint X, uint Y)
	{
		m_viewPosX = X;
		m_viewPosY = Y;
	}

	void SetCanvas(uint addr)
	{
		// Canvas registers all fall into the same sequence:
		// 4 - Image start
		// 2 - Image width
		//
		// The sequence applies to:
		// - Main image (MISA0)
		// - PIP image (PISA0)
		// - Canvas (CVSSA0)
		// - BTE source 0 (S0_STR0)
		// - BTE source 1 (S1_STR0)
		// - BTE destination (DT_STR0)

		WriteReg32(addr, m_imageAddress);
		WriteReg16(addr + 4, m_imageWidth);
	}

	void SetCanvasView(uint addr)
	{
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

		SetCanvas(addr);
		WriteRegXY(addr + 6, m_viewPosX, m_viewPosY);
	}

	void SetCanvasViewSize(uint addr)
	{
		// Canvas view registers all fall into the same sequence:
		// 10 - Canvas registers
		// 2 - Viewport width
		// 2 - Viewport height
		//
		// The sequence applies to:
		// - PIP image (PISA0)
		// - Canvas (CVSSA0)
		// - BTE destination (DT_STR0)

		SetCanvasView(addr);
		WriteRegXY(addr + 10, m_viewWidth, m_viewHeight);
	}

	//ColorDepths GetColorDepth()	{ return (ColorDepths)ColorDepth; }

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
