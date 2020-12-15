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
#include "HotspotList.h"
#include "UsbDro.h"
#include "FatFileSys.h"
#include "TextDisplay.h"
#include "TouchCanvas.h"


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
UsbDro		UsbPort;
TextDisplay	Text;

FatSd		Sd;
FatSysWait<true>	FileSys;
FAT_DRIVES_LIST(&Sd);

extern "C"
{
	extern const byte TargetCursor[256];
}

//****************************************************************************
// Static canvas data (RAM)

#define START_SCREEN(name)	TouchCanvas name(
#define IMAGE_ADDRESS(val)	val + RamScreenStart,
#define IMAGE_WIDTH(val)	val,
#define IMAGE_DEPTH(val)	val,
#define END_SCREEN(name)	&name##HotspotList);

#include "Images/Screen.h"

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

void DumpCanvas(uint addr)
{
	DEBUG_PRINT("Image addr: %lu, width: %u\n",  Lcd.ReadReg32(addr), Lcd.ReadReg16(addr + 4));
	DEBUG_PRINT("Window: (%u, %u) width: %u, height: %u\n",
		Lcd.ReadReg16(addr + 6), Lcd.ReadReg16(addr + 8),
		Lcd.ReadReg16(addr + 10), Lcd.ReadReg16(addr + 12));
}

void NO_INLINE_ATTR TextDisplay()
{
	Lcd.SetForeColor(0xFF);
	Lcd.FillRect(0, 0, 1023, 599);
	Lcd.SetForeColor(0xFF00);
	Lcd.FillRect(100, 100, 923, 499);

	// Write some text
	Lcd.SetForeColor(0xFFFFFF);
	Lcd.SetBackColor(0);
	Lcd.WriteReg(CCR1, CCR1_CharHeightX3 | CCR1_CharWidthX3 | CCR1_CharBackgroundTransparent);
	Lcd.WriteRegXY(F_CURX0, 0, 0);
	Lcd.ExternalFont(CCR0_CharHeight16, GTENT_CR_CharWidthFixed | GTFNT_CR_Ascii, 1);
	Lcd.WriteString("0123456789");
	Lcd.WriteRegXY(F_CURX0, 0, 64);
	Lcd.WriteReg(FLDR, 0);
	//Lcd.InternalFont(CCR0_CharHeight32, CCR0_CharSet8859_1);
	Lcd.WriteString("The quick brown fox jumped over the lazy dog.");
}

void NO_INLINE_ATTR CalibratePos(int X, int Y, int anchorX, int anchorY)
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

class SetAndRestore
{
public:
	SetAndRestore(uint addr, uint value)
	{
		Addr = addr;
		Value = RA8876::ReadReg(addr);
		RA8876::WriteData(value);
	}
	~SetAndRestore()
	{
		RA8876::WriteReg(Addr, Value);
	}
	uint	Value;
	uint	Addr;
};

void NO_INLINE_ATTR LoadFileToRam(uint hFile, uint addr)
{
	int		cb;
	ushort	*pus;

	SetAndRestore x(AW_COLOR, AW_COLOR_DataWidth16 | AW_COLOR_AddrModeLinear);
	Lcd.WriteReg32(CURH0, addr);

	for (;;)
	{
		cb = FileSys.ReadWait(hFile, NULL, 0x200);
		if (cb == 0)
			break;

		if (cb < 0)
		{
			DEBUG_PRINT("File read returned %i\n", cb);
			break;
		}

		pus = (ushort *)FileSys.GetDataBuf();

		while (cb > 0)
		{
			Lcd.FifoWrite16(*pus++);
			cb -= 2;
		}
	}
}

void NO_INLINE_ATTR WriteFileToFlash(uint hFile, uint addr)
{
	int		cb;

	cb = FileSys.GetSize(hFile);
	// Round up to full block size for erasure
	cb = (cb + SerialFlashBlockSize - 1) & ~(SerialFlashBlockSize - 1);

	WDT->CTRL.reg = 0;	// disable watchdog during long process
	Lcd.SerialMemErase(addr, cb, 1);
	WDT->CTRL.reg = WDT_CTRL_ENABLE;

	for (;;)
	{
		cb = FileSys.ReadWait(hFile, NULL, 0x200);
		if (cb == 0)
			break;

		if (cb < 0)
		{
			DEBUG_PRINT("File read returned %i\n", cb);
			break;
		}

		Lcd.SerialMemWrite(addr, cb, FileSys.GetDataBuf(), 1);
		addr += cb;
	}
}

//*********************************************************************
// Main program
//*********************************************************************

byte arbBuf[256];

int main(void)
{
	StartClock();
	Init();
	Timer::Init();
	Eeprom.Init();
	TCC1->CC[1].reg = Eeprom.Data.Brightness;

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

	// Copy serial data in graphics memory
	Lcd.CopySerialMemToRam(FlashScreenStart, RamScreenStart, ScreenFileLength, 1);
	Lcd.CopySerialMemToRam(FlashFontStart, RamFontStart, FontFileLength, 1);

	Lcd.LoadGraphicsCursor(TargetCursor, GTCCR_GraphicCursorSelect1);
	Lcd.SetGraphicsCursorColors(0xFF, 0x00);

	Lcd.WriteReg(AW_COLOR, AW_COLOR_CanvasColor16 | AW_COLOR_AddrModeXY);
	Lcd.WriteReg(MPWCTR, MPWCTR_MainImageColor16);
	MainScreen.SetCanvasView(MISA0);
	Lcd.DisplayOn();

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
				int	X, Y;

				X = Touch.GetX();
				Y = Touch.GetY();

				X = std::max(X - 16, 0);
				Y = std::max(Y - 16, 0);
				Lcd.SetGraphicsCursorPosition(X, Y);
				Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);

				if (flags & TOUCH_Start)
				{
					if (MainScreen.TestHit(X, Y) != NULL)
					{
						DEBUG_PRINT("Key hit\n");
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
				DEBUG_PRINT("File directory\n");
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

			case 'f':
				DEBUG_PRINT("Load font\n");
				{
					int	h;

					h = FileSys.OpenWait("Fonts.bin", 0, OPENFLAG_OpenExisting | OPENFLAG_File);
					if (h <= 0)
					{
						DEBUG_PRINT("Error code %i opening file\n", h);
						break;
					}
					WriteFileToFlash(h, FlashFontStart);
					FileSys.CloseWait(h);
				}
				break;

			case 'l':
				DEBUG_PRINT("Load image\n");
				{
					int	h;

					h = FileSys.OpenWait("Screen.bin", 0, OPENFLAG_OpenExisting | OPENFLAG_File);
					if (h <= 0)
					{
						DEBUG_PRINT("Error code %i opening file\n", h);
						break;
					}
					WriteFileToFlash(h, FlashScreenStart);
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
				i = Eeprom.Data.Brightness;
				i += LcdBacklightPwmMax / 10;
				if (i > LcdBacklightPwmMax)
					i = LcdBacklightPwmMax;
				Eeprom.Data.Brightness = i;
				TCC1->CC[1].reg = i;
				DEBUG_PRINT("up\n");
				break;

			case '-':
				i = Eeprom.Data.Brightness;
				i -= LcdBacklightPwmMax / 10;
				if (i < 0)
					i = 0;
				Eeprom.Data.Brightness = i;
				TCC1->CC[1].reg = i;
				DEBUG_PRINT("down\n");
				break;
			}
		}
    }
}
