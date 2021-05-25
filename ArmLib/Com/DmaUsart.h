//****************************************************************************
// DmaUsart.h
//
// Created 3/26/2020 9:42:40 AM by Tim
//
//****************************************************************************

#pragma once

#include <Dma/Dmac.h>


#define DECLARE_DMA_USART(usart, inbuf, outbuf, ctr) \
	DmaUsart_t<#usart[6] - '0', inbuf, outbuf, #ctr[2] == 'C', (#ctr[2] == 'C' ? #ctr[3] : #ctr[2]) - '0'>

#define SERCOM_ASYNC_BAUD(baud, clock)	((uint16_t)-(((uint32_t)(((uint64_t)baud * (65536 * 16 * 2)) / clock) + 1) / 2))

#define USART_PTR(i)	((SercomUsart *)((byte *)SERCOM0 + ((byte *)SERCOM1 - (byte *)SERCOM0) * i))
#define TCC_PTR(i)		((Tcc *)((byte *)TCC0 + ((byte *)TCC1 - (byte *)TCC0) * i))
#define TC_PTR(i)		((TcCount16 *)((byte *)TC0 + ((byte *)TC1 - (byte *)TC0) * i))


class DmaUsartBase
{
	static constexpr int SERCOM_DMAC_ID_OFFSET = SERCOM1_DMAC_ID_RX - SERCOM0_DMAC_ID_RX;

protected:
	// Constants for a given instance
	byte	*m_pbRcvBufEnd;
	byte	*m_pbXmitBufEnd;
protected:
	byte	*m_pbRead;
	byte	*m_pbWrite;
	byte	*volatile m_pbLastDma;
	ushort	m_usReadCnt;
	byte	m_bChanWrite;
	byte	m_arbRcvBuf[0];

protected:
	int		GetRcvBufLen()	{return m_pbRcvBufEnd - m_arbRcvBuf;}
	byte	*GetXmitBuf()	{return m_pbRcvBufEnd;}

protected:
	void UsartInit(RxPad padRx, TxPad padTx, int iUsart)
	{
		SercomUsart	*pSercom;
		SERCOM_USART_CTRLA_Type	serCtrlA;

		pSercom = USART_PTR(iUsart);

#if	defined(GCLK_PCHCTRL_GEN_GCLK0)
		// Enable clock
		MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0 << iUsart;

		// Clock it with GCLK0
		GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + iUsart].reg = GCLK_PCHCTRL_GEN_GCLK0 |
			GCLK_PCHCTRL_CHEN;
#else
		// Enable clock
		PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0 << iUsart;

		// Clock it with GCLK0
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
			(GCLK_CLKCTRL_ID_SERCOM0_CORE + iUsart);
#endif

		// standard 8,N,1 parameters
		serCtrlA.reg = 0;
		serCtrlA.bit.DORD = 1;		// LSB first
		serCtrlA.bit.MODE = 1;		// internal clock
		serCtrlA.bit.RXPO = padRx;
		serCtrlA.bit.TXPO = padTx;
		pSercom->CTRLA.reg = serCtrlA.reg;
		pSercom->CTRLB.reg = SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_RXEN;
	}

	void DmaInit(int iRdChan, int iWrChan, int iEvChan, int iUsart)
	{
		SercomUsart	*pSercom;

		pSercom = USART_PTR(iUsart);

		DMAC_SET_CHAN_ISR_METHOD(iWrChan, Isr);

		// Set up Read DMA descriptor
		Dma.Desc[iRdChan].BTCTRL.reg = DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_VALID | DMAC_BTCTRL_EVOSEL_BEAT;
		Dma.Desc[iRdChan].BTCNT.reg = GetRcvBufLen();
		Dma.Desc[iRdChan].SRCADDR.reg = (int)&pSercom->DATA;
		// Bizarrely, an incremented address is set to the end of the buffer
		Dma.Desc[iRdChan].DSTADDR.reg = (int)m_pbRcvBufEnd;
		Dma.Desc[iRdChan].DESCADDR.reg = (int)&Dma.Desc[iRdChan];

		// Set up Write DMA descriptor
		m_bChanWrite = iWrChan;
		Dma.Desc[iWrChan].BTCTRL.reg = DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID;
		Dma.Desc[iWrChan].DSTADDR.reg = (int)&pSercom->DATA;
		Dma.Desc[iWrChan].DESCADDR.reg = 0;

		// Read channel initialization
		DMAC->CHID.reg = iRdChan;
		DMAC->CHCTRLB.reg = DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_LVL(3) | DMAC_CHCTRLB_EVOE |
			DMAC_CHCTRLB_TRIGSRC(SERCOM0_DMAC_ID_RX + iUsart * SERCOM_DMAC_ID_OFFSET);
		DMAC->CHCTRLA.reg = DMAC_CHCTRLA_ENABLE;
		// Write channel initialization, but don't enable
		DMAC->CHID.reg = iWrChan;
		DMAC->CHCTRLB.reg = DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_LVL(2) |
			DMAC_CHCTRLB_TRIGSRC(SERCOM0_DMAC_ID_TX + iUsart * SERCOM_DMAC_ID_OFFSET);

		// Pipe the event on DMAC to event channel iEvChan
		// No GCLK needed for asynchronous operation
		MCLK->APBCMASK.reg |= MCLK_APBCMASK_EVSYS;
		EVSYS->CHANNEL[iEvChan].reg = EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
			EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_DMAC_CH_0 + iRdChan);

		m_pbRead = m_arbRcvBuf;
		m_usReadCnt = 0;
		m_pbLastDma = m_pbWrite = GetXmitBuf();
	}

	//************************************************************************
	// Read Functions

public:
	byte ReadByte() NO_INLINE_ATTR
	{
		byte	*pb;
		byte	b;

		m_usReadCnt++;
		pb = m_pbRead;
		b = *pb++;
		if (pb >= m_pbRcvBufEnd)
			pb = m_arbRcvBuf;
		m_pbRead = pb;
		return b;
	}

	void ReadBytes(void *pv, int cb) NO_INLINE_ATTR
	{
		byte	*pb;

		pb = (byte *)pv;
		for ( ; cb > 0; cb--, pb++)
			*pb = ReadByte();
	}

	byte PeekByte()
	{
		return *m_pbRead;
	}

	byte PeekByte(int off) NO_INLINE_ATTR
	{
		byte	*pb;

		pb = m_pbRead + off;
		if (pb >= m_pbRcvBufEnd)
			pb -= GetRcvBufLen();
		return *pb;
	}

	void DiscardReadBuf(int cnt) NO_INLINE_ATTR
	{
		byte	*pb;

		m_usReadCnt += cnt;
		pb = m_pbRead + cnt;
		if (pb >= m_pbRcvBufEnd)
			pb -= GetRcvBufLen();
		m_pbRead = pb;
	}

	//************************************************************************
	// Write Functions

	void PutByte(byte b) NO_INLINE_ATTR
	{
		byte	*pb;

		pb = m_pbWrite;
		*pb++ = b;
		if (pb >= m_pbXmitBufEnd)
			pb = GetXmitBuf();
		m_pbWrite = pb;
	}

	void WriteByte(byte b) NO_INLINE_ATTR
	{
		PutByte(b);
		SendBytes();
	}

	void SendBytes() NO_INLINE_ATTR
	{
		// A different DMA ISR could change the CHID register
		NVIC_DisableIRQ(DMAC_IRQn);

		if (m_pbLastDma != m_pbWrite)
		{
			DMAC->CHID.reg = m_bChanWrite;
			if (DMAC->CHCTRLA.bit.ENABLE)
			{
				// Have completion interrupt start next transfer
				DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
			}
			else
				Isr();	// Send what we have so far, up to end of buffer
		}

		NVIC_EnableIRQ(DMAC_IRQn);
	}

	// Use this after GetWriteBuf() to say how many bytes were written
	void SendBytes(int cb) NO_INLINE_ATTR
	{
		m_pbWrite += cb;
		SendBytes();
	}

	byte *GetWriteBuf(int cb) NO_INLINE_ATTR
	{
		if (m_pbWrite + cb >= m_pbXmitBufEnd)
		{
			// Must wrap to fit buffer.
			// Wait for current data to start sending.
			SendBytes();
			while (m_pbLastDma != m_pbWrite);
			m_pbLastDma = m_pbWrite = GetXmitBuf();
		}
		return m_pbWrite;
	}

	void WriteBytes(void *pv, int cb) NO_INLINE_ATTR
	{
		memcpy(GetWriteBuf(cb), pv, cb);
		m_pbWrite += cb;
		SendBytes();
	}

	void WriteBytes(int cb, ...) NO_INLINE_ATTR
	{
		va_list	args;
		va_start(args, cb);
		for ( ; cb > 0; cb--)
			PutByte(va_arg(args, int));
		va_end(args);
		SendBytes();
	}

	void WriteString(const char *psz) NO_INLINE_ATTR
	{
		char	ch;

		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				break;
			if (ch == '\n')
				PutByte('\r');
			PutByte(ch);
		}
		SendBytes();
	}

	byte IsXmitInProgress()
	{
		DMAC->CHID.reg = m_bChanWrite;
		return DMAC->CHCTRLA.reg & DMAC_CHCTRLA_ENABLE;
	}

	void Isr()
	{
		byte	*pbWrite;
		byte	*pbLastDma;
		byte	*pbNextDma;

		DMAC->CHID.reg = m_bChanWrite;
		DMAC->CHINTENCLR.reg = DMAC_CHINTENCLR_TCMPL;

		pbNextDma = pbWrite = m_pbWrite;
		pbLastDma = m_pbLastDma;
		// See if write pointer has wrapped
		if (pbLastDma > pbWrite)
		{
			pbWrite = m_pbXmitBufEnd;	// only send to end of buffer
			pbNextDma = GetXmitBuf();	// pick up at start of buffer
			// Have more, transfer it on next interrupt
			DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
		}
		Dma.Desc[m_bChanWrite].SRCADDR.reg = (int)pbWrite;
		Dma.Desc[m_bChanWrite].BTCNT.reg = pbWrite - pbLastDma;
		m_pbLastDma = pbNextDma;
		DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;	// Clear interrupt flag before we start
		DMAC->CHCTRLA.reg = DMAC_CHCTRLA_ENABLE;
	}

	//************************************************************************
	// Baud Rate Functions

	ushort CalcBaudRate(uint32_t rate, uint32_t clock) NO_INLINE_ATTR
	{
		uint32_t	quo;
		uint32_t	quoBit;

		rate *= 16;		// actual clock frequency
		// Need 17-bit result of rate / clock
		for (quo = 0, quoBit = 1 << 16; quoBit != 0; quoBit >>= 1)
		{
			if (rate >= clock)
			{
				rate -= clock;
				quo |= quoBit;
			}
			rate <<= 1;
		}
		// Round
		if (rate >= clock)
			quo++;
		return -quo;
	}

	ushort CalcBaudRateConst(uint32_t rate, uint32_t clock)
	{
		return SERCOM_ASYNC_BAUD(rate, clock);
	}

#ifdef F_CPU
	ushort CalcBaudRate(uint32_t rate)
	{
		return CalcBaudRate(rate, F_CPU);
	}

	ushort CalcBaudRateConst(uint32_t rate)
	{
		return CalcBaudRateConst(rate, F_CPU);
	}
#endif
};

//****************************************************************************

template <int iUsart, int cbRcvBuf, int cbXmitBuf, bool fIsTcc, int iTc>
class DmaUsart_t : public DmaUsartBase
{
protected:
	byte	reserved[cbRcvBuf];
	byte	m_arbXmitBuf[cbXmitBuf];

public:
	void UsartInit(RxPad padRx, TxPad padTx)
	{
		m_pbRcvBufEnd = m_arbRcvBuf + cbRcvBuf;
		m_pbXmitBufEnd = GetXmitBuf() + cbXmitBuf;

		DmaUsartBase::UsartInit(padRx, padTx, iUsart);
	}

	void DmaInit(int iRdChan, int iWrChan, int iEvChan)
	{
		DmaUsartBase::DmaInit(iRdChan, iWrChan, iEvChan, iUsart);

		if (fIsTcc)
		{
			int iEvUser;

			// Finish event routing
			iEvUser = iTc == 0 ? EVSYS_ID_USER_TCC0_EV_0 : (iTc == 1 ? EVSYS_ID_USER_TCC1_EV_0 : EVSYS_ID_USER_TCC2_EV_0);
			EVSYS->USER[iEvUser].reg = iEvChan + 1;	// channel n-1 selected

			// Set up counter
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC0 << iTc;
			GCLK->PCHCTRL[TCC0_GCLK_ID + iTc / 2].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
			TCC_PTR(iTc)->EVCTRL.reg = TCC_EVCTRL_TCEI0 | TCC_EVCTRL_EVACT0_INC;
			TCC_PTR(iTc)->CTRLA.reg = TCC_CTRLA_ENABLE;
		}
		else
		{
			// Finish event routing
			EVSYS->USER[EVSYS_ID_USER_TC0_EVU + iTc].reg = iEvChan + 1;	// channel n-1 selected

			// Set up counter
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0 << iTc;
			GCLK->PCHCTRL[TC0_GCLK_ID + iTc / 2].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
			TC_PTR(iTc)->EVCTRL.reg = TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_COUNT;
#ifdef TC_READREQ_OFFSET
			// Set up automatic synchronization
			TC_PTR(iTc)->READREQ.reg = TC_READREQ_RCONT | offsetof(TcCount16, COUNT);
#endif
			TC_PTR(iTc)->CTRLA.reg = TC_CTRLA_ENABLE;
		}
	}

	void Enable()
	{
		USART_PTR(iUsart)->CTRLA.bit.ENABLE = 1;
	}

	void Disable()
	{
		USART_PTR(iUsart)->CTRLA.bit.ENABLE = 0;
	}

	//************************************************************************
	// Read Functions

	ushort BytesCanRead() NO_INLINE_ATTR
	{
		ushort	cb;

		if (fIsTcc)
		{
			// Must synchronize first
			TCC_PTR(iTc)->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
			while (TCC_PTR(iTc)->SYNCBUSY.reg & (TCC_SYNCBUSY_CTRLB | TCC_SYNCBUSY_COUNT));
			cb = TCC_PTR(iTc)->COUNT.reg - m_usReadCnt;
		}
		else
		{
#ifndef TC_READREQ_OFFSET
			// Must synchronize first
			TC_PTR(iTc)->CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
			while (TC_PTR(iTc)->SYNCBUSY.reg & (TC_SYNCBUSY_CTRLB | TC_SYNCBUSY_COUNT));
#endif
			cb = TC_PTR(iTc)->COUNT.reg - m_usReadCnt;
		}

		// Make sure we don't report more bytes than the buffer size
		while (cb >= cbRcvBuf)
			cb -= cbRcvBuf;
		return cb;
	}

	bool IsByteReady()			{ return BytesCanRead() != 0; }

	void DiscardReadBuf()		{ DmaUsartBase::DiscardReadBuf(BytesCanRead()); }

	void DiscardReadBuf(int cb)	{ DmaUsartBase::DiscardReadBuf(cb); }

	//************************************************************************
	// Baud Rate Functions

	void SetBaudReg(uint16_t rate)
	{
		USART_PTR(iUsart)->BAUD.reg = rate;
	}

	void SetBaudRate(uint32_t rate, uint32_t clock)
	{
		SetBaudReg(CalcBaudRate(rate, clock));
	}

	void SetBaudRateConst(uint32_t rate, uint32_t clock)
	{
		SetBaudReg(CalcBaudRateConst(rate, clock));
	}

#ifdef F_CPU
	void SetBaudRate(uint32_t rate)
	{
		SetBaudRate(rate, F_CPU);
	}

	void SetBaudRateConst(uint32_t rate)
	{
		SetBaudRateConst(rate, F_CPU);
	}
#endif
};
