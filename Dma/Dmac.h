//****************************************************************************
// Dmac.h
//
// Created 3/28/2020 9:42:03 AM by Tim
//
//****************************************************************************

#pragma once


// Define ISR. Note this requires a variable named Dma to be declared.
#define DEFINE_DMAC_ISR	void DMAC_Handler() { Dma.Isr(); }

// Use this macro to set the ISR for a channel to a member function
#define DMAC_SET_CHAN_ISR_METHOD(chan, pfn) \
	typedef typeof(*this) typ; \
	union {void (typ::*mf)(); Dmac_Isr_t *p;} u = {&typ::pfn}; \
	Dma.SetChanIsr(chan, u.p, this)

		
typedef void Dmac_Isr_t(void *pv);

template <int iChanCnt, bool fEnableInt = true> 
class Dmac_t
{
	// Types
	struct IsrDesc
	{
		Dmac_Isr_t	*isr;
		void		*pv;
	};

	// Methods
public:
	static void Init()
	{
		DMAC->BASEADDR.reg = (uint)Desc;
		DMAC->WRBADDR.reg = (uint)Writeback;
		DMAC->CTRL.reg = DMAC_CTRL_LVLEN3 | DMAC_CTRL_LVLEN2 | DMAC_CTRL_LVLEN1 | DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE;
		if (fEnableInt)
		{
			for (int i = 0; i < iChanCnt; i++)
				SetChanIsr(i, DummyIsr);
			NVIC_EnableIRQ(DMAC_IRQn);
		}
	}

	static void SetChanIsr(int iChan, Dmac_Isr_t *pfn, void *pv = NULL)
	{
		m_arIsr[iChan].isr = pfn;
		m_arIsr[iChan].pv = pv;
	}

	static void Isr()
	{
		DMAC_INTPEND_Type	pend;
		IsrDesc		*pIsr;

		for (;;)
		{
			pend.reg = DMAC->INTPEND.reg;
			if ((pend.reg & (DMAC_INTPEND_SUSP | DMAC_INTPEND_TCMPL | DMAC_INTPEND_TERR)) == 0)
				break;
			pIsr = &m_arIsr[pend.bit.ID];
			pIsr->isr(pIsr->pv);
		}
	}

	// Dummy ISR to fill array of handlers
	static void DummyIsr(void *pv)	{}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************

private:
	inline static IsrDesc m_arIsr[fEnableInt ? iChanCnt : 0];

public:
	inline static DmacDescriptor	Desc[iChanCnt]  __attribute__ ((aligned (16)));
	inline static DmacDescriptor	Writeback[iChanCnt]  __attribute__ ((aligned (16)));
};
