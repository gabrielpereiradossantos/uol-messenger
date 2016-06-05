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
 

#ifndef _WM_CALLBACK_SERIALIZER_H
#define _WM_CALLBACK_SERIALIZER_H

#include "Atlbase.h"
#include "Atlwin.h"
#include "atlcrack.h"


#include "ILibPurpleToUIMThreadSerializer.h"


class CWMCallbackSerializer : public CWindowImpl<CWMCallbackSerializer>, 
							  public ILibPurpleToUIMThreadSerializer
{
public:
	
	CWMCallbackSerializer();
	virtual ~CWMCallbackSerializer();
	
	virtual BOOL Init();
	virtual BOOL Finalize();
	
protected:
	
	// Interface ILibPurpleToUIMThreadSerializer methods...
	//
	virtual void HandleOnConnected(CIMProtocolCallback* pCallback, const std::string& strUser);
	virtual void HandleOnDisconnected(CIMProtocolCallback* pCallback, const std::string& strUser);
	virtual void HandleOnReportDisconnect(CIMProtocolCallback* pCallback, const std::string& strUser, ConnectionError errorCode, const std::string& strMessage);
	
	virtual void HandleOnNotifyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	
	virtual void HandleOnContactListUpdate(CIMProtocolCallback* pCallback, const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup);
	virtual void HandleOnContactRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	virtual void HandleOnContactMoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup);
	virtual void HandleOnContactGotInfo(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strInfo);
	virtual void HandleOnContactIconReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen);
	virtual void HandleOnContactAliasReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strAlias);
	
	virtual void HandleOnGroupAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup);
	virtual void HandleOnGroupRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup);
	virtual void HandleOnGroupRenamed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup);
	
	virtual void HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText);
	virtual void HandleOnIMSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, const std::string& strText);
	virtual void HandleOnIMSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError);
	
	virtual void HandleOnContactSignedOn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnContactSignedOff(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnContactAway(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnContactAwayReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnContactIdle(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnContactIdleReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnContactStatusChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nStatus);
	virtual void HandleOnContactStatusChanged(CIMProtocolCallback7* pCallback, const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage);
	virtual void HandleOnAddContactDenied(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strMessage);
	
	virtual void HandleOnContactTyping(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, TypingState state);
	
	virtual void HandleOnConversationWindowClosed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnConversationTimeout(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	
	virtual void HandleOnPermitAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnDenyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnPermitRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	virtual void HandleOnDenyRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact);
	
	virtual void HandleOnNotifyContactAction(CIMProtocolCallback* pCallback, const std::string& strUser, NotifyReason reason, const std::string& strContact);
	
	virtual void HandleOnFileTransferInit(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void HandleOnFileTransferUpdateProgress(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void HandleOnFileTransferCanceled(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void HandleOnFileTransferError(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nError);
	
	virtual void HandleOnContactExtendedCapabilitiesChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities);
	
	virtual void HandleOnNotificationMessageReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam);
	
	virtual void HandleOnNudgeReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText);
	
	virtual void HandleOnCustomSmileyWrite(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension);
	virtual void HandleOnCustomSmileyClose(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley);
	virtual void HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList);
	
	virtual void HandleOnChatAddUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact);
	virtual void HandleOnChatRenameUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias);
	virtual void HandleOnChatRemoveUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName);
	virtual void HandleOnChatUpdateUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact);
    virtual void HandleOnChatReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList);
	virtual void HandleOnChatSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText);
	virtual void HandleOnChatSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError);
	
	virtual RequestResult HandleOnRequestAcceptFileAsync(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	
	virtual void HandleOnContactCanRequestAuthorization(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization);
	
protected:
	
	// CWindowImpl message handling methods...
	//
	
	enum WmCallbackSerializerMessages
	{
		WM_WMS_STR = WM_USER + 0X556,
		WM_WMS_STR_CONNECTIONERROR_STR,
		WM_WMS_STR_STR,
		WM_WMS_STR_CONTACTINFO_STR,
		WM_WMS_STR_STR_STR,
		WM_WMS_STR_STR_STR_STR,
		WM_WMS_STR_STR_PCHAR_INT,
		WM_WMS_STR_STR_IMPROTOCOLERROR,
		WM_WMS_STR_STR_INT,
		WM_WMS_STR_STR_TYPINGSTATE,
		WM_WMS_STR_NOTIFYREASON_STR,
		WM_WMS_STR_STR_PFILETRANSFERINFO,
		WM_WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2,
		WM_WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM,
		WM_WMS_CBK4_STR_STR_STR,
		WM_WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR,
		WM_WMS_CBK5_STR_STR_STR,
		WM_WMS_CBK5_STR_STR_STR_STR,
		WM_WMS_CBK6_STR_INT_STR_STR_INT,
		WM_WMS_CBK6_STR_INT_STR_STR_STR,
		WM_WMS_CBK6_STR_INT_STR,
		WM_WMS_CBK6_STR_INT_STR_STR,
		WM_WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR,
		WM_WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC,
		WM_WMS_CBK6_STR_STR_INT,
		WM_WMS_CBK7_STR_STR_INT_STR
	};
	
	BEGIN_MSG_MAP(CWMCallbackSerializer)
		MESSAGE_HANDLER_EX(WM_WMS_STR, OnWmsStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_CONNECTIONERROR_STR, OnWmsStrConnectionErrorStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR, OnWmsStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_CONTACTINFO_STR, OnWmsStrContactInfoStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_STR, OnWmsStrStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_STR_STR, OnWmsStrStrStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_PCHAR_INT, OnWmsStrStrPcharInt)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_IMPROTOCOLERROR, OnWmsStrStrIMProtocolError)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_INT, OnWmsStrStrInt)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_TYPINGSTATE, OnWmsStrStrTypingState)
		MESSAGE_HANDLER_EX(WM_WMS_STR_NOTIFYREASON_STR, OnWmsStrNotifyReasonStr)
		MESSAGE_HANDLER_EX(WM_WMS_STR_STR_PFILETRANSFERINFO, OnWmsStrStrPFileTransferInfo)
		MESSAGE_HANDLER_EX(WM_WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2, OnWmsCbk2StrStrPProtocolSettings2)
		MESSAGE_HANDLER_EX(WM_WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM, OnWmsCbk3StrNotificationMessageParam)
		MESSAGE_HANDLER_EX(WM_WMS_CBK4_STR_STR_STR, OnWmsCbk4StrStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR, OnWmsCbk5StrStrStrPucharIntStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK5_STR_STR_STR, OnWmsCbk5StrStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK5_STR_STR_STR_STR, OnWmsCbk5StrStrStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_INT_STR_STR_INT, OnWmsCbk6StrIntStrStrInt)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_INT_STR_STR_STR, OnWmsCbk6StrIntStrStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_INT_STR, OnWmsCbk6StrIntStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_INT_STR_STR, OnWmsCbk6StrIntStrStr)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR, OnWmsCbk6StrIntStrIMProtocolError)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC, OnWmsCbk6StrStrRequestFileTransferAsync)
		MESSAGE_HANDLER_EX(WM_WMS_CBK6_STR_STR_INT, OnWmsCbk6StrStrInt)
		MESSAGE_HANDLER_EX(WM_WMS_CBK7_STR_STR_INT_STR, OnWmsCbk7StrStrIntStr)
	END_MSG_MAP()
	
	LRESULT OnWmsStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrConnectionErrorStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrContactInfoStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrPcharInt(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrIMProtocolError(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrInt(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrTypingState(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrNotifyReasonStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsStrStrPFileTransferInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk2StrStrPProtocolSettings2(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk3StrNotificationMessageParam(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk4StrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk5StrStrStrPucharIntStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk5StrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk5StrStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrIntStrStrInt(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrIntStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrIntStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrIntStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrIntStrIMProtocolError(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrStrRequestFileTransferAsync(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk6StrStrInt(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmsCbk7StrStrIntStr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
private:
	
	// Utilitary methods...
	//
	
	CContactInfo* CloneContactInfo(const CContactInfo* pOriginalContactInfo);
	
	void* DuplicateData(void* pOriginalData, int nOriginalDataLength);
	
	void LogMethodInfo(void* pWMSParam, const std::string methodName);
	
private:
	
	BOOL			m_bInitialized;
	
	BOOL			m_bIsTerminating;
};


#endif // _WM_CALLBACK_SERIALIZER_H

