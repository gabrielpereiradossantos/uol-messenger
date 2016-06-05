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

#include <interfaces/IUOLMessengerMessageWindow.h>
#include <interfaces/IUOLMessengerAccount.h>
#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerSettingsEventSource.h>
#include <interfaces/IUOLMessengerSettingsEventListener.h>
#include <interfaces/IUOLMessengerConversationWindow.h>
#include <interfaces/IUOLMessengerConversationManager.h>
#include <interfaces/IUOLMessengerPrivacyManager.h>
#include <interfaces/IUOLMessengerTabViewCtrl.h>

#include "UOLToolbar.h"
#include "ConversationWindow.h"
#include "DummyWindow.h"
#include "mainfrm.h"
#include "TabView.h"
#include "TabViewBuilder.h"
#include "../ObserverRegister.h"
#include "tooltip/ContactTooltip.h"
#include "tooltip/SimpleTooltip.h"
#include "TabControllerSubclass.h"
#include "ConfirmDlg.h"

class CConversationWindowObserverNotifier : public CObserverRegister<CUOLMessengerMessageWindowObserver>
{
public:
	CConversationWindowObserverNotifier(IUOLMessengerMessageWindow* pMessageWindow);

	void NotifyAll(void (CUOLMessengerMessageWindowObserver::*pfnUpdate)(IUOLMessengerMessageWindow*, IUOLMessengerConversationWindow*), IUOLMessengerConversationWindow* pConversationWindow);

private:
	IUOLMessengerMessageWindow* m_pMessageWindow;
};



class CMessageWindowFrame;
class CMessageWindow;
typedef CWindowElement<CMessageWindow, CDummyWindow<CWindowImpl<CMessageWindow> > > CMessageWindowBase;

class CMessageWindow : 
	public CMessageWindowBase,
	public IUOLMessengerTabViewCtrlObserver2,
	public CUOLMessengerConversationManagerObserver,
	public CUOLMessengerPrivacyManagerObserver,
	public IUOLMessengerSettingsEventListener,
	public CUOLMessengerImageButtonEventListener2,
	public CUOLMessengerConversationWindowObserver2,
	public CUOLMessengerContactObserver,
	public CTabControllerSubclass<CMessageWindow>,
	public CUOLMessengerConversationObserver3
{
public:
    DECLARE_WND_CLASS("CMessageWindow");
    
    CMessageWindow(CMessageWindowFrame* pParentFrame);
	virtual ~CMessageWindow();

	virtual int AddConversation(IUOLMessengerConversationPtr pConversation);
	virtual IUOLMessengerConversationPtr RemoveConversation(int nIndex);
	virtual IUOLMessengerConversationPtr GetConversation(int nIndex) const;

	virtual int GetConversationCount() const;

	virtual BOOL CanAddMoreConversation();

	virtual IUOLMessengerConversationPtr FindConversation(IUOLMessengerAccountPtr pAccount, const CString& strContactName);
	virtual IUOLMessengerConversationPtr FindConversation(IUOLMessengerContactPtr pContact);
    virtual IUOLMessengerConversation2Ptr FindConversation(const CString& strChatKey);

	virtual void SetActiveNextConversation();
	virtual void SetActivePriorConversation();
	virtual void SetActiveConversation(int nIndex);
	virtual void SetActiveConversation(IUOLMessengerConversationPtr pConversation);
	virtual void SetActiveConversation(IUOLMessengerConversationPtr pConversation, BOOL bSetFocus);
	virtual IUOLMessengerConversationPtr GetActiveConversation() const;
	virtual int GetActiveConversationIndex() const;

	virtual int GetConversationIndex(IUOLMessengerConversationPtr pConversation) const;

	virtual void FlashConversation(IUOLMessengerConversationPtr pConversation);

	virtual void ShowInputFocus();

	virtual IUOLMessengerImageButtonContainerPtr GetToolbar() const;

	virtual BOOL HasUnreadMessages();
    virtual BOOL CanCloseChatWindow();
	virtual IUOLMessengerConversationWindow2* FindConversationWindow(IUOLMessengerConversationPtr pConversation);

	virtual void RegisterObserver(CUOLMessengerMessageWindowObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerMessageWindowObserver* pObserver);

	// CSettingsEventListener interface
	virtual void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);

	CString GetConversationTitle();

protected:
	// CUOLMessengerConversationWindowObserver2 interface
	virtual void OnCreate(IUOLMessengerConversationWindow* pConversationWindow);
	virtual void OnDestroy(IUOLMessengerConversationWindow* pConversationWindow);
    virtual void OnUpdateChatTitle(IUOLMessengerConversationWindow2* pConversationWindow);

	// IUOLMessengerTabViewCtrlObserver interface (CTabViewObserver)
	virtual void OnConversationTabSelected(IUOLMessengerTabViewCtrl* pTabView, 
			IUOLMessengerConversationPtr pConversation, int nTabIndex);

	// IUOLMessengerTabViewCtrlObserver2 interface (CTabViewObserver)
	virtual void OnConversationTabMouseHover(IUOLMessengerTabViewCtrl* pTabView, 
			IUOLMessengerConversationPtr pConversation, CPoint& pt, HWND wHandle);
	virtual void OnConversationTabMouseLeave();

	virtual void OnContactTyping(IUOLMessengerConversationPtr pConversation, TypingState state);

	virtual void OnDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual void OnDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact);

	virtual void OnContactAdded(IUOLMessengerAccount* pAccount, const CContactInfo* pContactInfo, const CString& strGroup);

	virtual void Update(IUOLMessengerContact* pContact);
	virtual void UpdateAlias(IUOLMessengerContact* pContact);
	virtual void UpdateStatus(IUOLMessengerContact* pContact);
	virtual void UpdateIcon(IUOLMessengerContact* pContact);

	// CUOLMessengerConversationObserver3 interface
	virtual void OnSwitchContact(IUOLMessengerConversation* pConversation, IUOLMessengerContactPtr pRemovedContact, IUOLMessengerContactPtr pAddedContact);

	enum
	{
		WM_USER_CREATE_CONVERSATION_WINDOW = WM_USER + 0x230,
		WM_USER_DESTROY_CONVERSATION_WINDOW,
		WM_USER_ACTIVATE_CONVERSATION_WINDOW,
		WM_USER_CONV_TIMEOUT,
		WM_USER_UPDATE_TOOLBAR,
		WM_USER_SHOW_INPUT_FOCUS
	};

	BEGIN_MSG_MAP(CMessageWindow)	
		CHAIN_MSG_MAP(CTabControllerSubclass<CMessageWindow>)
		MSG_WM_CREATE(OnCreate)	
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_USER_CREATE_CONVERSATION_WINDOW, OnCreateConversationWindow)
		MESSAGE_HANDLER(WM_USER_DESTROY_CONVERSATION_WINDOW, OnDestroyConversationWindow)
		MESSAGE_HANDLER(WM_USER_ACTIVATE_CONVERSATION_WINDOW, OnActivateConversationWindow)
		MESSAGE_HANDLER(WM_USER_SHOW_INPUT_FOCUS, OnShowInputFocus)

		COMMAND_ID_HANDLER(ID_ACCELERATOR_HISTORY, OnAccelaratorHistory)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_INFO, OnAccelaratorInfo)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_ESC, OnAccelaratorEscape)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_NEXT_TAB, OnAccelaratorNextTab)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_PRIOR_TAB, OnAcceleratorPriorTab)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_CLOSE_CONVERSATION_TAB, OnAcceleratorCloseTab)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_CLOSE_CHILD_WINDOW, OnAcceleratorCloseTab)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_NEXT_ITEM, OnAccelaratorNextTab)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_PRIOR_ITEM, OnAcceleratorPriorTab)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_ADDCONTACT, OnAcceleratorSelectAll)
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_PREFERENCE, OnAccelaratorPreference)		/* CTRL + P */
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_ADDCONTACT, OnAccelaratorAddContact)		/* CTRL + A */
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_PRIVACY, OnAccelaratorPrivacy)			/* CTRL + B */
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_ONLINE, OnAccelaratorOnline)				/* CTRL + O */
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_OFFLINE, OnAccelaratorOffline)			/* CTRL + D */
		//COMMAND_ID_HANDLER(ID_ACCELERATOR_HELP, OnAccelaratorHelp)					/* F1 */
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)

		MESSAGE_HANDLER(WM_USER_UPDATE_TOOLBAR, OnUpdateToolbar)
		CHAIN_MSG_MAP(CMessageWindowBase)
	END_MSG_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnCreate(LPCREATESTRUCT lpCreatestruct);

	LRESULT OnAccelaratorHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorEscape(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorNextTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAcceleratorPriorTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAcceleratorCloseTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAcceleratorSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorPreference(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorAddContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorPrivacy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorOffline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAccelaratorHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditCopy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void OnDestroy();
	LRESULT OnCreateConversationWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroyConversationWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActivateConversationWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowInputFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT CMessageWindow::OnUpdateToolbar(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	virtual IUOLMessengerCommandPtr GetCommand(const CString& strCommand);

	// CImageButtonEventListener overridden functions
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);

	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual void GetMinSize(CSize& size);

private:
	CConversationWindowPtr FindConversationWindow(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	CContactTooltipPtr m_pContactTooltip;
	void DestroyAllConversationWindows();
	void SetActiveConversationImpl(int nIndex, BOOL bSetFocus = TRUE);
	void UnregisterContactObserver(CConversationWindowPtr pConversationWindow);
	void CreateTabView();
	void CreateToolbar();

	BOOL MatchConversationContact(IUOLMessengerConversationPtr pConversation, IUOLMessengerAccountPtr pAccount, const CString& strContact) const;
    BOOL MatchConversationChat(IUOLMessengerConversation2Ptr pConversation, IUOLMessengerAccountPtr pAccount, const CString& strChatKey) const;
	CString GetContactAlias(IUOLMessengerConversationPtr pConversation);
	void UpdateWindowCaptionIcon(CConversationWindowPtr pConversationWindow);
	void UpdateToolbarButtonState(const CString& strButtonCommand, const CString& strButtonState);
	void UpdateToolbarButtonVisible(const CString& strButtonCommand, BOOL bVisible);

private:
	static const int MAX_TABS_COUNT;

	CMessageWindowFrame*				m_pParentFrame;
	HACCEL    m_haccelerator;


	CConversationWindowObserverNotifier m_ObserverNotifier;

	CUOLToolBarPtr			m_pToolbar;
	CConversationWindowPtr	m_pActiveConversationWindow;
	int						m_nActiveConversationIndex;
	BOOL					m_bIsTabVisible;
    BOOL                    m_bIsToolbarVisible;

	CAtlArray<CConversationWindowPtr>	m_arConversationWindow;
	CTabViewPtr				m_pTabView;
	CTabViewBuilderPtr		m_pTabViewBuilder;
	
	int						m_nHistoryWindowTabIndex;
	int						m_nRichEditWindowTabIndex;
	int						m_nSendButtonWindowTabIndex;

    CSimpleTooltip          m_ChatTooltip;
}; 


MAKEAUTOPTR(CMessageWindow);




class CMessageWindowFrameObserver
{
public:
	virtual ~CMessageWindowFrameObserver() {}

	virtual void OnCreateWindow(CMessageWindowFrame* pMessageWindowFrame) {}
	virtual void OnDestroyWindow(CMessageWindowFrame* pMessageWindowFrame) {}
};



class CMessageWindowFrameObserverNotifier : public CObserverRegister<CMessageWindowFrameObserver>
{
public:
	CMessageWindowFrameObserverNotifier(CMessageWindowFrame* pFrame);

	void NotifyAll(void (CMessageWindowFrameObserver::*pfnUpdate)(CMessageWindowFrame*));

private:
	CMessageWindowFrame*		m_pFrame;
};



class CMessageWindowFrame : 
	public IUOLMessengerMessageWindow3,
	public CMainFrame, 
	public IUOLMessengerSettingsEventListener
{
public:
	DECLARE_WND_CLASS("CMessageWindowFrame");
    
    CMessageWindowFrame();
	virtual ~CMessageWindowFrame();

	virtual int AddConversation(IUOLMessengerConversationPtr pConversation);
	virtual IUOLMessengerConversationPtr RemoveConversation(int nIndex);
	virtual IUOLMessengerConversationPtr GetConversation(int nIndex) const;

	virtual int GetConversationCount() const;

	virtual BOOL CanAddMoreConversation();

	virtual IUOLMessengerConversationPtr FindConversation(IUOLMessengerAccountPtr pAccount, const CString& strContactName);
	virtual IUOLMessengerConversationPtr FindConversation(IUOLMessengerContactPtr pContact);
    virtual IUOLMessengerConversation2Ptr FindConversation(const CString& strChatKey);

	virtual void SetActiveNextConversation(void);
	virtual void SetActiveConversation(int nIndex);
	virtual void SetActiveConversation(IUOLMessengerConversationPtr pConversation);
	virtual IUOLMessengerConversationPtr GetActiveConversation() const;
	virtual int GetActiveConversationIndex() const;

	virtual int GetConversationIndex(IUOLMessengerConversationPtr pConversation) const;

	virtual void Show(int nCmdShow = SW_NORMAL);
	virtual void Hide();
	virtual void Raise();
	virtual void Flash();
	virtual void GetMinSize(CSize& size);

	virtual void FlashConversation(IUOLMessengerConversationPtr pConversation);

	virtual BOOL HasFocus() const;
	virtual BOOL IsVisible() const;

	virtual void ShowInputFocus();

	virtual IUOLMessengerImageButtonContainerPtr GetToolbar() const;

	virtual BOOL HasUnreadMessages();
    virtual BOOL CanCloseChatWindow();
	virtual IUOLMessengerConversationWindow2* FindConversationWindow(IUOLMessengerConversationPtr pConversation);

	virtual void RegisterObserver(CUOLMessengerMessageWindowObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerMessageWindowObserver* pObserver);

	virtual void SetActivePriorConversation(void);
	
	virtual void SetActiveConversation(IUOLMessengerConversationPtr pConversation, BOOL bSetFocus);
	
	virtual void RegisterFrameObserver(CMessageWindowFrameObserver* pObserver);
	virtual void UnregisterFrameObserver(CMessageWindowFrameObserver* pObserver);

	void OnFinalMessage(HWND /*hWnd*/);

protected:
	BEGIN_MSG_MAP(CMessageWindowFrame)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		CHAIN_MSG_MAP(CMainFrame)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT, const CSize& size);
	void OnDestroy();
	void OnClose();
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT	OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	virtual void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);

private:
	void UpdateCaptionText();
	CString GetConversationTitle();
	CConfirmDlgPtr GetConfirmDialog();

private:
	CMessageWindowFrameObserverNotifier		m_observerNotifier;
	BOOL					m_bMinimized;
	UINT_PTR				m_nIDFlashTimer;
};

MAKEAUTOPTR(CMessageWindowFrame);





class CMessageWindowCommandFactory : public CUOLCommandFactory
{
public:
	static IUOLMessengerCommandPtr GetCommand(const CString& strCommand, CMessageWindow* pMessageWindow);
};