//****************************************************************************
// FT232Def.h
//
// Created 8/18/2020 10:40:12 AM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// If there is a custom descriptor larger than those kept by USBdevice,
// set its size here.

#define MAX_USB_DESCRIPTOR_SIZE	0

//****************************************************************************
// USB Device Descriptor

#define USB_DEV_UsbVer		USB_VER_2p0
#define	USB_DEV_Class		USBCLASS_None
#define USB_DEV_Subclass	0
#define	USB_DEV_Protocol	0
#define USB_DEV_Vendor		0x0403
#define USB_DEV_Product		0x6001
#define USB_DEV_Device		0x0600
#define USB_DEV_VendorStr	"FTDI"
#define USB_DEV_ProductStr	"FT232R USB UART"
#define USB_DEV_SerialNo	// defining it means GetSerialStrDesc() implemented
#define USB_DEV_NumConfigs	1

//****************************************************************************
// USB Configuration Descriptor

// Set up power configuration for USB descriptor
// Choose bus-powered or self-powered, then power consumption
//#define USB_CFG_Attr		USBCFG_PowerSelf
#define USB_CFG_Attr		USBCFG_PowerBus
#define USB_CFG_MaxPower	(50/2)	// 2 mA units

//****************************************************************************
// USB Interface Descriptor

#define	USB_IFC_Class		0xFF	// Vendor defined
#define	USB_IFC_Subclass	0xFF
#define USB_IFC_Protocol	0xFF

//****************************************************************************
// Endpoint description

// Endpoint 0
#define USB_ENDPOINT0_SIZE	8

#define FTDI_IN_BUF_SIZE	64
#define FTDI_OUT_BUF_SIZE	64

#define ENDPOINT_LIST \
ENDPOINT_IN(1, Bulk, FTDI_IN_BUF_SIZE, 0) \
ENDPOINT_OUT(2, Bulk, FTDI_OUT_BUF_SIZE, 0)

//****************************************************************************
// Enable SOF interrupt
#define	USB_SOF_INT
