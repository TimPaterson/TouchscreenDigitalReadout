//****************************************************************************
// IoBuf.h
//
// Created 9/4/2015 11:56:10 AM by Tim
//
// Class IoBuf is a generic base class that implements ring buffers for
// input and output. It can be used for serial I/O, two-wire slave, etc.
//
// This class does not include the buffers themselves. The UsartBuf
// class in UsartBuf.h shows how to create a template that creates a 
// derived class with specific buffer sizes.
//
//****************************************************************************

#pragma once

class IoBuf
{
public:
	NO_INLINE_ATTR void WriteByte(byte b)				{ WriteByteInline(b); }
	NO_INLINE_ATTR void WriteBytes(void *pv, int cb)	{ WriteBytesInline(pv, cb); }
	NO_INLINE_ATTR void WriteString(const char *psz)	{ WriteStringInline(psz); }
	NO_INLINE_ATTR byte ReadByte()						{ return ReadByteInline(); }
	NO_INLINE_ATTR void ReadBytes(void *pv, int cb)		{ ReadBytesInline(pv, cb); }
	NO_INLINE_ATTR byte PeekByte()						{ return PeekByteInline(); }
	NO_INLINE_ATTR byte PeekByte(int off)				{ return PeekByteInline(off); }
	NO_INLINE_ATTR int BytesCanWrite()					{ return BytesCanWriteInline(); }
	NO_INLINE_ATTR int BytesCanRead()					{ return BytesCanReadInline(); }
	NO_INLINE_ATTR bool CanWriteByte()					{ return CanWriteByteInline(); }
	NO_INLINE_ATTR void DiscardReadBuf(int cnt)			{ DiscardReadBufInline(cnt); }

public:
	bool IsByteReady()		{ return m_pbNextRcvIn != m_pbNextRcvOut; }
	void DiscardReadBuf()	{ m_pbNextRcvOut = m_pbNextRcvIn; }

protected:
	void WriteByteInline(byte b)
	{
		byte	*pb;

		pb = m_pbNextXmitIn + 1;
		if (pb == m_pbXmitBufEnd)
			pb = GetXmitBuf();

		// Wait for room in output buffer
		while (pb == m_pbNextXmitOut);

		*pb = b;
		m_pbNextXmitIn = pb;
	}

	void WriteBytesInline(void *pv, int cb)
	{
		byte	*pb;
		
		for (pb = (byte *)pv; cb > 0; cb--, pb++)
			WriteByte(*pb);
	}

	byte ReadByteInline()
	{
		byte	*pb;
		byte	b;

		pb = m_pbNextRcvOut + 1;
		if (pb == m_pbRcvBufEnd)
			pb = GetRcvBuf();
		b = *pb;	// get data from buffer
		m_pbNextRcvOut = pb;
		return b;
	}

	void ReadBytesInline(void *pv, int cb)
	{
		byte	*pb;
		
		for (pb = (byte *)pv; cb > 0; cb--, pb++)
			*pb = ReadByte();
	}

	byte PeekByteInline()
	{
		byte	*pb;

		pb = m_pbNextRcvOut + 1;
		if (pb == m_pbRcvBufEnd)
			pb = GetRcvBuf();
		return *pb;
	}

	byte PeekByteInline(int off)
	{
		byte	*pb;
		
		pb = m_pbNextRcvOut + off + 1;
		if (pb >= m_pbRcvBufEnd)
			pb -= m_pbRcvBufEnd - GetRcvBuf();
		return *pb;
	}

	int BytesCanWriteInline()
	{
		byte	*pbIn;
		byte	*pbOut;

		pbOut = m_pbNextXmitOut;
		pbIn = m_pbNextXmitIn;
		if (pbOut <= pbIn)
			pbOut += m_pbXmitBufEnd - GetXmitBuf();
		return pbOut - pbIn - 1;
	}
	
	int BytesCanReadInline()
	{
		byte	*pbIn;
		byte	*pbOut;

		pbIn = m_pbNextRcvIn;
		pbOut = m_pbNextRcvOut;
		if (pbIn < pbOut)
			pbIn += m_pbRcvBufEnd - GetRcvBuf();
		return pbIn - pbOut;
	}

	bool CanWriteByteInline()
	{
		byte	*pb;

		pb = m_pbNextXmitIn + 1;
		if (pb == m_pbXmitBufEnd)
			pb = GetXmitBuf();

		return pb != m_pbNextXmitOut;
	}

	void DiscardReadBufInline(int cnt)
	{
		byte	*pb;

		pb = m_pbNextRcvOut + cnt;
		if (pb >= m_pbRcvBufEnd)
			pb -= m_pbRcvBufEnd - GetRcvBuf();
		m_pbNextRcvOut = pb;
	}
	
	void WriteStringInline(const char *psz)
	{
		char	ch;
	
		for (;;)
		{
			ch = *psz++;
			if (ch == 0)
				return;
			if (ch == '\n')
				WriteByte('\r');
			WriteByte(ch);		
		}
	}

	//************************************************************************
	// These functions work on the I/O end of the buffer.
	// I.E., SendByte() removes a byte put there by WriteByte().
	// They must NOT be used if using interrupt-driven I/O except
	// within the ISR itself.
	
	byte SendByte()
	{
		byte	*pb;

		pb = m_pbNextXmitOut;
		if (m_pbNextXmitIn != pb)
		{
			pb++;
			if (pb == m_pbXmitBufEnd)
				pb = GetXmitBuf();

			m_pbNextXmitOut = pb;
			return *pb;
		}
		return 0;
	}
	
	bool IsByteToSend()
	{
		return m_pbNextXmitIn != m_pbNextXmitOut;
	}
	
	int BytesToSend()
	{
		byte	*pbIn;
		byte	*pbOut;

		pbIn = m_pbNextXmitIn;
		pbOut = m_pbNextXmitOut;
		if (pbIn < pbOut)
			pbIn += m_pbXmitBufEnd - GetXmitBuf();
		return pbIn - pbOut;
	}
	
	void DiscardSendBuf()
	{
		m_pbNextXmitOut = m_pbNextXmitIn;
	}
	
	void ReceiveByte(byte bDat)
	{
		byte	*pb;

		pb = m_pbNextRcvIn + 1;
		if (pb == m_pbRcvBufEnd)
			pb = GetRcvBuf();

		// Throw away if buffer is full
		if (pb == m_pbNextRcvOut)
			return;

		*pb = bDat;
		m_pbNextRcvIn = pb;
	}
	
	bool CanReceiveByte()
	{
		byte	*pb;

		pb = m_pbNextRcvIn + 1;
		if (pb == m_pbRcvBufEnd)
			pb = GetRcvBuf();

		return pb != m_pbNextRcvOut;		
	}
	
	int BytesCanReceive()
	{
		byte	*pbIn;
		byte	*pbOut;

		pbOut = m_pbNextRcvOut;
		pbIn = m_pbNextRcvIn;
		if (pbOut <= pbIn)
			pbOut += m_pbRcvBufEnd - GetRcvBuf();
		return pbOut - pbIn - 1;
	}

	//************************************************************************

protected:
	void Init(int cbRcvBuf, int cbXmitBuf)
	{
		m_pbNextRcvIn = m_pbNextRcvOut = GetRcvBuf(); 
		m_pbRcvBufEnd =  GetRcvBuf() + cbRcvBuf;
		m_pbNextXmitOut = m_pbNextXmitIn = GetXmitBuf();
		m_pbXmitBufEnd = GetXmitBuf() + cbXmitBuf;
	}
	
	byte	*GetRcvBuf()	{return m_arbRcvBuf;}
	byte	*GetXmitBuf()	{return m_pbRcvBufEnd;}

protected:
	// Constants for a given IoBuf instance
	void	*m_pvIO;	// memory-mapped IO port address
	byte	*m_pbRcvBufEnd;
	byte	*m_pbXmitBufEnd;
protected:
	// Buffer management
	byte	*m_pbNextRcvOut;
	byte	*m_pbNextXmitIn;

	// affected by interrupt service routines
	byte	*volatile m_pbNextRcvIn;
	byte	*volatile m_pbNextXmitOut;

protected:
	// Receive buffer
	byte	m_arbRcvBuf[0];
};
