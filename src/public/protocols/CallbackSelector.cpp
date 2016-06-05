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


#include ".\CallbackSelector.h"

#include "LibPurpleToUIMSerializerFactory.h"


CCallbackSelector::CCallbackSelector(CPurpleProtocolStack* pPurpleProtocolStack) :	
	m_pPurpleProtocolStack(pPurpleProtocolStack)
{	
}

CCallbackSelector::~CCallbackSelector(void)
{
}

void CCallbackSelector::OnConnected(const std::string& strUser)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer(CLibPurpleToUIMSerializerFactory::SERIALIZER_TYPE_WM)->HandleOnConnected(pCallback, strUser);
}

void CCallbackSelector::OnDisconnected(const std::string& strUser)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnDisconnected(pCallback, strUser);
}

void CCallbackSelector::OnReportDisconnect(const std::string& strUser, ConnectionError errorCode, const std::string& strMessage)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnReportDisconnect(pCallback, strUser, errorCode, strMessage);
}

void CCallbackSelector::OnNotifyAdded(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnNotifyAdded(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactListUpdate(const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactListUpdate(pCallback, strUser, pContactInfo, strGroup);
}

void CCallbackSelector::OnContactRemoved(const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactRemoved(pCallback, strUser, strContact, strGroup);
}

void CCallbackSelector::OnContactMoved(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactMoved(pCallback, strUser, strContact, strOldGroup, strNewGroup);
}

void CCallbackSelector::OnContactGotInfo(const std::string& strUser, const std::string& strContact, const std::string& strInfo)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactGotInfo(pCallback, strUser, strContact, strInfo);
}

void CCallbackSelector::OnContactIconReceived(const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactIconReceived(pCallback, strUser, strContact, pIconData, nLen);
}

void CCallbackSelector::OnContactAliasReceived(const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactAliasReceived(pCallback, strUser, strContact, strAlias);
}

void CCallbackSelector::OnAddContactDenied(const std::string& strUser, const std::string& strContact, const std::string& strMessage)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnAddContactDenied(pCallback, strUser, strContact, strMessage);
}

void CCallbackSelector::OnGroupAdded(const std::string& strUser, const std::string& strGroup)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnGroupAdded(pCallback, strUser, strGroup);
}

void CCallbackSelector::OnGroupRemoved(const std::string& strUser, const std::string& strGroup)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnGroupRemoved(pCallback, strUser, strGroup);
}

void CCallbackSelector::OnGroupRenamed(const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnGroupRenamed(pCallback, strUser, strOldGroup, strNewGroup);
}

void CCallbackSelector::OnIMReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnIMReceived(pCallback, strUser, strFromUser, strText);
}

void CCallbackSelector::OnIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strFromUser.c_str());
	
	GetCallbackSerializer()->HandleOnIMSent(pCallback, strFromUser, strToUser, strText);
}

void CCallbackSelector::OnIMSendError(const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strFromUser.c_str());
	
	GetCallbackSerializer()->HandleOnIMSendError(pCallback, strFromUser, strToUser, sendError);
}

void CCallbackSelector::OnContactSignedOn(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactSignedOn(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactSignedOff(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactSignedOff(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactAway(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactAway(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactAwayReturn(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactAwayReturn(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactIdle(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactIdle(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactIdleReturn(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactIdleReturn(pCallback, strUser, strContact);
}

void CCallbackSelector::OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus)
{
	OnContactStatusChanged(strUser, strContact, nStatus, "");
}

void CCallbackSelector::OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		CIMProtocolCallback7* pCallback7 = dynamic_cast<CIMProtocolCallback7*>(pCallback);
		
		if (pCallback7)
		{
			GetCallbackSerializer()->HandleOnContactStatusChanged(pCallback7, strUser, strContact, nStatus, strStatusMessage);
		}
	}
}

void CCallbackSelector::OnContactTyping(const std::string& strUser, const std::string& strContact, TypingState state)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactTyping(pCallback, strUser, strContact, state);
}

void CCallbackSelector::OnConversationWindowClosed(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnConversationWindowClosed(pCallback, strUser, strContact);
}

void CCallbackSelector::OnConversationTimeout(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnConversationTimeout(pCallback, strUser, strContact);
}

void CCallbackSelector::OnPermitAdded(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnPermitAdded(pCallback, strUser, strContact);
}

void CCallbackSelector::OnDenyAdded(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnDenyAdded(pCallback, strUser, strContact);
}

void CCallbackSelector::OnPermitRemoved(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnPermitRemoved(pCallback, strUser, strContact);
}

void CCallbackSelector::OnDenyRemoved(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnDenyRemoved(pCallback, strUser, strContact);
}

RequestResult CCallbackSelector::OnRequestInput(const std::string& strUser, RequestReason reason, char* pszBuffer, int nBufferLength)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		return pCallback->OnRequestInput(strUser, reason, pszBuffer, nBufferLength);
	}
	
	return REQUEST_RESULT_ERROR;
}

RequestResult CCallbackSelector::OnRequestAddContact(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		return pCallback->OnRequestAddContact(strUser, strContact);
	}
	
	return REQUEST_RESULT_ERROR;
}

RequestResult CCallbackSelector::OnRequestAddUser(const std::string& strUser, const std::string& strContact, const std::string& strAddReason)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		return pCallback->OnRequestAddUser(strUser, strContact, strAddReason);
	}
	
	return REQUEST_RESULT_ERROR;
}

RequestResult CCallbackSelector::OnRequestSendAuthorization(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		return pCallback->OnRequestSendAuthorization(strUser, strContact);
	}
	
	return REQUEST_RESULT_ERROR;
}

void CCallbackSelector::OnNotifyContactAction(const std::string& strUser, NotifyReason reason, const std::string& strContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnNotifyContactAction(pCallback, strUser, reason, strContact);
}

RequestResult CCallbackSelector::OnRequestAcceptFile(const std::string& strUser, const std::string& strContact, const std::string& strFilename)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		return pCallback->OnRequestAcceptFile(strUser, strContact, strFilename);
	}
	
	return REQUEST_RESULT_ERROR;
}

RequestResult CCallbackSelector::OnRequestFilePath(const std::string& strUser, const std::string& strContact, char* pszFilePath, int nMaxPathLength)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		return pCallback->OnRequestFilePath(strUser, strContact, pszFilePath, nMaxPathLength);
	}
	
	return REQUEST_RESULT_ERROR;
}

void CCallbackSelector::OnFileTransferInit(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnFileTransferInit(pCallback, strUser, strContact, pFileTransferInfo);
}

void CCallbackSelector::OnFileTransferUpdateProgress(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnFileTransferUpdateProgress(pCallback, strUser, strContact, pFileTransferInfo);
}

void CCallbackSelector::OnFileTransferCanceled(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnFileTransferCanceled(pCallback, strUser, strContact, pFileTransferInfo);
}

void CCallbackSelector::OnFileTransferError(const std::string& strUser, const std::string& strContact, int nError)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnFileTransferError(pCallback, strUser, strContact, nError);
}

void CCallbackSelector::OnContactExtendedCapabilitiesChanged(const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactExtendedCapabilitiesChanged(pCallback, strUser, strContact, pExtendedCapabilities);
}


RequestResult CCallbackSelector::OnRequestUserAuthorization(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		CIMProtocolCallback3* pCallback3 = dynamic_cast<CIMProtocolCallback3*>(pCallback);
		
		if (pCallback3)
		{
			return pCallback3->OnRequestUserAuthorization(strUser, pRequestUserAuthorizationParam);
		}
	}
	
	return REQUEST_RESULT_ERROR;
}


void CCallbackSelector::OnNotificationMessageReceived(const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnNotificationMessageReceived(pCallback, strUser, pNotificationMessageParam);
}


RequestResult CCallbackSelector::OnRequestAddContact(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		CIMProtocolCallback4* pCallback4 = dynamic_cast<CIMProtocolCallback4*>(pCallback);
		
		if (pCallback4)
		{
			return pCallback4->OnRequestAddContact(strUser, pRequestUserAuthorizationParam);
		}
	}
	
	return REQUEST_RESULT_ERROR;
}


void CCallbackSelector::OnNudgeReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnNudgeReceived(pCallback, strUser, strFromUser, strText);
}


CustomEmoticonAddResult CCallbackSelector::OnCustomSmileyAdd(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const int bRemote)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	if (pCallback)
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			return pCallback5->OnCustomSmileyAdd(strUser, strContact, strSmiley, bRemote);
		}
	}
	
	return CUSTOM_EMOTICON_ADD_RESULT_ERROR;
}


void CCallbackSelector::OnCustomSmileyWrite(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnCustomSmileyWrite(pCallback, strUser, strContact, strSmiley, pSmileyData, nSmileyDataSize, strFileExtension);
}


void CCallbackSelector::OnCustomSmileyClose(const std::string& strUser, const std::string& strContact, const std::string& strSmiley)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnCustomSmileyClose(pCallback, strUser, strContact, strSmiley);
}

void CCallbackSelector::OnIMReceived(const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnIMReceived(pCallback, strUser, strContact, strText, strSmileyList);
}

void CCallbackSelector::OnChatAddUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatAddUser(pCallback, strUser, nChatId, strChatContactName, strChatContactAlias, bIsContact);
}

void CCallbackSelector::OnChatRenameUser(const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatRenameUser(pCallback, strUser, nChatId, strChatContactOldName, strChatContactNewName, strChatContactNewAlias);
}

void CCallbackSelector::OnChatRemoveUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatRemoveUser(pCallback, strUser, nChatId, strChatContactName);
}

void CCallbackSelector::OnChatUpdateUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatUpdateUser(pCallback, strUser, nChatId, strChatContactName, strChatContactAlias, bIsContact);
}

void CCallbackSelector::OnChatReceived(const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatReceived(pCallback, strUser, nChatId, strFromUser, strText, strSmileyList);
}

void CCallbackSelector::OnChatSent(const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strFromUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatSent(pCallback, strFromUser, nChatId, strToUser, strText);
}

void CCallbackSelector::OnChatSendError(const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strFromUser.c_str());
	
	GetCallbackSerializer()->HandleOnChatSendError(pCallback, strFromUser, nChatId, strToUser, sendError);
}

RequestResult CCallbackSelector::OnRequestAcceptFileAsync(const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	return GetCallbackSerializer()->HandleOnRequestAcceptFileAsync(pCallback, strUser, strContact, pFileTransferAsyncInfo);
}

void CCallbackSelector::OnContactCanRequestAuthorization(const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization)
{
	CIMProtocolCallback* pCallback = m_pPurpleProtocolStack->GetCallback(strUser.c_str());
	
	GetCallbackSerializer()->HandleOnContactCanRequestAuthorization(pCallback, strUser, strContact, bCanRequestAuthorization);
}

ILibPurpleToUIMThreadSerializer* CCallbackSelector::GetCallbackSerializer()
{
	return CLibPurpleToUIMSerializerFactory::GetFactoryInstance()->GetDefaultCallbackSerializer();
}


ILibPurpleToUIMThreadSerializer* CCallbackSelector::GetCallbackSerializer(int iSerializerType)
{
	return CLibPurpleToUIMSerializerFactory::GetFactoryInstance()->GetCallbackSerializer(iSerializerType);
}

