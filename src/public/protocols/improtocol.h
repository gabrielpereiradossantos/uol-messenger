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
 
#ifndef _IM_PROTOCOL_H_
#define _IM_PROTOCOL_H_

#include "improtocolconstants.h"
#include "improtocolcallback.h"
#include "iconspec.h"
#include "proxyinfo.h"
#include "protocolsettings.h"
#include "criticalsection.h"

#include <string>
#include <list>


typedef std::list<std::string> CPrivacyList;


class CIMProtocol
{
public:
	CIMProtocol();
	virtual ~CIMProtocol();

	virtual unsigned GetProtocolPos();

	void SetNextProtocol(CIMProtocol* pProtocol);
	CIMProtocol* GetNextProtocol();

	virtual void Init();
	virtual void Finalize();

	virtual std::string GetId();
	virtual std::string GetName();

	virtual void SetCallback(const std::string& strUser, CIMProtocolCallback* pCallback);

	virtual bool HasUserDisplayIconSupport();
	virtual bool HasContactBlockingSupport();
	virtual bool HasFileTransferSupport();
	virtual bool HasContactServerAliasSupport();
	virtual bool HasReRequestAuthorizationSupport();

	virtual int GetPrivacyFlags();

	virtual bool AutoAddContactOnAuthorization();
	virtual std::string GetDefaultContactGroup();

	// User name validation
	virtual bool IsValidUserName(const std::string& strUser);

	// Connection
	virtual IMProtocolError Connect(const std::string& strUser, const std::string& strPass, CProtocolSettings* pExtraSettings = NULL, CProxyInfo* pProxyInfo = NULL);
	virtual IMProtocolError Disconnect(const std::string& strUser);
	virtual int IsConnected(const std::string& strUser);

	// Password
	virtual IMProtocolError ChangePassword(const std::string& strUser, const std::string& strOldPassword, const std::string& strNewPassword);

	// Contact
	virtual IMProtocolError AddContact(const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	virtual IMProtocolError RemoveContact(const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	virtual IMProtocolError AliasContact(const std::string& strUser, const std::string& strContact, const std::string& strAlias);
	virtual IMProtocolError MoveContact(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup);
	virtual IMProtocolError GetContactInfo(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError CanRequestContactAuthorization(const std::string& strUser, const std::string& strContact, bool& bCanRequest);
	virtual IMProtocolError RequestContactAuthorization(const std::string& strUser, const std::string& strContact);

	// Group
	virtual IMProtocolError RenameGroup(const std::string& strUser, const std::string& strOldName, const std::string& strNewName);
	virtual IMProtocolError RemoveGroup(const std::string& strUser, const std::string& strGroup);

	// Alias
	virtual IMProtocolError SetAlias(const std::string& strUser, const std::string& strAlias);

	// Icon
	virtual void GetIconSpecification(CIconSpec& iconSpec);
	virtual IMProtocolError SetUserDisplayIcon(const std::string& strUser, const std::string& strFile);

	// Status
	virtual IMProtocolError SetUserStatus(const std::string& strUser, int nStatus);

	// IM
	virtual IMProtocolError SendIM(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage);

	// Notifications
	virtual IMProtocolError SendTyping(const std::string& strFromUser, const std::string& strToUser, TypingState state);
	virtual IMProtocolError SendConversationWindowClosed(const std::string& strFromUser, const std::string& strToUser);
	virtual IMProtocolError SetIdle(const std::string& strUser, int nSeconds);

	// Privacy
	virtual IMProtocolError GetDenyContactsList(const std::string& strUser, CPrivacyList& listContacts);
	virtual IMProtocolError GetPermitContactsList(const std::string& strUser, CPrivacyList& listContacts);
	virtual IMProtocolError AddDenyContact(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError AddPermitContact(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError RemoveDenyContact(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError RemovePermitContact(const std::string& strUser, const std::string& strContact);

	// File transfer
	virtual IMProtocolError SendFile(const std::string& strFromUser, const std::string& strToUser, const std::string& strFilePath);
	virtual IMProtocolError CancelFileTransfer(const std::string& strUser, long lFileTransferId);

	// Mail
	//virtual void SetCheckMail(const std::string& strUser, int nCheck) {}
protected:		
	CIMProtocol*			m_pNext;	

	CAutoCriticalSectionWrapper	m_csProtocol;
};


class CIMProtocol2 : public CIMProtocol
{
public:
	CIMProtocol2();
	virtual ~CIMProtocol2();

	virtual IMProtocolError Connect(const std::string& strUser, const std::string& strPass, CProtocolSettings* pExtraSettings = NULL, CProxyInfo* pProxyInfo = NULL, CProtocolSettings2* pExtendedCapabilities = NULL);
	virtual IMProtocolError SetExtendedCapabilities(const std::string& strUser, CProtocolSettings2* pExtendedCapabilities);
};


class CIMProtocol3 : public CIMProtocol2
{
public:
	CIMProtocol3();
	virtual ~CIMProtocol3();

	virtual bool HasContactListSupport();
};


class CIMProtocol4 : public CIMProtocol3
{
public:
	CIMProtocol4();
	virtual ~CIMProtocol4();
	
	//
	// Command API
	//
	
	virtual bool HasCommandSupport(const std::string& strCmdId);
	virtual IMProtocolError SendCommand(const std::string& strCmdId, const std::string& strCmdParameters, const std::string& strFromUser, const std::string& strToUser);
	
	// For commands that have cooldown or any blocks per protocol...
	// The second parameter allows to inform the especific block error, just in case to exist more than one...
	//
	virtual bool FreeToSendCommand(const std::string& strCmdId, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError);
	
	// Just in case we have the same command with different names throughout the protocols, 
	// I think it's better have an method to get the name.
	// That way, we set generic commands id's on a general class, and use this to get the 
	// protocol command name.
	//
	virtual std::string GetProtocolCommandName(const std::string& strCmdId);
	
	/*
	//
	// Attention API
	//
	
	virtual bool HasSendAttentionSupport(int nAttentionType);
	virtual IMProtocolError SendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser);
	
	// For attention primitives that have cooldown or any blocks per protocol...
	// The second parameter allows to inform the especific block error, just in case to exist more than one...
	//
	virtual bool FreeToSendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError);
	*/
};


class CIMProtocol5 : public CIMProtocol4
{
public:
	CIMProtocol5();
	virtual ~CIMProtocol5();
	
	// Chat or Conference
	//
	virtual bool HasChatSupport();
	virtual IMProtocolError ChatInvite(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage, int& nChatId, bool& bNewChatInvite, const std::string& strChatInviteMenuName);
	virtual IMProtocolError ChatLeave(const std::string& strFromUser, const int& nChatId);
	virtual IMProtocolError ChatSend(const std::string& strFromUser, const std::string& strMessage, const int& nChatId);
    virtual std::string GetChatInviteMenuName();
	
	// Custom Emoticon Addition
	//
	virtual bool HasCustomEmoticonSupport();
	/*
	virtual IMProtocolError AddCustomEmoticonToAccount(const std::string& strFromUser, const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName);
	virtual IMProtocolError RemoveCustomEmoticonFromAccount(const std::string& strFromUser, const std::string& strCustomEmoticonShortcut);
	*/
	virtual IMProtocolError AddCustomEmoticon(const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName);
	virtual IMProtocolError RemoveCustomEmoticon(const std::string& strCustomEmoticonShortcut);
	virtual IMProtocolError ChangeCustomEmoticonShortcut(const std::string& strCustomEmoticonOldShortcut, const std::string& strCustomEmoticonNewShortcut);
	
	// Assyncronous File Transfer.
	//
	virtual IMProtocolError AcceptFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	virtual IMProtocolError RejectFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	
	// Assync version of methods "reflected" during initial contact loading.
	//
	virtual IMProtocolError DisconnectAsync(const std::string& strUser);
	virtual IMProtocolError SetAliasAsync(const std::string& strUser, const std::string& strAlias);
	virtual IMProtocolError SetUserDisplayIconAsync(const std::string& strUser, const std::string& strFile);
	virtual IMProtocolError SetUserStatusAsync(const std::string& strUser, int nStatus);
	virtual IMProtocolError AliasContactAsync(const std::string& strUser, const std::string& strContact, const std::string& strAlias);
	
	// Protocol allows user alias config ?
	virtual bool HasUserAliasSupport();
};


class CIMProtocol6 : public CIMProtocol5
{
public:
	CIMProtocol6();
	virtual ~CIMProtocol6();

	virtual IMProtocolError SetUserStatus(const std::string& strUser, int nStatus, const std::string& strStatusMessage);
	virtual IMProtocolError SetUserStatusAsync(const std::string& strUser, int nStatus, const std::string& strStatusMessage);

	virtual bool HasOfflineMessageSupport();
};


class CIMProtocol7 : public CIMProtocol6
{
public:
	CIMProtocol7();
	virtual ~CIMProtocol7();
	
	virtual IMProtocolError SetExtendedCapabilitiesAsync(const std::string& strUser, CProtocolSettings2* pExtendedCapabilities);
};



#endif /* _IM_PROTOCOL_H_ */
