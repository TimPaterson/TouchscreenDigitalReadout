//****************************************************************************
// Class FlashDrive
// FlashDrive.h
//
// Created 8/31/2020 4:15:42 PM by Tim
//
//****************************************************************************

#pragma once


#include <Usb/UsbMassStorage.h>
#include "FlashDriveDef.h"
#include <Usb/Device/USBdevice.h>
#include <algorithm>

template<class T>
class FlashDrive : public USBdeviceBase
{
	//*********************************************************************
	// Local Types
	//*********************************************************************

protected:
	struct CapacityList
	{
		ScsiCapacityListHeader	Head;
		ScsiCapacityDescriptor	Desc;
	};

	struct CachePage
	{
		ScsiModeParamterHeader6	Head;
		ScsiCachingPage			Cache;
	};

	struct InfoPage
	{
		ScsiModeParamterHeader6	Head;
		ScsiInfoExceptionsControlPage	Info;
	};

	union Buffer_t 
	{
		UsbMsCbwHead	CmdHead;
		UsbMsCbw		Cmd;
		UsbMsCswHead	StatusHead;
		UsbMsCsw		Status;
		ScsiInquiryData	InqResp;
		ScsiReadCapacityData	CapData;
		ScsiUnitSerialNumber	SerialNum;
		CachePage		Cache;
		InfoPage		Info;
		CapacityList	CapList;
		byte			Packet[OUT_PACKET_SIZE];
	};

	enum CommandState
	{
		CS_Command,
		CS_ReadInfo,
		CS_ReadData,
		CS_WriteData,
		CS_Status,
		CS_Detach,
	};

	//*********************************************************************
	// These are callbacks implemented in class T, the template argument
	//*********************************************************************

	/*
protected:
	static bool ReadData(ulong Lba, int cBlock);
	static bool WriteData(ulong Lba, int cBlock);
	static bool DataWritten(void *pv, int cb);
	static ulong GetLastLba();
	static ulong GetBlockSize();
	static bool MediaPresent();
	static void Reset();
	static bool NonStandardSetup(UsbSetupPacket *pSetup);
	static const void *NonStandardString(int index);
	static const UsbStringDesc *GetSerialStrDesc();
	*/

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	static void SendStatus(UsbMsStatus status, int cbResidue = 0)
	{
		Buffer.StatusHead.dCswSignature = CSW_SIGNATURE;
		Buffer.StatusHead.dCswTag = CmdTag;
		Buffer.StatusHead.dCswDataResidue = cbResidue;
		Buffer.Status.bCswStatus = status;
		SendToHost(IN_ENDPOINT_NUM, &Buffer, sizeof(UsbMsCsw));
		state = CS_Status;
		ReceiveFromHost(OUT_ENDPOINT_NUM, &Buffer, sizeof(UsbMsCbw));
	}

	static void InitSense(byte bKey = 0, byte bAdditional = 0)
	{
		// Setup sense data
		memset(&Sense, 0, sizeof Sense);
		Sense.bResponseCode = SCSISRC_CurrentError;
		Sense.bAdditionalSenseLength = ScsiAdditionalSenseLength;
		Sense.bSenseKey = bKey;
		Sense.bAdditionalSenseCode = bAdditional;
		fSenseSet = true;
	}

	//*********************************************************************
	// Implementation of callbacks from USBdevice class
	//*********************************************************************

public:
	static void DeviceConfigured()
	{
		// Get ready to accept a Command Block Wrapper
		ReceiveFromHost(OUT_ENDPOINT_NUM, &Buffer, sizeof(UsbMsCbw));
	}

	static void RxData(int iEp, void *pv, int cb) NO_INLINE_ATTR
	{
		if (state == CS_Command)
		{
			UsbMsCbwHead	*pCbwHead;
			UsbMsCbw		*pCbw;

			pCbwHead = (UsbMsCbwHead *)pv;
			if (cb == sizeof(UsbMsCbw) && pCbwHead->dCbwSignature == CBW_SIGNATURE)
			{
				// CBW is valid
				pCbw = (UsbMsCbw *)pv;
				CmdTag = pCbw->dCbwTag;
				cb = pCbwHead->dCbwDataTransferLength;
				cbTranferLen = cb;

				switch (pCbw->bOpcode)
				{
				case SCSIOP_StartStopUnit:
					SendStatus(USBMSSTAT_Passed);
					if (pCbw->bCbwCbLength == sizeof(ScsiStartStopUnit) && 
						pCbw->StartStop.LoEj == 1 && pCbw->StartStop.Start == 0)
					{
						state = CS_Detach;
					}
					return;

				case SCSIOP_PreventAllowMediumRemoval:
					SendStatus(USBMSSTAT_Passed);
					return;

				case SCSIOP_TestUnitReady:
					SendStatus(USBMSSTAT_Passed);
					return;

				case SCSIOP_Inquiry:
					if (pCbw->bCbwCbLength != sizeof(ScsiInquiry) || pCbw->bmCbwFlags != USBMSCF_DataIn)
						break;
					if (pCbw->Inquiry.CmdDt)
					{
						InitSense(SCSISK_IllegalRequest, SCSIASC_InvalidFieldInCdb);
						goto IllegalRequest;
					}
					if (pCbw->Inquiry.Evpd)
					{
						int		cbNum;

						switch (pCbw->Inquiry.bPageCode)
						{
						case SCSIVPD_UnitSerialNumber:
							// UNDONE: get SCSI serial number
							cbNum = sizeof("1234") - 1;
							Buffer.SerialNum.head.bDeviceType = SCSIIDT_DirectAccess;
							Buffer.SerialNum.head.bPageCode = SCSIVPD_UnitSerialNumber;
							Buffer.SerialNum.head.Reserved = 0;
							Buffer.SerialNum.head.bPageLength = cbNum;
							memcpy(Buffer.SerialNum.SerialNumber, "1234", cbNum);
							cbNum += sizeof(ScsiUnitSerialNumberHead);
							SendToHost(IN_ENDPOINT_NUM, &Buffer, std::min(cb, cbNum));
							state = CS_ReadInfo;
							return;

						default:
							InitSense(SCSISK_IllegalRequest, SCSIASC_InvalidFieldInCdb);
							goto IllegalRequest;
						}
						return;
					}
					// Standard inquiry data
					memcpy(&Buffer.InqResp, &InquiryResponse, sizeof InquiryResponse);
					SendToHost(IN_ENDPOINT_NUM, &Buffer, std::min((int)sizeof InquiryResponse, cb));
					state = CS_ReadInfo;
					return;

				case SCSIOP_ReadCapacity:
					if (pCbw->bCbwCbLength != sizeof(ScsiReadCapacity) || 
						pCbw->bmCbwFlags != USBMSCF_DataIn ||
						cb != sizeof(ScsiReadCapacityData))
						break;
					Buffer.CapData.dLastLba = T::GetLastLba();
					Buffer.CapData.dBlockSize = T::GetBlockSize();
					SendToHost(IN_ENDPOINT_NUM, &Buffer, sizeof(ScsiReadCapacityData));
					state = CS_ReadInfo;
					return;

				case SCSIOP_ReadFormatCapacities:
					if (pCbw->bCbwCbLength != sizeof(ScsiReadFormatCapacities) || pCbw->bmCbwFlags != USBMSCF_DataIn)
						break;
					// Build capacity list
					Buffer.CapList.Head.Reserved[0] = 0;
					Buffer.CapList.Head.Reserved[1] = 0;
					Buffer.CapList.Head.Reserved[2] = 0;
					Buffer.CapList.Head.bListLength = sizeof(ScsiCapacityDescriptor);
					Buffer.CapList.Desc.dNumberOfBlocks = T::GetLastLba() + 1;
					Buffer.CapList.Desc.bDescriptorCode = T::MediaPresent() ? SCSICDC_Formatted : SCSICDC_NoMedia;
					Buffer.CapList.Desc.BlockLength = T::GetBlockSize();
					SendToHost(IN_ENDPOINT_NUM, &Buffer, std::min((int)sizeof(CapacityList), cb));
					state = CS_ReadInfo;
					return;

				case SCSIOP_ModeSense6:
					if (pCbw->bCbwCbLength != sizeof(ScsiModeSense6) || pCbw->bmCbwFlags != USBMSCF_DataIn)
						break;
					switch (pCbw->ModeSense6.PageCode)
					{
					case SCSIPC_AllPages:
					case SCSIPC_Caching:
						memset(&Buffer, 0, sizeof(CachePage));
						Buffer.Cache.Head.bModeDataLength = sizeof(CachePage) - offsetof(ScsiModeParamterHeader6, bMediumType);
						Buffer.Cache.Cache.bPageCode = SCSIPC_Caching;
						Buffer.Cache.Cache.bPageLength = ScsiCachingPageLength;
						SendToHost(IN_ENDPOINT_NUM, &Buffer, std::min((int)sizeof(CachePage), cb));
						state = CS_ReadInfo;
						return;

					default:
						// Setup sense (error) data
						InitSense(SCSISK_IllegalRequest, SCSIASC_InvalidFieldInCdb);
						goto IllegalRequest;
					}
					break;

				case SCSIOP_Read10:
					if (pCbw->bCbwCbLength != sizeof(ScsiRead10) || pCbw->bmCbwFlags != USBMSCF_DataIn)
						break;
					LbaTransfer = pCbw->Read10.dLba;
					if (T::ReadData(LbaTransfer, pCbw->Read10.wTransferLength))
					{
						state = CS_ReadData;
						return;
					}
					break;

				case SCSIOP_Write10:
					if (pCbw->bCbwCbLength != sizeof(ScsiWrite10) || pCbw->bmCbwFlags != USBMSCF_DataOut)
						break;
					LbaTransfer = pCbw->Write10.dLba;
					if (T::WriteData(LbaTransfer, pCbw->Write10.wTransferLength))
					{
						state = CS_WriteData;
						return;
					}
					break;

				case SCSIOP_RequestSense:
					if (pCbw->bCbwCbLength < sizeof(ScsiRequestSense) || pCbw->bmCbwFlags != USBMSCF_DataIn)
						break;
					if (!fSenseSet)
						InitSense();
					SendToHost(IN_ENDPOINT_NUM, &Sense, std::min((int)sizeof Sense, cb));
					fSenseSet = false;
					state = CS_ReadInfo;
					return;

				default:
					DEBUG_PRINT("Command not implemented: %i\n", pCbw->bOpcode);
					break;
				}

				// Setup sense data
				InitSense(SCSISK_IllegalRequest, SCSIASC_InvalidOpcode);
IllegalRequest:
				DEBUG_PRINT("Command failed\n");
				// Command not implemented
				if (pCbwHead->dCbwDataTransferLength != 0)
				{
					// Host want to transfer data, STALL the endpoint
					if (pCbw->bmCbwFlags == USBMSCF_DataIn)
						SetStall(IN_ENDPOINT_NUM, StallDir::In);
					else
						SetStall(OUT_ENDPOINT_NUM, StallDir::Out);
				}

				// Return status fail
				SendStatus(USBMSSTAT_Failed, pCbwHead->dCbwDataTransferLength);
				return;
			}

			// Command not valid, set STALL on both IN & OUT
			DEBUG_PRINT("Command block invalid\n");
			if (IN_ENDPOINT_NUM == OUT_ENDPOINT_NUM)
				SetStall(IN_ENDPOINT_NUM, (StallDir)(StallDir::In | StallDir::Out));
			else
			{
				SetStall(IN_ENDPOINT_NUM, StallDir::In);
				SetStall(OUT_ENDPOINT_NUM, StallDir::Out);
			}
			SendStatus(USBMSSTAT_Failed, pCbwHead->dCbwDataTransferLength);
			return;
		}
		else if (state == CS_WriteData)
		{
			int		cbLeft;

			cbLeft = cbTranferLen - cb;
			if (cbLeft > 0)
			{
				cbTranferLen = cbLeft;
				cb /= T::GetBlockSize();
				LbaTransfer += cb;
				T::WriteData(LbaTransfer, cbLeft / T::GetBlockSize());
				return;
			}
			SendStatus(T::DataWritten(pv, cb) ? USBMSSTAT_Passed : USBMSSTAT_Failed, cbLeft);
			return;
		}

		// Data not expected
		DEBUG_PRINT("Expected data in state %i\n", state);
		if (IN_ENDPOINT_NUM == OUT_ENDPOINT_NUM)
			SetStall(IN_ENDPOINT_NUM, (StallDir)(StallDir::In | StallDir::Out));
		else
		{
			SetStall(IN_ENDPOINT_NUM, StallDir::In);
			SetStall(OUT_ENDPOINT_NUM, StallDir::Out);
		}
		SendStatus(USBMSSTAT_PhaseError);
	}

	static void TxDataRequest(int iEp)
	{
	}

	static void TxDataSent(int iEp, int cb)
	{
		int		cbLeft;

		switch (state)
		{
		case CS_Detach:
			Detach();
			//
			// Fall into CS_Status state
			//
		case CS_Status:
			// status sent, ready for next command
			state = CS_Command;		
			break;

		case CS_ReadData:
			cbLeft = cbTranferLen - cb;
			if (cbLeft > 0)
			{
				cbTranferLen = cbLeft;
				cb /= T::GetBlockSize();
				LbaTransfer += cb;
				T::ReadData(LbaTransfer, cbLeft / T::GetBlockSize());
				return;
			}
			SendStatus(USBMSSTAT_Passed, cbLeft);
			break;

		case CS_ReadInfo:
			// Data sent, send status
			SendStatus(USBMSSTAT_Passed, cbTranferLen - cb);
			break;

		}
	}

	static const void *NonStandardString(int index)
	{
		return T::NonStandardString(index);
	}

	static bool NonStandardSetup(UsbSetupPacket *pSetup)
	{
		byte	*pb;

		switch (pSetup->bmRequestType)
		{
		case USBRT_DirOut | USBRT_TypeClass | USBRT_RecipIface:
			switch (pSetup->bRequest)
			{
			case MSREQ_Reset:
				state = CS_Command;
				T::Reset();
				AckControlPacket();
				return true;
			}
			break;

		case USBRT_DirIn | USBRT_TypeClass | USBRT_RecipIface:
			switch (pSetup->bRequest)
			{
			case MSREQ_Get_Max_LUN:
				if (pSetup->wLength == 1)
				{
					pb = (byte *)GetSetupBuf();
					*pb = 0;	// only one LUN
					SendControlPacket(1);
					return true;
				}
				break;
			}
			break;
		}
		return T::NonStandardSetup(pSetup);
	}

	static const UsbStringDesc *GetSerialStrDesc()
	{
		return T::GetSerialStrDesc();
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

protected:
	inline static const ScsiInquiryData InquiryResponse =
	{
		SCSIIDT_DirectAccess,
		SCSIIR_Removable,
		SCSIIV_Spc2,
		SCSIIRDF_Std,
		sizeof(ScsiInquiryData) - offsetof(ScsiInquiryData, bOptions1),
		0, 0, 0,	// Options1, Options2, Options3
		{ SCSI_VendorId },
		{ SCSI_ProductId },
		{ SCSI_ProductRev }
	};

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

protected:
	inline static Buffer_t	Buffer;
	// USB buffers must be 4-byte aligned
	inline static ScsiFixedFormatSenseData	Sense ALIGNED_ATTR(int);
	inline static ulong		CmdTag;
	inline static ulong		LbaTransfer;
	inline static int		cbTranferLen;
	inline static byte		state;
	inline static bool		fSenseSet;

	//*********************************************************************
	// The interrupt service routine - class with no data
	//*********************************************************************

public:
	static USBdeviceIsr<FlashDrive<T>>	ISR;
};
