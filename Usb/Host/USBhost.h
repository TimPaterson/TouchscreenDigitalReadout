//****************************************************************************
// USBhost.h
//
// Created 9/8/2020 5:21:28 PM by Tim
//
//****************************************************************************

#pragma once

#include "SamUsbHost.h"
#include "..\UsbCtrl.h"
#include "UsbHostDriver.h"


#define DEFINE_USB_ISR()			void USB_Handler() { USBhost::UsbIsr(); }
#define USB_DRIVER_LIST(enm, ...)	\
	USBhost::DeviceDrivers_t const USBhost::s_DeviceDrivers = \
	{static_cast<EnumerationDriver *>(enm), {__VA_ARGS__}};

class EnumerationDriver;

#ifndef HOST_PIPE_COUNT
#define HOST_PIPE_COUNT 8
#endif

enum AutoZlpEnable
{
	ZLP_AutoOff = 0,
	ZLP_AutoOn = USB_DEVICE_PCKSIZE_AUTO_ZLP,
};

enum HostAction
{
	HOSTACT_None,
	HOSTACT_AddDevice,
	HOSTACT_RemoveDevice,
	HOSTACT_AddFailed,

	HOSTACT_DriverAction,	// Driver actions start here
	HOSTACT_MouseChange,
	HOSTACT_FlashReady,
	HOSTACT_KeyboardChange,
};

//*************************************************************************
// USBhost Class
//*************************************************************************

class USBhost : public UsbCtrl
{
	//*********************************************************************
	// Types
	//*********************************************************************

public:
	static constexpr uint FrameCountMask = 0x7FF;

private:
	static constexpr int DelayConnectToResetMs = 200;
	static constexpr int DelayResetToGetDescriptorMs = 200;

public:
	union ControlPacket
	{
		UsbSetupPacket	packet;
		uint64_t		u64;
	};

	struct DeviceDrivers_t
	{
		UsbHostDriver	*EnumDriver;
		UsbHostDriver	*arHostDriver[HOST_DRIVER_COUNT];
	};

protected:
	static constexpr int SetupBufSize = 256;

	// Tuck the pointer to the UsbHostDriver into unused fields
	// of Bank1 of the Pipe descriptor
	union PipeDescriptor
	{
		UsbHostDescBank		HostDescBank[2];
		struct
		{
			UsbHostDescBank	Bank0;
			ulong			Reserved[2];	// ADDR and PCKSIZE fields
			UsbHostDriver	*pDriver;
		};
	};

	union SetupBuffer_t
	{
		uint32_t		buf[SetupBufSize / sizeof(uint32_t)];
		UsbSetupPacket	packet;
		UsbDeviceDesc	descDev;
		UsbConfigDesc	descConfig;
		uint64_t		u64;
	};

	enum EnumState
	{
		ES_Idle,
		ES_Connected,
		ES_ResetComplete,
		ES_SetAddress,
		ES_DevDesc,
		ES_ConfigDesc,
		ES_FindDriver,
	};

	enum SetupState
	{
		SS_Idle,
		SS_CtrlRead,
		SS_CtrlWrite,
		SS_NoData,
		SS_GetStatus,	// after a write
		SS_SendStatus,	// after a read
		SS_WaitAck,		// Get/send final ACK
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	static void Init()
	{
		UsbCtrl::Init();
		USB->HOST.CTRLA.reg = USB_CTRLA_MODE_HOST | USB_CTRLA_ENABLE;
		// Enable Connect/Disconnect and Reset interrupts
		USB->HOST.INTENSET.reg = USB_HOST_INTENSET_DCONN |
			USB_HOST_INTENSET_DDISC | USB_HOST_INTENSET_RST;

		// Set pointer to pipe descriptor in RAM
		USB->HOST.DESCADD.reg = (uint32_t)&s_arPipeDesc;
	};

	static void Enable()
	{
		// Set VBUSOK
		USB->HOST.CTRLB.reg = USB_HOST_CTRLB_VBUSOK;
	}

	static void Disable()
	{
		Disconnect();

		// Turn off VBUSOK
		USB->HOST.CTRLB.reg = 0;
	}

	static int Process()
	{
		if (s_stEnum != ES_Idle)
		{
			if (s_stSetup == SS_Idle)
				ProcessEnum();
		}
		else
		{
			if (s_actHost != HOSTACT_None)
			{
				int act = s_actHost;
				s_actHost = HOSTACT_None;
				return act;
			}

			if (s_pActiveDriver != NULL)
				return s_pActiveDriver->Process();
		}

		return HOSTACT_None;
	}

	static UsbHostDriver *GetDriver()	{ return s_pActiveDriver; }

	static bool IsControlPipeReady()	{ return s_stSetup == SS_Idle; }

	static bool ControlTransfer(UsbHostDriver *pDriver, void *pv, uint64_t u64packet) NO_INLINE_ATTR
	{
		if (!IsControlPipeReady())
			return false;

		s_SetupBuffer.u64 = u64packet;
		StartSetup(pDriver, pv);
		return true;
	}

	static bool GetDescriptor(UsbHostDriver *pDriver, void *pv, ushort wValue, ushort wLength) NO_INLINE_ATTR
	{
		USBhost::ControlPacket	pkt;

		// Initialize Setup packet
		pkt.packet.bmRequestType =  USBRT_DirIn | USBRT_TypeStd | USBRT_RecipDevice;
		pkt.packet.bRequest = USBREQ_Get_Descriptor;
		pkt.packet.wValue = wValue;
		pkt.packet.wIndex = 0;
		pkt.packet.wLength = wLength;
		return ControlTransfer(pDriver, pv, pkt.u64);
	}

	static bool SetConfiguration(UsbHostDriver *pDriver, ushort wValue) NO_INLINE_ATTR
	{
		USBhost::ControlPacket	pkt;

		// Initialize Setup packet
		pkt.packet.bmRequestType =  USBRT_DirOut | USBRT_TypeStd | USBRT_RecipDevice;
		pkt.packet.bRequest = USBREQ_Set_Configuration;
		pkt.packet.wValue = wValue;
		pkt.packet.wIndex = 0;
		pkt.packet.wLength = 0;
		return ControlTransfer(pDriver, NULL, pkt.u64);
	}

	static int RequestPipe(UsbHostDriver *pDriver, UsbEndpointDesc *pEp) NO_INLINE_ATTR
	{
		int		i;
		uint	cfg;
		uint	interval;
		UsbHostPipe		*pPipe;
		PipeDescriptor	*pDesc;

		// Find a free pipe
		for (i = 1; i < HOST_PIPE_COUNT; i++)
		{
			pDesc = &s_arPipeDesc[i];
			pPipe = &USB->HOST.HostPipe[i];

			if (pDesc->pDriver == NULL)
			{
				pDesc->pDriver = pDriver;
				pDesc->Bank0.CTRL_PIPE.bit.PEPNUM = pEp->bEndpointAddr;
				pDesc->Bank0.CTRL_PIPE.bit.PDADDR = s_DeviceDrivers.EnumDriver->m_bAddr;
				pDesc->Bank0.PCKSIZE.reg = USB_DEVICE_PCKSIZE_SIZE(GetPacketSize(pEp->wMaxPacketSize));

				switch (pEp->bmAttributes)
				{
				case USBEP_Bulk:
					cfg = USB_HOST_PCFG_PTYPE_BULK;
					interval = 0;
					break;

				case USBEP_Interrupt:
					cfg = USB_HOST_PCFG_PTYPE_INTERRUPT;
					interval = pEp->bInterval;
					break;

				default:
					return -1;
				}

				cfg |= pEp->bEndpointAddr & USBEP_DirIn ? USB_HOST_PCFG_PTOKEN_IN : USB_HOST_PCFG_PTOKEN_OUT;
				pPipe->PCFG.reg = cfg;
				pPipe->BINTERVAL.reg = interval;
				pPipe->PINTENSET.reg = USB_HOST_PINTFLAG_TRCPT0 |
					USB_HOST_PINTFLAG_STALL | USB_HOST_PINTFLAG_PERR;
				return i;
			}
		}
		return -1;
	}

	static int EndpointFromPipe(int iPipe)
	{
		int		ep;

		ep = s_arPipeDesc[iPipe].Bank0.CTRL_PIPE.bit.PEPNUM;
		if (USB->HOST.HostPipe[iPipe].PCFG.bit.PTOKEN == USB_HOST_PCFG_PTOKEN_IN)
			ep |= USBRT_DirIn;
		return ep;
	}

	static void FreePipe(int iPipe)
	{
		USB->HOST.HostPipe[iPipe].PCFG.reg = USB_HOST_PCFG_PTYPE_DISABLE;
		s_arPipeDesc[iPipe].pDriver = NULL;
	}

	static void FreeAllPipes(UsbHostDriver *pDriver) NO_INLINE_ATTR
	{
		int		i;

		// Find any pipes assigned to this driver
		for (i = 1; i < HOST_PIPE_COUNT; i++)
		{
			if (s_arPipeDesc[i].pDriver == pDriver)
				FreePipe(i);
		}
	}

	static void SendData(int iPipe, void *pv, int cb, AutoZlpEnable zlp = ZLP_AutoOff) NO_INLINE_ATTR
	{
		PipeDescriptor	*pDesc;
		UsbHostPipe		*pPipe;

		pDesc = &s_arPipeDesc[iPipe];
		pPipe = &USB->HOST.HostPipe[iPipe];

		pDesc->Bank0.ADDR.reg = (ulong)pv;
		pDesc->Bank0.PCKSIZE.reg = USB_HOST_PCKSIZE_SIZE(pDesc->Bank0.PCKSIZE.bit.SIZE) |
			USB_HOST_PCKSIZE_BYTE_COUNT(cb) | zlp;

		// Set BK0RDY to indicate bank is full and ready to send
		pPipe->PSTATUSSET.reg = USB_HOST_PSTATUSSET_BK0RDY;
		pPipe->PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE;
	}

	static void ReceiveData(int iPipe, void *pv, int cb) NO_INLINE_ATTR
	{
		PipeDescriptor	*pDesc;
		UsbHostPipe		*pPipe;

		pDesc = &s_arPipeDesc[iPipe];
		pPipe = &USB->HOST.HostPipe[iPipe];

		pDesc->Bank0.ADDR.reg = (ulong)pv;
		pDesc->Bank0.PCKSIZE.reg = USB_HOST_PCKSIZE_SIZE(pDesc->Bank0.PCKSIZE.bit.SIZE) |
			USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(cb);

		// Clear BK0RDY to indicate bank is empty and ready to receive
		pPipe->PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE | USB_HOST_PSTATUSSET_BK0RDY;
	}

	static uint GetFrameNumber()
	{
		return USB->HOST.FNUM.bit.FNUM;
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	static void StartSetup(UsbHostDriver *pDriver, void *pv)
	{
		s_arPipeDesc[0].Bank0.ADDR.reg = (uint32_t)&s_SetupBuffer;
		s_arPipeDesc[0].Bank0.PCKSIZE.reg =
			USB_HOST_PCKSIZE_BYTE_COUNT(sizeof(s_SetupBuffer.packet)) | USB_HOST_PCKSIZE_SIZE(pDriver->m_PackSize);
		s_arPipeDesc[0].Bank0.CTRL_PIPE.reg = pDriver->m_bAddr;
		s_arPipeDesc[0].pDriver = pDriver;
		s_pvSetupData = pv;

		if (s_SetupBuffer.packet.Dir == USBRT_DirIn_Val)
			s_stSetup = SS_CtrlRead;
		else if (s_SetupBuffer.packet.wLength == 0)
			s_stSetup = SS_NoData;
		else
			s_stSetup = SS_CtrlWrite;

		USB->HOST.HostPipe[0].PCFG.reg = USB_HOST_PCFG_PTYPE_CONTROL |
			USB_HOST_PCFG_PTOKEN_SETUP;
		USB->HOST.HostPipe[0].PINTENSET.reg = USB_HOST_PINTFLAG_TXSTP |
			USB_HOST_PINTFLAG_STALL | USB_HOST_PINTFLAG_PERR;
		// Send the packet, making sure data toggle is zero
		USB->HOST.HostPipe[0].PSTATUSSET.reg = USB_HOST_PSTATUSSET_BK0RDY;
		USB->HOST.HostPipe[0].PSTATUSCLR.reg = USB_HOST_PSTATUSSET_DTGL;
		USB->HOST.HostPipe[0].PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE;
	}

	static void ProcessEnum() NO_INLINE_ATTR
	{
		UsbHostDriver	*pDriver;
		ControlPacket	pkt;

		pDriver = s_DeviceDrivers.EnumDriver;

		switch (s_stEnum)
		{
		case ES_Connected:
			if (!s_tmrEnum.CheckDelay_ms(DelayConnectToResetMs))
				return;

			// Device is connected, reset it
			USB->HOST.CTRLB.reg	 = USB_HOST_CTRLB_VBUSOK | USB_HOST_CTRLB_BUSRESET;
			s_stEnum = ES_Idle;
			break;

		case ES_ResetComplete:
			if (!s_tmrEnum.CheckDelay_ms(DelayResetToGetDescriptorMs))
				return;

			// Initialize Pipe 0 for control
			pDriver->m_bAddr = 0;	// default address
			pDriver->m_PackSize = USB_HOST_PCKSIZE_SIZE_8_Val;
			// Get 8 bytes of the device descriptor
			GetDescriptor(pDriver, &s_SetupBuffer, USBVAL_Type(USBDESC_Device), 8);
			break;

		case ES_SetAddress:
			pkt.packet.bmRequestType = USBRT_DirOut | USBRT_TypeStd | USBRT_RecipDevice;
			pkt.packet.bRequest = USBREQ_Set_Address;
			pkt.packet.wValue = 1;	// UNDONE: device address
			pkt.packet.wIndex = 0;
			pkt.packet.wLength = 0;
			ControlTransfer(pDriver, NULL, pkt.u64);
			break;

		case ES_DevDesc:
			GetDescriptor(pDriver, &s_SetupBuffer, USBVAL_Type(USBDESC_Device), sizeof(UsbDeviceDesc));
			break;

		case ES_ConfigDesc:
			GetDescriptor(pDriver, &s_SetupBuffer, USBVAL_Type(USBDESC_Config), sizeof s_SetupBuffer - 1);
			break;
		}
	}

	// This function is called from the Interrupt Service Routine.
	// It sets enumeration state to the next step for ProcessEnum().
	//
	static void SetupComplete(uint cbTransfer)
	{
		int		cb;
		UsbHostDriver *pDriver;

		pDriver = s_DeviceDrivers.EnumDriver;

		switch (s_stEnum)
		{
		case ES_ResetComplete:
			if (cbTransfer > offsetof(UsbDeviceDesc, bMaxPacketSize0))
			{
				cb = s_SetupBuffer.descDev.bMaxPacketSize0;
				// Make sure it's a power of two
				if ((cb & (cb - 1)) != 0 || cb < 8 || cb > 64)
					cb = 8;	// just use smallest packet size
				pDriver->m_PackSize = GetPacketSize(cb);
				s_stEnum = ES_SetAddress;
			}
			break;

		case ES_SetAddress:
			pDriver->m_bAddr = s_SetupBuffer.packet.wValue;	// Set the address
			s_stEnum = ES_DevDesc;
			break;

		case ES_DevDesc:
			s_devVidPid = (s_SetupBuffer.descDev.idProduct << 16) | s_SetupBuffer.descDev.idVendor;
			s_devClass = (s_SetupBuffer.descDev.bDeviceProtocol << 16) |
				(s_SetupBuffer.descDev.bDeviceSubclass << 8) | s_SetupBuffer.descDev.bDeviceClass;
			s_stEnum = ES_ConfigDesc;
			break;

		case ES_ConfigDesc:
			s_stEnum = ES_Idle;

			for (int i = 0; i < HOST_DRIVER_COUNT; i++)
			{
				UsbHostDriver	*pDriver;

				pDriver = s_DeviceDrivers.arHostDriver[i]->IsDriverForDevice(s_devVidPid, s_devClass, &s_SetupBuffer.descConfig);

				if (pDriver != NULL)
				{
					s_pActiveDriver = pDriver;
					pDriver->m_bAddr = s_DeviceDrivers.EnumDriver->m_bAddr;
					pDriver->m_PackSize = s_DeviceDrivers.EnumDriver->m_PackSize;
					pDriver->m_fDriverLoaded = true;
					s_actHost = HOSTACT_AddDevice;
					return;
				}
			}
			s_DeviceDrivers.EnumDriver->TransferError(0, TEC_NoDriver);
			s_actHost = HOSTACT_AddFailed;
			break;
		}
	}

	static PipePacketSize GetPacketSize(uint cb)
	{
		uint	uSize;

		// Make sure it's a power of two
		if ((cb & (cb - 1)) != 0 || cb < 8 || cb > 64)
			return USB_HOST_PCKSIZE_SIZE_8_Val;	// just use smallest packet size

		if (cb < 32)
			uSize = cb / 16;
		else
			uSize = cb / 32 + 1;

		return (PipePacketSize)uSize;
	};

	static void Disconnect()
	{
		// Free all pipes
		for (int i = 1; i < HOST_PIPE_COUNT; i++)
			FreePipe(i);

		// Free the driver
		if (s_pActiveDriver != NULL)
		{
			s_pActiveDriver->m_fDriverLoaded = false;
			s_pActiveDriver = NULL;
			s_actHost = HOSTACT_RemoveDevice;
		}
		// Device is disconnected
		s_stEnum = ES_Idle;
		s_stSetup = SS_Idle;
	}

	//*********************************************************************
	// Interrupt service routine
	//*********************************************************************

public:
	static void UsbIsr() INLINE_ATTR
	{
		int		iPipe;
		int		intFlags;
		int		intEnFlags;
		int		pipeIntSummary;
		UsbHostPipe		*pPipe;
		PipeDescriptor	*pDesc;
		UsbHostDriver	*pDriver;

		//*****************************************************************
		// Handle host-level interrupts

		intFlags = USB->HOST.INTFLAG.reg;
		if (intFlags & USB_HOST_INTFLAG_RST)
		{
			// Clear the interrupt flag
			USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RST;

			s_stEnum = ES_ResetComplete;
			s_tmrEnum.Start();
		}

		if (intFlags & USB_HOST_INTFLAG_DCONN)
		{
			// Clear the interrupt flag
			USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DCONN;

			s_stEnum = ES_Connected;
			s_tmrEnum.Start();
		}

		if (intFlags & USB_HOST_INTFLAG_DDISC)
		{
			// Clear the interrupt flag
			USB->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DDISC;

			Disconnect();
		}

		// Check for other host-level flags here

		//*****************************************************************
		// Handle pipe-level interrupts

		pipeIntSummary = USB->HOST.PINTSMRY.reg;

		for (iPipe = 0; pipeIntSummary != 0; iPipe++, pipeIntSummary >>= 1)
		{
			if ((pipeIntSummary & 1) == 0)
				continue;

			pPipe = &USB->HOST.HostPipe[iPipe];
			pDesc = &s_arPipeDesc[iPipe];
			pDriver = pDesc->pDriver;
			intFlags = pPipe->PINTFLAG.reg;
			intEnFlags = pPipe->PINTENSET.reg;

			s_arPipeDesc[0].Bank0.ADDR.reg = (uint32_t)&s_SetupBuffer;

			if (iPipe == 0)
			{
				int		cb;

				// Control pipe
				if (intFlags & USB_HOST_PINTFLAG_TXSTP)
				{
					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_TXSTP;
					pDesc->Bank0.ADDR.reg = (ulong)s_pvSetupData;
					cb = s_SetupBuffer.packet.wLength;

					switch (s_stSetup)
					{
					case SS_NoData:
						s_cbTransfer = 0;
						goto GetStatus;

					case SS_CtrlRead:
						pPipe->PCFG.reg = USB_HOST_PCFG_PTYPE_CONTROL |
							USB_HOST_PCFG_PTOKEN_IN;
						pDesc->Bank0.PCKSIZE.reg =
							USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(cb) |
							USB_DEVICE_PCKSIZE_SIZE(pDriver->m_PackSize);
						// BK0RDY is zero, indicating bank is empty and ready to receive
						s_stSetup = SS_SendStatus;
						break;

					case SS_CtrlWrite:
						pPipe->PCFG.reg = USB_HOST_PCFG_PTYPE_CONTROL |
							USB_HOST_PCFG_PTOKEN_OUT;
						pDesc->Bank0.PCKSIZE.reg =
							USB_DEVICE_PCKSIZE_BYTE_COUNT(cb) |
							USB_DEVICE_PCKSIZE_SIZE(pDriver->m_PackSize) |
							USB_DEVICE_PCKSIZE_AUTO_ZLP;
						// Set BK0RDY to indicate bank is full and ready to send
						pPipe->PSTATUSSET.reg = USB_HOST_PSTATUSSET_BK0RDY;
						s_stSetup = SS_GetStatus;
						break;
					};

					pPipe->PINTENSET.reg = USB_HOST_PINTFLAG_TRCPT0 |
						USB_HOST_PINTFLAG_STALL | USB_HOST_PINTFLAG_PERR;
					pPipe->PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE;
				}

				if (intFlags & USB_HOST_PINTFLAG_TRCPT0)
				{
					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT0;

					switch (s_stSetup)
					{
					case SS_GetStatus:
						s_cbTransfer = pDesc->Bank0.PCKSIZE.bit.MULTI_PACKET_SIZE;
GetStatus:
						pPipe->PCFG.reg = USB_HOST_PCFG_PTYPE_CONTROL |
							USB_HOST_PCFG_PTOKEN_IN;
						pDesc->Bank0.PCKSIZE.reg =
							USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(0) |
							USB_DEVICE_PCKSIZE_SIZE(pDriver->m_PackSize);
						// BK0RDY is zero, indicating bank is empty and ready to receive
						// Data toggle is 1 for IN Setup
						pPipe->PSTATUSSET.reg = USB_HOST_PSTATUSSET_DTGL;
						break;

					case SS_SendStatus:
						s_cbTransfer = pDesc->Bank0.PCKSIZE.bit.BYTE_COUNT;
						pPipe->PCFG.reg = USB_HOST_PCFG_PTYPE_CONTROL |
							USB_HOST_PCFG_PTOKEN_OUT;
						pDesc->Bank0.PCKSIZE.reg =
							USB_DEVICE_PCKSIZE_BYTE_COUNT(0) |
							USB_DEVICE_PCKSIZE_SIZE(pDriver->m_PackSize) |
							USB_DEVICE_PCKSIZE_AUTO_ZLP;
						// Set BK0RDY to indicate bank is full and ready to send
						// Data toggle is 1 for OUT Setup
						pPipe->PSTATUSSET.reg = USB_HOST_PSTATUSSET_BK0RDY | USB_HOST_PSTATUSSET_DTGL;
						break;

					case SS_WaitAck:
						pDriver->SetupTransactionComplete(s_cbTransfer);
						s_stSetup = SS_Idle;
						return;
					}

					s_stSetup = SS_WaitAck;
					// Send or receive final ACK (or error)
					pPipe->PINTENSET.reg = USB_HOST_PINTFLAG_TRCPT0 |
						USB_HOST_PINTFLAG_STALL | USB_HOST_PINTFLAG_PERR;
					pPipe->PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE;
				}

				if (intFlags & USB_HOST_PINTFLAG_STALL)
				{
					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_STALL;
					DEBUG_PRINT("Control STALL received\n");

					pDriver->TransferError(iPipe, TEC_Stall);
					s_stSetup = SS_Idle;
				}

				if (intFlags & USB_HOST_PINTFLAG_PERR)
				{
					byte	status;

					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_PERR;

					status = pDesc->Bank0.STATUS_PIPE.reg;
					pDesc->Bank0.STATUS_PIPE.reg = 0;
					if (status & USB_HOST_STATUS_PIPE_TOUTER)
						DEBUG_PRINT("Timeout\n");
					else
					{
						DEBUG_PRINT("Pipe error: %02X\n", status);
					}
					pDriver->TransferError(iPipe, TEC_Error);

					if (s_stEnum == ES_ResetComplete)
					{
						// Try reset again
						s_stEnum = ES_Connected;
						s_tmrEnum.Start();
					}
					s_stSetup = SS_Idle;
				}
			}
			else
			{
				// Not the control pipe
				if ((intFlags & USB_HOST_PINTFLAG_TRCPT0) &&
					(intEnFlags && USB_HOST_PINTFLAG_TRCPT0))
				{
					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT0;

					if (pPipe->PCFG.bit.PTOKEN == USB_HOST_PCFG_PTOKEN_OUT_Val)
						s_cbTransfer = pDesc->Bank0.PCKSIZE.bit.MULTI_PACKET_SIZE;
					else
						s_cbTransfer = pDesc->Bank0.PCKSIZE.bit.BYTE_COUNT;

					pDriver->TransferComplete(iPipe, s_cbTransfer);
				}

				if (intFlags & USB_HOST_PINTFLAG_STALL)
				{
					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_STALL;
					DEBUG_PRINT("STALL received on pipe %i\n", iPipe);

					pDriver->TransferError(iPipe, TEC_Stall);
				}

				if (intFlags & USB_HOST_PINTFLAG_PERR)
				{
					byte	status;

					// Clear the interrupt flag
					pPipe->PINTFLAG.reg = USB_HOST_PINTFLAG_PERR;

					status = pDesc->Bank0.STATUS_PIPE.reg;
					pDesc->Bank0.STATUS_PIPE.reg = 0;
					if (status & USB_HOST_STATUS_PIPE_TOUTER)
						DEBUG_PRINT("Timeout on pipe %i\n", iPipe);
					else
						DEBUG_PRINT("Error %02X on pipe %i\n", status, iPipe);

					pDriver->TransferError(iPipe, TEC_Error);
				}
			}
		}
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

	// Note this is NOT inline. It is defined in a code file using
	// the USB_DRIVER_LIST() macro.
	static const DeviceDrivers_t s_DeviceDrivers;

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

protected:
	inline static SetupBuffer_t s_SetupBuffer;
	inline static PipeDescriptor s_arPipeDesc[HOST_PIPE_COUNT];
	inline static UsbHostDriver *s_pActiveDriver;
	inline static void *s_pvSetupData;
	inline static ulong s_devVidPid;
	inline static ulong s_devClass;
	inline static int s_cbTransfer;
	inline static Timer s_tmrEnum;
	inline static byte s_stEnum;
	inline static byte s_stSetup;
	inline static byte s_actHost;		// HostAction enumeration

	//*********************************************************************
	// Allow enumeration driver to access stuff

	friend EnumerationDriver;
};


//****************************************************************************
// UsbHostDriver used during enumeration
//****************************************************************************

class EnumerationDriver : public UsbHostDriver
{
	virtual UsbHostDriver *IsDriverForDevice(ulong ulVidPid, ulong ulClass, UsbConfigDesc *pConfig)
	{
		return NULL;
	}

	virtual void SetupTransactionComplete(int cbTransfer)
	{
		USBhost::SetupComplete(cbTransfer);
	}

	virtual void TransferComplete(int iPipe, int cbTransfer)
	{
	}

	virtual void TransferError(int iPipe, TransferErrorCode err)
	{
		switch (err)
		{
		case TEC_NoDriver:
			DEBUG_PRINT("No driver selected\n");
			break;

		case TEC_Stall:
			// Reported by host
			break;

		case TEC_Error:
			DEBUG_PRINT("Transfer error during enumeration\n");
			break;

		case  TEC_None:
			break;
		}
	}

	virtual int Process()
	{
		return HOSTACT_None;
	}
};
