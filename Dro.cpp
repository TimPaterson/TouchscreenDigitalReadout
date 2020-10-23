//****************************************************************************
// Dro.cpp
//
// Created 10/1/2020 12:36:41 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "PosSensor.h"
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
FDEV_STANDARD_STREAMS(&Console_FILE, NULL);

Xtp2046	Touch;
RA8876	Lcd;

//*********************************************************************
// Main program
//*********************************************************************

int main(void)
{
	StartClock();
	Init();
	Timer::Init();

	Console.Init(RXPAD_Pad1, TXPAD_Pad2);
	Console.SetBaudRate(CONSOLE_BAUD_RATE);
	Console.StreamInit(&Console_FILE);
	Console.Enable();

	Touch.Init(SPIMISOPAD_Pad3, SPIOUTPAD_Pad0_MOSI_Pad1_SCK, SPIMODE_0);
	Touch.SetBaudRateConst(2000000);
	Touch.Enable();

	printf("\nTouchscreen starting up version " STRINGIFY(VERSION) "\n");

	Lcd.Init();
	Lcd.WriteReg(AW_COLOR, AW_COLOR_CanvasColor16 | AW_COLOR_AddrModeXY);
	Lcd.DisplayOn();
	Lcd.WriteRegRgb(FGCR, 0xFF);
	Lcd.FillRect(0, 0, 1023, 599);
	Lcd.WriteRegRgb(FGCR, 0xFF00);
	Lcd.FillRect(100, 100, 923, 499);

	// Write some text
	Lcd.WriteRegRgb(FGCR, 0xFFFFFF);
	Lcd.WriteRegRgb(BGCR, 0);
	Lcd.WriteReg(CCR0, CCR0_CharSet8859_1 | CCR0_CharHeight32 | CCR0_CharSourceInternal);
	Lcd.WriteReg(CCR1, CCR1_CharHeightX4 | CCR1_CharWidthX4 | CCR1_CharBackgroundTransparent);
	Lcd.WriteRegXY(F_CURX0, 0, 0);
	Lcd.WriteString("0123456789");
	Lcd.WriteRegXY(F_CURX0, 0, 32 * 4);
	Lcd.WriteString("The quick brown fox jumped over the lazy dog.");

	//************************************************************************
	// Main loop

	Timer	tmr;
	int		iCurPos;
	int		iLastPos;
	int		iLastTouch;
	int		i;

	iLastPos = 0;
	iLastTouch = 0;
	tmr.Start();

    while (1) 
    {
		if (tmr.CheckInterval_rate(10))
		{
			iCurPos = Xpos.GetPos();
			if (iCurPos != iLastPos)
			{
				printf("Pos: %i\n", iCurPos);
				iLastPos = iCurPos;
			}

			iCurPos = Touch.ReadX();
			if (iCurPos != iLastTouch)
			{
				//printf("Touch: %i\n", iCurPos);
				iLastTouch = iCurPos;
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
			case '+':
			case '=':
				i = TCC1->CC[1].reg;
				i += LcdBacklightPwmMax / 10;
				if (i > LcdBacklightPwmMax)
					i = LcdBacklightPwmMax;
				TCC1->CC[1].reg = i;
				printf("up\n");
				break;

			case '-':
				i = TCC1->CC[1].reg;
				i -= LcdBacklightPwmMax / 10;
				if (i < 0)
					i = 0;
				TCC1->CC[1].reg = i;
				printf("down\n");
				break;
			}
		}
    }
}
