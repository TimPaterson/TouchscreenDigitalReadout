//****************************************************************************
// TextDisplay.h
//
// Created 11/29/2020 10:01:40 AM by Tim
//
//****************************************************************************

#pragma once

#include "Canvas.h"
#include "FontInfo.h"


EXTERN_C FontInfo *FontList[];

void DumpCanvas(uint addr);

//****************************************************************************
// Enumeration of font names in FontList

#define START_FONT(name)	FID_##name,

enum FontId
{
	#include "Fonts/Fonts.h"
};

//****************************************************************************

class TextDisplay : public Canvas
{
public:
	void SetFont(FontId id, bool isTransparent = false)
	{
		m_pFontInfo = FontList[id];
		m_isTransparent = isTransparent;
		SetSpaceWidth();
		MakeActive();
	}

	void SetSpaceWidth(uint width = 0)
	{
		m_spaceWidth = width;
	}

	void SetForeColor(ulong color)
	{
		m_foreColor = color;
	}

	void SetBackColor(ulong color)
	{
		m_backColor = color;
	}

	void MakeActive()
	{
		SetCanvasView(DT_STR0);
		SetForeColor(m_foreColor);
		SetBackColor(m_backColor);
		WriteReg(BTE_COLR, m_colorDepth);
		WriteReg32(S0_STR0, m_pFontInfo->FontStart);
		WriteReg16(S0_WTH0, m_pFontInfo->CharsetWidth);
		WriteReg16(S0_Y0, 0);
		WriteReg16(BTE_HIG0, m_pFontInfo->Height);
		WriteReg(BTE_CTRL1, (7 << BTE_CTRL1_BitStartShift) |
			(m_isTransparent ? BTE_CTRL1_OpcodeMemoryCopyExpandMonoTransparent :
			BTE_CTRL1_OpcodeMemoryCopyExpandMono));
	}

	void ResetPosition()
	{
		m_curPosX = m_viewPosX;
		m_curPosY = m_viewPosY;
	}

	void SetPosition(uint position)
	{
		m_curPosX = position;
	}

	void NewLine()
	{
		m_curPosY += m_pFontInfo->Height;
	}

	void WriteChar(byte ch)
	{
		byte	width;

		ch -= m_pFontInfo->FirstChar;
		if (ch > m_pFontInfo->LastChar)
			return;

		WriteReg16(S0_X0, ch * m_pFontInfo->CharStride);
		if (ch + m_pFontInfo->FirstChar == ' ' && m_spaceWidth != 0)
			width = m_spaceWidth;
		else
			width = m_pFontInfo->arWidths[ch];
		WriteReg16(BTE_WTH0, width);
		WriteReg16(DT_X0, m_curPosX);
		m_curPosX += m_pFontInfo->arWidths[ch];
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	void WriteString(const char *psz) NO_INLINE_ATTR
	{
		byte	ch;

		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			WriteChar(ch);
		}
	}

	int GetCharWidth(byte ch)
	{
		ch -= m_pFontInfo->FirstChar;
		if (ch > m_pFontInfo->LastChar)
			return 0;

		if (ch + m_pFontInfo->FirstChar == ' ' && m_spaceWidth != 0)
			return m_spaceWidth;
		return m_pFontInfo->arWidths[ch];
	}

	int GetStringWidth(const char *psz)
	{
		byte	ch;
		int		len;

		for (len = 0;;)
		{
			ch = *psz++;
			if (ch == 0)
				break;
			len += GetCharWidth(ch);
		}
		return len;
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	FontInfo *m_pFontInfo;
	ulong	m_foreColor;
	ulong	m_backColor;
	ushort	m_curPosX;
	ushort	m_curPosY;
	bool	m_isTransparent;
	byte	m_spaceWidth;
};
