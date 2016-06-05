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


class CUOLMessengerAccountConnectionManagerObserver;


class __declspec(novtable) IUOLMessengerAccountConnectionManager
{
public:
	enum AccountStatus 
	{
		AS_ONLINE = 0,
		AS_OFFLINE,
		AS_BUSY,
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

public:
	virtual ~IUOLMessengerAccountConnectionManager() {}

	virtual int GetConnectedAccountsCount() = 0;
	virtual void GetConnectedAccountList(CAtlList<IUOLMessengerAccountPtr>& listAccounts) = 0;

	virtual BOOL Connect(IUOLMessengerAccountPtr pAccount) = 0; 
	virtual BOOL Disconnect(IUOLMessengerAccountPtr pAccount) = 0;
	virtual BOOL IsConnected(IUOLMessengerAccountPtr pAccount) = 0;
	virtual DWORD GetConnectionState(IUOLMessengerAccountPtr pAccount) = 0;

	virtual void ConnectAll(BOOL bAutoConnectOnly = FALSE) = 0;
	virtual void DisconnectAll() = 0;

	virtual void SetStatus(IUOLMessengerAccountPtr pAccount, DWORD dwStatus) = 0;
	virtual DWORD GetStatus(IUOLMessengerAccountPtr pAccount) = 0;
	virtual DWORD GetPreviousStatus(IUOLMessengerAccountPtr pAccount) = 0;

	virtual void UpdateServerAlias(IUOLMessengerAccountPtr pAccount) = 0;
	virtual BOOL UploadDisplayIcon(IUOLMessengerAccountPtr pAccount) = 0;

	virtual void AddReceivedAway(IUOLMessengerAccountPtr pAccount, const CString& strContact) = 0;
	virtual BOOL IsReceivedAway(IUOLMessengerAccountPtr pAccount, const CString& strContact) = 0;
	virtual void ResetReceivedAway(IUOLMessengerAccountPtr pAccount) = 0;

	virtual void SetIdleAll(UINT nSeconds) = 0;

	virtual void RegisterObserver(CUOLMessengerAccountConnectionManagerObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerAccountConnectionManagerObserver* pObserver) = 0;

	virtual void RegisterAccountObserver(IUOLMessengerAccountPtr pAccount, CUOLMessengerAccountConnectionManagerObserver* pObserver) = 0;
	virtual void UnregisterAccountObserver(IUOLMessengerAccountPtr pAccount, CUOLMessengerAccountConnectionManagerObserver* pObserver) = 0;
};

MAKEAUTOPTR(IUOLMessengerAccountConnectionManager);



class CUOLMessengerAccountConnectionManagerObserver
{
public:
	virtual ~CUOLMessengerAccountConnectionManagerObserver() {}

	virtual void OnConnected(IUOLMessengerAccountPtr pAccount) { }
	virtual void OnDisconnected(IUOLMessengerAccountPtr pAccount) { }
	virtual void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage) { }

	virtual void OnStatusChanged(IUOLMessengerAccountPtr pAccount) { }
};



class __declspec(novtable) IUOLMessengerAccountConnectionManager2 : public IUOLMessengerAccountConnectionManager
{
public:
	virtual ~IUOLMessengerAccountConnectionManager2() {}

	virtual BOOL UpdateExtendedCapabilities(IUOLMessengerAccount2Ptr pAccount);
};

MAKEAUTOPTR(IUOLMessengerAccountConnectionManager2);



class __declspec(novtable) IUOLMessengerAccountConnectionManager3 : public IUOLMessengerAccountConnectionManager2
{
public:
	virtual ~IUOLMessengerAccountConnectionManager3() {}
	
	virtual void GetAccountValidStatus(IUOLMessengerAccountPtr pAccount, CAtlList<int>& listValidStatus) = 0;
};

MAKEAUTOPTR(IUOLMessengerAccountConnectionManager3);


