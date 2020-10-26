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

	enum SerialFlashCommands
	{
		SFCMD_Read = 0x03,
		SFCMD_FastRead = 0x0B,
		SFCMD_WriteEnable = 0x06,
		SFCMD_Program = 0x02,
		SFCMD_SectorErase = 0x20,
		SFCMD_BlockErase = 0x52,
		SFCMD_ChipErase = 0x60,
		SFCMD_ReadStatus = 0x05,
		SFCMD_WriteStatus = 0x01,
	};

	enum SerialFlashStatus
	{
		SFSTAT_Busy = 0x01,
		SFSTAT_WriteEnabled = 0x02,
	};
};

//*********************************************************************

using namespace RA8876const;

class RA8876
{
public:

	//*********************************************************************

public:
	static void Init();
	static uint GetStatus();
	static void WriteAddr(uint addr);
	static void WriteData(uint val);
	static uint ReadData();
	static void TestPattern();
	static void DisplayOn();
	static void DisplayOff();

	static void WriteReg(uint addr, uint val)
	{
		WriteAddr(addr);
		WriteData(val);
	}

	static uint ReadReg(uint addr)
	{
		WriteAddr(addr);
		return ReadData();
	}

	//*********************************************************************
	// Generic register combination handlers

	static void WriteRegList(const RegValue *pList, int iLen)
	{
		do 
		{
			WriteReg(pList->addr, pList->val);
			pList++;
		} while (--iLen > 0);
	}

	static void WriteReg16(uint addr, uint val)
	{
		WriteReg(addr, val);
		WriteReg(addr + 1, val >> 8);
	}

	static void WriteReg32(uint addr, ulong val)
	{
		WriteReg16(addr, val);
		WriteReg16(addr + 2, val >> 16);
	}

	static void WriteRegXY(uint addr, uint X, uint Y)
	{
		WriteReg16(addr, X);
		WriteReg16(addr + 2, Y);
	}

	static void WriteRegRgb(uint addr, ulong val)
	{
		// standard format is RRGGBB, i.e., blue is
		// LSB. In register addresses, red is first.
		WriteReg(addr, val >> 16);		// red
		WriteReg(addr + 1, val >> 8);	// green
		WriteReg(addr + 2, val);		// blue
	}

	//*********************************************************************
	// Function-specific handlers

	static void SetMainImage(ulong addr, uint width)
	{
		WriteReg32(MISA0, addr);
		WriteReg16(MIW0, width);
		SetMainWindowPos(0, 0);
		SetCanvas(addr, width);
	}

	static void SetMainWindowPos(uint X, uint Y)
	{
		WriteReg16(MWULX0, X);
		WriteReg16(MWULY0, Y);
	}

	static void SetCanvas(ulong addr, uint width)
	{
		WriteReg32(CVSSA0, addr);
		WriteReg16(CVS_IMWTH0, width);
		SetActiveWindowPos(0, 0);
	}

	static void SetActiveWindowPos(uint X, uint Y)
	{
		WriteReg16(AWUL_X0, X);
		WriteReg16(AWUL_Y0, Y);
	}

	static void SetActiveWindowSize(uint width, uint height)
	{
		WriteReg16(AW_WTH0, width);
		WriteReg16(AW_HT0, height);
	}

	//*********************************************************************
	// Graphics Engine

	static void SetTwoPoints(uint X0, uint Y0, uint X1, uint Y1)
	{
		WriteReg16(DLHSR0, X0);
		WriteReg16(DLVSR0, Y0);
		WriteReg16(DLHER0, X1);
		WriteReg16(DLVER0, Y1);
	}

	static void FillRect(uint X0, uint Y0, uint X1, uint Y1)
	{
		SetTwoPoints(X0, Y0, X1, Y1);
		WriteReg(DCR1, DCR1_DrawRect | DCR1_FillOn | DCR1_DrawActive);
		while (GetStatus() & STATUS_CoreBusy);
	}

	//*********************************************************************
	// Text Engine

	static void TextMode()
	{
		WriteData(ReadReg(ICR) | ICR_TextMode);
	}

	static void GraphicsMode()
	{
		WriteData(ReadReg(ICR) & ~ICR_TextGraphicsMode_Mask);
	}

	// This used by printf
	static void WriteChar(void *pv, char ch)
	{
		uint	reg;

		// Slow, but gets there
		reg = ReadReg(ICR);
		WriteData(reg | ICR_TextMode);
		WriteReg(MRWDP, ch);
		while (GetStatus() & STATUS_CoreBusy);
		WriteReg(ICR, reg);
	}

	static void WriteString(const char *psz)
	{
		uint	reg;
		char	ch;

		reg = ReadReg(ICR);
		WriteData(reg | ICR_TextMode);
		WriteAddr(MRWDP);
		while ((ch = *psz++) != 0)
		{
			WriteData(ch);
			while (GetStatus() & STATUS_WriteFifoFull);
		}
		while (GetStatus() & STATUS_CoreBusy);
		WriteReg(ICR, reg);
	}

	static void InternalFont(uint charHeight, uint font)
	{
		WriteReg(CCR0, charHeight | CCR0_CharSourceInternal | font);
	}

	void ExternalFont(uint charHeight, uint font, uint port = 0)
	{
		WriteReg(CCR0, charHeight | CCR0_CharSourceExternal);
		WriteReg(GTFNT_CR, font);
		WriteReg(SFL_CTRL, (port ? SFL_CTRL_Init1 : SFL_CTRL_Init0) | SFL_CTRL_ModeFont);
		WriteReg(SPI_DIVSOR, port ? SpiDivisor1 : SpiDivisor0);
	}

	//*********************************************************************
	// Serial Flash/ROM
	//
	// NOTE: These functions block until completion.

	byte SerialReadByte()
	{
		//while (ReadReg(SPIMSR) & SPIMSR_RxFifoEmpty);	// wait for byte
		return ReadReg(SPIDR);
	}

	void SerialWriteByte(byte val)
	{
		//while (ReadReg(SPIMSR) & SPIMSR_TxFifoFull);	// wait for space
		WriteReg(SPIDR, val);
	}

	byte SerialMemGetStatus()
	{
		byte	bCs;
		byte	val;

		bCs = ReadReg(SPIMCR);
		WriteData(bCs | SPIMCR_SlaveSelectActive);
		WriteReg(SPIDR, SFCMD_ReadStatus);
		WriteReg(SPIDR, 0);
		SerialReadByte();		// dummy byte
		val = SerialReadByte();	// status
		WriteReg(SPIMCR, bCs);
		return val;
	}

	void SerialMemRead(ulong addr, int cb, byte *pb, uint port)
	{
		uint	val;
		byte	bCs;
		int		cbWrite;
		int		cbRead;

		WriteReg(SPI_DIVSOR, port ? SpiDivisor1 : SpiDivisor0);
		bCs = SPIMCR_SpiMode0 | (port ? SPIMCR_SlaveSelectCs1 : SPIMCR_SlaveSelectCs0);
		WriteReg(SPIMCR, bCs | SPIMCR_SlaveSelectActive);

		// Chip is selected, send command
		WriteReg(SPIDR, SFCMD_FastRead);
		WriteData(addr >> 16);
		WriteData(addr >> 8);
		WriteData(addr);
		WriteData(0);		// dummy byte for fast read

		cbWrite = cb;
		cbRead = -5;	// ignore response to command bytes
		do
		{
			val = SerialReadByte();
			if (cbRead >= 0)
				*pb++ = val;
			cbRead++;
			if (cbWrite > 0)
			{
				WriteReg(SPIDR, 0);
				cbWrite--;
			}
		} while (cbRead < cb);

		WriteReg(SPIMCR, bCs);
	}

	void SerialMemWrite(ulong addr, int cb, byte *pb, uint port)
	{
		byte	bCs;

		WriteReg(SPI_DIVSOR, port ? SpiDivisor1 : SpiDivisor0);
		bCs = SPIMCR_SpiMode0 | (port ? SPIMCR_SlaveSelectCs1 : SPIMCR_SlaveSelectCs0);

		do 
		{
			// Enable writes
			WriteReg(SPIMCR, bCs | SPIMCR_SlaveSelectActive);
			WriteReg(SPIDR, SFCMD_WriteEnable);
			WriteReg(SPIMCR, bCs);

			// Clear SPI status
			WriteReg(SPIMSR, SPIMSR_Idle);

			// Perform write, within a 256-byte page
			WriteReg(SPIMCR, bCs | SPIMCR_SlaveSelectActive);
			WriteReg(SPIDR, SFCMD_Program);
			WriteData(addr >> 16);
			WriteData(addr >> 8);
			WriteData(addr);

			do
			{
				SerialWriteByte(*pb++);
				addr++;
			} while (--cb > 0 && (addr & 0xFF) != 0);

			// Wait for SPI to finish
			while (!(ReadReg(SPIMSR) & SPIMSR_Idle));
			// Release CS, starting write operation
			WriteReg(SPIMCR, bCs);

			// Poll for completion
			while (SerialMemGetStatus() & SFSTAT_Busy);
		} while (cb > 0);
	}

	void SerialMemErase(ulong addr, uint cmd, uint port)
	{
		byte	bCs;

		WriteReg(SPI_DIVSOR, port ? SpiDivisor1 : SpiDivisor0);
		bCs = SPIMCR_SpiMode0 | (port ? SPIMCR_SlaveSelectCs1 : SPIMCR_SlaveSelectCs0);

		// Enable writes
		WriteReg(SPIMCR, bCs | SPIMCR_SlaveSelectActive);
		WriteReg(SPIDR, SFCMD_WriteEnable);
		WriteReg(SPIMCR, bCs);

		// Clear SPI status
		WriteReg(SPIMSR, SPIMSR_Idle);

		// Send command with address
		WriteReg(SPIMCR, bCs | SPIMCR_SlaveSelectActive);
		WriteReg(SPIDR, cmd);
		WriteReg(SPIDR, addr >> 16);
		WriteReg(SPIDR, addr >> 8);
		WriteReg(SPIDR, addr);

		// Wait for SPI to finish
		while (!(ReadReg(SPIMSR) & SPIMSR_Idle));
		// Release CS, starting write operation
		WriteReg(SPIMCR, bCs);

		// Poll for completion
		while (SerialMemGetStatus() & SFSTAT_Busy);
	}
};
