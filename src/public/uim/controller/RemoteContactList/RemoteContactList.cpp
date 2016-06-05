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
#include "RemoteContactList.h"

#include "ContactUserStatusHelper.h"
#include "../../view/UserIcon.h"
#include "../../resource.h"
#include "../../model/Contact.h"
#include "../UIMApplication.h"



CRemoteContactList::CRemoteContactList() :
	m_pFacade(NULL)
{
}


CRemoteContactList::~CRemoteContactList()
{
}


BOOL CRemoteContactList::Init(IUOLMessengerFacade* pFacade)
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

			IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = m_pFacade->GetAccountConnectionManager();
			
			if (pAccountConnectionManager)
			{
				pAccountConnectionManager->RegisterObserver(this);

				bRet = TRUE;
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, "pAccountConnectionManager == NULL");
			}
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


void CRemoteContactList::Finalize()
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

		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = m_pFacade->GetAccountConnectionManager();
			
		if (pAccountConnectionManager)
		{
			pAccountConnectionManager->UnregisterObserver(this);
		}

		m_pFacade = NULL;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CRemoteContactList::NotifyUserAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	BOOL bOnList = TRUE;

	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	if (!pContact)
	{
		pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
		bOnList = FALSE;
	}

	m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnNotifyAdded, pContact, bOnList);
}


void CRemoteContactList::NotifyContactListUpdate(IUOLMessengerAccountPtr pAccount, const CContactInfo* pContactInfo, const CString& strGroup)
{
	// Add to local contact list
	IUOLMessengerContactListPtr pContactList = m_pFacade->GetContactList();
	IUOLMessengerContactPtr pContact;

	pContact = pContactList->FindContact(pAccount, pContactInfo->GetUser().c_str(), strGroup);

	if (pContact != NULL)
	{
		return;
	}

	pContact = pContactList->CreateContact(pAccount, pContactInfo->GetUser().c_str());

	ATLASSERT(pContact.get());
	if (!pContact)
	{
		return;
	}

	BOOL bAdded = pContactList->AddContact(pContact, strGroup);

	if (bAdded)
	{
		// Initialize contact...
		// Time
		pContact->SetAddedTime(COleDateTime::GetCurrentTime());

		// Alias
		CString strAlias = pContactInfo->GetAlias().c_str();
		if (!strAlias.IsEmpty())
		{
			pContact->SetAlias(strAlias);
		}
	
		// Status
		pContact->SetStatus(IUOLMessengerContact::CS_OFFLINE);

		// User icon
		IUOLMessengerUserIconPtr pUserIcon = m_pFacade->GetUserIconCacheManager()->GetCachedIcon(pContact.get());

		if (!pUserIcon)
		{
			CString protocolId = pAccount->GetProtocolId();

			IUOLMessengerImagesProtocolPtr pImagesProtocol = m_pFacade->GetUIManager()->GetImagesProtocol();
			ATLASSERT(pImagesProtocol.get()!=NULL);
			
			CString strIconPath = pImagesProtocol->GetContactImagePath(protocolId, 1);
			IUOLMessengerImagePtr pImage = pImagesProtocol->GetContactImage(protocolId, 1);

			pUserIcon = new CUserIcon(pImage, strIconPath);
		}

		pContact->SetUserIcon(pUserIcon);

		pContact->NotifyAll();

		// Notify observers...
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnContactAdded, pContact);
	}
}


void CRemoteContactList::NotifyContactRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup)
{
	IUOLMessengerContactPtr pRemovedContact = m_pFacade->GetContactList()->RemoveContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, strGroup);

	// Notify observers...
	if (pRemovedContact)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnContactRemoved, pRemovedContact, strGroup);
	}
}


void CRemoteContactList::NotifyContactMoved(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup)
{
	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, strOldGroup);

	if (pContact != NULL)
	{
		pContact->SetAddedTime(COleDateTime::GetCurrentTime());
	}

	BOOL bMoved = m_pFacade->GetContactList()->MoveContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, strOldGroup, strNewGroup);
	ATLASSERT(bMoved);

	if (bMoved)
	{
		// Notify observers...
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnContactMoved, pContact, strOldGroup);
	}
}


void CRemoteContactList::NotifyContactGotInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strInfo)
{
	// Notify observers...
	m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnContactGotInfo, pAccount, strContact, strInfo);
}


void CRemoteContactList::NotifyContactIconReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const char* pIconData, int nLen)
{
	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	BOOL bCached = FALSE;

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);
		
		if (pContact)
		{
			IUOLMessengerUserIconPtr pUserIcon;

			if (!bCached)
			{
				// Cache contact icon...
				CIMProtocol* pProtocol = pAccount->GetProtocol();
				ATLASSERT(pProtocol != NULL);

				if (pProtocol != NULL)
				{
					CString strNewFormat = m_pFacade->GetUserIconCacheManager()->GetPreferedIconFormat(pProtocol);

					pUserIcon = m_pFacade->GetUserIconCacheManager()->CacheIcon(pContact.get(), pIconData, nLen, strNewFormat);

					bCached = (pUserIcon != NULL);
				}
			}
			else
			{
				// Load icon from cache
				pUserIcon = m_pFacade->GetUserIconCacheManager()->GetCachedIcon(pContact.get());
			}

			if (pUserIcon != NULL)
			{
				pContact->SetUserIcon(pUserIcon);
				pContact->NotifyAllIconChange();

				//m_observerNotifier.NotifyAll(CUOLMessengerRemoteContactListObserver::OnContactIconReceived, pContact);
			}
		}
	}
}


void CRemoteContactList::NotifyContactAliasReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias)
{
	if (!strAlias.IsEmpty())
	{
		IMProtocolError imProtocolError;
		
		// Only ICQ need this reflection
		CString strProtocolId = pAccount->GetProtocolId();
		if (0 == strProtocolId.Compare("prpl-icq"))
		{
			AliasContact(pAccount, strContact, strAlias, imProtocolError, TRUE);
		}

		CAtlList<IUOLMessengerContactPtr> listContacts;

		m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

		for (POSITION pos = listContacts.GetHeadPosition(); pos; )
		{
			IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);

			if (pContact)
			{
				pContact->SetAlias(strAlias);
				pContact->NotifyAllAliasChange();

				//m_observerNotifier.NotifyAll(CUOLMessengerRemoteContactListObserver::OnContactAliasReceived, pContact);
			}
		}
	}
}


void CRemoteContactList::NotifyGroupAdded(IUOLMessengerAccountPtr pAccount, const CString& strGroup)
{
	IUOLMessengerGroupPtr pGroup = m_pFacade->GetContactList()->AddGroup(strGroup);

	if (pGroup)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnGroupAdded, pGroup);
		pGroup->NotifyAll();
	}
}


void CRemoteContactList::NotifyGroupRemoved(IUOLMessengerAccountPtr pAccount, const CString& strGroup)
{
	IUOLMessengerGroupPtr pGroup = m_pFacade->GetContactList()->RemoveGroup(strGroup);

	if (pGroup)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnGroupRemoved, pGroup);
		pGroup->NotifyAll();
	}
}


void CRemoteContactList::NotifyGroupRenamed(IUOLMessengerAccountPtr pAccount, const CString& strOldGroup, const CString& strNewGroup)
{
	IUOLMessengerContactListPtr pContactList = m_pFacade->GetContactList();

	IUOLMessengerGroupPtr pNewGroup = pContactList->FindGroup(strNewGroup);

	if (!pNewGroup)
	{
		pContactList->AddGroup(strNewGroup);
	}

	IUOLMessengerGroupPtr pOldGroup = pContactList->FindGroup(strOldGroup);

	if (pOldGroup != NULL)
	{
		CAtlList<IUOLMessengerContactPtr> listContacts;

		pOldGroup->GetContactList(listContacts);

		// Move old group contacts to new group
		for (POSITION pos = listContacts.GetHeadPosition(); pos; )
		{
			IUOLMessengerContactPtr pMoveContact = listContacts.GetNext(pos);
			if (pMoveContact->GetAccount().get() == pAccount.get())
			{
				pContactList->MoveContact(pMoveContact->GetAccount(), pMoveContact->GetName(), strOldGroup, strNewGroup);
			}
		}

		if (0 == pOldGroup->GetContactCount())
		{
			// Remove old group
			pContactList->RemoveGroup(strOldGroup);

			if (pNewGroup)
			{
				m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnGroupRenamed, pNewGroup, strOldGroup);
				pNewGroup->NotifyAll();
			}
		}
	}
}


void CRemoteContactList::NotifyContactSignedOn(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IUOLMessengerContactPtr pContact;
	IUOLMessengerContactPtr pNotifyContact;

	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	COleDateTime timeNow(COleDateTime::GetCurrentTime());
	IUOLMessengerConversationPtr pConversation;

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		pContact = listContacts.GetNext(pos);

		if ((pContact) &&
			(IUOLMessengerContact::CS_ONLINE != pContact->GetStatus()))
		{
			pContact->SetSignedOnTime(timeNow);

			// Update contact status
			pContact->SetStatus(IUOLMessengerContact::CS_ONLINE);
			pContact->NotifyAllStatusChange();

			if (!pNotifyContact)
			{
				pNotifyContact = pContact;
			}

			if (!pConversation)
			{
				pConversation = m_pFacade->GetConversationManager()->FindConversation(pContact); 
			}
		}
	}

	if (pConversation)
	{
		CString strMessage;
		strMessage.Format(IDS_MESSAGE_WINDOW_CONTACT_SIGNED_ON_FORMAT, pConversation->GetContact()->GetAlias());

		pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
	}

	if (pNotifyContact)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnContactSignedOn, pNotifyContact);
	}
}


void CRemoteContactList::NotifyContactSignedOff(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IUOLMessengerContactPtr pContact;
	IUOLMessengerContactPtr pNotifyContact;

	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	IUOLMessengerConversationPtr pConversation;

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		pContact = listContacts.GetNext(pos);

		if ((pContact) &&
			(IUOLMessengerContact::CS_OFFLINE != pContact->GetStatus()))
		{
			// Update contact status
			pContact->SetStatus(IUOLMessengerContact::CS_OFFLINE);
			pContact->NotifyAllStatusChange();

			if (!pNotifyContact)
			{
				pNotifyContact = pContact;
			}

			if (!pConversation)
			{
				pConversation = m_pFacade->GetConversationManager()->FindConversation(pContact); 
			}
		}
	}

	if (pConversation)
	{
		CString strMessage;
		strMessage.Format(IDS_MESSAGE_WINDOW_CONTACT_SIGNED_OFF_FORMAT, pConversation->GetContact()->GetAlias());

		pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
	}

	if (pNotifyContact)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnContactSignedOff, pNotifyContact);
	}
}


void CRemoteContactList::NotifyContactAway(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	UpdateContactStatus(pAccount, strContact, IUOLMessengerContact::CS_AWAY, &CUOLMessengerRemoteContactListObserver::OnContactAway);
}


void CRemoteContactList::NotifyContactAwayReturn(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	UpdateContactStatus(pAccount, strContact, IUOLMessengerContact::CS_ONLINE, &CUOLMessengerRemoteContactListObserver::OnContactAwayReturn);
}


void CRemoteContactList::NotifyContactIdle(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	UpdateContactStatus(pAccount, strContact, IUOLMessengerContact::CS_IDLE, &CUOLMessengerRemoteContactListObserver::OnContactIdle);
}


void CRemoteContactList::NotifyContactIdleReturn(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	UpdateContactStatus(pAccount, strContact, IUOLMessengerContact::CS_ONLINE, &CUOLMessengerRemoteContactListObserver::OnContactIdleReturn);
}


void CRemoteContactList::NotifyContactStatusChanged(IUOLMessengerAccountPtr pAccount, const CString& strContact, int nStatus, const CString& strStatusMessage)
{
	int nContactStatus = CContactUserStatusHelper::GetContactStatus(pAccount->GetProtocolId(), nStatus);

	UpdateContactStatus(pAccount, strContact, nContactStatus, NULL);
	UpdateContactStatusMessage(pAccount, strContact, strStatusMessage);
}


void CRemoteContactList::NotifyAddContactDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage)
{
	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	if (pContact != NULL)
	{
		static_cast<IUOLMessengerRemoteContactList*>(this)->RemoveContact(pAccount, strContact, pContact->GetGroup()->GetName());
	}

	m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnAddContactDenied, pAccount, strContact, strMessage);
}


void CRemoteContactList::NotifyContactAction(IUOLMessengerAccountPtr pAccount, NotifyReason reason, const CString& strContact)
{
	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	if (!pContact)
	{
		pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
	}

	m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver::OnNotifyContactAction, pContact, reason);
}


void CRemoteContactList::NotifyContactExtendedCapabilitiesChanged(IUOLMessengerAccountPtr pAccount, const CString& strContact, CProtocolSettings2Ptr pExtendedCapabilities)
{
	IUOLMessengerContactPtr pContact;
	IUOLMessengerContact2Ptr pContact2;
	IUOLMessengerContact2Ptr pNotifyContact2;
	CProtocolSettings2Ptr pOldExtendedCapabilities;

	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		pContact = listContacts.GetNext(pos);

		pContact2 = ap_dynamic_cast<IUOLMessengerContact2Ptr>(pContact);

		if (pContact2)
		{
			pOldExtendedCapabilities = pContact2->GetExtendedCapabilities();
			
			if (pOldExtendedCapabilities.get() != pExtendedCapabilities.get())
			{
				pContact2->SetExtendedCapabilities(pExtendedCapabilities);
				pContact2->NotifyAllExtendedCapabilitiesChange();
			}
		}
	}

	if (pNotifyContact2)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerRemoteContactListObserver2::OnContactExtendedCapabilitiesChanged, pNotifyContact2);
	}
}


void CRemoteContactList::NotifyContactCanRequestAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, const int bCanRequestAuthorization)
{
	CContactPtr pInnerContact;
	CAtlList<IUOLMessengerContactPtr> listContacts;
	
	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);
	
	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		pInnerContact = ap_dynamic_cast<CContactPtr>(listContacts.GetNext(pos));
		
		if (pInnerContact)
		{
			// Update contact "CanRequestAuthorization" flag.
			pInnerContact->SetCanRequestAuthorization(bCanRequestAuthorization);
		}
	}
}



BOOL CRemoteContactList::MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup, IMProtocolError& imProtocolError)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		imProtocolError = pProtocol->MoveContact((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact, (LPCTSTR) strOldGroup, (LPCTSTR) strNewGroup);
		
		return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
	}

	return FALSE;
}


BOOL CRemoteContactList::RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup, IMProtocolError& imProtocolError)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		imProtocolError = pProtocol->RemoveContact((LPCTSTR)pAccount->GetUserId(), (LPCTSTR)strContact, (LPCTSTR)strGroup);

		return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
	}

	return FALSE;
}


BOOL CRemoteContactList::AddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup, IMProtocolError& imProtocolError)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		imProtocolError = pProtocol->AddContact((LPCTSTR)pAccount->GetUserId(), (LPCTSTR)strContact, (LPCTSTR)strGroup);

		return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
	}

	return FALSE;
}


BOOL CRemoteContactList::AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias, IMProtocolError& imProtocolError)
{
	return AliasContact(pAccount, strContact, strAlias, imProtocolError, FALSE);
}


BOOL CRemoteContactList::AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias, IMProtocolError& imProtocolError, BOOL bAsync)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();
	
	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		if (!bAsync)
		{
			imProtocolError = pProtocol->AliasContact((LPCTSTR)pAccount->GetUserId(), (LPCTSTR)strContact, (LPCTSTR)strAlias);
		}
		else
		{
			CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocol);
			
			if (pProtocol5)
			{
				imProtocolError = pProtocol5->AliasContactAsync((LPCTSTR)pAccount->GetUserId(), (LPCTSTR)strContact, (LPCTSTR)strAlias);
			}
		}
		
		return ( (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS) || (imProtocolError == IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING) );
	}
	
	return FALSE;
}


BOOL CRemoteContactList::RenameGroup(const CString& strOldGroup, const CString& strNewGroup, IMProtocolError& imProtocolError)
{
	ATLASSERT(m_pFacade);

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pFacade->GetAccountConnectionManager()->GetConnectedAccountList(listAccounts);

	imProtocolError = IM_PROTOCOL_ERROR_FAILED;

	CIMProtocol* pProtocol;
	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

		pProtocol = pAccount->GetProtocol();

		ATLASSERT(pProtocol != NULL);
		if (pProtocol != NULL)
		{
			if (IM_PROTOCOL_ERROR_SUCCESS == pProtocol->RenameGroup((LPCTSTR)pAccount->GetUserId(), (LPCTSTR)strOldGroup, (LPCTSTR)strNewGroup))
			{
				imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
			}
		}
	}

	return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CRemoteContactList::RemoveGroup(const CString& strGroup, IMProtocolError& imProtocolError)
{
	ATLASSERT(m_pFacade);

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pFacade->GetAccountConnectionManager()->GetConnectedAccountList(listAccounts);

	imProtocolError = IM_PROTOCOL_ERROR_FAILED;

	CIMProtocol* pProtocol;
	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

		pProtocol = pAccount->GetProtocol();

		ATLASSERT(pProtocol != NULL);
		if (pProtocol != NULL)
		{			
			imProtocolError = pProtocol->RemoveGroup((LPCTSTR)pAccount->GetUserId(), (LPCTSTR)strGroup);			
		}
	}

	return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CRemoteContactList::GetContactInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		imProtocolError = pProtocol->GetContactInfo((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact);
		
		return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
	}

	return FALSE;
}


/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value */
BOOL CRemoteContactList::CanRequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		bool bCanRequest = false;
		/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value */
		imProtocolError = pProtocol->CanRequestContactAuthorization((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact, bCanRequest);
		
		return ((imProtocolError == IM_PROTOCOL_ERROR_SUCCESS) && bCanRequest);
	}

	return FALSE;
}


BOOL CRemoteContactList::RequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError)
{
	CIMProtocol* pProtocol = pAccount->GetProtocol();

	ATLASSERT(pProtocol != NULL);
	if (pProtocol != NULL)
	{
		imProtocolError = pProtocol->RequestContactAuthorization((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContact);
		
		return (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS);
	}

	return FALSE;
}


void CRemoteContactList::RegisterObserver(CUOLMessengerRemoteContactListObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CRemoteContactList::UnregisterObserver(CUOLMessengerRemoteContactListObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CRemoteContactList::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
}


void CRemoteContactList::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	m_pFacade->GetContactList()->RemoveAllContacts(pAccount);
}


void CRemoteContactList::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	m_pFacade->GetContactList()->RemoveAllContacts(pAccount);

	UINT nConnectingAccounts = 0;

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pFacade->GetAccountManager()->GetAccountList(listAccounts);
	
	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = m_pFacade->GetAccountConnectionManager();

	DWORD dwState;
	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pTempAccount = listAccounts.GetNext(pos);

		dwState = pAccountConnectionManager->GetConnectionState(pTempAccount);
		if (dwState != IUOLMessengerAccountConnectionManager::CS_DISCONNECTED)
		{
			nConnectingAccounts++;
		}
	}

	if (0 == nConnectingAccounts)
	{
		m_pFacade->GetContactList()->RemoveAllGroups();
	}
}


void CRemoteContactList::UpdateContactStatus(IUOLMessengerAccountPtr pAccount, const CString& strContact, DWORD dwNewStatus, void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerContactPtr))
{
	IUOLMessengerContactPtr pNotifyContact;

	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);

		if ((pContact) &&
			(pContact->GetStatus() != dwNewStatus))
		{
			pContact->SetStatus(dwNewStatus);
			pContact->NotifyAllStatusChange();

			pNotifyContact = pContact;
		}
	}

	if ((pNotifyContact) && (pfnCallback))
	{
		m_observerNotifier.NotifyAll(pfnCallback, pNotifyContact);
	}
}


void CRemoteContactList::UpdateContactStatusMessage(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strStatusMessage)
{
	IUOLMessengerContactPtr pNotifyContact;

	CAtlList<IUOLMessengerContactPtr> listContacts;

	m_pFacade->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);

	for (POSITION pos = listContacts.GetHeadPosition(); pos; )
	{
		IUOLMessengerContact4Ptr pContact4 = ap_dynamic_cast<IUOLMessengerContact4Ptr>(listContacts.GetNext(pos));

		if (pContact4)
		{
			pContact4->SetStatusMessage(strStatusMessage);
			pContact4->NotifyAllStatusChange();

			pNotifyContact = pContact4;
		}
	}
}
