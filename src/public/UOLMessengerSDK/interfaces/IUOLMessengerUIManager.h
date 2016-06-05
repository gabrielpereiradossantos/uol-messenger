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

#include "IUOLMessengerAccount.h"
#include "IUOLMessengerContactListOrderTraits.h"
#include "IUOLMessengerMessageWindowManager.h"
#include "IUOLMessengerFileTransfer.h"
#include "IUOLMessengerImagesProtocol.h"
#include "IUOLMessengerNotifyPopUpResource.h"
#include "IUOLMessengerAccountPropPageBuilder.h"
#include "IUOLMessengerPluginCustomList.h"
#include "IUOLMessengerPluginCustomPanelContainer.h"
#include "IUOLMessengerPluginCustomTabContainer.h"
#include "IUOLMessengerFrameWindow.h"
#include "IUOLMessengerFrameWindowView.h"
#include "UOLMessengerUserInputHandler.h"
#include "IUOLMessengerContactListTab.h"
#include "IUOLMessengerContactListCtrl.h"
#include "IUOLMessengerMainFrameFocusNotifier.h"
#include "IUOLMessengerMainWindowEventsNotifier.h"
#include "IUOLMessengerTabViewCtrl.h"
#include "IUOLMessengerThemeProperties.h"
#include "IUOLMessengerPropertyPageCommandFactory.h"
#include "IUOLMessengerAddCustomEmoticonWindow.h"
#include "IUOLMessengerHistoryContact.h"
#include "IUOLMessengerHistoryWindow.h"
#include "IUOLMessengerHistoryRecord.h"
#include "IUOLMessengerPluginPreferencesPageContainer.h"
#include "IUOLMessengerHistoryManager.h"


class CUOLMessengerUIManagerObserver;


class __declspec(novtable) IUOLMessengerUIManager
{
public:
	enum ImageDrawStrategy
	{
		IDS_DRAW_OPAQUE = 1,
		IDS_DRAW_OPAQUE_STRETCHED,
		IDS_DRAW_TRANSPARENT,
		IDS_DRAW_TRANSPARENT_STRETCHED,
		IDS_DRAW_PATTERN
	};

	enum PreferencesTab
	{
		PT_ACCOUNTS_TAB = 0,
		PT_CONTACTS_TAB,
		PT_MESSAGES_TAB,
		PT_STATUS_TAB,
		PT_SOUNDS_TAB,
		PT_EMOTICONS_TAB,
		PT_NETWORK_TAB,
		PT_PLUGINS_TAB,
		PT_GENERAL_TAB
	};

public:
	virtual ~IUOLMessengerUIManager() { }

	virtual BOOL CreateAppWindow(int nCmdShow) = 0;
	virtual CWindow GetAppWindow() = 0;
	virtual void PlaceMainWindowAlwaysOnTop(BOOL bAlways) = 0;

	virtual UINT ShowMessageBox(const CString& strMessage, UINT nFlags) = 0;
	virtual UINT ShowMessageBox(HWND hWnd, const CString& strMessage, UINT nFlags) = 0;
	virtual void ShowGroupDlg() = 0;
	virtual BOOL ShowContactDlg(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual BOOL ShowInputDlg(const CString& strTitle, const CString& strLabel, CString& strText, UINT nInputTextLimit = 0, BOOL bPasswordInput = FALSE) = 0;
	virtual void ShowPreferences(PreferencesTab tabId = PT_ACCOUNTS_TAB) = 0;
	virtual void ShowHelp() = 0;
	__declspec(deprecated) virtual void ShowHistory() = 0;
	__declspec(deprecated) virtual void ShowHistory(const CString& strFilePath) = 0;
	__declspec(deprecated) virtual void UpdateHistory(const CString& strFilename) = 0;

	virtual BOOL ShowAuthorizeDlg(IUOLMessengerAccountPtr pAccount, const CString& strContact) = 0;

	virtual void ShowFileTransferDialog(IUOLMessengerFileTransferPtr pFileTransfer) = 0;

	virtual void LaunchInfoWindow(IUOLMessengerContactPtr pContact) = 0;
	virtual void LaunchMessageWindow(IUOLMessengerContactPtr pContact, BOOL bFlashWindow) = 0;
	virtual void LaunchMessageReceivingConfirmationWindow(IUOLMessengerContactPtr pContact) = 0;

	virtual void SetContactListOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits) = 0;
	virtual IUOLMessengerContactListOrderTraitsPtr GetContactListOrderTraits() = 0;
	virtual void SortContactList() = 0;

	virtual CUOLMessengerUserInputHandlerPtr GetDefaultUserInputHandler() = 0;

	virtual IUOLMessengerMessageWindowManagerPtr GetMessageWindowManager() = 0;
	
	virtual IUOLMessengerImagesProtocolPtr GetImagesProtocol() = 0;
	virtual IUOLMessengerNotifyPopUpResourcePtr GetNotifyPopUpResource() = 0;
	virtual IUOLMessengerAccountPropPageBuilderPtr GetAccountPropPageBuilder(const CString& strProtocolId) = 0;

	virtual IUOLMessengerPluginCustomListPtr GetPluginCustomList() = 0;
	virtual IUOLMessengerPluginCustomPanelContainerPtr GetPluginCustomPanelContainer() = 0;

	virtual IUOLMessengerImagePtr LoadImage(const CString& strFilePath) = 0;
	virtual IUOLMessengerImagePtr LoadImage(HMODULE hModule, DWORD dwResourceId) = 0;
	virtual IUOLMessengerImageDrawStrategyPtr CreateDrawStrategy(ImageDrawStrategy drawStrategy) = 0;

	virtual IUOLMessengerFrameWindowPtr CreateFrameWindow(IUOLMessengerFrameWindowViewPtr pView, _U_RECT rect = NULL, 
														  HWND hWndParent = ::GetDesktopWindow(), 
														  DWORD dwFrameWindowStyle = IUOLMessengerFrameWindow::FWS_STATUS_BAR | IUOLMessengerFrameWindow::FWS_RESIZABLE) = 0;

	virtual void OpenBrowser(const CString& strUrl = _T(""), BOOL bAuthenticated = FALSE) = 0;

	virtual void PlaySound(const CString& strFilePath, BOOL bThemeRelativePath) = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager);


class __declspec(novtable) IUOLMessengerUIManager2 : public IUOLMessengerUIManager
{
public:
	virtual ~IUOLMessengerUIManager2() { }

	virtual IUOLMessengerPluginCustomTabContainerPtr GetPluginCustomTabContainer() = 0;
	
	// Deprecated method, use GetAppTabViewCtrl() to insert custom tabs into application main window
	virtual IUOLMessengerImageButtonContainerPtr GetAppWindowToolbar() const = 0;

	virtual IUOLMessengerContactListTabPtr GetAppWindowContactListTab() const = 0;
	virtual IUOLMessengerContactListCtrlPtr GetContactListCtrl() const = 0;
	virtual IUOLMessengerMainFrameFocusNotifierPtr GetMainFrameFocusNotifier() const = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager2);


class __declspec(novtable) IUOLMessengerUIManager3 : public IUOLMessengerUIManager2
{
public:
	virtual ~IUOLMessengerUIManager3() {}

	virtual IUOLMessengerTabViewCtrlPtr GetAppTabViewCtrl() const = 0;
	virtual IUOLMessengerMainWindowEventsNotifierPtr GetMainWindowEventsNotifier() const = 0;
	virtual IUOLMessengerThemePropertiesPtr GetCurrentThemeProperties() const = 0;
	virtual void ShowPreferences(const CString& strPageTitle) = 0;
	virtual BOOL ShowPreferencesWithCommand(PreferencesTab tabId = PT_ACCOUNTS_TAB, 
			IUOLMessengerPropertyPageCommandPtr pPropPageCommand = NULL) = 0;
	virtual void RemovePreferencesPage(HWND hwndPrefPage) = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager3);


class __declspec(novtable) IUOLMessengerUIManager4 : public IUOLMessengerUIManager3
{
public:
	virtual ~IUOLMessengerUIManager4() {}

	virtual UINT ShowMessageBox(HWND hWnd, const CString& strMessage, const CString& strCaption, UINT nFlags) = 0;

    __declspec(deprecated) virtual void ShowHistory(IUOLMessengerContactPtr pContact) = 0;
	__declspec(deprecated) virtual void ShowHistory(const CString& strFilePath, IUOLMessengerContactPtr pContact) = 0;
	__declspec(deprecated) virtual void UpdateHistory(const CString& strFilename, IUOLMessengerContactPtr pContact) = 0;
    __declspec(deprecated) virtual void GetPreviousFile(HTREEITEM hSelectedItem, IUOLMessengerContactPtr pContact) = 0;
	__declspec(deprecated) virtual void GetNextFile(HTREEITEM hSelectedItem,IUOLMessengerContactPtr pContact) = 0;
    virtual void LaunchMessageWindow(IUOLMessengerAccountPtr pAccount, const int& nChatId, BOOL bFlashWindow) = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager4);


class __declspec(novtable) IUOLMessengerUIManager5 : public IUOLMessengerUIManager4
{
public:
	virtual ~IUOLMessengerUIManager5() {}
	
	virtual IUOLMessengerAddCustomEmoticonWindowPtr GetAddCustomEmoticonDialog(
			const CString& strAddingEmoticonShortcut, 
			const CString& strAddingEmoticonFileName, int nOperationModeId, 
			IEmoticonChangingInfoFinder* pEmoticonChangingInfoFinder) = 0;
	
	virtual void SetEmoticonPropPageWndHandle(const HWND hEmoticonPropPageWnd) = 0;
	virtual HWND GetEmoticonPropPageWndHandle() const = 0;
	
    __declspec(deprecated) virtual void ShowHistory(IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter, const CString& strCallStatus) = 0;
	__declspec(deprecated) virtual void ShowHistory(const CString& strFilePath, IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter,const CString& strCallStatus) = 0;
	__declspec(deprecated) virtual void UpdateHistory(const CString& strFilename, IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter, const CString& strCallStatus) = 0;
    __declspec(deprecated) virtual void GetPreviousFile(HTREEITEM hSelectedItem, IUOLMessengerHistoryContactPtr pHistContact) = 0;
	__declspec(deprecated) virtual void GetNextFile(HTREEITEM hSelectedItem,IUOLMessengerHistoryContactPtr pHistContact) = 0;
	__declspec(deprecated) virtual void DeleteMultipleHistory(IUOLMessengerHistoryContactPtr pHistContact) = 0;
	virtual void RegisterObserver(CUOLMessengerUIManagerObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerUIManagerObserver* pObserver) = 0;

	virtual IUOLMessengerPluginPreferencesPageContainerPtr GetPluginPreferencesPageContainer() = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager5);


class __declspec(novtable) IUOLMessengerUIManager6 : public IUOLMessengerUIManager5
{
public:
	virtual ~IUOLMessengerUIManager6() {}
	
	__declspec(deprecated) virtual void ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, const CString& strWindowCaption) = 0;
	virtual void FlashFileTransferDialog() = 0;
	virtual void UpdateAccountInfoPanel(IUOLMessengerAccountPtr pAccount) = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager6);


class __declspec(novtable) IUOLMessengerUIManager7 : public IUOLMessengerUIManager6
{
public:
	virtual ~IUOLMessengerUIManager7() {}
	
	virtual void ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, IUOLMessengerHistoryRecordListPtr pHistoryRecordList, const CString& strWindowCaption) = 0;
	virtual IUOLMessengerHistoryManagerPtr GetHistoryManager() = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager7);


class __declspec(novtable) IUOLMessengerUIManager8 : public IUOLMessengerUIManager7
{
public:
	virtual ~IUOLMessengerUIManager8() {}
	
	/*
	 * WaitSystrayMenuInitialization
	 *
	 * /////////////////////////////////// WARNING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	 * 
	 * This method executes a loop with a WaitForSingleObject that waits 
	 * until the application systray menu is created.
	 * 
	 * So, it will be usefull for threads that want to use the following 
	 * methods to add and remove menu items, and that could execute this 
	 * method before the application systray menu was already been created.
	 * 
	 * In other circunstances (when it's called from application's own thread), 
	 * it will return imediatelly.
	 * 
	 * /////////////////////////////////// WARNING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	 *
	 * First parameter must be a pointer to the implementation of the plugin executing 
	 * the method (i.e., "CUOLMessengerPlugin*").
	 * Only authorized plugins can execute these method.
	 *
	 * @param pUOLMessengerPlugin	Pointer to plugin executing method.
	 *
	 */
	virtual void WaitSystrayMenuInitialization(void* pUOLMessengerPlugin) = 0;
	
	/*
	 * AddSystrayMenuItem
	 *
	 * /////////////////////////////////// WARNING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	 * 
	 * This method MUST be called from interface's thread (no worker threads allowed).
	 * 
	 * Calling it from a different thread causes undesired behavior on Windows 
	 * created to handle the popup menu.
	 *
	 * A suggested approach is to serialize the creation of the systray menu items 
	 * through windows messages (PostMessage or SendMessage methods).
	 * 
	 * /////////////////////////////////// WARNING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	 *
	 * Calling of method "IsSystrayMenuReady" is adviced so you can be sure that the 
	 * main systray menu will exist before adding a new item.
	 *
	 * First parameter must be a pointer to the implementation of the plugin executing 
	 * the method (i.e., "CUOLMessengerPlugin*").
	 * Only authorized plugins can execute these method.
	 *
	 * @param pUOLMessengerPlugin	Pointer to plugin executing method.
	 * @param strMenuTitle			Text to be used as title to menu item, and as a 
	 *								identity on deletion. If it's empty the plugin 
	 *								name will be used instead.
	 *
	 */
	virtual IUOLMessengerContextMenuPtr AddSystrayMenuItem(void* pUOLMessengerPlugin, CString strMenuTitle) = 0;
	
	/*
	 * RemoveSystrayMenuItem
	 *
	 * /////////////////////////////////// WARNING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	 * 
	 * This method MUST be called from interface's thread (no worker threads allowed).
	 * 
	 * Calling it from a different thread causes undesired behavior on Windows 
	 * created to handle the popup menu.
	 *
	 * A suggested approach is to serialize the creation of the systray menu items 
	 * through windows messages (PostMessage or SendMessage methods).
	 * 
	 * /////////////////////////////////// WARNING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	 *
	 * Calling of method "IsSystrayMenuReady" is adviced so you can be sure that the 
	 * main systray menu will exist before removing the item.
	 *
	 * First parameter must be a pointer to the implementation of the plugin executing 
	 * the method (i.e., "CUOLMessengerPlugin*").
	 * Only authorized plugins can execute these method.
	 *
	 * @param pUOLMessengerPlugin	Pointer to plugin executing method.
	 * @param strMenuTitle			Text used as title to menu item, and informed 
	 *								when the menu item was added. If an empty string 
	 *								was provided on addition, the same shall be 
	 *								provided accordingly.
	 *
	 */
	virtual void RemoveSystrayMenuItem(void* pUOLMessengerPlugin, CString strMenuTitle) = 0;
};
MAKEAUTOPTR(IUOLMessengerUIManager8);


class CUOLMessengerUIManagerObserver
{
public:
	virtual ~CUOLMessengerUIManagerObserver() {}

	virtual void OnCreateHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow) {}
	virtual void OnDestroyHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow) {}
};
