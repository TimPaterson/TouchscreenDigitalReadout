//****************************************************************************
// ScreenMgr.h
//
// Created 12/15/2020 3:58:08 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876.h"


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


class Canvas : public Image
{
public:
	Canvas(ulong addr, ushort width, byte depth) : m_colorDepth{depth}
	{	
		m_imageAddress = addr; 
		m_imageWidth = width;
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
public:
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

static constexpr int ImageRegCount = sizeof(Image);
static constexpr int CanvasRegCount = ImageRegCount + 2 + 2;
static constexpr int CanvasViewRegCount = CanvasRegCount + 2 + 2;


class ScreenMgr : public RA8876
{
public:
	void SetMainImage(Canvas *pCanvas)
	{
		byte	val;

		m_pMainImage =  pCanvas;
		WriteSequentialRegisters(pCanvas, MISA0, CanvasRegCount);
		val = ReadReg(MPWCTR) & ~MPWCTR_MainImageColor_Mask;
		WriteData(val | (pCanvas->m_colorDepth << MPWCTR_MainImageColor_Shft));
	}

	void EnablePip1(Canvas *pCanvas, uint X, uint Y)
	{
		byte	val;

		m_pPip1Image = pCanvas;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip1 | MPWCTR_Pip1Enable));
		WriteRegXY(PWDULX0, X, Y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip1Color_Mask;
		WriteData(val | (pCanvas->m_colorDepth << PIPCDEP_Pip1Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	static void DisablePip1()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip1_Mask);
	}

	void EnablePip2(Canvas *pCanvas, uint X, uint Y)
	{
		byte	val;

		m_pPip1Image = pCanvas;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip2 | MPWCTR_Pip2Enable));
		WriteRegXY(PWDULX0, X, Y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip2Color_Mask;
		WriteData(val | (pCanvas->m_colorDepth << PIPCDEP_Pip2Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	static void DisablePip2()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip2_Mask);
	}

	static void SetBteDestination(Canvas *pCanvas)
	{
		byte	val;

		WriteSequentialRegisters(pCanvas, DT_STR0, CanvasRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_DestColor_Mask;
		WriteData(val | (pCanvas->m_colorDepth << BTE_COLR_DestColor_Shft));
	}

	static void SetBteSource0(Image *pImage, uint bpp)
	{
		byte	val;

		WriteSequentialRegisters(pImage, S0_STR0, ImageRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_Src0Color_Mask;
		WriteData(val | (bpp << BTE_COLR_DestColor_Shft));
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	Canvas	*m_pMainImage;
	Canvas	*m_pPip1Image;
	Canvas	*m_pPip2Image;
};
