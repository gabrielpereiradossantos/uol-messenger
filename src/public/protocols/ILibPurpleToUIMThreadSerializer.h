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
 

#ifndef I_LIBPURPLE_TO_UIM_THREAD_SERIALIZER_H
#define I_LIBPURPLE_TO_UIM_THREAD_SERIALIZER_H

#include "IMProtocolCallback.h"

#include <string>


class ILibPurpleToUIMThreadSerializer
{
public:
	virtual ~ILibPurpleToUIMThreadSerializer() {};
	
	virtual BOOL Init() = 0;
	virtual BOOL Finalize() = 0;
	
	virtual void HandleOnConnected(CIMProtocolCallback* pCallback, const std::string& strUser) = 0;
	virtual void HandleOnDisconnected(CIMProtocolCallback* pCallback, const std::string& strUser) = 0;
	virtual void HandleOnReportDisconnect(CIMProtocolCallback* pCallback, const std::string& strUser, ConnectionError errorCode, const std::string& strMessage) = 0;
	
	virtual void HandleOnNotifyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	
	virtual void HandleOnContactListUpdate(CIMProtocolCallback* pCallback, const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup) = 0;
	virtual void HandleOnContactRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strGroup) = 0;
	virtual void HandleOnContactMoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup) = 0;
	virtual void HandleOnContactGotInfo(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strInfo) = 0;
	virtual void HandleOnContactIconReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen) = 0;
	virtual void HandleOnContactAliasReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strAlias) = 0;
	
	virtual void HandleOnGroupAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup) = 0;
	virtual void HandleOnGroupRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup) = 0;
	virtual void HandleOnGroupRenamed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup) = 0;
	
	virtual void HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText) = 0;
	virtual void HandleOnIMSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, const std::string& strText) = 0;
	virtual void HandleOnIMSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError) = 0;
	
	virtual void HandleOnContactSignedOn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnContactSignedOff(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnContactAway(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnContactAwayReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnContactIdle(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnContactIdleReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnContactStatusChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nStatus) = 0;
	virtual void HandleOnContactStatusChanged(CIMProtocolCallback7* pCallback, const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage) = 0;
	virtual void HandleOnAddContactDenied(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strMessage) = 0;
	
	virtual void HandleOnContactTyping(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, TypingState state) = 0;
	
	virtual void HandleOnConversationWindowClosed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnConversationTimeout(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	
	virtual void HandleOnPermitAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnDenyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnPermitRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	virtual void HandleOnDenyRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact) = 0;
	
	virtual void HandleOnNotifyContactAction(CIMProtocolCallback* pCallback, const std::string& strUser, NotifyReason reason, const std::string& strContact) = 0;
	
	virtual void HandleOnFileTransferInit(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo) = 0;
	virtual void HandleOnFileTransferUpdateProgress(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo) = 0;
	virtual void HandleOnFileTransferCanceled(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo) = 0;
	virtual void HandleOnFileTransferError(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nError) = 0;
	
	virtual void HandleOnContactExtendedCapabilitiesChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities) = 0;
	
	virtual void HandleOnNotificationMessageReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam) = 0;
	
	virtual void HandleOnNudgeReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText) = 0;
	
	virtual void HandleOnCustomSmileyWrite(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension) = 0;
	virtual void HandleOnCustomSmileyClose(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley) = 0;
	virtual void HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList) = 0;
	
	virtual void HandleOnChatAddUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact) = 0;
	virtual void HandleOnChatRenameUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias) = 0;
	virtual void HandleOnChatRemoveUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName) = 0;
	virtual void HandleOnChatUpdateUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact) = 0;
    virtual void HandleOnChatReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList) = 0;
	virtual void HandleOnChatSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText) = 0;
	virtual void HandleOnChatSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError) = 0;
	
	virtual RequestResult HandleOnRequestAcceptFileAsync(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo) = 0;
	
	virtual void HandleOnContactCanRequestAuthorization(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization) = 0;
};


#endif // I_LIBPURPLE_TO_UIM_THREAD_SERIALIZER_H

