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
 
// uimView.h : interface of the CUimView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <interfaces/IUOLMessengerFacade.h>
#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerSettingsEventListener.h>
#include <interfaces/IUOLMessengerContactListSettings.h>
#include <interfaces/IUOLMessengerInternetConnectionMonitorCallback.h>

#include "skin/Element.h"
#include "ContactListTab.h"

#include "../resource.h"
#include "UserStatusPanel.h"
//#include "UOLToolbar.h"
#include "AccountsPanel.h"
#include "TabViewBuilder.h"
#include "TabView.h"


#include "ListSplitPanel.h"

#include "PluginCustomTabContainerView.h"
#include "AccountsPanel.h"
#include "ContextMenu.h"
#include "usermessages.h"

#include "UOLMessengerTrayWindow.h"

#include "AccountConnection/ConnectionErrorView.h"

#include "../controller/MainFrameFocusNotifier.h"
#include "../controller/MainWindowEventsNotifier.h"

#include "./AccountsToolbarWrapper.h"


class CUimView;
typedef CWindowElement<CUimView, CDummyWindow<CWindowImpl<CUimView> > > CWindowBase;


class CUimView : public CWindowBase,
				 public IUOLMessengerSettingsEventListener,
				 public CUOLMessengerImageButtonEventListener2,
				 public CUserStatusPanelEventListener,				 
				 public CAccountsToolbarEventListener,
				 public CUOLMessengerAccountListEventListener,
				 public CUOLMessengerAccountConnectionManagerObserver,				 				 
				 public CUOLMessengerPrivacyManagerObserver,
				 public CUOLMessengerAccountObserver,
				 public IUOLMessengerMenuItemCallback,
				 public CUOLMessengerTrayWindowEventListener,
				 public IUOLMessengerInternetConnectionMonitorCallback,
				 public IUOLMessengerTabViewCtrlObserver2
{
public:
	enum { IDD = IDD_UIM_FORM };
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, COLOR_WINDOW);
	
	CUimView();
	virtual ~CUimView();

	BOOL PreTranslateMessage(MSG* pMsg);
	
	CContactListTabPtr	GetContactListTab();
	CTabViewPtr			GetAppTabViewCtrl();
	CPluginCustomTabContainerViewPtr	GetPluginCustomTabContainerView();
	CMainFrameFocusNotifierPtr			GetMainFrameFocusNotifier();
	CMainWindowEventsNotifierPtr		GetMainWindowEventsNotifier();
	
	void UpdateAccountInfoPanel(IUOLMessengerAccountPtr pAccount);
	
	CContextMenuPtr GetSystrayContextMenu();

protected:
	enum
	{
		TIMER_RECONNECT_ACCOUNTS = 0x350
	};
	
	enum
	{
		WM_POS_CREATE_MESSAGE = WM_USER + 0x380,
		WM_POS_SHOWWINDOW_MESSAGE
	};
	
	BEGIN_MSG_MAP(CUimView)				
		MSG_WM_CREATE(OnCreate)	
		MSG_WM_DESTROY(OnDestroy)		
		COMMAND_ID_HANDLER(ID_ACCELERATOR_HISTORY, OnAccelaratorHistory)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_INFO, OnAccelaratorInfo)
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_PREFERENCE, OnAccelaratorPreference)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_ADDCONTACT, OnAccelaratorAddContact)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_FILE_TRANSFER_DLG, OnAccelaratorFileTransferDlg)
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_PRIVACY, OnAccelaratorPrivacy)
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_ONLINE, OnAccelaratorOnline)
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_OFFLINE, OnAccelaratorOffline)
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_HELP, OnAccelaratorHelp)
        COMMAND_ID_HANDLER(ID_ACCELERATOR_FIND, OnAcceleratorFindContact)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_USER_UPDATE_LAYOUT, OnUpdateLayout)
		MESSAGE_HANDLER(WM_USER_INTERNET_CONNECTION_STATE_CHANGED, OnInternetConnectionStateChanged)
		MESSAGE_HANDLER(WM_USER_SHOW_CONNECTION_ERROR, OnShowConnectionError)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER_EX(WM_POS_CREATE_MESSAGE, OnPosCreateMessage)
		MESSAGE_HANDLER_EX(WM_POS_SHOWWINDOW_MESSAGE, OnPosShowWindowMessage)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		CHAIN_MSG_MAP(CWindowBase)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();	
	void	OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/);

	LRESULT OnUpdateLayout(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInternetConnectionStateChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorPreference(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorAddContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorFileTransferDlg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);	
	LRESULT OnAccelaratorPrivacy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorOffline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAcceleratorFindContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShowConnectionError(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPosCreateMessage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/);
	LRESULT OnPosShowWindowMessage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/);
	LRESULT OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	

	// CUOLMessengerAccountConnectionManagerObserver overriden functions
	virtual void OnConnected(IUOLMessengerAccountPtr pAccount);
	virtual void OnDisconnected(IUOLMessengerAccountPtr pAccount);
	virtual void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage);
	virtual void OnStatusChanged(IUOLMessengerAccountPtr pAccount);

		
	// CUOLMessengerPrivacyManagerObserver overriden functions
	virtual void OnDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual void OnDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact);

	// CUOLMessengerAccountObserver overriden functions
	virtual void OnNickNameChanged(IUOLMessengerAccount* pAccount);
	virtual void OnUserIconChanged(IUOLMessengerAccount* pAccount);
	virtual void OnStatusMessageChanged(IUOLMessengerAccount* pAccount);

	// IUOLMessengerEventListener overriden functions
	void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);
	
	// CUserStatusPanelEventListener overridden functions
	void OnRButtonDownAvatar(const CPoint& ptClick);
	void OnLButtonDownAvatar(const CPoint& ptClick);

	void OnRButtonDownText(const CPoint& ptClick);
	void OnLButtonDownText(const CPoint& ptClick);

	void OnLButtonDownStatusButton(const CPoint& ptClick);

	void OnLButtonDownStatusMessage(const CPoint& ptClick);

	// CAccountsPanelEventListener overridden functions
	void OnLButtonDownAccountButton(const CPoint& ptClick, CAccountButtonPtr pButton);

	// CImageButtonEventListener overridden functions
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);


	// IUOLMessengerMenuItemCallback interface
	void OnCommand(IUOLMessengerContextMenuItem* pItem);

	// CAccountListEventListener overriden functions
	void OnAddAccount(IUOLMessengerAccountPtr pAccount);
	void OnRemoveAccount(IUOLMessengerAccountPtr pAccount);
	void OnAccountUserIdChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldUserId);

	// CUOLMessengerEventListener overriden functions
	void OnLButtonDblClick(IUOLMessengerTrayWindow* pTray, const CPoint& ptClick);
	void OnRButtonDown(IUOLMessengerTrayWindow* pTray, const CPoint& ptClick);

	// IUOLMessengerInternetConnectionMonitorCallback interface
	void OnInternetConnectionStateChanged(BOOL bConnected);

	// IUOLMessengerTabViewCtrlObserver interface
	void OnConversationTabSelected( IUOLMessengerTabViewCtrl* pTabView, 
									IUOLMessengerConversationPtr pConversation, 
									int nTabIndex );
	void OnConversationTabMouseHover(IUOLMessengerTabViewCtrl* pTabView, 
									 IUOLMessengerConversationPtr pConversation, 
									 CPoint& pt, HWND wHandle );
	void OnConversationTabMouseLeave();
	
	virtual void GetMinSize(CSize& size);
	virtual IUOLMessengerCommandPtr GetCommand(const CString& strCommand);

	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual void DoPaint(CDCHandle& dc);

private:
	void CreateUserStatusPanel();
	void CreateContactListTab();
	void CreatePluginCustomTab();
	//void CreateToolbarTab();
	void CreateTabView();
	void CreateUOLButtonImageSwitcher();
	void CreateListSplitPanel();
	void CreateContactListCtrl();
	void CreatePluginCustomListCtrl();
	void CreatePluginCustomPanelContainerView();
	//void CreateAccountsPanel();
	void CreateStatusContextMenu();
	void CreateAvatarContextMenu();
	void CreateNicknameContextMenu();
	void CreateTrayWindow();
	void DestroyTrayWindow();
	void CreateTrayWindowContextMenu();
	void InitListSplitPanel();
	void CreateAccountsPanel();
	CString GetCurrentAccountNickname();
	
	void UpdateUserStatus(IUOLMessengerAccountPtr pAccount);
	void UpdateUserInfo(IUOLMessengerAccount* pAccount);
	void UpdateContact(IUOLMessengerAccountPtr pAccount, const CString& strContact);		

	void GetStatusFromAllAccounts(CString & strStatus, DWORD dwStatusChanged);
	
	LONG IsOpenBrowserEnabled();
	void EnableOpenBrowser(LONG bEnable);

private:
	static const int MAX_TABS_COUNT;
	
	HACCEL    m_haccelerator;

	IUOLMessengerFacade*				m_pApplication;

	CUserStatusPanelPtr					m_pUserStatusPanel;
	//CUOLToolBarPtr						m_pUolToolbarTab;
	CTabViewPtr							m_pTabView;
										
	CPluginCustomTabContainerViewPtr	m_pPluginCustomTabContainerView;

	CContactListTabPtr					m_pContactListTab;

//	CListSplitPanelPtr					m_pListSplitPanel;		

//	CAccountsPanelPtr					m_pAccountsPanel;

	LONG								m_bOpenBrowser;	

	CAtlList<IUOLMessengerAccountPtr>	m_listAccounts;
	IUOLMessengerAccountPtr				m_pCurrentAccount;

	CUserAvatarPtr						m_pUserAvatar;

	CContextMenuPtr						m_pMenuStatus;
	CContextMenuPtr						m_pMenuAvatar;
	CContextMenuPtr						m_pMenuNickname;

	CContextMenuPtr						m_pTrayMenuStatus;
	CContextMenuPtr						m_pTrayWindowMenu;

	IUOLMessengerFrameWindowPtr			m_pFrameConnectionError;
	CConnectionErrorViewPtr				m_pConnectionErrorView;
	UINT								m_nReconnectTimer;
	
	CMainFrameFocusNotifierPtr			m_mainFrameFocusNotifier;
	
	CUOLToolBarPtr						m_pUserAccountsToolbar;
	//CAccountsPanelPtr					m_pAccountsPanel;
	
	CMainWindowEventsNotifierPtr		m_mainWindowEventsNotifier;
	
	CAccountsToolbarWrapperPtr			m_pAccountsToolbarWrapper;
};
MAKEAUTOPTR(CUimView);



