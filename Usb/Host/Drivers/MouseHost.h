//****************************************************************************
// MouseHost.h
//
// Created 9/20/2020 3:55:22 PM by Tim
//
//****************************************************************************

#pragma once


#include <Usb/Host/UsbHostDriver.h>


union ButtonState
{
	ulong	ul;
	struct  
	{
		byte	btnDown;
		byte	btnStart;
		byte	btnEnd;
		byte	btnDbl;
	};
};

enum ButtonBits
{
	BUTTON_Left = 1,
	BUTTON_Right = 2,
	BUTTON_Middle = 4,
	BUTTON_X0 = 8,
	BUTTON_X1 = 0x10,
	BUTTON_X2 = 0x20,
	BUTTON_X3 = 0x40,
	BUTTON_X4 = 0x80,
};

class MouseHost : public UsbHostDriver
{
	//*********************************************************************
	// Types
	//*********************************************************************

	static constexpr int PacketBufferSize = 8;

	enum DevState
	{
		DS_Idle,
		DS_SetConfig,
		DS_WaitConfig,
		DS_SetProtocol,
		DS_WaitProtocol,
		DS_Poll,
		DS_HaveMove,
	};

	struct MouseBuffer
	{
		byte	bButtons;
		sbyte	moveX;
		sbyte	moveY;
		byte	Reserved[5];
	};

	//*********************************************************************
	// Public interface
	//*********************************************************************

	public:
		int	GetX()					{ return m_curX; }
		int	GetY()					{ return m_curY; }
		void SetPos(int x, int y)	{ m_curX = x;  m_curY = y; }

	public:
		ButtonState GetButtons()
		{
			ButtonState btns = m_stButtons;
			m_stButtons.ul &= 0xFF;	// clear all edge-triggered states
			return btns;
		}

		void Init(int maxX, int maxY, int minX = 0, int minY = 0)
		{
			m_minX = minX;
			m_minY = minY;
			m_maxX = maxX;
			m_maxY = maxY;
		}

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
			pIf->bInterfaceProtocol != USBHIDPROTO_Mouse)
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
					DEBUG_PRINT("Mouse driver loaded\n");
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
			m_stDev = DS_Poll;
			InitPolling();
			break;
		}
	}

	virtual void TransferComplete(int iPipe, int cbTransfer)
	{
		if (cbTransfer >= 3)
			m_stDev = DS_HaveMove;
		InitPolling();
	}

	virtual void TransferError(int iPipe, TransferErrorCode err)
	{
		DEBUG_PRINT("Data transfer error\n");
		InitPolling();
	}

	virtual int Process()
	{
		byte	stBtnDwn;
		byte	stBtnChg;
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
				USBhost::ReceiveData(m_bInPipe, &m_bufMouse, PacketBufferSize);
			break;

		case DS_HaveMove:
			m_curX += m_bufMouse.moveX;
			if (m_curX > m_maxX)
				m_curX = m_maxX;
			if (m_curX < m_minX)
				m_curX = m_minX;

			m_curY += m_bufMouse.moveY;
			if (m_curY > m_maxY)
				m_curY = m_maxY;
			if (m_curY < m_minY)
				m_curY = m_minY;

			stBtnDwn = m_bufMouse.bButtons;
			stBtnChg = m_stButtons.btnDown ^ stBtnDwn;
			// Edge-triggered states (start, end) are kept until read.
			m_stButtons.btnStart |= stBtnChg & stBtnDwn;
			m_stButtons.btnEnd |= stBtnChg & ~stBtnDwn;
			m_stButtons.btnDown = stBtnDwn;
			// UNDONE: double-click
			m_stDev = DS_Poll;
			return HOSTACT_MouseChange;
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

	//*********************************************************************
	// Instance data
	//*********************************************************************

	// USB buffer must be 4-byte aligned
	MouseBuffer	m_bufMouse ALIGNED_ATTR(long);
	ButtonState	m_stButtons;

	int		m_curX;
	int		m_curY;
	int		m_minX;
	int		m_maxX;
	int		m_minY;
	int		m_maxY;

	bool	m_fPollStart;
	byte	m_stDev;
	byte	m_bConfig;
	byte	m_bInPipe;
};
