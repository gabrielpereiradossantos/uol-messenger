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
 
#pragma once

#include "IUOLMessengerAccount.h"
#include "IUOLMessengerContact.h"


class CUOLMessengerRemoteContactListObserver;


class __declspec(novtable) IUOLMessengerRemoteContactList
{
public:
	virtual ~IUOLMessengerRemoteContactList() {}

	virtual BOOL MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup) = 0;
	virtual BOOL RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup) = 0;
	virtual BOOL AddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup) = 0;
	virtual BOOL AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias) = 0;

	virtual BOOL RenameGroup(const CString& strOldGroup, const CString& strNewGroup) = 0;
	virtual BOOL RemoveGroup(const CString& strGroup) = 0;

	virtual BOOL GetContactInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact) = 0;

	/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value. */
	virtual BOOL CanRequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact) = 0;
	virtual BOOL RequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact) = 0;

	virtual void RegisterObserver(CUOLMessengerRemoteContactListObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerRemoteContactListObserver* pObserver) = 0;
};

MAKEAUTOPTR(IUOLMessengerRemoteContactList);

// New version of IUOLMessengerRemoteContactList2, the methods signatures was changed
// to provide more information about an error.
class __declspec(novtable) IUOLMessengerRemoteContactList2 : public IUOLMessengerRemoteContactList
{
public:
	virtual BOOL MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup, IMProtocolError& imProtocolError) = 0;
	virtual BOOL RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup, IMProtocolError& imProtocolError) = 0;
	virtual BOOL AddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup, IMProtocolError& imProtocolError) = 0;
	virtual BOOL AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias, IMProtocolError& imProtocolError) = 0;

	virtual BOOL RenameGroup(const CString& strOldGroup, const CString& strNewGroup, IMProtocolError& imProtocolError) = 0;
	virtual BOOL RemoveGroup(const CString& strGroup, IMProtocolError& imProtocolError) = 0;

	virtual BOOL GetContactInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError) = 0;

	/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value */
	virtual BOOL CanRequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError) = 0;
	virtual BOOL RequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError) = 0;

private:
	virtual BOOL MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup)
	{
		IMProtocolError error;

		return MoveContact(pAccount, strContact, strOldGroup, strNewGroup, error);
	}

	virtual BOOL RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup)
	{
		IMProtocolError error;

		return RemoveContact(pAccount, strContact, strGroup, error);
	}

	virtual BOOL AddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup)
	{
		IMProtocolError error;

		return AddContact(pAccount, strContact, strGroup, error);
	}

	virtual BOOL AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias)
	{
		IMProtocolError error;

		return AliasContact(pAccount, strContact, strAlias, error);
	}


	virtual BOOL RenameGroup(const CString& strOldGroup, const CString& strNewGroup)
	{
		IMProtocolError error;

		return RenameGroup(strOldGroup, strNewGroup, error);
	}

	virtual BOOL RemoveGroup(const CString& strGroup)
	{
		IMProtocolError error;

		return RemoveGroup(strGroup, error);
	}


	virtual BOOL GetContactInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact)
	{
		IMProtocolError error;

		return GetContactInfo(pAccount, strContact, error);
	}


	/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value */
	virtual BOOL CanRequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact)
	{
		IMProtocolError error;

		/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value */
		return CanRequestContactAuthorization(pAccount, strContact, error);
	}

	virtual BOOL RequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact)
	{
		IMProtocolError error;

		return RequestContactAuthorization(pAccount, strContact, error);
	}
};
MAKEAUTOPTR(IUOLMessengerRemoteContactList2);


class __declspec(novtable) IUOLMessengerRemoteContactList3 : public IUOLMessengerRemoteContactList2
{
public:
	virtual BOOL AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias, IMProtocolError& imProtocolError, BOOL bAsync = FALSE) = 0;
};
MAKEAUTOPTR(IUOLMessengerRemoteContactList3);


class CUOLMessengerRemoteContactListObserver
{
public:
	virtual ~CUOLMessengerRemoteContactListObserver() {}

	virtual void OnContactAdded(IUOLMessengerContactPtr pContact) { }
	virtual void OnContactRemoved(IUOLMessengerContactPtr pContact, const CString& strGroup) { }
	virtual void OnContactMoved(IUOLMessengerContactPtr pContact, const CString& strOldGroup) { }
	virtual void OnContactGotInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strInfo) { }
	//virtual void OnContactIconReceived(IUOLMessengerContactPtr pContact) { }
	//virtual void OnContactAliasReceived(IUOLMessengerContactPtr pContact) { }
	virtual void OnAddContactDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage) { }
	virtual void OnGroupAdded(IUOLMessengerGroupPtr pGroup) { }
	virtual void OnGroupRemoved(IUOLMessengerGroupPtr pGroup) { }
	virtual void OnGroupRenamed(IUOLMessengerGroupPtr pGroup, const CString& strOldGroup) { }

	virtual void OnContactSignedOn(IUOLMessengerContactPtr pContact) { }
	virtual void OnContactSignedOff(IUOLMessengerContactPtr pContact) { }
	virtual void OnContactAway(IUOLMessengerContactPtr pContact) { }
	virtual void OnContactAwayReturn(IUOLMessengerContactPtr pContact) { }
	virtual void OnContactIdle(IUOLMessengerContactPtr pContact) { }
	virtual void OnContactIdleReturn(IUOLMessengerContactPtr pContact) { }	

	virtual void OnNotifyAdded(IUOLMessengerContactPtr pContact, BOOL bOnContactList) { }
	virtual void OnNotifyContactAction(IUOLMessengerContactPtr pContact, NotifyReason reason) { }
};



class CUOLMessengerRemoteContactListObserver2 : public CUOLMessengerRemoteContactListObserver
{
public:
	virtual ~CUOLMessengerRemoteContactListObserver2() {}

	virtual void OnContactExtendedCapabilitiesChanged(IUOLMessengerContact2Ptr pContact2) { }
};