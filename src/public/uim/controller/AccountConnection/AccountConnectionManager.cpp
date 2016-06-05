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
#include "AccountConnectionManager.h"

#include "AccountUserStatusHelper.h"
#include "../../view/TextInputLimits.h"
#include "../../model/Account.h"
#include "../../model/AccountProxy.h"
#include "../UIMApplication.h"



CAccountConnectionManager::CAccountConnectionManager() :
	m_pFacade(NULL)
{
}


CAccountConnectionManager::~CAccountConnectionManager()
{
}


BOOL CAccountConnectionManager::Init(IUOLMessengerFacade* pFacade)
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (pFacade)
	{
		m_pFacade = pFacade;

		IUOLMessengerAccountManagerPtr pAccountManager = m_pFacade->GetAccountManager();

		if (pAccountManager)
		{
			pAccountManager->AddAccountListEventListener(this);

			bRet = TRUE;
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pAccountManager == NULL");
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CAccountConnectionManager::PreFinalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	//DisconnectAll(FALSE);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CAccountConnectionManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pFacade = [%x]", m_pFacade);
	
	if (m_pFacade)
	{
		IUOLMessengerAccountManagerPtr pAccountManager = m_pFacade->GetAccountManager();

		if (pAccountManager)
		{
			pAccountManager->RemoveAccountListEventListener(this);
		}

		m_mapAccountConnection.RemoveAll();
		m_mapAccountObserverNotifier.RemoveAll();
		m_observerNotifier.UnregisterAll();

		m_pFacade = NULL;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CAccountConnectionManager::NotifyConnected(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		// Update connection state...
		pAccountConnection->SetConnectionState(IUOLMessengerAccountConnectionManager::CS_CONNECTED);

		// Update display icon and alias
		InternalUploadDisplayIcon(pAccountConnection, TRUE);
		InternalUpdateServerAlias(pAccountConnection, TRUE);

		// TODO: set status with timer
		//::Sleep(1000);

		// Update status...
		DWORD dwCurrentUserStatus = pAccountConnection->GetCurrentUserStatus();

		if ((dwCurrentUserStatus != IUOLMessengerAccountConnectionManager::AS_AWAY) && 
			(dwCurrentUserStatus != IUOLMessengerAccountConnectionManager::AS_INVISIBLE) &&
			(dwCurrentUserStatus != IUOLMessengerAccountConnectionManager::AS_BUSY))
		{
			InternalSetStatus(pAccountConnection, IUOLMessengerAccountConnectionManager::AS_ONLINE, TRUE);
		}
		else
		{
			InternalSetStatus(pAccountConnection, dwCurrentUserStatus, TRUE);
		}

		// Notify observers...
		CAccountConnectionManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnConnected, pAccount);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnConnected, pAccount);
	}
}


void CAccountConnectionManager::NotifyDisconnected(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		pAccountConnection->SetConnectionState(IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);

		InternalSetStatus(pAccountConnection, IUOLMessengerAccountConnectionManager::AS_OFFLINE);

		// Notify observers...
		CAccountConnectionManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnDisconnected, pAccount);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnDisconnected, pAccount);
	}
}


void CAccountConnectionManager::NotifyReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError errorCode, const CString& strMessage)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		pAccountConnection->SetConnectionState(IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);
		pAccountConnection->SetPreviousUserStatus(pAccountConnection->GetCurrentUserStatus());

		if ((FALSE == pAccount->IsSavePasswordEnabled()) ||
			(errorCode == CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID))
		{
			// User may have entered worng password, reset password
			pAccount->SetPassword(_T(""));
		}

		// Notify observers...
		CAccountConnectionManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnReportDisconnect, pAccount, errorCode, strMessage);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnReportDisconnect, pAccount, errorCode, strMessage);
	}
}


int CAccountConnectionManager::GetConnectedAccountsCount()
{
	int nConnectedCount = 0;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountConnections);

	CAccountConnectionMap::CPair* pPair = NULL;

	for (POSITION pos = m_mapAccountConnection.GetStartPosition(); pos; )
	{
		pPair = m_mapAccountConnection.GetNext(pos);

		CAccountConnectionPtr pAccountConnection = pPair->m_value;

		if (pAccountConnection->IsConnected())
		{
			nConnectedCount++;
		}
	}

	return nConnectedCount;
}


void CAccountConnectionManager::GetConnectedAccountList(CAtlList<IUOLMessengerAccountPtr>& listAccounts)
{
	listAccounts.RemoveAll();

	ATLASSERT(m_pFacade);
	m_pFacade->GetAccountManager()->GetAccountList(listAccounts);

	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetAt(pos);

		if (FALSE == IsConnected(pAccount))
		{
			POSITION posCur = pos;
			listAccounts.GetNext(pos);
			listAccounts.RemoveAt(posCur);
		}
		else
		{
			listAccounts.GetNext(pos);
		}
	}
}


BOOL CAccountConnectionManager::Connect(IUOLMessengerAccountPtr pAccount)
{
	if (!pAccount)
	{
		return FALSE;
	}

	// Do connection...
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	if (!pProtocol)
	{
		return FALSE;
	}

	ATLTRACE(_T("%s - Connecting account %s\n"), __FUNCTION__, pAccount->GetUserId());

	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (!pAccountConnection)
	{
		pAccountConnection = CreateAccountConnection(pAccount);
	}

	// Retrieve account password...
	CString strPassword = pAccount->GetPassword();

	if (strPassword.IsEmpty())
	{
		BOOL bSavePassword;

		CUOLMessengerUserInputHandlerPtr pUserInputHandler = m_pFacade->GetUIManager()->GetDefaultUserInputHandler();

		if (pUserInputHandler)
		{
			RequestResult result;

			while (strPassword.IsEmpty())
			{
				result = pUserInputHandler->RequestPassword(pAccount, strPassword, TIL_ACCOUNT_PASSWORD, bSavePassword);

				if ((result == REQUEST_RESULT_CANCEL) ||
					(result == REQUEST_RESULT_ERROR))
				{
					pAccountConnection->SetConnectionState(IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);

					InternalSetStatus(pAccountConnection, IUOLMessengerAccountConnectionManager::AS_OFFLINE);

					ATLTRACE(_T("%s - user canceled password input\n"), __FUNCTION__);
					return FALSE;
				}
			}
		}

		pAccount->EnableSavePassword(bSavePassword);
		pAccount->SetPassword(strPassword);
	}

	// Do connection...
	pAccountConnection->SetConnectionState(IUOLMessengerAccountConnectionManager::CS_CONNECTING);

	IMProtocolError error;

	// Get extra settings and proxy setttings...
	CProtocolSettingsPtr pExtraSettings = pAccount->GetProtocolSettings();

	IUOLMessengerNetworkSettingsPtr pNetworkSettings = m_pFacade->GetSettingsManager()->GetNetworkSettings();

	int nProxyType = pNetworkSettings->GetProxyType();

	if (nProxyType != IUOLMessengerNetworkSettings::PT_PROXY_NONE)
	{
		// Connect without proxy...
		CAccountProxyInfo proxyInfo(nProxyType, pNetworkSettings->GetProxyHost(), pNetworkSettings->GetProxyPort(), pNetworkSettings->GetProxyUsername(), pNetworkSettings->GetProxyPassword());

		error = pProtocol->Connect((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strPassword, pExtraSettings.get(), &proxyInfo);
	}
	else
	{
		// Connect throught proxy
		error = pProtocol->Connect((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strPassword, pExtraSettings.get(), NULL);
	}

	if (error == IM_PROTOCOL_ERROR_SUCCESS)
	{
		return TRUE;
	}

	// Probably already connected
	ATLASSERT(error == IM_PROTOCOL_ERROR_ALREADY_CONNECTED);

	return FALSE;
}


BOOL CAccountConnectionManager::Disconnect(IUOLMessengerAccountPtr pAccount)
{
	return Disconnect(pAccount, TRUE);
}


BOOL CAccountConnectionManager::IsConnected(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		return pAccountConnection->IsConnected();
	}

	return FALSE;
}


DWORD CAccountConnectionManager::GetConnectionState(IUOLMessengerAccountPtr pAccount) 
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		return pAccountConnection->GetConnectionState();
	}

	return IUOLMessengerAccountConnectionManager::CS_DISCONNECTED;
}


void CAccountConnectionManager::ConnectAll(BOOL bAutoConnectOnly)
{
	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pFacade->GetAccountManager()->GetAccountList(listAccounts);

	for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

		if ((pAccount != NULL) && (FALSE == IsConnected(pAccount)))
		{
			if (!bAutoConnectOnly || pAccount->IsAutoConnectEnabled())
			{
				Connect(pAccount);
			}
		}
	}
}


void CAccountConnectionManager::DisconnectAll()
{
	DisconnectAll(FALSE);
}

void CAccountConnectionManager::SetStatus(IUOLMessengerAccountPtr pAccount, DWORD dwStatus)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (!pAccountConnection)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Creating AccountConnection...");
		
		pAccountConnection = CreateAccountConnection(pAccount);
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "AccountConnection found...");
	}

	InternalSetStatus(pAccountConnection, dwStatus);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


DWORD CAccountConnectionManager::GetStatus(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		return pAccountConnection->GetCurrentUserStatus();
	}

	return IUOLMessengerAccountConnectionManager::AS_OFFLINE;
}


DWORD CAccountConnectionManager::GetPreviousStatus(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		return pAccountConnection->GetPreviousUserStatus();
	}

	return IUOLMessengerAccountConnectionManager::AS_OFFLINE;
}


void CAccountConnectionManager::UpdateServerAlias(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (!pAccountConnection)
	{
		return;
	}

	InternalUpdateServerAlias(pAccountConnection);
}


BOOL CAccountConnectionManager::UploadDisplayIcon(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if ((!pAccountConnection) ||
		(FALSE == pAccountConnection->IsConnected()))
	{
		return FALSE;
	}

	return InternalUploadDisplayIcon(pAccountConnection);
}


void CAccountConnectionManager::AddReceivedAway(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		pAccountConnection->AddReceivedAway(strContact);
	}
}


BOOL CAccountConnectionManager::IsReceivedAway(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	BOOL bRet = FALSE;

	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		bRet = pAccountConnection->IsReceivedAway(strContact);
	}

	return bRet;
}


void CAccountConnectionManager::ResetReceivedAway(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if (pAccountConnection)
	{
		pAccountConnection->ResetReceivedAway();
	}
}


void CAccountConnectionManager::SetIdleAll(UINT nSeconds)
{
	ATLTRACE(_T("Reporting idle time %d\n"), nSeconds);

	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountConnections);

	for (POSITION pos = m_mapAccountConnection.GetStartPosition(); pos; )
	{
		CAccountConnectionPtr pAccountConnection = m_mapAccountConnection.GetNextValue(pos);

		if (pAccountConnection)
		{
			IUOLMessengerAccountPtr pAccount = pAccountConnection->GetAccount();

			if (pAccount)
			{
				CIMProtocol* pProtocol = pAccount->GetProtocol();

				if (pProtocol)
				{
					pProtocol->SetIdle((LPCTSTR) pAccount->GetUserId(), nSeconds);
				}
			}
		}
	}
}


void CAccountConnectionManager::RegisterObserver(CUOLMessengerAccountConnectionManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CAccountConnectionManager::UnregisterObserver(CUOLMessengerAccountConnectionManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CAccountConnectionManager::RegisterAccountObserver(IUOLMessengerAccountPtr pAccount, CUOLMessengerAccountConnectionManagerObserver* pObserver)
{
	if ((!pAccount) || (!pObserver))
	{
		return;
	}

	CAccountConnectionManagerObserverNotifierPtr pObserverNotifier;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountObserverNotifier);

		pObserverNotifier = FindObserverNotifier(pAccount);

		if (!pObserverNotifier)
		{
			pObserverNotifier = new CAccountConnectionManagerObserverNotifier();
			m_mapAccountObserverNotifier.SetAt(pAccount, pObserverNotifier);
		}
	}

	pObserverNotifier->RegisterObserver(pObserver);
}


void CAccountConnectionManager::UnregisterAccountObserver(IUOLMessengerAccountPtr pAccount, CUOLMessengerAccountConnectionManagerObserver* pObserver)
{
	if ((!pAccount) || (!pObserver))
	{
		return;
	}

	CAccountConnectionManagerObserverNotifierPtr pObserverNotifier;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountObserverNotifier);

		pObserverNotifier = FindObserverNotifier(pAccount);

		if (pObserverNotifier)
		{
			if (0 == pObserverNotifier->GetObserverCount())
			{
				m_mapAccountObserverNotifier.RemoveKey(pAccount);
			}
		}
	}

	if (pObserverNotifier)
	{
		pObserverNotifier->UnregisterObserver(pObserver);
	}
}


BOOL CAccountConnectionManager::UpdateExtendedCapabilities(IUOLMessengerAccount2Ptr pAccount)
{
	CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

	if ((!pAccountConnection) ||
		(FALSE == pAccountConnection->IsConnected()))
	{
		return FALSE;
	}

	return InternalUpdateExtendedCapabilities(pAccountConnection);
}


void CAccountConnectionManager::GetAccountValidStatus(
		IUOLMessengerAccountPtr pAccount, CAtlList<int>& listValidStatus)
{
	if (pAccount)
	{
		CString strProtocolId = pAccount->GetProtocolId();
		
		CAccountUserStatusHelper::GetProtocolValidStatus(strProtocolId, listValidStatus);
	}
}


void CAccountConnectionManager::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
}


void CAccountConnectionManager::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	CAccountConnectionManagerObserverNotifierPtr pObserverNotifier;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountObserverNotifier);

		m_mapAccountObserverNotifier.RemoveKey(pAccount);
	}

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountConnections);

		m_mapAccountConnection.RemoveKey(pAccount);
	}
}


CAccountConnectionPtr CAccountConnectionManager::FindAccountConnection(IUOLMessengerAccountPtr pAccount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountConnections);

	CAccountConnectionMap::CPair* pPair = m_mapAccountConnection.Lookup(pAccount);

	if (pPair)
	{
		return pPair->m_value;
	}

	return CAccountConnectionPtr();
}


CAccountConnectionPtr CAccountConnectionManager::CreateAccountConnection(IUOLMessengerAccountPtr pAccount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountConnections);

	CAccountConnectionPtr pAccountConnection = new CAccountConnection(pAccount);
	
	m_mapAccountConnection.SetAt(pAccount, pAccountConnection);

	return pAccountConnection;
}


int CAccountConnectionManager::GetProtocolUserStatus(CIMProtocol* pProtocol, DWORD dwUserStatus)
{
	CString strProtocolId = pProtocol->GetId().c_str();

	return CAccountUserStatusHelper::GetProtocolUserStatus(strProtocolId, dwUserStatus);
}


CAccountConnectionManagerObserverNotifierPtr CAccountConnectionManager::FindObserverNotifier(IUOLMessengerAccountPtr pAccount)
{
	CObserverNotifierMap::CPair* pPair = m_mapAccountObserverNotifier.Lookup(pAccount);

	if (pPair)
	{
		return pPair->m_value;
	}

	return CAccountConnectionManagerObserverNotifierPtr();
}


void CAccountConnectionManager::InternalSetStatus(CAccountConnectionPtr pAccountConnection, DWORD dwStatus, BOOL bAsync)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerAccountPtr pAccount = pAccountConnection->GetAccount();

	CIMProtocol* pProtocol = pAccount->GetProtocol();

	if (!pProtocol)
	{
		return;
	}

	// First we need to make a mapping from the desired status to 
	// one that is valid for this protocol.
	//
	DWORD dwValidStatus = 0;
	CAccountUserStatusHelper::GetProtocolValidStatusMapping(
			pProtocol->GetId().c_str(), dwStatus, dwValidStatus);

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pAccount->UserId = [%s], dwStatus = [%d], dwValidStatus = [%d]", 
			pAccount->GetUserId(), dwStatus, dwValidStatus);
	
	if (dwStatus != dwValidStatus)
	{
		if (IUOLMessengerAccountConnectionManager::AS_ONLINE == dwValidStatus)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogInfo(__FUNCTION__, "Connect()");

			Connect(pAccount);
			
			return;
		}
		else if (IUOLMessengerAccountConnectionManager::AS_OFFLINE == dwValidStatus)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogInfo(__FUNCTION__, "Disconnect()");

			Disconnect(pAccount);
			
			return;
		}
	}
	
	// If we get to this, we will proceed with normal flux of events.
	//
	DWORD dwCurrentStatus = pAccountConnection->GetCurrentUserStatus();

	if ((dwValidStatus == IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY) && 
		((dwCurrentStatus == IUOLMessengerAccountConnectionManager::AS_INVISIBLE) ||
		 (dwCurrentStatus == IUOLMessengerAccountConnectionManager::AS_OFFLINE)))	
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Invisible and offline don't respond to auto-away.");

		// Do not change status to away if user is invisible or offline
		return;
	}

	DWORD dwNewStatus;

	if (dwValidStatus == IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY_RETURN)
	{
		// Restore user status before becoming away automatically
		dwNewStatus = pAccountConnection->GetPreviousUserStatus();
	}
	else
	{
		dwNewStatus = dwValidStatus;
		pAccountConnection->SetPreviousUserStatus(dwCurrentStatus);
	}

	pAccountConnection->SetCurrentUserStatus(dwNewStatus);

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"dwNewStatus = [%d], dwCurrentStatus = [%d]", dwNewStatus, dwCurrentStatus);

	if (dwNewStatus != IUOLMessengerAccountConnectionManager::AS_OFFLINE)
	{
		if (pAccountConnection->IsConnected())
		{
			int nProtocolStatus = GetProtocolUserStatus(pProtocol, dwNewStatus);
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"nProtocolStatus = [%d]", nProtocolStatus);

			IUOLMessengerAccount3Ptr pAccount3 = ap_dynamic_cast<IUOLMessengerAccount3Ptr>(pAccount);
			CIMProtocol6* pProtocol6 = dynamic_cast<CIMProtocol6*>(pProtocol);
			if (pProtocol6 && pAccount3)
			{
				if (!bAsync)
				{
					IMProtocolError error = pProtocol6->SetUserStatus((LPCTSTR) pAccount3->GetUserId(), nProtocolStatus, (LPCTSTR) pAccount3->GetStatusMessage());
					ATLASSERT(error == IM_PROTOCOL_ERROR_SUCCESS);
				}
				else
				{
					IMProtocolError error = pProtocol6->SetUserStatusAsync((LPCTSTR) pAccount3->GetUserId(), nProtocolStatus, (LPCTSTR) pAccount3->GetStatusMessage());
					ATLASSERT( (error == IM_PROTOCOL_ERROR_SUCCESS) || (error == IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING) );
				}
			}
		}
	}

	pAccountConnection->ResetReceivedAway();

	// Notify observers...
	CAccountConnectionManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

	if (pNotifyObserver)
	{
		pNotifyObserver->NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnStatusChanged, pAccount);
	}

	m_observerNotifier.NotifyAll(&CUOLMessengerAccountConnectionManagerObserver::OnStatusChanged, pAccount);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}



void CAccountConnectionManager::InternalUpdateServerAlias(CAccountConnectionPtr pAccountConnection, BOOL bAsync)
{
	IUOLMessengerAccountPtr pAccount = pAccountConnection->GetAccount();

	if (pAccountConnection->IsConnected())
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();
	
		ATLASSERT(pProtocol != NULL);
		if (pProtocol != NULL)
		{
			if (!bAsync)
			{
				pProtocol->SetAlias((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) pAccount->GetNickName());
			}
			else
			{
				CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocol);
				
				if (pProtocol5)
				{
					pProtocol5->SetAliasAsync((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) pAccount->GetNickName());
				}
			}
		}
	}
}


BOOL CAccountConnectionManager::InternalUploadDisplayIcon(CAccountConnectionPtr pAccountConnection, BOOL bAsync)
{
	IUOLMessengerAccountPtr pAccount = pAccountConnection->GetAccount();

	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CIMProtocol* pProtocol = pAccount->GetProtocol();
	
	ATLASSERT(pProtocol != NULL);
	
	if ( (pProtocol != NULL) && (pProtocol->HasUserDisplayIconSupport()) )
	{
		IUOLMessengerUserIconPtr pUserIcon = pAccount->GetUserIcon();

		if (pUserIcon != NULL)
		{
			CString strFilePath = pUserIcon->GetImagePath();

			if (!strFilePath.IsEmpty())
			{
				if (!bAsync)
				{
					error = pProtocol->SetUserDisplayIcon((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strFilePath);
				}
				else
				{
					CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocol);
					
					if (pProtocol5)
					{
						error = pProtocol5->SetUserDisplayIconAsync((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strFilePath);
					}
				}

				ATLASSERT( (error == IM_PROTOCOL_ERROR_SUCCESS) || (error == IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING) );
			}
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CAccountConnectionManager::InternalUpdateExtendedCapabilities(CAccountConnectionPtr pAccountConnection)
{
	IUOLMessengerAccountPtr pAccount = pAccountConnection->GetAccount();

	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CIMProtocol* pProtocol = pAccount->GetProtocol();
	
	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		CIMProtocol7* pProtocol7 = dynamic_cast<CIMProtocol7*>(pProtocol);
		IUOLMessengerAccount2Ptr pAccount2 = ap_dynamic_cast<IUOLMessengerAccount2Ptr>(pAccount);

		if (pProtocol7 && pAccount2)
		{
			CProtocolSettings2Ptr pExtendedCapabilities = pAccount2->GetExtendedCapabilities();

			error = pProtocol7->SetExtendedCapabilitiesAsync(
					(LPCTSTR) pAccount->GetUserId(), pExtendedCapabilities.get());

			// Removed because at the app ending this won't be executed.
			//
			//ATLASSERT(error == IM_PROTOCOL_ERROR_SUCCESS);
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);

}


void CAccountConnectionManager::DisconnectAll(BOOL bMakeItAsync)
{
	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pFacade->GetAccountManager()->GetAccountList(listAccounts);

	for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

		if ((pAccount != NULL) && (IsConnected(pAccount)))
		{
			Disconnect(pAccount, bMakeItAsync);
		}
	}
}


BOOL CAccountConnectionManager::Disconnect(IUOLMessengerAccountPtr pAccount, BOOL bMakeItAsync)
{
	if (!pAccount)
	{
		return FALSE;
	}

	CIMProtocol* pProtocol = pAccount->GetProtocol();

	if (pProtocol != NULL)
	{
		IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;
		
		if (!bMakeItAsync)
		{
			error = pProtocol->Disconnect((LPCTSTR) pAccount->GetUserId());
		}
		else
		{
			CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocol);
			
			if (pProtocol5)
			{
				error = pProtocol5->DisconnectAsync((LPCTSTR) pAccount->GetUserId());
			}
		}
		
		if (error == IM_PROTOCOL_ERROR_SUCCESS)
		{
			// Disconnect operation started
			CAccountConnectionPtr pAccountConnection = FindAccountConnection(pAccount);

			ATLASSERT(pAccountConnection);
			if (pAccountConnection)
			{
				pAccountConnection->SetConnectionState(IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);
			
				return TRUE;
			}
		}

		ATLASSERT( (error == IM_PROTOCOL_ERROR_NOT_CONNECTED) || (error == IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING) );
	}

	return FALSE;
}

