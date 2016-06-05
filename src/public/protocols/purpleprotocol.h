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
 
#ifndef _PURPLE_PROTOCOL_H_
#define _PURPLE_PROTOCOL_H_

#include "improtocol.h"

#include <string>
#include <map>

#include <glib.h>

#include "purpleconsts.h"
#include "purpleservicecallback.h"
#include "callbacknotifier.h"

// Purple headers
#include <ft.h>
#include <account.h>
#include <blist.h>
#include <notify.h>
#include <request.h>

#include <atlsync.h>


class CPurpleProtocol : public CIMProtocol7,
						public CPurpleServiceCallback
{
public:
	CPurpleProtocol(const std::string& strProtocolId, const std::string& strProtocolName);
	virtual ~CPurpleProtocol();

	virtual unsigned GetProtocolPos();

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

	virtual bool IsValidUserName(const std::string& strUser);

	virtual IMProtocolError Connect(const std::string& strUser, const std::string& strPass, CProtocolSettings* pExtraSettings = NULL, CProxyInfo* pProxyInfo = NULL);
	virtual IMProtocolError Disconnect(const std::string& strUser);
	virtual int IsConnected(const std::string& strUser);

	virtual IMProtocolError ChangePassword(const std::string& strUser, const std::string& strOldPassword, const std::string& strNewPassword);

	virtual IMProtocolError AddContact(const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	virtual IMProtocolError RemoveContact(const std::string& strUser, const std::string& strContact, const std::string& strGroup);
	virtual IMProtocolError AliasContact(const std::string& strUser, const std::string& strContact, const std::string& strAlias);
	virtual IMProtocolError MoveContact(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup);
	virtual IMProtocolError GetContactInfo(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError CanRequestContactAuthorization(const std::string& strUser, const std::string& strContact, bool& bCanRequest);
	virtual IMProtocolError RequestContactAuthorization(const std::string& strUser, const std::string& strContact);


	virtual IMProtocolError RenameGroup(const std::string& strUser, const std::string& strOldName, const std::string& strNewName);
	virtual IMProtocolError RemoveGroup(const std::string& strUser, const std::string& strGroup);

	virtual IMProtocolError SetAlias(const std::string& strUser, const std::string& strAlias);

	virtual void GetIconSpecification(CIconSpec& iconSpec);
	virtual IMProtocolError SetUserDisplayIcon(const std::string& strUser, const std::string& strFile);

	virtual IMProtocolError SetUserStatus(const std::string& strUser, int nStatus);

	virtual IMProtocolError SendIM(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage);
	
	virtual IMProtocolError SendTyping(const std::string& strFromUser, const std::string& strToUser, TypingState state);
	virtual IMProtocolError SendConversationWindowClosed(const std::string& strFromUser, const std::string& strToUser);
	virtual IMProtocolError SetIdle(const std::string& strUser, int nSeconds);

	virtual IMProtocolError GetDenyContactsList(const std::string& strUser, CPrivacyList& listContacts);
	virtual IMProtocolError GetPermitContactsList(const std::string& strUser, CPrivacyList& listContacts);
	virtual IMProtocolError AddDenyContact(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError AddPermitContact(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError RemoveDenyContact(const std::string& strUser, const std::string& strContact);
	virtual IMProtocolError RemovePermitContact(const std::string& strUser, const std::string& strContact);
	
	//File Transfer
	virtual IMProtocolError SendFile(const std::string& strFromUser, const std::string& strToUser, const std::string& strFilePath);
	virtual IMProtocolError CancelFileTransfer(const std::string& strUser, long lFileTransferId);
	virtual IMProtocolError CancelFileTransfers(const std::string& strUser, const std::string& strContact);

	virtual IMProtocolError Connect(const std::string& strUser, const std::string& strPass, CProtocolSettings* pExtraSettings = NULL, CProxyInfo* pProxyInfo = NULL, CProtocolSettings2* pExtendedCapabilities = NULL);
	virtual IMProtocolError SetExtendedCapabilities(const std::string& strUser, CProtocolSettings2* pExtendedCapabilities);

	virtual bool HasContactListSupport();

	virtual bool HasCommandSupport(const std::string& strCmdId);
	virtual IMProtocolError SendCommand(const std::string& strCmdId, const std::string& strCmdParameters, const std::string& strFromUser, const std::string& strToUser);
	virtual bool FreeToSendCommand(const std::string& strCmdId, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError);
	virtual std::string GetProtocolCommandName(const std::string& strCmdId);

	/*
	virtual bool HasSendAttentionSupport(int nAttentionType);
	virtual IMProtocolError SendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser);
	virtual bool FreeToSendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError);
	*/
	
	virtual bool HasChatSupport();
	virtual IMProtocolError ChatInvite(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage, int& nChatId, bool& bNewChatInvite, const std::string& strChatInviteMenuName);
	virtual IMProtocolError ChatLeave(const std::string& strFromUser, const int& nChatId);
	virtual IMProtocolError ChatSend(const std::string& strFromUser, const std::string& strMessage, const int& nChatId);
    virtual std::string GetChatInviteMenuName();
	
	virtual bool HasCustomEmoticonSupport();
	/*
	virtual IMProtocolError AddCustomEmoticonToAccount(const std::string& strFromUser, const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName);
	virtual IMProtocolError RemoveCustomEmoticonFromAccount(const std::string& strFromUser, const std::string& strCustomEmoticonShortcut);
	*/
	virtual IMProtocolError AddCustomEmoticon(const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName);
	virtual IMProtocolError RemoveCustomEmoticon(const std::string& strCustomEmoticonShortcut);
	virtual IMProtocolError ChangeCustomEmoticonShortcut(const std::string& strCustomEmoticonOldShortcut, const std::string& strCustomEmoticonNewShortcut);

	virtual IMProtocolError AcceptFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	virtual IMProtocolError RejectFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	
	virtual IMProtocolError DisconnectAsync(const std::string& strUser);
	virtual IMProtocolError SetAliasAsync(const std::string& strUser, const std::string& strAlias);
	virtual IMProtocolError SetUserDisplayIconAsync(const std::string& strUser, const std::string& strFile);
	virtual IMProtocolError SetUserStatusAsync(const std::string& strUser, int nStatus);
	virtual IMProtocolError AliasContactAsync(const std::string& strUser, const std::string& strContact, const std::string& strAlias);
	
	virtual bool HasUserAliasSupport();

	// CIMProtocol6
	virtual IMProtocolError SetUserStatus(const std::string& strUser, int nStatus, const std::string& strStatusMessage);
	virtual IMProtocolError SetUserStatusAsync(const std::string& strUser, int nStatus, const std::string& strStatusMessage);

	// CIMProtocol7
	virtual IMProtocolError SetExtendedCapabilitiesAsync(const std::string& strUser, CProtocolSettings2* pExtendedCapabilities);
	
	virtual bool HasOfflineMessageSupport();
	
	// Purple service callbacks
	virtual void OnConnectProgress(PurpleAccount* pAccount, const char* pszText, int nStep, int nStepCount);
	virtual void OnConnected(PurpleAccount* pAccount);
	virtual void OnDisconnected(PurpleAccount* pAccount);
	virtual void OnReportDisconnect(PurpleAccount* pAccount, const char* pszText);

	virtual void OnNotifyAdded(PurpleAccount* pAccount, const char* pszContact);

	virtual void OnContactListUpdate(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleGroup* pGroup);
	virtual void OnContactRemoved(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleGroup* pGroup);
	virtual void OnContactIconReceived(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleBuddyIcon* pBuddyIcon);
	virtual void OnContactAliasReceived(PurpleAccount* pAccount, PurpleBuddy* pBuddy, const char* pszAlias);
	
	virtual void OnGroupAdded(PurpleGroup* pGroup);

	virtual void OnIMReceived(PurpleAccount* pAccount, const char* pszFromUser, const char* pszText);
	virtual void OnIMSent(PurpleAccount* pAccount, const char* pszToUser, const char* pszText);

	virtual void OnContactSignedOn(PurpleAccount* pAccount, PurpleBuddy* pBuddy);
	virtual void OnContactSignedOff(PurpleAccount* pAccount, PurpleBuddy* pBuddy);
	virtual void OnContactAway(PurpleAccount* pAccount, PurpleBuddy* pBuddy);
	virtual void OnContactAwayReturn(PurpleAccount* pAccount, PurpleBuddy* pBuddy);
	virtual void OnContactIdle(PurpleAccount* pAccount, PurpleBuddy* pBuddy);
	virtual void OnContactIdleReturn(PurpleAccount* pAccount, PurpleBuddy* pBuddy);
	virtual void OnContactTyping(PurpleAccount* pAccount, const char* pszFromUser, PurpleTypingState purpleTypingState);
	virtual void OnContactStatusChanged(PurpleAccount* pAccount, PurpleBuddy* pBuddy);

	virtual void OnPermitAdded(PurpleAccount* pAccount, const char* pszUser);
	virtual void OnDenyAdded(PurpleAccount* pAccount, const char* pszUser);
	virtual void OnPermitRemoved(PurpleAccount* pAccount, const char* pszUser);
	virtual void OnDenyRemoved(PurpleAccount* pAccount, const char* pszUser);

	virtual void OnFileTransferAdded(PurpleXfer* pXfer);
	virtual void OnFileTransferDestroyed(PurpleXfer* pXfer);
	virtual void OnFileTransferUpdateProgress(PurpleXfer* pXfer, double dPercent);
	virtual void OnFileTransferCanceled(PurpleXfer* pXfer);

	virtual CustomEmoticonAddResult OnCustomSmileyAdd(PurpleConversation* pConversation, const char* pszSmiley, gboolean bRemote);
	virtual void OnCustomSmileyWrite(PurpleConversation* pConversation, const char* pszSmiley, const guchar* pData, gsize nSize, const char* pszFileExtension);
	virtual void OnCustomSmileyClose(PurpleConversation* pConversation, const char* pszSmiley);
	virtual void OnIMReceived(PurpleAccount* pAccount, const char* pszFromUser, const char* pszText, const char* strSmileyList);

	virtual void OnChatAddUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactName, const char* pszChatContactAlias, const int bIsContact);
	virtual void OnChatRenameUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactOldName, const char* pszChatContactNewName, const char* pszChatContactNewAlias);
	virtual void OnChatRemoveUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactName);
	virtual void OnChatUpdateUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactName, const char* pszChatContactAlias, const int bIsContact);
    virtual void OnChatReceived(PurpleAccount* pAccount, const int nChatId, const char* pszFromUser, const char* pszText, const char* pszSmileyList);
	virtual void OnChatSent(PurpleAccount* pAccount, const int nChatId, const char* pszToUser, const char* pszText);

	// CPurpleServiceCallback implementation
	virtual int ProcessSystemMessage(PurpleAccount* pAccount, MessageParam* pParam);
	virtual int ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam);
	virtual int ProcessAccountRequestAuthorize(AccountRequestAuthorizeParam* pParam);
	virtual void ProcessAccountRequestAuthorizeCancel(AccountRequestAuthorizeParam* pParam);
	virtual int ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam);
	virtual int ProcessNotifyFormatted(PurpleAccount* pAccount, NotifyFormattedParam* pParam);
	virtual int ProcessNotifyUserInfo(PurpleAccount* pAccount, NotifyUserInfoParam* pParam);
	virtual int ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam);
	virtual int ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam);
	virtual int ProcessRequestFileTransferAction(PurpleXfer* pXfer, RequestActionParam* pParam);
	virtual int ProcessRequestFile(PurpleXfer* pXfer, RequestFileParam* pParam);

protected:
	virtual CIMProtocolCallback* GetCallback(const char* pszUser);
	virtual bool GetStatusText(int nStatus, std::string& statusText) = 0;
	virtual bool GetStatusCode(const std::string& statusText, int& nStatus) = 0;

	PurpleAccount* GetPurpleAccount(const char* pszUser);
	PurpleAccount* GetPurpleAccountConnected(const char* pszUser);
	int IsConnected(PurpleAccount* pAccount);	

	int ProcessRequestActionCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const std::string&), const std::string& strUser, const std::string& strContact, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction = -1);
	int ProcessRequestActionCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const std::string&, const std::string&), const std::string& strUser, const std::string& strContact, const std::string& strReason, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction = -1);
	int ProcessRequestInputCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const std::string&), const std::string& strUser, const std::string& strContact/*, const std::string& strReason*/, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction = -1);
	int ProcessRequestRecvFileCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const std::string&, char*, int), PurpleXfer* pXfer, GCallback cbOK, GCallback cbCancel, void* pUserData);
	int ProcessRequestUserAuthorizationCallback(CIMProtocolCallback3* pCallback3, const std::string& strUser, const std::string& strContact, const std::string& strReason, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction);

	PurpleProxyInfo* ProcessProxySettings(CProxyInfo* pProxyInfo);
	virtual void ProcessExtraSettings(PurpleAccount* pAccount, CProtocolSettings* pExtraSettings);
	virtual void ProcessExtendedCapabilities(PurpleAccount* pAccount, CProtocolSettings2* pExtendedCapabilities);
	virtual IMProtocolError UpdateServerExtendedCapabilities(PurpleAccount* pAccount, BOOL bPublishing);

	virtual std::string FormatSendingMessage(const std::string& strMessage);
	virtual std::string FormatSentMessage(const std::string& strMessage);
	virtual std::string FormatReceivedMessage(const std::string& strMessage);

	virtual std::string GetNormalizedPurpleUserName(const std::string& strUserName);
	virtual std::string GetNormalizedCallbackUserName(const char* pszPurpleUserName);
	virtual std::string GetNormalizedContactName(const char* pszContact);

	long AddPurpleXfer(PurpleXfer* pXfer);
	void RemovePurpleXfer(long lTransferId);
	PurpleXfer* GetPurpleXfer(long lTransferId);
	PurpleXfer* GetPurpleXfer(PurpleAccount* pAccount, const std::string& strContact);

	bool ExecuteBListNodeAction(PurpleAccount* pAccount, PurpleBlistNode* pNode, const std::string& strAction);

	virtual bool GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason);

	virtual void AddProtocolToStack(const char* pszUser, CIMProtocolCallback * pProtocolCallback);
	
	PurpleBuddy* GetPurpleBuddy(PurpleAccount* pAccount, const char* pszContact);
	PurpleBuddy* GetPurpleBuddyConnected(PurpleAccount* pAccount, const char* pszContact);

	static void WaitLibPurpleThreadAccess();
	static void ReleaseLibPurpleThreadAccess();
	static gboolean LibPurpleHolderMethod(gpointer data);
	static gboolean SerializedDisconnectMethod(gpointer data);


protected:
	typedef std::map<std::string, CIMProtocolCallback*> CCallbackMap;
	typedef std::map<long, PurpleXfer*> CFileTransferMap;

	const std::string	m_strId;
	const std::string	m_strName;

	CCallbackMap		m_mapCallback;

	long				m_lNextFileTransferId;
	CFileTransferMap	m_mapFileTransfer;

	CCallbackNotifier	m_callbackNotifier;	
	
	static BOOL			m_bEventsInitialized;
	static CEvent		m_libPurpleHoldEvent;
	static CEvent		m_libPurpleReleaseEvent;
	static GMutex*		m_pUIMMultipleThreadSerializeMutex;
	
	GMutex*				m_pAuthorizeCancelationMutex;
	
private:
	struct ConnectionErrorEntry
	{
		ConnectionError errorCode;
		char* pszText;
	};

	static ConnectionErrorEntry ms_pConnectionErrorEntries[];	

	friend class CPurpleService;
};

#endif // _PURPLE_PROTOCOL_H_
