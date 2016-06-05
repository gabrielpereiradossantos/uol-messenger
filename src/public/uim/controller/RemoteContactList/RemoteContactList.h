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

#include <interfaces/IUOLMessengerFacade.h>
#include <interfaces/IUOLMessengerRemoteContactList.h>
#include "RemoteContactListObserverNotifier.h"


class CRemoteContactList : 
	public IUOLMessengerRemoteContactList3,
	public CUOLMessengerAccountListEventListener,
	public CUOLMessengerAccountConnectionManagerObserver
{
public:
	CRemoteContactList();
	virtual ~CRemoteContactList();

	BOOL Init(IUOLMessengerFacade* pFacade);
	void Finalize();

	void NotifyUserAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactListUpdate(IUOLMessengerAccountPtr pAccount, const CContactInfo* pContactInfo, const CString& strGroup);
	void NotifyContactRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup);
	void NotifyContactMoved(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup);
	void NotifyContactGotInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strInfo);
	void NotifyContactIconReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const char* pIconData, int nLen);
	void NotifyContactAliasReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias);
	void NotifyGroupAdded(IUOLMessengerAccountPtr pAccount, const CString& strGroup);
	void NotifyGroupRemoved(IUOLMessengerAccountPtr pAccount, const CString& strGroup);
	void NotifyGroupRenamed(IUOLMessengerAccountPtr pAccount, const CString& strOldGroup, const CString& strNewGroup);
	void NotifyContactSignedOn(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactSignedOff(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactAway(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactAwayReturn(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactIdle(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactIdleReturn(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyContactStatusChanged(IUOLMessengerAccountPtr pAccount, const CString& strContact, int nStatus, const CString& strStatusMessage);
	void NotifyAddContactDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage);
	void NotifyContactAction(IUOLMessengerAccountPtr pAccount, NotifyReason reason, const CString& strContact);
	void NotifyContactExtendedCapabilitiesChanged(IUOLMessengerAccountPtr pAccount, const CString& strContact, CProtocolSettings2Ptr pExtendedCapabilities);
	void NotifyContactCanRequestAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, const int bCanRequestAuthorization);

protected:
	virtual BOOL MoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strOldGroup, const CString& strNewGroup, IMProtocolError& imProtocolError);
	virtual BOOL RemoveContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup, IMProtocolError& imProtocolError);
	virtual BOOL AddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strGroup, IMProtocolError& imProtocolError);
	virtual BOOL AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias, IMProtocolError& imProtocolError);
	virtual BOOL AliasContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAlias, IMProtocolError& imProtocolError, BOOL bAsync);

	virtual BOOL RenameGroup(const CString& strOldGroup, const CString& strNewGroup, IMProtocolError& imProtocolError);
	virtual BOOL RemoveGroup(const CString& strGroup, IMProtocolError& imProtocolError);

	virtual BOOL GetContactInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError);

	/* CanRequestContactAuthorization method is useless now that IUOLMessengerContact3 have this value. */
	virtual BOOL CanRequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError);
	virtual BOOL RequestContactAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError& imProtocolError);

	virtual void RegisterObserver(CUOLMessengerRemoteContactListObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerRemoteContactListObserver* pObserver);

	virtual void OnAddAccount(IUOLMessengerAccountPtr pAccount);
	virtual void OnRemoveAccount(IUOLMessengerAccountPtr pAccount);

	virtual void OnDisconnected(IUOLMessengerAccountPtr pAccount);

private:
	void UpdateContactStatus(IUOLMessengerAccountPtr pAccount, const CString& strContact, DWORD dwNewStatus, void (CUOLMessengerRemoteContactListObserver::*pfnCallback)(IUOLMessengerContactPtr));
	void UpdateContactStatusMessage(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strStatusMessage);

private:
	IUOLMessengerFacade*	m_pFacade;

	CRemoteContactListObserverNotifier	m_observerNotifier;
};

MAKEAUTOPTR(CRemoteContactList);