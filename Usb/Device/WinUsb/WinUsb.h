//****************************************************************************
// WinUsb.h
//
// Created 8/28/2020 12:26:49 PM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// Define biggest descriptor to reserve a large enough buffer

#include "MsftUsb.h"

struct MsftExtendedProps
{
	MsftExtendedPropOsDescHeader	header;
	// first prop is device GUID
	WinUsbProp		WinUsbGuid;
	// second prop is label
	DeviceNameProp	DeviceName;
};

#define MAX_USB_DESCRIPTOR_SIZE	(sizeof(MsftExtendedProps))

#include <Usb/Device/USBdevice.h>

template<class T>
class WinUsb : public USBdeviceBase
{
	//*********************************************************************
	// Local Types
	//*********************************************************************

protected:
	// Extended Compat ID Descriptor
	struct MsftWinUsbCompatIdDesc
	{
		MsftExtendedCompatIdOsDescHeader	header;
		MsftExtendedCompatIdOsDescFunction	function;
	};

	//*********************************************************************
	// Implementation of callbacks from USBdevice class
	//*********************************************************************

public:
	static const void *NonStandardString(int index)
	{
		if (index == MSFT_OS_STR_DESC)
			return &Ms10StrDesc;
		return T::NonStandardString(index);
	}

	static bool NonStandardSetup(UsbSetupPacket *pSetup)
	{
		int		cbReq;
		int		cbAvail;
		const void *pv;

		if (pSetup->bRequest == MS_VENDOR_CODE)
		{
			switch (pSetup->wIndex)
			{
				case MS_EXTENDED_COMPAT_ID_DESC:
					pv = (BYTE *)&WinUsbDesc;
					break;

				case MS_EXTENDED_PROP_DESC:
					pv = (BYTE *)&ExtendedProps;
					break;

				default:
					goto UnknownSetup;
			}

			// Send the descriptor, limited by request length
			cbAvail = ((UsbStringDescHead *)pv)->bLength;
			cbReq = pSetup->wLength;
			if (cbAvail > cbReq)
				cbAvail = cbReq;
			memcpy(GetSetupBuf(), pv, cbAvail);
			SendControlPacket(cbAvail);
			return true;
		}

	UnknownSetup:
		return T::NonStandardSetup(pSetup);
	}

	//*********************************************************************
	// These callbacks are not handled here, pass them on to implementation
	// in class T, the template argument

	static void DeviceConfigured()
	{
		T::DeviceConfigured();
	}

	static void RxData(int iEp, void *pv, int cb)
	{
		T::RxData(iEp, pv, cb);
	}

	static void TxDataRequest(int iEp)
	{
		T::TxDataRequest(iEp);
	}

	static void TxDataSent(int iEp, int cb)
	{
		T::TxDataSent(iEp, cb);
	}

	static void StartOfFrame()
	{
		T::StartOfFrame();
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

protected:
	inline static const MsftOs10StringDesc Ms10StrDesc =
	{
		sizeof(MsftOs10StringDesc),
		USBDESC_String,
		{ MSFT100 },
		MS_VENDOR_CODE
	};

	inline static const MsftWinUsbCompatIdDesc WinUsbDesc =
	{
		// header
		{
			sizeof(MsftWinUsbCompatIdDesc),
			MS_EXTENDED_BCD_VER,
			MS_EXTENDED_COMPAT_ID_DESC,
			1,	// bCount
		},
		// function
		{
			0,	// bFirstInterfaceNumber
			1,	// reserved
			WINUSB_ID
		}
	};

	inline static const MsftExtendedProps ExtendedProps =
	{
		// header
		{
			sizeof(MsftExtendedProps),
			MS_EXTENDED_BCD_VER,
			MS_EXTENDED_PROP_DESC,
			2,	// wCount of properties
		},
		// WinUSB GUID property
		{
			// header
			{
				sizeof(WinUsbProp),
				EXTENDED_PROP_Unicode,
				sizeof WINUSB_GUID_NAME
			},
			{ WINUSB_GUID_NAME },
			sizeof STRING16(WINUSB_GUID),
			{ STRING16(WINUSB_GUID) }
		},
		// Device Name property
		{
			// header
			{
				sizeof(DeviceNameProp),
				EXTENDED_PROP_Unicode,
				sizeof DEVICE_LABEL_NAME
			},
			{ DEVICE_LABEL_NAME },
			sizeof STRING16(USB_DEVICE_LABEL),
			{ STRING16(USB_DEVICE_LABEL) }
		}
	};

	//*********************************************************************
	// The interrupt service routine - class with no data
	//*********************************************************************

public:
	static USBdeviceIsr<WinUsb<T>>	ISR;
};
