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
	TextField(Canvas *pCanvas, const Area *pArea):
		TextField(pCanvas, pArea, 
		({union {void (TextField::*mf)(byte); _fdev_put_t *p;} u = {&TextField::WriteChar}; u.p;})) 
		{}

	TextField(Canvas *pCanvas, const Area *pArea, FontId id, ulong foreColor, ulong backColor):
		TextField(pCanvas, pArea, id, foreColor, backColor,
		({union {void (TextField::*mf)(byte); _fdev_put_t *p;} u = {&TextField::WriteChar}; u.p;})) 
		{}

	TextField(Canvas *pCanvas, const Area *pArea, _fdev_put_t *put):
		m_pCanvas{pCanvas},
		m_pArea{pArea},
		m_backColor{Transparent},
		m_file{{{put}}, this, 0, _FDEV_SETUP_WRITE},
		m_curPosX{pArea->Xpos},
		m_curPosY{pArea->Ypos}
		{}

	TextField(Canvas *pCanvas, const Area *pArea, FontId id, ulong foreColor, ulong backColor, _fdev_put_t *put):
		m_pCanvas{pCanvas},
		m_pArea{pArea},
		m_backColor{Transparent},
		m_file{{{put}}, this, 0, _FDEV_SETUP_WRITE},
		m_curPosX{pArea->Xpos},
		m_curPosY{pArea->Ypos}
	{
		SetFont(id);
		m_foreColor = foreColor;
		m_backColor = backColor;
		ResetPosition();
	}

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

	void SetArea(const Area *pArea)
	{
		m_pArea = pArea;
		ResetPosition();
	}

	void MakeActive()
	{
		byte	ctrl;
		ulong	color;

		ScreenMgr::SetBteDest(m_pCanvas);
		WriteReg16(DT_Y0, m_curPosY);
		RA8876::SetForeColor(m_foreColor);
		RA8876::SetBackColor(m_backColor);
		ScreenMgr::SetBteSrc0((Image *)m_pFontInfo, Color8bpp);
		WriteReg16(S0_Y0, 0);
		WriteReg16(BTE_HIG0, m_pFontInfo->Height);
		if (m_backColor < 0)
		{
			color = ~m_foreColor;	// make sure it's different
			ctrl = BTE_CTRL1_OpcodeMemoryCopyExpandMonoTransparent;
		}
		else
		{
			color = m_backColor;
			ctrl = BTE_CTRL1_OpcodeMemoryCopyExpandMono;
		}
		RA8876::SetBackColor(color);
		WriteReg(BTE_CTRL1, (7 << BTE_CTRL1_BitStartShift) | ctrl);
	}

	void ResetPosition()
	{
		m_curPosX = m_pArea->Xpos;
		m_curPosY = m_pArea->Ypos;
	}

	void SetXposition(uint position)
	{
		m_curPosX = position;
	}

	void MoveXposition(int cntPx)
	{
		m_curPosX += cntPx;
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

	void WriteString(const char *psz)
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

	int printf(const char *fmt, ...) __attribute__ ((format (printf, 2, 3)))
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

	void ClearArea()
	{
		ScreenMgr::FillRect(m_pCanvas, m_pArea, m_backColor);
	}

	void FillArea(ulong color)
	{
		ScreenMgr::FillRect(m_pCanvas, m_pArea, color);
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	Canvas		*m_pCanvas;
	FontInfo	*m_pFontInfo;
	const Area	*m_pArea;
	ulong		m_foreColor;
	long		m_backColor;	// -1 signifies transparent
	FILE		m_file;
	ushort		m_curPosX;
	ushort		m_curPosY;
	byte		m_spaceWidth;
};


class TextLine : public TextField
{
public:
	TextLine(Canvas *pCanvas, const Area *pArea): 
		TextLine(pCanvas, pArea,
		({union {void (TextLine::*mf)(byte); _fdev_put_t *p;} u = {&TextLine::WriteChar}; u.p;}))
		{}

	TextLine(Canvas *pCanvas, const Area *pArea, FontId id, ulong foreColor, ulong backColor): 
		TextLine(pCanvas, pArea, id, foreColor, backColor,
		({union {void (TextLine::*mf)(byte); _fdev_put_t *p;} u = {&TextLine::WriteChar}; u.p;}))
		{}

	TextLine(Canvas *pCanvas, const Area *pArea, _fdev_put_t *put):
		TextField(pCanvas, pArea, put) {}

	TextLine(Canvas *pCanvas, const Area *pArea, FontId id, ulong foreColor, ulong backColor, _fdev_put_t *put):
		TextField(pCanvas, pArea, id, foreColor, backColor, put) {}

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
	void WriteString(const char *psz)
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


class NumberLine : public TextLine
{
public:
	NumberLine(Canvas *pCanvas, const Area *pArea): 
		TextLine(pCanvas, pArea,
		({union {void (NumberLine::*mf)(byte); _fdev_put_t *p;} u = {&NumberLine::WriteChar}; u.p;}))
		{}

	NumberLine(Canvas *pCanvas, const Area *pArea, FontId id, ulong foreColor, ulong backColor): 
		TextLine(pCanvas, pArea, id, foreColor, backColor,
		({union {void (NumberLine::*mf)(byte); _fdev_put_t *p;} u = {&NumberLine::WriteChar}; u.p;}))
	{
		SetSpaceWidth(GetCharWidth('0'));
	}

public:
	int PrintNum(const char *fmt, double val)
	{
		if (val < 0)
			MoveXposition(GetCharWidth('0') - GetCharWidth('-'));
		return printf(fmt, val);
	}

	void WriteString(const char *psz)
	{
		byte	ch;

		MakeActive();
		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			if (ch == '-')
				MoveXposition(GetCharWidth('0') - GetCharWidth('-'));
			WriteChar(ch);
		}
	}

	void SetFont(FontId id)
	{
		TextLine::SetFont(id);
		SetSpaceWidth(GetCharWidth('0'));
	}
};
