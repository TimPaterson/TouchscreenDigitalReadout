//****************************************************************************
// DecimalToFrac.h
//
// Created 4/8/2020 11:43:24 AM by Tim
//
//****************************************************************************


#include <standard.h>
#include <cmath>

template <typename T>
T DecimalToFrac(T Targ, T MaxDen)
{
	T	d0, d1, d2, rem, remFloor;

	d0 = 0;
	d1 = 1;
	rem = 1 / (Targ - std::floor(Targ));
	do 
	{
		remFloor = std::floor(rem);
		rem = 1 / (rem - remFloor);
		d2 = remFloor * d1 + d0;
		d0 = d1;
		d1 = d2;
	} while (d1 <= MaxDen);

	return d0;
}
