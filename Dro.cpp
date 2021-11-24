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
#include "VersionUpdate.h"
#include "PowerDown.h"


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

VersionInfo_t VersionInfo VERSION_INFO = { DroFirmwareId, PROGRAM_VERSION, GRAPHICS_VERSION, FONT_VERSION };

Console_t	Console;
FILE		Console_FILE;
FDEV_STANDARD_STREAMS(&Console_FILE, NULL);

Xtp2046		Touch;
UsbDro		UsbPort;
FileBrowser	Files;
ToolLib		Tools;

FatSd			Sd;
FatSys			FileSys;
FileOperations	FileOp;
FAT_DRIVES_LIST(&FlashDrive, &Sd);

//********************************************************************
// Define the four sensors

static const int AxisUpdateRate = 20;	// updates per second
static const int FeedUpdateRate = 8;	// updates per second

PosSensor Qpos(&Eeprom.Data.QaxisInfo);
AxisPos Xpos(&Eeprom.Data.XaxisInfo);
AxisPos Ypos(&Eeprom.Data.YaxisInfo);
AxisPos Zpos(&Eeprom.Data.ZaxisInfo, &Qpos);

AxisDisplay Xdisplay(&Xpos, MainScreen_Areas.Xdisplay, MainScreen_Areas.UndoX1);
AxisDisplay Ydisplay(&Ypos, MainScreen_Areas.Ydisplay, MainScreen_Areas.UndoY1);
AxisDisplay Zdisplay(&Zpos, MainScreen_Areas.Zdisplay, MainScreen_Areas.UndoZ1);

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

void NO_INLINE_ATTR HardFault(int *p)
{
	*p = 0;
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

FatDateTime GetFatTime()
{
	FatDateTime	dt;
	RtcTime		time;

	dt.ul = time.ReadClock().GetFatTime();
	return dt;
}

void PrintHelp()
{
	printf("Commands:\n"
		"f - Load fonts from USB file Fonts.bin\n"
		"i - Load images from USB file Screen.bin\n"
		"t - Calibrate touchscreen\n"
		"x - Reset\n"
	);
}

//*********************************************************************
// Main program
//*********************************************************************

int main(void)
{
	RtcTime	timeCur, timeSave;
	bool	lcdPresent;

	StartClock();
	Init();
	Timer::Init();
	Eeprom.Init();
	RtcTime::Init();

	timeCur.ReadClock();
	timeSave = PowerDown::Restore();
	if (!timeCur.IsSet())
		timeSave.SetClock();

	Console.Init(RXPAD_Pad1, TXPAD_Pad2);
	Console.SetBaudRate(CONSOLE_BAUD_RATE);
	Console.StreamInit(&Console_FILE);
	Console.Enable();

	printf("\nDRO version " STRINGIFY(PROGRAM_VERSION) "\n");
	if (PM->RCAUSE.reg & PM_RCAUSE_WDT)
	{
		DEBUG_PRINT("WDT Reset\n");
	}

	// Put EEPROM data into effect
	TCC1->CC[1].reg = Eeprom.Data.Brightness;
	Xpos.SensorInfoUpdate();
	Ypos.SensorInfoUpdate();
	Zpos.SensorInfoUpdate();
	Qpos.SensorInfoUpdate();

	// Initialize USB
	Mouse.Init(Lcd.ScreenWidth, Lcd.ScreenHeight);
	UsbPort.Init();
	UsbPort.Enable();

	// Initialize file system
	Sd.SpiInit(SPIMISOPAD_Pad1, SPIOUTPAD_Pad2_MOSI_Pad3_SCK);
	Sd.Enable();
	FileSys.Init();

	// Initialize LCD and touch if present
	lcdPresent = Lcd.Init();
	if (lcdPresent)
	{
		// Copy serial data in graphics memory
		Lcd.CopySerialMemToRam(FlashScreenStart, RamScreenStart, ScreenFileLength, 1);
		Lcd.CopySerialMemToRam(FlashFontStart, RamFontStart, FontFileLength, 1);

		Lcd.LoadGraphicsCursor(PointerCursor, GTCCR_GraphicCursorSelect1);
		Lcd.LoadGraphicsCursor(TargetCursor, GTCCR_GraphicCursorSelect2);
		Lcd.SetGraphicsCursorColors(0xFF, 0x00);

		Lcd.SetMainImage(&MainScreen);
		Lcd.DisplayOn();

		Touch.Init(SPIMISOPAD_Pad3, SPIOUTPAD_Pad0_MOSI_Pad1_SCK, &Eeprom.Data.TouchInit, Lcd.ScreenWidth, Lcd.ScreenHeight);
		Touch.Enable();

		Actions::Init();

		DEBUG_PRINT("Graphics memory allocated: %lu bytes\n", Canvas::AllocVideoRam(0));

		PrintHelp();
	}

	// Start WDT now that initialization is complete
	WDT->CTRL.reg = WDT_CTRL_ENABLE;

	// Finally, enable NMI
	EIC->NMICTRL.reg = EIC_NMICTRL_NMISENSE_FALL | EIC_NMICTRL_NMIFILTEN;

	//************************************************************************
	// Main loop

	Timer	tmrAxis;
	Timer	tmrFeed;
	int		i;
	bool	fSdOut = true;
	RtcTime	timeLast{true};

	tmrFeed.Start(tmrAxis.Start());

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

		if (timeCur.ReadClock() != timeLast)
		{
			timeLast = timeCur;
			Tools.ShowExportTime(timeCur);
		}

		// Update the axis position displays
		if (tmrAxis.CheckInterval_rate(AxisUpdateRate))
			AxisDisplay::UpdateAll();

		// Update the current feed rate
		if (tmrFeed.CheckInterval_rate(FeedUpdateRate))
		{
			double	deltaX, deltaY, delta;

			deltaX = Xpos.GetDistance();
			deltaY = Ypos.GetDistance();
			delta = sqrt(deltaX * deltaX + deltaY * deltaY);
			// convert to per minute
			Tools.ShowFeedRate(delta * 60.0 * FeedUpdateRate);
		}

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

			case HOSTACT_KeyboardChange:
				KeyboardMgr::UsbKeyHit(Keyboard.GetKeyByte());
				break;

			case HOSTACT_AddDevice:
				if (Mouse.IsLoaded())
				{
					Lcd.EnableGraphicsCursor(GTCCR_GraphicCursorSelect1);
					Mouse.SetPos(Lcd.ScreenWidth / 2, Lcd.ScreenHeight / 2);
					Lcd.SetGraphicsCursorPosition(Lcd.ScreenWidth / 2, Lcd.ScreenHeight / 2);
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
		if (lcdPresent && Touch.Process())
		{
			uint	flags;

			// Touch sensor has an update
			flags = Touch.GetTouch();
			if (flags != TOUCH_None)
			{
				int	x, y;

				x = Touch.GetX();
				y = Touch.GetY();

				Actions::TakeAction(x, y, flags);
			}
		}

		if (Console.IsByteReady())
		{
			byte	ch;
			int		err;

			ch = Console.ReadByte();
			if (ch >= 'A' && ch <= 'Z')
				ch += 'a' - 'A';

			switch (ch)
			{
				// Use lower-case alphabetical order to find the right letter
			case 'f':
				printf("Loading font...");
				err = FileOp.WriteFileToFlash("Fonts.bin", FlashFontStart);
FileErrChk:
				if (err < 0)
					printf("file operation failed with code %i\n", err);
				break;

			case 'i':
				printf("Loading image...");
				err = FileOp.WriteFileToFlash("Screen.bin", FlashScreenStart);
				goto FileErrChk;

			case 't':
				if (lcdPresent)
					TouchCalibrate::Open();
				break;

			case 'x':
				HardFault((int *)&g_FileBuf[0][1]);
				break;

			default:
				if (lcdPresent)
					PrintHelp();
				break;
			}
		}
    }
}
