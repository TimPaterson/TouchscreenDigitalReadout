//****************************************************************************
// Class PosSensor
// PosSensor.h
//
// Created 10/13/2020 12:57:40 PM by Tim
//
//****************************************************************************

#pragma once


class PosSensor
{
	static constexpr int MaxOrigins = 3;
	static constexpr double MmPerInch = 25.4;
	static constexpr double MaxCompensation = 0.001;	// max adjust of 0.1%

public:
	PosSensor(AxisInfo *pInfo) : m_pInfo{pInfo}	{}

public:
	// Called from ISR
	void InputChange(uint uSignal)
	{
		uSignal &= 3;	// low two bits - A and B signals
		m_iCurPos += s_arbQuadDecode[(m_bPrevSig << 2) + uSignal];
		m_bPrevSig = uSignal;
	}

public:
	double GetPosition()
	{
		int		pos;

		pos = m_arOrigins[Eeprom.Data.OriginNum] + m_iCurPos;
		return pos * (Eeprom.Data.fIsMetric ? m_scaleMm : m_scaleInch);
	}

	void SetPosition(double pos)
	{
		int		iPos;

		iPos = lround(pos / (Eeprom.Data.fIsMetric ? m_scaleMm : m_scaleInch));
		m_arOrigins[Eeprom.Data.OriginNum] = iPos - m_iCurPos;
	}


	bool SetCompensation(double pos)
	{
		double	scale;
		double	curPos;

		curPos = GetPosition();
		if (curPos != 0)
		{
			scale = m_pInfo->Compensation * pos / curPos;
			if (fabs(scale - 1.0) > MaxCompensation)
				return false;
			m_pInfo->Compensation = scale;
			AxisInfoUpdate();
			return true;
		}
		return false;
	}

	void SetResolution(uint res)
	{
		m_pInfo->Resolution = res;
		AxisInfoUpdate();
	}

	void SetDirection(bool dir)
	{
		m_pInfo->Direction = dir;
		AxisInfoUpdate();
	}

	void AxisInfoUpdate()
	{
		m_scaleMm = m_pInfo->Compensation * m_pInfo->Resolution / 1000.0;
		if (m_pInfo->Direction)
			m_scaleMm = -m_scaleMm;
		m_scaleInch = m_scaleMm / MmPerInch;
	}

	int GetPosInt()	{ return m_iCurPos; }

public:
	static bool IsMetric()
	{
		return Eeprom.Data.fIsMetric;
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************
protected:
	inline static const sbyte s_arbQuadDecode[16]
	{
	//			Bprev	Aprev	Bcur	Acur
		 0, //	 0		 0		 0		 0		no change
		+1, //	 0		 0		 0		 1		A rise, B lo
		-1, //	 0		 0		 1		 0		B rise, A lo
		 0, //	 0		 0		 1		 1		both change, invalid
		-1, //	 0		 1		 0		 0		A fall, B lo
		 0, //	 0		 1		 0		 1		no change
		 0, //	 0		 1		 1		 0		both change, invalid
		+1, //	 0		 1		 1		 1		B rise, A hi
		+1, //	 1		 0		 0		 0		B fall, A lo
		 0, //	 1		 0		 0		 1		both change, invalid
		 0, //	 1		 0		 1		 0		no change
		-1, //	 1		 0		 1		 1		A rise, B hi
		 0, //	 1		 1		 0		 0		both change, invalid
		-1, //	 1		 1		 0		 1		B fall, A hi
		+1, //	 1		 1		 1		 0		A fall, B hi
		 0, //	 1		 1		 1		 1		no change
	};

	//*********************************************************************
	// member (RAM) data
	//*********************************************************************
protected:
	volatile int m_iCurPos;		// can change in ISR

protected:
	AxisInfo	*m_pInfo;
	double		m_scaleMm;
	double		m_scaleInch;
	int			m_arOrigins[MaxOrigins];
	byte		m_bPrevSig;
};
