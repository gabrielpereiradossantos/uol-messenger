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

class IUOLMessengerContact;
MAKEAUTOPTR(IUOLMessengerContact);

#include "IUOLMessengerGroup.h"
#include "IUOLMessengerUserIcon.h"


class CUOLMessengerContactObserver;


class __declspec(novtable) IUOLMessengerContact
{
public:
	virtual ~IUOLMessengerContact() {}

	virtual IUOLMessengerAccountPtr GetAccount() const = 0;
	//virtual void SetAccount(IUOLMessengerAccountPtr pAccount) = 0;

	virtual CString GetName() const = 0;
	//virtual void SetName(const CString& strName) = 0;

	virtual IUOLMessengerGroupPtr GetGroup() const = 0;
	//virtual void SetGroup(IUOLMessengerGroupPtr pGroup) = 0;

	virtual CString GetAlias() const = 0;
	virtual void SetAlias(const CString& strAlias) = 0;

	virtual DWORD GetStatus() const = 0;
	virtual void SetStatus(DWORD dwStatus) = 0;

	virtual IUOLMessengerUserIconPtr GetUserIcon() = 0;
	virtual void SetUserIcon(IUOLMessengerUserIconPtr pUserIcon) = 0;

	virtual void SetAddedTime(const COleDateTime& time) = 0;
	virtual COleDateTime GetAddedTime() = 0;

	virtual void SetSignedOnTime(const COleDateTime& time) = 0;
	virtual COleDateTime GetSignedOnTime() = 0;

	virtual void RegisterObserver(CUOLMessengerContactObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerContactObserver* pObserver) = 0;

	virtual void NotifyAll() = 0;
	virtual void NotifyAllAliasChange() = 0;
	virtual void NotifyAllStatusChange() = 0;
	virtual void NotifyAllIconChange() = 0;

public:
	enum ContactStatus 
	{
		CS_ONLINE = 10,
		CS_BUSY = 20,
		CS_AWAY = 40,
		CS_IDLE = 50,
		CS_OFFLINE = 60,
		CS_UNKNOWN = 100
	};

	enum
	{
		MAX_CONTACT_NAME_LENGTH = 128,
		MAX_CONTACT_ALIAS_LENGTH = 256
	};
};

class CUOLMessengerContactObserver2;


class __declspec(novtable) IUOLMessengerContact2 : public IUOLMessengerContact
{
public:
	virtual ~IUOLMessengerContact2() {}

	virtual void SetExtendedCapabilities(CProtocolSettings2Ptr pExtendedCapabilities) = 0;
	virtual CProtocolSettings2Ptr GetExtendedCapabilities() = 0;

	virtual void NotifyAllExtendedCapabilitiesChange() = 0;
};

MAKEAUTOPTR(IUOLMessengerContact2);

 // Chat exclusive methods
class CUOLMessengerContactObserver3;


class __declspec(novtable) IUOLMessengerContact3 : public IUOLMessengerContact2
{
public:
	virtual ~IUOLMessengerContact3() {}

	virtual void SetName(const CString& strName);    
    virtual BOOL IsContact() = 0;
    virtual void UpdateChatContactInfo(const CString& strName, 
			const CString& strAlias, const BOOL bIsContact);
	
	virtual void NotifyAllChatContactRename() = 0;
	virtual void NotifyAllChatContactUpdate() = 0;
    
    virtual BOOL CanRequestAuthorization() const = 0;
};

MAKEAUTOPTR(IUOLMessengerContact3);


class CUOLMessengerContactObserver
{
public:
	virtual ~CUOLMessengerContactObserver() {}

	virtual void Update(IUOLMessengerContact* pContact) = 0;
	virtual void UpdateAlias(IUOLMessengerContact* pContact) = 0;
	virtual void UpdateStatus(IUOLMessengerContact* pContact) = 0;
	virtual void UpdateIcon(IUOLMessengerContact* pContact) = 0;
};


class CUOLMessengerContactObserver2 : public CUOLMessengerContactObserver
{
public:
	virtual ~CUOLMessengerContactObserver2() {}

	virtual void UpdateExtendedCapabilities(IUOLMessengerContact2* pContact) = 0;
};


class CUOLMessengerContactObserver3 : public CUOLMessengerContactObserver2
{
public:
	virtual ~CUOLMessengerContactObserver3() {}

	virtual void OnChatContactRename(IUOLMessengerContact3* pChatContact) = 0;
	virtual void OnChatContactUpdate(IUOLMessengerContact3* pChatContact) = 0;
};

class __declspec(novtable) IUOLMessengerContact4 : public IUOLMessengerContact3
{
public:
	virtual ~IUOLMessengerContact4() {}

	virtual void SetStatusMessage(const CString& strStatusMessage) = 0;    
	virtual CString GetStatusMessage() = 0;    
};

MAKEAUTOPTR(IUOLMessengerContact4);