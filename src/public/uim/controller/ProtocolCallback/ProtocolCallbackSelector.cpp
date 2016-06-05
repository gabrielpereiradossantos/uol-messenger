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

#include "stdafx.h"
#include "ProtocolCallbackSelector.h"

#include "../UIMApplication.h"
#include "../AccountConnection/AccountConnectionManager.h"
#include "../RemoteContactList/RemoteContactList.h"
#include "../Conversation/ConversationManager.h"
#include "../Privacy/PrivacyManager.h"
#include "../FileTransfer/FileTransferManager.h"


using namespace std;


CProtocolCallbackSelector::CProtocolCallbackSelector(IUOLMessengerAccountPtr pAccount) :
	m_pAccount(pAccount)
{
	ATLASSERT(m_pAccount);
}


CProtocolCallbackSelector::~CProtocolCallbackSelector()
{
}


IUOLMessengerAccountPtr CProtocolCallbackSelector::GetAccount()
{
	return m_pAccount;
}


void CProtocolCallbackSelector::OnConnected(const std::string& strUser)
{
	CAccountConnectionManagerPtr pAccountConnectionManager = ap_dynamic_cast<CAccountConnectionManagerPtr>(CUIMApplication::GetApplication()->GetAccountConnectionManager());

	if (pAccountConnectionManager)
	{
		pAccountConnectionManager->NotifyConnected(m_pAccount);
	}
}

void CProtocolCallbackSelector::OnDisconnected(const std::string& strUser)
{
	CAccountConnectionManagerPtr pAccountConnectionManager = ap_dynamic_cast<CAccountConnectionManagerPtr>(CUIMApplication::GetApplication()->GetAccountConnectionManager());

	if (pAccountConnectionManager)
	{
		pAccountConnectionManager->NotifyDisconnected(m_pAccount);
	}
}

void CProtocolCallbackSelector::OnReportDisconnect(const std::string& strUser, ConnectionError errorCode, const std::string& strMessage)
{
	CAccountConnectionManagerPtr pAccountConnectionManager = ap_dynamic_cast<CAccountConnectionManagerPtr>(CUIMApplication::GetApplication()->GetAccountConnectionManager());

	if (pAccountConnectionManager)
	{
		pAccountConnectionManager->NotifyReportDisconnect(m_pAccount, errorCode, strMessage.c_str());
	}
}





void CProtocolCallbackSelector::OnNotifyAdded(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyUserAdded(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactListUpdate(const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactListUpdate(m_pAccount, pContactInfo, strGroup.c_str());
	}
}

void CProtocolCallbackSelector::OnContactRemoved(const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactRemoved(m_pAccount, strContact.c_str(), strGroup.c_str());
	}
}

void CProtocolCallbackSelector::OnContactMoved(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactMoved(m_pAccount, strContact.c_str(), strOldGroup.c_str(), strNewGroup.c_str());
	}
}

void CProtocolCallbackSelector::OnContactGotInfo(const std::string& strUser, const std::string& strContact, const std::string& strInfo)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactGotInfo(m_pAccount, strContact.c_str(), strInfo.c_str());
	}
}

void CProtocolCallbackSelector::OnContactIconReceived(const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactIconReceived(m_pAccount, strContact.c_str(), pIconData, nLen);
	}
}

void CProtocolCallbackSelector::OnContactAliasReceived(const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactAliasReceived(m_pAccount, strContact.c_str(), strAlias.c_str());
	}
}

void CProtocolCallbackSelector::OnGroupAdded(const std::string& strUser, const std::string& strGroup)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyGroupAdded(m_pAccount, strGroup.c_str());
	}
}

void CProtocolCallbackSelector::OnGroupRemoved(const std::string& strUser, const std::string& strGroup)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyGroupRemoved(m_pAccount, strGroup.c_str());
	}
}

void CProtocolCallbackSelector::OnGroupRenamed(const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyGroupRenamed(m_pAccount, strOldGroup.c_str(), strNewGroup.c_str());
	}
}

void CProtocolCallbackSelector::OnContactSignedOn(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactSignedOn(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactSignedOff(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactSignedOff(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactAway(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactAway(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactAwayReturn(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactAwayReturn(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactIdle(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactIdle(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactIdleReturn(const std::string& strUser, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactIdleReturn(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus)
{
	OnContactStatusChanged(strUser, strContact, nStatus, "");
}

void CProtocolCallbackSelector::OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactStatusChanged(m_pAccount, strContact.c_str(), nStatus, strStatusMessage.c_str());
	}
}

void CProtocolCallbackSelector::OnAddContactDenied(const std::string& strUser, const std::string& strContact, const std::string& strMessage)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyAddContactDenied(m_pAccount, strContact.c_str(), strMessage.c_str());
	}
}

void CProtocolCallbackSelector::OnNotifyContactAction(const std::string& strUser, NotifyReason reason, const std::string& strContact)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactAction(m_pAccount, reason, strContact.c_str());
	}
}



void CProtocolCallbackSelector::OnIMReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyIMReceived(m_pAccount, strFromUser.c_str(), strText.c_str());
	}
}

void CProtocolCallbackSelector::OnIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyIMSent(m_pAccount, strToUser.c_str(), strText.c_str());
	}
}

void CProtocolCallbackSelector::OnIMSendError(const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyIMSendError(m_pAccount, strToUser.c_str(), sendError);
	}
}

void CProtocolCallbackSelector::OnContactTyping(const std::string& strUser, const std::string& strContact, TypingState state)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyContactTyping(m_pAccount, strContact.c_str(), state);
	}
}

void CProtocolCallbackSelector::OnConversationWindowClosed(const std::string& strUser, const std::string& strContact)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyConversationWindowClosed(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnConversationTimeout(const std::string& strUser, const std::string& strContact)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyConversationTimeout(m_pAccount, strContact.c_str());
	}
}



void CProtocolCallbackSelector::OnPermitAdded(const std::string& strUser, const std::string& strContact)
{
	CPrivacyManagerPtr pPrivacyManager = ap_dynamic_cast<CPrivacyManagerPtr>(CUIMApplication::GetApplication()->GetPrivacyManager());

	if (pPrivacyManager)
	{
		pPrivacyManager->NotifyPermitAdded(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnDenyAdded(const std::string& strUser, const std::string& strContact)
{
	CPrivacyManagerPtr pPrivacyManager = ap_dynamic_cast<CPrivacyManagerPtr>(CUIMApplication::GetApplication()->GetPrivacyManager());

	if (pPrivacyManager)
	{
		pPrivacyManager->NotifyDenyAdded(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnPermitRemoved(const std::string& strUser, const std::string& strContact)
{
	CPrivacyManagerPtr pPrivacyManager = ap_dynamic_cast<CPrivacyManagerPtr>(CUIMApplication::GetApplication()->GetPrivacyManager());

	if (pPrivacyManager)
	{
		pPrivacyManager->NotifyPermitRemoved(m_pAccount, strContact.c_str());
	}
}

void CProtocolCallbackSelector::OnDenyRemoved(const std::string& strUser, const std::string& strContact)
{
	CPrivacyManagerPtr pPrivacyManager = ap_dynamic_cast<CPrivacyManagerPtr>(CUIMApplication::GetApplication()->GetPrivacyManager());

	if (pPrivacyManager)
	{
		pPrivacyManager->NotifyDenyRemoved(m_pAccount, strContact.c_str());
	}
}



RequestResult CProtocolCallbackSelector::OnRequestInput(const std::string& strUser, RequestReason reason, char* pszBuffer, int nBufferLength)
{
	RequestResult result = REQUEST_RESULT_CANCEL;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		CString strResponse;
		
		if (pszBuffer[0] != '\0')
		{
			strResponse = pszBuffer;
		}

		result = pUserInputHandler->RequestInput(m_pAccount, reason, strResponse, nBufferLength);

		::strncpy(pszBuffer, strResponse, nBufferLength);
		pszBuffer[nBufferLength - 1] = '\0';
	}

	return result;
}


RequestResult CProtocolCallbackSelector::OnRequestAddContact(const std::string& strUser, const std::string& strContact)
{
	RequestResult result = REQUEST_RESULT_ERROR;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		result = pUserInputHandler->RequestAddContact(m_pAccount, strContact.c_str());
	}

	return result;
}


RequestResult CProtocolCallbackSelector::OnRequestAddUser(const std::string& strUser, const std::string& strContact, const std::string& strAddReason)
{
	RequestResult result = REQUEST_RESULT_CANCEL;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		result = pUserInputHandler->RequestAddUser(m_pAccount, strContact.c_str(), strAddReason.c_str());
	}

	return result;
}


RequestResult CProtocolCallbackSelector::OnRequestSendAuthorization(const std::string& strUser, const std::string& strContact)
{
	RequestResult result = REQUEST_RESULT_CANCEL;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		result = pUserInputHandler->RequestSendAuthorization(m_pAccount, strContact.c_str());
	}

	return result;
}


RequestResult CProtocolCallbackSelector::OnRequestAcceptFile(const std::string& strUser, const std::string& strContact, const std::string& strFilename)
{
	RequestResult result;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		result = pUserInputHandler->RequestAcceptFile(m_pAccount, strContact.c_str(), strFilename.c_str());
	}

	return result;
}


RequestResult CProtocolCallbackSelector::OnRequestFilePath(const std::string& strUser, const std::string& strContact, char* pszFilePath, int nMaxPathLength)
{
	RequestResult result;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		CString strFilePath;
		if (pszFilePath != NULL)
		{
			strFilePath = pszFilePath;
		}

		result = pUserInputHandler->RequestFilePath(m_pAccount, strContact.c_str(), strFilePath, nMaxPathLength);

		::strncpy(pszFilePath, strFilePath, nMaxPathLength);
		pszFilePath[nMaxPathLength - 1] = '\0';
	}

	return result;
}



void CProtocolCallbackSelector::OnFileTransferInit(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CFileTransferManagerPtr pFileTransferManager = ap_dynamic_cast<CFileTransferManagerPtr>(CUIMApplication::GetApplication()->GetFileTransferManager());

	if (pFileTransferManager)
	{
		pFileTransferManager->NotifyFileTransferInit(m_pAccount, strContact.c_str(), pFileTransferInfo);
	}
}

void CProtocolCallbackSelector::OnFileTransferUpdateProgress(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CFileTransferManagerPtr pFileTransferManager = ap_dynamic_cast<CFileTransferManagerPtr>(CUIMApplication::GetApplication()->GetFileTransferManager());

	if (pFileTransferManager)
	{
		pFileTransferManager->NotifyFileTransferUpdateProgress(m_pAccount, strContact.c_str(), pFileTransferInfo);
	}
}

void CProtocolCallbackSelector::OnFileTransferCanceled(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CFileTransferManagerPtr pFileTransferManager = ap_dynamic_cast<CFileTransferManagerPtr>(CUIMApplication::GetApplication()->GetFileTransferManager());

	if (pFileTransferManager)
	{
		pFileTransferManager->NotifyFileTransferCanceled(m_pAccount, strContact.c_str(), pFileTransferInfo);
	}
}

void CProtocolCallbackSelector::OnFileTransferError(const std::string& strUser, const std::string& strContact, int nError)
{
	CFileTransferManagerPtr pFileTransferManager = ap_dynamic_cast<CFileTransferManagerPtr>(CUIMApplication::GetApplication()->GetFileTransferManager());

	if (pFileTransferManager)
	{
		pFileTransferManager->NotifyFileTransferError(m_pAccount, strContact.c_str(), nError);
	}
}

void CProtocolCallbackSelector::OnContactExtendedCapabilitiesChanged(const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());

	if (pRemoteContactList)
	{
		CProtocolSettings2Ptr pNewExtendedCapabilities;

		if (pExtendedCapabilities)
		{
			pNewExtendedCapabilities = pExtendedCapabilities->Clone();
		}

		pRemoteContactList->NotifyContactExtendedCapabilitiesChanged(m_pAccount, strContact.c_str(), pNewExtendedCapabilities);
	}
}

RequestResult CProtocolCallbackSelector::OnRequestUserAuthorization(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam)
{
	RequestResult result = REQUEST_RESULT_CANCEL;

	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();

	if (pUserInputHandler != NULL)
	{
		CUOLMessengerUserInputHandler2Ptr pUserInputHandler2 = ap_dynamic_cast<CUOLMessengerUserInputHandler2Ptr>(pUserInputHandler);

		if (pUserInputHandler2)
		{
			std::string strContact = pRequestUserAuthorizationParam->GetContact();
			std::string strAddReason = pRequestUserAuthorizationParam->GetContactMessage();
            BOOL bAddToContactList = FALSE;
			CString strAddGroup("");

			result = pUserInputHandler2->RequestUserAuthorization(m_pAccount, strContact.c_str(), strAddReason.c_str(), bAddToContactList, strAddGroup);

			pRequestUserAuthorizationParam->SetAddContactFlag(bAddToContactList);
			pRequestUserAuthorizationParam->SetContactGroup((LPCTSTR) strAddGroup);
		}
	}

	return result;
}

void CProtocolCallbackSelector::OnNotificationMessageReceived(const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam)
{
	// TODO: Handle this notification
}

RequestResult CProtocolCallbackSelector::OnRequestAddContact(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam)
{
	RequestResult result = REQUEST_RESULT_CANCEL;
	
	CUOLMessengerUserInputHandlerPtr pUserInputHandler = CUIMApplication::GetApplication()->GetUIManager()->GetDefaultUserInputHandler();
	
	if (pUserInputHandler != NULL)
	{
		CUOLMessengerUserInputHandler3Ptr pUserInputHandler3 = ap_dynamic_cast<CUOLMessengerUserInputHandler3Ptr>(pUserInputHandler);
		
		if (pUserInputHandler3)
		{
			std::string strContact = pRequestUserAuthorizationParam->GetContact();
			std::string strAddReason = pRequestUserAuthorizationParam->GetContactMessage();
			BOOL bAlreadyOnList = pRequestUserAuthorizationParam->GetContactAlreadyOnList();
			BOOL bAddToContactList = FALSE;
			CString strAddGroup("");
			
			result = pUserInputHandler3->RequestAddContact(m_pAccount, strContact.c_str(), strAddReason.c_str(), 
					bAddToContactList, bAlreadyOnList, strAddGroup);
			
			pRequestUserAuthorizationParam->SetAddContactFlag(bAddToContactList);
			pRequestUserAuthorizationParam->SetContactGroup((LPCTSTR) strAddGroup);
		}
	}

	return result;
}

void CProtocolCallbackSelector::OnNudgeReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyNudgeReceived(m_pAccount, strFromUser.c_str(), strText.c_str());
	}
}



CustomEmoticonAddResult CProtocolCallbackSelector::OnCustomSmileyAdd(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const BOOL bRemote)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		return pConversationManager->ConfirmCustomSmileyAdd(m_pAccount, strContact.c_str(), strSmiley.c_str(), bRemote);
	}
	
	return CUSTOM_EMOTICON_ADD_RESULT_ERROR;
}

void CProtocolCallbackSelector::OnCustomSmileyWrite(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyCustomSmileyWrite(m_pAccount, strContact.c_str(), strSmiley.c_str(), pSmileyData, nSmileyDataSize, strFileExtension.c_str());
	}
}

void CProtocolCallbackSelector::OnCustomSmileyClose(const std::string& strUser, const std::string& strContact, const std::string& strSmiley)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyCustomSmileyClose(m_pAccount, strContact.c_str(), strSmiley.c_str());
	}
}

void CProtocolCallbackSelector::OnIMReceived(const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyIMReceived(m_pAccount, strContact.c_str(), strText.c_str(), strSmileyList.c_str());
	}
}

void CProtocolCallbackSelector::OnChatAddUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyChatAddUser(nChatId, m_pAccount, strChatContactName.c_str(), strChatContactAlias.c_str(), bIsContact);
	}
}

void CProtocolCallbackSelector::OnChatContactRenameUser(const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyChatRenameUser(nChatId, m_pAccount, strChatContactOldName.c_str(), strChatContactNewName.c_str(), strChatContactNewAlias.c_str());
	}
}

void CProtocolCallbackSelector::OnChatRemoveUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyChatRemoveUser(nChatId, m_pAccount, strChatContactName.c_str());
	}
}

void CProtocolCallbackSelector::OnChatContactUpdateUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		pConversationManager->NotifyChatUpdateUser(nChatId, m_pAccount, strChatContactName.c_str(), strChatContactAlias.c_str(), bIsContact);
	}
}

void CProtocolCallbackSelector::OnChatReceived(const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyChatReceived(nChatId, m_pAccount, strFromUser.c_str(), strText.c_str(), strSmileyList.c_str());
	}
}

void CProtocolCallbackSelector::OnChatSent(const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyChatSent(nChatId, m_pAccount, strToUser.c_str(), strText.c_str());
	}
}

void CProtocolCallbackSelector::OnChatSendError(const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());

	if (pConversationManager)
	{
		pConversationManager->NotifyChatSendError(nChatId, m_pAccount, strToUser.c_str(), sendError);
	}
}

RequestResult CProtocolCallbackSelector::OnRequestAcceptFileAsync(const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	
	if (pConversationManager)
	{
		return pConversationManager->NotifyRequestFileTransferAsync(m_pAccount, strContact.c_str(), pFileTransferAsyncInfo);
	}
	
	return REQUEST_RESULT_ERROR;
}

void CProtocolCallbackSelector::OnContactCanRequestAuthorization(const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization)
{
	CRemoteContactListPtr pRemoteContactList = ap_dynamic_cast<CRemoteContactListPtr>(CUIMApplication::GetApplication()->GetRemoteContactList());
	
	if (pRemoteContactList)
	{
		pRemoteContactList->NotifyContactCanRequestAuthorization(m_pAccount, strContact.c_str(), bCanRequestAuthorization);
	}
}

