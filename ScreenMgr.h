//****************************************************************************
// ScreenMgr.h
//
// Created 12/15/2020 3:58:08 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876.h"
#include "TouchCanvas.h"


class ScreenMgr : public RA8876
{
public:
	void SetMainImage(TouchCanvas *pCanvas)
	{
		byte	val;

		m_pMainImage =  pCanvas;
		WriteSequentialRegisters(pCanvas, MISA0, CanvasRegCount);
		val = ReadReg(MPWCTR) & ~MPWCTR_MainImageColor_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << MPWCTR_MainImageColor_Shft));
	}

	void EnablePip1(TouchCanvas *pCanvas, uint X, uint Y)
	{
		byte	val;

		m_pPip1Image = pCanvas;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip1 | MPWCTR_Pip1Enable));
		WriteRegXY(PWDULX0, X, Y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip1Color_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << PIPCDEP_Pip1Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	void DisablePip1()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip1_Mask);
		m_pPip1Image = NULL;
	}

	void EnablePip2(TouchCanvas *pCanvas, uint X, uint Y)
	{
		byte	val;

		m_pPip1Image = pCanvas;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip2 | MPWCTR_Pip2Enable));
		WriteRegXY(PWDULX0, X, Y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip2Color_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << PIPCDEP_Pip2Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	void DisablePip2()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip2_Mask);
		m_pPip2Image = NULL;
	}

	static void SetBteDestination(Canvas *pCanvas)
	{
		byte	val;

		WriteSequentialRegisters(pCanvas, DT_STR0, CanvasRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_DestColor_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << BTE_COLR_DestColor_Shft));
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
	TouchCanvas	*m_pMainImage;
	TouchCanvas	*m_pPip1Image;
	TouchCanvas	*m_pPip2Image;
};
