//****************************************************************************
// FlashDriveDef.h
//
// Created 8/28/2020 12:31:37 PM by Tim
//
//****************************************************************************

#pragma once


//****************************************************************************
// If there is a custom descriptor larger than those kept by USBdevice,
// set its size here.

//#define MAX_USB_DESCRIPTOR_SIZE	0

//****************************************************************************
// USB Device Descriptor

#define USB_DEV_UsbVer		USB_VER_2p0
#define	USB_DEV_Class		USBCLASS_None
#define USB_DEV_Subclass	0
#define	USB_DEV_Protocol	0
#define USB_DEV_SerialNo	// defining it means GetSerialStrDesc() implemented

//****************************************************************************
// USB Interface Descriptor

#define	USB_IFC_Class		USBCLASS_MassStorage
#define	USB_IFC_Subclass	MSIFSC_SCSI
#define USB_IFC_Protocol	MSIFP_BULK

//****************************************************************************
// Endpoint description
// #define ENDPOINT_IN (endpointNum, transferType, packetSize, interval)
// #define ENDPOINT_OUT(endpointNum, transferType, packetSize, interval)

// Endpoint 0
#define USB_ENDPOINT0_SIZE	64	

#define IN_PACKET_SIZE		64
#define OUT_PACKET_SIZE		64

#define IN_ENDPOINT_NUM		1
#define OUT_ENDPOINT_NUM	1

// Define and endpoints inside one macro
#define ENDPOINT_LIST \
ENDPOINT_IN (IN_ENDPOINT_NUM, Bulk, IN_PACKET_SIZE, 0) \
ENDPOINT_OUT(OUT_ENDPOINT_NUM, Bulk, OUT_PACKET_SIZE, 0)
