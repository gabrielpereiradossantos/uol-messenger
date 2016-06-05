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
#include "ProtocolCallbackController.h"
#include "../UIMApplication.h"


CProtocolCallbackController::CProtocolCallbackController() :
	m_pFacade(NULL)
{
}


CProtocolCallbackController::~CProtocolCallbackController()
{
}


BOOL CProtocolCallbackController::Init(IUOLMessengerFacade* pFacade)
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (pFacade)
	{
		m_pFacade = pFacade;

		IUOLMessengerProtocolManagerPtr pProtocolManager = m_pFacade->GetProtocolManager();

		if (pProtocolManager)
		{
			// Register account protocol callbacks...
			IUOLMessengerAccountManagerPtr pAccountManager = m_pFacade->GetAccountManager();

			if (pAccountManager)
			{
				pAccountManager->AddAccountListEventListener(this);

				CAtlList<IUOLMessengerAccountPtr> listAccounts;
				pAccountManager->GetAccountList(listAccounts);

				for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
				{
					IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
					
					CIMProtocol* pProtocol = pAccount->GetProtocol();
					
					if (pProtocol)
					{
						CComCritSecLock<CComAutoCriticalSection> lock(m_csCallbacks);

						CProtocolCallbackSelectorPtr pProtocolCallback = new CProtocolCallbackSelector(pAccount);

						m_mapCallbackSelector.SetAt(pAccount, pProtocolCallback);
						
						pProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), pProtocolCallback.get());
					}
				}

				bRet = TRUE;
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, "pAccountManager == NULL");
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pProtocolManager == NULL");
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CProtocolCallbackController::Finalize()
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
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pAccountManager == NULL");
		}

		CAtlList<IUOLMessengerAccountPtr> listAccounts;
		pAccountManager->GetAccountList(listAccounts);

		for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
		{
			IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
					
			CIMProtocol* pProtocol = pAccount->GetProtocol();
					
			if (pProtocol)
			{
				pProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), NULL);
			}
		}

		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csCallbacks);

			m_mapCallbackSelector.RemoveAll();
		}

		m_pFacade = NULL;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CProtocolCallbackController::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csCallbacks);

	CProtocolCallbackSelectorMap::CPair* pPair = m_mapCallbackSelector.Lookup(pAccount);

	ATLASSERT(!pPair);
	if (pPair)
	{
		// Callback object should not already exists...
		m_mapCallbackSelector.RemoveKey(pAccount);
	}

	CIMProtocol* pProtocol = pAccount->GetProtocol();
		
	if (pProtocol)
	{
		CProtocolCallbackSelectorPtr pProtocolCallback = new CProtocolCallbackSelector(pAccount);

		m_mapCallbackSelector.SetAt(pAccount, pProtocolCallback);

		pProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), pProtocolCallback.get());
	}
}


void CProtocolCallbackController::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csCallbacks);

	CProtocolCallbackSelectorMap::CPair* pPair = m_mapCallbackSelector.Lookup(pAccount);

	ATLASSERT(pPair);
	if (pPair)
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();

		if (pProtocol)
		{
			pProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), NULL);
		}

		m_mapCallbackSelector.RemoveKey(pAccount);
	}
}


void CProtocolCallbackController::OnAccountProtocolChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldProtocolId)
{
	CProtocolCallbackSelectorPtr pCallbackSelector = FindProtocolCallbackSelector(strOldProtocolId, pAccount->GetUserId());

	if (pCallbackSelector)
	{
		CIMProtocol* pOldProtocol = m_pFacade->GetProtocolManager()->GetProtocol(strOldProtocolId);

		if (pOldProtocol)
		{
			pOldProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), NULL);
		}

		CIMProtocol* pProtocol = pAccount->GetProtocol();
		
		if (pProtocol)
		{
			pProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), pCallbackSelector.get());
		}
	}
}


void CProtocolCallbackController::OnAccountUserIdChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldUserId)
{
	CProtocolCallbackSelectorPtr pCallbackSelector = FindProtocolCallbackSelector(pAccount->GetProtocolId(), pAccount->GetUserId());

	if (pCallbackSelector)
	{
		CIMProtocol* pProtocol = pAccount->GetProtocol();
		
		if (pProtocol)
		{
			pProtocol->SetCallback((LPCTSTR) strOldUserId, NULL);
			pProtocol->SetCallback((LPCTSTR) pAccount->GetUserId(), pCallbackSelector.get());
		}
	}

}


CProtocolCallbackSelectorPtr CProtocolCallbackController::FindProtocolCallbackSelector(const CString& strProtocolId, const CString& strUserId)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csCallbacks);

	CProtocolCallbackSelectorPtr pCallbackSelector;

	for (POSITION pos = m_mapCallbackSelector.GetStartPosition(); pos; )
	{
		CProtocolCallbackSelectorPtr pTempCallbackSelector = m_mapCallbackSelector.GetNextValue(pos);

		IUOLMessengerAccountPtr pAccount = pTempCallbackSelector->GetAccount();

		if ((0 == strProtocolId.Compare(pAccount->GetProtocolId())) &&
			(0 == strUserId.Compare(pAccount->GetUserId())))
		{
			pCallbackSelector = pTempCallbackSelector;
			break;
		}			
	}

	return pCallbackSelector;
}