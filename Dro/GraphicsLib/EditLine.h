//****************************************************************************
// EditLine.h
//
// Created 2/7/2021 3:09:36 PM by Tim
//
//****************************************************************************

#pragma once

#include "TextField.h"


class EditLine : public TextField
{
public:
	static constexpr int EndLinePx = 10000;

protected:
	static constexpr ulong CursorColor = 0xFFFF00;

public:
	enum EditStatus
	{
		EditInProgress,
		EditBufFull,
		EditDone,
		EditCancel,
	};

public:
	EditLine(Canvas &canvas, const Area &area, char *pBuf, ushort chMax, FontInfo &font, ulong foreColor, ulong backColor):
		TextField(canvas, area, font, foreColor, backColor), m_pBufText{pBuf}, 
		m_maxCh{(ushort)(chMax - 1)},		
		m_cursorArea{0, area.Ypos, 1, area.Height}
		{}

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	int CharCount()		{ return m_cntCh; }

public:
	void StartEditPx(int pxPosCur = EndLinePx)
	{
		uint	chPosCur;

		m_scrollPosPx = 0;
		m_scrollPosCh = 0;
		m_cntCh = strlen(m_pBufText);
		chPosCur = CalcPosition(pxPosCur);
		m_cntPx = GetStringWidth(m_pBufText);
		m_curPosX = m_pArea->Xpos;
		SetPositionCh(chPosCur);
	}

	void EndEdit()
	{
		SetPositionCh(0);
		RemoveCursor();
	}

	void SetPositionPx(uint pxPosCur)
	{
		SetPositionCh(CalcPosition(pxPosCur));
	}

	void UpdateBuffer()
	{
		m_cntCh = strlen(m_pBufText);
		ResetPosition();
		WriteString(m_pBufText);
		ClearToEnd();
	}

	void SetPositionCh(uint chPosCur)
	{
		uint	pxPos;
		uint	chPos;

		if (chPosCur > m_cntCh)
			chPosCur = m_cntCh;

		// Get pixel position
		m_chPosCur = chPosCur;
		for (pxPos = 0, chPos = 0; chPos < chPosCur; chPos++)
			pxPos += GetCharWidth(m_pBufText[chPos]);

		MoveCursor(m_pArea->Xpos + pxPos);
	}

	void DeleteText()
	{
		m_pBufText[0] = '\0';
		ClearArea();
	}

	EditStatus ProcessKey(uint key)
	{
		uint	chWidth;
		uint	pos;

		if (key >= 0x20 && key <= 0x7E)
		{
			// handle printable character
			if (m_cntCh >= m_maxCh)
				return EditBufFull;

			chWidth = GetCharWidth(key);
			m_cntPx += chWidth;
			pos = m_curPosX + chWidth;
			if (m_chPosCur < m_cntCh)
			{
				// Inserting, move existing characters and null terminator
				memmove(&m_pBufText[m_chPosCur + 1], &m_pBufText[m_chPosCur], m_cntCh - m_chPosCur + 1);
				m_pBufText[m_chPosCur] = key;
				WriteString(&m_pBufText[m_chPosCur]);
			}
			else
			{
				m_pBufText[m_chPosCur] = key;
				m_pBufText[m_chPosCur + 1] = '\0';
				WriteChar(key);
			}
			m_chPosCur++;
			m_cntCh++;
			SetCursorRelative(pos);
			return EditInProgress;
		}
		else
		{
			// Control key
			switch (key)
			{
			case Key_enter:
				EndEdit();
				return EditDone;

			case Key_backspace:
				if (m_chPosCur == 0)
					break;

				chWidth = GetCharWidth(m_pBufText[m_chPosCur - 1]);
				pos = m_curPosX - chWidth;

				if (m_chPosCur < m_cntCh)
				{
					// Not at end, move characters up
					memmove(&m_pBufText[m_chPosCur - 1], &m_pBufText[m_chPosCur], m_cntCh - m_chPosCur + 1);
					m_curPosX = pos;
					WriteString(&m_pBufText[m_chPosCur - 1]);
				}
				else
				{
					m_curPosX = pos;
					m_pBufText[m_chPosCur - 1] = '\0';
				}

				ClearToEnd();
				m_cntCh--;
				m_chPosCur--;
				SetCursorRelative(pos);
				break;

			case Key_left:
				if (m_chPosCur == 0)
					break;

				chWidth = GetCharWidth(m_pBufText[--m_chPosCur]);
				MoveCursorBy(-chWidth);
				break;

			case Key_right:
				if (m_chPosCur >= m_cntCh)
					break;

				chWidth = GetCharWidth(m_pBufText[m_chPosCur++]);
				MoveCursorBy(chWidth);
				break;

			case Key_home:
				SetPositionCh(0);
				break;

			case Key_end:
				SetPositionCh(m_cntCh);
				break;
			}
		}

		return EditInProgress;
	}

protected:
	//*********************************************************************
	// Helpers
	//*********************************************************************

	// Compute character position from relative pixel position
	uint CalcPosition(int pxPos)
	{
		int		pxPosCur;
		uint	chWidth;
		uint	chPos;

		for (pxPosCur = 0, chPos = m_scrollPosCh; chPos < m_cntCh; chPos++)
		{
			chWidth = GetCharWidth(m_pBufText[chPos]);
			if (pxPos < (int)(pxPosCur + chWidth / 2))
				break;
			pxPosCur += chWidth;
		}
		return chPos;
	}

	void DrawCursor(bool fRemove = false)
	{
		m_cursorArea.Xpos = m_curPosX;
		FillRect(m_pCanvas, &m_cursorArea, fRemove ? m_backColor : CursorColor);
	}

	void SetCursorRelative(uint posPx)
	{
		SetCursor(posPx + m_scrollPosPx);
	}

	void SetCursor(uint posPx)
	{
		uint	scrollPosPx;

		scrollPosPx = m_scrollPosPx;

		while (posPx >= m_pArea->Width + scrollPosPx + m_pArea->Xpos)
		{
			// Exceeding right boundary
			scrollPosPx += GetCharWidth(m_pBufText[m_scrollPosCh++]);
		}

		while (posPx <= scrollPosPx + m_pArea->Xpos && scrollPosPx != 0)
		{
			// Exceeding left boundary
			scrollPosPx -= GetCharWidth(m_pBufText[--m_scrollPosCh]);
		}

		if (scrollPosPx != m_scrollPosPx)
		{
			// Scrolling required
			m_scrollPosPx = scrollPosPx;
			m_curPosX = m_pArea->Xpos;
			WriteString(&m_pBufText[m_scrollPosCh]);
			ClearToEnd();
		}
		m_curPosX = posPx - m_scrollPosPx;
		DrawCursor();
	}

	void RemoveCursor()
	{
		DrawCursor(true);
	}

	void MoveCursorBy(int offset)
	{
		MoveCursor(m_curPosX + offset + m_scrollPosPx);
	}

	void MoveCursor(uint posNew)
	{
		RemoveCursor();
		SetCursor(posNew);
	}

	//*********************************************************************
	// instance (RAM) data
	//*********************************************************************
protected:
	char	*m_pBufText;
	ushort	m_maxCh;
	ushort	m_cntCh;
	ushort	m_cntPx;
	ushort	m_chPosCur;
	ushort	m_scrollPosPx;
	ushort	m_scrollPosCh;
	Area	m_cursorArea;
};
