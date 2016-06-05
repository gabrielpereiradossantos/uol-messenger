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
 
#ifndef _PURPLE_JABBER_UOL_PROTOCOL_H_
#define _PURPLE_JABBER_UOL_PROTOCOL_H_

#include "purplejabberprotocol.h"
#include "purplejabberuolaccountinfo.h"
#include <xmlnode.h>


class CPurpleJabberUOLProtocol : public CPurpleJabberProtocol
{
public:
	CPurpleJabberUOLProtocol();
	CPurpleJabberUOLProtocol(const std::string& strProtocolId, const std::string& strProtocolName);
	virtual ~CPurpleJabberUOLProtocol();

	virtual bool HasUserDisplayIconSupport();

	virtual bool IsValidUserName(const std::string& strUser);

protected:
	virtual void Init();
	virtual void Finalize();

	virtual IMProtocolError Disconnect(const std::string& strUser);
	virtual IMProtocolError SetUserStatus(const std::string& strUser, int nStatus);
	virtual IMProtocolError SetUserStatus(const std::string& strUser, int nStatus, const std::string& strStatusMessage);
	
	virtual void OnReportDisconnect(PurpleAccount* pAccount, const char* pszText);

	virtual bool GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason);
	
	virtual void ProcessExtendedCapabilities(PurpleAccount* pAccount, CProtocolSettings2* pExtendedCapabilities);
	virtual IMProtocolError UpdateServerExtendedCapabilities(PurpleAccount* pAccount, BOOL bPublishing);

	void SetUserExtendedCapabilities(const char* pszUser, CProtocolSettings2* pExtendedCapabilities);
	CProtocolSettings2* GetUserExtendedCapabilities(const char* pszUser);

	void SetContactExtendedCapabilities(const char* pszUser, const std::string& strContactKey, CProtocolSettings2* pExtendedCapabilities);
	CProtocolSettings2* GetContactExtendedCapabilities(const char* pszUser, const std::string& strContactKey);
	std::string GetContactKey(PurpleBuddy* pBuddy);

	void RemoveExtendedCapabilites(const char* pszUser);
	void RemoveAllExtendedCapabilities();
	void SaveUserStatus(const char* pszUser, int nStatus, const std::string& strStatusMessage);
	bool LoadUserStatus(const char* pszUser, int& nStatus, std::string& strStatusMessage);

	virtual void PresencePreSendCallback(xmlnode* pXmlNode, PurpleConnection* pConnection);
	virtual void PresenceReceivedCallback(xmlnode* pXmlNode, PurpleBuddy* buddy);

	static void SetExtensionsCallback(CPurpleJabberUOLProtocol* pProtocol);
	static void PresencePreSendCallbackHandler(xmlnode* pXmlNode, PurpleConnection* pConnection);
	static void PresenceReceivedCallbackHandler(xmlnode* pXmlNode, PurpleBuddy* pBuddy);

private:
	void AddPresenceExtendedCapabilities(xmlnode* pXmlNode, CProtocolSettings2* pExtendedCapabilities);
	CProtocolSettings2* ExtractPresenceExtendedCapabilities(xmlnode* pXmlNode);

public:
	static const char* PURPLE_JABBER_UOL_PLUGIN_ID;
	static const char* PURPLE_JABBER_UOL_PROTOCOL_NAME;

protected:
	static CPurpleJabberUOLProtocol* ms_callbackHandler;
	static CAutoCriticalSectionWrapper ms_csCallback;

private:
	typedef std::map<std::string, CPurpleJabberUOLAccountInfo*> CAccountInfoMap;

	struct JabberStatus
	{
		int status;
		char* statusText;
	};

	static JabberStatus ms_status[];

	static const char* PURPLE_JABBER_UOL_DEFAULT_RESOURCE;
	static char PURPLE_JABBER_UOL_RESOURCE_SPLITTER;

	CAccountInfoMap		m_mapAccountInfo;
};

#endif