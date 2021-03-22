//****************************************************************************
// ResTouch.h
//
// Created 10/26/2020 5:21:30 PM by Tim
//
//****************************************************************************

#pragma once


struct ScaleMatrix
{
	int		aScale;
	int		bScale;
	int		base;
};

enum TouchFlags
{
	TOUCH_None = 0,
	TOUCH_Start = 1,
	TOUCH_End = 2,
	TOUCH_Touched = 4,
};

static constexpr int TouchShift = 18;
static constexpr int TouchScale = 1 << TouchShift;


class ResTouch
{
	// Types
protected:
	class Position
	{
	public:
		void Set(int posA, int posB)
		{
			posA = posA * m_pScale->aScale + posB *m_pScale->bScale;
			posA = ShiftIntRnd(posA, TouchShift) + m_pScale->base;
			if (posA < 0)
				posA = 0;
			else if (posA > m_max)
				posA = m_max;
			m_cur = posA;
		}

		uint Get()									{ return m_cur; }
		void SetMax(uint max)						{ m_max = max; }
		void SetMatrix(const ScaleMatrix &scale)	{ m_pScale = &scale; }

	protected:
		const ScaleMatrix	*m_pScale;
		ushort		m_max;
		ushort		m_cur;
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

protected:
	void ProcessRaw(ushort rawX, ushort rawY)
	{
		m_posX.Set(rawX, rawY);
		m_posY.Set(rawY, rawX);
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

	void SetMax(uint maxX, uint maxY)
	{
		m_posX.SetMax(maxX);
		m_posY.SetMax(maxY);
	}

	void SetMatrix(const ScaleMatrix &x, const ScaleMatrix &y)
	{
		m_posX.SetMatrix(x);
		m_posY.SetMatrix(y);
	}

protected:
	Position	m_posX;
	Position	m_posY;
	byte		m_touchFlags;
};
