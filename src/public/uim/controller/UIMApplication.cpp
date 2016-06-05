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

#include "stdafx.h"
#include "UIMApplication.h"
#include "../resource.h"

#include "HistoryManager.h"
#include "SettingsManager.h"
#include "AccountManager.h"
#include "ProtocolManager.h"
#include "UserIconCacheManager.h"
#include "UIManager.h"
#include "ThemeManager.h"
#include "ProfileManager.h"
#include "CommandFactory.h"
#include "ContactList.h"
#include "FileTransfer/FileTransferManager.h"
#include "UIMLiveUpdate.h"
#include "UIMLiveUpdateCallback.h"
#include "../view/UOLMessengerTrayWindow.h"
#include "NotifyPopUpManager.h"
#include "AuthenticationManager.h"
#include "Message/MessageReceivingFilter.h"
#include "diskcache/DiskCacheManager.h"
#include "InternetConnection/InternetConnectionMonitor.h"
#include "AccountConnection/AccountConnectionManager.h"
#include "RemoteContactList/RemoteContactList.h"
#include "Conversation/ConversationManager.h"
#include "Privacy/PrivacyManager.h"
#include "../model/CommonSettings.h"
#include "EmoticonParserManager.h"
#include "UIMApplicationInitHelper.h"


#include <commands/showprivacydialogcommand.h>


#define STR_DIAGNOSE_TOOL_PARAM "/diagnose"


static CUIMApplicationInitHelper	g_UIMApplicationInitHelper;

HHOOK hhook = NULL;
HWND hWndMain= NULL;
static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fCtrlPressed = (GetAsyncKeyState(VK_CONTROL) < 0);

	DWORD pID = 0;
	BOOL fHandled = FALSE;

	HWND hWndCtl = ::GetFocus();
	GetWindowThreadProcessId(hWndCtl, &pID);

	if ((GetCurrentProcessId() != pID)) 
		return CallNextHookEx(hhook, nCode, wParam, lParam);

	if (nCode == HC_ACTION)
	{
		if (static_cast<int>(lParam) >= 0)
		{		
			switch (wParam)
			{
			/* CTRL + P */
			case 'p':
			case 'P':
				{
					if (fCtrlPressed == TRUE)
					{
						IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

						if (pUOLMessengerFacade)
						{
							pUOLMessengerFacade->ExecuteCommand(new CShowPreferencesCommand());
						}
						fHandled = TRUE;
					}
					break;
				}
			/* CTRL + A */
			/*
			case 'a':
			case 'A':
				{

					if (fCtrlPressed == TRUE)
					{
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
						fHandled = TRUE;
					}
					break;
				}
			*/
			/* CTRL + B */
			case 'b':
			case 'B':
				{
					if (fCtrlPressed == TRUE)
					{
						IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

						if (pUOLMessengerFacade)
						{
							pUOLMessengerFacade->ExecuteCommand(new CShowPrivacyDialogCommand(pUOLMessengerFacade->GetUIManager()->GetAppWindow()));
						}
						fHandled = TRUE;
					}
					break;
				}
			/* CTRL + O */
			case 'o':
			case 'O':
				{
					if (fCtrlPressed == TRUE)
					{
						IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

						if (pUOLMessengerFacade)
						{
							IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
							IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("SetOnlineStatusAll"));
							if (pCommand)
							{
								pUOLMessengerFacade->ExecuteCommand(pCommand);
							}
						}
						fHandled = TRUE;
					}
					break;
				}
			/* CTRL + D */
			case 'd':
			case 'D':
				{
					if (fCtrlPressed == TRUE)
					{
						IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

						if (pUOLMessengerFacade)
						{
							IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
							IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("DisconnectAllAccounts"));
							if (pCommand)
							{
								pUOLMessengerFacade->ExecuteCommand(pCommand);
							}
						}
						fHandled = TRUE;
					}
					break;
				}			
			}
		}
		else
		{
			switch (wParam)
			{			
			case VK_F1:
				{
					IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

					if (pUOLMessengerFacade)
					{
						IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
						IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowHelp"));
						if (pCommand)
						{
							pUOLMessengerFacade->ExecuteCommand(pCommand);
						}
					}
				}

				break;
			}			
		}
	}

	return (fHandled ? TRUE : CallNextHookEx(hhook, nCode, wParam, lParam));
}

void InstallHook()
{
	hhook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, NULL, ::GetCurrentThreadId());
}

void RemoveHook()
{
	if(hhook != NULL)
		UnhookWindowsHookEx(hhook);
}


struct CMessageLoopTask : public ITask
{
	CMessageLoopTask(CMessageLoop &theLoop) : 
		m_theLoop(theLoop)
	{
		m_strTaskDescription = "Message Loop Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		_Module.AddMessageLoop(&m_theLoop);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		_Module.RemoveMessageLoop();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	CMessageLoop& m_theLoop;
};


struct CCommonSettingsTask : public ITask
{
	CCommonSettingsTask(IUOLMessengerCommonSettingsPtr& pCommonSettings) :
		m_pCommonSettings(pCommonSettings)
	{
		m_strTaskDescription = "Common Settings Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bTaskResult = FALSE;

		m_pCommonSettings = new CCommonSettings();

		if (m_pCommonSettings)
		{			
			 bTaskResult = m_pCommonSettings->LoadSettings();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pCommonSettings == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bTaskResult = [%d]", bTaskResult);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bTaskResult;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pCommonSettings = [%x]", m_pCommonSettings.get());
		
		if (m_pCommonSettings)
		{
			m_pCommonSettings->SaveSettings();
			m_pCommonSettings = NULL;
		}	
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerCommonSettingsPtr&	m_pCommonSettings;
};


struct CProfileManagerTask : public ITask
{
	CProfileManagerTask(IUOLMessengerProfileManagerPtr& pProfileManager, DWORD dwTaskState) :
		m_pProfileManager(pProfileManager),
		m_dwTaskState(dwTaskState)
	{
		m_strTaskDescription = "Profile Manager Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_dwTaskState = [%d].", m_dwTaskState);
		
		BOOL bRet = FALSE;
		
		if(m_dwTaskState == IUOLMessengerProfileManager::TS_INIT)
		{
			m_pProfileManager = new CProfileManager();

			if (m_pProfileManager)
			{			
				if (m_pProfileManager->LoadSettings())
				{
					bRet = m_pProfileManager->ShowWizardInit();
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"m_pProfileManager == NULL.");
			}
		}
		else if (m_dwTaskState == IUOLMessengerProfileManager::TS_ACCOUNTS)
		{
			if (m_pProfileManager)
			{		
				bRet = m_pProfileManager->ShowWizardAccounts();
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"m_pProfileManager == NULL.");
			}
		}
		
		if(bRet == TRUE)
		{
			InstallHook();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pProfileManager = [%x].", m_pProfileManager.get());
		
		if (m_pProfileManager)
		{
			m_pProfileManager->SaveSettings();
		}	
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerProfileManagerPtr&		m_pProfileManager;
	DWORD								m_dwTaskState;

};

struct CHistoryManagerTask : public ITask {
	CHistoryManagerTask(IUOLMessengerHistoryManager4Ptr& pHistoryManager) :
		m_pHistoryManager(pHistoryManager)
	{
		m_strTaskDescription = "History Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pHistoryManager = new CHistoryManager();

	/*	if (m_pHistoryManager)
		{			
			//return m_pSettingsManager->LoadSettings();
			return TRUE;
		}*/
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pHistoryManager = [%x]", m_pHistoryManager.get());
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;	
	}

	virtual void UndoTask()
	{
	}

    virtual void PreUndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pHistoryManager = [%x].", m_pHistoryManager.get());
		
		if (m_pHistoryManager)
		{
			static_cast<CHistoryManager*>(m_pHistoryManager.get())->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerHistoryManager4Ptr&	m_pHistoryManager;
	
};

struct CSettingsManagerTask : public ITask
{
	CSettingsManagerTask(IUOLMessengerSettingsManager2Ptr& pSettingsManager, IUOLMessengerProfileManagerPtr& pProfileManager) :
		m_pSettingsManager(pSettingsManager),
		m_pProfileManager(pProfileManager)
	{
		m_strTaskDescription = "Settings Manager Task";
	}

	virtual BOOL DoTask()
	{
		BOOL bInitOk = TRUE;
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pSettingsManager = new CSettingsManager(m_pProfileManager);

		if (m_pSettingsManager)
		{			
			bInitOk = m_pSettingsManager->LoadSettings();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pSettingsManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bInitOk = [%d]", bInitOk);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bInitOk;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pSettingsManager = [%x].", m_pSettingsManager.get());
		
		if (m_pSettingsManager)
		{
			m_pSettingsManager->SaveSettings();
		}	
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerSettingsManager2Ptr&	m_pSettingsManager;
	IUOLMessengerProfileManagerPtr&		m_pProfileManager;
};

//struct CThemeManagerTask : public ITask
//{
//	CThemeManagerTask(IUOLMessengerThemeManagerPtr& pThemeManager, IUOLMessengerCommonSettingsPtr& pCommonSettings) :
//		m_pThemeManager(pThemeManager),
//		m_pCommonSettings(pCommonSettings) { }
//
//	virtual BOOL DoTask()
//	{	
//		CString strCaption, strMessage;
//
//		strCaption.LoadString(IDS_APP_NAME);
//		m_pThemeManager = new CThemeManager(m_pCommonSettings);
//
//		if (m_pThemeManager)
//		{
//			IUOLMessengerThemeManager::EnLoadTheme enLoadTheme = m_pThemeManager->LoadTheme();
//
//			if (enLoadTheme == IUOLMessengerThemeManager::LT_CURRENT_THEME_OK)
//			{
//				return TRUE;
//			}
//			else
//			{
//                if (enLoadTheme == IUOLMessengerThemeManager::LT_CURRENT_THEME_INVALID)
//				{
//					strMessage.LoadString(IDS_UIM_APPLICATION_CURRENT_THEME_INVALID);
//					MessageBox(NULL, strMessage, strCaption, MB_ICONWARNING);
//
//					return TRUE;
//				}
//				else if(enLoadTheme == IUOLMessengerThemeManager::LT_DEFAULT_THEME_INVALID)
//				{
//					strMessage.LoadString(IDS_UIM_APPLICATION_DEFAULT_THEME_INVALID);
//					MessageBox(NULL, strMessage, strCaption, MB_ICONERROR);
//
//					return FALSE;
//				}
//			}
//		}
//
//		return FALSE;
//
//	}
//
//	virtual void UndoTask()
//	{
//		if (m_pThemeManager)
//		{
//			m_pThemeManager->SaveTheme();
//		}	
//	}
//
//	virtual BOOL GetRestartFlag() { return FALSE; }
//
//	IUOLMessengerThemeManagerPtr&		m_pThemeManager;
//	IUOLMessengerCommonSettingsPtr&		m_pCommonSettings;
//};

struct CUIPreInitTask : public ITask
{
	CUIPreInitTask(IUOLMessengerUIManagerPtr& pUIManager, CUIMApplication* pApplication) :
		m_pUIManager(pUIManager),
		m_pApplication(pApplication),
		m_bRestartFlag(FALSE)
	{
		m_strTaskDescription = "User Interface PreInit Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pUIManager = new CUIManager();

		if (m_pUIManager)
		{
			DWORD dwError = static_cast<CUIManager*>(m_pUIManager.get())->PreInit(m_pApplication);
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"dwError = [%d].", dwError);
			
			if (dwError == CUIManager::UIM_ERROR_USER_SKIN_PARSING)
			{
				m_bRestartFlag = TRUE;
			}

			bRet = (dwError == CUIManager::UIM_ERROR_SUCCESS);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pUIManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pUIManager = [%x].", m_pUIManager.get());
		
		if (m_pUIManager)
		{
			m_pUIManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag()
	{ 
		return m_bRestartFlag; 
	}

	IUOLMessengerUIManagerPtr& m_pUIManager;
	CUIMApplication* m_pApplication;
	BOOL m_bRestartFlag;
};

struct CUserIconCacheManagerTask : public ITask
{
	CUserIconCacheManagerTask(IUOLMessengerUserIconCacheManagerPtr& pUserIconCacheManager, IUOLMessengerProfileManagerPtr& pProfileManager) : 
		m_pUserIconCacheManager(pUserIconCacheManager),
		m_pProfileManager(pProfileManager)
	{
		m_strTaskDescription = "User Icon Cache Manager Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pUserIconCacheManager = new CUserIconCacheManager();

		if (m_pUserIconCacheManager)
		{
			bRet = static_cast<CUserIconCacheManager*>(m_pUserIconCacheManager.get())->Init(m_pProfileManager);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pUserIconCacheManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pUserIconCacheManager = [%x].", m_pUserIconCacheManager.get());
		
		if (m_pUserIconCacheManager)
		{
			m_pUserIconCacheManager->PurgeAccountIcons();
			m_pUserIconCacheManager->ClearContactIcons();

			static_cast<CUserIconCacheManager*>(m_pUserIconCacheManager.get())->Finalize();
			
			m_pUserIconCacheManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerUserIconCacheManagerPtr&	m_pUserIconCacheManager;
	IUOLMessengerProfileManagerPtr&			m_pProfileManager;
};


struct CContactListTask : public ITask
{
	CContactListTask(IUOLMessengerContactListPtr& pContactList) : 
		m_pContactList(pContactList)
	{
		m_strTaskDescription = "Contact List Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pContactList = new CContactList();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pContactList = [%x]", m_pContactList.get());
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pContactList.reset();	
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerContactListPtr& m_pContactList;
};

struct CCommandFactoryTask : public ITask
{
	CCommandFactoryTask(IUOLMessengerCommandFactoryPtr& pCommandFactory) :
		m_pCommandFactory (pCommandFactory)
	{
		m_strTaskDescription = "Command Factory Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pCommandFactory = new CUOLCommandFactory();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pCommandFactory = [%x]", m_pCommandFactory.get());
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return TRUE;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, 
				"Doing nothing by now.");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerCommandFactoryPtr& m_pCommandFactory;
};

struct CDiskCacheTask : public ITask
{
	CDiskCacheTask(IDiskCacheManagerPtr& pDiskCacheManager) :
		m_pDiskCacheManager(pDiskCacheManager)
	{
		m_strTaskDescription = "Disk Cache Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pDiskCacheManager = new CDiskCacheManager();

		if (m_pDiskCacheManager)
		{
			bRet = static_cast<CDiskCacheManager*>(m_pDiskCacheManager.get())->Init();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pDiskCacheManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pDiskCacheManager = [%x]", m_pDiskCacheManager.get());
		
		if (m_pDiskCacheManager)
		{
			static_cast<CDiskCacheManager*>(m_pDiskCacheManager.get())->Finalize();

			m_pDiskCacheManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IDiskCacheManagerPtr&	m_pDiskCacheManager;
};

struct CUITask : public ITask
{
	CUITask(IUOLMessengerUIManagerPtr& pUIManager, CUIMApplication* pApplication) :
		m_pUIManager(pUIManager),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "User Interface Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		if (m_pUIManager)
		{
			DWORD dwError = static_cast<CUIManager*>(m_pUIManager.get())->Init(m_pApplication);
			
			bRet = (dwError == CUIManager::UIM_ERROR_SUCCESS);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pUIManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pUIManager = [%x]", m_pUIManager.get());
		
		if (m_pUIManager)
		{
			static_cast<CUIManager*>(m_pUIManager.get())->Finalize();

			m_pUIManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() 
    {
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pUIManager = [%x]", m_pUIManager.get());
		
        if (m_pUIManager)
		{
			static_cast<CUIManager*>(m_pUIManager.get())->PreFinalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
    }

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerUIManagerPtr& m_pUIManager;
	CUIMApplication* m_pApplication;
};

struct CProtocolTask : public ITask
{
	CProtocolTask(IUOLMessengerProtocolManagerPtr& pProtocolManager) :
		m_pProtocolManager(pProtocolManager)
	{
		m_strTaskDescription = "Protocol Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pProtocolManager = new CProtocolManager();

		if (m_pProtocolManager != NULL)
		{
			bRet = static_cast<CProtocolManager*>(m_pProtocolManager.get())->Init();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pProtocolManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pProtocolManager = [%x]", m_pProtocolManager);
		
		if (m_pProtocolManager)
		{
			static_cast<CProtocolManager*>(m_pProtocolManager.get())->Finalize();
			m_pProtocolManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() 
    {
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pProtocolManager = [%x]", m_pProtocolManager.get());
		
        if (m_pProtocolManager)
		{
			static_cast<CProtocolManager*>(m_pProtocolManager.get())->PreFinalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
    }

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerProtocolManagerPtr&	m_pProtocolManager;
};

struct CAccountInitTask : public ITask
{
	CAccountInitTask(IUOLMessengerAccountManagerPtr& pAccountManager, CUIMApplication* pApplication) : 
		m_pAccountManager(pAccountManager),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Account Init Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pAccountManager = new CAccountManager();

		if (m_pAccountManager != NULL)
		{
			bRet = static_cast<CAccountManager*>(m_pAccountManager.get())->Init(m_pApplication);

			if (bRet)
			{
				bRet = m_pAccountManager->LoadAccounts();

				static_cast<CAccountManager*>(m_pAccountManager.get())->UpdateAccountsAuthenticationData();
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pAccountManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pAccountManager = [%x]", m_pAccountManager.get());
		
		if (m_pAccountManager != NULL)
		{
			m_pAccountManager->SaveAccounts();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerAccountManagerPtr& m_pAccountManager;
	CUIMApplication* m_pApplication;
};


struct CAccountPreInitTask : public ITask
{
	CAccountPreInitTask(IUOLMessengerAccountManagerPtr& pAccountManager) : 
		m_pAccountManager(pAccountManager)
	{
		m_strTaskDescription = "Account PreInit Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Doing nothing on Init by now.");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return TRUE;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pAccountManager = [%x]", m_pAccountManager.get());
		
		if (m_pAccountManager)
		{
			static_cast<CAccountManager*>(m_pAccountManager.get())->Finalize();
			m_pAccountManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerAccountManagerPtr& m_pAccountManager;
};


struct CProtocolCallbackTask : public ITask
{
	CProtocolCallbackTask(CProtocolCallbackController& protocolCallbackController, CUIMApplication* pApplication) : 
		m_protocolCallbackController(protocolCallbackController),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Protocol Callback Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bInitOk = m_protocolCallbackController.Init(m_pApplication);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bInitOk = [%d]", bInitOk);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return bInitOk;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_protocolCallbackController.Finalize();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	CProtocolCallbackController& m_protocolCallbackController;
	CUIMApplication* m_pApplication;
};


struct CNotifyPopUpTask : public ITask
{
	CNotifyPopUpTask(IUOLMessengerNotifyPopUpManagerPtr& pNotifyPopUpManager) : 
		m_pNotifyPopUpManager(pNotifyPopUpManager)
	{
		m_strTaskDescription = "Notify PopUp Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pNotifyPopUpManager = new CNotifyPopUpManager();

		if (m_pNotifyPopUpManager != NULL)
		{
			bRet = static_cast<CNotifyPopUpManager*>(m_pNotifyPopUpManager.get())->Init();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pNotifyPopUpManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pNotifyPopUpManager = [%x]", m_pNotifyPopUpManager.get());
		
		if (m_pNotifyPopUpManager != NULL)
		{
			static_cast<CNotifyPopUpManager*>(m_pNotifyPopUpManager.get())->Finalize();
			m_pNotifyPopUpManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerNotifyPopUpManagerPtr& m_pNotifyPopUpManager;
};

struct CTrayTask : public ITask
{
	CTrayTask(IUOLMessengerTrayWindowPtr& pTrayWindow, CUIMApplication* pApplication) : 
		m_pTrayWindow(pTrayWindow),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Tray Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pTrayWindow = new CUOLMessengerTrayWindow();

		if (m_pTrayWindow != NULL)
		{
			bRet = static_cast<CUOLMessengerTrayWindow*>(m_pTrayWindow.get())->Init(m_pApplication);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pTrayWindow == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pTrayWindow = [%x]", m_pTrayWindow.get());
		
		if (m_pTrayWindow)
		{
			m_pTrayWindow.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerTrayWindowPtr& m_pTrayWindow;
	CUIMApplication* m_pApplication;
};

struct CThreadTask : public ITask
{
	CThreadTask(CJobThreadPtr	pThreadJobs, CUIMApplication* pApplication) : 
		m_pThreadJobs(pThreadJobs),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Thread Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pThreadJobs = new CJobThread(FALSE);
		m_pThreadJobs->AddNewJob(new CUIMLiveUpdate<CUIMLiveUpdateCallback>(CUIMLiveUpdateCallback(*m_pApplication, *m_pApplication), *m_pApplication));

		BOOL bInit = IdleTrackInit();
		ATLASSERT(bInit);		
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bInit = [%d], m_pThreadJobs = [%x].", bInit, m_pThreadJobs.get());
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bFinalize = IdleTrackFinalize();
		ATLASSERT(bFinalize);		

		m_pThreadJobs->Stop();
		m_pThreadJobs->Wait();
		
		m_pThreadJobs.reset();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	CJobThreadPtr		m_pThreadJobs;
	CUIMApplication* m_pApplication;
};


struct CNotifyPluginThreadTask : public ITask
{
	CNotifyPluginThreadTask(CJobThreadPtr	pThreadJobs, CUIMApplication* pApplication) : 
		m_pThreadJobs(pThreadJobs),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Notify Plugin Thread Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pThreadJobs = new CJobThread(FALSE);
		m_pThreadJobs->AddNewJob(new CUIMPluginUpdate(*m_pApplication));
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pThreadJobs = [%x]", m_pThreadJobs);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pThreadJobs->Stop();
		m_pThreadJobs->Wait();

		m_pThreadJobs.reset();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	CJobThreadPtr		m_pThreadJobs;
	CUIMApplication* m_pApplication;
};



struct CFileTransferManagerTask : public ITask
{
	CFileTransferManagerTask(IUOLMessengerFileTransferManagerPtr& pFileTransferManager) : 
		m_pFileTransferManager(pFileTransferManager)
	{
		m_strTaskDescription = "File Transfer Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pFileTransferManager = new CFileTransferManager();

		if (m_pFileTransferManager != NULL)
		{
			bRet = static_cast<CFileTransferManager*>(m_pFileTransferManager.get())->Init();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pFileTransferManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pFileTransferManager = [%x]", m_pFileTransferManager);
		
		if (m_pFileTransferManager)
		{
			static_cast<CFileTransferManager*>(m_pFileTransferManager.get())->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerFileTransferManagerPtr& m_pFileTransferManager;
};

struct CPluginManagerTask : public ITask
{
	CPluginManagerTask(CUOLMessengerPluginManager& messengerPluginManager) :
		m_messengerPluginManager(messengerPluginManager)
	{
		m_strTaskDescription = "Plugin Manager Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bInit = m_messengerPluginManager.Init();

		if (bInit)
		{
			m_messengerPluginManager.LoadPlugins();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bInit = [%d]", bInit);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bInit;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_messengerPluginManager.UnloadPlugins();
		m_messengerPluginManager.Finalize();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	CUOLMessengerPluginManager& m_messengerPluginManager;
};


struct CAuthenticationManagerTask : public ITask
{
	CAuthenticationManagerTask(IUOLMessengerAuthenticationManagerPtr& AuthenticationManager) :
		m_pAuthenticationManager(AuthenticationManager)
	{
		m_strTaskDescription = "Authentication Manager Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pAuthenticationManager = new CAuthenticationManager();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pAuthenticationManager = [%x]", m_pAuthenticationManager.get());
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pAuthenticationManager = [%x]", m_pAuthenticationManager.get());
		
		m_pAuthenticationManager->StopReAuthentication();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerAuthenticationManagerPtr& m_pAuthenticationManager;
};


struct CAuthenticationTask : public ITask
{
	CAuthenticationTask(IUOLMessengerAuthenticationManagerPtr& AuthenticationManager) :
		m_pAuthenticationManager(AuthenticationManager)
	{
		m_strTaskDescription = "Authentication Task";
	} 

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		IUOLMessengerProfileManagerPtr pProfileManager;
		pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();
		
		if (pProfileManager.get()!=NULL) 
		{
			CString user, pass;
			user = pProfileManager->GetCurrentProfile();
            
			IUOLMessengerProfileElementPtr pProfileElement;
			pProfileManager->GetProfileElement(user, pProfileElement);

			ATLASSERT(pProfileElement.get());

			if (pProfileElement && pProfileElement->IsEnableAuthentication())
			{
				pass = pProfileElement->GetProfilePassword();

				ATLTRACE("%s Trying to authenticate user %s",__FUNCTION__,user);
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Trying to authenticate user [%s].", user);
				
				if  (m_pAuthenticationManager->DoAuthentication(user, pass,TRUE)==FALSE)
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogWarning(__FUNCTION__, 
							"m_pAuthenticationManager->DoAuthentication() failed for user [%s].", user);
					
					ATLTRACE("%s Cannot Authenticate",__FUNCTION__);
				}
			}
			else if (pProfileElement.get() == NULL)
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"pProfileElement == NULL.");
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"pProfileElement->IsEnableAuthentication() == FALSE.");
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pProfileManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return TRUE;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, 
				"Doing nothing by now.");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerAuthenticationManagerPtr& m_pAuthenticationManager;
};

struct CAccountConnectionManagerTask : public ITask
{
	CAccountConnectionManagerTask(IUOLMessengerAccountConnectionManagerPtr& pAccountConnectionManager, CUIMApplication* pApplication) : 
		m_pAccountConnectionManager(pAccountConnectionManager),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Account Connection Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pAccountConnectionManager = new CAccountConnectionManager();

		if (m_pAccountConnectionManager != NULL)
		{
			bRet = static_cast<CAccountConnectionManager*>(m_pAccountConnectionManager.get())->Init(m_pApplication);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pAccountConnectionManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pAccountConnectionManager = [%x]", m_pAccountConnectionManager);
		
		if (m_pAccountConnectionManager != NULL)
		{
			static_cast<CAccountConnectionManager*>(m_pAccountConnectionManager.get())->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pAccountConnectionManager = [%x]", m_pAccountConnectionManager);
		
		if (m_pAccountConnectionManager != NULL)
		{
			static_cast<CAccountConnectionManager*>(m_pAccountConnectionManager.get())->PreFinalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerAccountConnectionManagerPtr& m_pAccountConnectionManager;
	CUIMApplication* m_pApplication;
};


struct CRemoteContactListTask : public ITask
{
	CRemoteContactListTask(IUOLMessengerRemoteContactListPtr& pRemoteContactList, CUIMApplication* pApplication) : 
		m_pRemoteContactList(pRemoteContactList),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Remote Contact List Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pRemoteContactList = new CRemoteContactList();

		if (m_pRemoteContactList != NULL)
		{
			bRet = static_cast<CRemoteContactList*>(m_pRemoteContactList.get())->Init(m_pApplication);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pRemoteContactList == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pRemoteContactList = [%x]", m_pRemoteContactList.get());
		
		if (m_pRemoteContactList != NULL)
		{
			static_cast<CRemoteContactList*>(m_pRemoteContactList.get())->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerRemoteContactListPtr& m_pRemoteContactList;
	CUIMApplication* m_pApplication;
};

struct CConversationManagerTask : public ITask
{
	CConversationManagerTask(IUOLMessengerConversationManagerPtr& pConversationManager, CUIMApplication* pApplication) : 
		m_pConversationManager(pConversationManager),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Conversation Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pConversationManager = new CConversationManager();

		if (m_pConversationManager != NULL)
		{
			bRet = static_cast<CConversationManager*>(m_pConversationManager.get())->Init(m_pApplication);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pConversationManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pConversationManager = [%x]", m_pConversationManager);
		
		if (m_pConversationManager)
		{
			static_cast<CConversationManager*>(m_pConversationManager.get())->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerConversationManagerPtr& m_pConversationManager;
	CUIMApplication* m_pApplication;
};

struct CPrivacyManagerTask : public ITask
{
	CPrivacyManagerTask(IUOLMessengerPrivacyManagerPtr& pPrivacyManager, CUIMApplication* pApplication) : 
		m_pPrivacyManager(pPrivacyManager),
		m_pApplication(pApplication)
	{
		m_strTaskDescription = "Privacy Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pPrivacyManager = new CPrivacyManager();

		if (m_pPrivacyManager != NULL)
		{
			bRet = static_cast<CPrivacyManager*>(m_pPrivacyManager.get())->Init(m_pApplication);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pPrivacyManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pPrivacyManager = [%x]", m_pPrivacyManager);
		
		if (m_pPrivacyManager != NULL)
		{
			static_cast<CPrivacyManager*>(m_pPrivacyManager.get())->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerPrivacyManagerPtr& m_pPrivacyManager;
	CUIMApplication* m_pApplication;
};

struct CMessageReceivingFilterTask : public ITask
{
	CMessageReceivingFilterTask(IUOLMessengerMessageReceivingFilterPtr& pMessageReceivingFilter) : 
		m_pMessageReceivingFilter(pMessageReceivingFilter)
	{
		m_strTaskDescription = "Message Receiving Filter Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		m_pMessageReceivingFilter = new CMessageReceivingFilter();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pMessageReceivingFilter = [%x]", m_pMessageReceivingFilter.get());
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return TRUE;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pMessageReceivingFilter = [%x]", m_pMessageReceivingFilter.get());
		
		m_pMessageReceivingFilter.reset();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerMessageReceivingFilterPtr& m_pMessageReceivingFilter;
};


struct CInternetConnectionMonitorTask : public ITask
{
	CInternetConnectionMonitorTask(IUOLMessengerInternetConnectionMonitorPtr& pInternetConnectionMonitor) : 
		m_pInternetConnectionMonitor(pInternetConnectionMonitor)
	{
		m_strTaskDescription = "Internet Connection Monitor Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;

		m_pInternetConnectionMonitor = new CInternetConnectionMonitor();

		if (m_pInternetConnectionMonitor != NULL)
		{
			bRet = static_cast<CInternetConnectionMonitor*>(m_pInternetConnectionMonitor.get())->Init();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pInternetConnectionMonitor == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bRet;
	}

	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pInternetConnectionMonitor = [%x]", m_pInternetConnectionMonitor.get());
		
		if (m_pInternetConnectionMonitor != NULL)
		{
			static_cast<CInternetConnectionMonitor*>(m_pInternetConnectionMonitor.get())->Finalize();
			m_pInternetConnectionMonitor.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }

	IUOLMessengerInternetConnectionMonitorPtr& m_pInternetConnectionMonitor;
};

struct CStringLoaderTask: public ITask
{
	CStringLoaderTask(UOLLib::ISettings*& pStringLoader, IUOLMessengerFacade3* pFacade2) :
		m_pStringLoader(pStringLoader),
		m_pUOLMessengerFacade2(pFacade2)
	{
		m_strTaskDescription = "String Loader Task";
	}

	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bOk = FALSE;

		CMinDependencySpecialFolder	minDSF;
		CString strCompany, strProduct, strXMLDir, strResXMLFile, strResXML;

		strCompany.LoadString(IDS_STRING_COMPANY_NAME);
		strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
		strXMLDir.LoadString(IDS_STRING_WIZARD_XML_DIR);

		IUOLMessengerCommonSettingsPtr pCommonSettings = m_pUOLMessengerFacade2->GetCommonSettings();

		if (pCommonSettings)
		{
			pCommonSettings->GetValue(IUOLMessengerCommonSettings::APP_STRING_RESOURCE_SETTING, strResXMLFile);
		}
		else
		{
			strResXMLFile.LoadString(IDS_STRING_WIZARD_XML_FILE);
		}

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pCommonSettings = [%x], strResXMLFile = [%s].", 
				pCommonSettings.get(), strResXMLFile);
		
		strResXML.Format("%s%s\\%s\\%s\\%s", minDSF.GetCommonAppData(), strCompany, strProduct, strXMLDir, strResXMLFile);
		m_pStringLoader = UOLLib::CSettingsFactory::AllocXmlSettings(strResXML);

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"strResXML = [%s].", strResXML);
		
		ATLASSERT(m_pStringLoader);
		if (m_pStringLoader)
		{
			bOk = m_pStringLoader->Load();
			ATLASSERT(bOk);

			// Initialize environment variables
			if (bOk)
			{
				CString strAppNameValue = m_pStringLoader->GetValue(_T("uol.resource.IDS_APP_NAME"));
				ATLASSERT(FALSE == strAppNameValue.IsEmpty());

				m_pStringLoader->SetVar(_T("APP_NAME"), strAppNameValue);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pStringLoader == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "bOk = [%d]", bOk);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return bOk;
	}

	virtual void UndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pStringLoader = [%x]", m_pStringLoader);
		
		if (m_pStringLoader)
		{
			UOLLib::CSettingsFactory::ReleaseXmlSettings(m_pStringLoader);
			m_pStringLoader = NULL;
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

    virtual void PreUndoTask() {}

	virtual BOOL GetRestartFlag() { return FALSE; }
	
	UOLLib::ISettings*&		m_pStringLoader;
	IUOLMessengerFacade3*	m_pUOLMessengerFacade2;
};

struct CEmoticonsManagerTask : public ITask
{
	CEmoticonsManagerTask()
	{
		m_strTaskDescription = "Emoticons Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		// TODO: Validar se este eh o melhor lugar pra inicializar o mapa de parsers de 
		//       emoticons. De qualquer forma isso nao pode ser feito durante o 
		//       carregamento dos settings. Talvez criar uma ultima TASK.
		CEmoticonParserManager::GetInstance()->PrepareTags();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Parser manager first initialization...");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return TRUE;
	}
	
    virtual void PreUndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		// TODO: Validar se este eh o melhor lugar pra finalizar o mapa de parsers de 
		//       emoticons. De qualquer forma isso nao pode ser feito durante o 
		//       carregamento dos settings. Talvez criar uma ultima TASK.
		CEmoticonParserManager::GetInstance()->UnregisterObserver();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}
	
	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, 
				"Doing nothing by now.");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}
	
	virtual BOOL GetRestartFlag()
	{
		return FALSE;
	}
};

struct CMessageTemplateManagerTask : public ITask
{
	CMessageTemplateManagerTask(IUOLMessengerMessageTemplateManagerPtr& pMessageTemplateManager) : 
			m_pMessageTemplateManager(pMessageTemplateManager)
	{
		m_strTaskDescription = "Message Template Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;
		
		m_pMessageTemplateManager = new CMessageTemplateManager();
		
		if (m_pMessageTemplateManager != NULL)
		{
			bRet = static_cast<CMessageTemplateManager*>(m_pMessageTemplateManager.get())->Init();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pMessageTemplateManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return bRet;
	}
	
    virtual void PreUndoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pMessageTemplateManager = [%x]", m_pMessageTemplateManager.get());
		
		if (m_pMessageTemplateManager != NULL)
		{
			static_cast<CMessageTemplateManager*>(m_pMessageTemplateManager.get())->Finalize();
			m_pMessageTemplateManager.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}
	
	virtual void UndoTask()
	{	
	}
	
	virtual BOOL GetRestartFlag()
	{
		return FALSE;
	}
	
	IUOLMessengerMessageTemplateManagerPtr&		m_pMessageTemplateManager;
};


struct CMethodAuthorizationManagerTask : public ITask
{
	CMethodAuthorizationManagerTask(CMethodAuthorizationManagerPtr& pMethodAuthorizationManager) : 
			m_pMethodAuthorizationManager(pMethodAuthorizationManager)
	{
		m_strTaskDescription = "Method Authorization Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;
		
		m_pMethodAuthorizationManager = new CMethodAuthorizationManager();
		
		if (m_pMethodAuthorizationManager != NULL)
		{
			bRet = m_pMethodAuthorizationManager->Initialize();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pMethodAuthorizationManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return bRet;
	}
	
    virtual void PreUndoTask()
	{
	}
	
	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pMethodAuthorizationManager = [%x]", m_pMethodAuthorizationManager.get());
		
		if (m_pMethodAuthorizationManager != NULL)
		{
			m_pMethodAuthorizationManager->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}
	
	virtual BOOL GetRestartFlag()
	{
		return FALSE;
	}
	
	CMethodAuthorizationManagerPtr&		m_pMethodAuthorizationManager;
};


struct CPluginPropertiesRegisterTask : public ITask
{
	CPluginPropertiesRegisterTask(CPluginPropertiesRegisterPtr& pPluginPropertiesRegister) : 
			m_pPluginPropertiesRegister(pPluginPropertiesRegister)
	{
		m_strTaskDescription = "Plugin Properties Register Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;
		
		m_pPluginPropertiesRegister = new CPluginPropertiesRegister();
		/*
		// Crash test on very beginning of application.
		//
		CAtlList<IUOLMessengerPluginPropertiesPtr> listPluginProperties;
		ap_dynamic_cast<IUOLMessengerPluginPropertiesRegisterPtr>(
				m_pPluginPropertiesRegister)->GetPluginPropertiesList(listPluginProperties);
		*/
		
		if (m_pPluginPropertiesRegister != NULL)
		{
			bRet = TRUE;
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pPluginPropertiesRegister == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return bRet;
	}
	
    virtual void PreUndoTask()
	{
	}
	
	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pPluginPropertiesRegister = [%x]", m_pPluginPropertiesRegister.get());
		
		if (m_pPluginPropertiesRegister != NULL)
		{
			m_pPluginPropertiesRegister.reset();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}
	
	virtual BOOL GetRestartFlag()
	{
		return FALSE;
	}
	
	CPluginPropertiesRegisterPtr&		m_pPluginPropertiesRegister;
};


struct CDiagnoseManagerTask : public ITask
{
	CDiagnoseManagerTask(CDiagnoseManagerPtr& pDiagnoseManager, BOOL bShowDiagnoseTool) : 
			m_pDiagnoseManager(pDiagnoseManager), 
			m_bShowDiagnoseTool(bShowDiagnoseTool)
	{
		m_strTaskDescription = "Diagnose Manager Task";
	}
	
	virtual BOOL DoTask()
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		BOOL bRet = FALSE;
		
		// Initialize Diagnose Manager.
		m_pDiagnoseManager = new CDiagnoseManager();
		
		if (m_pDiagnoseManager != NULL)
		{
			if ((bRet = m_pDiagnoseManager->Initialize()) == TRUE)
			{
				UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
				ATLASSERT(pStringLoader != NULL);
				
				// Add information to send BugReport
				CString strTitle, strCookieName, strCookieValue, strMethod, strBugUrl;
				
				strMethod = pStringLoader->GetValue("uol.resource.IDS_BUGREPORT_SERVLET_METHOD");
				strBugUrl = pStringLoader->GetValue("uol.resource.IDS_BUGREPORT_SERVLET_URL");
				
				m_pDiagnoseManager->SetCookie(strCookieName, strCookieValue);
				m_pDiagnoseManager->SetMethod(strMethod);
				m_pDiagnoseManager->SetUrl(strBugUrl);
				m_pDiagnoseManager->SetUserAgent(GetIEUserAgent());

				// Set Product info
				CString strName;
				CString strValue;
						
				strName = pStringLoader->GetValue("uol.resource.IDS_PRODID_NAME");
				strValue = pStringLoader->GetValue("uol.resource.IDS_PRODID_VALUE");
				m_pDiagnoseManager->SetProductInfo(strName, strValue);
				
				// Add download tests.
				CString strUrl;
				strUrl = pStringLoader->GetValue("uol.resource.IDS_URL_PLUGINS_LIST");
				m_pDiagnoseManager->AddDownloadTest(strUrl);

				strUrl = pStringLoader->GetValue("uol.resource.IDS_METHOD_AUTHORIZING_FILE");
				m_pDiagnoseManager->AddDownloadTest(strUrl);
				
				// Add download test - Get GUID.
				GUID guidInfo;
				CUIMApplication::GetApplication()->GetInstallCookie(guidInfo);
				CString strRegisterCode = CDataGUID::GetString(guidInfo);	
				CString strAux;
				strUrl = pStringLoader->GetValue("uol.resource.IDS_LIVEUPDATE_URL");
				strUrl += "?";
				CString strVersionParam = pStringLoader->GetValue("uol.resource.IDS_VERSION_PARAM");
				strUrl += (strAux.Format(strVersionParam, strRegisterCode), strAux);
				m_pDiagnoseManager->AddDownloadTest(strUrl);
				
				// Add file system tests.
				CMinDependencySpecialFolder	minDSF;
				CString strAppData = minDSF.GetAppData();
				ATLPath::Append(strAppData.GetBuffer(), "UOL\\UIM\\Profiles");
				strAppData.ReleaseBuffer();
				CString strCommonAppData = minDSF.GetCommonAppData();
				ATLPath::Append(strCommonAppData.GetBuffer(), "UOL\\UIM");
				strCommonAppData.ReleaseBuffer();
				
				m_pDiagnoseManager->AddReadFileTest(strAppData, "Profiles.xml");
				m_pDiagnoseManager->AddReadFileTest(strCommonAppData, "Sounds.xml");
				m_pDiagnoseManager->AddWriteFileTest(strAppData);
				m_pDiagnoseManager->AddWriteFileTest(strCommonAppData);
								
				// Add TCP tests.
				m_pDiagnoseManager->AddTCPConnectionTest("messenger.hotmail.com", 1863); //MSN
				m_pDiagnoseManager->AddTCPConnectionTest("login.messaging.aol.com", 5190); //ICQ
				m_pDiagnoseManager->AddTCPConnectionTest("scs.msg.yahoo.com", 5050); //Yahoo
				m_pDiagnoseManager->AddTCPConnectionTest("talk.google.com", 5222); //G-Talk
				m_pDiagnoseManager->AddTCPConnectionTest("xmpp.uol.com.br", 5222); //UOL
				
				// Show diagnose tool if user passed "/diagnose" parameter.
				if (m_bShowDiagnoseTool)
				{
					m_pDiagnoseManager->Show();	
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"CDiagnoseManager isn't present on ctrlcenter.dll");
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_pDiagnoseManager == NULL.");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"bRet = [%d]", bRet);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return TRUE;
	}
	
    virtual void PreUndoTask()
	{
	}
	
	virtual void UndoTask()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strTaskDescription = [%s].", m_strTaskDescription);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_pDiagnoseManager = [%x]", m_pDiagnoseManager.get());
		
		// Finalize Diagnose Manager.
		if (m_pDiagnoseManager != NULL)
		{
			m_pDiagnoseManager->Finalize();
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}
	
	virtual BOOL GetRestartFlag()
	{
		return FALSE;
	}
	
	CDiagnoseManagerPtr&		m_pDiagnoseManager;
	BOOL						m_bShowDiagnoseTool;
};


IUOLMessengerFacade3* CUIMApplication::ms_pUIMApplication;


CUIMApplication::~CUIMApplication(void)
{
}

IUOLMessengerFacade3* CUIMApplication::AllocApplication()
{
	if (NULL == ms_pUIMApplication)
	{
		ms_pUIMApplication = new CUIMApplication();
	}
	else
	{
		ATLASSERT(FALSE);		
	}

	return GetApplication();
}

void CUIMApplication::ReleaseApplication()
{
	if (ms_pUIMApplication)
	{
		delete ms_pUIMApplication;
		ms_pUIMApplication = NULL;
	}
}

IUOLMessengerFacade3* CUIMApplication::GetApplication()
{
	ATLASSERT(ms_pUIMApplication);
	ATLASSERT(FALSE == ::IsBadReadPtr(ms_pUIMApplication));

	return ms_pUIMApplication;
}

DWORD CUIMApplication::Init(CString strCmdLine)
{
	BOOL bShowDiagnoseTool = (-1 != strCmdLine.Find(STR_DIAGNOSE_TOOL_PARAM));	
	
	m_dwAppThreadId = ::GetCurrentThreadId();
	
	m_pMessengerPluginManager = new CUOLMessengerPluginManager();
	
	m_pLoggingManager = new CLoggingManager();
	
	if ( (m_pLoggingManager == NULL) || 
			(m_pLoggingManager->Init() == FALSE) )
	{
		return AIE_ERROR_FAILED;
	}
	
	// Initialize Progress Window
	m_pInitProgressWnd = new CUIMInitProgressWnd();
	
	if ((m_pInitProgressWnd == NULL) || 
		(m_pInitProgressWnd->Initialize() == FALSE))
	{
		return AIE_ERROR_FAILED;
	}
	
	g_UIMApplicationInitHelper.SetInitHelperCallback(m_pInitProgressWnd);
	
	g_UIMApplicationInitHelper.AddTask(new CPluginPropertiesRegisterTask(m_pPluginPropertiesRegister));
	g_UIMApplicationInitHelper.AddTask(new CMessageLoopTask(m_theLoop));
	g_UIMApplicationInitHelper.AddTask(new CCommonSettingsTask(m_pCommonSettings));
	g_UIMApplicationInitHelper.AddTask(new CStringLoaderTask(m_pStringLoader, this));
	g_UIMApplicationInitHelper.AddTask(new CDiagnoseManagerTask(m_pDiagnoseManager, bShowDiagnoseTool));
	g_UIMApplicationInitHelper.AddTask(new CMethodAuthorizationManagerTask(m_pMethodAuthorizationManager));	// depends CStringLoaderTask
	//g_UIMApplicationInitHelper.AddTask(new CThemeManagerTask(m_pThemeManager, m_pCommonSettings));
	g_UIMApplicationInitHelper.AddTask(new CInternetConnectionMonitorTask(m_pInternetConnectionMonitor));
 	g_UIMApplicationInitHelper.AddTask(new CAuthenticationManagerTask(m_pAuthenticationManager));
	g_UIMApplicationInitHelper.AddTask(new CUIPreInitTask(m_pUIManager, this));
	g_UIMApplicationInitHelper.AddTask(new CProfileManagerTask(m_pProfileManager, IUOLMessengerProfileManager::TS_INIT));
	g_UIMApplicationInitHelper.AddTask(new CSettingsManagerTask(m_pSettingsManager, m_pProfileManager));	
	g_UIMApplicationInitHelper.AddTask(new CContactListTask(m_pContactList));
	g_UIMApplicationInitHelper.AddTask(new CAccountPreInitTask(m_pAccountManager));
	g_UIMApplicationInitHelper.AddTask(new CProtocolTask(m_pProtocolManager)); 
	g_UIMApplicationInitHelper.AddTask(new CDiskCacheTask(m_pDiskCacheManager));
	g_UIMApplicationInitHelper.AddTask(new CMessageTemplateManagerTask(m_pMessageTemplateManager));  // Moved to here because it was causing crash when close application while receiving message
	g_UIMApplicationInitHelper.AddTask(new CUITask(m_pUIManager, this));	// depends on CProtocolTask, CDiskCacheTask
	g_UIMApplicationInitHelper.AddTask(new CUserIconCacheManagerTask(m_pUserIconCacheManager, m_pProfileManager));	// depends on CProfileManagerTask, CUITask 
	g_UIMApplicationInitHelper.AddTask(new CAccountInitTask(m_pAccountManager, this));	// depends on CUserIconCacheManagerTask
	g_UIMApplicationInitHelper.AddTask(new CThreadTask(m_pThreadJobs, this));
	g_UIMApplicationInitHelper.AddTask(new CTrayTask(m_pTrayWindow, this));
	g_UIMApplicationInitHelper.AddTask(new CNotifyPopUpTask(m_pNotifyPopUpManager));
	g_UIMApplicationInitHelper.AddTask(new CHistoryManagerTask(m_pHistory));
	g_UIMApplicationInitHelper.AddTask(new CCommandFactoryTask(m_pCommandFactory));
	g_UIMApplicationInitHelper.AddTask(new CFileTransferManagerTask(m_pFileTransferManager));
	g_UIMApplicationInitHelper.AddTask(new CConversationManagerTask(m_pConversationManager, this));
	g_UIMApplicationInitHelper.AddTask(new CPrivacyManagerTask(m_pPrivacyManager, this));	// depends on CAccountInitTask
	g_UIMApplicationInitHelper.AddTask(new CMessageReceivingFilterTask(m_pMessageReceivingFilter));
	g_UIMApplicationInitHelper.AddTask(new CAuthenticationTask(m_pAuthenticationManager));	// depends on CAuthenticationManagerTask
	g_UIMApplicationInitHelper.AddTask(new CAccountConnectionManagerTask(m_pAccountConnectionManager, this));	// depends on CAccountInitTask
	g_UIMApplicationInitHelper.AddTask(new CRemoteContactListTask(m_pRemoteContactList, this));		// depends on CAccountInitTask, CAccountConnectionManagerTask
	g_UIMApplicationInitHelper.AddTask(new CProtocolCallbackTask(m_protocolCallbackController, this));	// depends on CAccountInitTask, CProtocolTask
	g_UIMApplicationInitHelper.AddTask(new CProfileManagerTask(m_pProfileManager, IUOLMessengerProfileManager::TS_ACCOUNTS));
	g_UIMApplicationInitHelper.AddTask(new CPluginManagerTask(*m_pMessengerPluginManager.get()));
	g_UIMApplicationInitHelper.AddTask(new CNotifyPluginThreadTask(m_pThreadJobs, this));
	g_UIMApplicationInitHelper.AddTask(new CEmoticonsManagerTask());	
	
	m_pPropPageCommandFactory = new CPropertyPageCommandFactory();
	
	BOOL bInit = g_UIMApplicationInitHelper.Init();
	
	m_pInitProgressWnd->Finalize(!bInit);
	
	if (!bInit)
	{
		if (g_UIMApplicationInitHelper.GetRestartFlag())
		{
			return AIE_ERROR_RESTART;
		}

		return AIE_ERROR_FAILED;
	}
	
	return AIE_ERROR_SUCCESS;
}

void CUIMApplication::PreFinalize()
{
	// First of all we need to stop assync methods on libpurple->UIM direction.
	//
	//static_cast<CProtocolManager*>(m_pProtocolManager.get())->StopProtocolService();
	
	g_UIMApplicationInitHelper.PreFinalize();	
}

void CUIMApplication::Finalize()
{
	m_bKillReceived = TRUE;
	RemoveHook();
	
	g_UIMApplicationInitHelper.Finalize();
	
	m_pLoggingManager->Finalize();	
}

DWORD CUIMApplication::GetApplicationThreadId()
{
	return m_dwAppThreadId;
}

IUOLMessengerMessageTemplateManagerPtr CUIMApplication::GetMessageTemplateManager()
{
	return m_pMessageTemplateManager;
}

int CUIMApplication::Run()
{
	if (m_pUIManager)
	{
		m_pUIManager->CreateAppWindow(SW_SHOWDEFAULT);
		hWndMain = m_pUIManager->GetAppWindow().m_hWnd;
		if (m_pAccountManager->GetAccountListCount() == 0)
		{
            m_pUIManager->ShowPreferences();
		}

		return m_theLoop.Run();
	}

	return -1;
}


CUOLMessengerPluginManager* CUIMApplication::GetPluginManager()
{
	return m_pMessengerPluginManager.get();
}


CMessageQueueController* CUIMApplication::GetMessageQueueController()
{
	return &m_messageQueueController;
}


IDiskCacheManagerPtr CUIMApplication::GetDiskCacheManager()
{
	return m_pDiskCacheManager;
}

UOLLib::ISettings* CUIMApplication::GetStringLoader()
{
	return m_pStringLoader;
}


IUOLMessengerSettingsManagerPtr CUIMApplication::GetSettingsManager()
{
	return m_pSettingsManager;
}

IUOLMessengerHistoryManagerPtr CUIMApplication::GetHistoryManager()
{
	return m_pHistory;
}

IUOLMessengerCommandFactoryPtr CUIMApplication::GetCommandFactory()
{
	return m_pCommandFactory;
}

IUOLMessengerAccountManagerPtr	CUIMApplication::GetAccountManager()
{
	return m_pAccountManager;
}

IUOLMessengerProtocolManagerPtr	CUIMApplication::GetProtocolManager()
{
	return m_pProtocolManager;
}

IUOLMessengerUserIconCacheManagerPtr CUIMApplication::GetUserIconCacheManager()
{
	return m_pUserIconCacheManager;
}

IUOLMessengerUIManagerPtr CUIMApplication::GetUIManager()
{
	return m_pUIManager;
}

__declspec(deprecated) IUOLMessengerThemeManagerPtr CUIMApplication::GetThemeManager()
{
	ATLASSERT(FALSE);
	return IUOLMessengerThemeManagerPtr();
}

IUOLMessengerProfileManagerPtr CUIMApplication::GetProfileManager()
{
	return m_pProfileManager;
}

IUOLMessengerContactListPtr CUIMApplication::GetContactList()
{
	return m_pContactList;
}


IUOLMessengerAuthenticationManagerPtr CUIMApplication::GetAuthenticationManager()
{
	return m_pAuthenticationManager;
}

IUOLMessengerFileTransferManagerPtr CUIMApplication::GetFileTransferManager()
{
	return m_pFileTransferManager;
}

IUOLMessengerTrayWindowPtr CUIMApplication::GetTrayWindow()
{
	return m_pTrayWindow;
}

IUOLMessengerNotifyPopUpManagerPtr CUIMApplication::GetNotifyPopUpManager()
{
	return m_pNotifyPopUpManager;
}

IUOLMessengerMessageReceivingFilterPtr CUIMApplication::GetMessageReceivingFilter()
{
	return m_pMessageReceivingFilter;
}

IUOLMessengerInternetConnectionMonitorPtr CUIMApplication::GetInternetConnectionMonitor()
{
	return m_pInternetConnectionMonitor;
}

IUOLMessengerAccountConnectionManagerPtr CUIMApplication::GetAccountConnectionManager()
{
	return m_pAccountConnectionManager;
}

IUOLMessengerRemoteContactListPtr CUIMApplication::GetRemoteContactList()
{
	return m_pRemoteContactList;
}

IUOLMessengerPrivacyManagerPtr CUIMApplication::GetPrivacyManager()
{
	return m_pPrivacyManager;
}

IUOLMessengerConversationManagerPtr CUIMApplication::GetConversationManager()
{
	return m_pConversationManager;
}

BOOL CUIMApplication::AddMessageFilter(CMessageFilter* pMessageFilter)
{	
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	if (pLoop)
	{
		return pLoop->AddMessageFilter(pMessageFilter);
	}

	return FALSE;	
}

BOOL CUIMApplication::RemoveMessageFilter(CMessageFilter* pMessageFilter)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	if (pLoop)
	{
		return pLoop->RemoveMessageFilter(pMessageFilter);
	}

	return FALSE;
}

BOOL CUIMApplication::AddIdleHandler(CIdleHandler* pIdleHandler)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	if (pLoop)
	{
		return pLoop->AddIdleHandler(pIdleHandler);
	}

	return FALSE;
}

BOOL CUIMApplication::RemoveIdleHandler(CIdleHandler* pIdleHandler)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	if (pLoop)
	{
		return pLoop->RemoveIdleHandler(pIdleHandler);
	}

	return FALSE;
}


IUOLMessengerCommonSettingsPtr CUIMApplication::GetCommonSettings()
{
	return m_pCommonSettings;
}


IUOLMessengerPropertyPageCommandFactoryPtr CUIMApplication::GetPropertyPageCommandFactory()
{
	return m_pPropPageCommandFactory;
}


void CUIMApplication::ExecuteCommand(IUOLMessengerCommandPtr pCommand)
{
	ATLASSERT(pCommand.get());

	pCommand->Execute();
}


void CUIMApplication::GetInstallCookie(GUID& cookie) const
{
	CRegKey reg;
	CString strKey;

	strKey.LoadString(IDS_APP_REG_KEY);

	if (ERROR_SUCCESS ==  reg.Create(HKEY_LOCAL_MACHINE, strKey))
	{
		ULONG lSize = sizeof(cookie);

		if (ERROR_SUCCESS != reg.QueryBinaryValue("guid", &cookie, &lSize))
		{
			if (FAILED(::CoCreateGuid(&cookie)))
			{
				::ZeroMemory(&cookie, lSize);
			}			

			reg.SetBinaryValue("guid", reinterpret_cast<LPVOID>(&cookie), lSize);				
		}
	}
}


CString CUIMApplication::GetChatKey(const int nChatId, IUOLMessengerAccountPtr pAccount)
{
	CString strChatKey;
    
    if (pAccount)
    {
        strChatKey.Format(_T("%s:%d"), pAccount->GetUserId(), nChatId);
    }

    return strChatKey;
}


CUIMGlobalLoggerPtr	CUIMApplication::GetLogger()
{
	return ap_dynamic_cast<CUIMGlobalLoggerPtr>(m_pLoggingManager->GetGlobalLogger());
}


CPurpleLoggerPtr CUIMApplication::GetPurpleLogger()
{
	return ap_dynamic_cast<CPurpleLoggerPtr>(m_pLoggingManager->GetPurpleLogger());
}


IUOLMessengerDiagnoseManagerPtr CUIMApplication::GetDiagnoseManager()
{
	return m_pDiagnoseManager;
}


void CUIMApplication::SetProgressWindowVisible(BOOL bVisible)
{
	if ((m_pInitProgressWnd) && (m_pInitProgressWnd->IsWindow()))
	{
		m_pInitProgressWnd->ShowWindow(bVisible ? SW_SHOW : SW_HIDE);
	}
}


CMethodAuthorizationManagerPtr CUIMApplication::GetMethodAuthorizationManager()
{
	return m_pMethodAuthorizationManager;
}


void CUIMApplication::ShakeWindow(HWND hWnd, int nRepetitions, DWORD dwDelay)
{
	// Define moves array
	int moveX[5] = {-3, 6, -6, 6, -3};
	int moveY[5] = {-3, 0, 6, 0, -3};
	int idx = 0;
	
	// Save original window position
	WINDOWPLACEMENT OriginalWP;
    OriginalWP.length = sizeof(OriginalWP);
	::GetWindowPlacement(hWnd, &OriginalWP);

    for (int i = 0; i < nRepetitions; i++)
    {
		// Get current window position
        WINDOWPLACEMENT pwp;
        pwp.length = sizeof(pwp);
		::GetWindowPlacement(hWnd, &pwp);

        // Move the window
        pwp.rcNormalPosition.left += moveX[idx];
        pwp.rcNormalPosition.right += moveX[idx];
        pwp.rcNormalPosition.top += moveY[idx];
        pwp.rcNormalPosition.bottom += moveY[idx];

		if (idx < 4)
		{
			idx++;
		}
		else
		{
			idx = 0;
		}

		::SetWindowPlacement(hWnd, &pwp);
        Sleep(dwDelay);
    }

	// Restore original window position
	::SetWindowPlacement(hWnd, &OriginalWP);
}


void CUIMApplication::PlayNudgeSound(IUOLMessengerContactPtr pContact)
{
	// Play sound...
	BOOL bEnableSound = GetSettingsManager()->GetGeneralSettings()->IsEnableSound();
	BOOL bIsMuteWhileAwayEnabled = GetSettingsManager()->GetGeneralSettings()->IsMuteWhileAwayEnabled();
	DWORD dwAccountStatus = GetAccountConnectionManager()->GetStatus(pContact->GetAccount());
	BOOL bAwayStatus = (dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AWAY);
	
	if (!bEnableSound || (bAwayStatus &&  bIsMuteWhileAwayEnabled))
	{
		// Do not play if mute when away
		return;
	}
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(GetSettingsManager());
	if (pSettingsManager2)
	{
		CString strSoundPath;
		
		if (pSettingsManager2->GetSoundSettings()->GetSoundEventFilePath(IUOLMessengerSoundSettings3::NUDGE_RECEIVED_SOUND_EVENT, strSoundPath, TRUE))
		{
			GetUIManager()->PlaySound(strSoundPath, FALSE);
		}
	}
}


IUOLMessengerLoggerPtr CUIMApplication::GetApplicationLogger()
{
	return m_pLoggingManager->GetGlobalLogger();
}


IUOLMessengerPluginPropertiesRegisterPtr CUIMApplication::GetPluginPropertiesRegister()
{
	return m_pPluginPropertiesRegister;
}


IUOLMessengerLoggingManagerPtr CUIMApplication::GetLoggingManager()
{
	return m_pLoggingManager;
}




CUIMApplication::CUIMApplication():
m_bKillReceived(FALSE),
m_bLiveUpdateAvailable(FALSE),
m_dwAppThreadId(0), 
m_pStringLoader(NULL)
{
}

