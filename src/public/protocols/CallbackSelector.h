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

#include "purpleprotocolstack.h"
#include "ILibPurpleToUIMThreadSerializer.h"


class CCallbackSelector : public CIMProtocolCallback7
{
public:
	CCallbackSelector(CPurpleProtocolStack* pPurpleProtocolStack);
	virtual ~CCallbackSelector();

	// User get connected
	virtual void OnConnected(const std::string& strUser);

	// User is disconnected
	virtual void OnDisconnected(const std::string& strUser);

	// Connection error
	virtual void OnReportDisconnect(const std::string& strUser, ConnectionError errorCode, const std::string& strMessage);
	
	// Contact has added user
	virtual void OnNotifyAdded(const std::string& strUser, const std::string& strContact);

	virtual void OnContactListUpdate(const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup);
	virtual void OnContactRemoved(const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	virtual void OnContactMoved(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup);
	virtual void OnContactGotInfo(const std::string& strUser, const std::string& strContact, const std::string& strInfo);
	virtual void OnContactIconReceived(const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen);
	virtual void OnContactAliasReceived(const std::string& strUser, const std::string& strContact, const std::string& strAlias);
	virtual void OnAddContactDenied(const std::string& strUser, const std::string& strContact, const std::string& strMessage);
	virtual void OnGroupAdded(const std::string& strUser, const std::string& strGroup);
	virtual void OnGroupRemoved(const std::string& strUser, const std::string& strGroup);
	virtual void OnGroupRenamed(const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup);

	virtual void OnIMReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText);
	virtual void OnIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText);
	virtual void OnIMSendError(const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError);

	virtual void OnContactSignedOn(const std::string& strUser, const std::string& strContact);
	virtual void OnContactSignedOff(const std::string& strUser, const std::string& strContact);
	virtual void OnContactAway(const std::string& strUser, const std::string& strContact);
	virtual void OnContactAwayReturn(const std::string& strUser, const std::string& strContact);
	virtual void OnContactIdle(const std::string& strUser, const std::string& strContact);
	virtual void OnContactIdleReturn(const std::string& strUser, const std::string& strContact);
	virtual void OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus);
	virtual void OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage);
	
	virtual void OnContactTyping(const std::string& strUser, const std::string& strContact, TypingState state);

	virtual void OnConversationWindowClosed(const std::string& strUser, const std::string& strContact);
	virtual void OnConversationTimeout(const std::string& strUser, const std::string& strContact);

	virtual void OnPermitAdded(const std::string& strUser, const std::string& strContact);
	virtual void OnDenyAdded(const std::string& strUser, const std::string& strContact);
	virtual void OnPermitRemoved(const std::string& strUser, const std::string& strContact);
	virtual void OnDenyRemoved(const std::string& strUser, const std::string& strContact);

	virtual RequestResult OnRequestInput(const std::string& strUser, RequestReason reason, char* pszBuffer, int nBufferLength);

	virtual RequestResult OnRequestAddContact(const std::string& strUser, const std::string& strContact);
	virtual RequestResult OnRequestAddUser(const std::string& strUser, const std::string& strContact, const std::string& strAddReason);
	virtual RequestResult OnRequestSendAuthorization(const std::string& strUser, const std::string& strContact);

	virtual void OnNotifyContactAction(const std::string& strUser, NotifyReason reason, const std::string& strContact);

	// File Transfer
	virtual RequestResult OnRequestAcceptFile(const std::string& strUser, const std::string& strContact, const std::string& strFilename);
	virtual RequestResult OnRequestFilePath(const std::string& strUser, const std::string& strContact, char* pszFilePath, int nMaxPathLength);
	virtual void OnFileTransferInit(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void OnFileTransferUpdateProgress(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void OnFileTransferCanceled(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void OnFileTransferError(const std::string& strUser, const std::string& strContact, int nError);

	virtual void OnContactExtendedCapabilitiesChanged(const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities);

	virtual RequestResult OnRequestUserAuthorization(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam);
	virtual void OnNotificationMessageReceived(const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam);
	
	virtual RequestResult OnRequestAddContact(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam);
	virtual void OnNudgeReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText);
	
	// Custom Smiley (Emoticons)
	virtual CustomEmoticonAddResult OnCustomSmileyAdd(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const int bRemote);
	virtual void OnCustomSmileyWrite(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension);
	virtual void OnCustomSmileyClose(const std::string& strUser, const std::string& strContact, const std::string& strSmiley);
	virtual void OnIMReceived(const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList);
	
	// Chat or Conference
	virtual void OnChatAddUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact);
	virtual void OnChatRenameUser(const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias);
	virtual void OnChatRemoveUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName);
	virtual void OnChatUpdateUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact);
    virtual void OnChatReceived(const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList);
	virtual void OnChatSent(const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText);
	virtual void OnChatSendError(const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError);
	
	// Assyncronous File Transfer.
	virtual RequestResult OnRequestAcceptFileAsync(const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	
	// Contact request authorization enablement.
	virtual void OnContactCanRequestAuthorization(const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization);

protected:
	ILibPurpleToUIMThreadSerializer* GetCallbackSerializer();
	ILibPurpleToUIMThreadSerializer* GetCallbackSerializer(int iSerializerType);

private:
	CPurpleProtocolStack*		m_pPurpleProtocolStack;	
};

