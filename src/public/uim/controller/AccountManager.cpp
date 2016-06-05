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
#include "AccountManager.h"

#include "../model/SettingsTraits.h"
#include "../model/Account.h"
#include "../resource.h"

/// <TODAOFERTA>
#include "../Controller/UIMApplication.h"
#include <jabberconstants.h>
/// </TODAOFERTA>


CAccountListEventListenerWrapper::CAccountListEventListenerWrapper(CUOLMessengerAccountListEventListener* pWrapped) :
	CObserverWrapper<CUOLMessengerAccountListEventListener>(pWrapped)
{
}


void CAccountListEventListenerWrapper::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
	BEGIN_HANDLER_CALL();

	m_pObserver->OnAddAccount(pAccount);

	END_HANDLER_CALL();
}


void CAccountListEventListenerWrapper::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	BEGIN_HANDLER_CALL();

	m_pObserver->OnRemoveAccount(pAccount);

	END_HANDLER_CALL();
}


void CAccountListEventListenerWrapper::OnAccountProtocolChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldProtocolId)
{
	BEGIN_HANDLER_CALL();

	m_pObserver->OnAccountProtocolChanged(pAccount, strOldProtocolId);
	
	END_HANDLER_CALL();
}


void CAccountListEventListenerWrapper::OnAccountUserIdChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldUserId)
{
	BEGIN_HANDLER_CALL();

	m_pObserver->OnAccountUserIdChanged(pAccount, strOldUserId);

	END_HANDLER_CALL();
}







void CAccountListEventListenerNotifier::RegisterListener(CUOLMessengerAccountListEventListener* pListener)
{
	if (pListener != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_csListeners);

		for (POSITION pos = m_listListeners.GetHeadPosition(); pos; )
		{
			CAccountListEventListenerWrapperPtr pListenerWrapper = m_listListeners.GetNext(pos);

			if (pListener == pListenerWrapper->GetWrappedObserver())
			{
				// Already registered
				ATLASSERT(FALSE);
				return;
			}
		}

		m_listListeners.AddTail(new CAccountListEventListenerWrapper(pListener));
	}
}


void CAccountListEventListenerNotifier::UnregisterListener(CUOLMessengerAccountListEventListener* pListener)
{
	if (pListener != NULL)
	{	
		CAccountListEventListenerWrapperPtr pRemovedListenerWrapper;
		
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csListeners);

			for (POSITION pos = m_listListeners.GetHeadPosition(); pos; m_listListeners.GetNext(pos))
			{
				CAccountListEventListenerWrapperPtr pListenerWrapper = m_listListeners.GetAt(pos);

				if (pListener == pListenerWrapper->GetWrappedObserver())
				{
					// Listener found
					pRemovedListenerWrapper = pListenerWrapper;
					m_listListeners.RemoveAt(pos);
					break;
				}
			}
		}

		//ATLASSERT(pRemovedListenerWrapper);
		if (pRemovedListenerWrapper)
		{
			pRemovedListenerWrapper->DisableNotifications();
		}
	}
}


void CAccountListEventListenerNotifier::UnregisterAll()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csListeners);

	for (POSITION pos = m_listListeners.GetHeadPosition(); pos; m_listListeners.GetNext(pos))
	{
		CAccountListEventListenerWrapperPtr pListenerWrapper = m_listListeners.GetAt(pos);

		pListenerWrapper->DisableNotifications();
	}

	m_listListeners.RemoveAll();
}


void CAccountListEventListenerNotifier::NotifyAll(void (CUOLMessengerAccountListEventListener::*pEventHandler)(IUOLMessengerAccountPtr), IUOLMessengerAccountPtr pAccount)
{
	CAtlList<CAccountListEventListenerWrapperPtr> listListeners;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csListeners);

		listListeners.AddTailList(&m_listListeners);
	}

	CAccountListEventListenerWrapperPtr pListener;

	for (POSITION pos = listListeners.GetHeadPosition(); pos; )
	{
		pListener = listListeners.GetNext(pos);

		((pListener.get())->*pEventHandler)(pAccount);
	}
}


void CAccountListEventListenerNotifier::NotifyAll(void (CUOLMessengerAccountListEventListener::*pEventHandler)(IUOLMessengerAccountPtr, const CString&), IUOLMessengerAccountPtr pAccount, const CString& strParam)
{
	CAtlList<CAccountListEventListenerWrapperPtr> listListeners;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csListeners);

		listListeners.AddTailList(&m_listListeners);
	}

	CAccountListEventListenerWrapperPtr pListener;

	for (POSITION pos = listListeners.GetHeadPosition(); pos; )
	{
		pListener = listListeners.GetNext(pos);

		((pListener.get())->*pEventHandler)(pAccount, strParam);
	}
}








CAccountManager::CAccountManager() : 
	m_pUOLMessengerFacade(NULL),
	m_settingsHandler("")
{
	InitTagProcessMap();
}


CAccountManager::~CAccountManager()
{
}


BOOL CAccountManager::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (pUOLMessengerFacade != NULL)
	{
		m_pUOLMessengerFacade = pUOLMessengerFacade;

		IUOLMessengerProfileManagerPtr pProfileManager = m_pUOLMessengerFacade->GetProfileManager();

		if (pProfileManager != NULL)
		{
			CString strPath;
			IUOLMessengerProfileElementPtr pProfileElement;

			pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);
			strPath = pProfileElement->GetProfilePath();

			CString strAccountsFile;
			strAccountsFile.LoadString(IDS_ACCOUNT_MANAGER_ACCOUNTS_FILE);

			strPath += "\\" + strAccountsFile;

			m_settingsHandler.Init(strPath, FALSE, FALSE);
			m_settingsHandler.AddSetting(_T("Accounts"), this);

			bRet = TRUE;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CAccountManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	for (POSITION pos = m_listAccounts.GetHeadPosition(); pos; m_listAccounts.GetNext(pos))
	{
		CAccountPtr pAccount = ap_dynamic_cast<CAccountPtr>(m_listAccounts.GetAt(pos));
		if (pAccount)
		{
			pAccount->Finalize();
		}
	}

	m_listAccounts.RemoveAll();
	m_listUnboundAccounts.RemoveAll();
	m_listRemovedAccounts.RemoveAll();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CAccountManager::UpdateAccountsAuthenticationData()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (m_pUOLMessengerFacade != NULL)
	{
		// Check if application current user is authenticated
		IUOLMessengerAuthenticationManagerPtr pAuthenticationManager = m_pUOLMessengerFacade->GetAuthenticationManager();

		if (pAuthenticationManager && pAuthenticationManager->IsAuthenticated())
		{
			// Retrieve profile authentication data and update UOL account password...
			IUOLMessengerProfileManagerPtr pProfileManager = m_pUOLMessengerFacade->GetProfileManager();

			if (pProfileManager != NULL)
			{
				CString strPath;
				IUOLMessengerProfileElementPtr pProfileElement;

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Getting profile [%s].", pProfileManager->GetCurrentProfile());
				
				pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);

				if (pProfileElement)
				{
					CString strAccountUserId = pProfileElement->GetProfileName();

					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogDebug(__FUNCTION__, 
							"Finding account for user [%s].", strAccountUserId);
					
					IUOLMessengerAccountPtr pAccount = FindAccount(strAccountUserId, _T("prpl-uol"));

					if (pAccount)
					{
						CString strAccountPassword = pProfileElement->GetProfilePassword();

						if (pAccount->IsSavePasswordEnabled())
						{
							pAccount->SetPassword(strAccountPassword);
						}
					}
					else
					{
						static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
								GetLogger()->LogError(__FUNCTION__, 
								"pAccount == NULL.");
					}
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogError(__FUNCTION__, 
							"pProfileElement == NULL.");
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"pProfileManager == NULL.");
			}
		}
		else if (pAuthenticationManager.get() == NULL)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pAuthenticationManager == NULL.");
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"pAuthenticationManager->IsAuthenticated() == FALSE.");
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


BOOL CAccountManager::LoadAccounts()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bRet = m_settingsHandler.LoadSettings(TRUE);

	/// <TODAOFERTA>
	// Force connection server
	for (POSITION pos = m_listAccounts.GetHeadPosition(); pos; m_listAccounts.GetNext(pos))
	{
		IUOLMessengerAccountPtr pAccount = m_listAccounts.GetAt(pos);

		CString strProtocolId = pAccount->GetProtocolId();

		if (0 == strProtocolId.Compare("prpl-uolto"))
		{
			CProtocolSettingsPtr pProtocolSettings = pAccount->GetProtocolSettings();

			if (pProtocolSettings)
			{
				UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

				if (pStringLoader)
				{
					CString strServer = pStringLoader->GetValue("uol.resource.domain.todaoferta");
					ATLASSERT(FALSE == strServer.IsEmpty());

					pProtocolSettings->SetString(JABBER_SETTING_CONNECT_SERVER, (LPCTSTR) strServer);
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"pProtocolSettings == NULL for protocol [%s] and user [%s].", 
						strProtocolId, pAccount->GetUserId());
			}
		}
	}
	/// </TODAOFERTA>
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);


	return bRet;
}


BOOL CAccountManager::SaveAccounts()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bRet = m_settingsHandler.SaveSettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bRet;
}


IUOLMessengerAccountPtr CAccountManager::CreateAccount(const CString& strUserId, const CString& strProtocolId)
{
	IUOLMessengerProtocolManagerPtr pUOLMessengerProtocolManager = m_pUOLMessengerFacade->GetProtocolManager();

	CIMProtocol* pProtocol = pUOLMessengerProtocolManager->GetProtocol(strProtocolId);

	if (pProtocol)
	{
		CAccountPtr pAccount = new CAccount();

		pAccount->SetProtocol(pProtocol);
		pAccount->SetUserId(strUserId);

		return pAccount;
	}

	return IUOLMessengerAccountPtr();
}


BOOL CAccountManager::AddAccount(IUOLMessengerAccountPtr pAccount)
{
	// Check if account already exists on list
	IUOLMessengerAccountPtr pFindAccount = FindAccount(pAccount->GetUserId(), pAccount->GetProtocolId());

	if (!pFindAccount)
	{
		m_listAccounts.AddTail(pAccount);

		m_eventListenerNotifier.NotifyAll(&CUOLMessengerAccountListEventListener::OnAddAccount, pAccount);

		return TRUE;
	}

	return FALSE;
}


BOOL CAccountManager::RemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	return RemoveAccount(pAccount->GetUserId(), pAccount->GetProtocolId());
}


BOOL CAccountManager::RemoveAccount(const CString& strUserId, const CString& strProtocolId)
{
	for (POSITION pos = m_listAccounts.GetHeadPosition(); pos; m_listAccounts.GetNext(pos))
	{
		IUOLMessengerAccountPtr pAccount = m_listAccounts.GetAt(pos);

		if ((pAccount != NULL) &&
			(0 == strUserId.Compare(pAccount->GetUserId())) &&
			(0 == strProtocolId.Compare(pAccount->GetProtocolId())))
		{
			CAccountPtr pDerivedAccount = ap_dynamic_cast<CAccountPtr>(pAccount);
			
			if (pDerivedAccount)
			{
				pDerivedAccount->Finalize();
			}

			m_listAccounts.RemoveAt(pos);

			m_eventListenerNotifier.NotifyAll(&CUOLMessengerAccountListEventListener::OnRemoveAccount, pAccount);

			// TODO: remove this list
			m_listRemovedAccounts.AddTail(pAccount);

			return TRUE;
		}
	}

	return FALSE;
}


IUOLMessengerAccountPtr CAccountManager::FindAccount(const CString& strUserId, const CString& strProtocolId)
{
	for (POSITION pos = m_listAccounts.GetHeadPosition(); pos;)
	{
		IUOLMessengerAccountPtr pAccount = m_listAccounts.GetNext(pos);
		if ((pAccount != NULL) &&
			(0 == strUserId.Compare(pAccount->GetUserId())) &&
			(0 == strProtocolId.Compare(pAccount->GetProtocolId())))
		{
				return pAccount;
		}
	}

	return IUOLMessengerAccountPtr();
}


void CAccountManager::GetAccountList(CAtlList<IUOLMessengerAccountPtr>& listAccounts)
{
	listAccounts.RemoveAll();

	for (POSITION pos = m_listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = m_listAccounts.GetNext(pos);
		listAccounts.AddTail(pAccount);
	}
}


int CAccountManager::GetAccountListCount()
{
	return m_listAccounts.GetCount();
}


BOOL CAccountManager::SetAccountProtocol(IUOLMessengerAccountPtr pAccount, const CString& strProtocolId)
{
	IUOLMessengerProtocolManagerPtr pUOLMessengerProtocolManager = m_pUOLMessengerFacade->GetProtocolManager();

	ATLASSERT(pUOLMessengerProtocolManager);

	CIMProtocol* pProtocol = pUOLMessengerProtocolManager->GetProtocol(strProtocolId);

	if (pProtocol)
	{
		CAccountPtr pChangeAccount = ap_dynamic_cast<CAccountPtr>(pAccount);

		if (pChangeAccount)
		{
			CString strOldProtocolId = pAccount->GetProtocolId();

			if (0 != strOldProtocolId.Compare(strProtocolId))
			{
				pChangeAccount->SetProtocol(pProtocol);

				m_eventListenerNotifier.NotifyAll(&CUOLMessengerAccountListEventListener::OnAccountProtocolChanged, pChangeAccount, strOldProtocolId);
			}

			return TRUE;
		}
	}

	return FALSE;
}


BOOL CAccountManager::SetAccountUserId(IUOLMessengerAccountPtr pAccount, const CString& strUserId)
{
	CAccountPtr pChangeAccount = ap_dynamic_cast<CAccountPtr>(pAccount);

	if (pChangeAccount)
	{
		CString strOldUserId = pAccount->GetUserId();

		if (0 != strOldUserId.Compare(strUserId))
		{
			pChangeAccount->SetUserId(strUserId);

			m_eventListenerNotifier.NotifyAll(&CUOLMessengerAccountListEventListener::OnAccountUserIdChanged, pChangeAccount, strOldUserId);
		}

		return TRUE;
	}

	return FALSE;
}


void CAccountManager::AddAccountListEventListener(CUOLMessengerAccountListEventListener* pListener)
{
	m_eventListenerNotifier.RegisterListener(pListener);
}


void CAccountManager::RemoveAccountListEventListener(CUOLMessengerAccountListEventListener* pListener)
{
	m_eventListenerNotifier.UnregisterListener(pListener);
}


void CAccountManager::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Account", NEW_SETTINGS_CALLBACK(AccountNode, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CAccountManager::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}


void CAccountManager::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}



IMPLEMENT_SETTINGS_CALLBACK(CAccountManager, AccountNode)
{
	CAccountPtr pAccount;

	if (bSave)
	{
		for (POSITION pos = m_pThis->m_listAccounts.GetHeadPosition(); pos;)
		{
			pAccount = ap_static_cast<CAccountPtr>(m_pThis->m_listAccounts.GetNext(pos));

			if (pAccount)
			{
				MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
				static_cast<ISettings*>(pAccount.get())->Save(pChild, strKey);
			}
		}

		for (POSITION pos = m_pThis->m_listUnboundAccounts.GetHeadPosition(); pos;)
		{
			pAccount = ap_static_cast<CAccountPtr>(m_pThis->m_listUnboundAccounts.GetNext(pos));

			if (pAccount)
			{
				MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
				static_cast<ISettings*>(pAccount.get())->Save(pChild, strKey);
			}
		}
	}
	else
	{
		pAccount = new CAccount();

		if (pAccount)
		{
			static_cast<ISettings*>(pAccount.get())->Load(pNode, strKey);

			// Divide accounts into 2 lists:
			// - account with some protocol object associated
			// - account with no protocol object associated
			// Drop accounts without protocol Id
			CIMProtocol* pProtocol = static_cast<IUOLMessengerAccount*>(pAccount.get())->GetProtocol();
			if (pProtocol != NULL)
			{
				m_pThis->m_listAccounts.AddTail(pAccount);
			}
			else
			{
				CString strProtocolId = static_cast<IUOLMessengerAccount*>(pAccount.get())->GetProtocolId();

				if (FALSE == strProtocolId.IsEmpty())
				{
					m_pThis->m_listUnboundAccounts.AddTail(pAccount);
				}
			}
		}
	}
}

