//****************************************************************************
// ResTouch.h
//
// Created 10/26/2020 5:21:30 PM by Tim
//
//****************************************************************************

#pragma once


struct TouchScaleAxis
{
	ushort	uBase;
	ushort	uScale;
	ushort	uMax;
	bool	fReverse;
};

enum TouchFlags
{
	TOUCH_None = 0,
	TOUCH_Start = 1,
	TOUCH_End = 2,
	TOUCH_Touched = 4,
};

class ResTouch
{
	static constexpr int ScaleShift = 16;

	// Types
protected:
	class Position
	{
	public:
		void Init(TouchScaleAxis *pScale)
		{
			m_pScale = pScale;
		}

		void Calibrate(int posRead, int posTarget, int posAnchor) NO_INLINE_ATTR
		{
			int	rawRead, rawAnchor;
			int	base, scale;

			// Adjust scale and base to move posRead to posTarget
			// without changing posAnchor

			// First calculate the raw touch values before scaling
			if (m_pScale->fReverse)
			{
				posRead = m_pScale->uMax - posRead;
				posAnchor = m_pScale->uMax - posAnchor;
				posTarget = m_pScale->uMax - posTarget;
			}
			rawRead = DivUintRnd(posRead << ScaleShift, m_pScale->uScale) + m_pScale->uBase;
			rawAnchor = DivUintRnd(posAnchor << ScaleShift, m_pScale->uScale) + m_pScale->uBase;

			/*
			Now we have two equations in two unknowns, base and scale:

			(1) posTarget = (rawRead - base) * scale
			(2) posAnchor = (rawAnchor - base) * scale

			Get base in terms of scale:

			(3) posTarget / scale = rawRead - base
			(4) base = rawRead - posTarget / scale

			Substitute this for base in (2):

			(5) posAnchor = (rawAnchor - rawRead + posTarget / scale) * scale
			(6) posAnchor = (rawAnchor - rawRead) * scale + posTarget
			(7) (rawAnchor - rawRead) * scale = posAnchor - posTarget
			(8) scale = (posAnchor - posTarget) / (rawAnchor - rawRead)

			*/

			scale = DivIntRnd((posAnchor - posTarget) << ScaleShift, rawAnchor - rawRead);
			base = rawRead - DivUintRnd(posTarget << ScaleShift, scale);
			DEBUG_PRINT("\nScale: %i to %i, Base: %i to %i\n", m_pScale->uScale, scale, m_pScale->uBase, base);
			m_pScale->uScale = scale;
			m_pScale->uBase = base;		
		}

		void Set(int pos)
		{
			pos -= m_pScale->uBase;
			if (pos < 0)
				pos = 0;
			pos *= m_pScale->uScale;
			pos = ShiftUintRnd(pos, ScaleShift);
			if (pos > m_pScale->uMax)
				pos = m_pScale->uMax;
			if (m_pScale->fReverse)
				pos = m_pScale->uMax - pos;
			m_uCur = pos;
		}

		uint Get()	{ return m_uCur; }

	protected:
		ushort	m_uCur;
		TouchScaleAxis	*m_pScale;
	};

public:
	uint GetX()		{ return m_posX.Get(); }
	uint GetY()		{ return m_posY.Get(); }
	uint GetTouch()	
	{
		uint flags = m_touchFlags;
		m_touchFlags &= ~(TOUCH_Start | TOUCH_End);	//  clear edge triggered events
		return flags;
	}

	void CalibrateX(int posRead, int posTarget, int posAnchor)
		{ m_posX.Calibrate(posRead, posTarget, posAnchor); }

	void CalibrateY(int posRead, int posTarget, int posAnchor)
		{ m_posY.Calibrate(posRead, posTarget, posAnchor); }

public:
	void InitScale(TouchScaleAxis *pScaleX, TouchScaleAxis *pScaleY)
	{
		m_posX.Init(pScaleX);
		m_posY.Init(pScaleY);
	}

protected:
	bool ProcessRaw(ushort rawX, ushort rawY)
	{
		m_posX.Set(rawX);
		m_posY.Set(rawY);
		return true;
	}

	void IsTouched(bool fIsTouched)	
	{
		uint	flags;

		flags = m_touchFlags;
		if (fIsTouched)
		{
			if (!(flags & TOUCH_Touched))
				flags = TOUCH_Start | TOUCH_Touched;
			else
				flags = TOUCH_Touched;
		}
		else
		{
			if (flags & TOUCH_Touched)
				flags = TOUCH_End;
			else
				flags = TOUCH_None;
		}
		m_touchFlags = flags;
	}

protected:
	Position	m_posX;
	Position	m_posY;
	byte		m_touchFlags;
};
