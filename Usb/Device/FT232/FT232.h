//****************************************************************************
// Class FT232
// FT232.h
//
// Created 8/18/2020 10:40:12 AM by Tim
//
//****************************************************************************

#pragma once

#include "FT232Def.h"
#include <Usb/Device/USBdevice.h>


#define FTDI_IN_ENDPOINT	1
#define FTDI_OUT_ENDPOINT	2
#define FTDI_STATUS_SIZE	2

// This is represents the format and length of the serial number
// First character is kept, remainder is replaced by number generated
// from D21 chip serial number.
#define	SERIAL_NUM	"B0000000"


//****************************************************************************
// FT232 Serial-to-USB converter
//****************************************************************************

template<class T>
class FT232 : public USBdeviceBase
{
	//*********************************************************************
	// Public Types
	//*********************************************************************

public:
	//*************************************************************************
	// Flags for GetPinStatus() / SetPinStatus()

	enum PinStatusBits
	{
		STAT_None,

		// Outputs, the first two mapped the same as FTDI_MODEM_CTRL
		STAT_DTR = 0x01,
		STAT_RTS = 0x02,
		STAT_Modem = STAT_DTR | STAT_RTS,
		STAT_Break = 0x04,
		STAT_Outputs = STAT_Modem | STAT_Break,

		// Inputs, mapped the same as returned by FTDI_GET_MODEM_STATUS
		STAT_CTS = 0x10,
		STAT_DSR = 0x20,
		STAT_RI  = 0x40,
		STAT_DCD = 0x80,
		STAT_Inputs = STAT_CTS | STAT_DSR | STAT_RI | STAT_DCD,
	};

	//*********************************************************************
	// Local Types
	//*********************************************************************

protected:
	static constexpr int InBufSize = FTDI_OUT_BUF_SIZE;	// host to device
	static constexpr int OutBufSize = FTDI_IN_BUF_SIZE;	// device to host

	//*************************************************************************
	// USB vendor-specific control requests

	enum FtdiRequest
	{
		FTDI_RESET,				// Reset the port
		FTDI_MODEM_CTRL,		// Set the modem control register
		FTDI_SET_FLOW_CTRL,		// Set flow control options
		FTDI_SET_BAUD_RATE,		// Set the baud rate
		FTDI_SET_DATA,			// Set the data characteristics of the port
		FTDI_GET_MODEM_STATUS,	// Retrieve current value of modem status register
		FTDI_SET_EVENT_CHAR,	// Set the event character
		FTDI_SET_ERROR_CHAR,	// Set the error character
		FT_SET_LATENCY_TIMER,	// Set the latency timer
		FT_GET_LATENCY_TIMER,	// Return the latency timer
		FT_SET_BIT_MODE,		// Set special bit mode or turn on a special function
		FT_GET_BIT_MODE			// Return the current values on the DBUS pins
	};

	// Upper byte of Set Data
	#define FTDI_Parity_Pos		0
	#define FTDI_Parity_Msk		0x07
	#define FTDI_StopBits_Pos	3
	#define FTDI_StopBits_Msk	0x07
	#define FTDI_SendBreak_Pos	6
	#define FTDI_SendBreak_Msk	1
	#define FTDI_SendBreak		(FTDI_SendBreak_Msk << FTDI_SendBreak_Pos)

	#define FTDI_LINE_STATUS		0x60
	#define FTDI_MODEM_STATUS_BASE	0x01

	//*************************************************************************
	// Flags for StateChange callback

	enum PortChange
	{
		PC_None,
		PC_BaudRate,
		PC_PinStatus,
	};

	//*********************************************************************
	// Buffer Manager

	template<int cbBuf>
	class BufMgr
	{
public:
		void Init()					{ ulStatus = 0; }
		void *GetCur()				{ return &arBuf[bCur]; }
		void Swap()					{ bCur ^= 1; }
		// count of bytes in use only used for transmit buffers
		int GetInUse()				{ return usInUse; }
		void SetInUse(int cb)		{ usInUse = cb; }
		bool GetIsSending()			{ return fSending; }
		void SetIsSending(bool f)	{ fSending = f; }

protected:
		union
		{
			uint32_t	ulStatus;
			struct
			{
				ushort	usInUse;
				byte	bCur;
				volatile bool fSending;
			};
		};
		uint32_t	arBuf[2][(cbBuf + sizeof(uint32_t) - 1)/sizeof(uint32_t)];
	};

	struct SetupDataSrc
	{
		int		cb;
		const ushort *pus;
	};

	//*********************************************************************
	// These are callbacks implemented in class T, the template argument
	//*********************************************************************

	/*
protected:
	static void RxData(void *pv, int cb);
	static void StateChange(int flags);
	*/

	//*********************************************************************
	// Public interface
	//*********************************************************************

	// To send data to the host, use TxData() or TxByte(). These fill a
	// buffer equal to the USB packet size (64 bytes) and send it when it's
	// full.
	//
	// Of course for normal small transmissions you don't want to wait
	// for the buffer to fill. To emulate normal FT232 behavior, the main
	// loop should call Process(). It will send the buffer every 1 ms.
	//
	// If your transmissions are packet oriented, or you prefer a different
	// latency interval, you can also call SendBuffer() whenever you want to
	// send the current contents of the buffer.

public:
	static bool Process()
	{
		if (fSendBuf)
		{
			fSendBuf = false;
			return SendBuffer();
		}
		return false;
	}

	static bool SendBuffer() NO_INLINE_ATTR
	{
		ushort	*pus;

		if (bufTrans.GetIsSending())
			return false;

		pus = (ushort *)bufTrans.GetCur();
		*pus = GetStatus();
		bufTrans.SetIsSending(true);
		SendToHost(FTDI_IN_ENDPOINT, pus, bufTrans.GetInUse());
		bufTrans.Swap();
		bufTrans.SetInUse(FTDI_STATUS_SIZE);	// Save space for status
		return true;
	}

	// This function returns the no. bytes actually sent
	static int TxData(void *pvTx, int cbTx) NO_INLINE_ATTR
	{
		byte	*pbTx = (byte *)pvTx;
		byte	*pb;
		int		cb;
		int		cbSend;
		int		cbTotal;

		cbTotal = 0;
		do
		{
			pb = (byte *)bufTrans.GetCur();
			cb = bufTrans.GetInUse();
			cbSend = OutBufSize - cb;
			if (cbSend > cbTx)
				cbSend = cbTx;
			memcpy(pb + cb, pbTx, cbSend);
			pbTx += cbSend;
			cbTx -= cbSend;
			cb += cbSend;
			cbTotal += cbSend;
			bufTrans.SetInUse(cb);
		} while (cb == OutBufSize && SendBuffer());

		return cbTotal;
	}

	static bool TxByte(byte b) NO_INLINE_ATTR
	{
		byte	*pb;
		int		cb;

		cb = bufTrans.GetInUse();
		if (cb >= OutBufSize)
		{
			if (!SendBuffer())
				return false;
			cb = bufTrans.GetInUse();
		}

		pb = (byte *)bufTrans.GetCur();
		pb[cb++] = b;
		bufTrans.SetInUse(cb);
		if (cb == OutBufSize)
		{
			// buffer full, send it off
			SendBuffer();
		}
		return true;
	}

	static int GetBaudRate()	{ return BaudRate; }

	static PinStatusBits GetPinStatus()	{ return PinStatus; }

	static void SetPinStatus(PinStatusBits pins)
		 { PinStatus = (PinStatusBits)((PinStatus & ~STAT_Inputs) | (pins & STAT_Inputs)); }

	//*********************************************************************
	// Implementation of callbacks from USBdevice class
	//*********************************************************************

public:
	static void DeviceConfigured()
	{
		bufTrans.Init();
		bufRcv.Init();
		bufTrans.SetInUse(FTDI_STATUS_SIZE);	// Save space for status
		ReceiveFromHost(FTDI_OUT_ENDPOINT, bufRcv.GetCur(), InBufSize);
	}

	static void RxData(int iEp, void *pv, int cb)
	{
		// Swap in other buffer
		bufRcv.Swap();
		ReceiveFromHost(FTDI_OUT_ENDPOINT, bufRcv.GetCur(), InBufSize);
		T::RxData(pv, cb);	// callback to report data
	}

	static void TxDataRequest(int iEp)
	{
	}

	static void TxDataSent(int iEp, int cb)
	{
		bufTrans.SetIsSending(false);
	}

	static void StartOfFrame()
	{
		// Send buffer with status whether or not we have data
		fSendBuf = true;
	}

	static const void *NonStandardString(int index)
	{
		return NULL;
	}

	static bool NonStandardSetup(UsbSetupPacket *pSetup)
	{
		int		cSrc;
		int		cCur;
		int		iTmp;
		int		iMask;
		byte	*pb;
		ushort	*pus;
		PinStatusBits		iStat;
		const UsbStringDesc	*pStr;
		const SetupDataSrc	*pSrc;

		switch (pSetup->bmRequestType)
		{
		case USBRT_DirOut | USBRT_TypeVendor | USBRT_RecipDevice:
			switch(pSetup->bRequest)
			{
			case FTDI_SET_BAUD_RATE:
				// Set baud rate
				uint	uFrac;
				uint	uDiv;
				uint	uRate;

				if (pSetup->wValue == 0)
					uRate = 3000000;
				else if (pSetup->wValue == 1)
					uRate = 2000000;
				else
				{
					uDiv = pSetup->wValue & 0x3FFF;
					uFrac = pSetup->wValue >> 14;
					uFrac |= (pSetup->wIndex & 1) << 2;
					// Convert fraction bits to fraction number, in eighths
					uFrac = arbBaudFrac[uFrac];
					uDiv = (uDiv << 3) | uFrac;
					uRate = DivUintRnd(24000000, uDiv);
				}
				if (uRate != BaudRate)
				{
					BaudRate = uRate;
					T::StateChange(PC_BaudRate);
				}
				break;

			case FTDI_MODEM_CTRL:
				iTmp = pSetup->bDescIndex;	// first value is state
				iMask = pSetup->bDescType;	// second value is mask
				iMask &= STAT_Modem;		// Only accept valid bits
				iTmp &= iMask;
				iStat = (PinStatusBits)((PinStatus & ~iMask) | iTmp);
SetStatus:
				if (iStat != PinStatus)
				{
					PinStatus = iStat;
					T::StateChange(PC_PinStatus);
				}
				break;

			case FTDI_SET_DATA:
				// First byte of wValue has no. of data bits.
				// Second byte also has parity & stop bits.
				// We don't care about that stuff, just get BREAK.
				iTmp = PinStatus & ~STAT_Break;
				if (pSetup->bDescType & FTDI_SendBreak)
					iTmp = (iTmp | STAT_Break);
				iStat = (PinStatusBits)iTmp;
				goto SetStatus;
			}
			// Whatever this is, just accept it
			AckControlPacket();
			return true;

		case USBRT_DirIn | USBRT_TypeVendor | USBRT_RecipDevice:
			switch(pSetup->bRequest)
			{
			case FTDI_GET_MODEM_STATUS:
				pb = (byte *)GetSetupBuf();
				pb[0] = (PinStatus & STAT_Inputs) | FTDI_MODEM_STATUS_BASE;
				// This is where we could send back errors -
				// overrun, framing, parity
				pb[1] = FTDI_LINE_STATUS;
				break;

			case 0x90:
				cCur = pSetup->wIndex;
				pSrc = arSrc;
				for (;;)
				{
					cSrc = pSrc->cb;
					if (cSrc == 0)
					{
						pStr = GetSerialStrDesc();
						cSrc = pStr->desc.bLength / 2;
						if (cCur < cSrc)
						{
							pus = (ushort *)pStr;
							iTmp = *(pus + cCur);
							goto SendBytes;
						}
					}
					else if (cCur < cSrc)
						break;
					pSrc++;
					if (pSrc >= &arSrc[_countof(arSrc)])
						return false;
					cCur -= cSrc;
				}

				pus = (ushort *)pSrc->pus;
				if (pus == NULL)
					iTmp = 0;
				else
					iTmp = *(pus + cCur);
SendBytes:
				pus = (ushort *)GetSetupBuf();
				*pus = iTmp;
				break;

			default:
				return false;

			} // switch bRequest

			SendControlPacket(2);
			return true;

		} // switch bmRequestType
		return false;
	}

	static const UsbStringDesc *GetSerialStrDesc() NO_INLINE_ATTR
	{
		if (uSerialNum == 0)
		{
			char		*pch;
			char16_t	*pch16;

			// Use D21 serial number to generate USB serial number
			uSerialNum = *NVM_SERIAL_NUMBER_0 + *NVM_SERIAL_NUMBER_1 +
				*NVM_SERIAL_NUMBER_2 + *NVM_SERIAL_NUMBER_3;

			pch16 = &SerialNumber.str[1];
			pch = (char *)pch16;
			// Convert the 32-bit number to a string in base 36.
			// This will use '0' - '9' and 'a' - 'z', but as
			// 8-bit characters. It will take at most 7 digits.
			itoa(uSerialNum, pch, 36);
			// Read the 8-bit characters and write them back as
			// 16-bit characters, starting at the end.
			for (int i = 6; i >= 0; i--)
			{
				char ch = pch[i];
				// convert to upper case
				ch &= ~((ch & 0x40) >> 1);
				pch16[i] = ch;
			}
		}
		return &SerialNumber;
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	static ushort GetStatus()
	{
		return ((PinStatus & STAT_Inputs) | FTDI_MODEM_STATUS_BASE) | (FTDI_LINE_STATUS << 8);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

protected:
	// The FT232 has a vendor-specific request to return some data, some
	// of which has unknown meaning. This table provides the data as
	// seen with a USB analyzer

	inline static const ushort arusSetupData1[] =
	{
		0x4000, USB_DEV_Vendor, USB_DEV_Product, 0x000, 0x2DA0,
		0x0008, 0x0000, 0x0A98, 0x20A2, 0x12C2, 0x1023, 0x0005
	};

	inline static const ushort arusSetupData2[] =
	{
		0x0B3B, 0xC0AC
	};

	inline static const ushort arusSetupData3[] =
	{
		0xC85A
	};

	inline static const SetupDataSrc arSrc[] =
	{
		{ _countof(arusSetupData1), arusSetupData1 },
		{ VendorStr.desc.bLength / 2, (ushort *)&VendorStr },
		{ ProductStr.desc.bLength / 2, (ushort *)&ProductStr },
		{ 0, NULL },	// Zero length indicates Serial Number
		{ _countof(arusSetupData2), arusSetupData2 },
		{ 19, NULL },
		{ _countof(arusSetupData3), arusSetupData3 },
	};

	//*********************************************************************
	// Baud rate lookup table

	inline static const byte arbBaudFrac[8] = {0, 4, 2, 1, 3, 5, 6, 7};

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

protected:
	inline static BufMgr<OutBufSize> bufTrans;
	inline static BufMgr<InBufSize> bufRcv;

	inline static uint BaudRate;
	inline static PinStatusBits PinStatus;

	//*********************************************************************
	// Serial number, generated from D21 chip serial number

	inline static uint32_t uSerialNum;

	inline static UsbStringDesc SerialNumber =
	{
		{sizeof(UsbStringDescHead) + sizeof(STRING16(SERIAL_NUM)) - sizeof(char16_t),
			USBDESC_String},
		STRING16(SERIAL_NUM)
	};

	//*********************************************************************
	// Set in ISR
	volatile inline static bool fSendBuf;

	//*********************************************************************
	// The interrupt service routine - class with no data
	//*********************************************************************

public:
	static USBdeviceIsr<FT232<T>>	ISR;
};
