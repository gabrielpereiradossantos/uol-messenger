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
#include "../ObserverRegister.h"

//#define _LEAKDEBUG

class CContactObserverNotifier : public CObserverRegister<CUOLMessengerContactObserver>
{
public:
	CContactObserverNotifier(IUOLMessengerContact3* pContact);

	void NotifyAll(void (CUOLMessengerContactObserver::*pfnUpdate)(IUOLMessengerContact*));
	void NotifyAll(void (CUOLMessengerContactObserver2::*pfnUpdate)(IUOLMessengerContact2*));
    void NotifyAll(void (CUOLMessengerContactObserver3::*pfnUpdate)(IUOLMessengerContact3*));

private:
	IUOLMessengerContact3*	m_pContact;
};



class CContact : public IUOLMessengerContact4
{
public:
	CContact(IUOLMessengerAccountPtr pAcccount, const CString& strName);
    CContact(IUOLMessengerAccountPtr pAcccount, const CString& strChatContactName, const CString& strChatContactAlias, const BOOL bIsContact);

    virtual ~CContact();

	// IUOLMessengerContact interface
	virtual IUOLMessengerAccountPtr GetAccount() const;
	//void SetAccount(IUOLMessengerAccountPtr pAccount);

	virtual CString GetName() const;
	//virtual void SetName(const CString& strName);

	virtual CString GetAlias() const;
	virtual void SetAlias(const CString& strAlias);

	virtual IUOLMessengerGroupPtr GetGroup() const;
	virtual void SetGroup(IUOLMessengerGroupPtr pGroup);

	virtual DWORD GetStatus() const;	
	virtual void SetStatus(DWORD dwStatus);

	virtual IUOLMessengerUserIconPtr GetUserIcon();
	virtual void SetUserIcon(IUOLMessengerUserIconPtr pUserIcon);

	virtual void SetAddedTime(const COleDateTime& time);
	virtual COleDateTime GetAddedTime();

	virtual void SetSignedOnTime(const COleDateTime& time);
	virtual COleDateTime GetSignedOnTime();

	virtual void RegisterObserver(CUOLMessengerContactObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerContactObserver* pObserver);
	
	virtual void NotifyAll();
	virtual void NotifyAllAliasChange();
	virtual void NotifyAllStatusChange();
	virtual void NotifyAllIconChange();

	virtual void SetExtendedCapabilities(CProtocolSettings2Ptr pExtendedCapabilities);
	virtual CProtocolSettings2Ptr GetExtendedCapabilities();

	virtual void NotifyAllExtendedCapabilitiesChange();

    // IUOLMessengerContact3 - Chat exclusive methods
    virtual void SetName(const CString& strName);
    virtual BOOL IsContact();
    virtual void UpdateChatContactInfo(const CString& strName, 
			const CString& strAlias, const BOOL bIsContact);

    virtual void NotifyAllChatContactRename();
	virtual void NotifyAllChatContactUpdate();
	
	virtual void SetCanRequestAuthorization(const BOOL bCanRequestAuthorization);
	virtual BOOL CanRequestAuthorization() const;

	// IUOLMessengerContact4
	virtual void SetStatusMessage(const CString& strStatusMessage);
	virtual CString CContact::GetStatusMessage();

	void SetActive(BOOL bActive);
	BOOL IsActive() const;

private:
#ifdef _LEAKDEBUG
	static LONG ms_nRefs;
#endif

protected:
	IUOLMessengerAccountPtr	m_pAccount;

	CString		m_strName;
	CString		m_strAlias;
	CString		m_strStatusMessage;
	IUOLMessengerGroupPtr	m_pGroup;

	DWORD		m_dwStatus;

	CComAutoCriticalSection	 m_csIcon;
	IUOLMessengerUserIconPtr m_pUserIcon;

	COleDateTime	m_timeAdded;
	COleDateTime	m_timeSignedOn;

	CProtocolSettings2Ptr	m_pExtendedCapabilites;

	BOOL		m_bActive;
    BOOL		m_bIsContact;
	
	BOOL		m_bCanRequestAuthorization;

	CContactObserverNotifier	m_observerNotifier;

}; MAKEAUTOPTR(CContact);