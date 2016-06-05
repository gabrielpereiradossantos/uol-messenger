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


#include "IUOLMessengerPluginCustomTabChildWindowContainer.h"
#include "IUOLMessengerPluginCustomTabChildWindow.h"
#include "interfaces/IUOLMessengerMainFrameFocusNotifier.h"

#include "UOLFoneInitializingWindow.h"
#include "UOLFoneKeyboardWindow.h"
#include "UOLFoneCallInProgressWindow.h"
#include "UOLFoneAcceptCallWindow.h"
#include "UOLFoneCallInPlaceWindow.h"
#include "UOLFoneCallErrorWindow.h"
#include "UOLFoneInformationWindow.h"


typedef CWinTraits<WS_CHILD | WS_CLIPCHILDREN, WS_EX_TOPMOST> CUOLFoneChildWindowContainerTraits;

struct ChildWindowInfo
{
	IUOLMessengerPluginCustomTabChildWindow* pTabChildWindow;
	CWindow*	pChildCWindow;
};


class CUOLFoneChildWindowContainerView : 
		public CWindowImpl<CUOLFoneChildWindowContainerView, CWindow, CUOLFoneChildWindowContainerTraits>, 
		public IUOLMessengerPluginCustomTabChildWindowContainer, 
		public CUOLMessengerMainFrameFocusObserver,
		public COrganizerWindow
{
public:
	enum
	{
		UOLFONE_KEYBOARD_WINDOW	= 2123,
		UOLFONE_CALL_IN_PROGRESS_WINDOW, 
		UOLFONE_ACCEPT_CALL_WINDOW, 
		UOLFONE_CALL_IN_PLACE_WINDOW, 
		UOLFONE_CALL_ERROR_WINDOW,
		UOLFONE_INITIALIZING_WINDOW,
		UOLFONE_INFORMATION_WINDOW
	};
	
	CUOLFoneChildWindowContainerView(void);
	virtual ~CUOLFoneChildWindowContainerView(void);
	
	// IUOLMessengerPluginCustomTabChildWindowContainer members
	virtual IUOLMessengerPluginCustomTabChildWindow* ShowChildWindow(int childWindowID, 
			CUOLFoneChildWindowInfo* pChildWindowInfo);
	virtual void GetMinSize(CSize& size);
	virtual void UpdateAccountInfo(CUOLFoneAccountPtr pAccount);
	virtual void UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall);
	virtual IUOLMessengerPluginCustomTabChildWindow* GetCurrentChildWindow();
	
	// CUOLMessengerMainFrameFocusObserver members
	virtual void OnSetFocus(HWND lostFocusWnd);
	virtual void OnKillFocus(HWND newFocusWnd);
	
	virtual CWindow* GetChildWindowPtr(int childWindowID);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneChildWindowContainerView)
		CHAIN_MSG_MAP(COrganizerWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MSG_WM_ERASEBKGND(OnEraseBackground)
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBackground(HDC hDc);
	
	void AddChildWindowInfoItem(int nItem, IUOLMessengerPluginCustomTabChildWindow* pTabChildWindow, CWindow* pChildCWindow);
	
	ChildWindowInfo* CreateChildWindowInfoItem(
			IUOLMessengerPluginCustomTabChildWindow* pTabChildWindow, 
			CWindow* pChildCWindow);
	
	void HandleChildWindowFocus();
	
private:
	
	CUOLFoneInitializingWindow				m_initializingWnd;
	CUOLFoneKeyboardWindow					m_keyboardWnd;
	CUOLFoneCallInProgressWindow			m_callInProgressWnd;
	CUOLFoneAcceptCallWindow				m_acceptCallWnd;
	CUOLFoneCallInPlaceWindow				m_callInPlaceWnd;
	CUOLFoneCallErrorWindow					m_callErrorWnd;
	CUOLFoneInformationWindow				m_informationWnd;
	
	typedef CAtlMap<int, ChildWindowInfo*> CWindowMap;
	
	CAtlList<ChildWindowInfo*>	m_uolfoneChildWindowsList;
	CWindowMap					m_uolfoneChildWindowsMap;
	ChildWindowInfo*			m_showingChildWindow;
	
	BOOL						m_showingWindow;
};

