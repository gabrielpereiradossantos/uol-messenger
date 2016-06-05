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

#include "stdafx.h"
#include "resource.h"
#include "FacadeWrapper.h"

#include "ContactListMonitor.h"
#include "MessageWindowMonitor.h"
#include "HistoryRecordVoip.h"
#include "core/UOLFoneController.h"
#include <interfaces/IUOLMessengerCommand.h>
#include <interfaces/IUOLMessengerProfileElement.h>
#include <interfaces/IUOLMessengerSettingsManager.h>
#include "core/UOLFoneRatesWindowFrame.h"

#include "HttpDownloader.h"
#include "VerifyDllsThread.h"

#include <MinDependencySpecialFolder.h>

using namespace std;
using namespace MSXML2;


//HHOOK		CFacadeWrapper::m_messagesHook = NULL;
//CWindow*	CFacadeWrapper::m_keyboardWindow = NULL;
//DWORD		CFacadeWrapper::m_lastKeyTime = 0;


const char* CFacadeWrapper::VOIP_LOGGER_MODULE_NAME = "UOLFoneClient";

const char* CFacadeWrapper::PLUGIN_LOGGER_MODULE_NAME = "UOLFonePlugin";

const IUOLMessengerLogger::LogLevelType CFacadeWrapper::VOIP_LOGGER_LOG_LEVEL = 
		IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_DEBUG;

const IUOLMessengerLogger::LogLevelType CFacadeWrapper::VOIP_LOGGER_TO_REPORT_LEVEL = 
		IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING;

const IUOLMessengerLogger::LogLevelType CFacadeWrapper::PLUGIN_LOGGER_LOG_LEVEL = 
		IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_ERROR;

const IUOLMessengerLogger::LogLevelType CFacadeWrapper::PLUGIN_LOGGER_TO_REPORT_LEVEL = 
		IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_ERROR;

const int CFacadeWrapper::VOIP_LOGGER_PURGE_INTERVAL = 1;


CFacadeWrapper::CFacadeWrapper() : 
		m_pFacade(NULL),
		m_uolFoneWndPtr(NULL),
		m_childContainerPtr(NULL),
		m_callDuration(0),
		m_pVoIPCallImage(NULL),
		m_lastCallErrorCode(0),
		m_callInPlace(FALSE),
		m_callIncoming(FALSE),
		m_nInitializationTimer(0), 
		m_showingFirstTime(TRUE), 
		m_nTabButtonIndex(-1), 
		m_bInitializationHasAlreadyFailed(FALSE), 
		m_bInitWindowHasBeenShown(FALSE),
		m_bIsShowingError(FALSE),
		m_bMediaTimeoutError(FALSE)
{
	m_initStatusCode = IUOLFoneControllerCallback::InitStatusCodeLibraryInitializationError;
	m_bIsToolbarButtonVisible = FALSE;
}


CFacadeWrapper::~CFacadeWrapper()
{
	m_pFacade = NULL;
	m_uolFoneWndPtr = NULL;
	m_childContainerPtr = NULL;	
}


CFacadeWrapper* CFacadeWrapper::GetInstance()
{
	static CFacadeWrapper instance;

	return &instance;
}


IUOLMessengerFacade* CFacadeWrapper::GetFacadeInstance()
{
	return CFacadeWrapper::GetInstance()->GetFacade();
}


void CFacadeWrapper::Init(IUOLMessengerFacade* pFacade, CUOLMessengerPlugin* pUOLMessengerPlugin)
{
	m_pFacade = pFacade;
	m_pUOLMessengerPlugin = pUOLMessengerPlugin;
	m_nInitializationTimer = 0;
	m_lastCallErrorCode = 0;
		
	RegisterAsMainWindowEventsNotifier(this);
	LoadFoneInfo();
	
	// Build errors log file path.
	//
	IUOLMessengerProfileElementPtr pProfileElement;
	
	m_pFacade->GetProfileManager()->GetProfileElement(
			m_pFacade->GetProfileManager()->GetCurrentProfile(), pProfileElement);
	
	m_errorsLogFilePath.Format("%s\\UOLFonePluginErrors.log", pProfileElement->GetProfilePath());
}


void CFacadeWrapper::Finalize()
{
	m_pImagePC2PC = NULL;
	m_pImagePC2PCSmall = NULL;
	m_pImagePC2Phone = NULL;
	m_pImagePC2PhoneSmall = NULL;
	m_pFacade = NULL;
	
	UnregisterAsMainWindowEventsNotifier(this);
}


void CFacadeWrapper::PluginInit()
{
	HWND hWnd = Create(NULL, NULL, "UOLFacadeWrapper", WS_POPUP);
	ATLASSERT (hWnd != NULL);
	
	IUOLMessengerLoggingManagerPtr pLoggingManager = GetLoggingManager();
	IUOLMessengerLogger2Ptr pNewLogger;
	
	if (pLoggingManager != NULL)
	{
		CString strFilesCommonPath;
		CString strPluginIniFile;
		CString strPluginLogFile;
		CString strFoneClientIniFile;
		CString strFoneClientLogFile;
		
		GetCurrentUserProfilePath(strFilesCommonPath);
		
		// Register the UOLFonePlugin logger.
		//
		strPluginIniFile.Format(
				"%s\\UOLFonePluginErrors.ini", strFilesCommonPath);
		strPluginLogFile.Format(
				"%s\\UOLFonePluginErrors.log", strFilesCommonPath);
		
		pNewLogger = pLoggingManager->RegisterLogger(
				PLUGIN_LOGGER_MODULE_NAME, 
				strPluginIniFile, 
				strPluginLogFile, 
				PLUGIN_LOGGER_LOG_LEVEL, 
				CUOLFonePluginConfig::GetInstance()->GetPurgeInterval(), 
				TRUE);
		
		if (pNewLogger != NULL)
		{
			CString strPluginLogsTitle;
			strPluginLogsTitle.LoadString(IDS_PLUGIN_LOGS_TO_REPORT_TITLE);
			
			pLoggingManager->AddLogInfoToReports(
					PLUGIN_LOGGER_MODULE_NAME, 
					PLUGIN_LOGGER_TO_REPORT_LEVEL, 
					NULL, // We won't use this for now.
					strPluginLogsTitle);
		}
		
		// Register the UOLFoneClient logger.
		//
		strFoneClientIniFile.Format("%s\\%sLog.ini", strFilesCommonPath, 
				VOIP_LOGGER_MODULE_NAME);
		strFoneClientLogFile.Format("%s\\%sLog.log", strFilesCommonPath, 
				VOIP_LOGGER_MODULE_NAME);
		
		pNewLogger = pLoggingManager->RegisterLogger(
				VOIP_LOGGER_MODULE_NAME, 
				strFoneClientIniFile, 
				strFoneClientLogFile, 
				VOIP_LOGGER_LOG_LEVEL, 
				VOIP_LOGGER_PURGE_INTERVAL, 
				TRUE);
		
		if (pNewLogger != NULL)
		{
			CString strPluginLogsTitle;
			strPluginLogsTitle.LoadString(IDS_VOIP_LOGS_TO_REPORT_TITLE);
			
			pLoggingManager->AddLogInfoToReports(
					VOIP_LOGGER_MODULE_NAME, 
					VOIP_LOGGER_TO_REPORT_LEVEL, 
					NULL, // We won't use this for now.
					strPluginLogsTitle);
		}
	}

	// ------------------------------------------------------------------------
	// Diagnose Tool [BEGIN BLOCK]
	// ------------------------------------------------------------------------
	// Create DiagnoseManager instance and add tests
	IUOLMessengerDiagnoseManagerPtr pDiagnoseManager = GetDiagnoseManager();
	if ((pDiagnoseManager != NULL) && pDiagnoseManager->IsDiagnoseManagerPresent())
	{
		HRESULT hr = CComObject<CTestSIP>::CreateInstance(&m_pTestSIP);
		if (SUCCEEDED(hr))
		{
			m_pTestSIP->AddRef();
			pDiagnoseManager->AddMethodTest(m_pTestSIP);

			CString strDllsUrl;
			strDllsUrl.LoadString(IDS_URL_RELEASE_DLLS);

			CString strUrl;
			strUrl.LoadString(IDS_URL_PHONE_INFO);
			pDiagnoseManager->AddDownloadTest(strUrl);

			strUrl.LoadString(IDS_URL_PHONE_VALIDATION_RULES);
			pDiagnoseManager->AddDownloadTest(strUrl);

			strUrl.LoadString(IDS_URL_CODECS_LIST);
			pDiagnoseManager->AddDownloadTest(strUrl);	

			strUrl = strDllsUrl + CUOLFoneController::GetInstance()->GetUOLFoneClientDllName();
			pDiagnoseManager->AddDownloadTest(strUrl);	
			
			strUrl = strDllsUrl + CUOLFoneController::GetInstance()->GetUOLFoneCryptDllName();
			pDiagnoseManager->AddDownloadTest(strUrl);	
			
			strUrl = strDllsUrl + CUOLFoneController::GetInstance()->GetUOLFoneServerConfigProviderDllName();
			pDiagnoseManager->AddDownloadTest(strUrl);	

			CMinDependencySpecialFolder	minDSF;
			CString strPluginsPath = minDSF.GetCommonAppData();
			ATLPath::Append(strPluginsPath.GetBuffer(), "UOL\\lib\\plugins");
			strPluginsPath.ReleaseBuffer();

			pDiagnoseManager->AddReadFileTest(strPluginsPath, "UOLFoneCodecs.xml");
			pDiagnoseManager->AddWriteFileTest(strPluginsPath);
		}
	}
	// ------------------------------------------------------------------------
	// Diagnose Tool [END BLOCK]
	// ------------------------------------------------------------------------
}


void CFacadeWrapper::PluginFinalize()
{
	// Remove and release method test
	IUOLMessengerDiagnoseManagerPtr pDiagnoseManager = GetDiagnoseManager();
	if ((pDiagnoseManager != NULL) && pDiagnoseManager->IsDiagnoseManagerPresent())
	{
		pDiagnoseManager->RemoveMethodTest(m_pTestSIP);
		m_pTestSIP->Release();
	}

	// Unregister the UOLFoneClient logger.
	//
	IUOLMessengerLoggingManagerPtr pLoggingManager = GetLoggingManager();
	
	if (pLoggingManager != NULL)
	{
		// Unregister the UOLFonePlugin logger.
		//
		pLoggingManager->RemoveLogInfoFromReports(PLUGIN_LOGGER_MODULE_NAME);
		pLoggingManager->UnregisterLogger(PLUGIN_LOGGER_MODULE_NAME);
		
		// Unregister the UOLFoneClient logger.
		//
		pLoggingManager->RemoveLogInfoFromReports(VOIP_LOGGER_MODULE_NAME);
		pLoggingManager->UnregisterLogger(VOIP_LOGGER_MODULE_NAME);
	}
	
	if (IsWindow())
	{
		DestroyWindow();
	}
	
	if (m_pFrameWindow!=NULL)
	{
		if (m_pFrameWindow->GetHWND())
		{
			CWindow wndFrameWindow = m_pFrameWindow->GetHWND();
			wndFrameWindow.DestroyWindow();
		}
	}

	if (m_pView!=NULL)
	{
		if (m_pView->IsWindow())
		{
			m_pView->DestroyWindow();
		}
	}

	m_bInitWindowHasBeenShown = FALSE;
	m_bInitializationHasAlreadyFailed = FALSE;
}

void CFacadeWrapper::DestroyRateWindow()
{
	if (m_pFrameWindow!=NULL)
	{
		if (m_pFrameWindow->GetHWND())
		{
			CWindow wndFrameWindow = m_pFrameWindow->GetHWND();
			wndFrameWindow.DestroyWindow();
		}
	}

	if (m_pView!=NULL)
	{
		if (m_pView->IsWindow())
		{
			m_pView->DestroyWindow();
		}
	}
}
IUOLMessengerFacade* CFacadeWrapper::GetFacade()
{
	return m_pFacade;
}


int CFacadeWrapper::AddTabButton()
{
	if (!m_bIsToolbarButtonVisible)
	{
		
		IUOLMessengerUIManager3Ptr pUIManager3 = GetUIManager3();
		
		if (pUIManager3)
		{
			// Temos de pegar a TabView criada na UIMView.
			IUOLMessengerTabViewCtrlPtr pTabViewCtrl = pUIManager3->GetAppTabViewCtrl();
			
			if (pTabViewCtrl)
			{
				m_nTabButtonIndex = 
						pTabViewCtrl->CreateTabButton(m_uolFoneWndPtr->GetTabName());
				
				pTabViewCtrl->SetTabButtonImage(m_nTabButtonIndex, 
						_AtlBaseModule.GetModuleInstance(), IDB_UOLFONE_TAB_ICON);
				
				m_bIsToolbarButtonVisible = TRUE;
				
				/*
				m_pUOLFoneTabButton = pImageButtonContainer->CreateExternalButton();
				
				m_pUOLFoneTabButtonImage = pUIManager2->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_UOLFONE_TAB_BUTTON);
				
				if ( (m_pUOLFoneTabButton) && (m_pUOLFoneTabButtonImage) )
				{
					CString strTooltipText;
					strTooltipText.LoadString(IDS_PLUGIN_NAME);
					
					m_pUOLFoneTabButton->SetTip(strTooltipText);
					m_pUOLFoneTabButton->SetImage(m_pUOLFoneTabButtonImage);
					m_pUOLFoneTabButton->SetWidth(89);
					//m_pUOLFoneTabButton->SetPushButton(TRUE);
					m_pUOLFoneTabButton->SetAuto(FALSE);
					m_pUOLFoneTabButton->SetDrawTransparent(TRUE);
					//m_pUOLFoneTabButton->SetPressed(FALSE);
					m_pUOLFoneTabButton->SetCommandName(m_uolFoneWndPtr->GetTabName());
					
					pImageButtonContainer->AddExternalButton(m_pUOLFoneTabButton);
					
					m_pUOLFoneTabButton->AddEventListener(this);

					m_bIsToolbarButtonVisible = TRUE;
				}
				*/
			}
		}
	}
	
	return m_nTabButtonIndex;
}


void CFacadeWrapper::RemoveTabButton()
{
	IUOLMessengerUIManager3Ptr pUIManager3 = GetUIManager3();
	
	if (pUIManager3) {
		
		IUOLMessengerTabViewCtrlPtr pTabViewCtrl = pUIManager3->GetAppTabViewCtrl();
		
		if (pTabViewCtrl)
		{
			pTabViewCtrl->RemoveTabButton(m_nTabButtonIndex);
			
			/*
			m_pUOLFoneTabButton->RemoveEventListener(this);
			
			pImageButtonContainer->RemoveExternalButton(m_pUOLFoneTabButton);
			*/
			
			m_bIsToolbarButtonVisible = FALSE;
		}
	}
}


void CFacadeWrapper::OnTimer(UINT nTimerId, TIMERPROC timerproc)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_initTimerLock);

	if ( nTimerId != m_nInitializationTimer ||
		 NULL == m_nInitializationTimer )
	{
		return;
	}

	if ((NULL == m_uolFoneWndPtr) || (FALSE == m_uolFoneWndPtr->IsWindow()))
	{
		return;
	}

	KillTimer(m_nInitializationTimer);
	m_nInitializationTimer = 0;

	CString strTitle;
	CString strText;

	strTitle.LoadString(IDS_PLUGIN_NAME);

	if (m_initStatusCode == IUOLFoneControllerCallback::InitStatusCodeSucess)
	{
		// Testing code
		//ShowKeyboardWindow();
		// Testing code
		
		DWORD dwTextResourceId = -1;

		CUOLFoneAccountManagerPtr pAccountManager = CUOLFoneController::GetInstance()->GetUOLFoneAccountManager();

		if (pAccountManager)
		{
			CUOLFoneAccountPtr pAccount = pAccountManager->GetCurrentAccount();
			
			if (pAccount)
			{
				// Show information window requesting account connection.
				//ShowUOLAccountDisconnected();
				ShowHomeWindow();
			}
			else
			{
				ShowUOLAccountNotFound();
			}
		}
		else
		{
			dwTextResourceId = IDS_ERROR_LIBRARY_INITIALIZATION_FAILED;
		}

		if (dwTextResourceId != -1)
		{
			strText.LoadString(dwTextResourceId);

			CategorizeAndShow(strTitle, strText, this, ACTION_CALLBACK_RESTART_APP);
		}
	}
	else
	{
		// Testing code
		//ShowKeyboardWindow();
		// Testing code
		
		if (m_initStatusCode == IUOLFoneControllerCallback::InitStatusCodeServerConnectionError)
		{
			strText.LoadString(IDS_ERROR_NOT_POSSIBLE_TO_CONNECT);
			
			LogFailureInfo(strText);
		}
		else if (m_initStatusCode == IUOLFoneControllerCallback::InitStatusCodeLibraryDownloadError)
		{
			if (m_pLastFailureInfo)
			{
				if ( (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_DOWNLOAD) && 
						(m_pLastFailureInfo->GetExtraFailureInfoDWORD(
								CUOLFoneFailureInfo::EXTRA_INFO_HTTP_STATUS_CODE) == 404) )
				{
					strText.LoadString(IDS_PLUGIN_FILES_NOT_FOUND_ON_SERVER);
				}
				else if ( (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_DOWNLOAD) && 
						(m_pLastFailureInfo->GetExtraFailureInfoDWORD(
								CUOLFoneFailureInfo::EXTRA_INFO_HTTP_STATUS_CODE) == 407) )
				{
					strText.LoadString(IDS_PROXY_AUTHENTICATION_REQUIRED);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_DOWNLOAD)
				{
					strText.LoadString(IDS_PLUGIN_FILES_DOWNLOAD_ERROR);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_SYSTEM)
				{
					strText.LoadString(IDS_FOLDER_PATH_CREATION_FAILURE);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_INITIALIZATION)
				{
					strText.LoadString(IDS_PLUGIN_INITIALIZATION_FAILURE);
				}
			}
			
			if (strText.IsEmpty())
			{
				strText.LoadString(IDS_ERROR_PLUGIN_NOT_INITIALIZED);
			}
			
			LogFailureInfo(strText);
		}
		else if (m_initStatusCode == IUOLFoneControllerCallback::InitStatusCodeLibraryInitializationError)
		{
			if (m_pLastFailureInfo)
			{
				if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_LIBRARY)
				{
					if (m_pLastFailureInfo->GetErrorCode() == CUOLFoneFailureInfo::LIBRARY_SET_OUTPUT_AUDIO_DEVICE_FAILURE)
					{	
						CString strLineDevice = CUOLFonePluginConfig::GetInstance()->GetLineOutDevice();
						CString strErrMsg; 
						strErrMsg.LoadString(IDS_AUDIO_DEVICE_INITIALIZATION_FAILURE);

						strText.Format(strErrMsg, strLineDevice);
					}
					else if (m_pLastFailureInfo->GetErrorCode() == CUOLFoneFailureInfo::LIBRARY_SET_INPUT_AUDIO_DEVICE_FAILURE)
					{	
						CString strLineDevice = CUOLFonePluginConfig::GetInstance()->GetLineInDevice();
						CString strErrMsg; 
						strErrMsg.LoadString(IDS_AUDIO_DEVICE_INITIALIZATION_FAILURE);

						strText.Format(strErrMsg, strLineDevice);
					}
					else
					{
						strText.LoadString(IDS_PLUGIN_LIBRARY_INITIALIZATION_FAILURE);
					}
				}
			}
			
			if (strText.IsEmpty())
			{
				strText.LoadString(IDS_ERROR_PLUGIN_NOT_INITIALIZED);
			}
			
			LogFailureInfo(strText);
		}
		else if (m_initStatusCode == IUOLFoneControllerCallback::InitStatusCodeCodecsListDownloadError)
		{
			if (m_pLastFailureInfo)
			{
				if ( (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_DOWNLOAD) && 
						(m_pLastFailureInfo->GetExtraFailureInfoDWORD(
								CUOLFoneFailureInfo::EXTRA_INFO_HTTP_STATUS_CODE) == 404) )
				{
					strText.LoadString(IDS_PLUGIN_FILES_NOT_FOUND_ON_SERVER);
				}
				else if ( (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_DOWNLOAD) && 
						(m_pLastFailureInfo->GetExtraFailureInfoDWORD(
								CUOLFoneFailureInfo::EXTRA_INFO_HTTP_STATUS_CODE) == 407) )
				{
					strText.LoadString(IDS_PROXY_AUTHENTICATION_REQUIRED);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_DOWNLOAD)
				{
					strText.LoadString(IDS_PLUGIN_FILES_DOWNLOAD_ERROR);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_SYSTEM)
				{
					strText.LoadString(IDS_FOLDER_PATH_CREATION_FAILURE);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_INITIALIZATION)
				{
					strText.LoadString(IDS_PLUGIN_INITIALIZATION_FAILURE);
				}
				else if (m_pLastFailureInfo->GetFailureType() == CUOLFoneFailureInfo::FAILURE_TYPE_LIBRARY)
				{
					strText.LoadString(IDS_PLUGIN_LIBRARY_INITIALIZATION_FAILURE);
				}
			}
			
			if (strText.IsEmpty())
			{
				strText.LoadString(IDS_ERROR_DOWNLOAD_CODECS);
			}
			
			LogFailureInfo(strText);
		}
		else
		{
			strText.LoadString(IDS_ERROR_UNKNOW);
			
			LogFailureInfo(strText);
		}
		
		CategorizeAndShow(strTitle, strText, NULL);
	}
}


/*
void CFacadeWrapper::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	HideContactListTab();
}
*/


IUOLMessengerUIManager2Ptr CFacadeWrapper::GetUIManager2()
{
	IUOLMessengerFacade* pUOLMessengerFacade = GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		return ap_static_cast<IUOLMessengerUIManager2Ptr>(pUOLMessengerFacade->GetUIManager());
	}
	else
	{
		return NULL;
	}
}


IUOLMessengerUIManager3Ptr CFacadeWrapper::GetUIManager3()
{
	IUOLMessengerFacade* pUOLMessengerFacade = GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		return ap_static_cast<IUOLMessengerUIManager3Ptr>(pUOLMessengerFacade->GetUIManager());
	}
	else
	{
		return NULL;
	}
}
IUOLMessengerUIManager4Ptr CFacadeWrapper::GetUIManager4()
{
	IUOLMessengerFacade* pUOLMessengerFacade = GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		return ap_static_cast<IUOLMessengerUIManager4Ptr>(pUOLMessengerFacade->GetUIManager());
	}
	else
	{
		return NULL;
	}
}

IUOLMessengerUIManager5Ptr CFacadeWrapper::GetUIManager5()
{
	IUOLMessengerFacade* pUOLMessengerFacade = GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		return ap_static_cast<IUOLMessengerUIManager5Ptr>(pUOLMessengerFacade->GetUIManager());
	}
	else
	{
		return NULL;
	}
}

void CFacadeWrapper::SetUOLFoneWindowPtr(CUOLFoneWindow* uolFoneWndPtr)
{
	m_uolFoneWndPtr = uolFoneWndPtr;
}


void CFacadeWrapper::SetChildContainerPtr(
		CUOLFoneChildWindowContainerView* childContainerPtr)
{
	m_childContainerPtr = childContainerPtr;
}


void CFacadeWrapper::ShowHomeWindow()
{
	CUOLFoneAccountPtr pUOLFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();

	if (!m_bInitWindowHasBeenShown)
	{
		ShowInitializingWindow();
		
		m_bInitWindowHasBeenShown = TRUE;
	}
	else if ( (IsBOLUser(CUOLFoneController::GetInstance()->GetCurrentAccount())) && (!m_bIsShowingError) )
	{
		ShowBOLAccountWarning();
	}
	/*removed until create a better message*/
	/*else if (pUOLFoneAccount && 
		 	 (FALSE == pUOLFoneAccount->HasInscription()))
	{
		ShowUOLFoneSubscriberWarning();
	}*/
	else if (!m_bIsShowingError)
	{
		ShowKeyboardWindow();
	}
}


void CFacadeWrapper::ShowSACButton()
{
	if (m_uolFoneWndPtr != NULL)
	{
		CUOLFoneAccountPtr pUOLFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
		m_uolFoneWndPtr->ShowSACButton(!IsBOLUser(pUOLFoneAccount));
	}
}


BOOL CFacadeWrapper::StartCallToPC(IUOLMessengerContactPtr pContact, BOOL bIsContactAddressTyped)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);
	
	BOOL success = FALSE;
	
	if (m_callInPlace)
	{
		if (pContact->GetAlias().CompareNoCase( m_voIPCallText ) == 0)
		{
			HangUpPhoneCall();
			OnStopCall();

			success = TRUE;
		}
		else
		{
			m_lastCallErrorDescription.LoadString(
					IDS_ERROR_VOIP_CALL_ESTABLISHED);			
		}
	}
	else
	{
		if (pContact->GetUserIcon())
		{
			SetVoIPCallInfo(pContact->GetAlias(), pContact->GetUserIcon()->GetImage());	
		}
		else
		{
			SetVoIPCallInfo(pContact->GetAlias(), GetPC2PCDefaultImage());	
		}

		success = CallPC(pContact);
		
		if ( (TRUE == success) && (TRUE == bIsContactAddressTyped) )
		{					
			CUOLFonePluginConfig::GetInstance()->
				AddCallNumberToHistory(pContact->GetName());
		}
	}
	
	return success;
}


BOOL CFacadeWrapper::StartCallToPhone()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);
	
	BOOL success = FALSE;
	BOOL restart = FALSE;
	
	if (m_callInPlace)
	{
		m_lastCallErrorDescription.LoadString(
				IDS_ERROR_VOIP_CALL_ESTABLISHED);		
	}
	else
	{		
		CWindow* keyboardWindowPtr = m_childContainerPtr->GetChildWindowPtr(
				CUOLFoneChildWindowContainerView::UOLFONE_KEYBOARD_WINDOW);
		
		if (keyboardWindowPtr == NULL)
		{
			m_lastCallErrorDescription.LoadString(
					IDS_ERROR_UNABLE_RETRIEVE_CHILD_WINDOW);
			restart = TRUE;
		}
		else
		{
			CString phoneNumberDigited, phoneNumberToCall;
			((CUOLFoneKeyboardWindow*)keyboardWindowPtr)->
					GetPhoneNumberToCall(phoneNumberDigited);

			
			if ( phoneNumberDigited.GetLength() == 0 )
			{
				m_lastCallErrorDescription.LoadString(IDS_ERROR_INVALID_PHONE_NUMBER);
			}
			else
			{
				SetVoIPCallInfo(phoneNumberDigited, IsUOLMessengerAppMode() ? IDB_PC2PHONE_IMAGE : IDB_PC2PHONE_IMAGE_ALT);
				
				phoneNumberToCall = phoneNumberDigited;
				FormatPhoneToCall(phoneNumberToCall);

				success = CallPhone(phoneNumberToCall);
				
				if ( TRUE == success)
				{					
					CUOLFonePluginConfig::GetInstance()->
						AddCallNumberToHistory(phoneNumberDigited);
				}
			}
		}
	}

	if ( FALSE == success )
	{
		if (restart)
		{
			CategorizeAndShow(NULL, m_lastCallErrorDescription, this, ACTION_CALLBACK_RESTART_APP);
		}
		else
		{
			CategorizeAndShow(NULL, m_lastCallErrorDescription, NULL);
		}
	}
	
	return success;
}
BOOL CFacadeWrapper::CallSAC()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);
	
	BOOL success = FALSE;
	BOOL restart = FALSE;	

	if (m_callInPlace)
	{
		m_lastCallErrorDescription.LoadString(
				IDS_ERROR_VOIP_CALL_ESTABLISHED);		
	}
	else
	{	
		
		CString strSAC;
		strSAC.LoadString(IDS_LABEL_SAC);

		SetVoIPCallInfo(strSAC, IsUOLMessengerAppMode() ? IDB_PC2PHONE_IMAGE : IDB_PC2PHONE_IMAGE_ALT);

		CString phoneNumberToCall = GetSACnumber(); 
		//phoneNumberToCall.LoadString();
		success = CallPhone(phoneNumberToCall);
	}

	if ( FALSE == success )
	{
		//if (restart)
		//{
		//	ShowUOLAccountDisconnected();
		//}
		//else
		//{
		//	
		//}
		ShowUOLAccountDisconnected();
	}
	return success;
}

BOOL CFacadeWrapper::AcceptCall()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);

	CUOLFoneController* pFoneController = CUOLFoneController::GetInstance();
	
	CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
	IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();
	
	BOOL success = FALSE;

	CString message;
	if (m_callInPlace)
	{
		m_lastCallErrorDescription.LoadString(
				IDS_ERROR_VOIP_CALL_ESTABLISHED);		
	}
	else
	{		
		success = CUOLFoneController::GetInstance()->AcceptIncomingCall();

		if (success)
		{
			m_callInPlace = TRUE;

			ShowCallInPlaceWindow();
		}
		else
		{
			m_lastCallErrorDescription.LoadString(IDS_ERROR_CALL_TO_PC_FAILED);
		}
	}

	if ( FALSE == success )
	{
		CategorizeAndShow(NULL, m_lastCallErrorDescription, NULL);
	}

	CString strType;
	strType.LoadString(IDS_HISTORY_CALL_STATUS_RECEIVED);
	IUOLMessengerContactPtr pContact = FindContact(pUIMAccount, m_contactAccount);

	if (pContact!= NULL)
	{
		CString strStatus;
		strStatus.LoadString(IDS_HISTORY_CALL_PC2PC);

		CString strText;
		strText.LoadString(IDS_HISTORY_CALL_MESSAGE_RECEIVED);

		CHistoryRecordVoipPtr pHistoryRecordVoip;
		pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, pContact->GetName(), pUIMAccount->GetUserId(), strStatus, strType, strText);

		IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
		ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
		pHistoryManager4->LogEvent(pHistoryRecordVoip);
	}
	else
	{
		CString strStatus;
		strStatus.LoadString(IDS_HISTORY_CALL_Phone2PC);

		CString strText;
		strText.LoadString(IDS_HISTORY_CALL_MESSAGE_RECEIVED);

		CHistoryRecordVoipPtr pHistoryRecordVoip;
		pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, m_voIPCallText, pUIMAccount->GetUserId(), strStatus, strType, strText);

		IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
		ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
		pHistoryManager4->LogEvent(pHistoryRecordVoip);
	}
	
	m_bCallAlreadyLogged = TRUE;

	return success;
}


void CFacadeWrapper::OnStopCall()
{
	if (m_bMediaTimeoutError)
	{
		return;
	}
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);
	m_callInPlace = FALSE;
	m_bCallAlreadyLogged = FALSE;
	//TODO: trocar o icone do botao de ligacoes perdidas se necessario
	BOOL iscalltopc = FALSE;
	if (NULL != m_pCurContact )
	{
		iscalltopc = TRUE;
	}

	m_voIPCallText.Empty();
	m_pVoIPCallImage = NULL;

	m_pCurContact = NULL;
	m_strCurPhoneNumber.Empty();

	PlayCallClosedSound();

	if (iscalltopc )
	{
		IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();

		if (pUIManager2)
		{
			IUOLMessengerContactListTabPtr pContactListTab = pUIManager2->GetAppWindowContactListTab();

			if (pContactListTab)
			{
				//this->ShowWindow(SW_HIDE);
				pContactListTab->ShowContactListTab();
			}
		}
	}

	m_bIsShowingError = FALSE;
	ShowHomeWindow();
}


void CFacadeWrapper::HangUpPhoneCall()
{
	CUOLFoneController::GetInstance()->HangUpPhoneCall();
}


void CFacadeWrapper::Redial()
{
	if ( NULL != m_pCurContact )
	{
		CallPC( m_pCurContact );
	}
	else if ( m_strCurPhoneNumber.GetLength() > 0 )
	{
		CallPhone( m_strCurPhoneNumber );
	}
	else
	{
		ATLASSERT(0);
	}
}


void CFacadeWrapper::TransferPhoneCall(CString& strPhoneNumber)
{
	CUOLFoneController::GetInstance()->TransferPhoneCall(strPhoneNumber);
}


void CFacadeWrapper::HoldPhoneCall()
{
	CUOLFoneController::GetInstance()->HoldPhoneCall();
}


void CFacadeWrapper::ResumePhoneCall()
{
	CUOLFoneController::GetInstance()->ResumePhoneCall();
}


void CFacadeWrapper::StartConferenceCall(CString& strPhoneNumber01, CString& strPhoneNumber02)
{
	CUOLFoneController::GetInstance()->MakeConferenceCall(strPhoneNumber01, strPhoneNumber02);
}


void CFacadeWrapper::HangUpConferenceCall()
{
	CUOLFoneController::GetInstance()->HangUpConferenceCall();
}


LPCTSTR CFacadeWrapper::GetVoIPCallText()
{
#ifdef _TAB_DEMO_MODE
	if ( m_voIPCallText.GetLength() == 0 )
	{
		return _T("1122223344");
	}
#endif

	return m_voIPCallText;
}


IUOLMessengerImagePtr CFacadeWrapper::GetVoIPCallImage()
{
#ifdef _TAB_DEMO_MODE
	if ( NULL == m_pVoIPCallImage )
	{
		return GetPC2PhoneDefaultImage();
	}
#endif
	
	return m_pVoIPCallImage;
}


void CFacadeWrapper::SetVoIPCallInfo(const CString& voIPCallText, IUOLMessengerImagePtr pVoIPCallImage)
{
	m_voIPCallText = voIPCallText;
	m_pVoIPCallImage = pVoIPCallImage;
}


void CFacadeWrapper::SetVoIPCallInfo(const CString& voIPCallText, UINT nResId)
{
	m_voIPCallText = voIPCallText;
	m_pVoIPCallImage = LoadImageFromResource(nResId);
}

void CFacadeWrapper::SetVoIPCallInfo(const CString& voIPCallText, IUOLMessengerImagePtr pVoIPCallImage, const CString& contactAccount)
{
	m_voIPCallText = voIPCallText;
	m_pVoIPCallImage = pVoIPCallImage;
	m_contactAccount = contactAccount;
}

LPCTSTR CFacadeWrapper::GetLastCallErrorDescription()
{
	return m_lastCallErrorDescription;
}


BOOL CFacadeWrapper::PlaySound(UINT nChar)
{
	BOOL bRtn = FALSE;

	CSoundEventPtr pSoundEvent = GetSoundEventByKey(nChar);
	
	if (pSoundEvent)
	{	
		bRtn = ::PlaySound(pSoundEvent->GetFilePath(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
	}
	
	return bRtn;
}


BOOL CFacadeWrapper::PlayCallIncomingSound()
{
	BOOL bRtn = FALSE;

	CString strEventType;
	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_CALL_RINGING);

	CSoundEventPtr pSoundEvent;
	pSoundEvent = CUOLFonePluginConfig::GetInstance()->GetSoundEvent(strEventType);	

	if (pSoundEvent)
	{
		m_FileWaveSound.SetOutputDevice(CUOLFoneController::GetInstance()->GetCurrentOutputDevice());
		m_FileWaveSound.SetLoops(CFileWaveSound::INFINITE_LOOP);
	
		bRtn = m_FileWaveSound.Play(pSoundEvent->GetFilePath());
	}

	return bRtn;
}


BOOL CFacadeWrapper::PlayCallClosedSound()
{
	BOOL bRtn = FALSE;

	CString strEventType;
	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_CALL_CLOSED);

	CSoundEventPtr pSoundEvent;
	pSoundEvent = CUOLFonePluginConfig::GetInstance()->GetSoundEvent(strEventType);	

	if (pSoundEvent)
	{
		m_FileWaveSound.SetOutputDevice(CUOLFoneController::GetInstance()->GetCurrentOutputDevice());
		m_FileWaveSound.SetLoops(1);
	
		bRtn = m_FileWaveSound.Play(pSoundEvent->GetFilePath());
	}

	return bRtn;
}

BOOL CFacadeWrapper::PlayCallCallingSound()
{
	BOOL bRtn = FALSE;

	CString strEventType;
	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_CALL_CALLING);

	CSoundEventPtr pSoundEvent;
	pSoundEvent = CUOLFonePluginConfig::GetInstance()->GetSoundEvent(strEventType);	

	if (pSoundEvent)
	{
		m_FileWaveSound.SetOutputDevice(CUOLFoneController::GetInstance()->GetCurrentOutputDevice());
		m_FileWaveSound.SetLoops(1);
	
		bRtn = m_FileWaveSound.Play(pSoundEvent->GetFilePath());
	}

	return bRtn;
}

BOOL CFacadeWrapper::StopCallSound()
{
	return m_FileWaveSound.Stop();
}


IUOLMessengerImageDrawStrategyPtr CFacadeWrapper::CreateDrawStrategy(
		IUOLMessengerUIManager::ImageDrawStrategy drawStrategy)
{
	IUOLMessengerImageDrawStrategyPtr pDrawStrategy = NULL;
	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();
	
	if (pUIManager2)
	{
		pDrawStrategy = pUIManager2->CreateDrawStrategy(drawStrategy);
	}
	
	return pDrawStrategy;
}


IUOLMessengerImagePtr CFacadeWrapper::GetPC2PhoneDefaultImage()
{
	if (!m_pImagePC2Phone)
	{
		if (IsUOLMessengerAppMode())
		{
			m_pImagePC2Phone = LoadImageFromResource(IDB_PC2PHONE_IMAGE);
		}
		else
		{
			m_pImagePC2Phone = LoadImageFromResource(IDB_PC2PHONE_IMAGE_ALT);
		}
	}

	return m_pImagePC2Phone;
}


IUOLMessengerImagePtr CFacadeWrapper::GetPC2PhoneDefaultSmallImage()
{
	if (!m_pImagePC2PhoneSmall)
	{
		if (IsUOLMessengerAppMode())
		{
			m_pImagePC2PhoneSmall = LoadImageFromResource(IDB_PC2PHONE_IMAGE_SMALL);
		}
		else
		{
			m_pImagePC2PhoneSmall = LoadImageFromResource(IDB_PC2PHONE_IMAGE_SMALL_ALT);
		}
	}

	return m_pImagePC2PhoneSmall;
}


IUOLMessengerImagePtr CFacadeWrapper::GetPC2PCDefaultImage()
{
	if (!m_pImagePC2PC)
	{
		if (IsUOLMessengerAppMode())
		{
			m_pImagePC2PC = LoadImageFromResource(IDB_CONTACT_IMAGE);
		}
		else
		{
			m_pImagePC2PC = LoadImageFromResource(IDB_CONTACT_IMAGE_ALT);
		}
	}

	return m_pImagePC2PC;
}


IUOLMessengerImagePtr CFacadeWrapper::GetPC2PCDefaultSmallImage()
{
	if (!m_pImagePC2PCSmall)
	{
		if (IsUOLMessengerAppMode())
		{
			m_pImagePC2PCSmall = LoadImageFromResource(IDB_CONTACT_IMAGE_SMALL);
		}
		else
		{
			m_pImagePC2PCSmall = LoadImageFromResource(IDB_CONTACT_IMAGE_SMALL_ALT);
		}
	}

	return m_pImagePC2PCSmall;
}


BOOL CFacadeWrapper::GetCurrentUserProfilePath(CString& profilePath)
{
	IUOLMessengerFacade* pUOLMessengerFacade = GetFacadeInstance();
	IUOLMessengerProfileManagerPtr pProfileManager = NULL;
	IUOLMessengerProfileElementPtr pProfileElement = NULL;
	CString currentProfile;
	BOOL result = FALSE;
	
	profilePath.Empty();
	
	if (pUOLMessengerFacade)
	{
		pProfileManager = pUOLMessengerFacade->GetProfileManager();
		
		if (pProfileManager != NULL)
		{
			currentProfile.Empty();
			currentProfile = pProfileManager->GetCurrentProfile();
			
			if (!currentProfile.IsEmpty())
			{
				if (pProfileManager->GetProfileElement(currentProfile, pProfileElement))
				{
					profilePath = pProfileElement->GetProfilePath();
					
					result = TRUE;
				}
			}
		}
	}
	
	return result;
}


BOOL CFacadeWrapper::LoadConfigFileStub(LPCTSTR resourceID, CString& configFileStub)
{
	BOOL bRtn = TRUE;
	LPTSTR lpRes;
	HRSRC hResInfo;
	HGLOBAL hRes;
	
	configFileStub.Empty();
	
	// Find the html resource.
	hResInfo = FindResource(ATL::_AtlBaseModule.GetResourceInstance(), 
			resourceID, RT_HTML);
	
	if (hResInfo == NULL)
	{
		return FALSE;
	}
	
	// Load the html resource. 
	hRes = LoadResource(ATL::_AtlBaseModule.GetResourceInstance(), hResInfo);
	
	if (hRes == NULL)
	{
		return FALSE;
	}
	
	// Lock the wave resource and play it.
	lpRes = (LPTSTR)LockResource(hRes);
	
	if (lpRes != NULL)
	{ 
		configFileStub.Format("%s", lpRes);
	}
	else
	{
		bRtn = 0;
	}
	
	return bRtn;
}


void CFacadeWrapper::OpenBrowserWithURL(CString url, BOOL bAuthenticated)
{
	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();
	
	if (pUIManager2)
	{
		pUIManager2->OpenBrowser(url, bAuthenticated);
	}
}


IUOLMessengerContactPtr CFacadeWrapper::FindContact(IUOLMessengerAccountPtr pIMAccount, const CString& strContact, BOOL bCreateWhenNotFound)
{
	IUOLMessengerContactPtr pContact;

	IUOLMessengerFacade* pUOLMessengerFacade = GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		IUOLMessengerContactListPtr pContactList = pUOLMessengerFacade->GetContactList();

		if (pContactList)
		{
			pContact = pContactList->FindContact(pIMAccount, strContact);

			if ((bCreateWhenNotFound) && (!pContact))
			{
				pContact = pContactList->CreateContact(pIMAccount, strContact);
			}
		}
	}

	return pContact;
}


void CFacadeWrapper::RegisterAsFocusObserver(
		CUOLMessengerMainFrameFocusObserver* pFocusObserver)
{
	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();
	IUOLMessengerMainFrameFocusNotifierPtr pFocusNotifier = NULL;
	
	if (pUIManager2)
	{
		pFocusNotifier = pUIManager2->GetMainFrameFocusNotifier();
		
		if (pFocusNotifier)
		{
			pFocusNotifier->RegisterObserver(pFocusObserver);
		}
	}
}


void CFacadeWrapper::UnregisterAsFocusObserver(
		CUOLMessengerMainFrameFocusObserver* pFocusObserver)
{
	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();
	IUOLMessengerMainFrameFocusNotifierPtr pFocusNotifier = NULL;
	
	if (pUIManager2)
	{
		pFocusNotifier = pUIManager2->GetMainFrameFocusNotifier();
		
		if (pFocusNotifier)
		{
			pFocusNotifier->UnregisterObserver(pFocusObserver);
		}
	}
}


BOOL CFacadeWrapper::RegisterAsMainWindowEventsNotifier(
		CUOLMessengerMainWindowEventsObserver* pEventsObserver)
{
	BOOL resultOk = FALSE;
	
	IUOLMessengerUIManager3Ptr pUIManager3 = GetUIManager3();
	IUOLMessengerMainWindowEventsNotifierPtr pWindowEventsNotifier = NULL;
	
	if (pUIManager3)
	{
		pWindowEventsNotifier = pUIManager3->GetMainWindowEventsNotifier();
		
		if (pWindowEventsNotifier)
		{
			pWindowEventsNotifier->RegisterObserver(pEventsObserver);
			
			resultOk = TRUE;
		}
	}
	
	return resultOk;
}


BOOL CFacadeWrapper::UnregisterAsMainWindowEventsNotifier(
		CUOLMessengerMainWindowEventsObserver* pEventsObserver)
{
	BOOL resultOk = FALSE;
	
	IUOLMessengerUIManager3Ptr pUIManager3 = GetUIManager3();
	IUOLMessengerMainWindowEventsNotifierPtr pWindowEventsNotifier = NULL;
	
	if (pUIManager3)
	{
		pWindowEventsNotifier = pUIManager3->GetMainWindowEventsNotifier();
		
		if (pWindowEventsNotifier)
		{
			pWindowEventsNotifier->UnregisterObserver(pEventsObserver);
			
			resultOk = TRUE;
		}
	}
	
	return resultOk;
}


void CFacadeWrapper::SetFocusToMainWindow()
{
	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();
	
	if (pUIManager2)
	{
		pUIManager2->GetAppWindow().SetFocus();
	}
}


void CFacadeWrapper::ExecuteInfomationButtonAction(UINT nActionID)
{
	if (nActionID == ACTION_CALLBACK_CONNECT_UOL_ACCOUNT)
	{
		if (m_pFacade)
		{
			IUOLMessengerAccountConnectionManagerPtr pConnectionManager = 
					m_pFacade->GetAccountConnectionManager();
			
			CUOLFoneAccountPtr pAccount = 
					CUOLFoneController::GetInstance()->GetCurrentAccount();
			
			if ( (pConnectionManager.get()) && (pAccount.get()) )
			{
				IUOLMessengerAccountPtr pIMAccount = pAccount->GetIMAccount();

				if (FALSE == pConnectionManager->IsConnected(pIMAccount))
				{
					pConnectionManager->Connect(pIMAccount);
				}
				else
				{
					::OutputDebugString(_T("CFacadeWrapper::ExecuteInfomationButtonAction() - IM Account already connected, connecting UOL Voip account\n"));

					CUOLFoneController::GetInstance()->Connect();
				}
			}
		}
	}
	else if (nActionID == ACTION_CALLBACK_CREATE_UOL_ACCOUNT)
	{
		IUOLMessengerFacade2* pFacade2 = static_cast<IUOLMessengerFacade2*>(m_pFacade);
		IUOLMessengerUIManager3Ptr pUIManager3 = GetUIManager3();
		
		if ( (pFacade2) && (pUIManager3) )
		{
			IUOLMessengerPropertyPageCommandFactoryPtr pPropPageCmdFactory = 
					pFacade2->GetPropertyPageCommandFactory();
			
			if (pPropPageCmdFactory)
			{
				IUOLMessengerPropertyPageCommandPtr pPropPageCommand = 
						pPropPageCmdFactory->GetPropertyPageCommand(
								IUOLMessengerPropertyPageCommandFactory::PP_COMMAND_TYPE_NEW_ACCOUNT);
				
				BOOL resultOk = TRUE;
				
				if (pPropPageCommand)
				{
					resultOk = pPropPageCommand->AddCommandValue(
							IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_TYPE, 
							IUOLMessengerPropertyPageCommand::ATV_PROTOCOL_UOL);
					
					// Testing code (Not used in fact).
					
					if (resultOk)
					{
						resultOk = pPropPageCommand->AddCommandValue(
								IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_USERNAME, 
								"mauro.brasil@tqi.com.br");
					}
					
					if (resultOk)
					{
						resultOk = pPropPageCommand->AddCommandValue(
								IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_PASSWORD, 
								"t3st3");
					}
					
					if (resultOk)
					{
						resultOk = pPropPageCommand->AddCommandValue(
								IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_SAVE_PASSWORD_FLAG, 
								TRUE);
					}
					
					// Testing code (Not used in fact).
					
					if (resultOk)
					{
						resultOk = pUIManager3->ShowPreferencesWithCommand(
								IUOLMessengerUIManager3::PT_ACCOUNTS_TAB, 
								pPropPageCommand);
					}
					
					ATLTRACE(_T("%s - ShowPreferencesWithCommand() resultOk = %d"), __FUNCTION__, resultOk);
				}
			}
		}
	}
	else if (nActionID == ACTION_CALLBACK_RECONNECT_VOIP_ACCOUNT)
	{
		if (m_pFacade)
		{
			IUOLMessengerAccountConnectionManagerPtr pConnectionManager = 
					m_pFacade->GetAccountConnectionManager();
			
			CUOLFoneAccountPtr pAccount = 
					CUOLFoneController::GetInstance()->GetCurrentAccount();
			
			if ( (pConnectionManager.get()) && (pAccount.get()) )
			{
				if ( TRUE == CUOLFoneController::GetInstance()->IsConnected())
				{
					CUOLFoneController::GetInstance()->Disconnect();
				}

				IUOLMessengerAccountPtr pIMAccount = pAccount->GetIMAccount();

				if (FALSE == pConnectionManager->IsConnected(pIMAccount))
				{
					pConnectionManager->Connect(pIMAccount);
				}
				else
				{
					CUOLFoneController::GetInstance()->Connect();
				}
			}
		}
	}
	else if (nActionID == ACTION_CALLBACK_RETURN)
	{
		ShowHomeWindow();	
	}
	else if (nActionID == ACTION_CALLBACK_RESTART_APP)
	{
		IUOLMessengerCommandPtr restartAppCommand = 
								m_pFacade->GetCommandFactory()->GetCommand(_T("RestartApp"));
		m_pFacade->ExecuteCommand(restartAppCommand);
	}
	else if ( (nActionID == ACTION_CALLBACK_CALL_STATE_CHANGE_TO_ERROR) || 
			(nActionID == ACTION_CALLBACK_CALL_ERROR_RECEIVED) ||
			(nActionID == ACTION_CALLBACK_BUY_CREDITS) ||
			(nActionID == ACTION_CALLBACK_ACCOUNT_STATE_CHANCE_TO_ERROR) )
	{
		OpenBrowserWithURL(m_lastCallErrorUrl);
	}
}


DWORD CFacadeWrapper::GetButtonImageResourceID(UINT nActionID)
{
	if (nActionID == ACTION_CALLBACK_CONNECT_UOL_ACCOUNT)
	{
		return IDB_BTN_CONECTAR_CONTA_UOL;
	}
	else if (nActionID == ACTION_CALLBACK_CREATE_UOL_ACCOUNT)
	{
		return IDB_BTN_CADASTRAR_CONTA_UOL;
	}
	else if (nActionID == ACTION_CALLBACK_RECONNECT_VOIP_ACCOUNT)
	{
		return IDB_BTN_RECONNECT;
	}
	else if (nActionID == ACTION_CALLBACK_RETURN)
	{
		return IDB_BTN_RETURN;
	}
	else if (nActionID == ACTION_CALLBACK_RESTART_APP)
	{
		return IDB_BTN_RESTART_APP;
	}
	else if (nActionID == ACTION_CALLBACK_BUY_CREDITS)
	{
		return IDB_BTN_BUY_CREDITS;
	}
	
	return IDB_BTN_SAIBA_MAIS;
}


void CFacadeWrapper::ShowUOLAccountDisconnected()
{
	ShowUserWarning(IDS_INFO_UOL_ACCOUNT_DISCONNECTED, 
			ACTION_CALLBACK_CONNECT_UOL_ACCOUNT);
}


void CFacadeWrapper::ShowUOLAccountNotFound()
{
	ShowUserWarning(IDS_INFO_UOL_ACCOUNT_NOT_FOUND, 
			ACTION_CALLBACK_CREATE_UOL_ACCOUNT);
}


void CFacadeWrapper::ShowBOLAccountWarning()
{
	ShowUserWarning(IDS_INFO_BOL_USER_WARNING, NULL);
}


void CFacadeWrapper::ShowUOLFoneSubscriberWarning()
{
	ShowUserWarning(IDS_INFO_UOLFONE_SUBSCRIBER_WARNING, NULL);

	//CString strUOLFoneSubscriberUrl;
	//strUOLFoneSubscriberUrl.LoadString(IDS_INFO_UOLFONE_SUBSCRIBER_URL);

	//ShowInformationWindow(strUOLFoneSubscriberUrl);
}


void CFacadeWrapper::ShowUserWarning(UINT nStrResourceID, int nActionID)
{
	CString strTitle;
	CString strText(_T(""));
	
	strTitle.LoadString(IDS_PLUGIN_NAME);

	if (nStrResourceID > 0)
	{
		strText.LoadString(nStrResourceID);
	}
	
	if (nActionID)
	{
		ShowInformationWindow(strTitle, strText, this, nActionID);
	}
	else
	{
		ShowInformationWindow(strTitle, strText, NULL);
	}
}


IUOLFoneControllerCallback::InitStatusCode CFacadeWrapper::GetInitStatusCode()
{
	return m_initStatusCode;
}


/////////////////////////////////////////////////////////////////////
// Protected members.
/////////////////////////////////////////////////////////////////////


void CFacadeWrapper::ShowKeyboardWindow()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_KEYBOARD_WINDOW, NULL);
}


void CFacadeWrapper::ShowCallInProgressWindow()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_CALL_IN_PROGRESS_WINDOW, NULL);
}


void CFacadeWrapper::ShowAcceptCallWindow()
{
	IUOLMessengerFacade* pFacade = GetFacadeInstance();

	if (pFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager =pFacade->GetUIManager();

		if (pUIManager)
		{
			CWindow wndAppWindow = pUIManager->GetAppWindow();

			if (wndAppWindow.IsWindow())
			{
				DWORD dwThreadId = GetCurrentThreadId();
				DWORD dwActiveThreadId = NULL;

				CWindow wndActive = GetForegroundWindow();

				dwActiveThreadId = ::GetWindowThreadProcessId(wndActive, &dwActiveThreadId);

				::AttachThreadInput(dwThreadId, dwActiveThreadId, TRUE);

				::SetForegroundWindow(wndAppWindow);

				::AttachThreadInput(dwThreadId, dwActiveThreadId, FALSE);

				wndAppWindow.ShowWindow(SW_RESTORE);
				wndAppWindow.BringWindowToTop();
			}
		}
	}

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
		
		// This invocation could be spread through the application classes, but I 
		// think all the window controling must be on this facade.
		
		ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_ACCEPT_CALL_WINDOW, NULL);
	}
}


void CFacadeWrapper::ShowCallInPlaceWindow()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_CALL_IN_PLACE_WINDOW, NULL);
}


void CFacadeWrapper::ShowCallErrorWindow(LPCTSTR errorStatus, LPCTSTR errorDescription, 
		ITabChildWindowActionCallback* pActionCallback, UINT nActionId)
{
	ATLTRACE(_T("%s\ncode = [%d]\ndescr = [%s]\nURL = [%s]\n"), 
			__FUNCTION__, m_lastCallErrorCode, m_lastCallErrorDescription, m_lastCallErrorUrl);

	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	CUOLFoneChildWindowInfo childWindowInfo;
	
	if (m_messagesHelper.IsPureErrorMessage(errorDescription))
	{
		childWindowInfo.SetWindowType(CUOLFoneCallErrorWindow::INFO_TYPE_PURE_ERROR);
	}
	else
	{
		childWindowInfo.SetWindowType(CUOLFoneCallErrorWindow::INFO_TYPE_CALL_ERROR);
	}
	childWindowInfo.SetTitle(errorStatus);
	childWindowInfo.SetMessage(errorDescription);
	childWindowInfo.SetActionCallback(pActionCallback);
	childWindowInfo.SetCallbackActionId(nActionId);

	ResetLastCallErrorInfo();

	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_CALL_ERROR_WINDOW, 
			&childWindowInfo);
}


void CFacadeWrapper::ShowCallErrorWindow(LPCTSTR strURL)
{
	ATLTRACE(_T("%s\ncode = [%d]\ndescr = [%s]\nURL = [%s]\n"), 
			__FUNCTION__, m_lastCallErrorCode, m_lastCallErrorDescription, m_lastCallErrorUrl);

	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	CUOLFoneChildWindowInfo childWindowInfo;
	
	childWindowInfo.SetKnowMoreURL(strURL);
	childWindowInfo.SetHoldWebBrowser(TRUE);
	
	ResetLastCallErrorInfo();
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_CALL_ERROR_WINDOW, 
			&childWindowInfo);
}


void CFacadeWrapper::ShowInitializingWindow()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_INITIALIZING_WINDOW, NULL);
}


void CFacadeWrapper::ShowInformationWindow(LPCTSTR title, LPCTSTR message, 
		ITabChildWindowActionCallback* pActionCallback, UINT nActionId)
{
	ATLTRACE(_T("%s\ncode = [%d]\ndescr = [%s]\nURL = [%s]\n"), 
			__FUNCTION__, m_lastCallErrorCode, m_lastCallErrorDescription, m_lastCallErrorUrl);

	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	CUOLFoneChildWindowInfo childWindowInfo;
	
	if (m_messagesHelper.IsCallWarningMessage(message))
	{
		childWindowInfo.SetWindowType(CUOLFoneCallErrorWindow::INFO_TYPE_CALL_WARNING);
	}
	else
	{
		childWindowInfo.SetWindowType(CUOLFoneCallErrorWindow::INFO_TYPE_PURE_WARNING);
	}
	childWindowInfo.SetTitle(title);
	childWindowInfo.SetMessage(message);
	childWindowInfo.SetActionCallback(pActionCallback);
	childWindowInfo.SetCallbackActionId(nActionId);
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_CALL_ERROR_WINDOW, 
			&childWindowInfo);

	//m_lastCallErrorDescription.Empty();
}


void CFacadeWrapper::ShowInformationWindow(LPCTSTR strURL)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_showChildWindowLock);
	
	// This invocation could be spread through the application classes, but I 
	// think all the window controling must be on this facade.
	
	CUOLFoneChildWindowInfo childWindowInfo;
	
	childWindowInfo.SetKnowMoreURL(strURL);
	childWindowInfo.SetHoldWebBrowser(TRUE);
	
	ShowChildWindow(CUOLFoneChildWindowContainerView::UOLFONE_INFORMATION_WINDOW, 
			&childWindowInfo);
}


void CFacadeWrapper::ShowChildWindow(int childWindowID, CUOLFoneChildWindowInfo* pChildWindowInfo)
{
	// TODO: optimize?
	if ( (m_uolFoneWndPtr != NULL) && (m_childContainerPtr != NULL) )
	{
		if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_KEYBOARD_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);
		}
		else if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_CALL_IN_PROGRESS_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);

			ShowUOLFoneTab();
		}
		else if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_ACCEPT_CALL_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);

			ShowUOLFoneTab();
		}
		else if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_CALL_IN_PLACE_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);
		}
		else if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_CALL_ERROR_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);
			
			m_bIsShowingError = TRUE;
		}
		else if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_INITIALIZING_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);
			
			m_bIsShowingError = FALSE;
		}
		else if (childWindowID == CUOLFoneChildWindowContainerView::UOLFONE_INFORMATION_WINDOW)
		{
			m_childContainerPtr->ShowChildWindow(childWindowID, pChildWindowInfo);
			m_uolFoneWndPtr->UpdateWindowInterface(childWindowID);
		}		
	}
}


void CFacadeWrapper::ShowUOLFoneTab()
{
	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();
	
	if (pUIManager2)
	{
		IUOLMessengerContactListTabPtr pContactListTab = pUIManager2->GetAppWindowContactListTab();

		pUIManager2->GetPluginCustomTabContainer()->ShowTab(
				m_pUOLMessengerPlugin->GetName(), m_pUOLMessengerPlugin->GetInternalName());
		
		if (pContactListTab)
		{
			pContactListTab->HideContactListTab();
		}
	}
}


IUOLMessengerImagePtr CFacadeWrapper::LoadImageFromResource(UINT nResId)
{
	IUOLMessengerImagePtr pImage;

	IUOLMessengerUIManager2Ptr pUIManager2 = GetUIManager2();

	if (pUIManager2)
	{
		pImage = pUIManager2->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), nResId);
	}

	return pImage;
}


void CFacadeWrapper::OnInitializationCompleted()
{
	m_initStatusCode = IUOLFoneControllerCallback::InitStatusCodeSucess;

	ATLASSERT(0 == m_nInitializationTimer);
	m_nInitializationTimer = SetTimer(TIMER_INITIALIZATION_PENDING, 500, NULL);
}


void CFacadeWrapper::OnInitializationFailed(
		InitStatusCode initStatusCode, CUOLFoneFailureInfoPtr pFailureInfo)
{
	// More than one assynchronous process call this method, so we need to 
	// protect it.
	if (!m_bInitializationHasAlreadyFailed)
	{
		m_initStatusCode = initStatusCode;
		
		if (pFailureInfo)
		{
			m_pLastFailureInfo = pFailureInfo->Clone();
		}
		
		ATLASSERT(0 == m_nInitializationTimer);
		m_nInitializationTimer = SetTimer(TIMER_INITIALIZATION_PENDING, 500, NULL);
		
		m_bInitializationHasAlreadyFailed = TRUE;
	}
}


void CFacadeWrapper::OnAccountConnectionStateChanged(CUOLFoneAccountPtr pAccount)
{
	CUOLFoneAccount::ConnectionState connectionState = pAccount->GetConnectionState();
	
	ATLTRACE(_T("%s - connectionState = %d\n"), __FUNCTION__, connectionState);
	
	BOOL bConnected = FALSE;

	CString strTitle;
	strTitle.LoadString(IDS_PLUGIN_NAME);

	m_callInPlace = FALSE;

	if (connectionState == CUOLFoneAccount::ConnectionStateOnline)
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_initTimerLock);

			if ( NULL != m_nInitializationTimer )
			{
				if ( KillTimer(m_nInitializationTimer) )
				{
					m_nInitializationTimer = NULL;
				}
			}
		}
		
		if ( (CUOLFoneController::GetInstance()->IsIMAccountConnected()) && 
				(!CUOLFoneController::GetInstance()->IsCallInProgress()) && 
				(!CUOLFoneController::GetInstance()->HasPendingCallToProceed()) )
		{
			ShowHomeWindow();
		}
		
		bConnected = TRUE;
	}
	else if ((connectionState == CUOLFoneAccount::ConnectionStateDisconnectionInProgress) ||
			 (connectionState == CUOLFoneAccount::ConnectionStateOffline))
	{
		ResetLastCallErrorInfo();

		//if (!m_lastCallErrorDescription.IsEmpty())
		//{
		//	//m_lastCallErrorDescription.LoadString(IDS_ERROR_ACCOUNT_OFFLINE);
		//	
		//	CategorizeAndShow(strTitle, m_lastCallErrorDescription, NULL);
		//}
		//else
		//{
			//ShowUOLAccountDisconnected();
			ShowHomeWindow();
		//}
	}
	
	else if (connectionState == CUOLFoneAccount::ConnectionStateErrorUnauthorized)
	{
		if ( m_lastCallErrorDescription.IsEmpty() )
		{
			m_lastCallErrorDescription.LoadString(IDS_ERROR_CONNECTION_UNAUTHORIZED);
		}
		
		if ( m_lastCallErrorUrl.IsEmpty())
		{
			CategorizeAndShow(strTitle, m_lastCallErrorDescription, 
				this, ACTION_CALLBACK_CONNECT_UOL_ACCOUNT);
		}
		else
		{
			CategorizeAndShow(strTitle, m_lastCallErrorDescription, 
				this, ACTION_CALLBACK_ACCOUNT_STATE_CHANCE_TO_ERROR);
		}
	}
	
	else if (connectionState == CUOLFoneAccount::ConnectionStateErrorServerUnavailable)
	{
		if ( m_lastCallErrorDescription.IsEmpty() )
		{
			m_lastCallErrorDescription.LoadString(IDS_ERROR_CONNECTION_SERVER_UNAVAILABLE);
		}

		CategorizeAndShow(strTitle, m_lastCallErrorDescription, NULL);
	}
	
	else if (connectionState == CUOLFoneAccount::ConnectionStateErrorTimeout)
	{
		if ( m_lastCallErrorDescription.IsEmpty() )
		{
			m_lastCallErrorDescription.LoadString(IDS_ERROR_CONNECTION_TIMEOUT);
		}

		CategorizeAndShow(strTitle, m_lastCallErrorDescription, this, ACTION_CALLBACK_CONNECT_UOL_ACCOUNT);
	}
	
	else if (connectionState == CUOLFoneAccount::ConnectionStateConnectionInProgress)
	{
		ShowInitializingWindow();
	}

	else
	{
		//connectionState == CUOLFoneAccount::ConnectionStateUnknown;
		ATLTRACE(_T("%s - CUOLFoneAccount::ConnectionStateUnknown\n"), __FUNCTION__);		
	}

	CContactListMonitor::GetInstance()->UpdateContactList(bConnected);
	CMessageWindowMonitor::GetInstance()->UpdateMessageWindows(bConnected);
	ShowSACButton();
}


void CFacadeWrapper::OnAccountInfoReceived(CUOLFoneAccountPtr pAccount)
{
	if (pAccount)
	{
		m_uolFoneWndPtr->UpdateAccountStats(pAccount);
	}
}


void CFacadeWrapper::OnAccountErrorReceived(CUOLFoneAccountPtr pAccount)
{
	ResetLastCallErrorInfo();

	if (NULL != pAccount)
	{
		CErrorInfoPtr pErrorInfo = pAccount->GetErrorInfo();
		if (NULL != pErrorInfo)
		{
			m_lastCallErrorDescription = pErrorInfo->GetMessage();
			m_lastCallErrorUrl = pErrorInfo->GetUrl();
			m_lastCallErrorCode = pErrorInfo->GetStatusCode();
			m_lastCallDisplayUrl = pErrorInfo->GetDisplayUrl();

			ATLTRACE(_T("%s:\n  code = [%d]\n  descr = [%s]\n  URL = [%s]\n  Display URL = [%s]\n"), 
					__FUNCTION__, m_lastCallErrorCode, m_lastCallErrorDescription, m_lastCallErrorUrl, m_lastCallDisplayUrl);
		}
	}
}

void CFacadeWrapper::LogBusyCall(const CString& phoneNumber)
{
	m_uolFoneWndPtr->OnLostCall();
	
	CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
	IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();

	CString strType;
	strType.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);

	CString strStatus;
	strStatus.LoadString(IDS_HISTORY_CALL_Phone2PC);

	CString strText;
	strText.LoadString(IDS_HISTORY_CALL_MESSAGE_MISSED);
	
	CHistoryRecordVoipPtr pHistoryRecordVoip;
	pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, pUIMAccount->GetUserId(), phoneNumber, strStatus, strType, strText);

	IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
		ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
	pHistoryManager4->LogEvent(pHistoryRecordVoip);
}

void CFacadeWrapper::LogBusyCall(IUOLMessengerContactPtr pContact)
{
	m_uolFoneWndPtr->OnLostCall();
	CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
	IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();

	CString strType;
	strType.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);

	CString strStatus;
	strStatus.LoadString(IDS_HISTORY_CALL_PC2PC);

	CString strText;
	strText.LoadString(IDS_HISTORY_CALL_MESSAGE_MISSED);

	CHistoryRecordVoipPtr pHistoryRecordVoip;
	pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, pUIMAccount->GetUserId(), pContact->GetName(), strStatus, strType, strText);

	IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
		ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
	pHistoryManager4->LogEvent(pHistoryRecordVoip);
}

void CFacadeWrapper::OnPhoneCallStateChanged(CPhoneCallPtr pPhoneCall)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);

	StopCallSound();

	CPhoneCall::PhoneCallState phoneCallState = pPhoneCall->GetPhoneCallState();

	if (phoneCallState == CPhoneCall::PhoneCallStateDialing)
	{
		m_lastCallErrorDescription.Empty();
		m_lastCallErrorUrl.Empty();
		
		ShowCallInProgressWindow();
	}

	else if (phoneCallState == CPhoneCall::PhoneCallStateClosed)
	{
		if (m_callIncoming)
		{
			m_callIncoming = FALSE;

			if (!m_bCallAlreadyLogged)
			{
				m_uolFoneWndPtr->OnLostCall();
				CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
				IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();
				
				CString strType;
				strType.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);

				IUOLMessengerContactPtr pContact = FindContact(pUIMAccount, m_contactAccount);
				
				if (pContact)
				{
					CString strStatus;
					strStatus.LoadString(IDS_HISTORY_CALL_PC2PC);

					CString strText;
					strText.LoadString(IDS_HISTORY_CALL_MESSAGE_MISSED);

					CHistoryRecordVoipPtr pHistoryRecordVoip;
					pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, pContact->GetName(), pUIMAccount->GetUserId(), strStatus, strType, strText);

					IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
						ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
					pHistoryManager4->LogEvent(pHistoryRecordVoip);
				}
				else
				{
					CString strStatus;
					strStatus.LoadString(IDS_HISTORY_CALL_Phone2PC);

					CString strText;
					strText.LoadString(IDS_HISTORY_CALL_MESSAGE_MISSED);

					CHistoryRecordVoipPtr pHistoryRecordVoip;
					pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, m_voIPCallText, pUIMAccount->GetUserId(), strStatus, strType, strText);

					IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
						ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
					pHistoryManager4->LogEvent(pHistoryRecordVoip);
				}

				m_bCallAlreadyLogged = FALSE;
				
			}

			CUOLFoneController::GetInstance()->RejectIncomingCall();
		}

		OnStopCall();
	}

	else if (phoneCallState == CPhoneCall::PhoneCallStateTalking)
	{
		m_callInPlace = TRUE;

		m_uolFoneWndPtr->UpdatePhoneCallStats(pPhoneCall);

		ShowCallInPlaceWindow();
	}

	else if (phoneCallState == CPhoneCall::PhoneCallStateError)
	{		
		m_callIncoming = FALSE;
		m_callInPlace = FALSE;

		BOOL bHandled = FALSE;

		// Handle special cases of error:
		// 1) Media timeout
		if (m_lastCallErrorCode == MEDIA_TIMEOUT_ERROR_CODE)
		{
			if (m_lastCallErrorUrl.IsEmpty())
			{
				ShowCallErrorWindow(NULL, m_lastCallErrorDescription, NULL);
			}
			else
			{
				ShowCallErrorWindow(NULL, m_lastCallErrorDescription, 
					this, ACTION_CALLBACK_CALL_ERROR_RECEIVED);
			}

			bHandled = TRUE;
		}
		// 2) Insufficient Account balance
		else if (m_lastCallErrorCode == NOT_ENOUGH_CREDITS_ERROR_CODE)
		{
			//if (FALSE == m_lastCallErrorUrl.IsEmpty())
			{
				ShowCallErrorWindow(NULL, m_lastCallErrorDescription, 
					this, ACTION_CALLBACK_BUY_CREDITS);

				bHandled = TRUE;
			}
		}

		// 3) Session not found or Session invalid
		else if (m_lastCallErrorCode == SESSION_NOT_FOUND ||
			m_lastCallErrorCode == SESSION_INVALID)
		{
			ShowCallErrorWindow(NULL, m_lastCallErrorDescription,
				this, ACTION_CALLBACK_RECONNECT_VOIP_ACCOUNT);
			
			bHandled = TRUE;
		}

		if (!bHandled)
		{
			// No display URL
			if (m_lastCallDisplayUrl.IsEmpty())
			{
				if (m_lastCallErrorDescription.IsEmpty())
				{
					/*
					// No error informed yet ? Let's try to get it.
					//
					CErrorInfoPtr pErrorInfo = pPhoneCall->GetErrorInfo();
					
					if (NULL != pErrorInfo)
					{
						CString strLastCallErrorDescription = pErrorInfo->GetMessage();
						CString strLastCallErrorUrl = pErrorInfo->GetUrl();
						UINT uLastCallErrorCode = pErrorInfo->GetStatusCode();
						CString strLastCallDisplayUrl = pErrorInfo->GetDisplayUrl();
						
						m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
								CUOLFoneFailureInfo::CALL_ERROR_UNDEFINED_FAILURE, 
								uLastCallErrorCode);
						
						if (m_pLastFailureInfo)
						{
							m_pLastFailureInfo->AddExtraFailureInfo(
									CUOLFoneFailureInfo::EXTRA_INFO_URL, strLastCallErrorUrl);
							
							m_pLastFailureInfo->AddExtraFailureInfo(
									CUOLFoneFailureInfo::EXTRA_INFO_ERROR_DISPLAY_URL, 
									strLastCallDisplayUrl);
						}
						
						LogFailureInfo(strLastCallErrorDescription);
					}
					*/
					
					if ( (m_pLastFailureInfo) && 
							(m_pLastFailureInfo->GetFailureType() == 
									CUOLFoneFailureInfo::FAILURE_TYPE_CALL) && 
							(m_pLastFailureInfo->GetExtraFailureInfoStr(
									CUOLFoneFailureInfo::EXTRA_INFO_STATUS_CODE).CompareNoCase("407") == 0) )
					{
						m_lastCallErrorDescription.LoadString(IDS_ERROR_PHONE_NOANSWER);
					}
					else
					{
						m_lastCallErrorDescription.LoadString(IDS_ERROR_PHONE_CALL);
					}
				}
				
				// Message only
				if (m_lastCallErrorUrl.IsEmpty())
				{
					CategorizeAndShow(NULL, m_lastCallErrorDescription, NULL);
				}
				// Message + Error URL
				else
				{
					CategorizeAndShow(NULL, m_lastCallErrorDescription, 
							this, ACTION_CALLBACK_CALL_STATE_CHANGE_TO_ERROR);
				}
			}
			// Display URL
			else
			{
				ShowCallErrorWindow(m_lastCallDisplayUrl);
			}
		}
	}
	
	else if (phoneCallState == CPhoneCall::PhoneCallStateRinging)
	{
		PlayCallCallingSound();
	}
	
	else if (phoneCallState == CPhoneCall::PhoneCallStateIncoming)
	{
		m_lastCallErrorDescription.Empty();
		m_lastCallErrorUrl.Empty();
		
		m_callIncoming = TRUE;

		PlayCallIncomingSound();
		ShowAcceptCallWindow();
	}
	
	else if (phoneCallState == CPhoneCall::PhoneCallStateBusy)
	{

		if (m_callIncoming)
		{
			CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
			IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();

			
			CTime startTime = CTime::GetCurrentTime();
			CString tmpDate, tmpTime;
			tmpDate = startTime.Format(_T("%d/%m/%y"));
			tmpTime = startTime.Format(_T("%H:%M:%S"));
			
			CString strType;
			strType.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);
			
			IUOLMessengerContactPtr pContact = FindContact(pUIMAccount, m_contactAccount);

			if (pContact)
			{
				CString strStatus;
				strStatus.LoadString(IDS_HISTORY_CALL_PC2PC);

				CString strText;
				strText.LoadString(IDS_HISTORY_CALL_MESSAGE_MISSED);
			
				CHistoryRecordVoipPtr pHistoryRecordVoip;
				pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, pContact->GetName(), pUIMAccount->GetUserId(), strStatus, strType, strText);

				IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
					ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
				pHistoryManager4->LogEvent(pHistoryRecordVoip);
			}
			else
			{
				CString strStatus;
				strStatus.LoadString(IDS_HISTORY_CALL_Phone2PC);
				
				CString strText;
				strText.LoadString(IDS_HISTORY_CALL_MESSAGE_MISSED);
				
				CHistoryRecordVoipPtr pHistoryRecordVoip;
				pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, m_voIPCallText, pUIMAccount->GetUserId(), strStatus, strType, strText);

				IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
					ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
				pHistoryManager4->LogEvent(pHistoryRecordVoip);
			}
		}
		m_callIncoming = FALSE;

		m_callInPlace = FALSE;

		if ( m_lastCallErrorDescription.IsEmpty() )
		{
			m_lastCallErrorDescription.LoadString(IDS_ERROR_PHONE_BUSY);
		}

		CategorizeAndShow(NULL, m_lastCallErrorDescription, NULL);
	}
	
	else if (phoneCallState == CPhoneCall::PhoneCallStateNoAnswer)
	{
		m_callInPlace = FALSE;

		if ( m_callIncoming )
		{
			ShowHomeWindow();
			//ligações perdidas -> nao atendidas
		}
		else
		{
			if ( m_lastCallErrorDescription.IsEmpty() )
			{
				m_lastCallErrorDescription.LoadString(IDS_ERROR_PHONE_NOANSWER);
			}

			CategorizeAndShow(NULL, m_lastCallErrorDescription, NULL);
		}

		m_callIncoming = FALSE;
	}
	
	//(phoneCallState == CPhoneCall::PhoneCallStateResumed)
	//(phoneCallState == CPhoneCall::PhoneCallStateHold)
	//(phoneCallState == CPhoneCall::PhoneCallStateRedirected)
}


void CFacadeWrapper::OnPhoneCallInfoReceived(CPhoneCallPtr pPhoneCall)
{
	if (pPhoneCall)
	{
		m_uolFoneWndPtr->UpdatePhoneCallStats(pPhoneCall);
	}
}


void CFacadeWrapper::OnPhoneCallErrorReceived(CPhoneCallPtr pPhoneCall, 
		CUOLFoneFailureInfoPtr pFailureInfo)
{
	m_pLastFailureInfo = pFailureInfo;
	
	LogFailureInfo("Falha na chamada!");
}


void CFacadeWrapper::OnPhoneCallServerErrorReceived(CPhoneCallPtr pPhoneCall)
{
	m_lastCallErrorDescription.Empty();
	m_lastCallErrorUrl.Empty();
	
	if (NULL != pPhoneCall)
	{
		CErrorInfoPtr pErrorInfo = pPhoneCall->GetErrorInfo();
		if (NULL != pErrorInfo)
		{
			m_lastCallErrorDescription = pErrorInfo->GetMessage();
			m_lastCallErrorUrl = pErrorInfo->GetUrl();
			m_lastCallErrorCode = pErrorInfo->GetStatusCode();
			m_lastCallDisplayUrl = pErrorInfo->GetDisplayUrl();
			
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_SERVER_ERROR_RECEIVED_FAILURE, 
					m_lastCallErrorCode);
			
			if (m_pLastFailureInfo)
			{
				m_pLastFailureInfo->AddExtraFailureInfo(
						CUOLFoneFailureInfo::EXTRA_INFO_URL, m_lastCallErrorUrl);
				
				m_pLastFailureInfo->AddExtraFailureInfo(
						CUOLFoneFailureInfo::EXTRA_INFO_ERROR_DISPLAY_URL, 
						m_lastCallDisplayUrl);
			}
			
			LogFailureInfo(m_lastCallErrorDescription);

			ATLTRACE(_T("%s:\n  code = [%d]\n  descr = [%s]\n  URL = [%s]\n  Display URL = [%s]\n"), 
					__FUNCTION__, m_lastCallErrorCode, m_lastCallErrorDescription, m_lastCallErrorUrl, m_lastCallDisplayUrl);

			if (pErrorInfo->GetStatusCode() == MEDIA_TIMEOUT_ERROR_CODE)
			{
				m_bMediaTimeoutError = TRUE;
				if (m_lastCallErrorUrl.IsEmpty())
				{
					ShowCallErrorWindow(NULL, m_lastCallErrorDescription, NULL);
				}
				else
				{		
					ShowCallErrorWindow(NULL, m_lastCallErrorDescription, 
						this, ACTION_CALLBACK_CALL_ERROR_RECEIVED);
				}
				HangUpPhoneCall();
				m_bMediaTimeoutError = FALSE;
			}

		}
	}
}


void CFacadeWrapper::OnExecutionFailure(ExecStatusCode execStatusCode, 
		CUOLFoneFailureInfoPtr pFailureInfo)
{
	CString strText;
	
	if (IUOLFoneControllerCallback::ExecStatusCodecUpdateFailure == execStatusCode)
	{
		strText = "Falha na atualização dos codecs.";
	}
	else
	{
		strText = "Falha de execução indeterminada.";
	}
	
	if (pFailureInfo)
	{
		m_pLastFailureInfo = pFailureInfo->Clone();
	}
	
	LogFailureInfo(strText);
}


BOOL CFacadeWrapper::CallPC(IUOLMessengerContactPtr pContact)
{
	BOOL bMakeCall = CUOLFoneController::GetInstance()->MakePhoneCall(pContact);

	//CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
	//IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();

	if (bMakeCall)
	{
		m_pCurContact = pContact;

		ShowCallInProgressWindow();
	}
	else
	{
		m_lastCallErrorDescription.LoadString(IDS_ERROR_CALL_TO_PC_FAILED);
		CategorizeAndShow(NULL, m_lastCallErrorDescription, NULL);
	}

	if (NULL != m_pCurContact)
	{
		CString strText;
		strText.LoadString(IDS_HISTORY_CALL_MESSAGE_DIALED); //= "Ligação efetuada.";

		CString strType;
		strType.LoadString(IDS_HISTORY_CALL_STATUS_DIALED);

		CString strStatus;
		strStatus.LoadString(IDS_HISTORY_CALL_PC2PC);

		CHistoryRecordVoipPtr pHistoryRecordVoip;
		pHistoryRecordVoip = new CHistoryRecordVoip(m_pCurContact->GetAccount(), m_pCurContact->GetAccount()->GetUserId(), m_pCurContact->GetName(), strStatus, strType, strText);

		IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
			ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
		pHistoryManager4->LogEvent(pHistoryRecordVoip);
	}

	return bMakeCall;
}


BOOL CFacadeWrapper::CallPhone(const CString& strPhoneNumber)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);

	BOOL bMakeCall = CUOLFoneController::GetInstance()->MakePhoneCall(strPhoneNumber);
	CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
	IUOLMessengerAccountPtr pUIMAccount = pFoneAccount->GetIMAccount();
	
	if (bMakeCall)
	{
		m_callInPlace = TRUE;

		m_strCurPhoneNumber = strPhoneNumber;

		ShowCallInProgressWindow();
	}
	else
	{
		m_lastCallErrorDescription.LoadString(IDS_ERROR_CALL_TO_PHONE_FAILED);
		
		m_pLastFailureInfo = CUOLFoneController::GetInstance()->GetLastFailureInfo();
		
		LogFailureInfo("Falha ao realizar a chamada!");
	}

	CString strText;
	strText.LoadString(IDS_HISTORY_CALL_MESSAGE_DIALED);

	CString strType;
	strType.LoadString(IDS_HISTORY_CALL_STATUS_DIALED);

	CString strStatus;
	strStatus.LoadString(IDS_HISTORY_CALL_PC2Phone);
	
	CString strContactName;
	if (0 == m_strCurPhoneNumber.Compare(GetSACnumber()))
	{
		strContactName.LoadString(IDS_LABEL_SAC);
	}
	else 
	{
		strContactName = m_strCurPhoneNumber;
	}

	if (!m_strCurPhoneNumber.IsEmpty())
	{
		CHistoryRecordVoipPtr pHistoryRecordVoip;
		pHistoryRecordVoip = new CHistoryRecordVoip(pUIMAccount, pUIMAccount->GetUserId(), strContactName, strStatus, strType, strText);

		IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
			ap_static_cast<IUOLMessengerHistoryManager4Ptr>(m_pFacade->GetHistoryManager());
		pHistoryManager4->LogEvent(pHistoryRecordVoip);
	}

	return bMakeCall;
}


void CFacadeWrapper::OnMainWindowPosCreate()
{
}


void CFacadeWrapper::OnMainWindowPosShowWindow(BOOL bShow)
{
	// Se for mostrar pela primeira vez, valida a flag que identifica a 
	// aba padrao da aplicacao.
	if ( (m_pFacade) && (m_showingFirstTime) && 
		 (bShow) && (m_nTabButtonIndex != -1) )
	{
		if (FALSE == IsUOLMessengerAppMode())
		{
			ShowUOLFoneTab();
		}

		m_showingFirstTime = FALSE;
	}
}


CSoundEventPtr CFacadeWrapper::GetSoundEventByKey(UINT nChar)
{
	CString strEventType;
	CSoundEventPtr pSoundEvent;
	
	if ('0' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY0);
	}
	else if ('1' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY1);
	}
	else if ('2' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY2);
	}
	else if ('3' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY3);
	}
	else if ('4' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY4);
	}
	else if ('5' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY5);
	}
	else if ('6' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY6);
	}
	else if ('7' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY7);
	}
	else if ('8' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY8);
	}
	else if ('9' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY9);
	}
	else if ('*' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_ASTERISK);
	}
	else if ('#' == nChar)
	{
		strEventType.LoadString(IDS_SOUND_EVENT_TYPE_POUND);
	}

	pSoundEvent = CUOLFonePluginConfig::GetInstance()->GetSoundEvent(strEventType);
	return pSoundEvent;
}


BOOL CFacadeWrapper::IsBOLUser(CUOLFoneAccountPtr pAccount)
{
	BOOL bIsBOLUser = FALSE;
	
	// TODO: Find a better way to validate this.
	//
	if (pAccount)
	{
		IUOLMessengerAccountPtr pIMAccount = pAccount->GetIMAccount();
		
		if (pIMAccount)
		{
			if (pIMAccount->GetUserId().Find("@bol.com.br") != -1)
			{
				bIsBOLUser = TRUE;
			}
		}
	}
	
	return bIsBOLUser;
}


void CFacadeWrapper::ShowPluginConfigDialog()
{
	m_pUOLMessengerPlugin->ShowConfigDialog();
}


void CFacadeWrapper::FormatPhoneToCall(CString& strPhoneNumber)
{
	if (FALSE == strPhoneNumber.IsEmpty())
	{
		if (strPhoneNumber[0] == '0')
		{
			if ((strPhoneNumber.GetLength() > 1) &&
				(strPhoneNumber[1] == '0'))
			{
				// Convert phones like 00ccdd99999999 into ccdd99999999
				strPhoneNumber = strPhoneNumber.Mid(2);
			}
			else
			{
				// Convert phones like 0dd99999999 into 55dd99999999
				strPhoneNumber = "55" + strPhoneNumber.Mid(1);
			}
		}
		else if (strPhoneNumber[0] != '+')
		{
			// Convert phones like dd99999999 into 55dd99999999
			strPhoneNumber = "55" + strPhoneNumber;
		}
		else
		{
			// Convert phones like +ccdd99999999 into ccdd99999999
			strPhoneNumber = strPhoneNumber.Mid(1);
		}
	}

	// Check if all characters are digit
	for (int nInd = 0; nInd < strPhoneNumber.GetLength(); nInd++)
	{
		TCHAR ch = strPhoneNumber[nInd];
		
		if (0 == ::_istdigit(ch))
		{
			// Non-digit
			strPhoneNumber.Empty();
		}
	}
}


BOOL CFacadeWrapper::IsUOLMessengerAppMode()
{
	if (m_pFacade)
	{
		IUOLMessengerFacade2* pFacade2 = static_cast<IUOLMessengerFacade2*>(m_pFacade);
		
		if (pFacade2)
		{
			IUOLMessengerCommonSettingsPtr pCommonSettings = 
					pFacade2->GetCommonSettings();
			
			if (pCommonSettings.get())
			{
				return (pCommonSettings->GetAppMode().CompareNoCase(IUOLMessengerCommonSettings::APP_MODE_DEFAULT_VALUE) == 0);
			}
		}
	}

	return TRUE;
}


void CFacadeWrapper::ResetLastCallErrorInfo()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	m_lastCallErrorCode = 0;
	m_lastCallErrorDescription.Empty();
	// TODO: reset call error URL
	// m_lastCallErrorUrl.Empty();
	m_lastCallDisplayUrl.Empty();
}



void CFacadeWrapper::ShowHistoryWindow(const CString& strTypeFilter)
{
	CUOLFoneAccountPtr pFoneAccount = CUOLFoneController::GetInstance()->GetCurrentAccount();
	if (pFoneAccount)
	{
		IUOLMessengerAccountPtr pAccount = pFoneAccount->GetIMAccount();

		IUOLMessengerUIManager7Ptr pUIManager7 = 
			ap_static_cast<IUOLMessengerUIManager7Ptr>(m_pFacade->GetUIManager());
		if (pUIManager7)
		{
			CString strCaption;
			strCaption.LoadString(IDS_HISTORY_WINDOW_CAPTION);

			CHistoryRecordVoipPtr pHistoryRecordVoip;
			pHistoryRecordVoip = new CHistoryRecordVoip(pAccount, strTypeFilter);

			CHistoryRecordVoipListPtr pHistoryRecordVoipList;
			pHistoryRecordVoipList = new CHistoryRecordVoipList(pHistoryRecordVoip);
			pUIManager7->ShowHistoryWindow(pHistoryRecordVoip, pHistoryRecordVoipList, strCaption);
		}
	}
	else
	{
		CString strCaption;
		CString strErrorNoCurrentAccount;

		strCaption.LoadString(IDS_PLUGIN_NAME);
		strErrorNoCurrentAccount.LoadString(IDS_HISTORY_ERROR_NO_CURRENT_ACCOUNT);

		IUOLMessengerUIManager4Ptr pUIManager4 = 
			ap_static_cast<IUOLMessengerUIManager4Ptr>(GetUIManager4());

		if (pUIManager4)
		{
			pUIManager4->ShowMessageBox(NULL, strErrorNoCurrentAccount, strCaption, MB_OK | MB_ICONERROR);
		}
	}
}


void CFacadeWrapper::ShowVoipRates()
{
	CString strUrl;
	CString strCaption;

	//strCaption.LoadString(IDS_SEARCH_CAPTION);
	//strUrl = pConfig->GetSiteSearchURL();
	strCaption.LoadString(IDS_TABELA_PRECOS_CAPTION);
	strUrl = GetpriceURL(); //.LoadString(IDS_URL_SITE_TABELA_PRECOS);

	IUOLMessengerUIManagerPtr pUIManager = CFacadeWrapper::GetFacadeInstance()->GetUIManager();

	if (pUIManager)
	{
		CWindow wndFrameWindow;

		if (m_pFrameWindow)
		{
			wndFrameWindow = m_pFrameWindow->GetHWND();

			if (wndFrameWindow.IsWindow())
			{
				wndFrameWindow.BringWindowToTop();
				wndFrameWindow.ShowWindow(SW_NORMAL);
				m_pView->SetUrl(strUrl);
				return ;
			}
		}

		if (!m_pView)
		{
			m_pView = new CUOLFoneRatesWindowFrame();
		}

		m_pFrameWindow = pUIManager->CreateFrameWindow(m_pView, CRect(CPoint(0, 0), CSize(490, (424+83))));

		if (m_pFrameWindow)
		{
			wndFrameWindow = m_pFrameWindow->GetHWND();

			CRect rect;
			CRect rectWindow;
			CRect cRect;
			CRect CrectWindow;
			CRect parRect;
			CRect rectDesktop;

			pUIManager->GetAppWindow().GetClientRect(&parRect);
			pUIManager->GetAppWindow().ClientToScreen(&parRect);

			wndFrameWindow.GetClientRect(rect);
			wndFrameWindow.GetWindowRect(rectWindow);
			wndFrameWindow.GetClientRect(cRect);
			wndFrameWindow.GetWindowRect(CrectWindow);

			wndFrameWindow.ClientToScreen(&cRect);
			wndFrameWindow.ClientToScreen(&CrectWindow);

			::GetWindowRect(::GetDesktopWindow(),&rectDesktop);

			int x,y;

			if (parRect.left>=(cRect.Width()+1))
			{
				x = parRect.left-cRect.Width()+1;
			} 
			else
			{
				x = parRect.right+1;
			}

			if (parRect.bottom>rectDesktop.bottom)
			{
				y = parRect.top;
			} 
			else
			{
				y = parRect.bottom-cRect.Height()+1;
			}


			//x=x - 5;
			//y=y - 24;
			x = x - (CrectWindow.right - cRect.right - 3);
			y = y - (CrectWindow.bottom - cRect.bottom - 3);


			wndFrameWindow.SetWindowPos(NULL,x, y,0,0,SWP_NOSIZE | SWP_NOZORDER);
			//wndFrameWindow.CenterWindow(::GetDesktopWindow());
			wndFrameWindow.ShowWindow(SW_NORMAL);

			m_pFrameWindow->SetCaptionText(strCaption);
			m_pFrameWindow->SetStatusBarText(_T(""));
		}

		m_pView->SetUrl(strUrl);
	}
}
void CFacadeWrapper::LoadFoneInfo()
{
	m_priceURLNode = NULL;

	CString strXML; //= _T("http://panel.fone.uol.com.br/uolfoneinfo.xml");
	strXML.LoadString(IDS_URL_PHONE_INFO);

	BOOL result = FALSE;

	HRESULT hr = S_OK;

	CString strFolder;
	CHttpDownloader httpDownloader;

	CString strLocalPath;
	GetCurrentUserProfilePath(strLocalPath);
	strLocalPath += _T("\\xmlTemp.xml");

	CDownloadDllItem* pDllItem = new CDownloadDllItem(strXML,strLocalPath);
	ATLASSERT(NULL != pDllItem);
	
	httpDownloader.SetUrl(pDllItem->m_strFromUrl);
	httpDownloader.SetLocalFilePath(pDllItem->m_strLocalFilePath);
	DWORD dwErrorCode = httpDownloader.DownloadFile();
	if ( CHttpDownloader::DE_ERROR_SUCCESS !=  dwErrorCode )
	{
		hr = E_FAIL;
	}

	
	if (hr != E_FAIL)
	{
		if (PathFileExists(strLocalPath))
		{
			result = m_uolfoneInfoDoc.Load(strLocalPath);
		}
	}

	if (result)
	{
		m_infoLoaded = TRUE;

		CXmlNodeWrapper sourceNode(m_uolfoneInfoDoc.AsNode());

		m_SACnumberNode = new CXmlNodeWrapper(
			sourceNode.GetNode("SACnumber"));

		//m_priceURLNode = new CXmlNodeWrapper(
		//	sourceNode.GetNode("priceURL"));

		m_promoURLNode = new CXmlNodeWrapper(
			sourceNode.GetNode("promoURL"));

		m_buyPlanURLNode = new CXmlNodeWrapper(
			sourceNode.GetNode("buyPlanURL"));

		m_buyCreditsURLNode = new CXmlNodeWrapper(
			sourceNode.GetNode("buyCreditsURL"));

		m_VoipOutPlanIDsNode = new CXmlNodeWrapper(
			sourceNode.GetNode("VoipOutPlanIDs"));
	}
	DeleteFile(strLocalPath);
}


CString CFacadeWrapper::GetSACnumber()
{
	if (m_infoLoaded)
	{
		m_strSACnumber = m_SACnumberNode->GetText();
		return m_strSACnumber;
		//return _T("");
	}
	else
	{
		return _T("");
	}
	
	//return _T("551140029002");
}

CString CFacadeWrapper::GetpriceURL()
{
	//if (m_infoLoaded)
	//{
	//	return m_priceURLNode->GetText();
	//}
	//else
	//{
	//	return _T("");
	//}
	m_strpriceURL.LoadString(IDS_URL_SITE_TABELA_PRECOS);
	return m_strpriceURL;

	//return _T("http://voip.uol.com.br/tarifas.html");
}

CString CFacadeWrapper::GetpromoURL()
{
	if (m_infoLoaded)
	{
		return m_promoURLNode->GetText();
	}
	else
	{
		return _T("");
	}
	
	//m_strpromoURL.LoadString(IDS_URL_PROMO_BANNER);
	//return m_strpromoURL;
}

CString CFacadeWrapper::GetBuyPlanURL()
{
	CString strUrl;
	
	if (m_infoLoaded)
	{
		strUrl = m_buyPlanURLNode->GetText();
		if (strUrl.IsEmpty())
		{
			strUrl.LoadString(IDS_URL_SITE_HOME_UOL_VOIP);
		}
	}

	return strUrl;
}

CString CFacadeWrapper::GetBuyCreditsURL()
{
	CString strUrl;
	
	if (m_infoLoaded)
	{
		strUrl = m_buyCreditsURLNode->GetText();
		if (strUrl.IsEmpty())
		{
			strUrl.LoadString(IDS_URL_SITE_HOME_UOL_VOIP);
		}
	}

	return strUrl;
}


CString CFacadeWrapper::GetVoipOutPlanIDs()
{
	CString strPlanIDs;
	
	if (m_infoLoaded)
	{
		strPlanIDs = m_VoipOutPlanIDsNode->GetText();
	}

	return strPlanIDs;
}


void CFacadeWrapper::AddSoundEvents()
{
	if (m_pFacade)
	{
		IUOLMessengerFacade3* pFacade3 = static_cast<IUOLMessengerFacade3*>(m_pFacade);

		if (pFacade3)
		{
			IUOLMessengerSettingsManager3Ptr pSettingsManager = 
				ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(pFacade3->GetSettingsManager());

			if (pSettingsManager.get())
			{
				IUOLMessengerSoundSettings3Ptr pSoundSettings = 
					ap_dynamic_cast<IUOLMessengerSoundSettings3Ptr>(pSettingsManager->GetSoundSettings());

				if (pSoundSettings.get())
				{
					pSoundSettings->RegisterObserver(this);
					
					CAtlList<CSoundEventPtr> soundEvents;
					CUOLFonePluginConfig::GetInstance()->GetSoundEvents(soundEvents);

					if (soundEvents.GetCount() > 0)
					{
						for (POSITION pos = soundEvents.GetHeadPosition(); pos; )
						{
							CSoundEventPtr pSoundEvent = soundEvents.GetNext(pos);

							CString strEventType = pSoundEvent->GetEventType();
							CString strDescription = pSoundEvent->GetDescription();
							CPath strFilePath(pSoundEvent->GetFilePath());
							BOOL bEnabled = pSoundEvent->IsEnabled();

							pSoundSettings->AddSoundEvent(strEventType, strDescription, 
											        strFilePath, bEnabled);
						}
					}
				}
			}
		}
	}
}


void CFacadeWrapper::RemoveSoundEvents()
{
	if (m_pFacade)
	{
		IUOLMessengerFacade3* pFacade3 = static_cast<IUOLMessengerFacade3*>(m_pFacade);

		if (pFacade3)
		{
			IUOLMessengerSettingsManager3Ptr pSettingsManager = 
				ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(pFacade3->GetSettingsManager());

			if (pSettingsManager.get())
			{
				IUOLMessengerSoundSettings3Ptr pSoundSettings = 
					ap_dynamic_cast<IUOLMessengerSoundSettings3Ptr>(pSettingsManager->GetSoundSettings());

				if (pSoundSettings.get())
				{
					pSoundSettings->UnregisterObserver(this);
					
					CAtlList<CSoundEventPtr> soundEvents;
					CUOLFonePluginConfig::GetInstance()->GetSoundEvents(soundEvents);

					if (soundEvents.GetCount() > 0)
					{
						for (POSITION pos = soundEvents.GetHeadPosition(); pos; )
						{
							CSoundEventPtr pSoundEvent = soundEvents.GetNext(pos);
							pSoundSettings->RemoveSoundEvent(pSoundEvent->GetEventType());
						}
					}
				}
			}
		}
	}
}


void CFacadeWrapper::OnSoundSettingsChanged(IUOLMessengerSoundSettings3Ptr pSoundSettings)
{
	CAtlList<IUOLMessengerSoundEventPtr> listSoundEvents;
	pSoundSettings->GetSoundEventList(listSoundEvents);

	for (POSITION pos = listSoundEvents.GetHeadPosition(); pos; )
	{
		IUOLMessengerSoundEventPtr pSoundEvent = listSoundEvents.GetNext(pos);

		if (pSoundEvent)
		{	
			CSoundEventPtr pPluginSoundEvent;
			pPluginSoundEvent = CUOLFonePluginConfig::GetInstance()->GetSoundEvent(pSoundEvent->GetEventType());

			if (pPluginSoundEvent)
			{
				if (pPluginSoundEvent->GetFilePath() != pSoundEvent->GetFilePath())
				{
					pPluginSoundEvent->SetFilePath(pSoundEvent->GetFilePath());
				}
			}
		}
	}

	CUOLFonePluginConfig::GetInstance()->SaveUOLFoneConfig();
}


void CFacadeWrapper::LogFailureInfo(CString strFriendlyMessage)
{
	IUOLMessengerLoggingManagerPtr pLoggingManager = GetLoggingManager();
	IUOLMessengerLogger2Ptr pPluginLogger;
	
	if (pLoggingManager != NULL)
	{
		pPluginLogger = pLoggingManager->GetLogger(PLUGIN_LOGGER_MODULE_NAME);
	}
	
	if (pPluginLogger != NULL)
	{
		CString strLoggingMessage;
		
		strLoggingMessage.Format("ERROR - User Error = [%s]. ", strFriendlyMessage);
		
		if (m_pLastFailureInfo != NULL)
		{
			strLoggingMessage += m_pLastFailureInfo->GetFailureInfoText();
		}
		else
		{
			strLoggingMessage += "m_pLastFailureInfo = NULL.";
			
			ATLTRACE("%s -> m_pLastFailureInfo = NULL", __FUNCTION__);
		}
		
		pPluginLogger->Log(strLoggingMessage);
	}
}


BOOL CFacadeWrapper::ChooseStartCallType()
{
	//Determines if user is calling a contact or phone
	CComCritSecLock<CComAutoCriticalSection> lock(m_callInPlaceLock);

	BOOL result = FALSE;
	BOOL bCallPhone = TRUE;

	CWindow* keyboardWindowPtr = m_childContainerPtr->GetChildWindowPtr(
		CUOLFoneChildWindowContainerView::UOLFONE_KEYBOARD_WINDOW);

	if (keyboardWindowPtr != NULL)
	{
		CString strContactName;
		((CUOLFoneKeyboardWindow*)keyboardWindowPtr)->GetDigitedText(strContactName);

		if (!strContactName.IsEmpty())
		{
			CUOLFoneAccountManagerPtr pAccountManager = CUOLFoneController::GetInstance()->GetUOLFoneAccountManager();

			if (pAccountManager)
			{
				IUOLMessengerAccountPtr pIMAccount = pAccountManager->GetCurrentIMAccount();

				if (pIMAccount)
				{
					CIMProtocol* pProtocol = pIMAccount->GetProtocol();

					if (pProtocol != NULL)
					{
						if (pProtocol->IsValidUserName((LPCTSTR) strContactName))
						{
							// Allow only UOL and BOL contacts
							if ((strContactName.Find("@bol.com.br") != -1) || 
								(strContactName.Find("@uol.com.br") != -1))
							{							
								// Find or create contact
								IUOLMessengerContactPtr pContact;
								pContact = FindContact(pIMAccount, strContactName, TRUE);

								if (pContact)
								{
									bCallPhone = FALSE;
									result = StartCallToPC(pContact, TRUE);
								}
							}
						}
					}
				}
			}		
		}
	}

	if (bCallPhone)
	{
		result = StartCallToPhone();
	}
	
	return result;
}


void CFacadeWrapper::UpdateBalancePanel(CUOLFoneAccountPtr pAccount)
{
	if (m_uolFoneWndPtr != NULL)
	{
		m_uolFoneWndPtr->UpdateBalancePanel(pAccount);
	}
}


void CFacadeWrapper::CategorizeAndShow(LPCTSTR statusText, LPCTSTR description, 
		ITabChildWindowActionCallback* pActionCallback, UINT nActionId)
{
	// We will consider that messages that are not warnings will be errors.
	//
	if ( (m_messagesHelper.IsPureWarningMessage(description)) || 
			(m_messagesHelper.IsCallWarningMessage(description)) )
	{
		ShowInformationWindow(statusText, description, pActionCallback, nActionId);
	}
	else
	{
		ShowCallErrorWindow(statusText, description, pActionCallback, nActionId);
	}
}


void CFacadeWrapper::LogOsipDebugEvent(
		const CString& strFileName, int iLineNumber, 
		int iLogLevel, const CString& strLogMessage)
{
	IUOLMessengerLoggingManagerPtr pLoggingManager = GetLoggingManager();
	IUOLMessengerLogger2Ptr pVoipLogger;
	
	if (pLoggingManager != NULL)
	{
		pVoipLogger = pLoggingManager->GetLogger(VOIP_LOGGER_MODULE_NAME);
	}
	
	if (pVoipLogger != NULL)
	{
		CPath filePath(strFileName);
		filePath.StripPath();
		
		// Remove all break line combination.
		//
		CString strNewLogMessage = strLogMessage;
		strNewLogMessage.Replace("\n", " ");
		strNewLogMessage.Replace("\r", " ");
		
		CString loggingMessage;
		loggingMessage.Format("%s - %s[%d]: %s", 
				pVoipLogger->GetLogLevelName(
						(IUOLMessengerLogger::LogLevelType)iLogLevel), 
				filePath.m_strPath, 
				iLineNumber, 
				strNewLogMessage);
		
		pVoipLogger->Log(loggingMessage, TRUE, TRUE);
	}
}


IUOLMessengerLoggingManagerPtr CFacadeWrapper::GetLoggingManager()
{
	IUOLMessengerFacade4* pFacade4 = 
			static_cast<IUOLMessengerFacade4*>(m_pFacade);
	
	IUOLMessengerLoggingManagerPtr pLoggingManager;
	
	if (pFacade4 != NULL)
	{
		pLoggingManager = pFacade4->GetLoggingManager();
	}
	
	return pLoggingManager;
}


CString CFacadeWrapper::GetLogsDefaultPath()
{
	IUOLMessengerLoggingManagerPtr pLoggingManager = GetLoggingManager();
	
	if (pLoggingManager != NULL)
	{
		return pLoggingManager->GetLogsDefaultPath();
	}
	
	return "";
}


int CFacadeWrapper::GetOsipLogCurrentLevel()
{
	IUOLMessengerLoggingManagerPtr pLoggingManager = GetLoggingManager();
	IUOLMessengerLogger2Ptr pVoipLogger;
	
	if (pLoggingManager != NULL)
	{
		pVoipLogger = pLoggingManager->GetLogger(VOIP_LOGGER_MODULE_NAME);
	}
	
	if (pVoipLogger != NULL)
	{
		return pVoipLogger->GetLogLevel();
	}
	
	return IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_DEBUG;
}


IUOLMessengerDiagnoseManagerPtr CFacadeWrapper::GetDiagnoseManager()
{
	IUOLMessengerFacade4* pFacade4 = 
			static_cast<IUOLMessengerFacade4*>(m_pFacade);
	
	IUOLMessengerDiagnoseManagerPtr pDiagnoseManager;
	
	if (pFacade4 != NULL)
	{
		pDiagnoseManager = pFacade4->GetDiagnoseManager();
	}
	
	return pDiagnoseManager;
}
