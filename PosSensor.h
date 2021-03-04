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
	static constexpr int MaxOrigins = 2;
	static constexpr double InchRounding = 5000.0;			// 1/5000 inch
	static constexpr double MmRounding = 100.0;				// 1/100 mm
	static constexpr double MaxCompensationPpm = 1000.0;	// max adjust of 0.1%

public:
	PosSensor(AxisInfo *pInfo) : m_pInfo{pInfo}	{}

public:
	// Called from ISR
	void InputChange(uint uSignal)
	{
		uSignal &= 3;	// low two bits - A and B signals
		m_posCur += s_arbQuadDecode[(m_bPrevSig << 2) + uSignal];
		m_bPrevSig = uSignal;
	}

public:
	uint GetResolution()		{ return m_pInfo->Resolution; }
	bool GetDirection()			{ return m_pInfo->Direction; }
	bool GetDisable()			{ return m_pInfo->Disable; }
	void SetDisable(bool fDis)	{ m_pInfo->Disable = fDis; }
	double GetCorrectionPpm()	{ return (m_pInfo->Correction - 1.0) * 1E6; }

public:
	double GetPosition()
	{
		long		pos;

		pos = m_arOrigins[Eeprom.Data.OriginNum] + m_posCur;
		if (IsMetric())
			return nearbyint(pos * m_scaleMm + m_offsetMm) / MmRounding;

		return nearbyint(pos *  m_scaleInch + m_offsetInch) / InchRounding;
	}

	void SetPosition(double pos)
	{
		// Round to display value first
		if (IsMetric())
		{
			pos = nearbyint(pos * MmRounding - m_offsetMm);
			pos /= m_scaleMm;
		}
		else
		{
			pos = nearbyint(pos * InchRounding - m_offsetInch);
			pos /= m_scaleInch;
		}

		m_arOrigins[Eeprom.Data.OriginNum] = lround(pos) - m_posCur;
	}

	void SetOffset(double offset)
	{
		// Offset is in current units
		if (IsMetric())
		{
			m_offsetMm = offset * MmRounding;
			m_offsetInch = offset / MmPerInch * InchRounding;
		}
		else
		{
			m_offsetInch = offset * InchRounding;
			m_offsetMm = offset * MmPerInch * MmRounding;
		}
	}

	bool SetCorrectionPpm(double pos)
	{
		if (fabs(pos) >= MaxCompensationPpm)
			return false;

		m_pInfo->Correction = pos / 1E6 + 1.0;
		AxisInfoUpdate();
		return true;
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
		m_scaleMm = m_pInfo->Correction * m_pInfo->Resolution * MmRounding / 1000.0;
		if (m_pInfo->Direction)
			m_scaleMm = -m_scaleMm;
		m_scaleInch = m_scaleMm * InchRounding / MmRounding / MmPerInch;
	}

	long GetPosInt()	{ return m_posCur; }

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
	volatile long m_posCur;		// can change in ISR

protected:
	AxisInfo	*m_pInfo;
	double		m_scaleMm;
	double		m_offsetMm;
	double		m_scaleInch;
	double		m_offsetInch;
	long		m_arOrigins[MaxOrigins];
	byte		m_bPrevSig;
};

constexpr double PosSensor::MmRounding;
