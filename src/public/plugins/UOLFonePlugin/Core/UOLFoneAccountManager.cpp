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
#include "UOLFoneAccountManager.h"

#include "../resource.h"
#include "../FacadeWrapper.h"
#include "UOLFoneController.h"



CUOLFoneAccountManager::CUOLFoneAccountManager()
{
}


CUOLFoneAccountManager::~CUOLFoneAccountManager()
{
}


BOOL CUOLFoneAccountManager::Initialize()
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

		m_pCurrentAccount = NULL;
	}

	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade)
	{
		IUOLMessengerAccountManagerPtr pAccountManager = pFacade->GetAccountManager();

		if (pAccountManager)
		{
			pAccountManager->AddAccountListEventListener(this);
		}

		return TRUE;
	}

	return FALSE;
}


void CUOLFoneAccountManager::Finalize()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade)
	{
		IUOLMessengerAccountManagerPtr pAccountManager = pFacade->GetAccountManager();

		if (pAccountManager)
		{
			pAccountManager->RemoveAccountListEventListener(this);
		}

		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = pFacade->GetAccountConnectionManager();

		if (pAccountConnectionManager && m_pCurrentAccount)
		{
			pAccountConnectionManager->UnregisterAccountObserver(m_pCurrentAccount->GetIMAccount(), this);
		}
	}

	m_pCurrentAccount = NULL;
}


CUOLFoneAccountPtr CUOLFoneAccountManager::GetCurrentAccount()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

	return m_pCurrentAccount;
}


CUOLFoneAccountPtr CUOLFoneAccountManager::SetCurrentAccount(IUOLMessengerAccountPtr pIMAccount, BOOL bNotifyObservers)
{
	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (!pIMAccount || !pFacade)
	{
		return CUOLFoneAccountPtr();
	}

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = pFacade->GetAccountConnectionManager();

	if (!pAccountConnectionManager)
	{
		return CUOLFoneAccountPtr();
	}

	CUOLFoneAccountPtr pOldAccount;
	CUOLFoneAccountPtr pNewAccount;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

		pOldAccount = m_pCurrentAccount;

		if (pOldAccount)
		{
			IUOLMessengerAccountPtr pOldIMAccount = pOldAccount->GetIMAccount();

			if (pOldIMAccount.get() != pIMAccount.get())
			{
				m_pCurrentAccount = NULL;

				pAccountConnectionManager->UnregisterAccountObserver(pOldIMAccount, this);
			}
			else
			{
				// Same IM account, return existent account
				return pOldAccount;
			}
		}

		pNewAccount = new CUOLFoneAccount(pIMAccount);

		pAccountConnectionManager->RegisterAccountObserver(pIMAccount, this);

		m_pCurrentAccount = pNewAccount;
	}

	if (bNotifyObservers)
	{
		NotifyAccountChange(pOldAccount);
	}

	return pNewAccount;
}


void CUOLFoneAccountManager::ResetCurrentAccount()
{
	CUOLFoneAccountPtr pOldAccount;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

		pOldAccount = m_pCurrentAccount;
		m_pCurrentAccount = CUOLFoneAccountPtr();
	}
}


void CUOLFoneAccountManager::GetIMAccountList(CAtlList<IUOLMessengerAccountPtr>& listAccounts)
{
	listAccounts.IsEmpty();

	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade)
	{
		IUOLMessengerAccountManagerPtr pAccountManager = pFacade->GetAccountManager();

		if (pAccountManager)
		{
			CString strUOLFoneProtocolId;
			strUOLFoneProtocolId.LoadString(IDS_UOL_PROTOCOL_ID);

			CAtlList<IUOLMessengerAccountPtr> listAllAccounts;
			pAccountManager->GetAccountList(listAllAccounts);

			for (POSITION pos = listAllAccounts.GetHeadPosition(); pos; )
			{
				IUOLMessengerAccountPtr pAccount = listAllAccounts.GetNext(pos);

				if (0 == strUOLFoneProtocolId.Compare(pAccount->GetProtocolId()))
				{
					listAccounts.AddTail(pAccount);
				}
			}
		}
	}
}


IUOLMessengerAccountPtr CUOLFoneAccountManager::GetIMAccount(const CString& strUserId)
{
	IUOLMessengerAccountPtr pAccount;

	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade)
	{
		IUOLMessengerAccountManagerPtr pAccountManager = pFacade->GetAccountManager();

		if (pAccountManager)
		{
			CString strUOLFoneProtocolId;
			strUOLFoneProtocolId.LoadString(IDS_UOL_PROTOCOL_ID);

			pAccount = pAccountManager->FindAccount(strUserId, strUOLFoneProtocolId);
		}
	}

	return pAccount;
}


IUOLMessengerAccountPtr CUOLFoneAccountManager::GetCurrentIMAccount()
{
	IUOLMessengerAccountPtr pIMAccount;
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (pAccount)
	{
		pIMAccount = pAccount->GetIMAccount();
	}

	return pIMAccount;
}


BOOL CUOLFoneAccountManager::IsIMAccountConnected(IUOLMessengerAccountPtr pIMAccount)
{
	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade)
	{
		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = pFacade->GetAccountConnectionManager();

		if (pAccountConnectionManager)
		{
			return pAccountConnectionManager->IsConnected(pIMAccount);
		}
	}

	return FALSE;
}


void CUOLFoneAccountManager::RegisterObserver(IUOLFoneAccountManagerObserver* pObserver)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	POSITION posFind = m_listObservers.Find(pObserver);

	if (!posFind)
	{
		m_listObservers.AddTail(pObserver);
	}
}


void CUOLFoneAccountManager::UnregisterObserver(IUOLFoneAccountManagerObserver* pObserver)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	POSITION posFind = m_listObservers.Find(pObserver);

	if (posFind)
	{
		m_listObservers.RemoveAt(posFind);
	}
}


void CUOLFoneAccountManager::NotifyAccountChange(CUOLFoneAccountPtr pOldAccount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	IUOLFoneAccountManagerObserver* pObserver = NULL;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);
		pObserver->OnUOLFoneAccountChanged(pOldAccount);
	}
}


void CUOLFoneAccountManager::OnAddAccount(IUOLMessengerAccountPtr pIMAccount)
{
	CUOLFoneAccountPtr pOldAccount;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

		pOldAccount = m_pCurrentAccount;
	}
	
	if (NULL == pOldAccount)
	{
		CAtlList<IUOLMessengerAccountPtr> listIMAccounts;

		GetIMAccountList(listIMAccounts);

		if (listIMAccounts.GetCount() == 1)
		{
			CUOLFonePluginConfig::GetInstance()->SetCallAccount(pIMAccount->GetUserId());
			SetCurrentAccount(pIMAccount, TRUE);
		}
	}
}


void CUOLFoneAccountManager::OnRemoveAccount(IUOLMessengerAccountPtr pIMAccount)
{
	CUOLFoneAccountPtr pOldAccount;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csCurrentAccount);

		pOldAccount = m_pCurrentAccount;
	}

	if (pOldAccount != NULL)
	{
		IUOLMessengerAccountPtr pCurIMAccount;

		pCurIMAccount = pOldAccount->GetIMAccount();

		if ((pCurIMAccount != NULL) && 
			(pCurIMAccount->GetUserId() == pIMAccount->GetUserId()))
		{
			CAtlList<IUOLMessengerAccountPtr> listIMAccounts;

			GetIMAccountList(listIMAccounts);
			
			if (listIMAccounts.GetCount() > 0)
			{
				IUOLMessengerAccountPtr pFirstIMAccount;
				
				pFirstIMAccount = listIMAccounts.GetHead();

				CUOLFonePluginConfig::GetInstance()->SetCallAccount(pFirstIMAccount->GetUserId());

				SetCurrentAccount(pFirstIMAccount, FALSE);
			}
			else
			{
				ResetCurrentAccount();
			}
			
			NotifyAccountChange(pOldAccount);
		}
	}
}


void CUOLFoneAccountManager::OnAccountProtocolChanged(IUOLMessengerAccountPtr pIMAccount, const CString& strOldProtocolId)
{
	if (m_pCurrentAccount)
	{
		IUOLMessengerAccountPtr pCurIMAccount = m_pCurrentAccount->GetIMAccount();

		if (pIMAccount.get() == pCurIMAccount.get())
		{
			CString strUOLProtocolId;
			strUOLProtocolId.LoadString(IDS_UOL_PROTOCOL_ID);

			if (0 != strUOLProtocolId.Compare(pIMAccount->GetProtocolId()))
			{
				NotifyAccountChange(m_pCurrentAccount);
			}
		}
	}
}


void CUOLFoneAccountManager::OnAccountUserIdChanged(IUOLMessengerAccountPtr pIMAccount, const CString& strOldUserId)
{
	//NotifyAccountChange(m_pCurrentAccount);
}


void CUOLFoneAccountManager::OnConnected(IUOLMessengerAccountPtr pIMAccount)
{
	if (!CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
	{
		CUOLFoneController::GetInstance()->Connect();
	}
	else
	{
		CUOLFoneController::GetInstance()->PublishCapabilities(pIMAccount);
	}
}


void CUOLFoneAccountManager::OnDisconnected(IUOLMessengerAccountPtr pIMAccount)
{
	ATLTRACE(_T("%s"), __FUNCTION__);

	if (!CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
	{
		CUOLFoneController::GetInstance()->Disconnect();
	}
	else
	{
		CUOLFoneController::GetInstance()->RevokeCapabilities(pIMAccount);
	}
}


void CUOLFoneAccountManager::OnReportDisconnect(IUOLMessengerAccountPtr pIMAccount, ConnectionError error, const CString& strMessage)
{
	ATLTRACE(_T("%s"), __FUNCTION__);

	CUOLFoneController::GetInstance()->Disconnect();
}


BOOL CUOLFoneAccountManager::MatchIMAccount(CUOLFoneAccountPtr pAccount, IUOLMessengerAccountPtr pIMAccount)
{
	ATLASSERT(pIMAccount != NULL);

	if ((!pAccount) || (!pIMAccount))
	{
		return FALSE;
	}

	IUOLMessengerAccountPtr pTargetIMAccount = pAccount->GetIMAccount();

	if (!pTargetIMAccount)
	{
		return FALSE;
	}

	CString strTargetUserId = pTargetIMAccount->GetUserId();
	CString strTargetProtocolId = pTargetIMAccount->GetProtocolId();

	return ((0 == strTargetUserId.Compare(pIMAccount->GetUserId())) &&
			(0 == strTargetProtocolId.Compare(pIMAccount->GetProtocolId())));
}