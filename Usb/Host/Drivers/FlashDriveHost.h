//****************************************************************************
// FlashDriveHost.h
//
// Created 9/16/2020 10:37:13 AM by Tim
//
//****************************************************************************

#pragma once

// Note that the following symbols must be defined at this point:
//
// FAT_SECT_BUF_CNT	- number of sector buffers
// FAT_MAX_HANDLES	- max number of file handles
// FAT_NUM_DRIVES	- number of drives
//
// This would typically be done in FatFileDef.h

#include <Usb/UsbMassStorage.h>
#include <Usb/Host/UsbHostDriver.h>
#include <FatFile/FatDrive.h>


#define BLOCK_SIZE			FAT_SECT_SIZE
#define MAX_PACKET_SIZE		64


class FlashDriveHost : public FatDrive, public UsbHostDriver
{
	//*********************************************************************
	// Types
	//*********************************************************************

	static constexpr int TestReadyIntervalMs = 1;
	static constexpr int ReadWriteTimeoutMs = 500;

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

	union CmdBuffer 
	{
		UsbMsCbwHead	CmdHead;
		UsbMsCbw		Cmd;
		ScsiFixedFormatSenseData	Sense;
		byte			Packet[MAX_PACKET_SIZE];
	};

	union StatusBuffer
	{
		UsbMsCswHead	StatusHead;
		UsbMsCsw		Status;
		byte			Packet[MAX_PACKET_SIZE];
	};

	enum DriveState
	{
		DS_Idle,
		DS_SetConfig,
		DS_WaitConfig,
		DS_TestReady,
		DS_WaitReady,
		DS_DeviceReady,
		DS_Busy,
		DS_Error,
	};

	enum CommandState
	{
		CS_Idle,
		CS_Read,
		CS_WaitData,
		CS_Write,
		CS_GetStatus,
		CS_CheckStatusRetry,
		CS_CheckStatus,
		// Clear STALL w/o full reset
		CS_StartClearStall,
		CS_WaitClearStall,
		// Steps for reset recovery
		CS_StartReset,
		CS_WaitReset,
		CS_StartClearIn,
		CS_WaitClearIn,
		CS_StartClearOut,
		CS_WaitClearOut,
	};

	//*********************************************************************
	// Implementation of Storage class in FatDrive
	//*********************************************************************
public:
	virtual int GetStatus()
	{
		int		err;

		switch (m_stDrive)
		{
		case DS_Idle:
			return STERR_None;

		case DS_Error:
			err = m_errCode;
			m_errCode = STERR_None;
			m_stDrive = DS_Idle;
			return err;

		default:
			// Ensure the USB process loop is executed. This is needed
			// when using FAT functions that spin on GetStatus waiting
			// for completion.
			Process();
			break;
		}
		return STERR_Busy;
	}

	virtual int ReadData(ulong Lba, void *pv, uint cBlock)
	{
		if (m_stDrive != DS_Idle)
			return STERR_Busy;

		m_stDrive = DS_Busy;
		m_stCommand = CS_Read;
		m_tmr.Start();

		m_pvTransfer = pv;
		FillCmdWrapper(cBlock * BLOCK_SIZE, SCSIOP_Read10);
		bufCommand.Cmd.bmCbwFlags = USBMSCF_DataIn;
		bufCommand.Cmd.bCbwCbLength = sizeof(ScsiRead10);
		bufCommand.Cmd.Read10.dLba = Lba;
		bufCommand.Cmd.Read10.wTransferLength = cBlock;

		UsbSendData(&bufCommand, sizeof bufCommand.Cmd);
		return STERR_None;
	}

	virtual int WriteData(ulong Lba, void *pv, uint cBlock)
	{
		if (m_stDrive != DS_Idle)
			return STERR_Busy;

		m_stDrive = DS_Busy;
		m_stCommand = CS_Write;
		m_tmr.Start();

		m_pvTransfer = pv;
		FillCmdWrapper(cBlock * BLOCK_SIZE, SCSIOP_Write10);
		bufCommand.Cmd.bmCbwFlags = USBMSCF_DataOut;
		bufCommand.Cmd.bCbwCbLength = sizeof(ScsiWrite10);
		bufCommand.Cmd.Write10.dLba = Lba;
		bufCommand.Cmd.Write10.wTransferLength = cBlock;

		UsbSendData(&bufCommand, sizeof bufCommand.Cmd);
		return STERR_None;
	}

	virtual int InitDev() { return STERR_None; }
	virtual int MountDev() { return STERR_None; }
	virtual int DismountDev() { return STERR_None; }

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	void FillCmdWrapper(int cb, int opCode)
	{
		bufCommand.CmdHead.dCbwDataTransferLength = m_cbTransfer = cb;
		bufCommand.CmdHead.dCbwSignature = CBW_SIGNATURE;
		bufCommand.CmdHead.dCbwTag = ++m_Tag;
		memset(&bufCommand.CmdHead + 1, 0, sizeof(UsbMsCbw) - sizeof(UsbMsCbwHead));
		bufCommand.Cmd.bOpcode = m_cmdCur = opCode;
	}

	void UsbSendData(void *pv, int cb)
	{
		USBhost::SendData(m_bOutPipe, pv, cb);
	}

	void UsbReceiveData(void *pv, int cb)
	{
		USBhost::ReceiveData(m_bInPipe, pv, cb);
	}

	bool ClearStall(int iPipe)
	{
		USBhost::ControlPacket	pkt;

		pkt.packet.bmRequestType = USBRT_DirOut | USBRT_TypeStd | USBRT_RecipEp;
		pkt.packet.bRequest = USBREQ_Clear_Feature;
		pkt.packet.wValue = USBFEAT_EndpointHalt;
		pkt.packet.wIndex = USBhost::EndpointFromPipe(iPipe);
		pkt.packet.wLength = 0;
		return USBhost::ControlTransfer(this, NULL, pkt.u64);
	}

	void ResetRecovery()
	{
		m_stCommand = CS_StartReset;
	}

	void RequestSense()
	{
		m_stCommand = CS_Read;

		m_pvTransfer = &bufCommand;
		FillCmdWrapper(sizeof(ScsiFixedFormatSenseData), SCSIOP_RequestSense);
		bufCommand.Cmd.bmCbwFlags = USBMSCF_DataIn;
		bufCommand.Cmd.bCbwCbLength = sizeof(ScsiRequestSense);
		bufCommand.Cmd.RequestSense.bAllocationLength = sizeof(ScsiFixedFormatSenseData);

		UsbSendData(&bufCommand, sizeof bufCommand.Cmd);
	}

	void TestUnitReady()
	{
		m_stCommand = CS_WaitData;

		m_pvTransfer = &bufCommand;
		FillCmdWrapper(0, SCSIOP_TestUnitReady);
		bufCommand.Cmd.bCbwCbLength = sizeof(ScsiTestUnitReady);

		UsbSendData(&bufCommand, sizeof bufCommand.Cmd);
	}

	int MapScsiError(uint Asc)
	{
		switch (Asc)
		{
		case 0x3A:
			return STERR_NoMedium;
		}

		return STERR_DevFail;
	}

	//*********************************************************************
	// Override of USBhost virtual functions
	//*********************************************************************

	virtual UsbHostDriver *IsDriverForDevice(ulong ulVidPid, ulong ulClass, UsbConfigDesc *pConfig)
	{
		int		i;
		UsbInterfaceDesc	*pIf;
		UsbEndpointDesc		*pEp;

		if (ulClass != 0 || pConfig->bNumInterfaces != 1)
			return NULL;

		m_bConfig = pConfig->bConfigValue;

		pIf = (UsbInterfaceDesc *)ADDOFFSET(pConfig, pConfig->bLength);
		if (pIf->bInterfaceClass != USBCLASS_MassStorage || 
			pIf->bInterfaceSubclass != MSIFSC_SCSI || pIf->bNumEndpoints != 2)
			return NULL;

		pEp = (UsbEndpointDesc *)ADDOFFSET(pIf, pIf->bLength);
		i = USBhost::RequestPipe(this, pEp);
		if (i < 0)
			return NULL;
		if (pEp->bEndpointAddr & USBEP_DirIn)
			m_bInPipe = i;
		else
			m_bOutPipe = i;

		pEp++;
		i = USBhost::RequestPipe(this, pEp);
		if (i < 0)
		{
			USBhost::FreeAllPipes(this);
			return NULL;
		}
		if (pEp->bEndpointAddr & USBEP_DirIn)
			m_bInPipe = i;
		else
			m_bOutPipe = i;

		m_stDrive = DS_SetConfig;
		m_stCommand = CS_Idle;
		return this;
	}

	virtual void SetupTransactionComplete(int cbTransfer)
	{
		switch (m_stCommand)
		{
		case CS_WaitClearStall:
			// Now reread command status
			m_stCommand = CS_CheckStatusRetry;
			UsbReceiveData(&bufStatus, sizeof(UsbMsCsw));
			return;

		case CS_WaitReset:
			m_stCommand = CS_StartClearIn;
			return;

		case CS_WaitClearIn:
			m_stCommand = CS_StartClearOut;
			return;

		case CS_WaitClearOut:
			m_stCommand = CS_Idle;
			DEBUG_PRINT("Mass storage reset complete\n");
			RequestSense();
			return;
		}

		switch (m_stDrive)
		{
		case DS_WaitConfig:
			m_stDrive = DS_TestReady;
			m_tmr.Start();
			break;
		}
	}

	virtual void TransferComplete(int iPipe, int cbTransfer)
	{
		switch (m_stCommand)
		{
		case CS_Read:
			// Now read the data
			m_stCommand = CS_WaitData;
			UsbReceiveData(m_pvTransfer, m_cbTransfer);
			break;

		case CS_WaitData:
			m_stCommand = CS_GetStatus;
			break;

		case CS_Write:
			m_stCommand = CS_WaitData;
			UsbSendData(m_pvTransfer, m_cbTransfer);
			break;

		case CS_CheckStatusRetry:
		case CS_CheckStatus:
			if (bufStatus.StatusHead.dCswSignature != CSW_SIGNATURE ||
				bufStatus.StatusHead.dCswTag != m_Tag)
			{
				// Invalid response
				DEBUG_PRINT("Invalid status response\n");
				m_stCommand = CS_GetStatus;	// Will probably STALL
				return;
			}

			if (bufStatus.Status.bCswStatus != USBMSSTAT_Passed)
			{
				DEBUG_PRINT("Status failed, request sense...\n");
				RequestSense();
			}
			else
			{
				switch (m_cmdCur)
				{
				case SCSIOP_RequestSense:
					if (bufCommand.Sense.bResponseCode == SCSISRC_CurrentError)
					{
						DEBUG_PRINT("Sense Key: %X, ASC: %02X, ASCQ: %02X\n", 
							bufCommand.Sense.bSenseKey, bufCommand.Sense.bAdditionalSenseCode,
							bufCommand.Sense.bAdditionalSenseCodeQualifier);
					}
					else
						DEBUG_PRINT("Invalid sense response\n");

					m_errCode = MapScsiError(bufCommand.Sense.bAdditionalSenseCode);
					m_stDrive = DS_Error;
					break;

				case SCSIOP_Read10:
				case SCSIOP_Write10:
					m_stDrive = DS_Idle;
					break;

				case SCSIOP_TestUnitReady:
					m_stDrive = DS_DeviceReady;
				}
				m_stCommand = CS_Idle;
			}
			break;
		}
	}

	virtual void TransferError(int iPipe, TransferErrorCode err)
	{
		if (iPipe == 0)
		{
			// UNDONE: Error on control pipe
			DEBUG_PRINT("Error on control pipe\n");
			m_errCode = STERR_DevFail;
			m_stDrive = DS_Error;
			return;
		}

		if (err == TEC_Stall)
		{
			if (m_stCommand == CS_CheckStatusRetry && m_bPipeCur == iPipe)
				ResetRecovery();
			else
			{
				m_bPipeCur = iPipe;
				m_stCommand = CS_StartClearStall;
			}
		}
	}

	virtual int Process()
	{
		USBhost::ControlPacket	pkt;

		// First process any commands in progress
		switch (m_stCommand)
		{
		case CS_GetStatus:
			// Now read command status
			m_stCommand = CS_CheckStatus;
			UsbReceiveData(&bufStatus, sizeof(UsbMsCsw));
			return HOSTACT_None;

		case CS_StartClearStall:
			// Must set next state before call
			m_stCommand = CS_WaitClearStall;
			if (!ClearStall(m_bPipeCur))
				m_stCommand = CS_StartClearStall;	// change state back
			return HOSTACT_None;

		case CS_StartReset:
			// Send Mass Storage Reset
			pkt.packet.bmRequestType =  USBRT_DirOut | USBRT_TypeClass | USBRT_RecipIface;
			pkt.packet.bRequest = MSREQ_Reset;
			pkt.packet.wValue = 0;
			pkt.packet.wIndex = 0;
			pkt.packet.wLength = 0;
			// Must set next state before call
			m_stCommand = CS_WaitReset;
			if (!USBhost::ControlTransfer(this, NULL, pkt.u64))
				m_stCommand = CS_StartReset;	// change state back
			return HOSTACT_None;

		case CS_StartClearIn:
			// Clear IN pipe
			// Must set next state before call
			m_stCommand = CS_WaitClearIn;
			if (!ClearStall(m_bInPipe))
				m_stCommand = CS_StartClearIn;	// change state back
			return HOSTACT_None;

		case CS_StartClearOut:
			// Clear OUT pipe
			// Must set next state before call
			m_stCommand = CS_WaitClearOut;
			if (!ClearStall(m_bOutPipe))
				m_stCommand = CS_StartClearOut;	// change state back
			return HOSTACT_None;
		}

		switch (m_stDrive)
		{
		case DS_Busy:
			if (m_tmr.CheckDelay_ms(ReadWriteTimeoutMs))
			{
				DEBUG_PRINT("Timeout error\n");
				m_errCode = STERR_TimeOut;
				m_stDrive = DS_Error;
			}
			break;

		case DS_SetConfig:
			// Must set next state before call
			m_stDrive = DS_WaitConfig;
			if (!USBhost::SetConfiguration(this, m_bConfig))
				m_stDrive = DS_SetConfig;	// change state back
			break;

		case DS_TestReady:
			if (m_tmr.CheckDelay_ms(TestReadyIntervalMs))
			{
				m_stDrive = DS_WaitReady;
				TestUnitReady();
			}
			break;

		case DS_DeviceReady:
			m_stDrive = DS_Idle;
			return HOSTACT_FlashReady;
		}
		return HOSTACT_None;
	}

	//*********************************************************************
	// Instance data
	//*********************************************************************

protected:
	void	*m_pvTransfer;
	ulong	m_Tag;
	ushort	m_cbTransfer;
	Timer	m_tmr;
	byte	m_bInPipe;
	byte	m_bOutPipe;
	byte	m_bConfig;
	byte	m_stDrive;
	byte	m_stCommand;
	byte	m_cmdCur;
	byte	m_bPipeCur;
	sbyte	m_errCode;

	// Large buffers at end
	CmdBuffer		bufCommand;
	StatusBuffer	bufStatus;
};
