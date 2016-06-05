#pragma once
#include "improtocolcallback.h"

class CPurpleProtocolCallbackStack : public CIMProtocolCallback 								   
{
public:
	CPurpleProtocolCallbackStack(CIMProtocolCallback* pIMProtocolCallbackHead);
	virtual ~CPurpleProtocolCallbackStack(void);

	virtual void AddProtocol(CIMProtocolCallback* pIMProtocolCallback);

	// CIMProtocolCallback
	
	virtual void OnConnected();	
	virtual void OnDisconnected();	
	virtual void OnReportDisconnect(ConnectionError errorCode);	
	virtual void OnNotifyAdded(const std::string& strContact);
	virtual void OnContactListUpdate(const CContactInfo* pContactInfo, const std::string& strGroup);
	virtual void OnContactRemoved(const std::string& strContact, const std::string& strGroup);
	virtual void OnContactMoved(const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup);
	virtual void OnContactGotInfo(const std::string& strContact, const std::string& strInfo);
	virtual void OnContactIconReceived(const std::string& strContact, const char* pIconData, int nLen);
	virtual void OnContactAliasReceived(const std::string& strContact, const std::string& strAlias);
	virtual void OnGroupAdded(const std::string& strGroup);
	virtual void OnGroupRemoved(const std::string& strGroup);
	virtual void OnGroupRenamed(const std::string& strOldGroup, const std::string& strNewGroup);
	virtual void OnIMReceived(const std::string& strFromUser, const std::string& strText);
	virtual void OnIMSent(const std::string& strToUser, const std::string& strText);
	virtual void OnIMSendError(const std::string& strToUser, IMProtocolError sendError);
	virtual void OnContactSignedOn(const std::string& strContact);
	virtual void OnContactSignedOff(const std::string& strContact);
	virtual void OnContactAway(const std::string& strContact);
	virtual void OnContactAwayReturn(const std::string& strContact);
	virtual void OnContactIdle(const std::string& strContact);
	virtual void OnContactIdleReturn(const std::string& strContact);	
	virtual void OnContactTyping(const std::string& strContact, TypingState state);
	virtual void OnConversationWindowClosed(const std::string& strContact);
	virtual void OnConversationTimeout(const std::string& strContact);
	virtual void OnPermitAdded(const std::string& strContact);
	virtual void OnDenyAdded(const std::string& strContact);
	virtual void OnPermitRemoved(const std::string& strContact);
	virtual void OnDenyRemoved(const std::string& strContact);
	virtual RequestResult OnRequestInput(RequestReason reason, char* pszBuffer, int nBufferLength);
	virtual RequestResult OnRequestAddContact(const std::string& strContact);
	virtual RequestResult OnRequestAddUser(const std::string& strContact, const std::string& strAddReason);
	virtual RequestResult OnRequestSendAuthorization(const std::string& strContact);
	virtual void OnNotifyContactAction(NotifyReason reason, const std::string& strContact);	
	virtual RequestResult OnRequestAcceptFile(const std::string& strContact, const std::string& strFilename);
	virtual RequestResult OnRequestFilePath(const std::string& strContact, char* pszFilePath, int nMaxPathLength);
	virtual void OnFileTransferInit(const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void OnFileTransferUpdateProgress(const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);
	virtual void OnFileTransferCanceled(const std::string& strContact, const CFileTransferInfo* pFileTransferInfo);

};
