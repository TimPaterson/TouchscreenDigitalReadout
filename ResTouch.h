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
	uint GetX()	{ return m_posX.Get(); }
	uint GetY()	{ return m_posY.Get(); }

public:
	void InitScale(const TouchScreenScale *pScaleX, const TouchScreenScale *pScaleY)
	{
		m_posX.Init(pScaleX);
		m_posY.Init(pScaleY);
	}

protected:
	bool ProcessRaw(ushort rawX, ushort rawY, ushort rawZ)
	{
		if (rawZ < m_minZtouch)
			return false;

		m_posX.Set(rawX);
		m_posY.Set(rawY);
		return true;
	}

protected:
	Position m_posX;
	Position m_posY;
	ushort m_minZtouch = 100;
};
