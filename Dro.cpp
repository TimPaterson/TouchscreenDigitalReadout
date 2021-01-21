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
#include "FatFileSd.h"
#include "UsbDro.h"
#include "AxisDisplay.h"
#include "Actions.h"
#include "FileOperations.h"


//*********************************************************************
// References

// DroInit.cpp
void StartClock();
void Init();

// Generated bitmap files
extern "C"
{
	extern const byte TargetCursor[256];
	extern const byte PointerCursor[256];
}

//*********************************************************************
// Static data
//*********************************************************************

//*********************************************************************
// Image descriptors

// "Canvas" images
#define START_SCREEN(name)		TouchCanvas name(
#define IMAGE_ADDRESS(val)		val + RamScreenStart,
#define IMAGE_WIDTH(val)		val,
#define IMAGE_HEIGHT(val)		val,
#define IMAGE_STRIDE(val)		val,
#define IMAGE_DEPTH(val)		val,
#define END_SCREEN(name)		&name##HotspotList);

// "ColorImage" images
#define START_SCREEN_Overlay(name)	const ColorImage name = {
#define IMAGE_ADDRESS_Overlay(val)	val + RamScreenStart,
#define IMAGE_STRIDE_Overlay(val)	val,
#define IMAGE_DEPTH_Overlay(val)	val
#define END_SCREEN_Overlay(name)	};

#include "Images/Screen.h"

// Areas
#define START_AREAS(name)					const name##_Areas_t name##_Areas = {
#define DEFINE_AREA(name, x1, y1, x2, y2)	{x1, y1, x2, y2},
#define END_AREAS(name)						};

#include "Images/Screen.h"

//*********************************************************************
// Component data

Console_t	Console;
FILE		Console_FILE;

Xtp2046		Touch;
ScreenMgr	Lcd;
UsbDro		UsbPort;

FatSd				Sd;
FatSysWait<wdt_reset> FileSys;
FileOperations		FileOp;
FAT_DRIVES_LIST(&FlashDrive, &Sd);
//FAT_DRIVES_LIST(&Sd, &FlashDrive);

//********************************************************************
// Define the four sensors

static const int AxisUpdateRate = 20;	// updates per second

AxisDisplay Xaxis(&Eeprom.Data.XaxisInfo, &MainScreen_Areas.Xdisplay);
AxisDisplay Yaxis(&Eeprom.Data.YaxisInfo, &MainScreen_Areas.Ydisplay);
AxisDisplay Zaxis(&Eeprom.Data.ZaxisInfo, &MainScreen_Areas.Zdisplay);
PosSensor Qpos(&Eeprom.Data.QaxisInfo);

//********************************************************************
// EEPROM data

// Define initial EEPROM data
#define EepromData(typ, name, ...)	__VA_ARGS__,

const Eeprom_t RwwData =
{
	#include "EepromData.h"
};

// Create an EepromMgr for it, which includes a copy in RAM
EepromMgr_t Eeprom;

//*********************************************************************
// Tests
//*********************************************************************

byte arbBuf[256];

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

void NO_INLINE_ATTR DumpRam(ulong addr, int  cb)
{
	ushort	*pus;

	if (cb > (int)sizeof arbBuf)
		cb = sizeof arbBuf;

	SetAndRestore x(AW_COLOR, AW_COLOR_DataWidth16 | AW_COLOR_AddrModeLinear);
	Lcd.WriteReg32(CURH0, addr);
	Lcd.ReadReg(MRWDP);	// dummy read
	pus = (ushort *)&arbBuf[0];
	for (int i = 0; i < cb / 2; i++)
		*pus++ = Lcd.FastFifoRead16();
	HexDump(arbBuf, cb);
}

void NO_INLINE_ATTR DumpRam(const Area *pArea, int  cb)
{
	ushort	*pus;

	if (cb > (int)sizeof arbBuf)
		cb = sizeof arbBuf;

	Lcd.WriteSequentialRegisters(&MainScreen, CVSSA0, ImageRegCount);
	Lcd.WriteSequentialRegisters(pArea, AWUL_X0, sizeof(Area));
	Lcd.WriteReg32(CURH0, 0);
	Lcd.ReadReg(MRWDP);	// dummy read
	pus = (ushort *)&arbBuf[0];
	for (int i = 0; i < cb / 2; i++)
		*pus++ = Lcd.FastFifoRead16();
	HexDump(arbBuf, cb);
}

void NO_INLINE_ATTR EnableCursor()
{
	uint height = 112;
	Lcd.WriteReg(CURHS, 1);
	Lcd.WriteReg(CURVS, height / 4 - 1);
	Lcd.WriteReg(BTCR, 35);
	Lcd.WriteReg(CCR0, CCR0_CharHeight32);
	Lcd.WriteReg(CCR1, CCR1_CharHeightX4);
	Lcd.WriteReg(ICR, ICR_TextMode);
	Lcd.WriteReg(GTCCR, GTCCR_TextCursorEnable | GTCCR_TextCursorBlink);
	//Lcd.WriteRegXY(F_CURX0, Zaxis_X + 54 * 3, Zaxis_Y - (32 * 4 - height));
}

//*********************************************************************
// Helpers
//*********************************************************************

void ChangeScreenBrightness(int change)
{
	change = LcdBacklightPwmMax * change / 100;	// change was %
	change += Eeprom.Data.Brightness;
	if (change < 0)
		change = 0;
	if (change > LcdBacklightPwmMax)
		change = LcdBacklightPwmMax;
	Eeprom.Data.Brightness = change;
	TCC1->CC[1].reg = change;
}

//*********************************************************************
// Main program
//*********************************************************************

int main(void)
{
	StartClock();
	Init();
	Timer::Init();
	Eeprom.Init();

	// Put EEPROM data into effect
	TCC1->CC[1].reg = Eeprom.Data.Brightness;
	Xaxis.AxisInfoUpdate();
	Yaxis.AxisInfoUpdate();
	Zaxis.AxisInfoUpdate();
	Qpos.AxisInfoUpdate();

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

	Lcd.LoadGraphicsCursor(PointerCursor, GTCCR_GraphicCursorSelect1);
	Lcd.LoadGraphicsCursor(TargetCursor, GTCCR_GraphicCursorSelect2);
	Lcd.SetGraphicsCursorColors(0xFF, 0x00);

	Lcd.SetMainImage(&MainScreen);
	Lcd.DisplayOn();

	Actions::ShowAbsInc();
	Actions::ShowInchMetric();
	Actions::ShowToolInfo();

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

	Timer	tmrAxis;
	int		i;
	bool	fSdOut = true;

	tmrAxis.Start();

    while (1)
    {
		wdt_reset();

		// Check status of SD card
		if (!GetSdCd() == fSdOut && !FileOp.IsBusy())
		{
			fSdOut = !fSdOut;
			if (fSdOut)
			{
				Sd.Dismount();
				DEBUG_PRINT("SD card dismounted\n");
			}
			else
			{
				FileOp.Mount(Sd.GetDrive());
				DEBUG_PRINT("SD card mounting...");
			}
		}

		// Process EEPROM save if in progress
		Eeprom.Process();

		// Update the axis position displays
		if (tmrAxis.CheckInterval_rate(AxisUpdateRate))
			AxisDisplay::UpdateAll();

		// Process USB events
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
				Lcd.SetGraphicsCursorPosition(X, Y);

				buttons = Mouse.GetButtons();
				if (buttons.btnStart & BUTTON_Left)
					Actions::TakeAction(X, Y);
				break;

			case HOSTACT_FlashReady:
				FileOp.Mount(FlashDrive.GetDrive());
				DEBUG_PRINT("USB drive mounting...");
				break;

			case HOSTACT_AddDevice:
				if (Mouse.IsLoaded())
				{
					Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);
					Mouse.SetPos(LcdWidthPx / 2, LcdHeightPx / 2);
					Lcd.SetGraphicsCursorPosition(LcdWidthPx / 2, LcdHeightPx / 2);
				}
				break;

			case HOSTACT_RemoveDevice:
				if (FlashDrive.IsMounted())
				{
					FlashDrive.Dismount();
					DEBUG_PRINT("USB drive dismounted\n");
				}

				// Turn mouse off
				Lcd.DisableGraphicsCursor();				
				break;
			}
		}

		// Process file operations
		FileOp.Process();

		// Process screen touch
		if (Touch.Process())
		{
			uint	flags;

			// Touch sensor has an update
			flags = Touch.GetTouch();
			if (flags & TOUCH_Start)
			{
				int	X, Y;

				X = Touch.GetX();
				Y = Touch.GetY();
				Actions::TakeAction(X, Y);
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
				FileOp.FileDirectory((char *)arbBuf, sizeof arbBuf);
				break;

			case 'f':
				DEBUG_PRINT("Loading font...");
				FileOp.WriteFileToFlash("Fonts.bin", FlashFontStart);
				break;

			case 'i':
				DEBUG_PRINT("Loading image...");
				FileOp.WriteFileToFlash("Screen.bin", FlashScreenStart);
				break;

			case 'r':
				DumpRam(&MainScreen_Areas.Mem1, 16);
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
			}
		}
    }
}
