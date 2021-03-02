//****************************************************************************
// TextField.h
//
// Created 11/29/2020 10:01:40 AM by Tim
//
//****************************************************************************

#pragma once

#include "ScreenMgr.h"
#include "FontInfo.h"


//#define FONT_BPP_8

#ifdef FONT_BPP_8
#define FONT_DEPTH		Color8bpp
#define FONT_BIT_START	7
#else
#define FONT_DEPTH		Color16bpp
#define FONT_BIT_START	15
#endif


EXTERN_C FontInfo *FontList[];

//****************************************************************************

class TextField : public RA8876
{
public:
	TextField(Canvas &canvas, const Area &area):
		TextField(canvas, area, 
		({union {void (TextField::*mf)(byte); _fdev_put_t *p;} u = {&TextField::WriteCharActive}; u.p;})) 
		{}

	TextField(Canvas &canvas, const Area &area, uint id, ulong foreColor, ulong backColor):
		TextField(canvas, area, id, foreColor, backColor,
		({union {void (TextField::*mf)(byte); _fdev_put_t *p;} u = {&TextField::WriteCharActive}; u.p;})) 
		{}

protected:
	TextField(Canvas &canvas, const Area &area, _fdev_put_t *put):
		m_pCanvas{&canvas},
		m_pArea{&area},
		m_file{{{put}}, this, 0, _FDEV_SETUP_WRITE},
		m_curPosX{area.Xpos},
		m_curPosY{area.Ypos}
		{}

	TextField(Canvas &canvas, const Area &area, uint id, ulong foreColor, ulong backColor, _fdev_put_t *put):
		m_pCanvas{&canvas},
		m_pFontInfo{FontList[id]},
		m_pArea{&area},
		m_foreColor{foreColor},
		m_backColor{backColor},
		m_file{{{put}}, this, 0, _FDEV_SETUP_WRITE},
		m_curPosX{area.Xpos},
		m_curPosY{area.Ypos}
		{}

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	void SetFont(uint id)
	{
		m_pFontInfo = FontList[id];
		SetSpaceWidth();
	}

	void SetBackgroundTransparent(bool fTransparent)
	{
		m_fTransparent = fTransparent;
	}

	void SetSpaceWidth(uint width = 0)
	{
		m_spaceWidth = width;
	}

	uint GetSpaceWidth()
	{
		return m_spaceWidth;
	}

	void SetTextColor(ulong color)
	{
		m_foreColor = color;
	}

	void SetTextBackcolor(ulong color)
	{
		m_backColor = color;
	}

	void SetArea(const Area &area)
	{
		m_pArea = &area;
		ResetPosition();
	}

	void ResetPosition()
	{
		m_curPosX = m_pArea->Xpos;
		m_curPosY = m_pArea->Ypos;
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
		MakeActive();
		WriteCharActive(ch);
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
			WriteCharActive(ch);
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
		FillArea(m_backColor);
	}

	void ClearToEnd()
	{
		ClearToEnd(m_curPosX);
	}

	void ClearToEnd(uint posX)
	{
		ScreenMgr::SetBteDest(m_pCanvas);
		SetForeColor(m_backColor);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeSolidFill);
		WriteRegXY(DT_X0, posX, m_pArea->Ypos);
		WriteRegXY(BTE_WTH0, m_pArea->Xpos + m_pArea->Width - posX, m_pArea->Height);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	void FillArea(ulong color)
	{
		ScreenMgr::FillRect(m_pCanvas, m_pArea, color);
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	void MakeActive()
	{
		byte	ctrl;
		ulong	color;

		ScreenMgr::SetBteDest(m_pCanvas);
		WriteReg16(DT_Y0, m_curPosY);
		ScreenMgr::SetBteSrc0((Image *)m_pFontInfo, FONT_DEPTH);
		WriteReg16(S0_Y0, 0);
		WriteReg16(BTE_HIG0, m_pFontInfo->Height);
		SetForeColor(m_foreColor);
		if (m_fTransparent)
		{
			color = ~m_foreColor;	// make sure it's different
			ctrl = BTE_CTRL1_OpcodeMemoryCopyExpandMonoTransparent;
		}
		else
		{
			color = m_backColor;
			ctrl = BTE_CTRL1_OpcodeMemoryCopyExpandMono;
		}
		SetBackColor(color);
		WriteReg(BTE_CTRL1, (FONT_BIT_START << BTE_CTRL1_BitStartShift) | ctrl);
	}

	void WriteCharActive(byte ch)
	{
		byte	width;
		int		remain;

		ch -= m_pFontInfo->FirstChar;
		if (ch > m_pFontInfo->LastChar)
			return;

		WriteReg16(S0_X0, ch * m_pFontInfo->CharStride);
		if (ch + m_pFontInfo->FirstChar == ' ' && m_spaceWidth != 0)
			width = m_spaceWidth;
		else
			width = m_pFontInfo->arWidths[ch];
		remain = m_pArea->Xpos + m_pArea->Width - m_curPosX;
		if (remain <= 0)
			return;
		if (remain < width)
			width = remain;
		WriteReg16(BTE_WTH0, width);
		WriteReg16(DT_X0, m_curPosX);
		m_curPosX += width;
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	Canvas		*m_pCanvas;
	FontInfo	*m_pFontInfo;
	const Area	*m_pArea;
	ulong		m_foreColor;
	ulong		m_backColor;
	FILE		m_file;
	ushort		m_curPosX;
	ushort		m_curPosY;
	byte		m_spaceWidth;
	bool		m_fTransparent;
};


class TextLine : public TextField
{
public:
	TextLine(Canvas &canvas, const Area &area): 
		TextLine(canvas, area,
		({union {void (TextLine::*mf)(byte); _fdev_put_t *p;} u = {&TextLine::WriteCharActive}; u.p;}))
		{}

	TextLine(Canvas &canvas, const Area &area, uint id, ulong foreColor, ulong backColor): 
		TextLine(canvas, area, id, foreColor, backColor,
		({union {void (TextLine::*mf)(byte); _fdev_put_t *p;} u = {&TextLine::WriteCharActive}; u.p;}))
		{}

protected:
	TextLine(Canvas &canvas, const Area &area, _fdev_put_t *put):
		TextField(canvas, area, put) {}

	TextLine(Canvas &canvas, const Area &area, uint id, ulong foreColor, ulong backColor, _fdev_put_t *put):
		TextField(canvas, area, id, foreColor, backColor, put) {}

public:
	void WriteCharActive(byte ch)
	{
		if (ch == '\n')
		{
			ResetPosition();
			return;
		}

		TextField::WriteCharActive(ch);
	}

	// Local version to use our WriteCharActive()
	void WriteString(const char *psz)
	{
		byte	ch;

		MakeActive();
		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			WriteCharActive(ch);
		}
	}
};


class NumberLine : public TextLine
{
public:
	NumberLine(Canvas &canvas, const Area &area): 
		TextLine(canvas, area)
	{
		SetSpaceWidth(GetCharWidth('0'));
	}

	NumberLine(Canvas &canvas, const Area &area, uint id, ulong foreColor, ulong backColor): 
		TextLine(canvas, area, id, foreColor, backColor)
	{
		SetSpaceWidth(GetCharWidth('0'));
	}

public:
	void SetFont(uint id)
	{
		TextLine::SetFont(id);
		SetSpaceWidth(GetCharWidth('0'));
	}

	int PrintNum(const char *fmt, double val)
	{
		if (val < 0)
			MoveXposition(GetCharWidth('0') - GetCharWidth('-'));
		return printf(fmt, val);
	}

};


class NumberLineBlankZ : public NumberLine
{
public:
	NumberLineBlankZ(Canvas &canvas, const Area &area): 
		NumberLine(canvas, area)
	{
		SetBackgroundTransparent(true);
	}

	NumberLineBlankZ(Canvas &canvas, const Area &area, uint id, ulong foreColor, ulong backColor): 
		NumberLine(canvas, area, id, foreColor, backColor)
	{
		SetBackgroundTransparent(true);
	}

public:
	int PrintDbl(const char *fmt, double val)
	{
		ClearArea();
		if (val == 0)
			return 0;

		if (val < 0)
			MoveXposition(GetCharWidth('0') - GetCharWidth('-'));
		return printf(fmt, val);
	}

	int PrintDbl(const char *fmt, double val, const Area &area)
	{
		SetArea(area);
		return PrintDbl(fmt, val);
	}

	int PrintInt(const char *fmt, int val)
	{
		ClearArea();
		if (val == 0)
			return 0;

		if (val < 0)
			MoveXposition(GetCharWidth('0') - GetCharWidth('-'));
		return printf(fmt, val);
	}

	int PrintInt(const char *fmt, int val, const Area &area)
	{
		SetArea(area);
		return PrintInt(fmt, val);
	}

	int PrintUint(const char *fmt, uint val)
	{
		ClearArea();
		if (val == 0)
			return 0;

		return printf(fmt, val);
	}

	int PrintUint(const char *fmt, uint val, const Area &area)
	{
		SetArea(area);
		return PrintInt(fmt, val);
	}
};

