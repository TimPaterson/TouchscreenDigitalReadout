//****************************************************************************
// KeyboardHost.h
//
// Created 9/20/2020 12:01:37 PM by Tim
//
//****************************************************************************

#pragma once

#include <Usb/Host/UsbHostDriver.h>
#include <Usb/Host/Drivers/KeyboardDef.h>


class KeyboardHost : public UsbHostDriver
{
	//*********************************************************************
	// Types
	//*********************************************************************

protected:
	static constexpr int FirstKbKeycode = 4;	// 'a' is keycode 4
	static constexpr int PacketBufferSize = 8;
	static constexpr int KeyBufSize = 6;

	enum DevState
	{
		DS_Idle,
		DS_SetConfig,
		DS_WaitConfig,
		DS_SetProtocol,
		DS_WaitProtocol,
		DS_Poll,
		DS_HaveKey,
		DS_SetIndicators,
	};

	struct UsbKeyBuffer
	{
		byte	bModifiers;
		byte	Reserved;
		byte	Keys[KeyBufSize];
	};

	union UsbOutBuffer
	{
		byte	bOut;
		ushort	usOut;
		ulong	ulOut;
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************

public:
	// This includes:
	//	bits 0 - 7 = key
	//	bits 8 - 15 = source, keyboard vs. keypad
	//	bits 16 - 24 = key modifiers (shift, ctrl, etc.)
	//
	ulong GetKey()
	{
		ulong key = m_keyReady;
		m_keyReady = NoKey;
		return key;
	}

	// Strip to basic key code
	byte GetKeyByte()	{ return (byte)GetKey(); }

	//*********************************************************************
	// Override of virtual functions
	//*********************************************************************

	virtual UsbHostDriver *IsDriverForDevice(ulong ulVidPid, ulong ulClass, UsbConfigDesc *pConfig)
	{
		int		iPipe;
		int		cEp;
		UsbInterfaceDesc	*pIf;
		UsbEndpointDesc		*pEp;

		if (ulClass != 0)
			return NULL;

		m_bConfig = pConfig->bConfigValue;

		pIf = (UsbInterfaceDesc *)ADDOFFSET(pConfig, pConfig->bLength);
		cEp = pIf->bNumEndpoints;
		if (pIf->bInterfaceClass != USBCLASS_Hid || 
			pIf->bInterfaceSubclass != USBHIDSUB_Boot || 
			pIf->bInterfaceProtocol != USBHIDPROTO_Keyboard)
			return NULL;

		pEp = (UsbEndpointDesc *)ADDOFFSET(pIf, pIf->bLength);
		
		// Search for IN endpoint
		while (cEp > 0)
		{
			if (pEp->bDescType == USBDESC_Endpoint)
			{
				if ((pEp->bEndpointAddr & USBEP_DirIn) &&
					pEp->bmAttributes == USBEP_Interrupt &&
					pEp->wMaxPacketSize <= 8)
				{
					iPipe = USBhost::RequestPipe(this, pEp);
					if (iPipe < 0)
						return NULL;
					m_bInPipe = iPipe;
					m_stDev = DS_SetConfig;
					m_keyReady = NoKey;
					m_bKeyLocks = 0;	// CONSIDER: user-settable default?
					DEBUG_PRINT("Keyboard driver loaded\n");
					return this;
				}
				cEp--;
			}
			pEp = (UsbEndpointDesc *)ADDOFFSET(pEp, pEp->bLength);
		}
		return NULL;
	}

	virtual void SetupTransactionComplete(int cbTransfer)
	{
		switch (m_stDev)
		{
		case DS_WaitConfig:
			m_stDev = DS_SetProtocol;
			break;

		case DS_WaitProtocol:
			m_stDev = DS_SetIndicators;
			InitPolling();
			break;
		}
	}

	virtual void TransferComplete(int iPipe, int cbTransfer)
	{
		if (cbTransfer >= 3)
			m_stDev = DS_HaveKey;
		InitPolling();
	}

	virtual void TransferError(int iPipe, TransferErrorCode err)
	{
		DEBUG_PRINT("Data transfer error\n");
		InitPolling();
	}

	virtual int Process()
	{
		ulong	key;
		uint	code;
		uint	codeNew;
		uint	keyMap;
		USBhost::ControlPacket	pkt;

		switch (m_stDev)
		{
		case DS_SetConfig:
			// Must set next state before call
			m_stDev = DS_WaitConfig;
			if (!USBhost::SetConfiguration(this, m_bConfig))
				m_stDev = DS_SetConfig;	// change state back
			break;

		case DS_SetProtocol:
			pkt.packet.bmRequestType = USBRT_DirOut | USBRT_TypeClass | USBRT_RecipIface;
			pkt.packet.bRequest = USBHIDREQ_Set_Protocol;
			pkt.packet.wValue = USBHIDRP_Boot;
			pkt.packet.wIndex = 0;	// interface no.
			pkt.packet.wLength = 0;
			// Must set next state before call
			m_stDev = DS_WaitProtocol;
			if (!USBhost::ControlTransfer(this, NULL, pkt.u64))
				m_stDev = DS_SetProtocol;	// change state back
			break;

		case DS_Poll:
			if (IsPollTime())
				USBhost::ReceiveData(m_bInPipe, &m_bufUsbKey, PacketBufferSize);
			break;

		case DS_HaveKey:
			// Need to see if we have any new keys by comparing to the array
			// of previous keys.
			keyMap = 0;
			codeNew = 0;
			// Look at each incoming key
			for (int i = 0; i < KeyBufSize; i++)
			{
				code = m_bufUsbKey.Keys[i];
				if (code == 0)
					continue;

				for (int j = 0; j < KeyBufSize; j++)
				{
					if (m_arPrevKey[j] == code)
					{
						keyMap |= 1 << j;
						m_bufUsbKey.Keys[i] = 0;
						goto OldKey;
					}
				}
				if (codeNew == 0)
					codeNew = code;
OldKey:
				continue;
			}

			// Remove missing keys from list of previous keys
			code = codeNew;
			for (int i = 0; i < KeyBufSize; i++)
			{
				if ((keyMap & (1 << i)) == 0)
				{
					// This position did not match. Store new
					// code, or just free it.
					m_arPrevKey[i] = code;
					code = 0;
				}
			}

			if (codeNew != 0)
			{
				key = MapKey(codeNew, m_bufUsbKey.bModifiers);
				if (key == LockPrefix)
					m_stDev = DS_SetIndicators;
				else
				{
					m_keyReady = key | ((ulong)m_bufUsbKey.bModifiers << 16);
					m_stDev = DS_Poll;
					return HOSTACT_KeyboardChange;
				}
			}
			else
				m_stDev = DS_Poll;
			break;

		case DS_SetIndicators:
			if (SetIndicators(m_bKeyLocks))
				m_stDev = DS_Poll;
		}
		return HOSTACT_None;
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************

protected:
	void InitPolling()
	{
		m_fPollStart = true;
	}

	bool IsPollTime()
	{
		if (m_fPollStart)
		{
			m_fPollStart = false;
			return true;
		}
		return false;
	}

	uint MapKey(uint uCode, uint uMod)
	{
		if (uCode < FirstKbKeycode)
			return NoKey;

		uCode -= FirstKbKeycode;

		// Is it keypad?
		if (uCode > _countof(mapKbKeycode) + ('z' - 'a'))
		{
			uCode -= _countof(mapKbKeycode) + ('z' - 'a') + 1;
			if (uCode >= _countof(mapKpNumLockKeycode))
				return NoKey;
			else if (m_bKeyLocks & KL_NumLock)
				uCode = mapKpNumLockKeycode[uCode];
			else
				uCode = mapKpEditKeycode[uCode];
			return uCode | KS_Keypad;
		}

		// First block of codes are the letters 'a' - 'z'
		if (uCode <= 'z' - 'a')
		{
			uCode += 'a';
			if (((uMod & KM_Shift) != 0) ^ ((m_bKeyLocks & KL_CapsLock) != 0))
				uCode -= ('a' - 'A');
		}
		else
		{
			uCode -= 'z' - 'a' + 1;

			if ((uMod & KM_Shift) && uCode < _countof(mapKbShift))
				uCode = mapKbShift[uCode];
			else
			{
				uCode = mapKbKeycode[uCode];

				// Check for CapLock, etc.
				if (uCode >= LockPrefix && uCode != NoKey)
				{
					uCode &= ~LockPrefix;
					m_bKeyLocks ^= uCode;
					return LockPrefix;
				}
			}
		}

		if (uMod & KM_Ctl)
		{
			if (uCode >= 0x40 && uCode <= 0x7F)
				uCode &= ~0x60;
			else if (uCode == 0x13)	// CR mapped to LF
				uCode = 0x0A;
			else if (uCode == 0x08)	// BS mapped to 0x7F
				uCode = 0x7F;
			else
				uCode = NoKey;
		}

		return uCode;
	}

	bool SetIndicators(byte bIndicators)
	{
		USBhost::ControlPacket	pkt;

		pkt.packet.bmRequestType = USBRT_DirOut | USBRT_TypeClass | USBRT_RecipIface;
		pkt.packet.bRequest = USBHIDREQ_Set_Report;
		pkt.packet.wValue = USBHIDRT_Output;
		pkt.packet.wIndex = 0;	// interface no.
		pkt.packet.wLength = 1;
		m_bufUsbOut.bOut = bIndicators;
		return USBhost::ControlTransfer(this, &m_bufUsbOut, pkt.u64);
	}

	//*********************************************************************
	// const (flash) data
	//*********************************************************************

	// Table for keys on the main keyboard (not keypad)
	inline const static byte mapKbKeycode[] =
	{
		'1','2','3','4','5','6','7','8','9','0',
		CR, ESC, BS, TAB,' ','-','=','[',']','\\',
		CD32,';','\'','`',',','.','/',CapsLk,
		F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
		PrtSc,ScrLk,Pause,Ins,Home,PgUp,Del,End,PgDn,
		Rt,Lf,Dn,Up,NumLk,
	};

	inline const static byte mapKbShift[] =
	{
		'!','@','#','$','%','^','&','*','(',')',
		CR, ESC, BS, TAB,' ','_','+','{','}','|',
		CD32,':','"','~','<','>','?',
	};

	// Table for keys on the keypad w/Num Lock
	inline const static byte mapKpNumLockKeycode[]
	{
		'/','*','-','+',CR,
		'1','2','3','4','5','6','7','8','9','0','.'
	};

	// Table for keys on the keypad w/o Num Lock
	inline const static byte mapKpEditKeycode[]
	{
		'/','*','-','+',CR,
		End,Dn,PgDn,Lf,'5',Rt,Home,Up,PgUp,Ins,Del
	};

	//*********************************************************************
	// Instance data
	//*********************************************************************

	// USB buffer must 4-byte aligned
	UsbKeyBuffer ALIGNED_ATTR(int)	m_bufUsbKey;
	UsbOutBuffer ALIGNED_ATTR(int)	m_bufUsbOut;

	ulong	m_keyReady;
	byte	m_arPrevKey[KeyBufSize];
	bool	m_fPollStart;
	byte	m_stDev;
	byte	m_bConfig;
	byte	m_bInPipe;
	byte	m_bKeyLocks;
};
