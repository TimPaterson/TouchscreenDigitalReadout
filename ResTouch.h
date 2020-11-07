//****************************************************************************
// ResTouch.h
//
// Created 10/26/2020 5:21:30 PM by Tim
//
//****************************************************************************

#pragma once


struct TouchScreenScale
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
		void Init(const TouchScreenScale *pScale)
		{
			memcpy(&m_scale, pScale, sizeof m_scale);
		}

		void Set(int pos)
		{
			pos -= m_scale.uBase;
			if (pos < 0)
				pos = 0;
			pos *= m_scale.uScale;
			pos >>= ScaleShift;
			if (pos > m_scale.uMax)
				pos = m_scale.uMax;
			if (m_scale.fReverse)
				pos = m_scale.uMax - pos;
			m_uCur = pos;
		}

		uint Get()	{ return m_uCur; }

	protected:
		ushort	m_uCur;
		TouchScreenScale	m_scale;
	};

public:
	uint GetX()		{ return m_posX.Get(); }
	uint GetY()		{ return m_posY.Get(); }
	uint GetTouch()	{ return m_touchFlags; }

public:
	void InitScale(const TouchScreenScale *pScaleX, const TouchScreenScale *pScaleY)
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
