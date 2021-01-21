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
	static const int BorderThickness = 4;

public:
	static TouchCanvas *GetPip1()	{ return s_pPip1Image; }
	static TouchCanvas *GetPip2()	{ return s_pPip2Image; }

	static void SetMainImage(TouchCanvas *pCanvas)
	{
		byte	val;

		s_pMainImage =  pCanvas;
		WriteSequentialRegisters(pCanvas, MISA0, CanvasRegCount);
		val = ReadReg(MPWCTR) & ~MPWCTR_MainImageColor_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << MPWCTR_MainImageColor_Shft));
	}

	static void EnablePip1(TouchCanvas *pCanvas, uint x, uint y, bool fModal = false)
	{
		byte	val;

		s_pPip1Image = pCanvas;
		s_pip1x = x;
		s_pip1y = y;
		s_fPip1Modal = fModal;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip1 | MPWCTR_Pip1Enable));
		WriteRegXY(PWDULX0, x, y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip1Color_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << PIPCDEP_Pip1Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	static void DisablePip1()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip1_Mask);
		s_pPip1Image = NULL;
	}

	static void EnablePip2(TouchCanvas *pCanvas, uint x, uint y)
	{
		byte	val;

		s_pPip2Image = pCanvas;
		s_pip2x = x;
		s_pip2y = y;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip2 | MPWCTR_Pip2Enable));
		WriteRegXY(PWDULX0, x, y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip2Color_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << PIPCDEP_Pip2Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	static void DisablePip2()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip2_Mask);
		s_pPip2Image = NULL;
	}

	static void SetBteDest(Canvas *pCanvas)
	{
		byte	val;

		WriteSequentialRegisters(pCanvas, DT_STR0, ImageRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_DestColor_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << BTE_COLR_DestColor_Shft));
	}

	static void SetDrawCanvas(Canvas *pCanvas)
	{
		WriteSequentialRegisters(pCanvas, CVSSA0, CanvasViewDepthRegCount);
	}

	static void SetBteSrc0(const Image *pImage, uint bpp)
	{
		byte	val;

		WriteSequentialRegisters(pImage, S0_STR0, ImageRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_Src0Color_Mask;
		WriteData(val | (bpp << BTE_COLR_Src0Color_Shft));
	}

	static void SetBteSrc1(const Image *pImage, uint bpp)
	{
		byte	val;

		WriteSequentialRegisters(pImage, S1_STR0, ImageRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_Src1Color_Mask;
		WriteData(val | (bpp << BTE_COLR_Src1Color_Shft));
	}

	static HotspotData *TestHit(int x, int y)
	{
		HotspotData	*pSpot;

		// PIP1 is on top if enabled. If PIP1 is modal, don't look elsewhere
		if (s_pPip1Image != NULL)
		{
			pSpot = s_pPip1Image->TestHit(x - s_pip1x, y - s_pip1y);
			if (pSpot != NOT_ON_CANVAS)
				return pSpot;
			if (s_fPip1Modal)
				return NULL;
		}

		// PIP2 is next if enabled
		if (s_pPip2Image != NULL)
		{
			pSpot = s_pPip2Image->TestHit(x - s_pip2x, y - s_pip2y);
			if (pSpot != NOT_ON_CANVAS)
				return pSpot;
		}

		pSpot = s_pMainImage->TestHit(x, y);
		if (pSpot == NOT_ON_CANVAS)
		{
			DEBUG_PRINT("Unexpected off-screen touch coordinates\n");
			return NULL;
		}
		return pSpot;
	}

	static void CopyRect(const ColorImage *pSrc, uint srcX, uint srcY, Canvas *pDst, const Area *pAreaDst)
	{
		SetBteSrc0((Image *)pSrc, pSrc->GetColorDepth());
		WriteRegXY(S0_X0, srcX, srcY);
		SetBteDest(pDst);
		WriteSequentialRegisters(pAreaDst, DT_X0, sizeof *pAreaDst);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeMemoryCopyWithRop | BTE_CTRL1_RopS0);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	static void FillRect(Canvas *pDst, const Area *pAreaDst, ulong color)
	{
		SetBteDest(pDst);
		SetForeColor(color);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeSolidFill);
		WriteSequentialRegisters(pAreaDst, DT_X0, sizeof *pAreaDst);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	static void RectBorder(Canvas *pDst, const Area *pAreaDst, const ColorImage *pSrc)
	{
		SetBteSrc0((Image *)pSrc, pSrc->GetColorDepth());
		WriteRegXY(S0_X0, 0, 0);
		SetBteDest(pDst);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodePatternFillWithRop | BTE_CTRL1_RopS0);
		RectBorderDraw(pAreaDst, BTE_CTRL0_Pattern16x16);
	}

	static void RectBorder(Canvas *pDst, const Area *pAreaDst, ulong color)
	{
		SetBteDest(pDst);
		SetForeColor(color);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeSolidFill);
		RectBorderDraw(pAreaDst);
	}

protected:
	static void RectBorderDraw(const Area *pAreaDst, uint pattern = BTE_CTRL0_Pattern8x8)
	{
		pattern |= BTE_CTRL0_Enable;

		// Write along each edge
		// Top: X = 0, Y = 0
		WriteSequentialRegisters(pAreaDst, DT_X0, 3 * sizeof(ushort));
		WriteReg16(BTE_HIG0, BorderThickness);
		WriteReg(BTE_CTRL0, pattern);
		WaitWhileBusy();
		// Bottom: X = 0, Y = Height - BorderThickness
		WriteReg16(DT_Y0, pAreaDst->Ypos + pAreaDst->Height - BorderThickness);
		WriteReg(BTE_CTRL0, pattern);
		WaitWhileBusy();
		// Left: X = 0, Y = BorderThickness
		WriteReg16(DT_Y0, pAreaDst->Ypos + BorderThickness);
		WriteRegXY(BTE_WTH0, BorderThickness, pAreaDst->Height - 2 * BorderThickness);
		WriteReg(BTE_CTRL0, pattern);
		WaitWhileBusy();
		// Right: X = Width - BorderThickness, Y = BorderThickness
		WriteReg16(DT_X0, pAreaDst->Xpos + pAreaDst->Width - BorderThickness);
		WriteReg(BTE_CTRL0, pattern);
		WaitWhileBusy();
	}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static TouchCanvas	*s_pMainImage;
	inline static TouchCanvas	*s_pPip1Image;
	inline static TouchCanvas	*s_pPip2Image;
	inline static ushort		s_pip1x;
	inline static ushort		s_pip1y;
	inline static ushort		s_pip2x;
	inline static ushort		s_pip2y;
	inline static bool			s_fPip1Modal;
};
