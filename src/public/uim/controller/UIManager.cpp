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
 
#include "StdAfx.h"
#include "./uimanager.h"

#include "UIMApplication.h"
#include "UserInputHandler.h"
#include "NotifyPopUpResource.h"
#include "MessageWindow/MessageWindowManager.h"
#include "InfoWindow/InfoWindowController.h"
#include "../resource.h"
#include "../view/InputDialog.h"
#include "../view/AuthorizeDialog.h"
#include "../view/ContactDialog.h"
#include "../view/Preferences/PreferencesPropertySheetBuilder.h"
#include "../view/HistoryFrameBuilder.h"
#include "../view/skin/ProtocolImageElement.h"
#include "../view/image/image.h"
#include "../view/image/ImageBuilder.h"
#include "../view/FrameWindow.h"
#include "../view/FrameWindowViewElement.h"
#include "../view/FrameWindowBuilder.h"
#include "../view/DialogManager.h"
#include "../view/MessageBox.h"
#include "../view/Skin/MainDialogElement.h"
#include "../view/AddCustomEmoticonDialog.h"

#include <commands/OpenBrowserCommand.h>
#include <commands/OpenBrowserAuthenticatedCommand.h>
#include <commands/LaunchInfoWindowCommand.h>
#include <commands/LaunchMessageWindowCommand.h>
#include <commands/InviteForChatCommand.h>
#include <MinDependencySpecialFolder.h>


#define			SYSTRAY_MENU_WAITING_TIMEOUT			500


void CUIManagerObserverNotifier::NotifyAll(void (CUOLMessengerUIManagerObserver::*pfnUpdate)(IUOLMessengerHistoryWindow2*), 
							IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	CAtlList<CUOLMessengerUIManagerObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	CUOLMessengerUIManagerObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(pHistoryWindow);
	}
}


CUIManager::CUIManager() : 
	m_hEmoticonPropPageWnd(NULL), 
	m_hAddCustomEmoticonWnd(NULL),
	m_bEmoticonAdditionOnTheWay(FALSE),
	m_bIsBugReportActive(FALSE), 
	m_bHasAddedSystrayMenuSeparator(FALSE)
{
}


CUIManager::~CUIManager()
{
}


const CUOLWindowManager* CUIManager::GetWindowManager() const
{
	return &m_windowManager;
}


const CProtocolResourceTranslator* CUIManager::GetProtocolResourceTranslator() const
{
	return &m_protocolResourceTranslator;
}


CMessageReceivingDialogController* CUIManager::GetMessageReceivingDialogController()
{
	return &m_messageReceivingDialogController;
}


DWORD CUIManager::PreInit(IUOLMessengerFacade* pUOLMessengerFacade)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pThemeProperties = GetDefaultThemeProperties(pUOLMessengerFacade);

	if (!m_pThemeProperties)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "m_pThemeProperties == NULL");
		
		return UIM_ERROR_FAILED;
	}

	DWORD dwRet = UIM_ERROR_SUCCESS;

	CUOLWindowManager::LoadError leError = m_windowManager.Load(m_pThemeProperties);

	if (leError != CUOLWindowManager::LE_ERROR_SUCCESS)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "leError = %d", leError);
		
		if (leError == CUOLWindowManager::LE_USER_SKIN_PARSE_ERROR)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogInfo(__FUNCTION__, "leError == LE_USER_SKIN_PARSE_ERROR");
			
			dwRet = UIM_ERROR_USER_SKIN_PARSING;

			// User skin loading failed, restore default theme
			CString strDefaultTheme = pUOLMessengerFacade->GetSettingsManager()->GetGeneralSettings()->GetDefaultTheme();
			pUOLMessengerFacade->GetSettingsManager()->GetGeneralSettings()->SetSelectedTheme(strDefaultTheme);
		}
		else 
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogInfo(__FUNCTION__, "leError == UIM_ERROR_DEFAULT_SKIN_PARSING");
			
			dwRet = UIM_ERROR_DEFAULT_SKIN_PARSING;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "dwRet = %d", dwRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return dwRet;
}



DWORD CUIManager::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	DWORD dwRet = UIM_ERROR_FAILED;

	if (m_pThemeProperties)
	{
		m_pwndAppWindow = ap_dynamic_cast<CAppWindowPtr>(m_windowManager.GetElement("__AppWindowInstance"));
		m_pUserInputHandler = new CUserInputHandler();
		m_pHistory = ap_dynamic_cast<CHistoryFramePtr>(m_windowManager.GetElement("__HistoryWindowInstance"));
		m_pMessageWindowManager = new CMessageWindowManager();
		m_pImagesProtocol = new CUOLMessengerImagesProtocolImpl();
		m_pPluginCustomList = new CPluginCustomList();
		m_pPluginCustomPanelContainer = new CPluginCustomPanelContainer();
		m_pPluginCustomTabContainer = new CPluginCustomTabContainer();
		m_pPluginPreferencesPageContainer = new CPluginPreferencesPageContainer();
		m_pNotifyPopUpResource = new CNotifyPopUpResource();
		m_pFileTransferDialogManager = new CFileTransferDialogManager();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pwndAppWindow = [%x], m_pUserInputHandler = [%x], "
				"m_pHistory = [%x], m_pMessageWindowManager = [%x], "
				"m_pImagesProtocol = [%x], m_pPluginCustomList = [%x], "
				"m_pPluginCustomPanelContainer = [%x], m_pPluginCustomTabContainer = [%x], "
				"m_pPluginPreferencesPageContainer = [%x], m_pNotifyPopUpResource = [%x], "
				"m_pFileTransferDialogManager = [%x].", 
				m_pwndAppWindow.get(), m_pUserInputHandler.get(), 
				m_pHistory.get(), m_pMessageWindowManager.get(), 
				m_pImagesProtocol.get(), m_pPluginCustomList.get(), 
				m_pPluginCustomPanelContainer.get(), m_pPluginCustomTabContainer.get(), 
				m_pPluginPreferencesPageContainer.get(), m_pNotifyPopUpResource.get(), 
				m_pFileTransferDialogManager.get());

		m_mapHistoryWindows.RemoveAll();

		InitImagesProtocol();

		BOOL bOk = FALSE;

		if (m_pMessageWindowManager)
		{
			bOk = static_cast<CMessageWindowManager*>(m_pMessageWindowManager.get())->Init();

			if (bOk && m_pNotifyPopUpResource)
			{
				bOk = static_cast<CNotifyPopUpResource*>(m_pNotifyPopUpResource.get())->Init(pUOLMessengerFacade);
			}
		}

		if (bOk)
		{
			bOk = CInfoWindowController::GetInstance()->Init();
		}

		if (bOk)
		{
			bOk = m_messageReceivingDialogController.Init();
		}

		if (m_pFileTransferDialogManager)
		{
			m_pFileTransferDialogManager->Init();
		}
		
		_Module.GetMessageLoop()->AddIdleHandler(this);

		if (bOk)
		{
			dwRet = UIM_ERROR_SUCCESS;
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "m_pThemeProperties == NULL");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "dwRet = %d", dwRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return dwRet;
}


void CUIManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	_Module.GetMessageLoop()->RemoveIdleHandler(this);

	//Moved to PreFinalize because CPluginManagerTask is called first
    /*
    if ((m_pPreferences != NULL) && (m_pPreferences->IsWindow()))
	{
		m_pPreferences->SendMessage(WM_CLOSE);
		//m_pPreferences->DestroyWindow();
	}
    */

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pHistory = [%x].", m_pHistory.get());
	
	if ((m_pHistory != NULL) && (m_pHistory->IsWindow()))
	{
		m_pHistory->SendMessage(WM_CLOSE);		
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pMessageWindowManager = [%x].", m_pMessageWindowManager.get());
	
	if (m_pMessageWindowManager)
	{
		static_cast<CMessageWindowManager*>(m_pMessageWindowManager.get())->Finalize();
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pNotifyPopUpResource = [%x].", m_pNotifyPopUpResource.get());
	
	if (m_pNotifyPopUpResource)
	{
		static_cast<CNotifyPopUpResource*>(m_pNotifyPopUpResource.get())->Finalize();
	}

	CInfoWindowController::GetInstance()->Finalize();

	m_messageReceivingDialogController.Finalize();

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pFileTransferDialogManager = [%x].", m_pFileTransferDialogManager.get());
	
	if (m_pFileTransferDialogManager)
	{
		m_pFileTransferDialogManager->Finalize();
	}

	m_pHistory.reset();
	m_pwndAppWindow.reset();
	m_pMessageWindowManager.reset();
	m_pFileTransferDialogManager.reset();

	m_mapChatInviteContacts.RemoveAll();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CUIManager::PreFinalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
    CDialogManager::GetInstance()->UnregisterAll();
    
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pPreferences = [%x].", m_pPreferences.get());
	
    if ((m_pPreferences != NULL) && (m_pPreferences->IsWindow()))
	{
		::PostMessage(m_pPreferences->m_hWnd, WM_CLOSE, 0, 0);
		//m_pPreferences->SendMessage(WM_CLOSE);
	}

	//Got back to Finalize.
	/*
    if (m_pMessageWindowManager)
	{
		static_cast<CMessageWindowManager*>(m_pMessageWindowManager.get())->Finalize();
	}
	*/

	for (POSITION pos = m_mapHistoryWindows.GetStartPosition(); pos;)
	{
		CHistoryFramePtr pHistoryWindow = m_mapHistoryWindows.GetNextValue(pos);
		if (pHistoryWindow->IsWindow())
		{
			pHistoryWindow->DestroyWindow();
		}
	}
	m_mapHistoryWindows.RemoveAll();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IUOLMessengerImageButtonContainerPtr CUIManager::GetAppWindowToolbar() const
{
	return IUOLMessengerImageButtonContainerPtr();
}


IUOLMessengerTabViewCtrlPtr CUIManager::GetAppTabViewCtrl() const
{
	return m_pwndAppWindow->GetAppTabViewCtrl();
}


IUOLMessengerContactListTabPtr CUIManager::GetAppWindowContactListTab() const
{
	return m_pwndAppWindow->GetContactListTab();
}


IUOLMessengerMainFrameFocusNotifierPtr CUIManager::GetMainFrameFocusNotifier() const
{
	return m_pwndAppWindow->GetMainFrameFocusNotifier();
}


IUOLMessengerMainWindowEventsNotifierPtr CUIManager::GetMainWindowEventsNotifier() const
{
	return m_pwndAppWindow->GetMainWindowEventsNotifier();
}


BOOL CUIManager::CreateAppWindow(int nCmdShow)
{
	if (m_pwndAppWindow->CreateAppWindow() == NULL)	
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		
		return FALSE;
	}

	m_pwndAppWindow->ShowWindow(nCmdShow);

	PlaceMainWindowAlwaysOnTop(IsTopMost());

	return TRUE;
}


CWindow CUIManager::GetAppWindow()
{
	return m_pwndAppWindow->m_hWnd;
}


void CUIManager::PlaceMainWindowAlwaysOnTop(BOOL bAlways)
{
	if ((m_pwndAppWindow != NULL) && (m_pwndAppWindow->IsWindow()))
	{
		m_pwndAppWindow->SetWindowPos(bAlways ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE);
	}
}


UINT CUIManager::ShowMessageBox(const CString& strMessage, UINT nFlags)
{
	return ShowMessageBox(GetAppWindow(), strMessage, nFlags);
}


UINT CUIManager::ShowMessageBox(HWND hWnd, const CString& strMessage, UINT nFlags)
{
    UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strCaption;
	strCaption = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

    return ShowMessageBox(hWnd, strMessage, strCaption, nFlags);
}

UINT CUIManager::ShowMessageBox(HWND hWnd, const CString& strMessage, const CString& strCaption, UINT nFlags)
{
	// Replaced standard MessageBox to Dialog based MessageBox to avoid closing errors
	IElementPtr pElement = GetWindowManager()->GetElement("__MessageBoxBuilderInstance");

	if (pElement)
    {
        CElementBuilderPtr pBuilder;	

        pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);

        if (pBuilder)
        {
            CMessageBoxPtr pMessageBox = ap_dynamic_cast<CMessageBoxPtr>(pBuilder->CreateElement());			
            ATLASSERT(pMessageBox.get());
            pMessageBox->SetMessageBoxFlags(nFlags);
            pMessageBox->SetMessageBoxTitle(strCaption);
            pMessageBox->SetMessageBoxText(strMessage);
            
            if (pMessageBox.get())
            {
                return pMessageBox->DoModal(hWnd);
            }
        }		
    }

    return ::MessageBox(hWnd, strMessage, strCaption, nFlags);
}


void CUIManager::ShowGroupDlg()
{
	CString strMessage;

	// Check online accounts count
	if (0 == CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount())
	{
		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		strMessage = pStringLoader->GetValue("uol.resource.error.IDS_SHOW_GROUP_DIALOG_NO_ONLINE_ACCOUNTS_MSG");

		ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);

		return;
	}

	// If groups are not visible, ask user to turn them visible
	if (FALSE == CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->IsShowGroupsEnabled())
	{
		strMessage.LoadString(IDS_SHOW_GROUP_DIALOG_NO_VISIBLE_GROUPS_MSG);

		if (IDYES == ShowMessageBox(strMessage, MB_YESNO | MB_ICONQUESTION))
		{
			CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->EnableShowGroups(TRUE);
			CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_GROUPS);
		}
	}

	// Check if groups are visble
	if (CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->IsShowGroupsEnabled())
	{
		CString strCaption, strGroup;

		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		strCaption = pStringLoader->GetValue("uol.resource.IDS_UI_MANAGER_ADDGROUP_CAPTION");
		strMessage = pStringLoader->GetValue("uol.resource.IDS_UI_MANAGER_ADDGROUP_NAMEGROUP");

		// Get group name
		if (ShowInputDlg(strCaption, strMessage, strGroup, IUOLMessengerGroup::MAX_GROUP_NAME_LENGTH))
		{
			strGroup.Trim();

			// Check empty group
			if (strGroup.IsEmpty())
			{
				strMessage = pStringLoader->GetValue("uol.resource.error.IDS_UI_MANAGER_ERROR_EMPTY_NAME");

				ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
			}
			else
			{
				IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();

				IUOLMessengerGroupPtr pGroup =  pContactList->FindGroup(strGroup);

				// Check existent group
				if (pGroup != NULL)
				{
					CString strFormat;
					strFormat = pStringLoader->GetValue("uol.resource.error.IDS_UI_MANAGER_ERROR_GROUP_EXISTS");
					strMessage.Format(strFormat, strGroup);

					ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
				}
				else
				{
					pGroup = pContactList->AddGroup(strGroup);
				}
			}
		}
	}
}


BOOL CUIManager::ShowContactDlg(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	BOOL bRet = FALSE;

	CString strErrorMessage;

	if (CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount() > 0)
	{
		bool bShowDialog = false;

		CAtlList<IUOLMessengerAccountPtr> listOnlineAccounts;
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountList(listOnlineAccounts);

		for (POSITION pos = listOnlineAccounts.GetHeadPosition(); pos; )
		{
			IUOLMessengerAccountPtr pOnlineAccount = listOnlineAccounts.GetNext(pos);

			if (pOnlineAccount)
			{
				CIMProtocol* pProtocol = pOnlineAccount->GetProtocol();
				ATLASSERT(pProtocol);

				if (pProtocol)
				{
					CIMProtocol3* pProtocol3 = dynamic_cast<CIMProtocol3*>(pProtocol);

					if (pProtocol3)
					{
						bShowDialog |= pProtocol3->HasContactListSupport();

						if (bShowDialog)
						{
							break;
						}
					}
				}
			}
		}

		if (bShowDialog)
		{
			CContactDialogPtr pContactDlg;
					
			IElementPtr pElement = GetWindowManager()->GetElement("__ContactDialogBuilderInstance");			
			if (pElement)
			{
				CElementBuilderPtr pBuilder;				
				pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
				if (pBuilder)
				{
					pContactDlg = ap_dynamic_cast<CContactDialogPtr>(pBuilder->CreateElement());			
					ATLASSERT(pContactDlg.get());
					pContactDlg->Initialize(strContact, pAccount);
					bRet = (IDOK == pContactDlg->DoModal(GetAppWindow()));
				}
			}
		}
		else
		{
			// No account with contact list supoort is online
			UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			strErrorMessage = pStringLoader->GetValue("uol.resource.error.NoContactListSupport");
		}
	}
	else
	{
		// No account is online
		UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		strErrorMessage = pStringLoader->GetValue("uol.resource.error.IDS_ADD_CONTACT_NO_ONLINE_ACCOUNTS_ERROR_MSG");
	}
	
	if (FALSE == strErrorMessage.IsEmpty())
	{
		ShowMessageBox(strErrorMessage, MB_OK | MB_ICONERROR);
	}

	return bRet;
}


BOOL CUIManager::ShowInputDlg(const CString& strTitle, const CString& strLabel, CString& strText, UINT nInputTextLimit, BOOL bPasswordInput)
{
	CInputDialogPtr pInputDialog = GetInputDialog();

	if (pInputDialog.get())
	{
		pInputDialog->Initialize(strTitle, strLabel, nInputTextLimit, bPasswordInput, strText);
		UINT nDialogRet = pInputDialog->DoModal(GetAppWindow());

		if (IDOK == nDialogRet)
		{
			strText = pInputDialog->GetText();

			return TRUE;
		}
	}

	return FALSE;
}


CInputDialogPtr CUIManager::GetInputDialog()
{
	CInputDialogPtr pInputDialog;

	IElementPtr pElement = GetWindowManager()->GetElement("__InputDialogBuilderInstance");			
	if (pElement)
	{
		CElementBuilderPtr pBuilder;				
		pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
		if (pBuilder)
		{
			pInputDialog = ap_dynamic_cast<CInputDialogPtr>(pBuilder->CreateElement());			
			ATLASSERT(pInputDialog.get());
		}
	}

	return pInputDialog;
}


void CUIManager::ShowPreferences(PreferencesTab tabId)
{
	ShowPreferencesWithCommand(tabId, NULL);
}


void CUIManager::ShowPreferences(const CString& strPageTitle)
{
	class Helper : public IAppThreadCallback
	{
	public:
		Helper(CUIManager* pThis, const CString& strPageTitle) :
		  m_pThis(pThis), 
		  m_strTabTitle(strPageTitle)
		{
		}

		virtual void operator()(UINT /*nId*/)
		{
			if ((!m_pThis->m_pPreferences) || (!m_pThis->m_pPreferences->IsWindow()))
			{
				CPreferencesPropertySheetBuilderPtr pBuilder = ap_dynamic_cast<CPreferencesPropertySheetBuilderPtr>(m_pThis->m_windowManager.GetElement(_T("__PreferencesBuilderInstance")));

				m_pThis->m_pPreferences = ap_dynamic_cast<CPreferencesPropertySheetPtr>(pBuilder->CreateElement());
				ATLASSERT(m_pThis->m_pPreferences.get());

				///m_pPreferences->SetBuilder(pBuilder);

				if (m_pThis->m_pPreferences.get())
				{
					m_pThis->m_pPreferences->CreatePreferecesWindow(::GetDesktopWindow());
					m_pThis->m_pPreferences->CenterWindow();
				}
			}

			ATLASSERT(m_pThis->m_pPreferences.get());

			if (m_pThis->m_pPreferences.get())
			{
				m_pThis->m_pPreferences->ShowWindow(SW_SHOWNORMAL);
				m_pThis->m_pPreferences->BringWindowToTop();
			}

			m_pThis->m_pPreferences->SetActivePage(m_strTabTitle);
		}

	private:
		CUIManager*		m_pThis;
		CString			m_strTabTitle;
	};

	Helper h(this, strPageTitle);

	UINT nId = m_pwndAppWindow->RegisterCustomWindowMessage();	

	ExecuteMethodInAppThread(nId, &h);

	m_pwndAppWindow->UnregisterCustomWindowMessage(nId);	
}


BOOL CUIManager::ShowPreferencesWithCommand(PreferencesTab tabId, 
		IUOLMessengerPropertyPageCommandPtr pPropPageCommand)
{
	class Helper : public IAppThreadCallback
	{
	public:
		Helper(CUIManager* pThis, PreferencesTab tabId, IUOLMessengerPropertyPageCommandPtr pPropPageCommand) :
				m_pThis(pThis), 
				m_tabId(tabId), 
				m_pPropPageCommand(pPropPageCommand), 
				m_lResult(CALLBACK_EXECUTION_FAILURE)
		{
		}

		virtual void operator()(UINT /*nId*/)
		{
			if ((!m_pThis->m_pPreferences) || (!m_pThis->m_pPreferences->IsWindow()))
			{
				CPreferencesPropertySheetBuilderPtr pBuilder = ap_dynamic_cast<CPreferencesPropertySheetBuilderPtr>(m_pThis->m_windowManager.GetElement(_T("__PreferencesBuilderInstance")));

				m_pThis->m_pPreferences = ap_dynamic_cast<CPreferencesPropertySheetPtr>(pBuilder->CreateElement());
				ATLASSERT(m_pThis->m_pPreferences.get());

				///m_pPreferences->SetBuilder(pBuilder);

				if (m_pThis->m_pPreferences.get())
				{
					m_pThis->m_pPreferences->CreatePreferecesWindow(::GetDesktopWindow());
					m_pThis->m_pPreferences->CenterWindow();
				}
			}

			ATLASSERT(m_pThis->m_pPreferences.get());

			if (m_pThis->m_pPreferences.get())
			{
				m_pThis->m_pPreferences->ShowWindow(SW_SHOWNORMAL);
				m_pThis->m_pPreferences->BringWindowToTop();
			}

			m_pThis->m_pPreferences->SetActivePage(m_tabId);
			
			if (m_pPropPageCommand)
			{
				m_lResult = m_pThis->m_pPreferences->HandlePropPageCommand(m_pPropPageCommand);
			}
		}

		virtual LRESULT GetResult()
		{
			return m_lResult;
		}

	private:
		CUIManager*							m_pThis;
		PreferencesTab						m_tabId;
		IUOLMessengerPropertyPageCommandPtr	m_pPropPageCommand;
		LRESULT								m_lResult;
	};
	
	Helper h(this, tabId, pPropPageCommand);
	
	UINT nId = m_pwndAppWindow->RegisterCustomWindowMessage();	
	
	ExecuteMethodInAppThread(nId, &h);
	
	m_pwndAppWindow->UnregisterCustomWindowMessage(nId);
	
	return h.GetResult();
}


void CUIManager::RemovePreferencesPage(HWND hwndPrefPage)
{
	if ((m_pPreferences != NULL) && (m_pPreferences->IsWindow()))
	{
		m_pPreferences->RemovePrefsPage(hwndPrefPage);
	}
}
void CUIManager::ShowHelp()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strHelpUrl = pStringLoader->GetValue(_T("uol.resource.IDS_HELP_URL"));

	CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strHelpUrl));
}


BOOL CUIManager::ShowAuthorizeDlg(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	BOOL bResult = FALSE;

	CAuthorizeDialogPtr pAuthorizeDialog = GetAuthorizeDialog();

	if (pAuthorizeDialog.get())
	{
		pAuthorizeDialog->Initialize(pAccount, strContact, (pContact != NULL));
		bResult = (IDOK == pAuthorizeDialog->DoModal(GetAppWindow()));
	}
	
	return bResult;
}


CAuthorizeDialogPtr CUIManager::GetAuthorizeDialog()
{
	CAuthorizeDialogPtr pAuthorizeDialog;

	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__AuthorizeDialogBuilderInstance");			
	if (pElement)
	{
		CElementBuilderPtr pBuilder;				
		pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
		if (pBuilder)
		{
			pAuthorizeDialog = ap_dynamic_cast<CAuthorizeDialogPtr>(pBuilder->CreateElement());			
			ATLASSERT(pAuthorizeDialog.get());
		}
	}

	return pAuthorizeDialog;
}


void CUIManager::ShowFileTransferDialog(IUOLMessengerFileTransferPtr pFileTransfer)
{
	m_pFileTransferDialogManager->UpdateRecentHistory();
	m_pFileTransferDialogManager->ShowFileTransferWindow(pFileTransfer);
}


void CUIManager::FlashFileTransferDialog()
{
	m_pFileTransferDialogManager->FlashFileTransferWindow();
}


void CUIManager::UpdateAccountInfoPanel(IUOLMessengerAccountPtr pAccount)
{
	m_pwndAppWindow->UpdateAccountInfoPanel(pAccount);
}


void CUIManager::LaunchInfoWindow(IUOLMessengerContactPtr pContact)
{
	CUIMApplication::GetApplication()->ExecuteCommand(new CLaunchInfoWindowCommand(pContact, GetAppWindow()));
}


void CUIManager::LaunchMessageWindow(IUOLMessengerContactPtr pContact, BOOL bFlashWindow)
{
	CUIMApplication::GetApplication()->ExecuteCommand(new CLaunchMessageWindowCommand(pContact, bFlashWindow));
}


void CUIManager::LaunchMessageWindow(IUOLMessengerAccountPtr pAccount, const int& nChatId, BOOL bFlashWindow)
{
    CUIMApplication::GetApplication()->ExecuteCommand(new CLaunchMessageWindowCommand(pAccount, nChatId, bFlashWindow));
}


void CUIManager::LaunchMessageReceivingConfirmationWindow(IUOLMessengerContactPtr pContact)
{
	CMessageReceivingDialogPtr pDialog;

	pDialog = m_messageReceivingDialogController.FindNotifyDialog(pContact);

	if (!pDialog)
	{
		pDialog = m_messageReceivingDialogController.CreateNotifyDialog(pContact, (HWND) GetAppWindow());
	}
}


void CUIManager::SetContactListOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits)
{
	CContactListCtrlPtr pContactListCtrl = ap_dynamic_cast<CContactListCtrlPtr>(GetContactListCtrl());

	pContactListCtrl->SetOrderTraits(pOrderTraits);
}


IUOLMessengerContactListOrderTraitsPtr CUIManager::GetContactListOrderTraits()
{
	CContactListCtrlPtr pContactListCtrl = ap_dynamic_cast<CContactListCtrlPtr>(GetContactListCtrl());

	return pContactListCtrl->GetOrderTraits();
}


void CUIManager::SortContactList()
{
	CContactListCtrlPtr pContactListCtrl = ap_dynamic_cast<CContactListCtrlPtr>(GetContactListCtrl());

	pContactListCtrl->Sort();
}


CUOLMessengerUserInputHandlerPtr CUIManager::GetDefaultUserInputHandler()
{
	return m_pUserInputHandler;
}


IUOLMessengerMessageWindowManagerPtr CUIManager::GetMessageWindowManager()
{
	return m_pMessageWindowManager;
}


IUOLMessengerImagesProtocolPtr CUIManager::GetImagesProtocol()
{
	return m_pImagesProtocol;
}


IUOLMessengerNotifyPopUpResourcePtr CUIManager::GetNotifyPopUpResource()
{
	return m_pNotifyPopUpResource;
}


IUOLMessengerAccountPropPageBuilderPtr CUIManager::GetAccountPropPageBuilder(const CString& strProtocolId)
{
	CAccountPropPageBuilderMap::CPair* pPair = m_mapAccountPropPageBuilder.Lookup(strProtocolId);

	if (pPair)
	{
		return pPair->m_value;
	}

	return NULL;
}


IUOLMessengerPluginCustomListPtr CUIManager::GetPluginCustomList()
{
	return m_pPluginCustomList;
}


IUOLMessengerPluginCustomPanelContainerPtr CUIManager::GetPluginCustomPanelContainer()
{
	return m_pPluginCustomPanelContainer;
}


IUOLMessengerPluginCustomTabContainerPtr CUIManager::GetPluginCustomTabContainer()
{
	return m_pPluginCustomTabContainer;
}


IUOLMessengerImagePtr CUIManager::LoadImage(const CString& strFilePath)
{
	return UOLMessengerImage::CImageBuilder::LoadImage(strFilePath, FALSE);
}


IUOLMessengerImagePtr CUIManager::LoadImage(HMODULE hModule, DWORD dwResourceId)
{
	return UOLMessengerImage::CImageBuilder::LoadImageFromResource(hModule, dwResourceId);
}


IUOLMessengerImageDrawStrategyPtr CUIManager::CreateDrawStrategy(ImageDrawStrategy drawStrategy)
{
	IUOLMessengerImageDrawStrategyPtr pStrategy;

	if (drawStrategy == IUOLMessengerUIManager::IDS_DRAW_OPAQUE)
	{
		pStrategy = new UOLMessengerImage::CDrawOpaqueStrategy();
	}
	else if (drawStrategy == IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT)
	{
		pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();
	}
	else if (drawStrategy == IUOLMessengerUIManager::IDS_DRAW_PATTERN)
	{
		pStrategy = new UOLMessengerImage::CPatternStrategy();
	}
	else if (drawStrategy == IUOLMessengerUIManager::IDS_DRAW_OPAQUE_STRETCHED)
	{
		pStrategy = new UOLMessengerImage::CDrawStretchOpaqueStrategy();
	}
	else if (drawStrategy == IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED)
	{
		pStrategy = new UOLMessengerImage::CDrawStretchTransparentStrategy();
	}

	return pStrategy;
}


IUOLMessengerFrameWindowPtr CUIManager::CreateFrameWindow(IUOLMessengerFrameWindowViewPtr pView, _U_RECT rect, HWND hWndParent, DWORD dwFrameWindowStyle)
{
	CFrameWindowPtr pFrameWindow;

	if (pView)
	{
		CFrameWindowViewElementPtr pFrameWindowView = new CFrameWindowViewElement(pView);

		CFrameWindowBuilderPtr pBuilder = ap_dynamic_cast<CFrameWindowBuilderPtr>(m_windowManager.GetElement(_T("__FrameWindowBuilderInstance")));

		ATLASSERT(pBuilder.get());
		if (pBuilder.get())
		{
			pFrameWindow = ap_dynamic_cast<CFrameWindowPtr>(pBuilder->CreateElement());	
				
			pFrameWindow->SetView(pFrameWindowView);
			pFrameWindow->SetStyle(dwFrameWindowStyle);
			pFrameWindow->Create(hWndParent, rect);
		}
	}

	return pFrameWindow;
}


void CUIManager::OpenBrowser(const CString& strUrl, BOOL bAuthenticated)
{
	if (FALSE == bAuthenticated)
	{
		CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strUrl));
	}
	else
	{
		CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserAuthenticatedCommand(strUrl));
	}
}


void CUIManager::PlaySound(const CString& strFilePath, BOOL bThemeRelativePath)
{
	if (strFilePath.IsEmpty())
	{
		return;
	}

	CString strSound;

	if (bThemeRelativePath)
	{
		CString strThemePath = GetCurrentThemeProperties()->GetDirPath();
		strSound = strThemePath + "\\" + strFilePath;
	}
	else
	{
		strSound = strFilePath;
	}

	::PlaySound(strSound, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}


IUOLMessengerThemePropertiesPtr CUIManager::GetCurrentThemeProperties() const
{
	return m_pThemeProperties;
}


BOOL CUIManager::OnIdle()
{
	CMenuCommonBasePtr p = ap_dynamic_cast<CMenuCommonBasePtr>(m_windowManager.GetElement("StatusSubMenu"));

	if (p)
	{
		BOOL bRet = p->EnableItem(CUIMApplication::GetApplication()->GetAccountManager()->GetAccountListCount() > 0);
		ATLASSERT(bRet >= 0);		
	}

	return TRUE;
}


BOOL CUIManager::IsTopMost()
{
	IUOLMessengerContactListSettingsPtr pContactSettings;
	
	pContactSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings();

	return (pContactSettings->IsTopMost());
}


IUOLMessengerAddCustomEmoticonWindowPtr CUIManager::GetAddCustomEmoticonDialog(
		const CString& strAddingEmoticonShortcut, 
		const CString& strAddingEmoticonFileName, int nOperationModeId, 
		IEmoticonChangingInfoFinder* pEmoticonChangingInfoFinder)
{
	CAddCustomEmoticonDialogPtr pAddCustomEmoticonDialog;
	
	CString strErrorMessage;
	
	if (CUIMApplication::GetApplication()->GetAccountManager()->GetAccountListCount() > 0)
	{
		if (!GetEmoticonAdditionOnTheWay())
		{
			SetEmoticonAdditionOnTheWay(TRUE);
			
			IElementPtr pElement = GetWindowManager()->GetElement("__AddCustomEmoticonDialogBuilderInstance");
			
			if (pElement)
			{
				CElementBuilderPtr pBuilder;
				
				pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);
				
				if (pBuilder)
				{
					pAddCustomEmoticonDialog = 
							ap_dynamic_cast<CAddCustomEmoticonDialogPtr>(pBuilder->CreateElement());			
					ATLASSERT(pAddCustomEmoticonDialog.get());
					
					if (pAddCustomEmoticonDialog.get())
					{
						pAddCustomEmoticonDialog->InitializeAddCustomEmoticonInfo(NULL, 
								strAddingEmoticonShortcut, strAddingEmoticonFileName, 
								(IUOLMessengerAddCustomEmoticonWindow::OperationModeId)nOperationModeId,
								pEmoticonChangingInfoFinder);
					}
				}
			}
		}
		else
		{
			// Two add custom emoticon MODAL dialogs = hell on earth.
			UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			strErrorMessage = pStringLoader->GetValue("uol.resource.error.IDS_ADD_CUSTOM_EMOTICON_WINDOW_IN_USE_ERROR_MSG");
			ShowMessageBox(GetAddCustomEmoticonWndHandle(), strErrorMessage, MB_OK | MB_ICONERROR);
		}
	}
	else
	{
		// No account stored.
		UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		strErrorMessage = pStringLoader->GetValue("uol.resource.error.IDS_ADD_CUSTOM_EMOTICON_NO_ACCOUNTS_ERROR_MSG");
		ShowMessageBox(::GetActiveWindow(), strErrorMessage, MB_OK | MB_ICONERROR);
	}
	
	return pAddCustomEmoticonDialog;
}


void CUIManager::SetEmoticonPropPageWndHandle(const HWND hEmoticonPropPageWnd)
{
	m_hEmoticonPropPageWnd = hEmoticonPropPageWnd;
}


HWND CUIManager::GetEmoticonPropPageWndHandle() const
{
	return m_hEmoticonPropPageWnd;
}


void CUIManager::SetAddCustomEmoticonWndHandle(const HWND hAddCustomEmoticonWnd)
{
	m_hAddCustomEmoticonWnd = hAddCustomEmoticonWnd;
}


HWND CUIManager::GetAddCustomEmoticonWndHandle() const
{
	return m_hAddCustomEmoticonWnd;
}




CThemePropertiesPtr CUIManager::GetDefaultThemeProperties(IUOLMessengerFacade* pUOLMessengerFacade) const
{
	CThemePropertiesPtr pThemeProperties;

	IUOLMessengerFacade2* pUOLMessengerFacade2 = dynamic_cast<IUOLMessengerFacade2*>(pUOLMessengerFacade);

	if (pUOLMessengerFacade2)
	{
		IUOLMessengerCommonSettingsPtr pCommonSettings = pUOLMessengerFacade2->GetCommonSettings();
		
		if (pCommonSettings)
		{
			CString strThemeName = pCommonSettings->GetDefaultTheme();
			CString strThemeDirPath;

			CMinDependencySpecialFolder	minDSF;
			CString	strCompany;
			CString	strProduct;
			CString strThemeDir;

			strCompany.LoadString(IDS_STRING_COMPANY_NAME);
			strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
			strThemeDir.LoadString(IDS_STRING_THEME_DIR);

			strThemeDirPath.Format(_T("%s%s\\%s\\%s\\%s"), 
					minDSF.GetCommonAppData(), 
					strCompany, 
					strProduct,
					strThemeDir,
					strThemeName);

			pThemeProperties = new CThemeProperties(strThemeName, strThemeDirPath);
		}
	}

	return pThemeProperties;
}


IUOLMessengerContactListCtrlPtr CUIManager::GetContactListCtrl() const
{
	return ap_dynamic_cast<IUOLMessengerContactListCtrlPtr>(GetWindowManager()->GetElement("__ContactListInstance"));
}

void CUIManager::ExecuteMethodInAppThread(UINT nId, IAppThreadCallback* pCallback)
{
	if (m_pwndAppWindow)
	{
		m_pwndAppWindow->CallCustomWindowMessage(nId, pCallback);
	}
}

void CUIManager::InitImagesProtocol()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());
	ATLASSERT(pUIManager);
	CProtocolBagListElementPtr pProtocolBagList;
	CProtocolContactStatusElementPtr pContactStatusMsn;
	CProtocolContactStatusElementPtr pContactStatusICQ;

	pProtocolBagList = ap_dynamic_cast<CProtocolBagListElementPtr>(const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement(_T("__CProtocolBagListElementInstance")));
	ATLASSERT(pProtocolBagList.get());

	CConstAtlListIterator<CIMProtocol*> itProtocolsList;
	
	CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocolList(itProtocolsList);

	CIMProtocol* pProtocol;

	while (itProtocolsList != NULL)
	{
		pProtocol = *itProtocolsList;

		if (pProtocol != NULL)
		{
			CString strProtocol = pProtocol->GetName().c_str();
			CString strProtocolId = pProtocol->GetId().c_str();

			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"Initializing images for protocol [%s] with id [%s].",
					strProtocol, strProtocolId);
			
			CProtocolBagElementPtr pBagElement = pProtocolBagList->GetProtocolBagList(strProtocol);

			ATLASSERT(pBagElement);
			if (pBagElement)
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Inserting default image for protocol [%s].", strProtocolId);
				
				m_pImagesProtocol->InsertDefaultImage(strProtocolId,1,pBagElement->GetProtocolIcon());

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Default image for protocol [%s] inserted.", strProtocolId);
				
				CProtocolImageListElementPtr pImageList = pBagElement->GetProtocolImageList();

				if (pImageList)
				{
					CProtocolUserIconElementPtr pUserIcon = pImageList->GetProtocolUserIconElement();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pUserIcon = [%x].", pUserIcon.get());
					
					if (pUserIcon)
					{
						m_pImagesProtocol->InsertUserImage(strProtocolId,1,pUserIcon->GetUserIconPath());
					}

					CProtocolUserStatusElementPtr pUserStatus = pImageList->GetProtocolUserStatusElement();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pUserStatus = [%x].", pUserStatus.get());
					
					if (pUserStatus)
					{
						m_pImagesProtocol->InsertUserStatusImage(strProtocolId,IUOLMessengerAccountConnectionManager::AS_OFFLINE,pUserStatus->GetUserIconOffline());
						m_pImagesProtocol->InsertUserStatusImage(strProtocolId,IUOLMessengerAccountConnectionManager::AS_ONLINE,pUserStatus->GetUserIconOnline());
						m_pImagesProtocol->InsertUserStatusImage(strProtocolId,IUOLMessengerAccountConnectionManager::AS_BUSY,pUserStatus->GetUserIconBusy());
						m_pImagesProtocol->InsertUserStatusImage(strProtocolId,IUOLMessengerAccountConnectionManager::AS_AWAY,pUserStatus->GetUserIconAway());
						m_pImagesProtocol->InsertUserStatusImage(strProtocolId,IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY,pUserStatus->GetUserIconAway());
						m_pImagesProtocol->InsertUserStatusImage(strProtocolId,IUOLMessengerAccountConnectionManager::AS_INVISIBLE,pUserStatus->GetUserIconInvisible());
					}

					CProtocolContactIconElementPtr pContactIcon = pImageList->GetProtocolContactIconElement();;

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pContactIcon = [%x].", pContactIcon.get());
					
					if (pContactIcon)
					{
						m_pImagesProtocol->InsertContactImage(strProtocolId,1,pContactIcon->GetContactIconPath());
					}

					CProtocolContactStatusElementPtr pContactStatus = pImageList->GetProtocolContactStatus();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pContactStatus = [%x].", pContactStatus.get());
					
					if (pContactStatus)
					{
						m_pImagesProtocol->InsertContactStatusImage(strProtocolId, 
							IUOLMessengerContact::CS_ONLINE,pContactStatus->GetUserIconOnline());
						m_pImagesProtocol->InsertContactStatusImage(strProtocolId, 
							IUOLMessengerContact::CS_BUSY,	pContactStatus->GetUserIconBusy());
						m_pImagesProtocol->InsertContactStatusImage(strProtocolId, 
							IUOLMessengerContact::CS_AWAY,	pContactStatus->GetUserIconAway());
						m_pImagesProtocol->InsertContactStatusImage(strProtocolId, 
							IUOLMessengerContact::CS_IDLE,	pContactStatus->GetUserIconIdle());
						m_pImagesProtocol->InsertContactStatusImage(strProtocolId, 
							IUOLMessengerContact::CS_OFFLINE, pContactStatus->GetUserIconOffline());
                        m_pImagesProtocol->InsertContactStatusImage(strProtocolId, 
							IUOLMessengerContact::CS_UNKNOWN, pContactStatus->GetUserIconUnknown());
					}

					CProtocolStatusButtonElementPtr pStatusButton = pImageList->GetProtocolStatusButton();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pStatusButton = [%x].", pStatusButton.get());
					
					IUOLMessengerImagesProtocol3Ptr pImagesProtocol3 = 
							ap_dynamic_cast<IUOLMessengerImagesProtocol3Ptr>(m_pImagesProtocol);
					
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pImagesProtocol3 = [%x].", pImagesProtocol3.get());
					
					if ( (pStatusButton) && (pImagesProtocol3) )
					{
						pImagesProtocol3->InsertStatusButtonImage(strProtocolId, 
							IUOLMessengerAccountConnectionManager::AS_ONLINE, 
							pStatusButton->GetStatusButtonOnline());
						pImagesProtocol3->InsertStatusButtonImage(strProtocolId, 
							IUOLMessengerAccountConnectionManager::AS_BUSY, 
							pStatusButton->GetStatusButtonBusy());
						pImagesProtocol3->InsertStatusButtonImage(strProtocolId, 
							IUOLMessengerAccountConnectionManager::AS_AWAY, 
							pStatusButton->GetStatusButtonAway());
						pImagesProtocol3->InsertStatusButtonImage(strProtocolId, 
							IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY, 
							pStatusButton->GetStatusButtonAway());
						pImagesProtocol3->InsertStatusButtonImage(strProtocolId, 
							IUOLMessengerAccountConnectionManager::AS_OFFLINE, 
							pStatusButton->GetStatusButtonOffline());
                        pImagesProtocol3->InsertStatusButtonImage(strProtocolId, 
							IUOLMessengerAccountConnectionManager::AS_INVISIBLE, 
							pStatusButton->GetStatusButtonInvisible());
					}

					CProtocolContactBlockedElementPtr pContactBlocked = pImageList->GetProtocolContactBlocked();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pContactBlocked = [%x].", pContactBlocked.get());
					
					if (pContactBlocked)
					{
						m_pImagesProtocol->InsertContactBlockedImage(strProtocolId, 
							pContactBlocked->GetIconBlocked());
					}

					CProtocolContactAuthorizationElementPtr pContactAuthorizationElement = pImageList->GetProtocolContactAuthorizationElement();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"pContactAuthorizationElement = [%x].", pContactAuthorizationElement.get());
					
					if (pContactAuthorizationElement)
					{
						m_pImagesProtocol->InsertContactAuthorizationIcon(strProtocolId, 
							pContactAuthorizationElement->GetNotAuthorizedIcon(), FALSE);
					}
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogError(__FUNCTION__, "pImageList == NULL.");
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, "pBagElement == NULL.");
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pProtocol == NULL.");
		}

		itProtocolsList++;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CUIManager::RegisterObserver(CUOLMessengerUIManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CUIManager::UnregisterObserver(CUOLMessengerUIManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CUIManager::OnCreateHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	m_observerNotifier.NotifyAll(&CUOLMessengerUIManagerObserver::OnCreateHistoryWindow, pHistoryWindow);
}


void CUIManager::OnDestroyHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	m_observerNotifier.NotifyAll(&CUOLMessengerUIManagerObserver::OnDestroyHistoryWindow, pHistoryWindow);
}


void CUIManager::SetEmoticonAdditionOnTheWay(BOOL bEmoticonAdditionOnTheWay)
{
	m_bEmoticonAdditionOnTheWay = bEmoticonAdditionOnTheWay;
}


BOOL CUIManager::GetEmoticonAdditionOnTheWay()
{
	return m_bEmoticonAdditionOnTheWay;
}


void CUIManager::AddChatInviteContact(IUOLMessengerAccountPtr pAccount, IUOLMessengerContactPtr pContact, CAtlList<IUOLMessengerContactPtr>* pListChatInviteContacts)
{
	CString strChatInviteId;
	strChatInviteId.Format(_T("%s:%s"), pAccount->GetUserId(), pContact->GetName());

	m_mapChatInviteContacts.SetAt(strChatInviteId, pListChatInviteContacts);
}


void CUIManager::InviteContactsForChat(IUOLMessengerConversation2Ptr pConversation, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName)
{
	if (pConversation->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
	{
		return;
	}

	CString strChatInviteId;
	strChatInviteId.Format(_T("%s:%s"), pAccount->GetUserId(), strChatContactName);

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csLock);

		CChatInviteContactsMap::CPair* pPair = m_mapChatInviteContacts.Lookup(strChatInviteId);
		if (pPair)
		{
			CAtlList<IUOLMessengerContactPtr>* pListChatInviteContacts = pPair->m_value;

			for (POSITION pos = pListChatInviteContacts->GetHeadPosition(); pos;)
			{
				IUOLMessengerContactPtr pContact = pListChatInviteContacts->GetNext(pos);
				if (pContact)
				{
					// Verify if is the same account
					CString strConvUserId = pConversation->GetAccount()->GetUserId();
					CString strAccountUserId = pAccount->GetUserId();
					if (0 == strAccountUserId.Compare(strConvUserId))
					{
						// Invite this contact to chat.
						IUOLMessengerCommandPtr pCommand = new CInviteForChatCommand(
							pConversation, pContact, FALSE);
						if (pCommand)
						{
							CUIMApplication::GetApplication()->ExecuteCommand(pCommand);
						}
					}
				}
			}

			// Empty and delete list
			pListChatInviteContacts->RemoveAll();
			delete pListChatInviteContacts;
			m_mapChatInviteContacts.RemoveKey(strChatInviteId);
		}
	}
}


void CUIManager::ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, IUOLMessengerHistoryRecordListPtr pHistoryRecordList, const CString& strWindowCaption)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();	
	
	CString strTitle;
	strTitle = pStringLoader->GetValue("uol.resource.IDS_HISTORY_WINDOW_TITLE");
	strTitle.Append(strWindowCaption);

	CString strWndKey;
	strWndKey.Format("%s:%s", pHistoryRecord->GetProtocolId(), 
		pHistoryRecord->GetContactName());
		
	CHistoryFramePtr pHistoryFrame;

	CHistoryWindowsMap::CPair* pPair;
	pPair = m_mapHistoryWindows.Lookup(strWndKey);

	if (pPair != NULL)
	{
		pHistoryFrame = pPair->m_value;
	}
	
	if ((!pHistoryFrame) || (!pHistoryFrame->IsWindow()))
	{
		CHistoryFrameBuilderPtr pBuilder = ap_dynamic_cast<CHistoryFrameBuilderPtr>(m_windowManager.GetElement(_T("__HistoryFrameBuilderInstance")));

		ATLASSERT(pBuilder.get());

		pHistoryFrame = ap_dynamic_cast<CHistoryFramePtr>(pBuilder->CreateElement());	
		ATLASSERT(pHistoryFrame.get());

		if (pHistoryFrame.get())
		{
			pHistoryFrame->SetHistoryRecordList(pHistoryRecordList);
			pHistoryFrame->getHistoryWindow()->RegisterObserver(this);
			pHistoryFrame->CreateHistoryWindow(::GetDesktopWindow());
			pHistoryFrame->SetCaptionText(strTitle);
			m_mapHistoryWindows.SetAt(strWndKey, pHistoryFrame);
		}
	}

	ATLASSERT(pHistoryFrame.get());

	if (pHistoryFrame.get())
	{
		pHistoryFrame->SetHistoryRecordList(pHistoryRecordList);
		pHistoryFrame->BringWindowToTop();
		pHistoryFrame->UpdateEventsList();
		pHistoryFrame->ShowWindow(SW_SHOW);
	}
}


void CUIManager::WaitSystrayMenuInitialization(void* pUOLMessengerPlugin)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	CContextMenuPtr pSystrayContextMenu;
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "WaitSystrayMenuInitialization")) )
	{
		do
		{
			pSystrayContextMenu = m_pwndAppWindow->GetSystrayContextMenu();
			
			if (pSystrayContextMenu == NULL)
			{
				::WaitForSingleObject(m_waitingEvent, SYSTRAY_MENU_WAITING_TIMEOUT);
			}
		}
		while (pSystrayContextMenu == NULL);
	}
}


IUOLMessengerContextMenuPtr CUIManager::AddSystrayMenuItem(void* pUOLMessengerPlugin, CString strMenuTitle)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	CContextMenuPtr pSystrayContextMenu;
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "AddSystrayMenuItem")) )
	{
		pSystrayContextMenu = m_pwndAppWindow->GetSystrayContextMenu();
		
		if (pSystrayContextMenu)
		{
			if (!m_bHasAddedSystrayMenuSeparator)
			{
				pSystrayContextMenu->InsertSeparator(0);
				
				m_bHasAddedSystrayMenuSeparator = TRUE;
			}
			
			CString strMenuLabel = strMenuTitle;
			
			if (strMenuLabel.IsEmpty())
			{
				strMenuLabel = ((CUOLMessengerPlugin*)pUOLMessengerPlugin)->GetName();
			}
			
			CContextMenuPtr pPluginMenu = new CContextMenu(strMenuLabel);
			
			if ( (pPluginMenu != NULL) && (pPluginMenu->CreateMenu()) )
			{
				m_pluginMenuMap.SetAt(strMenuLabel, pPluginMenu);
				pSystrayContextMenu->InsertItem(0, pPluginMenu);
			}
			
			return pPluginMenu;
		}
	}
	
	return IUOLMessengerContextMenuPtr();
}


void CUIManager::RemoveSystrayMenuItem(void* pUOLMessengerPlugin, CString strMenuTitle)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	CContextMenuPtr pSystrayContextMenu;
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "RemoveSystrayMenuItem")) )
	{
		pSystrayContextMenu = m_pwndAppWindow->GetSystrayContextMenu();
		
		if (pSystrayContextMenu)
		{
			CString strMenuLabel = strMenuTitle;
			
			if (strMenuLabel.IsEmpty())
			{
				strMenuLabel = ((CUOLMessengerPlugin*)pUOLMessengerPlugin)->GetName();
			}
			
			CPluginMenuMap::CPair* pPair = m_pluginMenuMap.Lookup(strMenuLabel);
			
			if (pPair != NULL)
			{
				pPair->m_value->RemoveAllMenuElements();
				
				pSystrayContextMenu->RemoveItem(
						ap_static_cast<IUOLMessengerContextMenuItemPtr>(pPair->m_value));
				
				// Force the destruction considering that the creation of this submenu 
				// could be made by a thread what will lead to first "IsWindow()" test 
				// failure.
				pPair->m_value->DestroyMenu(TRUE);
				
				m_pluginMenuMap.RemoveKey(strMenuLabel);
				
				if (m_pluginMenuMap.GetCount() == 0)
				{
					pSystrayContextMenu->RemoveItem(0);
					
					m_bHasAddedSystrayMenuSeparator = FALSE;
				}
			}
		}
	}
}


IUOLMessengerPluginPreferencesPageContainerPtr CUIManager::GetPluginPreferencesPageContainer()
{
	return m_pPluginPreferencesPageContainer;
}


IUOLMessengerHistoryManagerPtr CUIManager::GetHistoryManager()
{
	return CUIMApplication::GetApplication()->GetHistoryManager();
}


HWND CUIManager::GetFileTransferWindow()
{
	if (m_pFileTransferDialogManager)
	{
		return m_pFileTransferDialogManager->GetFileTransferWindow();
	}

	return NULL;
}


BOOL CUIManager::IsBugReportActive()
{
	return m_bIsBugReportActive;
}


void CUIManager::SetBugReportActive(BOOL bActive)
{
	m_bIsBugReportActive = bActive;
}



//---------------------------------------------------------------------------\\
// IUOLMessengerUIManager Deprecated methods
void CUIManager::ShowHistory()
{
    //Deprecated method, please use ShowHistoryWindow() instead.
    ATLASSERT(FALSE);
}


void CUIManager::ShowHistory(const CString& strFilePath)
{
    //Deprecated method, please use ShowHistoryWindow() instead.
    ATLASSERT(FALSE);
}


void CUIManager::UpdateHistory(const CString& strFilename)
{
    //Deprecated method
    ATLASSERT(FALSE);
}

// IUOLMessengerUIManager4 Deprecated methods
void CUIManager::ShowHistory(IUOLMessengerContactPtr pContact)
{
    //Deprecated method, please use ShowHistoryWindow() instead.
    ATLASSERT(FALSE);
}

void CUIManager::ShowHistory(const CString& strFilePath, IUOLMessengerContactPtr pContact)
{
    //Deprecated method, please use ShowHistoryWindow() instead.
    ATLASSERT(FALSE);
}

void CUIManager::UpdateHistory(const CString& strFilename, IUOLMessengerContactPtr pContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}

void CUIManager::GetPreviousFile(HTREEITEM hSelectedItem, IUOLMessengerContactPtr pContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CUIManager::GetNextFile(HTREEITEM hSelectedItem,IUOLMessengerContactPtr pContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


// IUOLMessengerUIManager5 Deprecated methods
void CUIManager::ShowHistory(IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter, const CString& strCallStatus)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CUIManager::ShowHistory(const CString& strFilePath, IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter,const CString& strCallStatus)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CUIManager::UpdateHistory(const CString& strFilename, IUOLMessengerHistoryContactPtr pHistContact, const CString& strParameter, const CString& strCallStatus)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CUIManager::GetPreviousFile(HTREEITEM hSelectedItem, IUOLMessengerHistoryContactPtr pHistContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CUIManager::GetNextFile(HTREEITEM hSelectedItem,IUOLMessengerHistoryContactPtr pHistContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CUIManager::DeleteMultipleHistory(IUOLMessengerHistoryContactPtr pHistContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


// IUOLMessengerUIManager6 Deprecated methods
void CUIManager::ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, const CString& strWindowCaption)
{
	// Deprecated method, please use ShowHistoryWindow(IUOLMessengerHistoryRecordPtr pHistoryRecord, IUOLMessengerHistoryRecordListPtr pHistoryRecordList, const CString& strWindowCaption) instead.
	ATLASSERT(FALSE);
}
