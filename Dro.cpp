//****************************************************************************
// Dro.cpp
//
// Created 10/1/2020 12:36:41 PM by Tim
//
//****************************************************************************

#include <standard.h>
#include "Dro.h"
#include "PosSensor.h"
#include "LcdDef.h"
#include "RA8876.h"
#include "Xtp2046.h"


//*********************************************************************
// Functions in DroInit

void StartClock();
void Init();

//****************************************************************************
// Static data

Console_t	Console;
FILE		Console_FILE;

Xtp2046	Touch;
RA8876	Lcd;

//*********************************************************************
// Tests
//*********************************************************************

void HexDump(const byte *pb, int cb)
{
	int		col;

	// Dump on console in hex
	col = 16;

	for (; cb > 0; cb--, col--, pb++)
	{
		if (col == 0)
		{
			DEBUG_PRINT("\n");
			col = 16;
		}
		DEBUG_PRINT("%02X ", *pb);
		wdt_reset();
	}
	DEBUG_PRINT("\n");
}

void TextDisplay()
{
	Lcd.WriteRegRgb(FGCR, 0xFF);
	Lcd.FillRect(0, 0, 1023, 599);
	Lcd.WriteRegRgb(FGCR, 0xFF00);
	Lcd.FillRect(100, 100, 923, 499);

	// Write some text
	Lcd.WriteRegRgb(FGCR, 0xFFFFFF);
	Lcd.WriteRegRgb(BGCR, 0);
	Lcd.WriteReg(CCR1, CCR1_CharHeightX3 | CCR1_CharWidthX3 | CCR1_CharBackgroundTransparent);
	Lcd.WriteRegXY(F_CURX0, 0, 0);
	Lcd.ExternalFont(CCR0_CharHeight16, GTENT_CR_CharWidthFixed | GTFNT_CR_Ascii, 1);
	Lcd.WriteString("0123456789");
	Lcd.WriteRegXY(F_CURX0, 0, 64);
	Lcd.WriteReg(FLDR, 0);
	//Lcd.InternalFont(CCR0_CharHeight32, CCR0_CharSet8859_1);
	Lcd.WriteString("The quick brown fox jumped over the lazy dog.");
}

//*********************************************************************
// Main program
//*********************************************************************

#define MEMLOC (0x1DD780)

byte arbBuf[256];

int main(void)
{
	StartClock();
	Init();
	Timer::Init();

	Console.Init(RXPAD_Pad1, TXPAD_Pad2);
	Console.SetBaudRate(CONSOLE_BAUD_RATE);
	Console.StreamInit(&Console_FILE);
	Console.Enable();

	Touch.Init(SPIMISOPAD_Pad3, SPIOUTPAD_Pad0_MOSI_Pad1_SCK);
	Touch.Enable();

	DEBUG_PRINT("\nTouchscreen starting up version " STRINGIFY(VERSION) "\n");

	Lcd.Init();
	Lcd.WriteReg(AW_COLOR, AW_COLOR_CanvasColor16 | AW_COLOR_AddrModeXY);
	Lcd.DisplayOn();
	TextDisplay();
	Lcd.WriteReg(CCR1, CCR1_CharHeightX3 | CCR1_CharWidthX3 | CCR1_CharBackgroundSet);

	// Start WDT now that initialization is complete
	WDT->CTRL.reg = WDT_CTRL_ENABLE;

	//************************************************************************
	// Main loop

	Timer	tmr;
	int		iCurPos;
	int		iLastPos;
	int		i;

	iLastPos = 0;
	tmr.Start();

    while (1) 
    {
		wdt_reset();

		if (Touch.Process())
		{
			Lcd.WriteRegXY(F_CURX0, 0, 300);
			printf("X: %5i", Touch.GetX());
			Lcd.WriteRegXY(F_CURX0, 0, 350);
			printf("Y: %5i", Touch.GetY());
		}

		if (tmr.CheckInterval_rate(10))
		{
			iCurPos = Xpos.GetPos();
			if (iCurPos != iLastPos)
			{
				DEBUG_PRINT("Pos: %i\n", iCurPos);
				iLastPos = iCurPos;
			}
		}
		if (Console.IsByteReady())
		{
			byte	ch;
			
			ch = Console.ReadByte();
			if (ch >= 'A' && ch <= 'Z')
				ch += 'a' - 'A';

			switch (ch)
			{
				// Use lower-case alphabetical order to find the right letter
			case 'e':
				DEBUG_PRINT("Erase\n");
				Lcd.SerialMemErase(MEMLOC, SFCMD_SectorErase, 1);
				break;

			case 'p':
				DEBUG_PRINT("Program\n");
				for (int i = 0; i < 6; i++)
				{
					Lcd.SerialMemRead(MEMLOC + sizeof arbBuf * i, sizeof arbBuf, arbBuf, 0);
					Lcd.SerialMemWrite(MEMLOC + sizeof arbBuf * i, sizeof arbBuf, arbBuf, 1);
				}
				break;

			case 'r':
				DEBUG_PRINT("Read\n");
				Lcd.SerialMemRead(MEMLOC, sizeof arbBuf, arbBuf, 1);
				HexDump(arbBuf, sizeof arbBuf);
				break;

			case 't':
				TextDisplay();
				break;

			case '+':
			case '=':
				i = TCC1->CC[1].reg;
				i += LcdBacklightPwmMax / 10;
				if (i > LcdBacklightPwmMax)
					i = LcdBacklightPwmMax;
				TCC1->CC[1].reg = i;
				DEBUG_PRINT("up\n");
				break;

			case '-':
				i = TCC1->CC[1].reg;
				i -= LcdBacklightPwmMax / 10;
				if (i < 0)
					i = 0;
				TCC1->CC[1].reg = i;
				DEBUG_PRINT("down\n");
				break;
			}
		}
    }
}
