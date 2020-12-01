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
	void Init(ulong address, uint width, uint height, ColorDepths colorDepth)
	{
		m_imageAddress = address;
		m_imageWidth = width;
		m_colorDepth = colorDepth;
		m_length = width * height * (colorDepth + 1);
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

		WriteReg32(addr, m_imageAddress);
		WriteReg16(addr + 4, m_imageWidth);
		WriteRegXY(addr + 6, m_viewPosX, m_viewPosY);
	}

	//ColorDepths GetColorDepth()	{ return (ColorDepths)ColorDepth; }

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	ulong		m_imageAddress;
	ulong		m_length;
	ushort		m_imageWidth;
	ushort		m_viewPosX;
	ushort		m_viewPosY;
	byte		m_colorDepth;
};
