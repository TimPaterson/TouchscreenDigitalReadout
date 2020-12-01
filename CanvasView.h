//****************************************************************************
// CanvasView.h
//
// Created 11/29/2020 4:25:06 PM by Tim
//
//****************************************************************************

#pragma once


class CanvasView : public Canvas
{
public:
	void SetCanvasView(uint addr)
	{
		// Canvas view registers all fall into the same sequence:
		// 10 - Canvas registers
		// 2 - Viewport width
		// 2 - Viewport height
		//
		// The sequence applies to:
		// - PIP image (PISA0)
		// - Canvas (CVSSA0)
		// - BTE destination (DT_STR0)

		SetCanvas(addr);
		WriteRegXY(addr + 10, m_viewWidth, m_viewHeight);
	}

	//*********************************************************************
	// instance data
	//*********************************************************************
protected:
	ushort	m_viewWidth;
	ushort	m_viewHeight;
};
