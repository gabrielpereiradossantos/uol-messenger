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

#include <interfaces/IUOLMessengerContact.h>

#include "Contact.h"


class CGroupObserver;


class CGroup : public IUOLMessengerGroup
{
public:
	CGroup(const CString& strName);
	virtual ~CGroup();

protected:
	virtual CString GetName() const;
	virtual void SetName(const CString& strName);

	virtual IUOLMessengerContactPtr GetContact(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	virtual void GetContactList(CAtlList<IUOLMessengerContactPtr>& listContacts);
	virtual UINT GetContactCount(); 
	virtual UINT GetConnectedContactCount();

	virtual void RegisterObserver(CUOLMessengerGroupObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerGroupObserver* pObserver);
	virtual void NotifyAll();

	void SetActive(BOOL bActive);
	BOOL IsActive() const;

protected:
	BOOL AddContact(CContactPtr pContact);
	CContactPtr RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	CContactPtr RemoveContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact);
	CContactPtr FindContact(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	CContactPtr FindContact(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact);
	int FindContacts(IUOLMessengerAccountPtr pAccount, CAtlList<CContactPtr>& listContacts);
	int FindContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, CAtlList<CContactPtr>& listContacts);
	void RemoveAllContacts(IUOLMessengerAccountPtr pAccount, CAtlList<CContactPtr>& listRemovedContacts);
	void RemoveAllContacts(const CString& strAccountUserId, const CString& strAccountProtocolId, CAtlList<CContactPtr>& listRemovedContacts);
	void RemoveAllContacts();

private:
	BOOL Equals(CContactPtr pContact, const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact);

	CContactPtr FindContactImpl(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strContact);

protected:
	friend class CContactList;

private:
#ifdef _LEAKDEBUG
	static LONG ms_nRefs;
#endif

	CString		m_strName;
	CAtlList<CContactPtr>		m_listContacts;
	CComAutoCriticalSection		m_csContacts;

	BOOL		m_bActive;

	CAtlList<CUOLMessengerGroupObserver*> m_listObservers;
	CComAutoCriticalSection				  m_csObservers;
}; 

MAKEAUTOPTR(CGroup);
