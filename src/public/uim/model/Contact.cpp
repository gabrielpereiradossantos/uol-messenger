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
#include "Contact.h"


CContactObserverNotifier::CContactObserverNotifier(IUOLMessengerContact3* pContact) :
	m_pContact(pContact)
{
}


void CContactObserverNotifier::NotifyAll(void (CUOLMessengerContactObserver::*pfnUpdate)(IUOLMessengerContact*))
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(m_pContact);
	}
}


void CContactObserverNotifier::NotifyAll(void (CUOLMessengerContactObserver2::*pfnUpdate)(IUOLMessengerContact2*))
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactObserver* pObserver;
	CUOLMessengerContactObserver2* pObserver2;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);

		if (pObserver)
		{
			pObserver2 = dynamic_cast<CUOLMessengerContactObserver2*>(pObserver);

			if (pObserver2)
			{
				(pObserver2->*pfnUpdate)(m_pContact);
			}
		}
	}
}


void CContactObserverNotifier::NotifyAll(void (CUOLMessengerContactObserver3::*pfnUpdate)(IUOLMessengerContact3*))
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	CUOLMessengerContactObserver* pObserver;
	CUOLMessengerContactObserver3* pObserver3;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);

		if (pObserver)
		{
			pObserver3 = dynamic_cast<CUOLMessengerContactObserver3*>(pObserver);

			if (pObserver3)
			{
				(pObserver3->*pfnUpdate)(m_pContact);
			}
		}
	}
}





#ifdef _LEAKDEBUG
	LONG CContact::ms_nRefs = 0;
#endif


CContact::CContact(IUOLMessengerAccountPtr pAcccount, const CString& strName) :
	m_pAccount(pAcccount),
	m_strName(strName), 
	m_strAlias(strName),
	m_dwStatus(IUOLMessengerContact::CS_OFFLINE),
	m_timeAdded(COleDateTime::GetCurrentTime()),
	m_timeSignedOn(COleDateTime::GetCurrentTime()),
	m_observerNotifier(this),
    m_bIsContact(TRUE),
	m_bCanRequestAuthorization(FALSE),
	m_strStatusMessage("")
{
	m_timeAdded.SetStatus(COleDateTime::invalid);
	m_timeSignedOn.SetStatus(COleDateTime::invalid);

#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CContact::CContact(IUOLMessengerAccountPtr pAcccount, const CString& strChatContactName, const CString& strChatContactAlias, const BOOL bIsContact) :
	m_pAccount(pAcccount),
	m_strName(strChatContactName), 
	m_strAlias(strChatContactAlias),
	m_dwStatus(IUOLMessengerContact::CS_UNKNOWN),
	m_timeAdded(COleDateTime::GetCurrentTime()),
	m_timeSignedOn(COleDateTime::GetCurrentTime()),
	m_observerNotifier(this),
    m_bIsContact(bIsContact),
	m_strStatusMessage("")
{
	m_timeAdded.SetStatus(COleDateTime::invalid);
	m_timeSignedOn.SetStatus(COleDateTime::invalid);

#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CContact::~CContact()
{
#ifdef _LEAKDEBUG
	::InterlockedDecrement(&ms_nRefs);
	
	ATLTRACE(_T("%s: %d - %s (%s)\n"), __FUNCTION__, ms_nRefs, m_strName, (m_pGroup != NULL) ? m_pGroup->GetName() : _T("no group"));
#endif

	m_pGroup.reset();
}


IUOLMessengerAccountPtr CContact::GetAccount() const
{
	return m_pAccount;
}


//void CContact::SetAccount(IUOLMessengerAccountPtr pAccount)
//{
//	m_pAccount = pAccount;
//}


CString CContact::GetName() const
{
	return m_strName;
}


//void CContact::SetName(const CString& strName)
//{
//	m_strName = strName;
//}


CString CContact::GetAlias() const
{
	return m_strAlias;
}


void CContact::SetAlias(const CString& strAlias)
{
	m_strAlias = strAlias;
}


IUOLMessengerGroupPtr CContact::GetGroup() const
{
	return m_pGroup;
}


void CContact::SetGroup(IUOLMessengerGroupPtr pGroup)
{
	m_pGroup = pGroup;
}


DWORD CContact::GetStatus() const
{
	return m_dwStatus;
}


void CContact::SetStatus(DWORD dwStatus)
{
	m_dwStatus = dwStatus;
}


IUOLMessengerUserIconPtr CContact::GetUserIcon()
{
	// TODO: Check if this critical section is really necessary
	CComCritSecLock<CComAutoCriticalSection> lock(m_csIcon);

	return m_pUserIcon;
}


void CContact::SetUserIcon(IUOLMessengerUserIconPtr pUserIcon)
{
	ATLASSERT(FALSE == ::IsBadReadPtr(pUserIcon.get(), sizeof(IUOLMessengerUserIcon*)));

	// TODO: Check if this critical section is really necessary
	CComCritSecLock<CComAutoCriticalSection> lock(m_csIcon);

	m_pUserIcon = pUserIcon;
}


void CContact::SetAddedTime(const COleDateTime& time)
{
	m_timeAdded = time;
}


COleDateTime CContact::GetAddedTime()
{
	return m_timeAdded;
}


void CContact::SetSignedOnTime(const COleDateTime& time)
{
	m_timeSignedOn = time;
}


COleDateTime CContact::GetSignedOnTime()
{
	return m_timeSignedOn;
}


void CContact::RegisterObserver(CUOLMessengerContactObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CContact::UnregisterObserver(CUOLMessengerContactObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CContact::NotifyAll()
{
	if (IsActive())
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver::Update);
	}
}


void CContact::NotifyAllAliasChange()
{
	if (IsActive())
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver::UpdateAlias);
	}
}


void CContact::NotifyAllStatusChange()
{
	if (IsActive())
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver::UpdateStatus);
	}
}


void CContact::NotifyAllIconChange()
{
	if (IsActive())
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver::UpdateIcon);
	}
}


void CContact::SetExtendedCapabilities(CProtocolSettings2Ptr pExtendedCapabilities)
{
	m_pExtendedCapabilites = pExtendedCapabilities;
}


CProtocolSettings2Ptr CContact::GetExtendedCapabilities()
{
	return m_pExtendedCapabilites;
}


void CContact::NotifyAllExtendedCapabilitiesChange()
{
	if (IsActive())
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver2::UpdateExtendedCapabilities);
	}
}


void CContact::SetActive(BOOL bActive)
{
	m_bActive = bActive;
}


BOOL CContact::IsActive() const
{
	return m_bActive;
}

void CContact::SetName(const CString& strName)
{
	BOOL bChanged = FALSE;
	
	if (m_strName.Compare(strName) != 0)
	{
		m_strName = strName;
		
		bChanged = TRUE;
	}
	
	if (bChanged)
	{
		NotifyAllChatContactRename();
	}
}


BOOL CContact::IsContact()
{
    return m_bIsContact;
}


void CContact::UpdateChatContactInfo(const CString& strName, 
                                     const CString& strAlias, const BOOL bIsContact)
{
    BOOL bChanged = FALSE;
	
	if ( (m_strName.Compare(strName) != 0) || (m_strAlias.Compare(strAlias) != 0) || 
			(m_bIsContact != bIsContact) )
	{
		m_strName = strName;
		m_strAlias = strAlias;
		m_bIsContact = bIsContact;
		
		bChanged = TRUE;
	}
	
	if (bChanged)
	{
		NotifyAllChatContactUpdate();
	}
}


void CContact::NotifyAllChatContactRename()
{
    if (IsActive())
	{
        m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver3::OnChatContactRename);
    }
}


void CContact::NotifyAllChatContactUpdate()
{
    if (IsActive())
	{
        m_observerNotifier.NotifyAll(&CUOLMessengerContactObserver3::OnChatContactUpdate);
    }
}


void CContact::SetCanRequestAuthorization(const BOOL bCanRequestAuthorization)
{
	m_bCanRequestAuthorization = bCanRequestAuthorization;
}


BOOL CContact::CanRequestAuthorization() const
{
	return m_bCanRequestAuthorization;
}


void CContact::SetStatusMessage(const CString& strStatusMessage)
{
	m_strStatusMessage = strStatusMessage;
}


CString CContact::GetStatusMessage()
{
	return m_strStatusMessage;
}