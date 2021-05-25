#pragma once

//****************************************************************************
// USB Version definition
//****************************************************************************

#define USB_VER_1p1		0x0110
#define USB_VER_2p0		0x0200

//****************************************************************************
// Standard control transfers
//****************************************************************************

enum UsbStdReq
{
	USBREQ_Get_Status,
	USBREQ_Clear_Feature,
	USBREQ_Set_Feature = 3,
	USBREQ_Set_Address = 5,
	USBREQ_Get_Descriptor,
	USBREQ_Set_Descriptor,
	USBREQ_Get_Configuration,
	USBREQ_Set_Configuration,
	USBREQ_Get_Interface,
	USBREQ_Set_Interface,
	USBREQ_Synch_Frame
};

struct UsbSetupPacket
{
	union
	{
		byte	bmRequestType;
		struct  
		{
			byte	Recipient:5;
			byte	Type:2;
			byte	Dir:1;
		};
	};
	byte	bRequest;
	union
	{
		ushort	wValue;
		byte	bValue;
		struct  
		{
			byte	bDescIndex;
			byte	bDescType;
		};
	};
	ushort	wIndex;
	ushort	wLength;
};

// Bits in bmRequestType
#define	USBRT_Dir_Pos			7		// bit position
#define	USBRT_Dir_Msk			(1 << USBRT_Dir_Pos)
#define USBRT_DirOut_Val		0
#define USBRT_DirIn_Val			1
#define USBRT_DirOut			(USBRT_DirOut_Val << USBRT_Dir_Pos)
#define USBRT_DirIn				(USBRT_DirIn_Val << USBRT_Dir_Pos)

#define USBRT_Type_Pos			5		// bit position
#define USBRT_Type_Msk			(3 << USBRT_Type_Pos)
#define USBRT_Type(value)		(USBRT_Type_Msk & ((value) << USBRT_Type_Pos))
#define USBRT_TypeStd_Val		0
#define USBRT_TypeClass_Val		1
#define USBRT_TypeVendor_Val	2
#define USBRT_TypeStd			USBRT_Type(USBRT_TypeStd_Val)
#define USBRT_TypeClass			USBRT_Type(USBRT_TypeClass_Val)
#define USBRT_TypeVendor		USBRT_Type(USBRT_TypeVendor_Val)

#define USBRT_Recip_Pos			0		// bit position
#define USBRT_Recip_Msk			(0x1F << USBRT_Recip_Pos)
#define USBRT_Recip(value)		(USBRT_Recip_Msk & ((value) << USBRT_Recip_Pos))
#define USBRT_RecipDevice_Val	0
#define USBRT_RecipIface_Val	1
#define USBRT_RecipEp_Val		2
#define USBRT_RecipOther_Val	3
#define USBRT_RecipDevice		USBRT_Recip(USBRT_RecipDevice_Val)
#define USBRT_RecipIface		USBRT_Recip(USBRT_RecipIface_Val)
#define USBRT_RecipEp			USBRT_Recip(USBRT_RecipEp_Val)
#define USBRT_RecipOther		USBRT_Recip(USBRT_RecipOther_Val)

// Bits in wValue
#define USBVAL_Index_Pos		0
#define USBVAL_Index_Msk		(0xFF << USBVAL_Index_Pos)
#define USBVAL_Index(value)		(USBVAL_Index_Msk & ((value) << USBVAL_Index_Pos))
#define USBVAL_Type_Pos			8
#define USBVAL_Type_Msk			(0xFF << USBVAL_Type_Pos)
#define USBVAL_Type(value)		(USBVAL_Type_Msk & ((value) << USBVAL_Type_Pos))

// Features for Set/Get Feature
enum UsbFeature
{
	USBFEAT_EndpointHalt,
	USBFEAT_DeviceRemoteWakeup,
	USBFEAT_TestMode,
};

//****************************************************************************
// Structure definitions for USB descriptors
//
// To ensure descriptors are packed end-to-end regardless of size and
// alignment, set structure packing to byte alignment
//
//****************************************************************************

#pragma pack(push, 1)

enum UsbDescType
{
	USBDESC_Device = 1,
	USBDESC_Config,
	USBDESC_String,
	USBDESC_Interface,
	USBDESC_Endpoint,
	USBDESC_Device_qualifier,
	USBDESC_OtherSpeedConfig,
	USBDESC_InterfacePower,
	USBDESC_Otg,
	USBDESC_Debug,
	USBDESC_InterfaceAssociation,
	USBDESC_Hid = 33,
};


//****************************************************************************
// Device Descriptor
//

struct UsbDeviceDesc
{
	byte	bLength;
	byte	bDescType;
	ushort	bcdUSB;
	byte	bDeviceClass;
	byte	bDeviceSubclass;
	byte	bDeviceProtocol;
	byte	bMaxPacketSize0;
	ushort	idVendor;
	ushort	idProduct;
	ushort	bcdDevice;
	byte	iManufacturer;
	byte	iProduct;
	byte	iSerialNumber;
	byte	bNumConfigs;
};

enum UsbClass
{
	USBCLASS_None,
	USBCLASS_Audio,
	USBCLASS_Cdc,
	USBCLASS_Hid,
	USBCLASS_Physical = 5,
	USBCLASS_Image,
	USBCLASS_Printer,
	USBCLASS_MassStorage,
	USBCLASS_Hub,
	USBCLASS_CdcData,
	USBCLASS_SmartCard,
	USBCLASS_Security = 13,
	USBCLASS_Video
};

//****************************************************************************
// Configuration Descriptor
//

struct UsbConfigDesc
{
	byte	bLength;
	byte	bDescType;
	ushort	wTotalLength;
	byte	bNumInterfaces;
	byte	bConfigValue;
	byte	iConfig;
	byte	bmAttributes;
	byte	bMaxPower;
};

// Definition of bmAttributes
#define USBCFG_Power_Pos			6
#define USBCFG_Power_Msk			(1 << USBCFG_Power_Pos)
#define USBCFG_PowerBus_Val			0
#define USBCFG_PowerSelf_Val		1
#define USBCFG_PowerBus				(USBCFG_PowerBus_Val << USBCFG_Power_Pos)
#define USBCFG_PowerSelf			(USBCFG_PowerBus_Val << USBCFG_Power_Pos)
#define USBCFG_RemoteWakeup_Pos		5
#define USBCFG_RemoteWakeup_Msk		(1 << USBCFG_RemoteWakeup_Msk)
#define USBCFG_RemoteWakeup_Val		1
#define USBCFG_RemoteWakeup			(USBCFG_RemoteWakeup_Val << USBCFG_RemoteWakeup_Pos)
#define USBCFG_BaseAttributes		0x80

//****************************************************************************
// String Descriptor
//

struct UsbStringDescHead
{
	byte	bLength;
	byte	bDescType;
};

struct UsbStringDesc
{
	UsbStringDescHead	desc;
	char16_t			str[];
};

//****************************************************************************
// Interface Descriptor
//

struct UsbInterfaceDesc
{
	byte	bLength;
	byte	bDescType;
	byte	bInterfaceNumber;
	byte	bAlternateSetting;
	byte	bNumEndpoints;
	byte	bInterfaceClass;
	byte	bInterfaceSubclass;
	byte	bInterfaceProtocol;
	byte	iInterface;
};

//****************************************************************************
// Endpoint Descriptor
//

struct UsbEndpointDesc
{
	byte	bLength;
	byte	bDescType;
	byte	bEndpointAddr;
	byte	bmAttributes;
	ushort	wMaxPacketSize;
	byte	bInterval;
};

// Definitions for bEndpointAddr
#define USBEP_Dir_Pos		7	// bit position
#define USBEP_Dir_Msk		(1 << USBEP_Dir_Pos)
#define	USBEP_DirIn_Val		1
#define USBEP_DirOut_Val	0
#define USBEP_DirIn			(USBEP_DirIn_Val << USBEP_Dir_Pos)
#define USBEP_DirOut		(USBEP_DirOut_Val << USBEP_Dir_Pos)

// Definition of bmAttributes
#define USBEP_Control	0
#define USBEP_Bulk		2
#define	USBEP_Interrupt	3

//****************************************************************************
// HID class
//

enum UsbHidReq
{
	USBHIDREQ_Get_Report = 1,
	USBHIDREQ_Get_Idle,
	USBHIDREQ_Get_Protocol,
	USBHIDREQ_Set_Report = 9,
	USBHIDREQ_Set_Idle,
	USBHIDREQ_Set_Protocol,
};

enum UsbHidSubclass
{
	USBHIDSUB_NoBoot,
	USBHIDSUB_Boot,
};

enum UsbHidProtocol
{
	USBHIDPROTO_Keyboard = 1,
	USBHIDPROTO_Mouse,
};

enum UsbHidReportProtocol
{
	USBHIDRP_Boot,
	USBHIDRP_Report,
};

enum UsbHidReportType
{
	USBHIDRT_Input = 0x100,
	USBHIDRT_Output = 0x200,
	USBHIDRT_Feature = 0x300,
};


#pragma pack(pop)
