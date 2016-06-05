#include "./purpleprotocolcallbackstack.h"

CPurpleProtocolCallbackStack::CPurpleProtocolCallbackStack(CIMProtocolCallback* pIMProtocolCallbackHead) :
	CIMProtocolCallback(pIMProtocolCallbackHead)
{
}

CPurpleProtocolCallbackStack::~CPurpleProtocolCallbackStack(void)
{
}

void CPurpleProtocolCallbackStack::AddProtocol(CIMProtocolCallback* pIMProtocolCallback)
{
	pIMProtocolCallback->m_pNext = m_pNext;
	m_pNext = pIMProtocolCallback;
}

void CPurpleProtocolCallbackStack::OnConnected()
{
	m_pNext->OnConnected();
}

void CPurpleProtocolCallbackStack::OnDisconnected()
{
	m_pNext->OnDisconnected();
}

void CPurpleProtocolCallbackStack::OnReportDisconnect(ConnectionError errorCode)
{
	m_pNext->OnReportDisconnect(errorCode);
}

void CPurpleProtocolCallbackStack::OnNotifyAdded(const std::string& strContact)
{
	m_pNext->OnNotifyAdded(strContact);
}

void CPurpleProtocolCallbackStack::OnContactListUpdate(const CContactInfo* pContactInfo, const std::string& strGroup)
{
	m_pNext->OnContactListUpdate(pContactInfo, strGroup);
}

void CPurpleProtocolCallbackStack::OnContactRemoved(const std::string& strContact, const std::string& strGroup)
{
	m_pNext->OnContactRemoved(strContact, strGroup);
}

void CPurpleProtocolCallbackStack::OnContactMoved(const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	m_pNext->OnContactMoved(strContact, strOldGroup, strNewGroup);
}

void CPurpleProtocolCallbackStack::OnContactGotInfo(const std::string& strContact, const std::string& strInfo)
{
	m_pNext->OnContactGotInfo(strContact, strInfo);
}

void CPurpleProtocolCallbackStack::OnContactIconReceived(const std::string& strContact, const char* pIconData, int nLen)
{
	m_pNext->OnContactIconReceived(strContact, pIconData, nLen);
}

void CPurpleProtocolCallbackStack::OnContactAliasReceived(const std::string& strContact, const std::string& strAlias)
{
	m_pNext->OnContactAliasReceived(strContact, strAlias);
}

void CPurpleProtocolCallbackStack::OnGroupAdded(const std::string& strGroup)
{
	m_pNext->OnGroupAdded(strGroup);
}

void CPurpleProtocolCallbackStack::OnGroupRemoved(const std::string& strGroup)
{
	m_pNext->OnGroupRemoved(strGroup);
}

void CPurpleProtocolCallbackStack::OnGroupRenamed(const std::string& strOldGroup, const std::string& strNewGroup)
{
	m_pNext->OnGroupRenamed(strOldGroup, strNewGroup);
}

void CPurpleProtocolCallbackStack::OnIMReceived(const std::string& strFromUser, const std::string& strText)
{
	m_pNext->OnIMReceived(strFromUser, strText);
}

void CPurpleProtocolCallbackStack::OnIMSent(const std::string& strToUser, const std::string& strText)
{
	m_pNext->OnIMSent(strToUser, strText);
}

void CPurpleProtocolCallbackStack::OnIMSendError(const std::string& strToUser, IMProtocolError sendError)
{
	m_pNext->OnIMSendError(strToUser, sendError);
}

void CPurpleProtocolCallbackStack::OnContactSignedOn(const std::string& strContact)
{
	m_pNext->OnContactSignedOn(strContact);
}

void CPurpleProtocolCallbackStack::OnContactSignedOff(const std::string& strContact)
{
	m_pNext->OnContactSignedOff(strContact);
}

void CPurpleProtocolCallbackStack::OnContactAway(const std::string& strContact)
{
	m_pNext->OnContactAway(strContact);
}

void CPurpleProtocolCallbackStack::OnContactAwayReturn(const std::string& strContact)
{
	m_pNext->OnContactAwayReturn(strContact);
}

void CPurpleProtocolCallbackStack::OnContactIdle(const std::string& strContact)
{
	m_pNext->OnContactIdle(strContact);
}

void CPurpleProtocolCallbackStack::OnContactIdleReturn(const std::string& strContact)
{
	m_pNext->OnContactIdleReturn(strContact);
}

void CPurpleProtocolCallbackStack::OnContactTyping(const std::string& strContact, TypingState state)
{
	m_pNext->OnContactTyping(strContact, state);
}

void CPurpleProtocolCallbackStack::OnConversationWindowClosed(const std::string& strContact)
{
	m_pNext->OnConversationWindowClosed(strContact);
}

void CPurpleProtocolCallbackStack::OnConversationTimeout(const std::string& strContact)
{
	m_pNext->OnConversationTimeout(strContact);
}

void CPurpleProtocolCallbackStack::OnPermitAdded(const std::string& strContact)
{
	m_pNext->OnPermitAdded(strContact);
}

void CPurpleProtocolCallbackStack::OnDenyAdded(const std::string& strContact)
{
	m_pNext->OnDenyAdded(strContact);
}

void CPurpleProtocolCallbackStack::OnPermitRemoved(const std::string& strContact)
{
	m_pNext->OnPermitRemoved(strContact);
}

void CPurpleProtocolCallbackStack::OnDenyRemoved(const std::string& strContact)
{
	m_pNext->OnDenyRemoved(strContact);
}

RequestResult CPurpleProtocolCallbackStack::OnRequestInput(RequestReason reason, char* pszBuffer, int nBufferLength)
{
	return m_pNext->OnRequestInput(reason, pszBuffer, nBufferLength);
}

RequestResult CPurpleProtocolCallbackStack::OnRequestAddContact(const std::string& strContact)
{
	return m_pNext->OnRequestAddContact(strContact);
}

RequestResult CPurpleProtocolCallbackStack::OnRequestAddUser(const std::string& strContact, const std::string& strAddReason)
{
	return m_pNext->OnRequestAddUser(strContact, strAddReason);
}

RequestResult CPurpleProtocolCallbackStack::OnRequestSendAuthorization(const std::string& strContact)
{
	return m_pNext->OnRequestSendAuthorization(strContact);
}

void CPurpleProtocolCallbackStack::OnNotifyContactAction(NotifyReason reason, const std::string& strContact)
{
	return m_pNext->OnNotifyContactAction(reason, strContact);
}

RequestResult CPurpleProtocolCallbackStack::OnRequestAcceptFile(const std::string& strContact, const std::string& strFilename)
{
	return m_pNext->OnRequestAcceptFile(strContact, strFilename);
}

RequestResult CPurpleProtocolCallbackStack::OnRequestFilePath(const std::string& strContact, char* pszFilePath, int nMaxPathLength)
{
	return m_pNext->OnRequestFilePath(strContact, pszFilePath, nMaxPathLength);
}

void CPurpleProtocolCallbackStack::OnFileTransferInit(const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	m_pNext->OnFileTransferInit(strContact, pFileTransferInfo);
}

void CPurpleProtocolCallbackStack::OnFileTransferUpdateProgress(const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	m_pNext->OnFileTransferUpdateProgress(strContact, pFileTransferInfo);
}

void CPurpleProtocolCallbackStack::OnFileTransferCanceled(const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	m_pNext->OnFileTransferCanceled(strContact, pFileTransferInfo);
}
