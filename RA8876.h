//****************************************************************************
// Class RA8876
// RA8876.h
//
// Created 10/11/2020 4:57:27 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876const.h"


namespace RA8876const
{
	//*********************************************************************
	// Types

	struct RegValue
	{
		byte	addr;
		byte	val;
	};
};

//*********************************************************************

using namespace RA8876const;

class RA8876
{
public:

	//*********************************************************************

public:
	static void Init();
	static uint GetStatus();
	static void WriteAddr(uint addr);
	static void WriteData(uint val);
	static uint ReadData();
	static void TestPattern();
	static void DisplayOn();
	static void DisplayOff();
	static void ExternalFont(uint uCharHeight, uint uFont);

	static void WriteReg(uint addr, uint val)
	{
		WriteAddr(addr);
		WriteData(val);
	}

	static uint ReadReg(uint addr)
	{
		WriteAddr(addr);
		return ReadData();
	}

	//*********************************************************************
	// Generic register combination handlers

	static void WriteRegList(const RegValue *pList, int iLen)
	{
		do 
		{
			WriteReg(pList->addr, pList->val);
			pList++;
		} while (--iLen > 0);
	}

	static void WriteReg16(uint addr, uint val)
	{
		WriteReg(addr, val);
		WriteReg(addr + 1, val >> 8);
	}

	static void WriteReg32(uint addr, ulong val)
	{
		WriteReg16(addr, val);
		WriteReg16(addr + 2, val >> 16);
	}

	static void WriteRegXY(uint addr, uint X, uint Y)
	{
		WriteReg16(addr, X);
		WriteReg16(addr + 2, Y);
	}

	static void WriteRegRgb(uint addr, ulong val)
	{
		// standard format is RRGGBB, i.e., blue is
		// LSB. In register addresses, red is first.
		WriteReg(addr, val >> 16);		// red
		WriteReg(addr + 1, val >> 8);	// green
		WriteReg(addr + 2, val);		// blue
	}

	//*********************************************************************
	// Function-specific handlers

	static void SetMainImage(ulong addr, uint width)
	{
		WriteReg32(MISA0, addr);
		WriteReg16(MIW0, width);
		SetMainWindowPos(0, 0);
		SetCanvas(addr, width);
	}

	static void SetMainWindowPos(uint X, uint Y)
	{
		WriteReg16(MWULX0, X);
		WriteReg16(MWULY0, Y);
	}

	static void SetCanvas(ulong addr, uint width)
	{
		WriteReg32(CVSSA0, addr);
		WriteReg16(CVS_IMWTH0, width);
		SetActiveWindowPos(0, 0);
	}

	static void SetActiveWindowPos(uint X, uint Y)
	{
		WriteReg16(AWUL_X0, X);
		WriteReg16(AWUL_Y0, Y);
	}

	static void SetActiveWindowSize(uint width, uint height)
	{
		WriteReg16(AW_WTH0, width);
		WriteReg16(AW_HT0, height);
	}

	//*********************************************************************
	// Graphics Engine

	static void SetTwoPoints(uint X0, uint Y0, uint X1, uint Y1)
	{
		WriteReg16(DLHSR0, X0);
		WriteReg16(DLVSR0, Y0);
		WriteReg16(DLHER0, X1);
		WriteReg16(DLVER0, Y1);
	}

	static void FillRect(uint X0, uint Y0, uint X1, uint Y1)
	{
		SetTwoPoints(X0, Y0, X1, Y1);
		WriteReg(DCR1, DCR1_DrawRect | DCR1_FillOn | DCR1_DrawActive);
		while (GetStatus() & STATUS_CoreBusy);
	}

	//*********************************************************************
	// Text Engine

	static void TextMode()
	{
		WriteData(ReadReg(ICR) | ICR_TextMode);
	}

	static void GraphicsMode()
	{
		WriteData(ReadReg(ICR) & ~ICR_TextGraphicsMode_Mask);
	}

	static void WriteString(const char *psz)
	{
		uint	reg;
		char	ch;

		reg = ReadReg(ICR);
		WriteData(reg | ICR_TextMode);
		WriteAddr(MRWDP);
		while ((ch = *psz++) != 0)
		{
			WriteData(ch);
			while (GetStatus() & STATUS_WriteFifoFull);
		}
		while (GetStatus() & STATUS_CoreBusy);
		WriteReg(ICR, reg);
	}

	static void InternalFont(uint uCharHeight, uint uFont)
	{
		WriteReg(CCR0, uCharHeight | CCR0_CharSourceInternal | uFont);
	}
};
