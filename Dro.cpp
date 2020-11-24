//****************************************************************************
// Dro.cpp
//
// Created 10/1/2020 12:36:41 PM by Tim
//
//****************************************************************************

#include <standard.h>
#include <Nvm/EepromMgr.h>
#include "Dro.h"
#include "PosSensor.h"
#include "LcdDef.h"
#include "RA8876.h"
#include "Xtp2046.h"
#include "KeypadHit.h"
#include "UsbDro.h"
#include "FatFileSys.h"

//*********************************************************************
// Functions in DroInit

void StartClock();
void Init();

//****************************************************************************
// Static data

Console_t	Console;
FILE		Console_FILE;

Xtp2046		Touch;
RA8876		Lcd;
KeypadHit	KeyHit;
UsbDro		UsbPort;

FatSd		Sd;
FatSysWait<true>	FileSys;
FAT_DRIVES_LIST(&Sd);

extern "C"
{
extern const byte TargetCursor[256];
}

//****************************************************************************
// EEPROM data

// The first two rows are reserved to store position at shutdown
static constexpr int ReservedEepromRows = 2;

// Define the structure
#define EepromData(typ, name, ...)	typ name;
struct Eeprom_t
{
#include "EepromData.h"
};

// Define initial EEPROM data
#define EepromData(typ, name, ...)	__VA_ARGS__,
const Eeprom_t RwwData =
{
#include "EepromData.h"
};

// Create an EepromMgr for it, which includes a copy in RAM
EepromMgr<Eeprom_t, &RwwData, ReservedEepromRows> Eeprom;

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

bool DownloadImage(ulong addr, int cbData)
{
	static byte arPageBuf[SerialFlashPageSize];
	Timer	tmr;
	int		i;

	// Flush out receiver buffer
	Console.DiscardReadBuf();
	// Allow 10 seconds for user to start download
	for (i = 0; i < 10; i++)
	{
		tmr.Start();
		while (!tmr.CheckDelay(1))
		{
			wdt_reset();
			if (Console.IsByteReady())
				goto Ready;
		}
	}
	return false;

Ready:
	// First 4 bytes of download are header, skip them
	// Not included in count cbData
	tmr.Start();
	for (i = 0; i < 4; i++)
	{
		while (!Console.IsByteReady())
		{
			if (tmr.CheckDelay_ms(2))
				return false;
		}
		// throw away header byte
	}

	while (cbData > 0)
	{
		wdt_reset();
		tmr.Start();
		for (i = 0; i < SerialFlashPageSize && cbData > 0; i++, cbData--)
		{
			while (!Console.IsByteReady())
			{
				if (tmr.CheckDelay_ms(50))
					return false;
			}
			arPageBuf[i] = Console.ReadByte();
		}

		Lcd.SerialMemWrite(addr, i, arPageBuf, 1);
		addr += i;
	}
	return true;
}

void NO_INLINE_ATTR DisplaySerialImage(ulong addr, int width, int height, int x, int y)
{
	Lcd.WriteReg32(DMA_SSTR0, addr);
	Lcd.WriteReg16(DMA_SWTH0, width);
	Lcd.WriteReg16(DMAW_WTH0, width);
	Lcd.WriteReg16(DMAW_HIGH0, height);
	Lcd.WriteReg16(DMA_DX0, x);
	Lcd.WriteReg16(DMA_DY0, y);
	Lcd.SerialSelectPort(SFL_CTRL_ModeDma, 1);
	Lcd.WriteReg(DMA_CTRL, DMA_CTRL_Start);
}


void CalibratePos(int X, int Y, int anchorX, int anchorY)
{
	int		readX, readY;

	Lcd.SetGraphicsCursorPosition(X - 16, Y - 16);

	while (!(Touch.Process() && Touch.GetTouch() & TOUCH_Start))
	{
		wdt_reset();
		if (Console.IsByteReady())
			return;
	}

	readX = Touch.GetX();
	readY = Touch.GetY();
	Touch.CalibrateX(readX, X, anchorX);
	Touch.CalibrateY(readY, Y, anchorY);
}

void NO_INLINE_ATTR CalibrateTouch()
{
	static constexpr int MinX = 32;
	static constexpr int MaxX = LcdWidthPx - 1 - 32;
	static constexpr int MinY = 32;
	static constexpr int MaxY = LcdHeightPx - 1 - 32;

	Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);
	CalibratePos(MinX, MinY, MaxX, MaxY);
	CalibratePos(MaxX, MaxY, MinX, MinY);
}

void LoadFileImage(uint hFile, uint addr)
{
	int		cbXfer;
	int		cb;
	ushort	*pus;

	Lcd.WriteReg16(CURH0, 0);
	Lcd.WriteReg16(CURV0, 0);

	FileSys.SeekWait(hFile, 4);	// skip 4-byte header
	cbXfer = FileSys.GetSize(hFile) - 5;	// don't count trailing byte either
	
	for (cb = 0; cbXfer > 0; cb -= 2, cbXfer -= 2)
	{
		if (cb == 0)
		{
			cb = FileSys.ReadWait(hFile, NULL, cbXfer);
			if (cb <= 0)
			{
				DEBUG_PRINT("File read returned %i\n", cb);
				break;
			}
			pus = (ushort *)FileSys.GetDataBuf();
		}
		Lcd.FifoWrite16(*pus++);
	}
}

//*********************************************************************
// Main program
//*********************************************************************

// Locations in serial flash
#define FONT_LOC		(0x1DD780)
#define IMAGE_LOC		(0x8000)
#define IMAGE_Height	250
#define IMAGE_Width		190
#define IMAGE_SIZE		(IMAGE_Width * IMAGE_Height * 2)

byte arbBuf[256];

int main(void)
{
	StartClock();
	Init();
	Timer::Init();
	Eeprom.Init();

	Console.Init(RXPAD_Pad1, TXPAD_Pad2);
	Console.SetBaudRate(CONSOLE_BAUD_RATE);
	Console.StreamInit(&Console_FILE);
	Console.Enable();

	Touch.Init(SPIMISOPAD_Pad3, SPIOUTPAD_Pad0_MOSI_Pad1_SCK, &Eeprom.Data.TouchScale);
	Touch.Enable();

	DEBUG_PRINT("\nTouchscreen starting up version " STRINGIFY(VERSION) "\n");
	if (PM->RCAUSE.reg & PM_RCAUSE_WDT)
	{
		DEBUG_PRINT("WDT Reset\n");
	}


	Lcd.Init();
	Lcd.LoadGraphicsCursor(TargetCursor, GTCCR_GraphicCursorSelect1);
	Lcd.SetGraphicsCursorColors(0xFF, 0x00);

	Lcd.WriteReg(MPWCTR, MPWCTR_MainImageColor16);
	Lcd.WriteReg(AW_COLOR, AW_COLOR_CanvasColor16 | AW_COLOR_AddrModeXY);
	Lcd.DisplayOn();
	TextDisplay();
	Lcd.WriteReg(CCR1, CCR1_CharHeightX3 | CCR1_CharWidthX3 | CCR1_CharBackgroundSet);

	// Initialize USB
	Mouse.Init(LcdWidthPx, LcdHeightPx);
	UsbPort.Init();
	UsbPort.Enable();

	// Initialize file system
	Sd.SpiInit(SPIMISOPAD_Pad1, SPIOUTPAD_Pad2_MOSI_Pad3_SCK);
	Sd.Enable();
	FileSys.Init();

	// Start WDT now that initialization is complete
	WDT->CTRL.reg = WDT_CTRL_ENABLE;

	//************************************************************************
	// Main loop

	Timer	tmr;
	int		iCurPos;
	int		iLastPos;
	int		i;
	bool	fShow = false;

	iLastPos = 0;
	tmr.Start();

    while (1)
    {
		wdt_reset();

		// Process EEPROM save if in progress
		Eeprom.Process();

		i = UsbPort.Process();
		if (i != HOSTACT_None)
		{
			int	X, Y;
			ButtonState buttons;

			switch (i)
			{
			case HOSTACT_MouseChange:
				X = Mouse.GetX();
				Y = Mouse.GetY();

				X = std::max(X - 16, 0);
				Y = std::max(Y - 16, 0);
				Lcd.SetGraphicsCursorPosition(X, Y);
				Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);

				buttons = Mouse.GetButtons();
				if (buttons.btnStart & BUTTON_Left)
					fShow = !fShow;
				if (fShow)
					Lcd.DisableGraphicsCursor();
				else
					Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);
				break;
			}
		}

		if (Touch.Process())
		{
			uint	flags;

			// Touch sensor has an update
			flags = Touch.GetTouch();
			if (flags & TOUCH_Touched)
			{
				int	X, Y, hit;

				X = Touch.GetX();
				Y = Touch.GetY();

				X = std::max(X - 16, 0);
				Y = std::max(Y - 16, 0);
				Lcd.SetGraphicsCursorPosition(X, Y);
				Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);

				if (flags & TOUCH_Start)
				{
					hit = KeyHit.TestHit(X, Y);
					if (hit >= 0)
					{
						if (hit < 10)
							Console.WriteByte(hit + '0');
						else if (hit == Key_Decimal)
							Console.WriteByte('.');
						else
							DEBUG_PRINT("\n");
					}
				}
			}
			else
			{
				if (!Mouse.IsLoaded())
					Lcd.DisableGraphicsCursor();
			}
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
			case 'c':
				DEBUG_PRINT("Calibrate touch screen...");
				CalibrateTouch();
				DEBUG_PRINT("complete.\n");
				break;

			case 'd':
				DEBUG_PRINT("Download\n");
				if (DownloadImage(IMAGE_LOC, IMAGE_SIZE))
					DEBUG_PRINT("Success\n");
				else
					DEBUG_PRINT("Failed\n");
				break;

			case 'e':
				DEBUG_PRINT("Erase\n");
				WDT->CTRL.reg = 0;	// disable watchdog during long process
				Lcd.SerialMemErase(IMAGE_LOC, IMAGE_SIZE, 1);
				WDT->CTRL.reg = WDT_CTRL_ENABLE;
				break;

			case 'f':
				DEBUG_PRINT("Show files\n");
				{
					uint	h;
					int		err;
					int		cnt;

					h = FileSys.StartEnum(0);
					cnt = 0;
					for (;;) 
					{
						err = FileSys.EnumNextWait(h, (char *)arbBuf, sizeof arbBuf);
						if (FileSys.IsError(err))
							break;
						DEBUG_PRINT("%-20s", arbBuf);
						if (!FileSys.IsFolder(err))
							DEBUG_PRINT(" %6li\n", FileSys.GetSize(err));
						else
							DEBUG_PRINT("\n");
						FileSys.CloseWait(err);
						cnt++;
					}
					FileSys.Close(h);
					DEBUG_PRINT("%i files\n", cnt);
				}
				break;

			case 'k':
				DEBUG_PRINT("Show keypad\n");
				DisplaySerialImage(IMAGE_LOC, IMAGE_Width, IMAGE_Height, 50, 200);
				KeyHit.Init(50, 200);
				break;

			case 'l':
				DEBUG_PRINT("Load image\n");
				{
					int	h;

					h = FileSys.OpenWait("MainScreen.bin", 0, OPENFLAG_OpenExisting | OPENFLAG_File);
					if (h <= 0)
					{
						DEBUG_PRINT("Error code %i opening file\n", h);
						break;
					}
					LoadFileImage(h, 0);
					FileSys.CloseWait(h);
				}
				break;

			case 'm':
				DEBUG_PRINT("Mount SD card\n");
				i = FileSys.MountWait(0);
				if (FileSys.IsError(i))
					DEBUG_PRINT("Failed with error code %i\n", i);
				else
					DEBUG_PRINT("Success\n");
				break;

			case 'p':
				DEBUG_PRINT("Program\n");
				for (i = 0; i < 6; i++)
				{
					Lcd.SerialMemRead(FONT_LOC + sizeof arbBuf * i, sizeof arbBuf, arbBuf, 0);
					Lcd.SerialMemWrite(FONT_LOC + sizeof arbBuf * i, sizeof arbBuf, arbBuf, 1);
				}
				break;

			case 'r':
				DEBUG_PRINT("Read\n");
				Lcd.SerialMemRead(IMAGE_LOC + 0x770, sizeof arbBuf, arbBuf, 1);
				HexDump(arbBuf, sizeof arbBuf);
				break;

			case 's':
				DEBUG_PRINT("Save EEPROM (y/n)?");
				while(!Console.IsByteReady())
					wdt_reset();
				ch = Console.ReadByte();
				if (ch == 'y' || ch == 'Y')
				{
					Eeprom.StartSave();
					DEBUG_PRINT("...Saved\n");
				}
				else
					DEBUG_PRINT("...Not saved\n");
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
