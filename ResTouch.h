//****************************************************************************
// ResTouch.h
//
// Created 10/26/2020 5:21:30 PM by Tim
//
//****************************************************************************

#pragma once


struct TouchPoint
{
	ushort	x;
	ushort	y;
};

struct TouchCalPoints
{
	TouchPoint	topLeft;
	TouchPoint	bottomRight;
};

enum TouchFlags
{
	TOUCH_None = 0,
	TOUCH_Start = 1,
	TOUCH_End = 2,
	TOUCH_Touched = 4,
};

static constexpr int TouchEdgeOffset = 32;
static constexpr ushort TouchPointInvalid = 0xFFFF;


class ResTouch
{
	static constexpr int TouchShift = 18;

	// Types
protected:
	class Position
	{
	public:
		void Set(int pos)
		{
			pos -= m_base;
			pos = ShiftIntRnd(pos * m_scale, TouchShift);
			if (pos < 0)
				pos = 0;
			else if (pos > m_max)
				pos = m_max;
			m_uCur = pos;
		}

		void CalcScale(int rawA, int rawB, uint size)
		{
			m_max = size - 1;
			m_scale = DivIntRnd((size - 2 * TouchEdgeOffset) << TouchShift, rawB - rawA);
			m_base = rawA - DivIntRnd(TouchEdgeOffset << TouchShift, m_scale);
		}

		uint Get()	{ return m_uCur; }

	protected:
		int		m_scale;
		short	m_base;
		ushort	m_max;
		ushort	m_uCur;
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

	void SetCalPoint(TouchPoint *pPoint, uint x, uint y)
	{
		pPoint->x = x;
		pPoint->y  = y;
	}

	void CalcScales(TouchCalPoints *pPoints, uint width, uint height)
	{
		m_posX.CalcScale(pPoints->topLeft.x, pPoints->bottomRight.x, width);
		m_posY.CalcScale(pPoints->topLeft.y, pPoints->bottomRight.y, height);
	}

	bool IsTouchDataValid(TouchCalPoints *pPoints)
	{
		return pPoints->topLeft.x != TouchPointInvalid && 
			pPoints->topLeft.y != TouchPointInvalid &&
			pPoints->bottomRight.x != TouchPointInvalid &&
			pPoints->bottomRight.y != TouchPointInvalid;
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
