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
#include "ContactList.h"

#include "UOLMessengerContactTraits.h"


void CContactListObserverNotifier::NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerContactPtr), IUOLMessengerContactPtr pContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);;

	CUOLMessengerContactListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CUOLMessengerContactListObserver*)))
		{
			(pObserver->*pfnUpdate)(pContact);
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
}


void CContactListObserverNotifier::NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerContactPtr, const CString&), IUOLMessengerContactPtr pContact, const CString& strParam)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CUOLMessengerContactListObserver*)))
		{
			(pObserver->*pfnUpdate)(pContact, strParam);
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
}


void CContactListObserverNotifier::NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerGroupPtr), IUOLMessengerGroupPtr pGroup)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);;

	CUOLMessengerContactListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CUOLMessengerContactListObserver*)))
		{
			(pObserver->*pfnUpdate)(pGroup);
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
}


void CContactListObserverNotifier::NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerGroupPtr, const CString&), IUOLMessengerGroupPtr pGroup, const CString& strParam)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CUOLMessengerContactListObserver*)))
		{
			(pObserver->*pfnUpdate)(pGroup, strParam);
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
}


void CContactListObserverNotifier::NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(CAtlList<IUOLMessengerContactPtr>&), CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CUOLMessengerContactListObserver*)))
		{
			(pObserver->*pfnUpdate)(listContacts);
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
}


void CContactListObserverNotifier::NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)())
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CUOLMessengerContactListObserver*)))
		{
			(pObserver->*pfnUpdate)();
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
}









typedef CAtlMap<IUOLMessengerContactPtr, IUOLMessengerContactPtr, CUOLMessengerContactTraits> CUOLMessengerContactMap;



CContactList::CContactList()
{
}


CContactList::~CContactList()
{
	ATLTRACE(_T("%s"), __FUNCTION__);

	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		CGroupPtr pGroup = m_listGroups.GetNext(pos);

		pGroup->RemoveAllContacts();
	}

	m_listGroups.RemoveAll();
}


IUOLMessengerContactPtr CContactList::CreateContact(IUOLMessengerAccountPtr pAccount, const CString& strUser)
{
	CContactPtr pContact = new CContact(pAccount, strUser);

	return pContact;
}


BOOL CContactList::AddContact(IUOLMessengerContactPtr pContact, const CString& strGroup)
{
	BOOL bAdded = FALSE;

	if ((pContact != NULL) &&
		(!strGroup.IsEmpty()))
	{
		CContactPtr pNewContact = ap_dynamic_cast<CContactPtr>(pContact);

		if (pNewContact != NULL)
		{
			BOOL bNewGroup = FALSE;
			CGroupPtr pGroup;

			{
				CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

				pGroup = InternalFindGroup(strGroup);

				if (!pGroup)
				{
					bNewGroup = TRUE;
					pGroup = InternalAddGroup(strGroup);
				}

				bAdded = InternalAddContact(pNewContact, pGroup);
			}

			if (bNewGroup)
			{
				m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnAddGroup, pGroup);
			}

			if (bAdded)
			{
				m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnAddContact, pNewContact);
			}
		}
	}

	return bAdded;
}


IUOLMessengerContactPtr CContactList::AddContact(IUOLMessengerAccountPtr pAccount, const CString& strUser, const CString& strGroup)
{
	if ((pAccount != NULL) &&
		(!strUser.IsEmpty()) &&
		(!strGroup.IsEmpty()))
	{
		CContactPtr pContact = new CContact(pAccount, strUser);

		BOOL bAdded = FALSE;
		BOOL bNewGroup = FALSE;
		CGroupPtr pGroup;

		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

			pGroup = InternalFindGroup(strGroup);

			if (!pGroup)
			{
				bNewGroup = TRUE;
				pGroup = InternalAddGroup(strGroup);
			}

			bAdded = InternalAddContact(pContact, pGroup);
		}

		if (bNewGroup)
		{
			m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnAddGroup, pGroup);
		}

		if (bAdded)
		{
			m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnAddContact, pContact);

			return pContact;
		}
	}

	return IUOLMessengerContactPtr();
}


IUOLMessengerContactPtr CContactList::RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	if ((pAccount != NULL) && (!strContact.IsEmpty()))
	{
		return RemoveContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);
	}

	return IUOLMessengerContactPtr();
}


IUOLMessengerContactPtr CContactList::RemoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact)
{
	IUOLMessengerContactPtr pRemovedContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
		{
			CGroupPtr pGroup = m_listGroups.GetNext(pos);

			IUOLMessengerContactPtr pContact = pGroup->RemoveContact(strAccountUserId, strAccountProtocolId, strContact);

			if (pContact != NULL)
			{
				pRemovedContact = pContact;
				break;
			}
		}
	}

	if (pRemovedContact != NULL)
	{
		m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnRemoveContact, pRemovedContact);
	}

	return pRemovedContact;
}


IUOLMessengerContactPtr CContactList::RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup)
{
	if ((pAccount != NULL) && 
		(!strContact.IsEmpty()) && 
		(!strGroup.IsEmpty()))
	{
		return RemoveContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, strGroup);
	}

	return IUOLMessengerContactPtr();
}


IUOLMessengerContactPtr CContactList::RemoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, const CString& strGroup)
{	
	IUOLMessengerContactPtr pRemovedContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		CGroupPtr pGroup = InternalFindGroup(strGroup);

		if (pGroup != NULL)
		{
			IUOLMessengerContactPtr pContact = pGroup->RemoveContact(strAccountUserId, strAccountProtocolId, strContact);

			if (pContact != NULL)
			{
				pRemovedContact = pContact;
			}
		}
	}

	if (pRemovedContact)
	{
		m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnRemoveContact, pRemovedContact);
	}

	return pRemovedContact;
}


IUOLMessengerContactPtr CContactList::FindContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	if ((pAccount != NULL) && (!strContact.IsEmpty()))
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		return InternalFindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);
	}

	return IUOLMessengerContactPtr();
}


IUOLMessengerContactPtr CContactList::FindContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

	return InternalFindContact(strAccountUserId, strAccountProtocolId, strContact);
}


IUOLMessengerContactPtr CContactList::FindContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup)
{
	IUOLMessengerContactPtr pContact;

	if ((pAccount != NULL) && (!strContact.IsEmpty()) && (!strGroup.IsEmpty()))
	{
		pContact = FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, strGroup);
	}

	return pContact;
}


IUOLMessengerContactPtr CContactList::FindContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, const CString& strGroup)
{
	IUOLMessengerContactPtr pContact;

	if ((!strAccountUserId.IsEmpty()) && (!strAccountProtocolId.IsEmpty()) &&
		(!strContact.IsEmpty()) && (!strGroup.IsEmpty()))
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		CGroupPtr pGroup = InternalFindGroup(strGroup);

		if (pGroup != NULL)
		{
			pContact = pGroup->FindContact(strAccountUserId, strAccountProtocolId, strContact);
		}
	}

	return pContact;
}


int CContactList::FindContacts(IUOLMessengerAccountPtr pAccount, const CString& strContact, CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if ((pAccount != NULL) && (!strContact.IsEmpty()))
	{
		return FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, listContacts);
	}

	return 0;
}


int CContactList::FindContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if ((!strAccountUserId.IsEmpty()) && (!strAccountProtocolId.IsEmpty()) &&
		(!strContact.IsEmpty()))
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		return InternalFindContacts(strAccountUserId, strAccountProtocolId, strContact, listContacts);
	}

	return 0;
}


int CContactList::FindContacts(IUOLMessengerAccountPtr pAccount, CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if (pAccount)
	{
		return FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), listContacts);
	}

	return 0;
}


int CContactList::FindContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if ((!strAccountUserId.IsEmpty()) && (!strAccountProtocolId.IsEmpty()))
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		return InternalFindContacts(strAccountUserId, strAccountProtocolId, listContacts);
	}

	return 0;
}



void CContactList::RemoveAllContacts(IUOLMessengerAccountPtr pAccount)
{
	if (pAccount != NULL)
	{
		RemoveAllContacts(pAccount->GetUserId(), pAccount->GetProtocolId());
	}
}


void CContactList::RemoveAllContacts(const CString& strAccountUserId, const CString& strAccountProtocolId)
{
	CAtlList<IUOLMessengerContactPtr> listRemovedContacts;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
		{
			CGroupPtr pGroup = m_listGroups.GetAt(pos);

			CAtlList<CContactPtr> listGroupContacts;
			pGroup->RemoveAllContacts(strAccountUserId, strAccountProtocolId, listGroupContacts);

			for (POSITION posRemoved = listGroupContacts.GetHeadPosition(); posRemoved; )
			{
				IUOLMessengerContactPtr pContact = listGroupContacts.GetNext(posRemoved);

				listRemovedContacts.AddTail(pContact);
			}

			m_listGroups.GetNext(pos);
		}
	}

	m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnRemoveContacts, listRemovedContacts);
}


BOOL CContactList::MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strGroup)
{
	if ((pAccount != NULL) &&
		(!strContact.IsEmpty()) &&
		(!strGroup.IsEmpty()))
	{
		return MoveContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact, strOldGroup,strGroup);
	}

	return FALSE;
}


BOOL CContactList::MoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, const CString& strOldGroup, const CString& strGroup)
{
	BOOL bMoved = FALSE;

	CGroupPtr pOldGroup;
	CContactPtr pRemovedContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		pOldGroup = InternalFindGroup(strOldGroup);

		if (pOldGroup != NULL)
		{
			CContactPtr pContact = pOldGroup->FindContact(strAccountUserId, strAccountProtocolId, strContact);

			if (pContact != NULL)
			{
				CGroupPtr pNewGroup = InternalFindGroup(strGroup);

				if (pNewGroup != NULL)
				{
					pRemovedContact = pOldGroup->RemoveContact(strAccountUserId, strAccountProtocolId, strContact);

					if (pRemovedContact != NULL)
					{
						bMoved = pNewGroup->AddContact(pRemovedContact);

						if (bMoved)
						{
							pContact->SetGroup(pNewGroup);
						}
					}
				}
			}
		}
	}

	if (bMoved)
	{
		m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnMoveContact, pRemovedContact, pOldGroup->GetName());
	}

	return bMoved;
}


IUOLMessengerGroupPtr CContactList::AddGroup(const CString& strGroup)
{
	IUOLMessengerGroupPtr pNewGroup;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		pNewGroup = InternalFindGroup(strGroup);
		if (pNewGroup != NULL)
		{
			return pNewGroup;
		}

		pNewGroup = InternalAddGroup(strGroup);
	}

	if (pNewGroup != NULL)
	{
		m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnAddGroup, pNewGroup);
	}

	return pNewGroup;
}


IUOLMessengerGroupPtr CContactList::RemoveGroup(const CString& strGroup)
{
	CGroupPtr pRemovedGroup;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		for (POSITION pos = m_listGroups.GetHeadPosition(); pos; m_listGroups.GetNext(pos))
		{
			CGroupPtr pGroup = m_listGroups.GetAt(pos);

			if (0 == strGroup.CompareNoCase(pGroup->GetName()))
			{
				m_listGroups.RemoveAt(pos);

				pRemovedGroup = pGroup;
				pRemovedGroup->SetActive(FALSE);
				break;
			}
		}
	}

	if (pRemovedGroup != NULL)
	{
		m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnRemoveGroup, pRemovedGroup);
	}

	return pRemovedGroup;
}


IUOLMessengerGroupPtr CContactList::FindGroup(const CString& strGroup)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

	return InternalFindGroup(strGroup);
}


BOOL CContactList::RenameGroup(const CString& strOldGroup, const CString& strNewGroup)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

	IUOLMessengerGroupPtr pNewGroup = InternalFindGroup(strNewGroup);

	if (pNewGroup != NULL)
	{
		// Group name already exists
		return FALSE;
	}

	IUOLMessengerGroupPtr pGroup = InternalFindGroup(strOldGroup);

	if (pGroup != NULL)
	{
		pGroup->SetName(strNewGroup);
		pGroup->NotifyAll();
	}

	return TRUE;
}


void CContactList::RemoveAllGroups()
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

		POSITION posCur;
		for (POSITION pos = m_listGroups.GetHeadPosition(); pos;)
		{
			CGroupPtr pGroup = m_listGroups.GetAt(pos);
			pGroup->SetActive(FALSE);

			posCur = pos;
			m_listGroups.GetNext(pos);

			pGroup->RemoveAllContacts();
			
			m_listGroups.RemoveAt(posCur);			
		}
	}

	m_ObserverNotifier.NotifyObservers(&CUOLMessengerContactListObserver::OnRemoveAllGroups);
}


void CContactList::GetGroupList(CAtlList<IUOLMessengerGroupPtr>& listGroups)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csGroups);

	listGroups.RemoveAll();

	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		listGroups.AddTail(m_listGroups.GetNext(pos));
	}
}


void CContactList::RegisterObserver(CUOLMessengerContactListObserver* pObserver)
{
	m_ObserverNotifier.RegisterObserver(pObserver);
}


void CContactList::UnregisterObserver(CUOLMessengerContactListObserver* pObserver)
{
	m_ObserverNotifier.UnregisterObserver(pObserver);
}


BOOL CContactList::InternalAddContact(CContactPtr pContact, CGroupPtr pGroup)
{
	BOOL bRet = pGroup->AddContact(pContact);
	if (bRet)
	{
		pContact->SetGroup(pGroup);
	}

	return bRet;
}


CGroupPtr CContactList::InternalAddGroup(const CString& strGroup)
{
	/*CGroupPtr pNewGroup = new CGroup(strGroup);

	m_listGroups.AddTail(pNewGroup);
	pNewGroup->SetActive(TRUE);

	return pNewGroup;*/

	// Add group into a sortted position
	POSITION pos;
	POSITION itemPos;
	POSITION tailPos = m_listGroups.GetTailPosition();
	CString strListItem;
	CGroupPtr pNewGroup = new CGroup(strGroup);

	//Insert first item to the list
	if (m_listGroups.GetCount() == 0)
	{
		m_listGroups.AddTail(pNewGroup);
	}
	else
	{
		pos = m_listGroups.GetHeadPosition();
		//Insert others itens to the list			
		while (pos)
		{
			//I have to do this because 'GetNext' updates 'pos'
			itemPos = pos;
			strListItem = m_listGroups.GetNext(pos)->GetName();
			if ((strGroup.Compare(strListItem) <= 0))
			{
				m_listGroups.InsertBefore(itemPos, pNewGroup);
				break;
			}
			else if (pos == NULL)
			{
				m_listGroups.AddTail(pNewGroup);
				break;
			}

		}
	}
	pNewGroup->SetActive(TRUE);

	return pNewGroup;
}


CContactPtr CContactList::InternalFindContact(const CString& strAccountUserId, const CString& strAccountProtcolId, const CString& strContact)
{
	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		CGroupPtr pGroup = m_listGroups.GetNext(pos);

		CContactPtr pContact = pGroup->FindContact(strAccountUserId, strAccountProtcolId, strContact);

		if (pContact != NULL)
		{
			return pContact;
		}
	}

	return CContactPtr();
}


int CContactList::InternalFindContacts(const CString& strAccountUserId, const CString& strAccountProtcolId, const CString& strContact, CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	listContacts.RemoveAll();

	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		CGroupPtr pGroup = m_listGroups.GetNext(pos);

		CContactPtr pContact = pGroup->FindContact(strAccountUserId, strAccountProtcolId, strContact);

		if (pContact != NULL)
		{
			listContacts.AddTail(pContact);
		}
	}

	return (int) listContacts.GetCount();
}


int CContactList::InternalFindContacts(const CString& strAccountUserId, const CString& strAccountProtcolId, CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	listContacts.RemoveAll();

	CUOLMessengerContactMap mapContacts;

	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		CGroupPtr pGroup = m_listGroups.GetNext(pos);

		CAtlList<CContactPtr> listGroupContacts;

		if (pGroup->FindContacts(strAccountUserId, strAccountProtcolId, listGroupContacts))
		{
			CUOLMessengerContactMap::CPair* pPair;

			for (POSITION posContact = listGroupContacts.GetHeadPosition(); posContact; )
			{
				CContactPtr pContact = listGroupContacts.GetNext(posContact);

				pPair = mapContacts.Lookup(pContact);
				if (!pPair)
				{
					mapContacts.SetAt(pContact, pContact);
				}
			}
		}
	}
	
	CUOLMessengerContactMap::CPair* pPair;
	for (POSITION posMap = mapContacts.GetStartPosition(); posMap; )
	{
		pPair = mapContacts.GetNext(posMap);

		listContacts.AddTail(pPair->m_value);
	}

	return (int) listContacts.GetCount();
}



CGroupPtr CContactList::InternalFindGroup(const CString& strGroup)
{
	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		CGroupPtr pGroup = m_listGroups.GetNext(pos);

		if (0 == strGroup.CompareNoCase(pGroup->GetName()))
		{
			return pGroup;
		}
	}

	return CGroupPtr();
}
