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
 
#ifndef _CALLBACK_NOTIFIER_H_
#define _CALLBACK_NOTIFIER_H_

#include <glib.h>
#include <string>

#include "improtocolconstants.h"
#include "improtocolcallback.h"
#include "purpleconsts.h"

#include <ft.h>


class CCallbackNotifier
{
public:
	CCallbackNotifier();
	virtual ~CCallbackNotifier();

	void HandleOnConnected(CIMProtocolCallback* pCallback, const std::string& strUser);
	void HandleOnDisconnected(CIMProtocolCallback* pCallback, const std::string& strUser);
	void HandleOnReportDisconnect(CIMProtocolCallback* pCallback, const std::string& strUser, ConnectionError errorCode, const std::string& strMessage);
	void HandleOnNotifyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);

	void HandleOnContactListUpdate(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strAlias, const std::string& strGroup);
	void HandleOnContactRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	void HandleOnContactMoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup);
	//void HandleOnContactGotInfo(CIMProtocolCallback* pCallback, const std::string& strContact, const std::string& strInfo);
	void HandleOnContactIconReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen);
	void HandleOnContactAliasReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strAlias);

	void HandleOnGroupAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup);
	void HandleOnGroupRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup);
	void HandleOnGroupRenamed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup);

	void HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strToUser, const std::string& strFromUser, const std::string& strText);
	void HandleOnIMSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, const std::string& strText);
	void HandleOnIMSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError);

	void HandleOnContactSignedOn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnContactSignedOff(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnContactAway(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnContactAwayReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnContactIdle(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnContactIdleReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	
	void HandleOnContactTyping(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, TypingState state);

	//void HandleOnConversationWindowClosed(CIMProtocolCallback* pCallback, const std::string& strContact);
	//void HandleOnConversationTimeout(CIMProtocolCallback* pCallback, const std::string& strContact);

	void HandleOnPermitAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnDenyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnPermitRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	void HandleOnDenyRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);

	//RequestResult HandleOnRequestInput(CIMProtocolCallback* pCallback, RequestReason reason, char* pszBuffer, int nBufferLength);

	//RequestResult HandleOnRequestAddContact(CIMProtocolCallback* pCallback, const std::string& strContact);
	//RequestResult HandleOnRequestAddUser(CIMProtocolCallback* pCallback, const std::string& strContact, const std::string& strAddReason);
	//RequestResult HandleOnRequestSendAuthorization(CIMProtocolCallback* pCallback, const std::string& strContact);

	//void HandleOnNotifyContactAction(CIMProtocolCallback* pCallback, NotifyReason reason, const std::string& strContact);

	//RequestResult HandleOnRequestAcceptFile(CIMProtocolCallback* pCallback, const std::string& strContact, const std::string& strFilename);
	//RequestResult HandleOnRequestFilePath(CIMProtocolCallback* pCallback, const std::string& strContact, char* pszFilePath, int nMaxPathLength);
	//void HandleOnFileTransferInit(CIMProtocolCallback* pCallback, PurpleXfer* pXfer);
	//void HandleOnFileTransferUpdateProgress(CIMProtocolCallback* pCallback, PurpleXfer* pXfer);
	//void HandleOnFileTransferCanceled(CIMProtocolCallback* pCallback, PurpleXfer* pXfer);

private:
	static int HandleVoidParamThread(void* pParam);
	static int HandleParamThread1(void* pParam);
	static int HandleParamThread2(void* pParam);
	static int HandleParamThread3(void* pParam);
	static int HandleConnectionErrorThread(void* pParam);
	static int HandleContactListUpdateThread(void* pParam);
	static int HandleIMSendErrorThread(void* pParam);
	static int HandleContactIconThread(void* pParam);
	static int HandleTypingThread(void* pParam);
	//static int HandleContactActionThread(void* pParam);
	//static int HandleFileTransferThread(void* pParam);
};

#endif // _CALLBACK_NOTIFIER_H_