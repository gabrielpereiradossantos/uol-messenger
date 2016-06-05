/* UOL Messenger
 * Copyright (c) 2005 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 * sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 * qualquer versão posterior.
 * Este programa é distribuído na expectativa de que seja útil, porém,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 * OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 * com este programa; se não, escreva para a Free Software Foundation, Inc.,
 * no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Messenger 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */
 


#pragma once


enum
{
	WM_ON_CREATE_EVENT_NOTIFIER = WM_USER + 0x234, 
	WM_ON_CHAR_EVENT_NOTIFIER, 
	WM_ON_SETFOCUS_EVENT_NOTIFIER, 
	WM_ON_KILLFOCUS_EVENT_NOTIFIER, 
	WM_ON_CTLCOLOR_EVENT_NOTIFIER
};


template<class T>
class CWMEventNotifier
{
public:
	
	CWMEventNotifier()
	{
		m_bSendNewMessage = TRUE;
		
		m_hNotifyingWnd = NULL;
		
		m_pT = (T*)this;
	}
	
	void SetNotifyingWindow(HWND hWnd)
	{
		m_hNotifyingWnd = hWnd;
	}
	
	void SetSendNewMessage(BOOL bSendNewMessage)
	{
		m_bSendNewMessage = bSendNewMessage;
	}
	
protected:
	
	BEGIN_MSG_MAP(CWMEventNotifier<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, OnCtlColor)
	END_MSG_MAP()
	
	virtual void	OnCreateEventNotifier() {};
	virtual void	OnCharEventNotifier(UINT nChar) {};
	virtual void	OnSetFocusEventNotifier(HWND hLostFocusWnd) {};
	virtual void	OnKillFocusEventNotifier(HWND hGetFocusWnd) {};
	virtual HBRUSH	OnCtlColorEventNotifier(UINT uMsg, HDC hdc, HWND hWnd) { return NULL; };
	
	inline LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		ATLTRACE(_T("CWMEventNotifier::OnCreate()\n"));
		
		ATLASSERT(m_pT != NULL);
		
		if (m_bSendNewMessage)
		{
			if (m_hNotifyingWnd)
			{
				PostMessage(m_hNotifyingWnd, WM_ON_CREATE_EVENT_NOTIFIER, 0, 0);
			}
			else if (m_pT != NULL)
			{
				PostMessage(m_pT->m_hWnd, WM_ON_CREATE_EVENT_NOTIFIER, 0, 0);
			}
		}
		else
		{
			OnCreateEventNotifier();
		}
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		ATLTRACE(_T("CWMEventNotifier::OnChar()\n"));
		
		if (m_bSendNewMessage)
		{
			if (m_hNotifyingWnd)
			{
				PostMessage(m_hNotifyingWnd, WM_ON_CHAR_EVENT_NOTIFIER, wParam, 0);
			}
			else if (m_pT != NULL)
			{
				PostMessage(m_pT->m_hWnd, WM_ON_CHAR_EVENT_NOTIFIER, wParam, 0);
			}
		}
		else
		{
			OnCharEventNotifier((UINT)wParam);
		}
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		ATLTRACE(_T("CWMEventNotifier::OnSetFocus()\n"));
		
		if (m_bSendNewMessage)
		{
			if (m_hNotifyingWnd)
			{
				PostMessage(m_hNotifyingWnd, WM_ON_SETFOCUS_EVENT_NOTIFIER, wParam, 0);
			}
			else if (m_pT != NULL)
			{
				PostMessage(m_pT->m_hWnd, WM_ON_SETFOCUS_EVENT_NOTIFIER, wParam, 0);
			}
		}
		else
		{
			OnSetFocusEventNotifier((HWND)wParam);
		}
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		ATLTRACE(_T("CWMEventNotifier::OnKillFocus()\n"));
		
		if (m_bSendNewMessage)
		{
			if (m_hNotifyingWnd)
			{
				PostMessage(m_hNotifyingWnd, WM_ON_KILLFOCUS_EVENT_NOTIFIER, wParam, 0);
			}
			else if (m_pT != NULL)
			{
				PostMessage(m_pT->m_hWnd, WM_ON_KILLFOCUS_EVENT_NOTIFIER, wParam, 0);
			}
		}
		else
		{
			OnKillFocusEventNotifier((HWND)wParam);
		}
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLTRACE(_T("CWMEventNotifier::OnCtlColor()\n"));
		
		HBRUSH retBrush = NULL;
		
		if (m_bSendNewMessage)
		{
			if (m_hNotifyingWnd)
			{
				PostMessage(m_hNotifyingWnd, WM_ON_CTLCOLOR_EVENT_NOTIFIER, wParam, lParam);
			}
			else if (m_pT != NULL)
			{
				PostMessage(m_pT->m_hWnd, WM_ON_CTLCOLOR_EVENT_NOTIFIER, wParam, lParam);
			}
		}
		else
		{
			retBrush = OnCtlColorEventNotifier(uMsg, (HDC)wParam, (HWND)lParam);
		}
		
		if (retBrush)
		{
			bHandled = TRUE;
		}
		else
		{
			bHandled = FALSE;
		}
		
		return (LRESULT) retBrush;
	}
	
private:
	
	T			*m_pT;
	
	HWND		m_hNotifyingWnd;
	
	BOOL		m_bSendNewMessage;
};

