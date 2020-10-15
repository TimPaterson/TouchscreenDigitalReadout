//****************************************************************************
// Dro.cpp
//
// Created 10/1/2020 12:36:41 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "Dro.h"
#include "PosSensor.h"
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

//*********************************************************************
// Main program
//*********************************************************************

int main(void)
{
	StartClock();
	Init();

	Touch.Init(SPIMISOPAD_Pad3, SPIOUTPAD_Pad0_MOSI_Pad1_SCK, SPIMODE_0);
	Touch.SetBaudRateConst(2000000);
	Touch.Enable();

	Console.Init(RXPAD_Pad1, TXPAD_Pad2);
	Console.SetBaudRate(CONSOLE_BAUD_RATE);
	Console.StreamInit(&Console_FILE);
	Console.Enable();

	printf("\nTouchscreen starting up version " STRINGIFY(VERSION) "\n");

	//************************************************************************
	// Main loop

	int		iCurPos;
	int		iLastPos;
	int		iLastTouch;

	iLastPos = 0;
	iLastTouch = 0;

    while (1) 
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
			printf("Touch: %i\n", iCurPos);
			iLastTouch = iCurPos;
		}
		if (Console.IsByteReady())
		{
			Console.WriteByte(Console.ReadByte());
		}
    }
}
