//****************************************************************************
// UsbPort.h
//
// Created 11/8/2020 10:26:59 AM by Tim
//
//****************************************************************************

#pragma once

// Must define the number of drivers first
#define HOST_DRIVER_COUNT	3

#include <Usb/Host/USBhost.h>
#include <Usb/Host/Drivers/KeyboardHost.h>
#include <Usb/Host/Drivers/MouseHost.h>
#include <Usb/Host/Drivers/FlashDriveHost.h>


//****************************************************************************
// List of USB drivers

extern KeyboardHost		Keyboard;
extern MouseHost		Mouse;
extern FlashDriveHost	FlashDrive;

//****************************************************************************

class UsbDro : public USBhost
{
public:
	int Process()
	{
		int		act;

		act = USBhost::Process();
		if (act != HOSTACT_None)
		{
			switch (act)
			{
			case HOSTACT_AddDevice:
				DEBUG_PRINT("Device attached\n");
				break;

			case HOSTACT_RemoveDevice:
				DEBUG_PRINT("Device removed\n");
				break;

			case HOSTACT_AddFailed:
				DEBUG_PRINT("Device driver not found\n");
				break;
			}
		}
		return act;
	}
};
