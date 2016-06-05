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

#include "../../protocols/improtocol.h"
#include "IUOLMessengerUserIcon.h"

class IUOLMessengerAccount;

#include "UOLMessengerAccountObserver.h"

class CUOLMessengerUserInputHandler;

MAKEAUTOPTR(CProtocolSettings);
MAKEAUTOPTR(CProtocolSettings2);


class __declspec(novtable) IUOLMessengerAccount
{
public:
	virtual ~IUOLMessengerAccount() {}

	virtual CIMProtocol* GetProtocol() const = 0;
	//virtual void SetProtocol(CIMProtocol* pProtocol) = 0;

	virtual CString GetUserId() const = 0;
	//virtual void SetUserId(const CString& strUserId)= 0;

	virtual CString GetPassword() const = 0;
	virtual void SetPassword(const CString& strPassword) = 0;

	virtual CString GetNickName() const = 0;
	virtual void SetNickName(const CString& strNickName) = 0;

	virtual BOOL IsAutoConnectEnabled() const = 0;
	virtual void EnableAutoConnect(BOOL bAutoConnect) = 0;

	virtual BOOL IsSavePasswordEnabled() const = 0;
	virtual void EnableSavePassword(BOOL bSavePassword) = 0;

	virtual IUOLMessengerUserIconPtr GetUserIcon() const = 0;
	virtual void SetUserIcon(IUOLMessengerUserIconPtr pUserIcon) = 0;

	virtual CString GetProtocolId() const = 0;
	virtual CString GetProtocolName() const = 0;

	virtual CProtocolSettingsPtr GetProtocolSettings() const = 0;
	
	// Callback register/unregister
	virtual void RegisterAccountObserver(CUOLMessengerAccountObserver* pObserver) = 0;
	virtual void UnregisterAccountObserver(CUOLMessengerAccountObserver* pObserver) = 0;

	
public:
	enum AccountStatus 
	{
		AS_ONLINE = 0,
		AS_OFFLINE,
		AS_AWAY,
		AS_AUTO_AWAY,
		AS_AUTO_AWAY_RETURN,
		AS_INVISIBLE,
		AS_PERSONALIZED
	};

	enum ConnectionState
	{
		CS_DISCONNECTED = 0,
		CS_CONNECTED,
		CS_CONNECTING
	};

	enum
	{
		MAX_ACCOUNT_STATUS_MESSAGE_LENGTH = 256
	};
};

MAKEAUTOPTR(IUOLMessengerAccount);



class __declspec(novtable) IUOLMessengerAccount2 : public IUOLMessengerAccount
{
public:
	virtual ~IUOLMessengerAccount2() {}

	virtual CProtocolSettings2Ptr GetExtendedCapabilities() const = 0;
	virtual void SetExtendedCapabilities(CProtocolSettings2Ptr pExtendedCapabilities) = 0;
};

MAKEAUTOPTR(IUOLMessengerAccount2);


class __declspec(novtable) IUOLMessengerAccount3 : public IUOLMessengerAccount2
{
public:
	virtual ~IUOLMessengerAccount3() {}

	virtual CString GetStatusMessage() const = 0;
	virtual void SetStatusMessage(const CString& strStatusMessage) = 0;
};

MAKEAUTOPTR(IUOLMessengerAccount3);
