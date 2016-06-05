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

#include <interfaces/IUOLMessengerContactList.h>
#include "../model/Group.h"
#include "../model/Contact.h"
#include "../ObserverRegister.h"


class CContactListObserverNotifier : public CObserverRegister<CUOLMessengerContactListObserver>
{
public:
	void NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerContactPtr), IUOLMessengerContactPtr pContact);
	void NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerContactPtr, const CString&), IUOLMessengerContactPtr pContact, const CString& strParam);
	void NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerGroupPtr), IUOLMessengerGroupPtr pGroup);
	void NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(IUOLMessengerGroupPtr, const CString&), IUOLMessengerGroupPtr pGroup, const CString& strParam);
	void NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)(CAtlList<IUOLMessengerContactPtr>&), CAtlList<IUOLMessengerContactPtr>& listContacts);
	void NotifyObservers(void (CUOLMessengerContactListObserver::*pfnUpdate)());
};




class CContactList : public IUOLMessengerContactList
{
public:
	CContactList();
	virtual ~CContactList();

	virtual IUOLMessengerContactPtr CreateContact(IUOLMessengerAccountPtr pAccount, const CString& strUser);
	virtual BOOL AddContact(IUOLMessengerContactPtr pContact, const CString& strGroup);
	virtual IUOLMessengerContactPtr AddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup);
	virtual IUOLMessengerContactPtr RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual IUOLMessengerContactPtr RemoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact);
	virtual IUOLMessengerContactPtr RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup);
	virtual IUOLMessengerContactPtr RemoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, const CString& strGroup);
	virtual IUOLMessengerContactPtr FindContact(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual IUOLMessengerContactPtr FindContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact);
	virtual IUOLMessengerContactPtr FindContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup);
	virtual IUOLMessengerContactPtr FindContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, const CString& strGroup);
	virtual int FindContacts(IUOLMessengerAccountPtr pAccount, const CString& strContact, CAtlList<IUOLMessengerContactPtr>& listContacts);
	virtual int FindContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, CAtlList<IUOLMessengerContactPtr>& listContacts);
	virtual int FindContacts(IUOLMessengerAccountPtr pAccount, CAtlList<IUOLMessengerContactPtr>& listContacts);
	virtual int FindContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, CAtlList<IUOLMessengerContactPtr>& listContacts);
	virtual void RemoveAllContacts(IUOLMessengerAccountPtr pAccount);
	virtual void RemoveAllContacts(const CString& strAccountUserId, const CString& strAccountProtocolId);
	virtual BOOL MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strGroup);
	virtual BOOL MoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact, const CString& strOldGroup, const CString& strGroup);

	virtual IUOLMessengerGroupPtr AddGroup(const CString& strGroup);
	virtual IUOLMessengerGroupPtr RemoveGroup(const CString& strGroup);
	virtual IUOLMessengerGroupPtr FindGroup(const CString& strGroup);
	virtual BOOL RenameGroup(const CString& strOldGroup, const CString& strNewGroup);
	virtual void RemoveAllGroups();

	virtual void GetGroupList(CAtlList<IUOLMessengerGroupPtr>& listGroups);

	virtual void RegisterObserver(CUOLMessengerContactListObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerContactListObserver* pObserver);

private:
	BOOL InternalAddContact(CContactPtr pContact, CGroupPtr pGroup);
	CGroupPtr InternalAddGroup(const CString& strGroup);
	CContactPtr InternalFindContact(const CString& strAccountUserId, const CString& strAccountProtcolId, const CString& strContact);
	int InternalFindContacts(const CString& strAccountUserId, const CString& strAccountProtcolId, const CString& strContact, CAtlList<IUOLMessengerContactPtr>& listContacts);
	int InternalFindContacts(const CString& strAccountUserId, const CString& strAccountProtcolId, CAtlList<IUOLMessengerContactPtr>& listContacts);
	CGroupPtr InternalFindGroup(const CString& strGroup);

private:
	CComAutoCriticalSection		m_csGroups;
	CAtlList<CGroupPtr>			m_listGroups;

	CContactListObserverNotifier	m_ObserverNotifier;
};
