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

#include <interfaces/IUOLMessengerAccount.h> 
#include "ISettings.h"
#include "AccountProtocolSettings.h"


class CAccount : public IUOLMessengerAccount3,
				 public ISettings
{
public:
	CAccount();
	virtual ~CAccount(void);

	virtual void SetProtocol(CIMProtocol* pProtocol);
	virtual void SetUserId(const CString& strUserId);
	virtual void Finalize();

protected:
	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	// IUOLMessengerAccount interface
	virtual CIMProtocol* GetProtocol() const;

	virtual CString GetUserId() const;

	virtual CString GetPassword() const;
	virtual void SetPassword(const CString& strPassword);

	virtual CString GetNickName() const;
	virtual void SetNickName(const CString& strNickName);

	virtual BOOL IsAutoConnectEnabled() const;
	virtual void EnableAutoConnect(BOOL bAutoConnect);

	virtual BOOL IsSavePasswordEnabled() const;
	virtual void EnableSavePassword(BOOL bSavePassword);

	virtual IUOLMessengerUserIconPtr GetUserIcon() const;
	virtual void SetUserIcon(IUOLMessengerUserIconPtr pUserIcon);

	virtual CString GetProtocolId() const;
	virtual CString GetProtocolName() const;

	virtual CProtocolSettingsPtr GetProtocolSettings() const;

	virtual CProtocolSettings2Ptr GetExtendedCapabilities() const;
	virtual void SetExtendedCapabilities(CProtocolSettings2Ptr pExtendedCapabilities);

	virtual CString GetStatusMessage() const;
	virtual void SetStatusMessage(const CString& strStatusMessage);


	// Callback register/unregister
	virtual void RegisterAccountObserver(CUOLMessengerAccountObserver* pObserver);
	virtual void UnregisterAccountObserver(CUOLMessengerAccountObserver* pObserver);

protected:

	DECLARE_SETTINGS_CALLBACK(CAccount, Protocol);
	DECLARE_SETTINGS_CALLBACK(CAccount, UserId);
	DECLARE_SETTINGS_CALLBACK(CAccount, Password);
	DECLARE_SETTINGS_CALLBACK(CAccount, NickName);
	DECLARE_SETTINGS_CALLBACK(CAccount, IconPath);
	DECLARE_SETTINGS_CALLBACK(CAccount, AutoConnect);
	DECLARE_SETTINGS_CALLBACK(CAccount, SavePassword);
	DECLARE_SETTINGS_CALLBACK(CAccount, Advanced);
	DECLARE_SETTINGS_CALLBACK(CAccount, StatusMessage);

private:
	void SetActive(BOOL bActive);
	BOOL IsActive() const;

	void NotifyObservers(void (CUOLMessengerAccountObserver::*pfnCallback)(IUOLMessengerAccount*));

protected:
	CIMProtocol*	m_pProtocol;
	CString		m_strUserId;
	CString		m_strPassword;
	BOOL		m_bSavePassword;
	CString		m_strNickName;
	IUOLMessengerUserIconPtr m_pUserIcon;
	BOOL		m_bAutoConnect;

	CAccountProtocolSettingsPtr m_pProtocolSettings;
	CProtocolSettings2Ptr		m_pExtendedCapabilities;

	CString		m_strProtocolId;
	CString		m_strStatusMessage;

private:
	LONG		m_bActive;

	CAtlList<CUOLMessengerAccountObserver*>		m_listAccountObservers;

	CProcessMap								m_tagProcess;
	CComAutoCriticalSection					m_criticalSection;
};

MAKEAUTOPTR(CAccount);