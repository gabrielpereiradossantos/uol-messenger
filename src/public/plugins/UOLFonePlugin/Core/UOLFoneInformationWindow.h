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


#include "WTLWndComponentsOrganizer.h"
#include "IUOLMessengerPluginCustomTabChildWindow.h"
#include "UOLFoneDegradeWindow.h"
#include "AutoResizeStatic.h"
#include "UOLFoneMultiStateButton.h"
#include "UOLFoneChildWindowInfo.h"
#include "WebBrowserWnd.h"




/////////////////////////////////////////////////////////////////////
// CUOLFoneInformationWindow
/////////////////////////////////////////////////////////////////////


class CUOLFoneInformationWindow;


class CUOLFoneInformationWindow : 
		public CUOLFoneDegradeWindow,
		public IUOLMessengerPluginCustomTabChildWindow,
		public IWebBrowserWindowHolder
{
public:
	
	DECLARE_WND_CLASS(NULL)
	
	CUOLFoneInformationWindow();
	virtual ~CUOLFoneInformationWindow();
	
	virtual void GetMinSize(CSize& size);
	
	virtual void UpdateAccountInfo(CUOLFoneAccountPtr pAccount);
	virtual void UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall);
	
	virtual void SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo);
	
	virtual BOOL CanGetFocus();
	
	virtual void ResetWindow();
	
protected:
	enum
	{
		ID_BUTTON_SAIBA_MAIS = 0x2587,
	};
	
	BEGIN_MSG_MAP(CUOLFoneInformationWindow27)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_HANDLER_EX(ID_BUTTON_SAIBA_MAIS, BN_CLICKED, OnButtonClicked)
		//MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, OnCtlColor)
		CHAIN_MSG_MAP(CUOLFoneDegradeWindow)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	//LRESULT OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	BOOL ShouldCancelNavigation(CString strURL);
	
private:
	
	// Interface elements.
	WTL::CStatic				m_uolfoneLabel;
	WTL::CStatic				m_separator1;
	CAutoResizeStatic			m_infoLabel;
	WTL::CStatic				m_separator2;
	WTL::CButton				m_saibaMaisButton;
	CUOLFoneMultiStateButton	m_saibaMaisMultiStateButton;
	
	WTL::CFont					m_uolfoneFont;
	WTL::CFont					m_infoFont;
	
	CWTLWndComponentsOrganizer	m_componentsOrganizer;
	
	CString							m_infoText;
	CString							m_knowMoreURL;
	ITabChildWindowActionCallback*	m_pActionCallback;
	UINT							m_nActionId;
	
	POSITION						m_saibaMaisButtonLine;
	POSITION						m_saibaMaisButtonComponent;
	
	CWebBrowserWindow				m_webBrowserWnd;
	
	POSITION						m_webBrowserWindowLine;
};

MAKEAUTOPTR(CUOLFoneInformationWindow);

