//****************************************************************************
// Class RA8876
// RA8876.h
//
// Created 10/11/2020 4:57:27 PM by Tim
//
//****************************************************************************

#pragma once

#include "RA8876const.h"


namespace RA8876const
{
	//*********************************************************************
	// Types

	struct RegValue
	{
		byte	addr;
		byte	val;
	};
};

//*********************************************************************

class RA8876
{
public:

	//*********************************************************************

public:
	static void Init();
	static uint GetStatus();
	static void WriteReg(uint addr, uint val);
	static uint ReadReg(uint addr);

public:
	static void WriteRegList(const RA8876const::RegValue *pList, int iLen)
	{
		do 
		{
			WriteReg(pList->addr, pList->val);
			pList++;
		} while (--iLen > 0);
	}
};
