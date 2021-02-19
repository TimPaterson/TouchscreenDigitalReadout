//****************************************************************************
// ListScroll.h
//
// Created 1/21/2021 3:54:39 PM by Tim
//
//****************************************************************************

#pragma once

#include "TouchCanvas.h"
#include "TextField.h"


class ListScroll : public TouchCanvas, public RA8876
{
	// Make these variables?
	static constexpr ulong ThumbColor = 0xFFFFFF;
	static constexpr ulong ScrollBarColor = 0x303030;
	static constexpr ulong ThumbGapColor = 0x00FF00;

	static constexpr int JitterIgnore = 8;
	static constexpr int ThumbHeight = 60;
	static constexpr int ExtraLines = 2;	// at both top and bottom

	// Imitate HotpostList
	struct ScrollHotspots
	{
		ushort	count;
		Hotspot	list[2];
	};

public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wuninitialized"
	ListScroll(ushort width, ushort height, ushort lineHeight, ColorDepths depth, byte HotspotGroup) :
		TouchCanvas(ScreenMgr::AllocVideoRam(0), width, height, width, depth, (HotspotList *)&m_hotSpots),
		m_lineHeight{lineHeight}, m_hotSpots{ 2, {
			// Array of Hotspots: the display area, and the scroll thumb
			{0, 0, (ushort)(m_viewWidth - ScrollBarWidth - 1), (ushort)(m_imageHeight - 1), 
				{ScrollDisplay, HotspotGroup}},
			{(ushort)(m_viewWidth - ScrollThumbWidth), 0, (ushort)(m_viewWidth - 1), (ushort)(m_imageHeight - 1), 
				{ScrollThumb, HotspotGroup}}
		}}
	{
		m_extraLineCnt = ExtraLines;
		m_lineViewCnt = height / lineHeight;
		m_lineCnt = m_lineViewCnt + 2 * ExtraLines + 2;
		height = m_lineCnt * lineHeight;
		m_imageHeight = height;
		// Allocate image RAM
		ScreenMgr::AllocVideoRam((int)width * height * PixelSizeFromDepth(depth));
		Invalidate();
	}
#pragma GCC diagnostic pop

public:
	void Invalidate()	{ m_lineTop = m_posCur = 0x40000000; }

public:
	void Init()
	{
		Area	area;

		// Initialize gap
		area.Height = m_imageHeight;
		area.Width = ScrollGapLeft;
		area.Xpos = m_viewWidth - ScrollBarWidth;
		area.Ypos = 0;
		ScreenMgr::FillRect(this, &area, ThumbGapColor);

		// Initialize scroll bar
		area.Width = ScrollThumbWidth;
		area.Xpos = m_viewWidth - ScrollThumbWidth;
		ScreenMgr::FillRect(this, &area, ScrollBarColor);
	}

	void SetTotalLines(int lines)	
	{
		m_posMax = (lines - m_lineViewCnt + 1) * m_lineHeight;
		if (m_posMax < 0)
			m_posMax = 0;
	}

	bool StartCapture(int x, int y, ScrollAreas spot)
	{
		m_capturePos = y;
		if (spot == ScrollThumb)
		{
			// Are we outside the scroll thumb?
			y += m_viewPosY;
			if (y < m_thumbPos)
			{
				// Scroll up by a page
				SetScrollPosition(m_posCur - (m_viewHeight - m_lineHeight));
				return false;
			}

			if (y >= m_thumbPos + ThumbHeight)
			{
				// Scroll down by a page
				SetScrollPosition(m_posCur + (m_viewHeight - m_lineHeight));
				return false;
			}

			m_fTrackThumb = true;
			return true;
		}
		m_fTrackThumb = false;
		m_fDidMove  = false;
		return true;
	}

	void EndCapture()
	{
		if (m_fDidMove)
			return;

		// No movement on last touch, select the line instead
		LineSelected((m_posCur + m_capturePos) / m_lineHeight);
	}

	void ScrollToLine(int line)
	{
		int		posLine;

		posLine = line * m_lineHeight;

		// See if line is in buffer
		if (line >= m_lineTop && line < m_lineTop + m_lineCnt)
		{
			// In buffer, just make sure it's visible
			if (posLine >= m_posCur)
			{
				posLine -= m_viewHeight - m_lineHeight;
				if (posLine <= m_posCur)
					return; 
			}
		}
		SetScrollPosition(posLine);
	}

	void InvalidateLine(int line)
	{
		InvalidateLines(line, line);
	}

	void InvalidateLines(int lineStart, int lineEnd)
	{
		int		lineLast;

		lineLast = m_lineTop + m_lineCnt - 1;
		if (lineStart > lineLast || lineEnd < m_lineTop)
			return;

		if (lineStart < m_lineTop)
			lineStart = m_lineTop;
		if (lineEnd > lineLast)
			lineEnd = lineLast;

		FillLines(lineStart, lineStart - m_lineTop, lineEnd - lineStart + 1);
	}

	void NewPosition(int x, int y)
	{
		int		delta;
		int		thumbPos;
		int		posNew;
		int		scrollHeight;

		delta = y - m_capturePos;
		if (abs(delta) <= JitterIgnore)
			return;

		m_capturePos = y;
		m_fDidMove = true;

		if (m_fTrackThumb)
		{
			thumbPos = m_thumbPos - m_viewPosY + delta;
			scrollHeight = m_viewHeight - ThumbHeight;
			if (thumbPos < 0)
				thumbPos = 0;
			else if (thumbPos > scrollHeight)
				thumbPos = scrollHeight;

			posNew = DivUintRnd(m_posMax * thumbPos, scrollHeight);
		}
		else
			posNew = m_posCur - delta;

		SetScrollPosition(posNew);
	}

	bool SetScrollPosition(int posNew)
	{
		int		lineTopNew;
		int		lineDelta;

		if (posNew < 0)
			posNew = 0;
		else if (posNew > m_posMax)
			posNew = m_posMax;

		MakeActive();

		lineTopNew  = posNew / m_lineHeight;
		if (lineTopNew < m_lineTop)
		{
			// Scrolling higher than top line
			// Move existing lines down to make room.
			lineTopNew -= m_extraLineCnt;	// add room at top
			if (lineTopNew < 0)
				lineTopNew = 0;
			lineDelta = m_lineTop - lineTopNew;
			if (lineDelta < m_lineCnt)
				MoveLinesDown(lineDelta);
			else
				lineDelta = m_lineCnt;

			// Now fill in new lines
			FillLines(lineTopNew, 0, lineDelta);
			m_lineTop = lineTopNew;
		}
		else if (lineTopNew - m_lineTop > 2 * m_extraLineCnt)
		{
			// Scrolling lower than bottom line
			// Move existing lines up to make room
			lineTopNew -= m_extraLineCnt;	// add room at top
			lineDelta = lineTopNew - m_lineTop;
			if (lineDelta < m_lineCnt)
				MoveLinesUp(lineDelta);
			else
				lineDelta = m_lineCnt;

			// Now fill in new lines
			FillLines(lineTopNew + m_lineCnt - lineDelta, m_lineCnt - lineDelta, lineDelta);
			m_lineTop = lineTopNew;
		}

		// We have all the lines we need to display
		// Erase thumb
		WriteRegXY(DT_X0, m_viewWidth - ScrollThumbWidth, m_thumbPos);
		WriteRegXY(BTE_WTH0, ScrollThumbWidth, ThumbHeight);
		SetForeColor(ScrollBarColor);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeSolidFill);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();

		// Set window start position
		ScreenMgr::SetViewPos(this, 0, posNew - m_lineTop * m_lineHeight);
		m_posCur = posNew;

		// Place thumb
		WaitVsync();
		m_thumbPos = DivUintRnd((m_viewHeight - ThumbHeight) * posNew, m_posMax) + m_viewPosY;
		WriteReg16(DT_Y0, m_thumbPos);
		SetForeColor(ThumbColor);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeSolidFill);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();

		return true;
	}

protected:
	void MoveLinesDown(int lineDelta)
	{
		int		lineSrc, lineDst;

		lineDst = m_lineCnt - 1;
		lineSrc = lineDst - lineDelta;
		for (; lineSrc >= 0; lineSrc--, lineDst--)
			CopyLine(lineSrc, lineDst);
	}

	void MoveLinesUp(int lineDelta)
	{
		int		lineSrc, lineDst;

		lineDst = 0;
		lineSrc = lineDst + lineDelta;
		for (; lineSrc < m_lineCnt; lineSrc++, lineDst++)
			CopyLine(lineSrc, lineDst);
	}

	void CopyLine(int lineSrc, int lineDst)
	{
		WriteReg16(S0_Y0, lineSrc * m_lineHeight);
		WriteReg16(DT_Y0, lineDst * m_lineHeight);
		WriteReg(BTE_CTRL0, BTE_CTRL0_Enable);
		WaitWhileBusy();
	}

	void FillLines(int lineStart, int lineDst, int lineCnt)
	{
		for (; lineCnt > 0; lineCnt--)
		{
			m_lineArea.Ypos = lineDst++ * m_lineHeight;
			FillLine(lineStart++, &m_lineArea);
		}
	}

	void MakeActive()
	{
		ScreenMgr::SetBteSrc0((const Image *)(TouchCanvas *)this, m_colorDepth);
		WriteReg16(S0_X0, 0);
		ScreenMgr::SetBteDest(this);
		WriteReg16(DT_X0, 0);
		WriteRegXY(BTE_WTH0, m_lineArea.Width, m_lineHeight);
		WriteReg(BTE_CTRL1, BTE_CTRL1_OpcodeMemoryCopyWithRop | BTE_CTRL1_RopS0);
	}

protected:
	virtual void FillLine(int lineNum, Area *pArea) = 0;
	virtual void LineSelected(int lineNum) = 0;

protected:
	int		m_posCur;
	int		m_posMax;
	int		m_lineTop;
	ushort	m_lineHeight;
	short	m_lineCnt;		// Total lines on canvas
	short	m_extraLineCnt;
	short	m_imageHeight;
	short	m_lineViewCnt;
	short	m_thumbPos;
	short	m_capturePos;
	Area	m_lineArea{0, 0, (ushort)(m_viewWidth - ScrollBarWidth), (ushort)m_lineHeight}; // Area for a single line
	ScrollHotspots	m_hotSpots;
	bool	m_fTrackThumb;
	bool	m_fDidMove;
};
