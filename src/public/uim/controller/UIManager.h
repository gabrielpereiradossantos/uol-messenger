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

#include <interfaces/IUOLMessengerFacade.h>
#include <interfaces/IUOLMessengerUIManager.h>
#include <interfaces/IUOLMessengerImagesProtocol.h>
#include <interfaces/IUOLMessengerPropertyPageCommandFactory.h>
#include <interfaces/IUOLMessengerHistoryWindow.h>

#include "UOLMessengerImagesProtocolImpl.h"
#include "../view/AppWindow.h"
#include "../view/Preferences/PreferencesPropertySheet.h"
#include "../view/ContactList/ContactListCtrl.h"
#include "../view/HistoryFrame.h"
#include "../view/IAppThreadCallback.h"
#include "../view/InputDialog.h"
#include "../view/AuthorizeDialog.h"
#include "UOLWindowManager.h"
#include "ProtocolResourceTranslator.h"
#include "FileTransfer/FileTransferDialogManager.h"
#include "Message/MessageReceivingDialogController.h"
#include "PluginCustomList.h"
#include "PluginCustomPanelContainer.h"
#include "PluginCustomTabContainer.h"
#include "PluginPreferencesPageContainer.h"
#include "../model/ThemeProperties.h"
#include "../view/IAppThreadCallback.h"



class CUIManagerObserverNotifier : public CObserverRegister<CUOLMessengerUIManagerObserver>
{
public:
	void NotifyAll(void (CUOLMessengerUIManagerObserver::*pfnUpdate)(IUOLMessengerHistoryWindow2*), 
							IUOLMessengerHistoryWindow2* pHistoryWindow);
};


class CUIManager : public IUOLMessengerUIManager8,
				   public CIdleHandler,
				   public CUOLMessengerHistoryWindowObserver
{
public:
	enum UIMANAGER_ERROR
	{
		UIM_ERROR_SUCCESS = 0,
		UIM_ERROR_FAILED = 1,
		UIM_ERROR_USER_SKIN_PARSING,
		UIM_ERROR_DEFAULT_SKIN_PARSING
	};

public:
	CUIManager();
	virtual ~CUIManager();

	const CUOLWindowManager* GetWindowManager() const;
	const CProtocolResourceTranslator* GetProtocolResourceTranslator() const;
	CMessageReceivingDialogController* GetMessageReceivingDialogController();
	
	DWORD PreInit(IUOLMessengerFacade* pUOLMessengerFacade);
	DWORD Init(IUOLMessengerFacade* pUOLMessengerFacade);
	void Finalize();
    void PreFinalize();

	//CHistoryFramePtr GetHistoryFrame();


	virtual IUOLMessengerImageButtonContainerPtr GetAppWindowToolbar() const;
	virtual IUOLMessengerTabViewCtrlPtr GetAppTabViewCtrl() const;
	virtual IUOLMessengerContactListTabPtr GetAppWindowContactListTab() const;
	virtual IUOLMessengerContactListCtrlPtr GetContactListCtrl() const;
	virtual IUOLMessengerMainFrameFocusNotifierPtr GetMainFrameFocusNotifier() const;
	virtual IUOLMessengerMainWindowEventsNotifierPtr GetMainWindowEventsNotifier() const;

	virtual void RegisterObserver(CUOLMessengerUIManagerObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerUIManagerObserver* pObserver);
	virtual void OnCreateHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow);
	virtual void OnDestroyHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow);

	virtual IUOLMessengerPluginPreferencesPageContainerPtr GetPluginPreferencesPageContainer();

	void SetEmoticonAdditionOnTheWay(BOOL bEmoticonAdditionOnTheWay);
	BOOL GetEmoticonAdditionOnTheWay();

	void SetAddCustomEmoticonWndHandle(const HWND hAddCustomEmoticonWnd);
	HWND GetAddCustomEmoticonWndHandle() const;

	void AddChatInviteContact(IUOLMessengerAccountPtr pAccount, IUOLMessengerContactPtr pContact, CAtlList<IUOLMessengerContactPtr>* pListChatInviteContacts);
	void InviteContactsForChat(IUOLMessengerConversation2Ptr pConversation, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName);

	virtual void LaunchMessageWindow(IUOLMessengerContactPtr pContact, BOOL bFlashWindow);
    virtual void LaunchMessageWindow(IUOLMessengerAccountPtr pAccount, const int& nChatId, BOOL bFlashWindow);

	// IUOLMessengerUIManager6 methods
	virtual void FlashFileTransferDialog();
	virtual void UpdateAccountInfoPanel(IUOLMessengerAccountPtr pAccount);

	// IUOLMessengerUIManager7 methods
	virtual void ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, IUOLMessengerHistoryRecordListPtr pHistoryRecordList, const CString& strWindowCaption);
	virtual IUOLMessengerHistoryManagerPtr GetHistoryManager();

	HWND GetFileTransferWindow();

	CInputDialogPtr GetInputDialog();
	CAuthorizeDialogPtr GetAuthorizeDialog();

	BOOL IsBugReportActive();
	void SetBugReportActive(BOOL bActive);
	
private:
	virtual BOOL CreateAppWindow(int nCmdShow);
	virtual CWindow GetAppWindow();
	virtual void PlaceMainWindowAlwaysOnTop(BOOL bAlways);
	
	virtual UINT ShowMessageBox(const CString& strMessage, UINT nFlags);
	virtual UINT ShowMessageBox(HWND hWnd, const CString& strMessage, UINT nFlags);
    virtual UINT ShowMessageBox(HWND hWnd, const CString& strMessage, const CString& strCaption, UINT nFlags);
	virtual void ShowGroupDlg();
	virtual BOOL ShowContactDlg(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual BOOL ShowInputDlg(const CString& strTitle, const CString& strLabel, CString& strText, UINT nInputTextLimit = 0, BOOL bPasswordInput = FALSE);
	virtual void ShowPreferences(PreferencesTab tabId = PT_GENERAL_TAB);
	virtual void ShowPreferences(const CString& strPageTitle);
	virtual BOOL ShowPreferencesWithCommand(PreferencesTab tabId = PT_ACCOUNTS_TAB, 
			IUOLMessengerPropertyPageCommandPtr pPropPageCommand = NULL);
	virtual void RemovePreferencesPage(HWND hwndPrefPage);
	virtual void ShowHelp();
    
    virtual BOOL ShowAuthorizeDlg(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual void ShowFileTransferDialog(IUOLMessengerFileTransferPtr pFileTransfer);

	virtual void LaunchInfoWindow(IUOLMessengerContactPtr pContact);
	virtual void LaunchMessageReceivingConfirmationWindow(IUOLMessengerContactPtr pContact);

	virtual void SetContactListOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits);
	virtual IUOLMessengerContactListOrderTraitsPtr GetContactListOrderTraits();
	virtual void SortContactList();

	virtual CUOLMessengerUserInputHandlerPtr GetDefaultUserInputHandler();

	virtual IUOLMessengerMessageWindowManagerPtr GetMessageWindowManager();
	
	virtual IUOLMessengerImagesProtocolPtr GetImagesProtocol();
	virtual IUOLMessengerNotifyPopUpResourcePtr GetNotifyPopUpResource();
	virtual IUOLMessengerAccountPropPageBuilderPtr GetAccountPropPageBuilder(const CString& strProtocolId);

	virtual IUOLMessengerPluginCustomListPtr GetPluginCustomList();
	virtual IUOLMessengerPluginCustomPanelContainerPtr GetPluginCustomPanelContainer();
	virtual IUOLMessengerPluginCustomTabContainerPtr GetPluginCustomTabContainer();

	virtual IUOLMessengerImagePtr LoadImage(const CString& strFilePath);
	virtual IUOLMessengerImagePtr LoadImage(HMODULE hModule, DWORD dwResourceId);
	virtual IUOLMessengerImageDrawStrategyPtr CreateDrawStrategy(ImageDrawStrategy drawStrategy);

	virtual IUOLMessengerFrameWindowPtr CreateFrameWindow(IUOLMessengerFrameWindowViewPtr pView, _U_RECT rect = NULL, 
														  HWND hWndParent = ::GetDesktopWindow(),
														  DWORD dwFrameWindowStyle = IUOLMessengerFrameWindow::FWS_STATUS_BAR | IUOLMessengerFrameWindow::FWS_RESIZABLE);

	virtual void OpenBrowser(const CString& strUrl = _T(""), BOOL bAuthenticated = FALSE);

	virtual void PlaySound(const CString& strFilePath, BOOL bThemeRelativePath);

	virtual IUOLMessengerThemePropertiesPtr GetCurrentThemeProperties() const;
	
	virtual BOOL OnIdle();

	void InitImagesProtocol();
	BOOL IsTopMost();
	
	virtual IUOLMessengerAddCustomEmoticonWindowPtr GetAddCustomEmoticonDialog(
			const CString& strAddingEmoticonShortcut, 
			const CString& strAddingEmoticonFileName, int nOperationModeId, 
			IEmoticonChangingInfoFinder* pEmoticonChangingInfoFinder);
	
	virtual void SetEmoticonPropPageWndHandle(const HWND hEmoticonPropPageWnd);
	virtual HWND GetEmoticonPropPageWndHandle() const;

	// IUOLMessengerUIManager Deprecated methods
	virtual void ShowHistory();
	virtual void ShowHistory(const CString& strFilePath);
	virtual void UpdateHistory(const CString& strFilename);

	// IUOLMessengerUIManager4 Deprecated methods
    virtual void ShowHistory(IUOLMessengerContactPtr pContact);
	virtual void ShowHistory(const CString& strFilePath, IUOLMessengerContactPtr pContact);
	virtual void UpdateHistory(const CString& strFilename, IUOLMessengerContactPtr pContact);
	virtual void GetPreviousFile(HTREEITEM hSelectedItem, IUOLMessengerContactPtr pContact);
	virtual void GetNextFile(HTREEITEM hSelectedItem,IUOLMessengerContactPtr pContact);
	
	// IUOLMessengerUIManager5 Deprecated methods
	virtual void ShowHistory(IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter, const CString& strCallStatus);
	virtual void ShowHistory(const CString& strFilePath, IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter,const CString& strCallStatus);
	virtual void UpdateHistory(const CString& strFilename, IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter, const CString& strCallStatus);
    virtual void GetPreviousFile(HTREEITEM hSelectedItem, IUOLMessengerHistoryContactPtr pHistContact);
	virtual void GetNextFile(HTREEITEM hSelectedItem,IUOLMessengerHistoryContactPtr pHistContact);
	virtual void DeleteMultipleHistory(IUOLMessengerHistoryContactPtr pHistContact);

	// IUOLMessengerUIManager6 Deprecated methods
	virtual void ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, const CString& strWindowCaption);
	
	// IUOLMessengerUIManager8 interface.
	//
	virtual void WaitSystrayMenuInitialization(void* pUOLMessengerPlugin);
	virtual IUOLMessengerContextMenuPtr AddSystrayMenuItem(void* pUOLMessengerPlugin, CString strMenuTitle);
	virtual void RemoveSystrayMenuItem(void* pUOLMessengerPlugin, CString strMenuTitle);
	
private:	
	CThemePropertiesPtr GetDefaultThemeProperties(IUOLMessengerFacade* pUOLMessengerFacade) const;

	// This method executes a method inside the AppWindow 
	// thread's context. It's always a good idea lauch UI 
	// stuff in a UI App Thread.

	void ExecuteMethodInAppThread(UINT nId, IAppThreadCallback* pCallback);


private:
	CAppWindowPtr							m_pwndAppWindow;
	CPreferencesPropertySheetPtr			m_pPreferences;	
	CHistoryFramePtr						m_pHistory;
	CUOLWindowManager						m_windowManager;
	CProtocolResourceTranslator				m_protocolResourceTranslator;
	CUOLMessengerUserInputHandlerPtr		m_pUserInputHandler;
	IUOLMessengerMessageWindowManagerPtr	m_pMessageWindowManager;
	CFileTransferDialogManagerPtr			m_pFileTransferDialogManager;
	IUOLMessengerImagesProtocolPtr			m_pImagesProtocol;
	IUOLMessengerNotifyPopUpResourcePtr		m_pNotifyPopUpResource;

	CPluginCustomListPtr				m_pPluginCustomList;
	CPluginCustomPanelContainerPtr		m_pPluginCustomPanelContainer;
	CPluginCustomTabContainerPtr		m_pPluginCustomTabContainer;
	CPluginPreferencesPageContainerPtr	m_pPluginPreferencesPageContainer;

	CThemePropertiesPtr		m_pThemeProperties;

	CMessageReceivingDialogController	m_messageReceivingDialogController;
	
	typedef CAtlMap<CString, IUOLMessengerAccountPropPageBuilderPtr> CAccountPropPageBuilderMap;
	typedef CAtlMap<CString, CHistoryFramePtr> CHistoryWindowsMap;

	CAccountPropPageBuilderMap		m_mapAccountPropPageBuilder;
	CHistoryWindowsMap				m_mapHistoryWindows;

	CUIManagerObserverNotifier	m_observerNotifier;
	
	HWND						m_hEmoticonPropPageWnd;
	HWND						m_hAddCustomEmoticonWnd;
	BOOL						m_bEmoticonAdditionOnTheWay;
	
	typedef CAtlMap<CString, CAtlList<IUOLMessengerContactPtr>* > CChatInviteContactsMap;
	CChatInviteContactsMap m_mapChatInviteContacts;

	CComAutoCriticalSection			m_csLock;
	BOOL							m_bIsBugReportActive;
	
	BOOL							m_bHasAddedSystrayMenuSeparator;
	
	typedef CAtlMap<CString, CContextMenuPtr>	CPluginMenuMap;
	CPluginMenuMap								m_pluginMenuMap;
	
	CEvent							m_waitingEvent;
};

MAKEAUTOPTR(CUIManager);
