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
#include "PrivacyManager.h"
#include "../UIMApplication.h"


CPrivacyManager::CPrivacyManager()
{
}


CPrivacyManager::~CPrivacyManager()
{
}


BOOL CPrivacyManager::Init(IUOLMessengerFacade* pFacade)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (pFacade)
	{
		m_pFacade = pFacade;

		IUOLMessengerAccountManagerPtr pAccountManager = m_pFacade->GetAccountManager();

		if (pAccountManager)
		{
			pAccountManager->AddAccountListEventListener(this);

			CAtlList<IUOLMessengerAccountPtr> listAccounts;
			pAccountManager->GetAccountList(listAccounts);

			for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
			{
				IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
			
				m_mapPrivacyInfo.SetAt(pAccount, new CPrivacyInfo(pAccount));
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pAccountManager == NULL");
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return TRUE;
}


void CPrivacyManager::Finalize()
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

		m_mapPrivacyInfo.RemoveAll();

		m_pFacade = NULL;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CPrivacyManager::NotifyDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		pPrivacyInfo->AddDenied(strContact);

		// Notify observers...
		CPrivacyManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnDenyAdded, pAccount, strContact);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnDenyAdded, pAccount, strContact);
	}
}


void CPrivacyManager::NotifyPermitAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		pPrivacyInfo->AddPermited(strContact);

		// Notify observers...
		CPrivacyManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnPermitAdded, pAccount, strContact);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnPermitAdded, pAccount, strContact);
	}
}


void CPrivacyManager::NotifyDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		pPrivacyInfo->RemoveDenied(strContact);

		// Notify observers...
		CPrivacyManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnDenyRemoved, pAccount, strContact);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnDenyRemoved, pAccount, strContact);
	}
}


void CPrivacyManager::NotifyPermitRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		pPrivacyInfo->RemovePermited(strContact);

		// Notify observers...
		CPrivacyManagerObserverNotifierPtr pNotifyObserver = FindObserverNotifier(pAccount);

		if (pNotifyObserver)
		{
			pNotifyObserver->NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnPermitRemoved, pAccount, strContact);
		}

		m_observerNotifier.NotifyAll(&CUOLMessengerPrivacyManagerObserver::OnPermitRemoved, pAccount, strContact);
	}
}


int CPrivacyManager::GetProtocolPrivacyFlags(IUOLMessengerAccountPtr pAccount)
{
	ATLASSERT(FALSE);
	return 0;
}


BOOL CPrivacyManager::IsDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		return pPrivacyInfo->IsDenied(strContact);
	}

	return FALSE;
}


BOOL CPrivacyManager::IsPermited(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		return pPrivacyInfo->IsPermited(strContact);
	}

	return FALSE;
}



BOOL CPrivacyManager::AddDeniedContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();

		ATLASSERT(pProtocol != NULL);

		if (pProtocol != NULL)
		{
			if (FALSE == pPrivacyInfo->IsDenied(strContact))
			{
				error = pProtocol->AddDenyContact((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact);
			}
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CPrivacyManager::AddPermitedContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();

		ATLASSERT(pProtocol != NULL);

		if (pProtocol != NULL)
		{
			if (FALSE == pPrivacyInfo->IsPermited(strContact))
			{
				error = pProtocol->AddPermitContact((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact);
			}
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CPrivacyManager::RemoveDeniedContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();

		ATLASSERT(pProtocol != NULL);

		if (pProtocol != NULL)
		{
			if (pPrivacyInfo->IsDenied(strContact))
			{
				error = pProtocol->RemoveDenyContact((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact);
			}
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CPrivacyManager::RemovePermitedContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();

		ATLASSERT(pProtocol != NULL);

		if (pProtocol != NULL)
		{
			if (pPrivacyInfo->IsPermited(strContact))
			{
				error = pProtocol->RemovePermitContact((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact);
			}
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


void CPrivacyManager::GetDeniedList(IUOLMessengerAccountPtr pAccount, CAtlList<CString>& listContacts)
{
	CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

	if (pPrivacyInfo)
	{
		pPrivacyInfo->GetDeniedList(listContacts);
	}
}


void CPrivacyManager::GetPermitedList(IUOLMessengerAccountPtr pAccount, CAtlList<CString>& listContacts)
{
    CAtlList<IUOLMessengerContactPtr> listContactNode;
	IUOLMessengerContactPtr pContact;
	CString strContact;

    listContacts.RemoveAll();

	CIMProtocol* pProtocol = pAccount->GetProtocol();
	ATLASSERT(pProtocol != NULL);

	if ((pProtocol != NULL) && 
		(pProtocol->GetPrivacyFlags() & PRIVACY_FLAGS_HAS_PERMIT_LIST))
	{
		CPrivacyInfoPtr pPrivacyInfo = FindPrivacyInfo(pAccount);

		if (pPrivacyInfo)
		{
			pPrivacyInfo->GetPermitedList(listContacts);
		}
	}
	else
	{
		m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pProtocol->GetId().c_str(), listContactNode);

		for (POSITION pos = listContactNode.GetHeadPosition(); pos; )
		{
			pContact = listContactNode.GetNext(pos);
			strContact = pContact->GetName();

			listContacts.AddTail(strContact);
		}
	}
}


void CPrivacyManager::RegisterObserver(CUOLMessengerPrivacyManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CPrivacyManager::UnregisterObserver(CUOLMessengerPrivacyManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CPrivacyManager::RegisterAccountObserver(IUOLMessengerAccountPtr pAccount, CUOLMessengerPrivacyManagerObserver* pObserver)
{
	if ((!pAccount) || (!pObserver))
	{
		return;
	}

	CPrivacyManagerObserverNotifierPtr pObserverNotifier;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountObserverNotifier);

		pObserverNotifier = FindObserverNotifier(pAccount);

		if (!pObserverNotifier)
		{
			pObserverNotifier = new CPrivacyManagerObserverNotifier();
			m_mapAccountObserverNotifier.SetAt(pAccount, pObserverNotifier);
		}
	}

	pObserverNotifier->RegisterObserver(pObserver);
}


void CPrivacyManager::UnregisterAccountObserver(IUOLMessengerAccountPtr pAccount, CUOLMessengerPrivacyManagerObserver* pObserver)
{
	if ((!pAccount) || (!pObserver))
	{
		return;
	}

	CPrivacyManagerObserverNotifierPtr pObserverNotifier;

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


void CPrivacyManager::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
	m_mapPrivacyInfo.SetAt(pAccount, new CPrivacyInfo(pAccount));
}


void CPrivacyManager::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	CPrivacyManagerObserverNotifierPtr pObserverNotifier;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountObserverNotifier);

		m_mapAccountObserverNotifier.RemoveKey(pAccount);
	}

	m_mapPrivacyInfo.RemoveKey(pAccount);
}


CPrivacyInfoPtr CPrivacyManager::FindPrivacyInfo(IUOLMessengerAccountPtr pAccount)
{
	CPrivacyInfoMap::CPair* pPair = m_mapPrivacyInfo.Lookup(pAccount);

	if (pPair)
	{
		return pPair->m_value;
	}

	return CPrivacyInfoPtr();
}


CPrivacyManagerObserverNotifierPtr CPrivacyManager::FindObserverNotifier(IUOLMessengerAccountPtr pAccount)
{
	CObserverNotifierMap::CPair* pPair = m_mapAccountObserverNotifier.Lookup(pAccount);

	if (pPair)
	{
		return pPair->m_value;
	}

	return CPrivacyManagerObserverNotifierPtr();
}