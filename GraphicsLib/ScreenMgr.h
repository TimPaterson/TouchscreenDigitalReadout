//****************************************************************************
// ScreenMgr.h
//
// Created 12/15/2020 3:58:08 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876.h"
#include "TouchCanvas.h"


struct PipInfo
{
	TouchCanvas	*pImage;
	ushort		x;
	ushort		y;

	bool IsEnabled()	{ return pImage != NULL; }
};


class ScreenMgr : public RA8876
{
	static const int BorderThickness = 4;

public:
	static ulong AllocVideoRam(int size)
	{
		ulong res = s_NextFreeRam;
		s_NextFreeRam += size;
		return res;
	}

	static PipInfo *GetPip1()	{ return &s_pip1; }
	static PipInfo *GetPip2()	{ return &s_pip2; }

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

		s_pip1.pImage = pCanvas;
		s_pip1.x = x;
		s_pip1.y = y;
		s_fPip1Modal = fModal;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip1 | MPWCTR_Pip1Enable));
		WriteRegXY(PWDULX0, x, y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip1Color_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << PIPCDEP_Pip1Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	static void SetPip1Modal(bool fModal)
	{
		s_fPip1Modal = fModal;
	}

	static void DisablePip1()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip1_Mask);
		s_pip1.pImage = NULL;
	}

	static void EnablePip2(TouchCanvas *pCanvas, uint x, uint y, bool fModal = false)
	{
		byte	val;

		s_pip2.pImage = pCanvas;
		s_pip2.x = x;
		s_pip2.y = y;
		s_fPip2Modal = fModal;
		val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
		WriteData(val | (MPWCTR_ConfigurePip2 | MPWCTR_Pip2Enable));
		WriteRegXY(PWDULX0, x, y);
		val = ReadReg(PIPCDEP) & ~PIPCDEP_Pip2Color_Mask;
		WriteData(val | (pCanvas->GetColorDepth() << PIPCDEP_Pip2Color_Shft));
		WriteSequentialRegisters(pCanvas, PISA0, CanvasViewRegCount);
	}

	static void SetPip2Modal(bool fModal)
	{
		s_fPip2Modal = fModal;
	}

	static void DisablePip2()
	{
		WriteData(ReadReg(MPWCTR) & ~MPWCTR_Pip2_Mask);
		s_pip2.pImage = NULL;
	}

	static void SetViewPos(TouchCanvas *pCanvas, uint x, uint y)
	{
		byte	val;

		pCanvas->SetViewPos(x, y);
		if (s_pMainImage == pCanvas)
		{
			WriteRegXY(MWULX0, x, y);
			return;
		}

		if (pCanvas == s_pip1.pImage || pCanvas == s_pip2.pImage)
		{
			val = ReadReg(MPWCTR) & ~MPWCTR_ConfigurePip_Mask;
			val |= pCanvas == s_pip1.pImage ? MPWCTR_ConfigurePip1 : MPWCTR_ConfigurePip2;
			WriteData(val);
			WriteRegXY(PWIULX0, x, y);
		}
	}

	static PipInfo *GetPip(TouchCanvas *pCanvas)
	{
		if (s_pip1.pImage == pCanvas)
			return &s_pip1;
		if (s_pip2.pImage == pCanvas)
			return &s_pip2;
		return NULL; 
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

	static void SetBteSrc0(const TouchCanvas *pCanvas)
	{
		SetBteSrc0((const Image *)pCanvas, pCanvas->GetColorDepth());
	}

	static void SetBteSrc0(const ColorImage *pImage)
	{
		SetBteSrc0((const Image *)pImage, pImage->GetColorDepth());
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
		if (s_pip1.IsEnabled())
		{
			pSpot = s_pip1.pImage->TestHit(x - s_pip1.x, y - s_pip1.y);
			if (pSpot != NOT_ON_CANVAS)
				return pSpot;
			if (s_fPip1Modal)
				return NULL;
		}

		// PIP2 is next if enabled
		if (s_pip2.IsEnabled())
		{
			pSpot = s_pip2.pImage->TestHit(x - s_pip2.x, y - s_pip2.y);
			if (pSpot != NOT_ON_CANVAS)
				return pSpot;
			if (s_fPip2Modal)
				return NULL;
		}

		pSpot = s_pMainImage->TestHit(x, y);
		if (pSpot == NOT_ON_CANVAS)
		{
			DEBUG_PRINT("Unexpected off-screen touch coordinates\n");
			return NULL;
		}
		return pSpot;
	}

	static void SelectImage(Canvas *pDst, const Area *pAreaDst, const ColorImage *pSrc, uint index)
	{
		CopyRect(pDst, pAreaDst, pSrc, pAreaDst->Width * index, 0);
	}

	static void CopyRect(Canvas *pDst, const Area *pAreaDst, const ColorImage *pSrc, uint srcX = 0, uint srcY = 0)
	{
		SetBteSrc0(pSrc);
		WriteRegXY(S0_X0, srcX, srcY);
		CopyRectSrcSet(pDst, pAreaDst);
	}

	static void CopyRect(Canvas *pDst, const Area *pAreaDst, Canvas *pSrc)
	{
		byte	val;

		WriteSequentialRegisters(pSrc, S0_STR0, CanvasRegCount);
		val = ReadReg(BTE_COLR) & ~BTE_COLR_Src0Color_Mask;
		WriteData(val | (pSrc->GetColorDepth() << BTE_COLR_Src0Color_Shft));
		CopyRectSrcSet(pDst, pAreaDst);
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
		SetBteSrc0(pSrc);
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
	static void CopyRectSrcSet(Canvas *pDst, const Area *pAreaDst)
	{
		SetBteDest(pDst);
		WriteSequentialRegisters(pAreaDst, DT_X0, sizeof *pAreaDst);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeMemoryCopyWithRop | BTE_CTRL1_RopS0);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

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
	inline static ulong			s_NextFreeRam{RamFreeStart};
	inline static TouchCanvas	*s_pMainImage;
	inline static PipInfo		s_pip1;
	inline static PipInfo		s_pip2;
	inline static bool			s_fPip1Modal;
	inline static bool			s_fPip2Modal;
};
