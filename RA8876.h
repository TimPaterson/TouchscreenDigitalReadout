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
	static void WriteData16(uint val);
	static uint ReadData16();
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

	static bool IsBusy()
	{
		return GetStatus() & STATUS_CoreBusy;
	}

	static void WaitWhileBusy()
	{
		while (IsBusy());
	}

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

	static uint ReadReg16(uint addr)
	{
		uint val  = ReadReg(addr);
		return (ReadReg(addr + 1) << 8) + val;
	}

	static void WriteReg32(uint addr, ulong val)
	{
		WriteReg16(addr, val);
		WriteReg16(addr + 2, val >> 16);
	}

	static ulong ReadReg32(uint addr)
	{
		uint val  = ReadReg16(addr);
		return (ReadReg16(addr + 2) << 16) + val;
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

	static bool IsFifoWriteReady()
	{
		return !(GetStatus() & STATUS_WriteFifoFull);
	}

	static void WaitFifoWrite()
	{
		while (!IsFifoWriteReady());	// wait for space
	}

	static void FifoWrite(uint val)
	{
		WaitFifoWrite();
		WriteReg(MRWDP, val);
	}

	static void FifoWrite16(uint val)
	{
		WaitFifoWrite();
		WriteAddr(MRWDP);
		WriteData16(val);
	}

	static void FastFifoWrite16(uint val)
	{
		// Assumes address register already set
		WaitFifoWrite();
		WriteData16(val);
	}

	static bool IsFifoReadReady()
	{
		return !(GetStatus() & STATUS_ReadFifoEmpty);
	}

	static void WaitFifoRead()
	{
		while (!IsFifoReadReady());	// wait for data
	}

	static byte FifoRead()
	{
		WaitFifoRead();
		return ReadReg(MRWDP);
	}

	static ushort FifoRead16()
	{
		WaitFifoRead();
		WriteAddr(MRWDP);
		return ReadData16();
	}

	static ushort FastFifoRead16()
	{
		// Assumes address register already set
		WaitFifoRead();
		return ReadData16();
	}

	static void WriteSequentialRegisters(const void *pv, uint addr, int cnt)
	{
		byte *pb = (byte *)pv;

		do 
		{
			WriteReg(addr++, *pb++);
		} while (--cnt > 0);
	}

	//*********************************************************************
	// Function-specific handlers

	static void SetMainWindowPos(uint X, uint Y)
	{
		WriteReg16(MWULX0, X);
		WriteReg16(MWULY0, Y);
	}

	static void SetCanvasView(ulong addr, uint width)
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

	static void SetForeColor(ulong color)
	{
		WriteRegRgb(FGCR, color);
	}

	static void SetBackColor(ulong color)
	{
		WriteRegRgb(BGCR, color);
	}

	static void SetTwoPoints(uint X0, uint Y0, uint X1, uint Y1)
	{
		WriteReg16(DLHSR0, X0);
		WriteReg16(DLVSR0, Y0);
		WriteReg16(DLHER0, X1);
		WriteReg16(DLVER0, Y1);
	}

	static void LoadGraphicsCursor(const byte *pbCursor, uint id)
	{
		uint	icr;
		uint	gtccr;

		icr = ReadReg(ICR);
		WriteData(icr | ICR_MemPortCursor);
		gtccr = ReadReg(GTCCR);
		WriteData((gtccr & ~GTCCR_GraphicCursorSelect_Mask) | id);

		// Write 256-byte block to data port
		WriteAddr(MRWDP);
		for (int i = 0; i < 256; i++)
			WriteData(*pbCursor++);

		// Restore registers
		WriteReg(GTCCR, gtccr);
		WriteReg(ICR, icr);
	}

	static void SetGraphicsCursorColors(byte color0, byte color1)
	{
		WriteReg(GCC0, color0);
		WriteReg(GCC1, color1);
	}

	static void SetGraphicsCursorPosition(uint X, uint Y)
	{
		WriteReg16(GCHP0, X);
		WriteReg16(GCVP0, Y);
	}

	static void EnableGraphicsCursor(uint id)
	{
		WriteData((ReadReg(GTCCR) & ~GTCCR_GraphicCursorSelect_Mask) | id | GTCCR_GraphicsCursorEnable);
	}

	static void DisableGraphicsCursor()
	{
		WriteData(ReadReg(GTCCR) & ~GTCCR_GraphicsCursorEnable);
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
		WaitWhileBusy();
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
		WaitWhileBusy();
		WriteReg(ICR, reg);
	}

	static void InternalFont(uint charHeight, uint font)
	{
		WriteReg(CCR0, charHeight | CCR0_CharSourceInternal | font);
	}

	static void ExternalFont(uint charHeight, uint font, uint port = 0)
	{
		WriteReg(CCR0, charHeight | CCR0_CharSourceExternal);
		WriteReg(GTFNT_CR, font);
		SerialSelectPort(SFL_CTRL_ModeFont, port);
	}

	//*********************************************************************
	// Serial Flash/ROM
	//
	// NOTE: These functions block until completion.

	static byte SerialReadByte()
	{
		//while (ReadReg(SPIMSR) & SPIMSR_RxFifoEmpty);	// wait for byte
		return ReadReg(SPIDR);
	}

	static void SerialWriteByte(byte val)
	{
		//while (ReadReg(SPIMSR) & SPIMSR_TxFifoFull);	// wait for space
		WriteReg(SPIDR, val);
	}

	static void SerialSelectPort(uint mode, uint port)
	{
		WriteReg(SFL_CTRL, (port ? SFL_CTRL_Init1 : SFL_CTRL_Init0) | mode);
		WriteReg(SPI_DIVSOR, port ? SpiDivisor1 : SpiDivisor0);
	}

	static byte SerialMemGetStatus()
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

	static bool IsSerialMemBusy()
	{
		return SerialMemGetStatus() & SFSTAT_Busy;
	}

	static void SerialMemRead(ulong addr, int cb, byte *pb, uint port) NO_INLINE_ATTR
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

	static int SerialMemWriteStart(ulong addr, int cb, void *pv, uint port)
	{
		byte	*pb = (byte *)pv;
		int		cbPage;
		byte	bCs;

		WriteReg(SPI_DIVSOR, port ? SpiDivisor1 : SpiDivisor0);
		bCs = SPIMCR_SpiMode0 | (port ? SPIMCR_SlaveSelectCs1 : SPIMCR_SlaveSelectCs0);
		cbPage = cb;

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
		} while (--cbPage > 0 && (addr % SerialFlashPageSize) != 0);

		// Wait for SPI to finish
		while (!(ReadReg(SPIMSR) & SPIMSR_Idle));
		// Release CS, starting write operation
		WriteReg(SPIMCR, bCs);
		return cb - cbPage;
	}

	static void SerialMemWrite(ulong addr, int cb, void *pv, uint port) NO_INLINE_ATTR
	{
		byte	*pb = (byte *)pv;
		int		cbPage;

		do 
		{
			cbPage = SerialMemWriteStart(addr, cb, pb, port);
			addr += cbPage;
			pb += cbPage;
			cb -= cbPage;
			while (IsSerialMemBusy());
		} while (cb > 0);
	}

	static void SerialMemErase(ulong addr, int cb, uint port) NO_INLINE_ATTR
	{
		int		cbBase;

		while (cb > 0)
		{
			// Do a full 32K block if aligned and want that much
			if (cb >= SerialFlashBlockSize && addr % SerialFlashBlockSize == 0)
			{
				SerialMemEraseCmd(addr, SFCMD_BlockErase, port);
				cbBase = SerialFlashBlockSize;
			}
			else
			{
				// This could be unaligned, meaning we'll erase stuff
				// preceding the address. Sorry about that.
				SerialMemEraseCmd(addr, SFCMD_SectorErase, port);
				// Count only the bytes we intended (subtract preceding bytes)
				cbBase = SerialFlashSectorSize - addr % SerialFlashSectorSize;
			}
			cb -= cbBase;
			addr += cbBase;
		}
	}

	static void SerialMemEraseCmd(ulong addr, uint cmd, uint port) NO_INLINE_ATTR
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

	void CopySerialMemToRam(ulong addrFlash, ulong addrRam, int cb, uint port) NO_INLINE_ATTR
	{
		byte	modeSave;

		SerialSelectPort(SFL_CTRL_ModeDma, port);
		modeSave = ReadReg(AW_COLOR);
		WriteData(AW_COLOR_DataWidth16 | AW_COLOR_AddrModeLinear);
		WriteReg32(DMA_SSTR0, addrFlash);
		WriteReg32(DMA_DX0, addrRam);
		WriteReg32(DMAW_WTH0, cb);
		WriteReg(DMA_CTRL, DMA_CTRL_Start);
		WaitWhileBusy();
		// Restore mode
		WriteReg(AW_COLOR, modeSave);
	}
};
