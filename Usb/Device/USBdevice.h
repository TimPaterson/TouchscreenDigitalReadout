//****************************************************************************
// Class USBdevice
// USBdevice.h
//
// Created 8/17/2020 5:16:10 PM by Tim
//
//****************************************************************************

#pragma once

#include "..\UsbCtrl.h"
#include "SamUsbDevice.h"


#define DEFINE_USB_ISR(dev)	void USB_Handler() { dev::ISR.UsbIsr(); }

// Set default version if not explicit
#ifndef USB_DEV_UsbVer
#define USB_DEV_UsbVer USB_VER_2p0
#endif

#ifndef MAX_USB_DESCRIPTOR_SIZE
#define MAX_USB_DESCRIPTOR_SIZE	0
#endif

//*************************************************************************
// Compute the max USB endpoint number
// Get the max endpoint nubmer by opening a call to the max() function
// for each endpoint declaration. It would look like this:
//   max(ep, max(ep, max(ep, ...
// Close up the functions with a zero and one ")" per endpoint.

constexpr int max(int a, int b) { return a > b ? a : b; }

#define ENDPOINT_IN(ep, trans, size, interval) max(ep,
#define ENDPOINT_OUT(ep, trans, size, interval) max(ep,

static constexpr int MaxEndpointNumber = ENDPOINT_LIST 0

#undef ENDPOINT_IN
#undef ENDPOINT_OUT
#define ENDPOINT_IN(ep, trans, size, interval) )
#define ENDPOINT_OUT(ep, trans, size, interval) )

ENDPOINT_LIST;

#undef ENDPOINT_IN
#undef ENDPOINT_OUT

//*************************************************************************
// USBdeviceBase Class
//*************************************************************************

class USBdeviceBase : public UsbCtrl
{
	//*********************************************************************
	// Local Types
	//*********************************************************************

	//*************************************************************************
	// Count number of endpoints

	#define ENDPOINT_IN(ep, trans, size, interval)	EPINCNT_##ep,
	#define ENDPOINT_OUT(ep, trans, size, interval)	EPOUTCNT_##ep,

	enum EndpointCountList
	{
		ENDPOINT_LIST
		EndpointCount	// reflects no. of elements in ENDPOINT_LIST
	};
	#undef ENDPOINT_IN
	#undef ENDPOINT_OUT

	//*************************************************************************
	// Declare the entire configuration description & its children

	struct UsbConfig
	{
		UsbConfigDesc		config;
		UsbInterfaceDesc	iface;
		UsbEndpointDesc		arEp[EndpointCount];
	};

	//*************************************************************************
	// Standardize some string descriptor indices

	enum StringDescIndex
	{
		USBSTR_LangId,
		USBSTR_Vendor,
		USBSTR_Product,
#ifdef USB_DEV_SerialNo
		USBSTR_SerialNo
#endif
	};

	//*************************************************************************
	// Values to set a STALL on and endpoint.
	// Or them together to set both directions.

	enum StallDir
	{
		In = USB_DEVICE_EPSTATUSSET_STALLRQ1,
		Out = USB_DEVICE_EPSTATUSSET_STALLRQ0
	};

	//*************************************************************************
	// Internal structure

	struct EndpointSize
	{
		byte	Bank0;
		byte	Bank1;
	};

	enum AutoZlpEnable
	{
		ZLP_AutoOff = 0,
		ZLP_AutoOn = USB_DEVICE_PCKSIZE_AUTO_ZLP,
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	static void Init()
	{
		UsbCtrl::Init();
		USB->DEVICE.CTRLA.reg = USB_CTRLA_MODE_DEVICE | USB_CTRLA_ENABLE;
		USB->DEVICE.DeviceEndpoint[0].EPCFG.reg =
			USB_DEVICE_EPCFG_EPTYPE0_CONTROL | USB_DEVICE_EPCFG_EPTYPE1_CONTROL;
#ifdef USB_SOF_INT
		USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST |
			USB_DEVICE_INTENSET_SOF;
#else
		USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
#endif

		USB->DEVICE.DESCADD.reg = (uint32_t)&EndpointDesc;
		EndpointDesc[0].DeviceDescBank[0].ADDR.reg = GetSetupBufInt();
		EndpointDesc[0].DeviceDescBank[0].PCKSIZE.reg =
			CONCAT(USB_DEVICE_PCKSIZE_SIZE_, USB_ENDPOINT0_SIZE);
		arEpSize[0].Bank0 = CAT3(USB_DEVICE_PCKSIZE_SIZE_, USB_ENDPOINT0_SIZE, _Val);
		arEpSize[0].Bank1 = CAT3(USB_DEVICE_PCKSIZE_SIZE_, USB_ENDPOINT0_SIZE, _Val);

		// Create tables of initialization values for additional endpoints
		#define Bulk		USB_DEVICE_EPCFG_EPTYPE_BULK_Val
		#define Interrupt	USB_DEVICE_EPCFG_EPTYPE_INTERRUPT_Val
		#define ENDPOINT_IN(ep, trans, size, interval)	\
			arEpSize[ep].Bank1 = CAT3(USB_DEVICE_PCKSIZE_SIZE_, size, _Val);
		#define ENDPOINT_OUT(ep, trans, size, interval)	\
			arEpSize[ep].Bank0 = CAT3(USB_DEVICE_PCKSIZE_SIZE_, size, _Val);

		ENDPOINT_LIST

		#undef ENDPOINT_IN
		#undef ENDPOINT_OUT
		#undef Bulk
		#undef Interrupt
	};

	static void Attach()
	{
		USB->DEVICE.CTRLB.reg = USB_DEVICE_CTRLB_SPDCONF_FS;
	};

	static void Detach()
	{
		USB->DEVICE.CTRLB.reg = USB_DEVICE_CTRLB_DETACH;
	};

	//*********************************************************************
	// Helpers for transactions
	//*********************************************************************

protected:
	static UsbSetupPacket *GetSetupBuf()	{ return (UsbSetupPacket *)arSetupBuf; }
	static uint32_t GetSetupBufInt()		{ return (uint32_t)arSetupBuf; }

	static void AckControlPacket()			{ SendToHost(0, NULL, 0); }

	static void SendControlPacket(int cb) NO_INLINE_ATTR
	{
		// Tell USB we're ready to receive handshake packet on Bank 0
		USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK0RDY;
		// Tell USB to send data on Bank 1
		SendToHostZlp(0, GetSetupBuf(), cb);
	}

	// The first call to GetSetupData() enables receiving the OUT data
	// packet from the host, and returns NULL to indicate data not ready.
	// Once the data has been received, the setup process is repeated
	// and the second call returns the pointer to the data. A call to
	// GetSetupDataLength() is needed to get size of received packet.
	//
	static byte *GetSetupData() NO_INLINE_ATTR
	{
		byte	*pb;
		UsbDeviceDescBank	*pBank;

		pBank = &EndpointDesc[0].DeviceDescBank[0];
		pb = (byte *)(GetSetupBuf() + 1);
		if ((byte *)pBank->ADDR.reg != pb)
		{
			// We need to get the data from the host
			//
			// Enable the transfer complete interrupt so we can restart the
			// setup request.
			// Clear flag first
			USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
			USB->DEVICE.DeviceEndpoint[0].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0;

			ReceiveFromHost(0, pb, SetupBufSize - sizeof(UsbSetupPacket));

			// UNDONE: Must also be ready to ACK an IN packet, which indicates abort
			return NULL;
		}
		return pb;	// already fetched the data
	}

	static int GetSetupDataLength()
	{
		return EndpointDesc[0].DeviceDescBank[0].PCKSIZE.bit.BYTE_COUNT;
	}

	static void SendToHost(int iEp, void *pv, int cb, AutoZlpEnable zlp) NO_INLINE_ATTR
	{
		UsbDeviceDescBank	*pBank;

		if (((int)pv & 0x3) != 0 || cb > MaxUsbTransferSize)
			DEBUG_PRINT("SendToHost invalid\n");
		pBank = &EndpointDesc[iEp].DeviceDescBank[1];
		pBank->ADDR.reg = (uint32_t)pv;
		pBank->PCKSIZE.reg = USB_DEVICE_PCKSIZE_BYTE_COUNT(cb) |
			USB_DEVICE_PCKSIZE_SIZE(arEpSize[iEp].Bank1) | zlp;
		USB->DEVICE.DeviceEndpoint[iEp].EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK1RDY;
	}

	static void SendToHost(int iEp, void *pv, int cb) NO_INLINE_ATTR
	{
		SendToHost(iEp, pv, cb, ZLP_AutoOff);
	}

	static void SendToHostZlp(int iEp, void *pv, int cb) NO_INLINE_ATTR
	{
		SendToHost(iEp, pv, cb, ZLP_AutoOn);
	}

	static void ReceiveFromHost(int iEp, void *pv, int cb) NO_INLINE_ATTR
	{
		UsbDeviceDescBank	*pBank;

		if (((int)pv & 0x3) != 0 || cb > MaxUsbTransferSize)
			DEBUG_PRINT("ReceiveFromHost invalid\n");
		pBank = &EndpointDesc[iEp].DeviceDescBank[0];
		pBank->ADDR.reg = (uint32_t)pv;
		pBank->PCKSIZE.reg = USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(cb) |
			USB_DEVICE_PCKSIZE_SIZE(arEpSize[iEp].Bank0);
		USB->DEVICE.DeviceEndpoint[iEp].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK0RDY;
	}

	static void SetStall(int iEp, StallDir iStallDir)
	{
		USB->DEVICE.DeviceEndpoint[iEp].EPSTATUSSET.reg = iStallDir;
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

protected:
	//*********************************************************************
	// Device Descriptor

	inline const static UsbDeviceDesc DeviceDesc =
	{
		sizeof (UsbDeviceDesc),
		USBDESC_Device,
		USB_DEV_UsbVer,
		USB_DEV_Class,
		USB_DEV_Subclass,
		USB_DEV_Protocol,
		USB_ENDPOINT0_SIZE,
		USB_DEV_Vendor,
		USB_DEV_Product,
		USB_DEV_Device,
		USBSTR_Vendor,
		USBSTR_Product,
	#ifdef USB_DEV_SerialNo
		USBSTR_SerialNo,
	#else
		0,
	#endif	// serial number
		USB_DEV_NumConfigs
	};

	//*********************************************************************
	// Configuration Descriptor

#define ENDPOINT_IN(ep, trans, size, interval) \
{								\
	sizeof(UsbEndpointDesc),	\
	USBDESC_Endpoint,			\
	USBEP_DirIn | ep,			\
	USBEP_##trans,				\
	size,						\
	interval					\
},

#define ENDPOINT_OUT(ep, trans, size, interval) \
{								\
	sizeof(UsbEndpointDesc),	\
	USBDESC_Endpoint,			\
	USBEP_DirOut | ep,			\
	USBEP_##trans,				\
	size,						\
	interval					\
},

	inline const static UsbConfig ConfigDesc =
	{
		{
			sizeof(UsbConfigDesc),
			USBDESC_Config,
			sizeof(UsbConfig),
			1,	// number of interfaces
			1,	// config value
			0,	// config string
			USBCFG_BaseAttributes | USB_CFG_Attr,
			USB_CFG_MaxPower
		},
		{
			sizeof(UsbInterfaceDesc),
			USBDESC_Interface,
			0,	// interface no.
			0,	// alt
			EndpointCount,	// count of endpoints
			USB_IFC_Class,
			USB_IFC_Subclass,
			USB_IFC_Protocol,
			0	// string
		},
		{
			ENDPOINT_LIST
		}
	};

#undef ENDPOINT_IN
#undef ENDPOINT_OUT

	//****************************************************************************
	// String descriptors

#define STRING16(str)	CONCAT(u, str)	// Give string literal 16-bit characters

	// Descriptor 0 had Lang ID
	inline const static UsbStringDesc LangId =
	{
		{sizeof(UsbStringDescHead) + sizeof(ushort), USBDESC_String},
		{(char16_t)0x0409}	// English only
	};

	// Define string descriptors
	#define DEF_STR_DESC(name, string)			\
	inline const static UsbStringDesc name =		\
	{											\
		{sizeof(UsbStringDescHead) + sizeof(STRING16(string)) - sizeof(char16_t), \
			USBDESC_String},					\
		STRING16(string)						\
	}

	DEF_STR_DESC(VendorStr, USB_DEV_VendorStr);
	DEF_STR_DESC(ProductStr, USB_DEV_ProductStr);

	// Build table of pointers to string descriptors
	inline const static UsbStringDesc* const arpStrDesc[] =
	{
		&LangId,
		&VendorStr,
		&ProductStr,
	};

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

protected:

	// Get the max endpoint size by opening a call to the max() function
	// for each endpoint declaration. It would look like this:
	//   max(size, max(size, max(size, ...
	// then make the size of the Configuration Descriptor the last argument.
	// Close up the functions with one ")" per endpoint.
#define ENDPOINT_IN(ep, trans, size, interval) max(size,
#define ENDPOINT_OUT(ep, trans, size, interval) max(size,

	static constexpr int SetupBufSize = ENDPOINT_LIST 
		max(sizeof(UsbConfig), MAX_USB_DESCRIPTOR_SIZE)

#undef ENDPOINT_IN
#undef ENDPOINT_OUT
#define ENDPOINT_IN(ep, trans, size, interval) )
#define ENDPOINT_OUT(ep, trans, size, interval) )

	ENDPOINT_LIST;

#undef ENDPOINT_IN
#undef ENDPOINT_OUT

private:
	// Buffer for all Setup transactions - must be 32-bit aligned
	inline static uint32_t arSetupBuf[SetupBufSize / sizeof(uint32_t)];

	// Endpoint descriptors for MCU
	inline static UsbDeviceDescriptor EndpointDesc[MaxEndpointNumber + 1];

	// Packet size for each endpoint
	inline static EndpointSize arEpSize[MaxEndpointNumber + 1];
};


//*************************************************************************
// USBdeviceIsr template class
//
// Interrupt service routine for USBdevice. Passes callbacks to template
// parameter class T.
//
//*************************************************************************

template<class T>
class USBdeviceIsr : public USBdeviceBase
{

	//*********************************************************************
	// These are callbacks implemented in class T, the template argument
	//*********************************************************************

	/*
protected:
	static void DeviceConfigured();
	static void RxData(int iEp, void *pv, int cb);
	static void TxDataRequest(int iEp);
	static void TxDataSent(int iEp, int cb);
	static bool NonStandardSetup(UsbSetupPacket *pSetup);
	static const void *NonStandardString(int index);
#ifdef USB_DEV_SerialNo
	static const UsbStringDesc *GetSerialStrDesc();
#endif
#ifdef USB_SOF_INT
	static void StartOfFrame();
#endif
	*/

	//*********************************************************************
	// Interrupt service routine
	//*********************************************************************

public:
	static void UsbIsr() INLINE_ATTR
	{
		int		iEp;
		int		epIntSummary;
		int		intFlags;
		int		intEnFlags;
		UsbDeviceEndpoint	*pEp;
		UsbDeviceDescBank	*pBank;

		//*****************************************************************
		// Handle device-level interrupts

		intFlags = USB->DEVICE.INTFLAG.reg;
		if (intFlags & USB_DEVICE_INTFLAG_EORST)
		{
			// Clear the interrupt flag
			USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;

			// Interrupts are disabled by reset
			USB->DEVICE.DeviceEndpoint[0].EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;

			//*************************************************************
			// Use ENDPOINT_LIST to generate code to initialize each endpoint

			#define Bulk		BULK
			#define Interrupt	INTERRUPT

			// Code to initialize IN endpoint:
			// Set the configuration register
			// Enable Transfer Complete and Transfer Fail interrupts
			#define ENDPOINT_IN(ep, trans, size, interval) \
				pEp = &USB->DEVICE.DeviceEndpoint[ep];						\
				pEp->EPCFG.reg |= CONCAT(USB_DEVICE_EPCFG_EPTYPE1_, trans); \
				pEp->EPINTENSET.reg |= USB_DEVICE_EPINTENSET_TRCPT1 |		\
					USB_DEVICE_EPINTENSET_TRFAIL1;

			// Code to initialize OUT endpoint:
			// Set the configuration register
			// Set the BK0RDY bit so it will not be ready to receive
			// Enable Transfer Complete interrupt
			#define ENDPOINT_OUT(ep, trans, size, interval)	\
				pEp = &USB->DEVICE.DeviceEndpoint[ep];						\
				pEp->EPCFG.reg |= CONCAT(USB_DEVICE_EPCFG_EPTYPE0_, trans); \
				pEp->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK0RDY;		\
				pEp->EPINTENSET.reg |= USB_DEVICE_EPINTENSET_TRCPT0;

			ENDPOINT_LIST

			#undef ENDPOINT_IN
			#undef ENDPOINT_OUT
			#undef Bulk
			#undef Interrupt

		}

#ifdef USB_SOF_INT
		if (intFlags & USB_DEVICE_INTFLAG_SOF)
		{
			// Clear the interrupt flag
			USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_SOF;

			T::StartOfFrame();
		}
#endif

		// Check for other device-level flags here

		//*****************************************************************
		// Handle endpoint-level interrupts

		epIntSummary = USB->DEVICE.EPINTSMRY.reg;

		for (iEp = 0; epIntSummary != 0; iEp++, epIntSummary >>= 1)
		{
			if ((epIntSummary & 1) == 0)
				continue;

			pEp = &USB->DEVICE.DeviceEndpoint[iEp];
			intFlags = pEp->EPINTFLAG.reg;
			intEnFlags = pEp->EPINTENSET.reg;

			if (iEp == 0)
			{
				// Handle control endpoint

				if (intFlags & USB_DEVICE_EPINTFLAG_RXSTP)
				{
					// Clear the interrupt flag
					pEp->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;

					// setup request
					ProcessSetup();
				}
				else if ((intFlags & USB_DEVICE_EPINTFLAG_TRCPT0) &&
					(intEnFlags & USB_DEVICE_EPINTFLAG_TRCPT0))
				{
					// Stop responding to transfer complete
					pEp->EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT0;

					// Restore setup data pointer
					EndpointDesc[0].DeviceDescBank[0].ADDR.reg = GetSetupBufInt();

					// We've received setup data, continue processing
					ProcessSetup();
				}
				else if ((intFlags & USB_DEVICE_EPINTFLAG_TRCPT1) &&
					(intEnFlags & USB_DEVICE_EPINTFLAG_TRCPT1))
				{
					// Stop responding to transfer complete
					pEp->EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT1;

					// Enable device address
					USB->DEVICE.DADD.reg |= USB_DEVICE_DADD_ADDEN;
				}
			}
			else
			{
				// Not the control endpoint

				if (intFlags & USB_DEVICE_EPINTFLAG_TRCPT0)
				{
					// OUT endpoint completed
					//
					// Clear the interrupt flag
					pEp->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;

					pBank = &EndpointDesc[iEp].DeviceDescBank[0];
					T::RxData(iEp, (void *)pBank->ADDR.reg, pBank->PCKSIZE.bit.BYTE_COUNT);
				}

				if (intFlags & USB_DEVICE_EPINTFLAG_TRCPT1)
				{
					// IN endpoint completed
					//
					// Clear the interrupt flag
					pEp->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;

					pBank = &EndpointDesc[iEp].DeviceDescBank[1];
					T::TxDataSent(iEp, pBank->PCKSIZE.bit.BYTE_COUNT);
				}

				if (intFlags & USB_DEVICE_EPINTFLAG_TRFAIL1)
				{
					// IN endpoint failed
					//
					// Clear the interrupt flag
					pEp->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL1;
					T::TxDataRequest(iEp);
				}
			}
		}
	}

	//*********************************************************************
	// Handle setup requests in ISR

protected:
	static void ProcessSetup()
	{
		int		cbReq;
		int		cbAvail;
		ushort	*pus;
		const void *pv;
		UsbSetupPacket	*pSetup;

		pSetup = GetSetupBuf();

		if (pSetup->Type == USBRT_TypeStd_Val)
		{
			// Standard setup
			if (pSetup->Dir == USBRT_DirOut_Val)
			{
				// From host to device
				switch (pSetup->bRequest)
				{
				case USBREQ_Clear_Feature:
					if (pSetup->wValue == USBFEAT_EndpointHalt &&
						pSetup->bmRequestType == (USBRT_DirOut | USBRT_TypeStd | USBRT_RecipEp) &&
						(pSetup->wIndex & ~USBRT_Dir_Msk) <= MaxEndpointNumber)
					{
						USB->DEVICE.DeviceEndpoint[pSetup->wIndex & ~USBRT_Dir_Msk].EPSTATUSCLR.reg =
							pSetup->wIndex & USBRT_DirIn ? USB_DEVICE_EPSTATUSCLR_STALLRQ1 :
							USB_DEVICE_EPSTATUSCLR_STALLRQ0;
						break;
					}
					goto BadRequest;

				case USBREQ_Set_Feature:
					if (pSetup->wValue == USBFEAT_EndpointHalt &&
						pSetup->bmRequestType == (USBRT_DirOut | USBRT_TypeStd | USBRT_RecipEp) &&
						(pSetup->wIndex & ~USBRT_Dir_Msk) <= MaxEndpointNumber)
					{
						USB->DEVICE.DeviceEndpoint[pSetup->wIndex & ~USBRT_Dir_Msk].EPSTATUSSET.reg =
							pSetup->wIndex & USBRT_DirIn ? USB_DEVICE_EPSTATUSSET_STALLRQ1 :
							USB_DEVICE_EPSTATUSSET_STALLRQ0;
						break;
					}
					goto BadRequest;

				case USBREQ_Set_Address:
					// Set device address, but don't enable it
					USB->DEVICE.DADD.reg = USB_DEVICE_DADD_DADD(pSetup->bValue);
					DEBUG_PRINT("USB address = %i\n", pSetup->bValue);
					// Enable the transfer complete interrupt so we can enable the address when we're done
					// Clear flag first
					USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
					USB->DEVICE.DeviceEndpoint[0].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1;
					break;

				case USBREQ_Set_Configuration:
					// Only 1 configuration supported
					if (pSetup->bValue > 1)
						goto BadRequest;

					T::DeviceConfigured();
					break;

				default:
					goto BadRequest;
				}
				// Accept the packet
				AckControlPacket();
				return;
			}
			else
			{
				// From device to host
				switch (pSetup->bRequest)
				{
				case USBREQ_Get_Descriptor:
					switch (pSetup->bDescType)
					{
					case USBDESC_Device:
						pv = &DeviceDesc;
						cbAvail = sizeof DeviceDesc;
						break;

					case USBDESC_Config:
						pv = &ConfigDesc;
						cbAvail = sizeof(UsbConfig);
						break;

					case USBDESC_String:
						if (pSetup->bDescIndex < _countof(arpStrDesc))
							pv = arpStrDesc[pSetup->bDescIndex];
#ifdef USB_DEV_SerialNo
						else if (pSetup->bDescIndex == USBSTR_SerialNo)
							pv = T::GetSerialStrDesc();
#endif
						else if ((pv = T::NonStandardString(pSetup->bDescIndex)) == NULL)
							goto BadRequest;

						cbAvail = ((UsbStringDesc *)pv)->desc.bLength;
						break;

					default:
						goto BadRequest;
					}

					// Send the descriptor, limited by request length
					cbReq = pSetup->wLength;
					if (cbAvail > cbReq)
						cbAvail = cbReq;
					memcpy(GetSetupBuf(), pv, cbAvail);
					break;

				case USBREQ_Get_Status:
					pus = (ushort *)GetSetupBuf();
					if (pSetup->bmRequestType == (USBRT_DirIn | USBRT_TypeStd | USBRT_RecipDevice))
					{
						// Device status
						*pus = (USB_CFG_Attr & USBCFG_PowerSelf) ? 1 : 0;
					}
					else if (pSetup->bmRequestType == (USBRT_DirIn | USBRT_TypeStd | USBRT_RecipEp))
					{
						// Endpoint status
						if ((pSetup->wIndex & ~USBRT_Dir_Msk) > MaxEndpointNumber)
							goto BadRequest;
						*pus = (USB->DEVICE.DeviceEndpoint[pSetup->wIndex].EPSTATUS.reg &
							(pSetup->wIndex & USBRT_DirIn ? USB_DEVICE_EPSTATUS_STALLRQ1_Pos :
							USB_DEVICE_EPSTATUS_STALLRQ0_Pos)) ? 1 : 0;
					}
					else if (pSetup->bmRequestType == (USBRT_DirIn | USBRT_TypeStd | USBRT_RecipIface))
						// Interface status
						*pus = 0;
					else
						goto BadRequest;

					cbAvail = 2;
					break;

				default:
					goto BadRequest;
				}
				SendControlPacket(cbAvail);
				return;
			}
		}
		else
		{
			if (T::NonStandardSetup(pSetup))
				return;
		}

BadRequest:
		USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ0 |
			USB_DEVICE_EPSTATUSSET_STALLRQ1;
	}
};


//*************************************************************************
// USBdevice
//
//*************************************************************************

template<class T>
class USBdevice : public USBdeviceBase
{
	//*********************************************************************
	// The interrupt service routine - class with no data
	//*********************************************************************

public:
	static USBdeviceIsr<T>	ISR;
};
