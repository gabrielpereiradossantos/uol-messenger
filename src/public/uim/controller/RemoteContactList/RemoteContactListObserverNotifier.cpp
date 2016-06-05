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
#include "RemoteContactListObserverNotifier.h"


void CRemoteContactListObserverNotifier::RegisterObserver(CUOLMessengerRemoteContactListObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
		{
			CRemoteContactListObserverWrapperPtr pObserverWrapper = m_listObservers.GetNext(pos);

			if (pObserver == pObserverWrapper->GetWrappedObserver())
			{
				// Observer already registered
				ATLASSERT(FALSE);
				return;
			}
		}

		m_listObservers.AddTail(new CRemoteContactListObserverWrapper(pObserver));
	}
}


void CRemoteContactListObserverNotifier::UnregisterObserver(CUOLMessengerRemoteContactListObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CRemoteContactListObserverWrapperPtr pRemovedObserverWrapper;
		
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

			for (POSITION pos = m_listObservers.GetHeadPosition(); pos; m_listObservers.GetNext(pos))
			{
				CRemoteContactListObserverWrapperPtr pObserverWrapper = m_listObservers.GetAt(pos);

				if (pObserver == pObserverWrapper->GetWrappedObserver())
				{
					// Observer found
					pRemovedObserverWrapper = pObserverWrapper;
					m_listObservers.RemoveAt(pos);
					break;
				}
			}
		}

		ATLASSERT(pRemovedObserverWrapper);
		if (pRemovedObserverWrapper)
		{
			pRemovedObserverWrapper->DisableNotifications();
		}
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerContactPtr), IUOLMessengerContactPtr pContact)
{	
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pContact);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver2::*pfnCallback)(IUOLMessengerContact2Ptr), IUOLMessengerContact2Ptr pContact2)
{	
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pContact2);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerContactPtr, const CString&), IUOLMessengerContactPtr pContact, const CString& strParam1)
{
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pContact, strParam1);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerAccountPtr, const CString&, const CString&), IUOLMessengerAccountPtr pAccount, const CString& strParam1, const CString& strParam2)
{
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pAccount, strParam1, strParam2);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerGroupPtr), IUOLMessengerGroupPtr pGroup)
{
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pGroup);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerGroupPtr, const CString&), IUOLMessengerGroupPtr pGroup, const CString& strParam1)
{
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pGroup, strParam1);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerContactPtr, NotifyReason), IUOLMessengerContactPtr pContact, NotifyReason reason)
{
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pContact, reason);
	}
}


void CRemoteContactListObserverNotifier::NotifyAll(void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerContactPtr, BOOL), IUOLMessengerContactPtr pContact, BOOL bValue)
{
	CAtlList<CRemoteContactListObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CRemoteContactListObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)(pContact, bValue);
	}
}