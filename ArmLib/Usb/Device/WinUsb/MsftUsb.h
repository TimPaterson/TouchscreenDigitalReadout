//****************************************************************************
// Microsoft-specific USB descriptors
//
// To ensure descriptors are packed end-to-end regardless of size and
// alignment, set structure packing to byte alignment
//
//****************************************************************************

#pragma once
#pragma pack(push, 1)


//****************************************************************************
// Microsoft OS 1.0 String Descriptor

#define	MSFT_OS_STR_DESC			0xEE

#define	MSFT100		'M','S','F','T','1','0','0'
#define MSFT100str	"MSFT100"

#define MS_EXTENDED_COMPAT_ID_DESC	4
#define MS_EXTENDED_PROP_DESC		5

struct MsftOs10StringDesc
{
	byte	bLength;
	byte	bDescType;
	char16_t qwSignature[sizeof MSFT100str - 1];
	byte	bMS_VendorCode;
	byte	bPad;
};

//****************************************************************************
// This is the header the Extended Compat ID Descriptor

#define	MS_EXTENDED_BCD_VER			0x0100

// Functions
#define	WINUSB_ID	"WINUSB"
#define RNDIS_ID	"RNDIS"
#define PTP_ID		"PTP"	// picture transfer protocol
#define MTP_ID		"MTP"	// media transfer protocol

struct MsftExtendedCompatIdOsDescHeader
{
	ulong	dwLength;
	ushort	bcdVersion;
	ushort	wIndex;
	byte	bCount;
	byte	Reserved[7];
};

// After the above header, include <bCount> of these
struct MsftExtendedCompatIdOsDescFunction
{
	byte	bFirstInterfaceNumber;
	byte	Reserved1;
	char	compatID[8];
	byte	subCompatID[8];
	byte	Reserved2[6];
};

//****************************************************************************
// Extended Properties OS Feature Descriptor

// Property data type
enum ExtendedPropDataType
{
	EXTENDED_PROP_Unicode = 1,
	EXTENDED_PROP_UnicodeExpand,
	EXTENDED_PROP_Binary,
	EXTENDED_PROP_Int32,
	EXTENDED_PROP_Int32BigEndian,
	EXTENDED_PROP_UnicodeLink,
	EXTENDED_PROP_MultiUnicode
};

#define	EXTENDED_PROP_UNICODE	1

struct MsftExtendedPropOsDescHeader
{
	ulong	dwLength;
	ushort	bcdVersion;
	ushort	wIndex;
	ushort	wCount;
};

// Extended Properties OS Feature Descriptor
struct MsftPropHeader
{
	ulong	dwSize;
	ulong	dwPropDataType;
	ushort	wPropNameLength;
};

// Names of extended properties
#define	WINUSB_GUID_NAME				u"DeviceInterfaceGUID"

#define	DEVICE_LABEL_NAME				u"Label"

#define	SELECTIVE_SUSPEND_ENABLED		u"SelectiveSuspendEnabled"

struct WinUsbProp
{
	MsftPropHeader	header;
	char16_t	bPropName[_countof(WINUSB_GUID_NAME)];
	ulong		dwPropDataLength;
	char16_t	bPropertyData[_countof(WINUSB_GUID)];
};

struct DeviceNameProp
{
	MsftPropHeader	header;
	char16_t	bPropName[_countof(DEVICE_LABEL_NAME)];
	ulong		dwPropDataLength;
	char16_t	bPropertyData[_countof(USB_DEVICE_LABEL)];
};

// Additional property names

// This value is set to 1 to indicate that the device can 
// power down when idle (selective suspend).
#define	DEVICE_IDLE_ENABLED				u"DeviceIdleEnabled"

// This value is set to 1 to indicate that the device can be 
// suspended when idle by default.
#define	DEFAULT_IDLE_STATE				u"DefaultIdleState"

// This value is set to 5000 in milliseconds to indicate the 
// amount of time in milliseconds to wait before determining 
// that a device is idle.
#define DEFAULT_IDLE_TIMEOUT			u"DefaultIdleTimeout"

// This value is set to 1 to allow the user to control the ability 
// of the device to enable or disable USB selective suspend. A check 
// box Allow the computer to turn off this device to save power on 
// the device Power Management property page and the user can check 
// or uncheck the box to enable or disable USB selective suspend.
#define USER_SET_DEVICE_IDLE_ENABLED	u"UserSetDeviceIdleEnabled"

// This value is set to 1 to allow the user to control the ability 
// of the device to wake the system from a low-power state. A check 
// box "Allow this device to wake the computer" shows up in the 
// device power management property page and the user can check or 
// uncheck the box to enable or disable USB system wake.
#define SYSTEM_WAKE_ENABLED				u"SystemWakeEnabled"

#pragma pack(pop)
