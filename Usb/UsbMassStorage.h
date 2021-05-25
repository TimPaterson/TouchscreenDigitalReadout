//****************************************************************************
// UsbMassStorage.h
//
// Created 8/31/2020 4:50:39 PM by Tim
//
//****************************************************************************

#pragma once


#define EXPAND_SIGNATURE(a, b, c, d)	((ulong)a | ((ulong)b << 8) | ((ulong)c << 16) | ((ulong)d << 24))
#define MAKE_SIGNATURE(x)	EXPAND_SIGNATURE(x)


//****************************************************************************
// Nothing is aligned, so pack the structures

#pragma pack(push, 1)

//****************************************************************************
// SCSI
//****************************************************************************

// SCSI is big-endian. Here are classes that hide the conversion

#define USE_BUILTIN_BYTESWAP	0

class uibe16
{
#if USE_BUILTIN_BYTESWAP
	ushort	val;

public:
	uibe16 operator=(uint u)
	{
		val = __builtin_bswap16(u);
		return *this;
	}

	operator uint() { return __builtin_bswap16(val); }
#else
	byte	val[2];

public:
	uibe16 operator=(uint u)
	{
		val[1] = u;
		val[0] = u >> 8;
		return *this;
	}

	operator uint() { return ((uint)val[0] << 8) | val[1]; }
#endif
};

class uibe24
{
#if USE_BUILTIN_BYTESWAP
	byte	valHi;
	ushort	valLo;

public:
	uibe24 operator=(uint u)
	{
		valHi = u >> 16;
		valLo = __builtin_bswap16(u);
		return *this;
	}

	operator uint() { return __builtin_bswap16(valLo) | ((uint)valHi << 16); }
#else
	byte	val[3];

public:
	uibe24 operator=(uint u)
	{
		val[2] = u;
		val[1] = u >> 8;
		val[0] = u >> 16;
		return *this;
	}

	operator uint()
	{
		return ((uint)val[0] << 16) | ((uint)val[1] << 8) | val[2];
	}
#endif
};

class uibe32
{
#if USE_BUILTIN_BYTESWAP
	ulong	val;

public:
	uibe32 operator=(uint u)
	{
		val = __builtin_bswap32(u);
		return *this;
	}

	operator uint() { return __builtin_bswap32(val); }
#else
	byte	val[4];

public:
	uibe32 operator=(uint u)
	{
		val[3] = u;
		val[2] = (u >> 8);
		val[1] = (u >> 16);
		val[0] = (u >> 24);
		return *this;
	}

	operator uint()
	{
		return ((uint)val[0] << 24) | ((uint)val[1] << 16) | ((uint)val[2] << 8) | val[3];
	}
#endif
};

//****************************************************************************
// SCSI Command Blocks

enum ScsiOpcode
{
	SCSIOP_FormatUnit = 0x04,
	SCSIOP_Inquiry = 0x12,
	SCSIOP_ModeSelect6 = 0x15,
	SCSIOP_ModeSense6 = 0x1A,
	SCSIOP_PreventAllowMediumRemoval = 0x1E,
	SCSIOP_Read6 = 0x08,
	SCSIOP_Read10 = 0x28,
	SCSIOP_ReadCapacity = 0x25,
	SCSIOP_ReadFormatCapacities = 0x23,
	SCSIOP_ReportLuns = 0xA0,
	SCSIOP_RequestSense = 0x03,
	SCSIOP_SendDiagnostic = 0x1D,
	SCSIOP_StartStopUnit = 0x1B,
	SCSIOP_TestUnitReady = 0x00,
	SCSIOP_Write6 = 0x0A,
	SCSIOP_Write10 = 0x2A,
};

enum ScsiPageCode
{
	SCSIPC_Caching = 0x08,
	SCSIPC_InformationalExceptionsControl = 0x1C,
	SCSIPC_AllPages = 0x3F,
};

struct ScsiFormatUnit
{
	byte	bOpcode;
	byte	bOptions;
	byte	bVendorSpecific;
	ushort	Obsolete;
	byte	bControl;
};

struct ScsiInquiry
{
	byte	bOpcode;

	byte	Evpd:1;
	byte	CmdDt:1;
	byte	:6;

	byte	bPageCode;
	uibe16	wAllocationLength;
	byte	bControl;
};

struct ScsiModeSelect6
{
	byte	bOpcode;
	byte	bOptions;
	byte	Reserved[2];
	byte	bParameterListLength;
	byte	bControl;
};

struct ScsiModeSense6
{
	byte	bOpcode;
	byte	bOptions;

	byte	PageCode:6;
	byte	PC:2;

	byte	Reserved;
	byte	bAllocationLength;
	byte	bControl;
};

struct ScsiRead6
{
	byte	bOpcode;
	uibe24	Lba;
	byte	bTransferLength;
	byte	bControl;
};

struct ScsiRead10
{
	byte	bOpcode;
	byte	bOptions;
	uibe32	dLba;
	byte	bGroup;
	uibe16	wTransferLength;
	byte	bControl;
};

struct ScsiReadCapacity
{
	byte	bOpcode;
	byte	Reserved1;
	uibe32	dLba;
	ushort	Reserved2;
	byte	bPmi;
	byte	bControl;
};

struct ScsiReadFormatCapacities
{
	byte	bOpcode;
	byte	Reserved1;
	uibe32	dLba;
	ushort	Reserved2;
	byte	bPmi;
	byte	bControl;
};

struct ScsiReportLuns
{
	byte	bOpcode;
	byte	bLun;
	byte	Reserved1[5];
	uibe16	wAllocationLength;
	ushort	Reserved2;
	byte	bControl;
};

struct ScsiRequestSense
{
	byte	bOpcode;
	byte	bDesc;
	ushort	Reserved;
	byte	bAllocationLength;
	byte	bControl;
};

struct ScsiSendDiagnostic
{
	byte	bOpcode;
	byte	bOptions;
	byte	Reserved;
	uibe16	ParameterListLength;
	byte	bControl;
};

struct ScsiStartStopUnit
{
	byte	bOpcode;
	byte	bImmed;
	byte	Reserved[2];

	byte	Start:1;
	byte	LoEj:1;
	byte	:2;
	byte	PowerCondition:4;

	byte	bControl;
};

struct ScsiTestUnitReady
{
	byte	bOpcode;
	ushort	Reserved;
	byte	bControl;
};

struct ScsiWrite6
{
	byte	bOpcode;
	uibe24	Lba;
	byte	bTransferLength;
	byte	bControl;
};

struct ScsiWrite10
{
	byte	bOpcode;
	byte	bOptions;
	uibe32	dLba;
	byte	bGroup;
	uibe16	wTransferLength;
	byte	bControl;
};

//****************************************************************************
// SCSI Responses

struct ScsiInquiryData
{
	byte	bDeviceType;
	byte	bRemovable;
	byte	bVersion;
	byte	bResponseDataFormat;
	byte	bAdditionalLength;
	byte	bOptions1;
	byte	bOptions2;
	byte	bOptions3;
	char	VendorId[8];
	char	ProductId[16];
	char	ProductRev[4];
};

struct ScsiUnitSerialNumberHead
{
	byte	bDeviceType;
	byte	bPageCode;
	byte	Reserved;
	byte	bPageLength;
};

struct ScsiUnitSerialNumber
{
	ScsiUnitSerialNumberHead	head;
	byte	SerialNumber[];
};

enum ScsiInquiryVpdCodes
{
	SCSIVPD_SupportedVpdPages = 0,
	SCSIVPD_UnitSerialNumber = 0x80,
	SCSIVPD_DeviceId = 0x83,
};

enum ScsiInquiryDeviceType
{
	SCSIIDT_DirectAccess,
	SCSIIDT_SequentialAccess,
};

enum ScsiInquiryVersion
{
	SCSIIV_Spc1 = 3,
	SCSIIV_Spc2 = 4,
	SCSIIV_Spc3 = 5,
};

enum ScsiInquiryRemovable
{
	SCSIIR_NotRemovable = 0,
	SCSIIR_Removable = 0x80
};

enum ScsiInquiryResponseDataFormat
{
	SCSIIRDF_Std = 2
};

//****************************************************************************

struct ScsiFixedFormatSenseData
{
	byte	bResponseCode:7;
	byte	bValid:1;

	byte	Reserved1;
	byte	bSenseKey;
	uibe32	dInformation;
	byte	bAdditionalSenseLength;
	uibe32	dCommandSpecificInformation;
	byte	bAdditionalSenseCode;
	byte	bAdditionalSenseCodeQualifier;
	byte	bFieldReplaceableUnitCode;
	byte	SenseKeySpecific[3];
};

enum ScsiSenseResponseCode
{
	SCSISRC_CurrentError = 0x70,
	SCSISRC_DeferredError = 0x71,
	SCSISRC_VendorSpecific = 0x7F,
};

enum ScsiSenseKey
{
	SCSISK_NoSense,
	SCSISK_RecoveredError,
	SCSISK_NotReady,
	SCSISK_MediumError,
	SCSISK_HardwareError,
	SCSISK_IllegalRequest,
	SCSISK_UnitAttention,
	SCSISK_DataProtect,
	SCSISK_BlankCheck,
	SCSISK_VendorSpecific,
	SCSISK_CopyAborted,
	SCSISK_AbortedCommand,
	SCSISK_VolumeOverflow = 13,
	SCSISK_Miscompare,
};

enum ScsiAdditionalSenseCode
{
	SCSIASC_InvalidOpcode = 0x20,
	SCSIASC_InvalidFieldInCdb = 0x24,
};

static constexpr int ScsiAdditionalSenseLength = sizeof(ScsiFixedFormatSenseData) - 
	offsetof(ScsiFixedFormatSenseData, dCommandSpecificInformation);

//****************************************************************************

struct ScsiReadCapacityData
{
	uibe32	dLastLba;
	uibe32	dBlockSize;
};

struct ScsiCapacityListHeader
{
	byte	Reserved[3];
	byte	bListLength;
};

struct ScsiCapacityDescriptor
{
	uibe32	dNumberOfBlocks;
	byte	bDescriptorCode;
	uibe24	BlockLength;
};

enum ScsiCapacityDescriptorCode
{
	SCSICDC_Unformatted = 1,
	SCSICDC_Formatted,
	SCSICDC_NoMedia
};

//****************************************************************************
// Mode Pages

struct ScsiModeParamterHeader6
{
	byte	bModeDataLength;
	byte	bMediumType;
	byte	bDeviceSpecificParameter;
	byte	bBlockDescriptorLength;
};

static constexpr int ScsiCachingPageLength = 18;	// after bPageLength

struct ScsiCachingPage
{
	byte	bPageCode;
	byte	bPageLength;
	byte	Stuff[ScsiCachingPageLength];
};

static constexpr int ScsiInfoExceptionsControlPageLength = 10;	// after bPageLength

struct ScsiInfoExceptionsControlPage
{
	byte	bPageCode;
	byte	bPageLength;
	byte	Stuff[ScsiInfoExceptionsControlPageLength];
};

//****************************************************************************
// USB Mass Storage
//****************************************************************************

enum UsbMsInterfaceSubclass
{
	MSIFSC_RBC = 1,
	MSIFSC_SFF8020I,
	MSIFSC_QIC157,
	MSIFSC_UFI,
	MSIFSC_SFF8070I,
	MSIFSC_SCSI
};

enum UsbMsInterfaceProtocol
{
	MSIFP_CBI_INT,
	MSIFP_CBI_NO_INT,
	MSIFP_BULK = 0x50
};

enum UsbMsReq
{
	MSREQ_Get_Max_LUN = 254,
	MSREQ_Reset = 255
};

//****************************************************************************
// USB Command Block Wrapper

#define CBW_SIGNATURE_CHARS	'U','S','B','C'

static constexpr ulong CBW_SIGNATURE = MAKE_SIGNATURE(CBW_SIGNATURE_CHARS);

// The start of the wrapper is aligned
#pragma pack(4)

struct UsbMsCbwHead
{
	ulong	dCbwSignature;
	ulong	dCbwTag;
	ulong	dCbwDataTransferLength;
};

// The rest is not aligned
#pragma pack(1)

struct UsbMsCbw : UsbMsCbwHead
{
	byte	bmCbwFlags;
	byte	bCbwLun;
	byte	bCbwCbLength;
	union
	{
		byte				bOpcode;
		ScsiFormatUnit		FormatUnit;
		ScsiInquiry			Inquiry;
		ScsiModeSelect6		ModeSelect6;
		ScsiModeSense6		ModeSense6;
		ScsiRead6			Read6;
		ScsiRead10			Read10;
		ScsiReadCapacity	ReadCapacity;
		ScsiReadFormatCapacities	ReadCapacities;
		ScsiReportLuns		ReportLuns;
		ScsiRequestSense	RequestSense;
		ScsiSendDiagnostic	SendDiagnostic;
		ScsiStartStopUnit	StartStop;
		ScsiTestUnitReady	TestUnitReady;
		ScsiWrite6			Write6;
		ScsiWrite10			Write10;
		byte				CommandBlock[16];
	};
};

enum UsbMsCbwFlags
{
	USBMSCF_DataOut = 0,
	USBMSCF_DataIn = 0x80
};

//****************************************************************************
// USB Command Status Wrapper

#define CSW_SIGNATURE_CHARS	'U','S','B','S'

static constexpr ulong CSW_SIGNATURE = MAKE_SIGNATURE(CSW_SIGNATURE_CHARS);

//****************************************************************************
// The start of the two wrappers are aligned

// The start of the wrapper is aligned
#pragma pack(4)

struct UsbMsCswHead
{
	ulong	dCswSignature;
	ulong	dCswTag;
	ulong	dCswDataResidue;
};

// The rest is not aligned
#pragma pack(1)

struct UsbMsCsw : UsbMsCswHead
{
	byte	bCswStatus;
};

enum UsbMsStatus
{
	USBMSSTAT_Passed,
	USBMSSTAT_Failed,
	USBMSSTAT_PhaseError
};


#pragma pack(pop)
