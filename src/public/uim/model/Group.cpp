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
#include "Group.h"



#ifdef _LEAKDEBUG
	LONG CGroup::ms_nRefs = 0;
#endif



CGroup::CGroup(const CString& strName) : 
	m_strName(strName),
	m_bActive(TRUE)
{
#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CGroup::~CGroup()
{
#ifdef _LEAKDEBUG
	::InterlockedDecrement(&ms_nRefs);
	
	ATLTRACE(_T("%s: %d - %s\n"), __FUNCTION__, ms_nRefs, m_strName);
#endif

	ATLASSERT(m_listContacts.GetCount() == 0);

	m_listContacts.RemoveAll();
}


CString CGroup::GetName() const
{
	return m_strName;
}


void CGroup::SetName(const CString& strName)
{
	m_strName = strName;
}


IUOLMessengerContactPtr CGroup::GetContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IUOLMessengerContactPtr pContact;

	if (pAccount != NULL)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

		pContact = FindContactImpl(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);
	}

	return pContact;
}


void CGroup::GetContactList(CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	listContacts.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

		for (POSITION pos = m_listContacts.GetHeadPosition(); pos; )
		{
			listContacts.AddTail(m_listContacts.GetNext(pos));
		}
	}
}


UINT CGroup::GetContactCount()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

	return (UINT) m_listContacts.GetCount();
}


UINT CGroup::GetConnectedContactCount()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

	UINT nCount = 0;

	for (POSITION pos = m_listContacts.GetHeadPosition(); pos; )
	{
		IUOLMessengerContactPtr pContact = m_listContacts.GetNext(pos);
		
		if (pContact->GetStatus() != IUOLMessengerContact::CS_OFFLINE)
		{
			nCount++;
		}
	}

	return nCount;
}


void CGroup::RegisterObserver(CUOLMessengerGroupObserver* pObserver)
{
	if (pObserver != NULL)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		POSITION pos = m_listObservers.Find(pObserver);

		if (!pos)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CGroup::UnregisterObserver(CUOLMessengerGroupObserver* pObserver)
{
	if (pObserver != NULL)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		POSITION pos = m_listObservers.Find(pObserver);

		if (pos != NULL)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}


void CGroup::NotifyAll()
{
	if (IsActive())
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		CUOLMessengerGroupObserver* pObserver;

		for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
		{
			pObserver = m_listObservers.GetNext(pos);
			pObserver->Update(this);
		}
	}
}


void CGroup::SetActive(BOOL bActive)
{
	m_bActive = bActive;
}


BOOL CGroup::IsActive() const
{
	return m_bActive;
}


BOOL CGroup::AddContact(CContactPtr pContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

	CContactPtr pFindContact = FindContactImpl(pContact->GetAccount()->GetUserId(), pContact->GetAccount()->GetProtocolId(), pContact->GetName());

	if (!pFindContact)
	{
		m_listContacts.AddTail(pContact);
		pContact->SetActive(TRUE);

		return TRUE;
	}

	return FALSE;
}


CContactPtr CGroup::RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	return RemoveContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);
}


CContactPtr CGroup::RemoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

	for (POSITION pos = m_listContacts.GetHeadPosition(); pos; m_listContacts.GetNext(pos))
	{
		CContactPtr pContact = m_listContacts.GetAt(pos);

		if (Equals(pContact, strAccountUserId, strAccountProtocolId, strContact))
		{
			m_listContacts.RemoveAt(pos);
			pContact->SetActive(FALSE);

			return pContact;
		}
	}

	return CContactPtr();
}


CContactPtr CGroup::FindContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	return FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);
}


CContactPtr CGroup::FindContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

	return FindContactImpl(strAccountUserId, strAccountProtocolId, strContact);
}


int CGroup::FindContacts(IUOLMessengerAccountPtr pAccount, CAtlList<CContactPtr>& listContacts)
{
	if (pAccount)
	{
		return FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), listContacts);
	}

	return 0;
}


int CGroup::FindContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, CAtlList<CContactPtr>& listContacts)
{
	listContacts.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

		for (POSITION pos = m_listContacts.GetHeadPosition(); pos; )
		{
			CContactPtr pContact = m_listContacts.GetNext(pos);

			if ((0 == strAccountUserId.Compare(pContact->GetAccount()->GetUserId())) &&
				(0 == strAccountProtocolId.Compare(pContact->GetAccount()->GetProtocolId())))
			{
				listContacts.AddTail(pContact);
			}
		}
	}

	return (int) listContacts.GetCount();
}


void CGroup::RemoveAllContacts(IUOLMessengerAccountPtr pAccount, CAtlList<CContactPtr>& listRemovedContacts)
{
	RemoveAllContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), listRemovedContacts);
}


void CGroup::RemoveAllContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, CAtlList<CContactPtr>& listRemovedContacts)
{
	listRemovedContacts.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

		for (POSITION pos = m_listContacts.GetHeadPosition(); pos;)
		{
			CContactPtr pContact = m_listContacts.GetAt(pos);

			if ((0 == strAccountUserId.Compare(pContact->GetAccount()->GetUserId())) &&
				(0 == strAccountProtocolId.Compare(pContact->GetAccount()->GetProtocolId())))
			{
				POSITION posCur = pos;
				m_listContacts.GetNext(pos);
				m_listContacts.RemoveAt(posCur);

				pContact->SetActive(FALSE);
				listRemovedContacts.AddTail(pContact);
			}
			else
			{
				m_listContacts.GetNext(pos);
			}
		}
	}
}


void CGroup::RemoveAllContacts()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csContacts);

	m_listContacts.RemoveAll();
}


BOOL CGroup::Equals(CContactPtr pContact, const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact)
{
	return ((0 == strContact.Compare(pContact->GetName())) &&
			(0 == strAccountUserId.Compare(pContact->GetAccount()->GetUserId())) &&
			(0 == strAccountProtocolId.Compare(pContact->GetAccount()->GetProtocolId())));
}


CContactPtr CGroup::FindContactImpl(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact)
{
	for (POSITION pos = m_listContacts.GetHeadPosition(); pos; )
	{
		CContactPtr pContact = m_listContacts.GetNext(pos);

		if (Equals(pContact, strAccountUserId, strAccountProtocolId, strContact))
		{
			return pContact;
		}
	}

	return CContactPtr();
}