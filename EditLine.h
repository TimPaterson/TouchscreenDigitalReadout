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
	EditLine(Canvas &canvas, const Area &area, char *pBuf, ushort chMax):
		TextField(canvas, area), m_pBufText{pBuf}, m_maxCh{(ushort)(chMax - 1)}
		{}

	EditLine(Canvas &canvas, const Area &area, char *pBuf, ushort chMax, FontId id, ulong foreColor, ulong backColor):
		TextField(canvas, area, id, foreColor, backColor), m_pBufText{pBuf}, m_maxCh{(ushort)(chMax - 1)}
		{}

	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	void StartEditPx(int pxPosCur)
	{
		StartEditCh(CalcPosition(pxPosCur));
	}

	void StartEditCh(uint chPosCur)
	{
		m_cntCh = strlen(m_pBufText);
		m_scrollPos = 0;
		m_cntPx = GetStringWidth(m_pBufText);
		m_curPosX = m_pArea->Xpos;
		SetPositionCh(chPosCur);
	}

	void EndEdit()
	{
		RemoveCursor();
	}

	void SetPositionPx(uint pxPosCur)
	{
		SetPositionCh(CalcPosition(pxPosCur));
	}

	void SetPositionCh(uint chPosCur) NO_INLINE_ATTR
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

	EditStatus ProcessKey(uint key)
	{
		uint	chWidth;
		uint	pos;

		MakeActive();

		if (key >= 0x20 && key <= 0x7E)
		{
			// handle printable character
			if (m_cntCh >= m_maxCh)
				return EditBufFull;

			chWidth = GetCharWidth(key);
			if (chWidth + m_cntPx - m_scrollPos >= m_pArea->Width)
			{
				// UNDONE: need to scroll the text
			}
			m_cntPx += chWidth;
			if (m_chPosCur < m_cntCh)
			{
				// Inserting, move existing characters and null terminator
				memmove(&m_pBufText[m_chPosCur + 1], &m_pBufText[m_chPosCur], m_cntCh - m_chPosCur + 1);
				m_pBufText[m_chPosCur] = key;
				pos = m_curPosX + chWidth;
				WriteString(&m_pBufText[m_chPosCur]);
				m_curPosX = pos;
			}
			else
			{
				m_pBufText[m_chPosCur] = key;
				m_pBufText[m_chPosCur + 1] = '\0';
				WriteChar(key);
			}
			m_chPosCur++;
			m_cntCh++;
			SetCursor();
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
				m_curPosX = pos;

				if (m_chPosCur < m_cntCh)
				{
					// Not at end, move characters up
					memmove(&m_pBufText[m_chPosCur - 1], &m_pBufText[m_chPosCur], m_cntCh - m_chPosCur + 1);
					WriteString(&m_pBufText[m_chPosCur - 1]);
				}
				else
					m_pBufText[m_chPosCur - 1] = '\0';

				ClearToEnd();
				m_curPosX = pos;
				SetCursor();
				m_cntCh--;
				m_chPosCur--;
				break;

			case Key_left:
				if (m_chPosCur == 0)
					break;

				chWidth = GetCharWidth(m_pBufText[m_chPosCur - 1]);
				MoveCursor(m_curPosX - chWidth);
				m_chPosCur--;
				break;

			case Key_right:
				if (m_chPosCur > m_cntCh)
					break;

				chWidth = GetCharWidth(m_pBufText[m_chPosCur]);
				MoveCursor(m_curPosX + chWidth);
				m_chPosCur++;
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

	//*********************************************************************
	// Compute character position from pixel position
	//
	uint CalcPosition(int pxPos)
	{
		int		pxPosCur;
		uint	chWidth;
		uint	chPos;

		for (pxPosCur = 0, chPos = 0; chPos < m_cntCh; chPos++)
		{
			chWidth = GetCharWidth(m_pBufText[chPos]);
			if (pxPos < (int)(pxPosCur + chWidth / 2))
				break;
			pxPosCur += chWidth;
		}
		return chPos;
	}

	void PrepareDrawCursor()
	{
		ScreenMgr::SetDrawCanvas(m_pCanvas);
		WriteReg16(DLVSR0, m_curPosY);
		WriteReg16(DLVER0, m_curPosY + m_pArea->Height - 1);
	}

	void DrawCursor(bool fRemove = false)
	{
		SetForeColor(fRemove ? m_backColor : CursorColor);
		WriteReg16(DLHSR0, m_curPosX);
		WriteReg16(DLHER0, m_curPosX);
		WriteReg(DCR0, DCR0_DrawLine | DCR0_DrawActive);
		WaitWhileBusy();
		SetForeColor(m_foreColor);
	}

	void SetCursor()
	{
		PrepareDrawCursor();
		DrawCursor();
	}

	void RemoveCursor()
	{
		PrepareDrawCursor();
		DrawCursor(false);
	}

	void MoveCursor(uint posNew) NO_INLINE_ATTR
	{
		PrepareDrawCursor();
		DrawCursor(true);
		m_curPosX = posNew;
		DrawCursor();
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
	ushort	m_scrollPos;
};
