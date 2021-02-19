//****************************************************************************
// KeyboardMgr.h
//
// Created 2/10/2021 10:44:29 AM by Tim
//
//****************************************************************************

#pragma once

#include "HotspotList.h"


typedef	void KbUser(void *pv, uint key);


class KeyboardMgr
{
	//*********************************************************************
	// Public interface
	//*********************************************************************3
public:
	static void KeyHit(uint key)
	{
		if (m_pUser == NULL)
			return;

		switch (key)
		{
		case Key_normal:
			ShowKb(KeyLower);
			break;

		case Key_shift:
			if (m_kbState == Key_shift || m_kbState == Key_shiftLock)
			{
				ShowKb(KeyLower);
				key = Key_normal;
			}
			else
				ShowKb(KeyUpper);
			break;

		case Key_shiftLock:
			if (m_kbState == Key_shiftLock)
			{
				ShowKb(KeyLower);
				key = Key_normal;
			}
			else
				ShowKb(KeyUpper);
			break;

		case Key_symbols:
			ShowKb(KeySymbol);
			break;

		default:
			if (key >= ' ' && key < 0x7F && m_kbState == Key_shift)
			{
				m_kbState = Key_normal;
				ShowKb(KeyLower);
			}
			m_pUser(m_pUserData, key);
			return;
		}
		m_kbState = key;
	}

	static void OpenKb(KbUser *pUser, void *pv = NULL)
	{
		m_pUser = pUser;
		m_pUserData = pv;
		m_oldPip = *ScreenMgr::GetPip2();
		// Start with upper case for easy capitalization
		m_kbState = Key_shift;
		ShowKb(KeyUpper);
	}

	static void CloseKb()
	{
		if (m_oldPip.IsEnabled())
			ScreenMgr::EnablePip2(m_oldPip.pImage, m_oldPip.x, m_oldPip.y);
		else
			ScreenMgr::DisablePip2();
	}

	//*********************************************************************
	// Helpers
	//*********************************************************************
protected:
	static void ShowKb(TouchCanvas &canvas)
	{
		ScreenMgr::EnablePip2(&canvas, KeyboardLeft, KeyboardTop);
	}

	//*********************************************************************
	// static (RAM) data
	//*********************************************************************
protected:
	inline static KbUser	*m_pUser;
	inline static void		*m_pUserData;
	inline static PipInfo	m_oldPip;
	inline static byte		m_kbState;
};
