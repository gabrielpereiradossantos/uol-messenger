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
 
// uimView.cpp : implementation of the CUimView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uimView.h"

#include "AccountButton.h"
#include "IPainterRepository.h"
#include "image/imagebuilder.h"
#include "skin/ProtocolImageElement.h"
#include "AccountConnection/ConnectionErrorWindowBuilder.h"
#include "../controller/UIMApplication.h"
#include "../controller/uimanager.h"
#include "ContactListTab.h"
#include "UOLToolbarBuilder.h"

#include <commands/openbrowsercommand.h>
#include <commands/openbrowserauthenticatedcommand.h>
#include <commands/showpreferencescommand.h>
#include <commands/disconnectallaccountscommand.h>
#include <commands/connectallaccountscommand.h>
#include <commands/minimizewindowcommand.h>
#include <commands/maximizewindowcommand.h>
#include <commands/closewindowcommand.h>
#include <commands/closeappcommand.h>
#include <commands/setbusystatusallcommand.h>
#include <commands/setawaystatusallcommand.h>
#include <commands/setofflinestatusallcommand.h>
#include <commands/setonlinestatusallcommand.h>
#include <commands/showaboutdialogcommand.h>
#include <commands/showhelpcommand.h>
#include <commands/hidewindowcommand.h>
#include <commands/showwindowcommand.h>
#include <commands/changeavatarcommand.h>
#include <commands/changenicknamecommand.h>
#include <commands/ChangeStatusMessageCommand.h>


class CCommandFactory
{
public:
	static IUOLMessengerCommandPtr GetCommand(const CString& strCommand, CWindow* pWindow);
};
#include <commands/RegisterUOLKCommand.h>

#include <improtocolconstants.h>

// Debug code
//#include "PluginTest/DummyPluginCustomPanel.h"
// Debug code


// ------------------------------------------------------------

const int CUimView::MAX_TABS_COUNT = 10;


CUimView::CUimView() : 
	m_nReconnectTimer(0)
{
	m_pApplication = CUIMApplication::GetApplication();
	
	m_mainWindowEventsNotifier = new CMainWindowEventsNotifier();
}


CUimView::~CUimView()
{
	if (m_pTabView.get())
	{
		m_pTabView->UnregisterObserver(this);
	}
}


BOOL CUimView::PreTranslateMessage(MSG* pMsg)
{
	// << debug code
	//return FALSE;
	// >> debug code
	// Pass keyboard messages along to the child window that has the focus.
	// When the browser has the focus, the message is passed to its containing
	// CAxWindow, which lets the control handle the message.
	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
	{
		return FALSE;
	}

	HWND hWndCtl = ::GetFocus();

	if (IsChild(hWndCtl))
	{
		if(::TranslateAccelerator(m_hWnd, m_haccelerator, pMsg))
			return TRUE;

		// Find a direct child of the dialog from the window that has focus
		while (::GetParent(hWndCtl) != m_hWnd)
		{
			hWndCtl = ::GetParent(hWndCtl);
		}

		// Give control a chance to translate this message
		if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
		{
			return TRUE;
		}
	}

	// A normal control has the focus, so call IsDialogMessage() so that
	// the dialog shortcut keys work (TAB, etc.)
	//return IsDialogMessage(pMsg);
	return FALSE;
}



LRESULT CUimView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	m_pApplication->GetSettingsManager()->GetContactListSettings()->AddEventListener(this);
	
	m_mainFrameFocusNotifier = new CMainFrameFocusNotifier();

	CreateUserStatusPanel();

	//CreateToolbarTab();
	CreateTabView();

	// A criacao destes dois elementos deve sempre ficar nessa ordem, e apos a 
	// criacao do TabView.
	CreateContactListTab();
	CreatePluginCustomTab();

	CreateAccountsPanel();

	CreateStatusContextMenu();
	/*
	// No need for menus. Just one option now.
	CreateAvatarContextMenu();
	CreateNicknameContextMenu();
	*/
	
	CreateTrayWindow();
	CreateTrayWindowContextMenu();

	// Register as account list event listener
	m_pApplication->GetAccountManager()->AddAccountListEventListener(this);

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pApplication->GetAccountManager()->GetAccountList(listAccounts);

	// Initialize avatar and nickname text
	m_pUserAvatar = new CUserAvatar();
	m_pUserAvatar->AddEventListener(this);
	m_pUserStatusPanel->SetAvatar(m_pUserAvatar);

	if (listAccounts.GetCount() > 0)
	{
		IUOLMessengerAccountPtr pFirstAccount = listAccounts.GetHead();
		UpdateUserInfo(pFirstAccount.get());
		UpdateUserStatus(pFirstAccount);

		m_pCurrentAccount = pFirstAccount;
	}

	if (m_pAccountsToolbarWrapper.get())
	{
		m_pAccountsToolbarWrapper->Initialize();
		m_pAccountsToolbarWrapper->AddEventListener(this);
		
		if ((m_pUserAccountsToolbar.get()) && (m_pUserAccountsToolbar->IsWindow()))
		{
			// Build account icons list and register as user input handler

			for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
			{
				IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
				pAccount->RegisterAccountObserver(this);

				m_pApplication->GetAccountConnectionManager()->RegisterAccountObserver(pAccount, this);

				m_pAccountsToolbarWrapper->AddAccountButton(pAccount);
			}
		}
	}
	
	m_pApplication->GetPrivacyManager()->RegisterObserver(this);

	IUOLMessengerInternetConnectionMonitorPtr pInternetConnectionMonitor = m_pApplication->GetInternetConnectionMonitor();

	if (pInternetConnectionMonitor)
	{
		pInternetConnectionMonitor->RegisterCallback(this);

		if (!pInternetConnectionMonitor->IsConnected())
		{
			PostMessage(WM_USER_INTERNET_CONNECTION_STATE_CHANGED, FALSE, NULL);
		}
	}

	ATL::_U_STRINGorID table = IDR_MAINFRAME;
	m_haccelerator = ::LoadAccelerators(ATL::_AtlBaseModule.GetResourceInstance(), table.m_lpstr);
	
	// Send the post create message.
	::PostMessage(m_hWnd, WM_POS_CREATE_MESSAGE, 0, 0);
	
	return 0L;
}


void CUimView::OnDestroy()
{
	DestroyTrayWindow();

	m_pApplication->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(this);

	if (m_pAccountsToolbarWrapper.get())
	{
		m_pAccountsToolbarWrapper->RemoveEventListener(this);
		m_pAccountsToolbarWrapper->Finalize();
	}

	// Unregister as account list event listener
	m_pApplication->GetAccountManager()->RemoveAccountListEventListener(this);

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pApplication->GetAccountManager()->GetAccountList(listAccounts);

	// Unregister as account observer
	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
		pAccount->UnregisterAccountObserver(this);

		m_pApplication->GetAccountConnectionManager()->UnregisterAccountObserver(pAccount, this);
	}
	
	m_pApplication->GetPrivacyManager()->UnregisterObserver(this);
	
	IUOLMessengerInternetConnectionMonitorPtr pInternetConnectionMonitor = m_pApplication->GetInternetConnectionMonitor();

	if (pInternetConnectionMonitor)
	{
		pInternetConnectionMonitor->UnregisterCallback(this);
	}

	if (m_pFrameConnectionError)
	{
		CWindow wndFrameWindow = m_pFrameConnectionError->GetHWND();

		if (wndFrameWindow.IsWindow())
		{
			wndFrameWindow.DestroyWindow();
		}
	}

	if (0 != m_nReconnectTimer)
	{
		KillTimer(m_nReconnectTimer);
	}

	SetMsgHandled(FALSE);
}



void CUimView::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
{
	if (nTimerId == m_nReconnectTimer)
	{
		BOOL bKilled = KillTimer(m_nReconnectTimer);
		ATLASSERT(bKilled);

		m_nReconnectTimer = 0;

		if (m_pConnectionErrorView && (m_pConnectionErrorView->IsWindow()))
		{
			//::OutputDebugString(_T("Reconnecting all accounts"));

			m_pConnectionErrorView->ReconnectAll();
		}
	}
}


LRESULT CUimView::OnUpdateLayout(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_pPluginCustomTabContainerView && 
		(0 == m_pPluginCustomTabContainerView->GetTabsCount()))
	{
		CContactListTabPtr pContactListTab = GetContactListTab();

		if (pContactListTab)
		{
			pContactListTab->ShowContactListTab();
		}
	}

	UpdateLayout();

	return 0L;
}


LRESULT CUimView::OnInternetConnectionStateChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	BOOL bConnected = (BOOL) wParam;

	if (bConnected)
	{
		//::OutputDebugString(_T("Now connected\n"));
		ATLTRACE(_T("Now connected\n"));

		if (m_pConnectionErrorView && (m_pConnectionErrorView->IsWindow()))
		{
			m_nReconnectTimer = SetTimer(TIMER_RECONNECT_ACCOUNTS, 10000);
		}
	}

	return 0L;
}


void CUimView::OnConversationTabSelected(IUOLMessengerTabViewCtrl* pTabView, 
		IUOLMessengerConversationPtr pConversation, int nTabIndex )
{
	if (nTabIndex == 0)
	{
		// Firstly, we hide the custom tabs.
		m_pPluginCustomTabContainerView->HideAllTabs();
		
		// Right after, we show the contact list up.
		m_pContactListTab->ShowContactListTab();
	}
	else
	{
		// Firstly, we hide the contact list.
		m_pContactListTab->HideContactListTab();
		
		// Right after, we show the custom tabs.
		IUOLMessengerUIManager2Ptr pUIManager2Ptr = 
				ap_static_cast<IUOLMessengerUIManager2Ptr>(CUIMApplication::GetApplication()->GetUIManager());
		
		if (pUIManager2Ptr)
		{
			IUOLMessengerPluginCustomTabContainerPtr pCustomTabContainer = pUIManager2Ptr->GetPluginCustomTabContainer();

			if (pCustomTabContainer)
			{
				IUOLMessengerPluginCustomTabContainer2Ptr pCustomTabContainer2 = ap_dynamic_cast<IUOLMessengerPluginCustomTabContainer2Ptr>(pCustomTabContainer);
			
				if (pCustomTabContainer2)
				{
					pCustomTabContainer2->ShowTab(nTabIndex);
				}
			}
		}
	}
}

void CUimView::OnConversationTabMouseHover(IUOLMessengerTabViewCtrl* pTabView, 
		IUOLMessengerConversationPtr pConversation, CPoint& pt, HWND wHandle)
{
}

void CUimView::OnConversationTabMouseLeave()
{
}

LRESULT CUimView::OnShowConnectionError(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	BOOL bCreateFrame = TRUE;
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	if (m_pFrameConnectionError)
	{
		CWindow wndFrameWindow = m_pFrameConnectionError->GetHWND();

		bCreateFrame = (FALSE == wndFrameWindow.IsWindow());
	}

	if (bCreateFrame)
	{
		CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(m_pApplication->GetUIManager());

		CConnectionErrorWindowBuilderPtr pConnectionErrorWindowBuilder = ap_dynamic_cast<CConnectionErrorWindowBuilderPtr>(pUIManager->GetWindowManager()->GetElement("__ConnectionErrorWindowBuilder"));

		ATLASSERT(pConnectionErrorWindowBuilder);
		if (!pConnectionErrorWindowBuilder)
		{
			return 0L;
		}

		m_pConnectionErrorView = ap_dynamic_cast<CConnectionErrorViewPtr>(pConnectionErrorWindowBuilder->CreateElement());

		m_pFrameConnectionError = m_pApplication->GetUIManager()->CreateFrameWindow(m_pConnectionErrorView, 
			CRect(CPoint(0, 0), CSize(430, 370)), m_hWnd, 0);

		CString strAppName;
		strAppName = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

		CString strCaption;
		strCaption.Format(IDS_CONNECTION_ERROR_DIALOG_CAPTION, strAppName);

		m_pFrameConnectionError->SetCaptionText(strCaption);
	}

	if (m_pFrameConnectionError)
	{
		CWindow wndFrameWindow = m_pFrameConnectionError->GetHWND();

		if (wndFrameWindow.IsWindow())
		{
			if (bCreateFrame)
			{
				wndFrameWindow.CenterWindow(::GetDesktopWindow());
			}

			wndFrameWindow.ShowWindow(SW_NORMAL);

			LPSHOWCONNECTIONERRORPARAM pScep = (LPSHOWCONNECTIONERRORPARAM) wParam;

			m_pConnectionErrorView->AddErrorEvent(pScep->pAccount, pScep->error, CTime::GetCurrentTime(), pScep->message);
		}
	}

	return 0L;
}


LRESULT CUimView::OnSetFocus(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_mainFrameFocusNotifier->SetFocus((HWND)wParam);
	
	return 0L;
}


LRESULT CUimView::OnKillFocus(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_mainFrameFocusNotifier->KillFocus((HWND)wParam);
	
	return 0L;
}


LRESULT CUimView::OnShowWindow(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// Send the post showwindow message.
	::PostMessage(m_hWnd, WM_POS_SHOWWINDOW_MESSAGE, wParam, 0);
	
	return 0L;
}


LRESULT CUimView::OnPosCreateMessage(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	m_mainWindowEventsNotifier->MainWindowPosCreate();

	// Connect only auto connect enabled accounts
	m_pApplication->ExecuteCommand(new CConnectAllAccountsCommand(TRUE));
	
	return 0L;
}


LRESULT CUimView::OnPosShowWindowMessage(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	m_mainWindowEventsNotifier->MainWindowPosShowWindow((BOOL)wParam);
	
	return 0L;
}


LRESULT CUimView::OnChar(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_mainWindowEventsNotifier->MainWindowChar((UINT)wParam);
	
	SetMsgHandled(FALSE);
	
	return 0L;
}


void CUimView::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateUserInfo(pAccount.get());

	m_pCurrentAccount = pAccount;

	if (m_pConnectionErrorView && (m_pConnectionErrorView->IsWindow()))
	{
		m_pConnectionErrorView->RemoveErrorEvent(pAccount);
		
		UINT nErrorEventCount = m_pConnectionErrorView->GetErrorEventCount();
		if (0 == nErrorEventCount)
		{
			ATLASSERT(m_pFrameConnectionError);
			CWindow wndFrameWindow = m_pFrameConnectionError->GetHWND();
			wndFrameWindow.PostMessage(WM_CLOSE);
		}
	}

	IUOLMessengerGeneralSettingsPtr pGeneralSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings();

	if (pGeneralSettings)
	{
		if ((pGeneralSettings->IsOpenBrowserEnabled()) && 
			(IsOpenBrowserEnabled()))
		{
			EnableOpenBrowser(FALSE);

			m_pApplication->ExecuteCommand(new COpenBrowserCommand());
		}
	}
}


void CUimView::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	UINT nConnectingAccounts = 0;

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pApplication->GetAccountManager()->GetAccountList(listAccounts);
	
	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	DWORD dwState;
	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pTempAccount = listAccounts.GetNext(pos);

		dwState = pAccountConnectionManager->GetConnectionState(pTempAccount);
		if (dwState != IUOLMessengerAccountConnectionManager::CS_DISCONNECTED)
		{
			nConnectingAccounts++;
		}
	}

	if (0 == nConnectingAccounts)
	{
		EnableOpenBrowser(TRUE);
	}
}


void CUimView::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	SHOWCONNECTIONERRORPARAM scep;
	scep.pAccount = pAccount;
	scep.error = error;
	BOOL bSendErrorMsg = TRUE;

	if (0 == strcmp(_T("prpl-uol"), pAccount->GetProtocol()->GetId().c_str()))
	{
		scep.message = strMessage;

		if (error == CONNECTION_ERROR_REGISTRATION_REQUIRED)
		{
			IUOLMessengerCommandPtr pRegisterUOLKCommand = 
					new CRegisterUOLKCommand(pAccount);
			
			m_pApplication->ExecuteCommand(pRegisterUOLKCommand);
			
			bSendErrorMsg = !dynamic_cast<CRegisterUOLKCommand*>(
					pRegisterUOLKCommand.get())->RegisterSuccessfull();
			
			if (!bSendErrorMsg)
			{
				bSendErrorMsg = !CUIMApplication::GetApplication()->
						GetAccountConnectionManager()->Connect(pAccount);
			}
		}
	}
	/// <TODAOFERTA>
	else if (0 == strcmp(_T("prpl-uolto"), pAccount->GetProtocol()->GetId().c_str()))
	{
		scep.message = strMessage;
	}
	/// </TODAOFERTA>

	if (bSendErrorMsg)
	{
		SendMessage(WM_USER_SHOW_CONNECTION_ERROR, (WPARAM) &scep, NULL);
	}
}


void CUimView::OnStatusChanged(IUOLMessengerAccountPtr pAccount)
{
	if (pAccount.get() == m_pCurrentAccount.get())
	{
		UpdateUserInfo(pAccount.get());
	}
	
	UpdateUserStatus(pAccount);
}





void CUimView::OnDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	UpdateContact(pAccount, strContact);
}


void CUimView::OnDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	UpdateContact(pAccount, strContact);
}


void CUimView::OnNickNameChanged(IUOLMessengerAccount* pAccount)
{
	if (pAccount == m_pCurrentAccount.get())
	{
		UpdateUserInfo(pAccount);
	}
}


void CUimView::OnUserIconChanged(IUOLMessengerAccount* pAccount)
{
	if (pAccount == m_pCurrentAccount.get())
	{
		UpdateUserInfo(pAccount);
	}

}


void CUimView::OnStatusMessageChanged(IUOLMessengerAccount* pAccount)
{
	if (pAccount == m_pCurrentAccount.get())
	{
		UpdateUserInfo(pAccount);
	}
}


void CUimView::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	IUOLMessengerContactListSettingsPtr pContactListSettings = pApplication->GetSettingsManager()->GetContactListSettings();

	if (pSource == pContactListSettings.get())
	{
		if (IUOLMessengerContactListSettings::CL_TOPMOST == nSettingId)
		{
			pApplication->GetUIManager()->PlaceMainWindowAlwaysOnTop(pContactListSettings->IsTopMost());
		}
	}
}


void CUimView::OnRButtonDownAvatar(const CPoint& ptClick)
{
	/*
	// No need for menus. Just one option now.
	//
	// Show context menu
	CPoint ptMenu = ptClick;
	
	m_pUserStatusPanel->ClientToScreen(&ptMenu);
	m_pMenuAvatar->ShowMenu(ptMenu);
	*/
}


void CUimView::OnRButtonDownText(const CPoint& ptClick)
{
	/*
	// No need for menus. Just one option now.
	//
	CPoint ptMenu = ptClick;

	m_pUserStatusPanel->ClientToScreen(&ptMenu);
	m_pMenuNickname->ShowMenu(ptMenu);
	*/
}


void CUimView::OnLButtonDownAvatar(const CPoint& ptClick)
{
	m_pApplication->ExecuteCommand(new CChangeAvatarCommand(m_hWnd));
}


void CUimView::OnLButtonDownStatusMessage(const CPoint& ptClick)
{
	CString strOldStatusMessage;
	IUOLMessengerAccount3Ptr pAccount3;
	pAccount3 = ap_dynamic_cast<IUOLMessengerAccount3Ptr>(m_pCurrentAccount);

	if (pAccount3)
	{
		strOldStatusMessage = pAccount3->GetStatusMessage();
	}
	
	m_pApplication->ExecuteCommand(new CChangeStatusMessageCommand(m_hWnd, strOldStatusMessage));
}


void CUimView::OnLButtonDownText(const CPoint& ptClick)
{
	m_pApplication->ExecuteCommand(new CChangeNicknameCommand(m_hWnd, GetCurrentAccountNickname()));
}


void CUimView::OnLButtonDownStatusButton(const CPoint& ptClick)
{
	if (m_pApplication->GetAccountManager()->GetAccountListCount() > 0)
	{
		// Show context menu
		CPoint ptMenu;
		CRect rectStatus;

		rectStatus = this->m_pUserStatusPanel->GetRectStatus();

		ptMenu.x = rectStatus.left;
		ptMenu.y = rectStatus.bottom;

		m_pUserStatusPanel->ClientToScreen(&ptMenu);

		m_pMenuStatus->ShowMenu(ptMenu);
	}
}

void CUimView::OnLButtonDownAccountButton(const CPoint& ptClick, CAccountButtonPtr pButton)
{
}

void CUimView::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	IUOLMessengerCommandPtr pCommand = GetCommand(pButton->GetCommandName());

	if (pCommand)
	{
		m_pApplication->ExecuteCommand(pCommand);
	}
}

void CUimView::OnCommand(IUOLMessengerContextMenuItem* pItem)
{
	if (pItem != NULL)
	{
		int nCommandId = pItem->GetCommand();

		if (nCommandId == ID_CONNECT_ALL)
		{
			// Connect all accounts
			m_pApplication->ExecuteCommand(new CConnectAllAccountsCommand());
		}
		else if (nCommandId == ID_DISCONNECT_ALL)
		{
			// Disconnect all accounts
			m_pApplication->ExecuteCommand(new CDisconnectAllAccountsCommand());
		}
		else if (nCommandId == ID_SHOW_HELP)
		{
			m_pApplication->ExecuteCommand(new CShowHelpCommand());
		}
		else if (nCommandId == ID_APP_ABOUT)
		{
			m_pApplication->ExecuteCommand(new CShowAboutDialogCommand());
		}
		else if (nCommandId == ID_ESTADO_ONLINE)
		{
			m_pApplication->ExecuteCommand(new CSetOnlineStatusAllCommand());
		}
		else if (nCommandId == ID_ESTADO_OCUPADO)
		{
			m_pApplication->ExecuteCommand(new CSetBusyStatusAllCommand());
		}
		else if (nCommandId == ID_ESTADO_AUSENTE)
		{
			m_pApplication->ExecuteCommand(new CSetAwayStatusAllCommand());
		}
		else if (nCommandId == ID_ESTADO_OFFLINE)
		{
			m_pApplication->ExecuteCommand(new CSetOfflineStatusAllCommand());
		}
		else if (nCommandId == ID_CLOSE_WINDOW)
		{
			m_pApplication->ExecuteCommand(new CCloseAppCommand());
		}
		else if (nCommandId == ID_CHANGE_ALL_AVATAR)
		{
			m_pApplication->ExecuteCommand(new CChangeAvatarCommand(m_hWnd));
		}
		else if (nCommandId == ID_CHANGE_ONE_AVATAR)
		{
			m_pApplication->ExecuteCommand(new CChangeAvatarCommand(m_hWnd, m_pCurrentAccount));
		}
		else if (nCommandId == ID_CHANGE_ALL_NICKNAME)
		{
			m_pApplication->ExecuteCommand(new CChangeNicknameCommand(m_hWnd, GetCurrentAccountNickname()));
		}
		else if (nCommandId == ID_CHANGE_ONE_NICKNAME)
		{
			m_pApplication->ExecuteCommand(new CChangeNicknameCommand(m_hWnd, GetCurrentAccountNickname(), m_pCurrentAccount));
		}
	}
}


void CUimView::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
	// Register as account observer.
	//
	pAccount->RegisterAccountObserver(this);
	CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(pAccount, this);
	
	if (m_pAccountsToolbarWrapper.get())
	{
		m_pAccountsToolbarWrapper->AddAccountButton(pAccount);
	}
	
	if (m_pCurrentAccount.get() == NULL)
	{
		UpdateUserInfo(pAccount.get());
		UpdateUserStatus(pAccount);
		
		m_pCurrentAccount = pAccount;
	}
}


void CUimView::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	// Unregister as accout observer.
	//
	pAccount->UnregisterAccountObserver(this);
	CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(pAccount, this);
	
	// Remove account associated objects...
	//
	if (m_pAccountsToolbarWrapper.get())
	{
		m_pAccountsToolbarWrapper->RemoveAccountButton(pAccount);
	}
	
	// Change current account info to first account available or 
	// clear data
	if (m_pCurrentAccount.get() == pAccount.get())
	{
		CAtlList<IUOLMessengerAccountPtr> listAccounts;
		m_pApplication->GetAccountManager()->GetAccountList(listAccounts);

		if (listAccounts.GetCount() > 0)
		{
			IUOLMessengerAccountPtr pFirstAccount = listAccounts.GetHead();
			UpdateUserInfo(pFirstAccount.get());
			UpdateUserStatus(pFirstAccount);

			m_pCurrentAccount = pFirstAccount;
		}
		else
		{
			// Clear user info...
			m_pCurrentAccount = IUOLMessengerAccountPtr();
			m_pApplication->GetContactList()->RemoveAllGroups();
		}

		UpdateUserInfo(m_pCurrentAccount.get());
	}

	if (m_pConnectionErrorView && (m_pConnectionErrorView->IsWindow()))
	{
		m_pConnectionErrorView->RemoveErrorEvent(pAccount);
		
		UINT nErrorEventCount = m_pConnectionErrorView->GetErrorEventCount();
		if (0 == nErrorEventCount)
		{
			ATLASSERT(m_pFrameConnectionError);
			CWindow wndFrameWindow = m_pFrameConnectionError->GetHWND();
			wndFrameWindow.PostMessage(WM_CLOSE);
		}
	}
}


void CUimView::OnAccountUserIdChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldUserId)
{
	if (pAccount.get() == m_pCurrentAccount.get())
	{
		UpdateUserInfo(pAccount.get());
	}
}


void CUimView::OnLButtonDblClick(IUOLMessengerTrayWindow* pTray, const CPoint& ptClick)
{
	m_pApplication->ExecuteCommand(new CShowWindowCommand(GetParent()));
}


void CUimView::OnRButtonDown(IUOLMessengerTrayWindow* pTray, const CPoint& ptClick)
{
	// Show tray window context menu
	CPoint ptMenu = ptClick;

	BOOL bHasAccounts = (m_pApplication->GetAccountManager()->GetAccountListCount() > 0);

	m_pTrayWindowMenu->EnableItem(m_pTrayMenuStatus, bHasAccounts);
	m_pTrayWindowMenu->ShowMenu(ptMenu);
}


void CUimView::OnInternetConnectionStateChanged(BOOL bConnected)
{
	PostMessage(WM_USER_INTERNET_CONNECTION_STATE_CHANGED, (WPARAM) bConnected, NULL);
}


void CUimView::GetMinSize(CSize& size)
{
	int cx = 0;
	int cy = 0;
	
	if (m_pContactListTab.get() && (m_pContactListTab->GetWindow()))
	{
		CSize sizeContactListTab;
		m_pContactListTab->GetMinSize(sizeContactListTab);
		
		cx = sizeContactListTab.cx;
		cy += sizeContactListTab.cy;
	}	
	
	
	if (m_pPluginCustomTabContainerView.get())
	{
		CSize sizeTabContainerView;
		m_pPluginCustomTabContainerView->GetMinSize(sizeTabContainerView);
		
		cx = __max(cx, sizeTabContainerView.cx);
		cy = __max(cy, sizeTabContainerView.cy);
	}
	
	
	if (m_pUserStatusPanel.get() && (m_pUserStatusPanel->GetWindow()))
	{
		CSize sizeUserStatusPanel;
		m_pUserStatusPanel->GetMinSize(sizeUserStatusPanel);
		
		cy += sizeUserStatusPanel.cy;
	}
	
	
	if ( (m_pTabView.get()) && (m_pTabView->IsWindow()) )
	{
		CSize sizeTabViewPanel;
		m_pTabView->GetMinSize(sizeTabViewPanel);
		
		cy += sizeTabViewPanel.cy;
	}
	
	if ((m_pUserAccountsToolbar.get()) && (m_pUserAccountsToolbar->IsWindow()))
	{
		CSize sizeAccountsPanel;
		m_pUserAccountsToolbar->GetMinSize(sizeAccountsPanel);
		
		cy += sizeAccountsPanel.cy;
	}

	size.cx = cx;
	size.cy = cy;
}


IUOLMessengerCommandPtr CUimView::GetCommand(const CString& strCommand)
{
	return CCommandFactory::GetCommand(strCommand, this);
}

void CUimView::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());
	CSize sizeAccountsPanel;

	int y = 0;	

	if ((m_pUserStatusPanel.get()) && (m_pUserStatusPanel->IsWindow()))
	{
		CSize sizeElement;
		m_pUserStatusPanel->GetMinSize(sizeElement);

		m_pUserStatusPanel->SetWindowPos(NULL, 0, y, size.cx, sizeElement.cy,  SWP_NOZORDER);
		y += sizeElement.cy;
	}

	if ((m_pTabView.get()) && (m_pTabView->IsWindow()))
	{
		m_pTabView->ShowWindow(SW_SHOW);
		
		CSize sizeElement;
		m_pTabView->GetMinSize(sizeElement);

		m_pTabView->SetWindowPos(NULL, 0, y, size.cx, sizeElement.cy,  SWP_NOZORDER);
		y += sizeElement.cy;
	}

	if ((m_pUserAccountsToolbar.get()) && (m_pUserAccountsToolbar->IsWindow()))
	{
		m_pUserAccountsToolbar->GetMinSize(sizeAccountsPanel);

		m_pUserAccountsToolbar->SetWindowPos(NULL, 0, size.cy - sizeAccountsPanel.cy, size.cx, sizeAccountsPanel.cy,  SWP_NOZORDER);
		// The height of this panel is not added to "y" variable, but 
		// must be considered bellow when calculating the height of 
		// centered components.
	}

	if ((m_pContactListTab.get()) && (m_pContactListTab->IsWindow()))
	{

		m_pContactListTab->SetWindowPos(NULL, 
			0,			// xCoord
			y,			// yCoord
			size.cx,	// width
			(size.cy > y) ? (size.cy - y - sizeAccountsPanel.cy) : 0, // height
			SWP_NOZORDER);

	}

	if ((m_pPluginCustomTabContainerView.get()) && (m_pPluginCustomTabContainerView->IsWindow()))
	{

		m_pPluginCustomTabContainerView->SetWindowPos(NULL, 
			0,			// xCoord
			y,			// yCoord
			size.cx,	// width
			(size.cy > y) ? (size.cy - y - sizeAccountsPanel.cy) : 0, // height
			SWP_NOZORDER);

	}


}


void CUimView::DoPaint(CDCHandle& dc)
{
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));

	CRect rect;
	GetClientRect(&rect);

	dc.FillRect(&rect, brush);
}



void CUimView::CreateUserStatusPanel()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(m_pApplication->GetUIManager());	
	m_pUserStatusPanel = ap_dynamic_cast<CUserStatusPanelPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__UserStatusPanelInstance"));	

	CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
	m_pUserStatusPanel->Create(m_hWnd, rect);	

	// Register as event listener
	m_pUserStatusPanel->AddEventListener(this);
}

void CUimView::CreateContactListTab()
{
	m_pContactListTab = FindElement<CContactListTabPtr>();

	if (m_pContactListTab.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pContactListTab->Create(m_hWnd, rect);
		m_pContactListTab->Init(m_pTabView);
	}
}


void CUimView::CreateStatusContextMenu()
{
	m_pMenuStatus = new CContextMenu();

	if (m_pMenuStatus->CreateMenu())
	{
		// Add menu items...
		CString strText;
		
		struct MenuItem
		{
			UINT nCommandId;
			UINT nResourceStrId;
		};

		MenuItem vMenuItem[] = 
		{
			{ ID_ESTADO_ONLINE, IDS_STATUS_ONLINE },
			{ ID_ESTADO_OCUPADO, IDS_STATUS_BUSY },
			{ ID_ESTADO_AUSENTE, IDS_STATUS_AWAY },
			{ ID_ESTADO_OFFLINE, IDS_STATUS_INVISIBLE },
			{ ID_DISCONNECT_ALL, IDS_STATUS_OFFLINE }
		};

		for (int nInd = 0; nInd < sizeof(vMenuItem) / sizeof(vMenuItem[0]); nInd++)
		{
			strText.LoadString(vMenuItem[nInd].nResourceStrId);

			IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(vMenuItem[nInd].nCommandId, strText, this);
			m_pMenuStatus->AddItem(pMenuItem);
		}
	}
}


void CUimView::CreateAvatarContextMenu()
{
	m_pMenuAvatar = new CContextMenu();

	if (m_pMenuAvatar->CreateMenu())
	{
		// Add menu items...
		CString strText;
		
		struct MenuItem
		{
			UINT nCommandId;
			UINT nResourceStrId;
		};

		MenuItem vMenuItem[] = 
		{
			{ ID_CHANGE_ALL_AVATAR, IDS_CHANGE_ALL_AVATAR },
			{ ID_CHANGE_ONE_AVATAR, IDS_CHANGE_ONE_AVATAR }
		};

		for (int nInd = 0; nInd < sizeof(vMenuItem) / sizeof(vMenuItem[0]); nInd++)
		{
			strText.LoadString(vMenuItem[nInd].nResourceStrId);

			IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(vMenuItem[nInd].nCommandId, strText, this);
			m_pMenuAvatar->AddItem(pMenuItem);
		}
	}
}


void CUimView::CreateNicknameContextMenu()
{
	m_pMenuNickname = new CContextMenu();

	if (m_pMenuNickname->CreateMenu())
	{
		// Add menu items...
		CString strText;
		
		struct MenuItem
		{
			UINT nCommandId;
			UINT nResourceStrId;
		};

		MenuItem vMenuItem[] = 
		{
			{ ID_CHANGE_ALL_NICKNAME, IDS_CHANGE_ALL_NICKNAME },
			{ ID_CHANGE_ONE_NICKNAME, IDS_CHANGE_ONE_NICKNAME }
		};

		for (int nInd = 0; nInd < sizeof(vMenuItem) / sizeof(vMenuItem[0]); nInd++)
		{
			strText.LoadString(vMenuItem[nInd].nResourceStrId);

			IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(vMenuItem[nInd].nCommandId, strText, this);
			m_pMenuNickname->AddItem(pMenuItem);
		}
	}
}


void CUimView::CreateTrayWindow()
{
	CUOLMessengerTrayWindowPtr pTrayWindow = ap_dynamic_cast<CUOLMessengerTrayWindowPtr>(m_pApplication->GetTrayWindow());

	ATLASSERT(pTrayWindow.get());

	if (pTrayWindow.get())
	{
		pTrayWindow->Create(::GetDesktopWindow());
		pTrayWindow->AddTrayWindowEventListener(this);
	}
}


void CUimView::DestroyTrayWindow()
{
	CUOLMessengerTrayWindowPtr pTrayWindow = ap_dynamic_cast<CUOLMessengerTrayWindowPtr>(m_pApplication->GetTrayWindow());

	ATLASSERT(pTrayWindow.get());

	if (pTrayWindow.get())
	{
		pTrayWindow->RemoveTrayWindowEventListener(this);
		pTrayWindow->DestroyWindow();
	}
}


void CUimView::CreateTrayWindowContextMenu()
{
	m_pTrayWindowMenu = new CContextMenu();

	if (m_pTrayWindowMenu->CreateMenu())
	{
		CString	strLabel;
		IUOLMessengerContextMenuItem* pMenuItem;

		strLabel.LoadString(IDS_MENU_CHANGE_STATUS);

		m_pTrayMenuStatus = new CContextMenu(strLabel);

		if (m_pTrayMenuStatus->CreateMenu())
		{		
			strLabel.LoadString(IDS_STATUS_ONLINE);
			pMenuItem = new CContextMenuItem(ID_ESTADO_ONLINE, strLabel, this);
			m_pTrayMenuStatus->AddItem(pMenuItem);

			strLabel.LoadString(IDS_STATUS_BUSY);
			pMenuItem = new CContextMenuItem(ID_ESTADO_OCUPADO, strLabel, this);
			m_pTrayMenuStatus->AddItem(pMenuItem);

			strLabel.LoadString(IDS_STATUS_AWAY);
			pMenuItem = new CContextMenuItem(ID_ESTADO_AUSENTE, strLabel, this);
			m_pTrayMenuStatus->AddItem(pMenuItem);

			strLabel.LoadString(IDS_STATUS_INVISIBLE);
			pMenuItem = new CContextMenuItem(ID_ESTADO_OFFLINE, strLabel, this);
			m_pTrayMenuStatus->AddItem(pMenuItem);

			strLabel.LoadString(IDS_STATUS_OFFLINE);
			pMenuItem = new CContextMenuItem(ID_DISCONNECT_ALL, strLabel, this);
			m_pTrayMenuStatus->AddItem(pMenuItem);

			m_pTrayWindowMenu->AddItem(m_pTrayMenuStatus);
		}

		m_pTrayWindowMenu->AddSeparator();

		strLabel.LoadString(IDS_MENU_SHOW_HELP);
		pMenuItem = new CContextMenuItem(ID_SHOW_HELP, strLabel, this);
		m_pTrayWindowMenu->AddItem(pMenuItem);

		m_pTrayWindowMenu->AddSeparator();

		strLabel.LoadString(IDS_MENU_APP_ABOUT);
		pMenuItem = new CContextMenuItem(ID_APP_ABOUT, strLabel, this);
		m_pTrayWindowMenu->AddItem(pMenuItem);

		m_pTrayWindowMenu->AddSeparator();

		strLabel.LoadString(IDS_MENU_APP_CLOSE);
		pMenuItem = new CContextMenuItem(ID_CLOSE_WINDOW, strLabel, this);
		m_pTrayWindowMenu->AddItem(pMenuItem);
	}
}


void CUimView::UpdateUserStatus(IUOLMessengerAccountPtr pAccount)
{
	// Show user avatar and nickname
	if (m_pUserStatusPanel->IsWindow())
	{
		CString strStatus;

		DWORD dwStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(pAccount);

		GetStatusFromAllAccounts(strStatus, dwStatus);
		m_pUserStatusPanel->SetStatus(strStatus);

		m_pUserStatusPanel->InvalidateRect(NULL);
		m_pUserStatusPanel->UpdateWindow();
	}
}


void CUimView::UpdateUserInfo(IUOLMessengerAccount* pAccount)
{
	// Show user avatar and nickname
	if (m_pUserStatusPanel->IsWindow())
	{
		if (pAccount != NULL)
		{
			IUOLMessengerImagePtr pImage;
			IUOLMessengerUserIconPtr pUserIcon = pAccount->GetUserIcon();

			if (pUserIcon != NULL)
			{
				pImage = pUserIcon->GetImage();
			}

			if (!pImage)
			{
				IUOLMessengerImagesProtocolPtr pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
				pImage = pImagesProtocol->GetUserImage(pAccount->GetProtocolId(), 1);

				ATLASSERT(pImage.get());
			}

			if (pImage)
			{
				m_pUserAvatar->SetImage(pImage);

				m_pUserStatusPanel->SetAvatar(m_pUserAvatar);
			}

			CString strText = pAccount->GetNickName();
			if (strText.IsEmpty())
			{
				strText = pAccount->GetUserId();
			}

			m_pUserStatusPanel->SetText(strText);

			// Update account Status Message
			IUOLMessengerAccount3* pAccount3 = dynamic_cast<IUOLMessengerAccount3*>(pAccount);
			if (pAccount3)
			{
				m_pUserStatusPanel->SetStatusMessage(pAccount3->GetStatusMessage());			
			}
		}
		else
		{
			m_pUserStatusPanel->SetText(_T(""));
			m_pUserStatusPanel->SetAvatar(CUserAvatarPtr());
			m_pUserStatusPanel->SetStatusMessage("");
		}

		m_pUserStatusPanel->InvalidateRect(NULL);
		m_pUserStatusPanel->UpdateWindow();
	}
}


void CUimView::UpdateContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pApplication->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);

		if (pContact)
		{
			pContact->NotifyAll();
		}
	}
}


void CUimView::GetStatusFromAllAccounts(CString & strStatus, DWORD dwStatusChanged)
{
	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(listAccounts);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

		if ((pAccount != NULL))
		{
			DWORD dwAccountStatus = pAccountConnectionManager->GetStatus(pAccount);

			if (dwAccountStatus != dwStatusChanged)
			{
				strStatus.LoadString(IDS_STATUS_PERSONALIZED);
				return;
			}
		}
	}

	switch(dwStatusChanged)
	{
	case IUOLMessengerAccountConnectionManager::AS_ONLINE:
		strStatus.LoadString(IDS_STATUS_ONLINE);
		break;
	case IUOLMessengerAccountConnectionManager::AS_INVISIBLE:
		strStatus.LoadString(IDS_STATUS_INVISIBLE);
		break;
	case IUOLMessengerAccountConnectionManager::AS_AWAY:
	case IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY:
        strStatus.LoadString(IDS_STATUS_AWAY);
		break;
	case IUOLMessengerAccountConnectionManager::AS_OFFLINE:
        strStatus.LoadString(IDS_STATUS_OFFLINE);
		break;
	case IUOLMessengerAccountConnectionManager::AS_BUSY:
        strStatus.LoadString(IDS_STATUS_BUSY);
		break;
	}

}


LONG CUimView::IsOpenBrowserEnabled()
{
	return m_bOpenBrowser;
}


void CUimView::EnableOpenBrowser(LONG bEnable)
{
	::InterlockedExchange(&m_bOpenBrowser, bEnable);
}

CContactListTabPtr CUimView::GetContactListTab()
{
	return m_pContactListTab;
}

CTabViewPtr CUimView::GetAppTabViewCtrl()
{
	return m_pTabView;
}

CPluginCustomTabContainerViewPtr CUimView::GetPluginCustomTabContainerView()
{
	return m_pPluginCustomTabContainerView;
}

CMainFrameFocusNotifierPtr CUimView::GetMainFrameFocusNotifier()
{
	return m_mainFrameFocusNotifier;
}

CMainWindowEventsNotifierPtr CUimView::GetMainWindowEventsNotifier()
{
	return m_mainWindowEventsNotifier;
}

void CUimView::UpdateAccountInfoPanel(IUOLMessengerAccountPtr pAccount)
{
	m_pCurrentAccount = pAccount;
	
	UpdateUserInfo(m_pCurrentAccount.get());
}

CContextMenuPtr CUimView::GetSystrayContextMenu()
{
	return m_pTrayWindowMenu;
}











// --------------------
// CCommandFactory

IUOLMessengerCommandPtr CCommandFactory::GetCommand(const CString& strCommand, CWindow* pWindow)
{
	ATLASSERT(pWindow);

	if (strCommand.CompareNoCase("ShowContactListTab") == 0)
	{
		// Firstly, we hide de custom tabs.
		CPluginCustomTabContainerViewPtr pPluginCustomTabContainerView = 
				static_cast<CUimView*>(pWindow)->GetPluginCustomTabContainerView();
		
		pPluginCustomTabContainerView->HideAllTabs();
		
		// Right after, we show the contact list up.
		IUOLMessengerContactListTabPtr pContactListTab = static_cast<CUimView*>(pWindow)->GetContactListTab();

		pContactListTab->ShowContactListTab();
	}

	return IUOLMessengerCommandPtr();
}


//LRESULT CUimView::OnAccelaratorPreference(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		pUOLMessengerFacade->ExecuteCommand(new CShowPreferencesCommand());
//	}
//	return S_OK;
//}
//
LRESULT CUimView::OnAccelaratorAddContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

	if (pUOLMessengerFacade)
	{
		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowAddContactDialog"));
		if (pCommand)
		{
			pUOLMessengerFacade->ExecuteCommand(pCommand);
		}
	}
	return S_OK;
}

LRESULT CUimView::OnAccelaratorFileTransferDlg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

	if (pUOLMessengerFacade)
	{
		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowFileTransfersDialog"));
		if (pCommand)
		{
			pUOLMessengerFacade->ExecuteCommand(pCommand);
		}
	}
	return S_OK;
}

//LRESULT CUimView::OnAccelaratorPrivacy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		pUOLMessengerFacade->ExecuteCommand(new CShowPrivacyDialogCommand(pUOLMessengerFacade->GetUIManager()->GetAppWindow()));
//	}
//	return S_OK;
//}

//LRESULT CUimView::OnAccelaratorHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowHelp"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//	return S_OK;
//}

//LRESULT CUimView::OnAccelaratorOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("SetOnlineStatusAll"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//
//	return S_OK;
//}

LRESULT CUimView::OnAccelaratorHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

	IUOLMessengerCommandPtr pCommand = GetCommand(_T("ShowHistory"));

	if (pCommand)
	{
		m_pApplication->ExecuteCommand(pCommand);
	}

	return S_OK;
}

LRESULT CUimView::OnAccelaratorInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	ATLTRACE("%s",__FUNCTION__);
	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

	IUOLMessengerCommandPtr pCommand = GetCommand(_T("ShowUserInfo"));

	if (pCommand)
	{
		m_pApplication->ExecuteCommand(pCommand);
	}

	//if (pUOLMessengerFacade)
	//{
	//	IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
	//	IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("SetOnlineStatusAll"));
	//	if (pCommand)
	//	{
	//		pUOLMessengerFacade->ExecuteCommand(pCommand);
	//	}
	//}

	return S_OK;
}

LRESULT CUimView::OnAcceleratorFindContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_pContactListTab->SetFindContactPanelFocus();

    return S_OK;
}

//LRESULT CUimView::OnAccelaratorOffline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("DisconnectAllAccounts"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//
//	return S_OK;
//}


/*
void CUimView::CreateToolbarTab()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());	
	CUOLToolBarBuilderPtr pBuilder = FindElement<CUOLToolBarBuilderPtr>();
	
	if (pBuilder)
	{
		m_pUolToolbarTab = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pUolToolbarTab);
	}

	if (m_pUolToolbarTab.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pUolToolbarTab->Create(m_hWnd, rect);			

		ATLASSERT(m_pUolToolbarTab.get());

		m_pUolToolbarTab->AddButtonsEventListener(this);
	}
}
*/


void CUimView::CreateTabView()
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	CTabViewBuilderPtr pBuilder;
	
	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pBuilder.get() == NULL)); )
	{
		pBuilder = ap_dynamic_cast<CTabViewBuilderPtr>(m_listChilds.GetNext(pos));
	}	
	
	if (pBuilder)
	{
		m_pTabView = ap_dynamic_cast<CTabViewPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pTabView);
	}
	
	if (m_pTabView.get())
	{
		//CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pTabView->Create(m_hWnd,  rcDefault);
		m_pTabView->SetTabViewType(IUOLMessengerTabViewCtrl::TAB_VIEW_OTHER);
		m_pTabView->ShowCloseButton(FALSE);
		m_pTabView->SetInterButtonSpace(2);
		m_pTabView->SetTabButtonMargins(7, 0, 7, 0);
		m_pTabView->AutoCalculateTabsWidths(TRUE);
		m_pTabView->SetMaxTabs(MAX_TABS_COUNT);
		m_pTabView->RegisterObserver(this);
	}
}

void CUimView::CreatePluginCustomTab()
{
	m_pPluginCustomTabContainerView = new CPluginCustomTabContainerView();

	ATLASSERT(m_pPluginCustomTabContainerView != NULL);
	if (m_pPluginCustomTabContainerView != NULL)
	{
		IUOLMessengerUIManager2Ptr pUIManager2 = ap_static_cast<IUOLMessengerUIManager2Ptr>(CUIMApplication::GetApplication()->GetUIManager());

		m_pPluginCustomTabContainerView->Init(pUIManager2->GetPluginCustomTabContainer(), m_pTabView);

		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pPluginCustomTabContainerView->Create(m_hWnd, rect);

		ATLASSERT(m_pPluginCustomTabContainerView->IsWindow());
	}

	//m_pPluginCustomTabContainerView = new CPluginCustomPanelContainerView();

	//ATLASSERT(m_pPluginCustomTabContainerView != NULL);
	//if (m_pPluginCustomTabContainerView != NULL)
	//{
	//	m_pPluginCustomTabContainerView->Init(CUIMApplication::GetApplication()->GetUIManager()->GetPluginCustomPanelContainer());

	//	CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
	//	m_pPluginCustomTabContainerView->Create(m_hWnd, rect);

	//	ATLASSERT(m_pPluginCustomTabContainerView->IsWindow());
	//}

}

void CUimView::CreateAccountsPanel()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	CUOLToolBarBuilderPtr pBuilder = FindElement<CUOLToolBarBuilderPtr>();
	
	if (pBuilder)
	{
		m_pUserAccountsToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pUserAccountsToolbar);
	}
	
	if (m_pUserAccountsToolbar.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pUserAccountsToolbar->Create(m_hWnd, rect);
		
		ATLASSERT(m_pUserAccountsToolbar.get());
		
		m_pAccountsToolbarWrapper = new CAccountsToolbarWrapper(m_pUserAccountsToolbar);
		ATLASSERT(m_pAccountsToolbarWrapper.get());
	}
}

CString CUimView::GetCurrentAccountNickname()
{
	CString strNickname = "";
	
	if (m_pCurrentAccount) 
	{
		strNickname = m_pCurrentAccount->GetNickName();
		if (strNickname.IsEmpty())
		{
			strNickname = m_pCurrentAccount->GetUserId();
		}
	}
	return strNickname;
}
