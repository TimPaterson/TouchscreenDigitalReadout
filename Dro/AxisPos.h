//****************************************************************************
// AxisPos.h
//
// Created 11/20/2021 12:56:17 PM by Tim
//
//****************************************************************************

#pragma once

#include "PosSensor.h"


class AxisPos : public PosSensor
{
public:
	AxisPos(SensorInfo *pInfoZ, PosSensor *pSense = NULL) : 
		PosSensor(pInfoZ), m_pSenseQ(pSense) {}

public:
	double GetPosition()
	{
		double	pos;

		pos = PosSensor::GetPosition();

		if (m_pSenseQ != NULL && !m_pSenseQ->IsDisabled())
			pos += m_pSenseQ->GetPosition();

		return pos;
	}

	long SetPosition(double pos)
	{
		if (m_pSenseQ != NULL && !m_pSenseQ->IsDisabled())
		{
			// We will be removing Q's affect on position.
			// Adjust desired position to include Q's current contribution.
			// We use current units so Q and Z resolution can be different.
			pos -= m_pSenseQ->GetDistance(m_pSenseQ->GetRelativePos());
		}

		return PosSensor::SetPosition(pos);
	}

	long GetSavePos()
	{
		if (m_pSenseQ != NULL && !m_pSenseQ->IsDisabled())
		{
			// Adjust the Z origins by Q's contribution. This allows us to
			// only save Z to remember our position.
			long delta = -PosSensor::SetPosition(AxisPos::GetPosition());

			// Current origin adjusted, fix the others
			for (int i = 0; i < MaxOrigins; i++)
			{
				if (i != Eeprom.Data.OriginNum)
					AdjustOrigin(i, delta);
			}
		}

		return GetRelativePos();
	}

	//*********************************************************************
	// member (RAM) data
	//*********************************************************************
protected:
	PosSensor	*m_pSenseQ;
};

