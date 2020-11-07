//****************************************************************************
// HitTest.h
//
// Created 11/6/2020 4:56:31 PM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// These are compatible with C so we can use it's superior initialization

typedef struct
{
	ushort	X1;
	ushort	Y1;
	ushort	X2;
	ushort	Y2;
} Hotspot;

typedef struct
{
	ushort	count;
	Hotspot	HotspotList[];
} HotspotList;

//****************************************************************************
// class HitTest

#ifdef __cplusplus

class HitTest
{
public:
	void SetList(const void *pSpots)	{ m_pSpots = (HotspotList *)pSpots; }
	void SetOrigin(uint x, uint y)		{ m_Xorigin = x; m_Yorigin = y; }

public:
	uint TestHit(int x, int y)
	{
		int		i;

		x -= m_Xorigin;
		y -= m_Yorigin;
		if (x < 0 || y < 0)
			return -1;

		for (i = 0; i < m_pSpots->count; i++)
		{
			if (x >= m_pSpots->HotspotList[i].X1 && x <= m_pSpots->HotspotList[i].X2 &&
				y >= m_pSpots->HotspotList[i].Y1 && y <= m_pSpots->HotspotList[i].Y2)
				return i;
		}

		return -1;
	}

	int		m_Xorigin;
	int		m_Yorigin;
	HotspotList	*m_pSpots;
};

#endif	// __cplusplus