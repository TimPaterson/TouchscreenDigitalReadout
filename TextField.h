//****************************************************************************
// TextField.h
//
// Created 11/29/2020 10:01:40 AM by Tim
//
//****************************************************************************

#pragma once

#include "ScreenMgr.h"
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

// Direct access to font by name

#define START_FONT(name)	extern FontInfo FONT_##name;

extern "C"
{
	#include "Fonts/Fonts.h"
};

//****************************************************************************

class TextField : public RA8876
{
public:
	static constexpr long Transparent = -1;

public:
	TextField(Canvas *pCanvas, ushort X, ushort Y):
		TextField(pCanvas, X, Y, 
		({union {void (TextField::*mf)(byte); _fdev_put_t *p;} u = {&TextField::WriteChar}; u.p;})) 
		{}

	TextField(Canvas *pCanvas, ushort X, ushort Y, _fdev_put_t *put):
		m_pCanvas{pCanvas},
		m_backColor{Transparent},
		m_file{{{put}}, this, 0, _FDEV_SETUP_WRITE},
		m_viewPosX{X},
		m_viewPosY{Y},
		m_curPosX{X},
		m_curPosY{Y}
		{}

public:
	void SetFont(FontId id)
	{
		m_pFontInfo = FontList[id];
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

	void SetBackColor(long color)
	{
		m_backColor = color;
	}

	void MakeActive()
	{
		ScreenMgr::SetBteDestination(m_pCanvas);
		WriteReg16(DT_Y0, m_curPosY);
		RA8876::SetForeColor(m_foreColor);
		RA8876::SetBackColor(m_backColor);
		ScreenMgr::SetBteSource0((Image *)m_pFontInfo, Color8bpp);
		WriteReg16(S0_Y0, 0);
		WriteReg16(BTE_HIG0, m_pFontInfo->Height);
		WriteReg(BTE_CTRL1, (7 << BTE_CTRL1_BitStartShift) |
			(m_backColor < 0 ? BTE_CTRL1_OpcodeMemoryCopyExpandMonoTransparent :
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
		m_curPosX += width;
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	void WriteString(const char *psz) NO_INLINE_ATTR
	{
		byte	ch;

		MakeActive();
		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			WriteChar(ch);
		}
	}

	int printf(const char *fmt, ...)
	{
		va_list ap;
		int i;

		MakeActive();
		va_start(ap, fmt);
		i = vfprintf(&m_file, fmt, ap);
		va_end(ap);

		return i;
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
	Canvas		*m_pCanvas;
	FontInfo	*m_pFontInfo;
	ulong		m_foreColor;
	long		m_backColor;	// -1 signifies transparent
	FILE		m_file;
	ushort		m_viewPosX;
	ushort		m_viewPosY;
	ushort		m_curPosX;
	ushort		m_curPosY;
	byte		m_spaceWidth;
};


class TextLine : public TextField
{
public:
	TextLine(Canvas *pCanvas, ushort X, ushort Y): 
		TextField(pCanvas, X, Y,
		({union {void (TextLine::*mf)(byte); _fdev_put_t *p;} u = {&TextLine::WriteChar}; u.p;}))
		{}

public:
	void WriteChar(byte ch)
	{
		if (ch == '\n')
		{
			ResetPosition();
			return;
		}

		TextField::WriteChar(ch);
	}

	// Local version to use our WriteChar()
	void WriteString(const char *psz) NO_INLINE_ATTR
	{
		byte	ch;

		MakeActive();
		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
			return;
			WriteChar(ch);
		}
	}

};
