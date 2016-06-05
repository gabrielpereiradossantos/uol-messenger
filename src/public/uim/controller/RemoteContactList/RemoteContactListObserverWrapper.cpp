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
#include "RemoteContactListObserverWrapper.h"


CRemoteContactListObserverWrapper::CRemoteContactListObserverWrapper(CUOLMessengerRemoteContactListObserver* pObserver) :
	CDerivedObserverWrapper<CUOLMessengerRemoteContactListObserver, CUOLMessengerRemoteContactListObserver2>(pObserver)
{
}


CRemoteContactListObserverWrapper::~CRemoteContactListObserverWrapper()
{
}


void CRemoteContactListObserverWrapper::OnContactAdded(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactAdded(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactRemoved(IUOLMessengerContactPtr pContact, const CString& strGroup)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactRemoved(pContact, strGroup);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactMoved(IUOLMessengerContactPtr pContact, const CString& strOldGroup)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactMoved(pContact, strOldGroup);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactGotInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strInfo)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactGotInfo(pAccount, strContact, strInfo);
	}

	END_HANDLER_CALL();
}


//void CRemoteContactListObserverWrapper::OnContactIconReceived(IUOLMessengerContactPtr pContact)
//{
//	BEGIN_HANDLER_CALL();
//
//	if (m_pObserver)
//	{
//		m_pObserver->OnContactIconReceived(pContact);
//	}
//
//	END_HANDLER_CALL();
//}
//
//
//void CRemoteContactListObserverWrapper::OnContactAliasReceived(IUOLMessengerContactPtr pContact)
//{
//	BEGIN_HANDLER_CALL();
//
//	if (m_pObserver)
//	{
//		m_pObserver->OnContactAliasReceived(pContact);
//	}
//
//	END_HANDLER_CALL();
//}


void CRemoteContactListObserverWrapper::OnAddContactDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact,  const CString& strMessage)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnAddContactDenied(pAccount, strContact, strMessage);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnGroupAdded(IUOLMessengerGroupPtr pGroup)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnGroupAdded(pGroup);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnGroupRemoved(IUOLMessengerGroupPtr pGroup)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnGroupRemoved(pGroup);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnGroupRenamed(IUOLMessengerGroupPtr pGroup, const CString& strOldGroup)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnGroupRenamed(pGroup, strOldGroup);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactSignedOn(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactSignedOn(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactSignedOff(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactSignedOff(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactAway(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactAway(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactAwayReturn(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactAwayReturn(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactIdle(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactIdle(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactIdleReturn(IUOLMessengerContactPtr pContact)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnContactIdleReturn(pContact);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnNotifyAdded(IUOLMessengerContactPtr pContact, BOOL bOnContactList)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnNotifyAdded(pContact, bOnContactList);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnNotifyContactAction(IUOLMessengerContactPtr pContact, NotifyReason reason)
{
	BEGIN_HANDLER_CALL();

	if (m_pObserver)
	{
		m_pObserver->OnNotifyContactAction(pContact, reason);
	}

	END_HANDLER_CALL();
}


void CRemoteContactListObserverWrapper::OnContactExtendedCapabilitiesChanged(IUOLMessengerContact2Ptr pContact2)
{
	BEGIN_HANDLER_CALL();

	CUOLMessengerRemoteContactListObserver2* pObserver2 = GetWrappedDerivedObserver();

	if (pObserver2)
	{
		pObserver2->OnContactExtendedCapabilitiesChanged(pContact2);
	}

	END_HANDLER_CALL();
}