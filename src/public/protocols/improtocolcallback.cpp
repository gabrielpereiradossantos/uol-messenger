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

#include "IMProtocolCallback.h"
#include "critseclock.h"


CIMProtocolCallback::CIMProtocolCallback() : 
	m_pPrev(NULL) 
{
}

CIMProtocolCallback::~CIMProtocolCallback() 
{
}

unsigned CIMProtocolCallback::GetCallbackPos() 
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{		
		return pPrev->GetCallbackPos();
	}

	return 0;
}


void CIMProtocolCallback::SetPreviousCallback(CIMProtocolCallback* pProtocolCallback)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(m_csCallback);

	m_pPrev = pProtocolCallback;
}


CIMProtocolCallback* CIMProtocolCallback::GetPreviousCallback()
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(m_csCallback);

	return m_pPrev;
}


void CIMProtocolCallback::OnConnected(const std::string& strUser)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnConnected(strUser);
	}
}

void CIMProtocolCallback::OnDisconnected(const std::string& strUser)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnDisconnected(strUser);
	}
}

void CIMProtocolCallback::OnReportDisconnect(const std::string& strUser, ConnectionError errorCode, const std::string& strMessage)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnReportDisconnect(strUser, errorCode, strMessage);
	}
}

void CIMProtocolCallback::OnNotifyAdded(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnNotifyAdded(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactListUpdate(const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactListUpdate(strUser, pContactInfo, strGroup);
	}
}

void CIMProtocolCallback::OnContactRemoved(const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactRemoved(strUser, strContact, strGroup);
	}
}

void CIMProtocolCallback::OnContactMoved(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactMoved(strUser, strContact, strOldGroup, strNewGroup);
	}
}

void CIMProtocolCallback::OnContactGotInfo(const std::string& strUser, const std::string& strContact, const std::string& strInfo)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactGotInfo(strUser, strContact, strInfo);
	}
}

void CIMProtocolCallback::OnContactIconReceived(const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactIconReceived(strUser, strContact, pIconData, nLen);
	}
}

void CIMProtocolCallback::OnContactAliasReceived(const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactAliasReceived(strUser, strContact, strAlias);
	}
}

void CIMProtocolCallback::OnGroupAdded(const std::string& strUser, const std::string& strGroup)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnGroupAdded(strUser, strGroup);
	}
}

void CIMProtocolCallback::OnGroupRemoved(const std::string& strUser, const std::string& strGroup)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnGroupRemoved(strUser, strGroup);
	}
}

void CIMProtocolCallback::OnGroupRenamed(const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnGroupRenamed(strUser, strOldGroup, strNewGroup);
	}
}

void CIMProtocolCallback::OnIMReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnIMReceived(strUser, strFromUser, strText);
	}
}

void CIMProtocolCallback::OnIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnIMSent(strFromUser, strToUser, strText);
	}
}

void CIMProtocolCallback::OnIMSendError(const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnIMSendError(strFromUser, strToUser, sendError);
	}
}

void CIMProtocolCallback::OnContactSignedOn(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactSignedOn(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactSignedOff(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactSignedOff(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactAway(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactAway(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactAwayReturn(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactAwayReturn(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactIdle(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactIdle(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactIdleReturn(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactIdleReturn(strUser, strContact);
	}
}

void CIMProtocolCallback::OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactStatusChanged(strUser, strContact, nStatus);
	}
}

void CIMProtocolCallback::OnAddContactDenied(const std::string& strUser, const std::string& strContact, const std::string& strMessage)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnAddContactDenied(strUser, strContact, strMessage);
	}
}

void CIMProtocolCallback::OnContactTyping(const std::string& strUser, const std::string& strContact, TypingState state)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnContactTyping(strUser, strContact, state);
	}
}

void CIMProtocolCallback::OnConversationWindowClosed(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnConversationWindowClosed(strUser, strContact);
	}
}

void CIMProtocolCallback::OnConversationTimeout(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnConversationTimeout(strUser, strContact);
	}
}

void CIMProtocolCallback::OnPermitAdded(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnPermitAdded(strUser, strContact);
	}
}

void CIMProtocolCallback::OnDenyAdded(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnDenyAdded(strUser, strContact);
	}
}

void CIMProtocolCallback::OnPermitRemoved(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnPermitRemoved(strUser, strContact);
	}
}

void CIMProtocolCallback::OnDenyRemoved(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnDenyRemoved(strUser, strContact);
	}
}

RequestResult CIMProtocolCallback::OnRequestInput(const std::string& strUser, RequestReason reason, char* pszBuffer, int nBufferLength)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnRequestInput(strUser, reason, pszBuffer, nBufferLength);
	}

	return REQUEST_RESULT_ERROR;
}

RequestResult CIMProtocolCallback::OnRequestAddContact(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnRequestAddContact(strUser, strContact);
	}

	return REQUEST_RESULT_ERROR;
}

RequestResult CIMProtocolCallback::OnRequestAddUser(const std::string& strUser, const std::string& strContact, const std::string& strAddReason)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnRequestAddUser(strUser, strContact, strAddReason);
	}

	return REQUEST_RESULT_ERROR;
}

RequestResult CIMProtocolCallback::OnRequestSendAuthorization(const std::string& strUser, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnRequestSendAuthorization(strUser, strContact);
	}

	return REQUEST_RESULT_ERROR;
}

void CIMProtocolCallback::OnNotifyContactAction(const std::string& strUser, NotifyReason reason, const std::string& strContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnNotifyContactAction(strUser, reason, strContact);
	}
}

RequestResult CIMProtocolCallback::OnRequestAcceptFile(const std::string& strUser, const std::string& strContact, const std::string& strFilename)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnRequestAcceptFile(strUser, strContact, strFilename);
	}

	return REQUEST_RESULT_ERROR;
}

RequestResult CIMProtocolCallback::OnRequestFilePath(const std::string& strUser, const std::string& strContact, char* pszFilePath, int nMaxPathLength)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		return pPrev->OnRequestFilePath(strUser, strContact, pszFilePath, nMaxPathLength);
	}

	return REQUEST_RESULT_ERROR;
}

void CIMProtocolCallback::OnFileTransferInit(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnFileTransferInit(strUser, strContact, pFileTransferInfo);
	}
}

void CIMProtocolCallback::OnFileTransferUpdateProgress(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnFileTransferUpdateProgress(strUser, strContact, pFileTransferInfo);
	}
}

void CIMProtocolCallback::OnFileTransferCanceled(const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnFileTransferCanceled(strUser, strContact, pFileTransferInfo);
	}
}

void CIMProtocolCallback::OnFileTransferError(const std::string& strUser, const std::string& strContact, int nError)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		pPrev->OnFileTransferError(strUser, strContact, nError);
	}
}




////////////////////////////////////////////////////
// CIMProtocolCallback2


CIMProtocolCallback2::~CIMProtocolCallback2()
{
}


void CIMProtocolCallback2::OnContactExtendedCapabilitiesChanged(const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback2* pPrev2 = dynamic_cast<CIMProtocolCallback2*>(pPrev);

		if (pPrev2)
		{
			pPrev2->OnContactExtendedCapabilitiesChanged(strUser, strContact, pExtendedCapabilities);
		}
	}
}








////////////////////////////////////////////////////
// CIMProtocolCallback2


CIMProtocolCallback3::~CIMProtocolCallback3()
{
}


RequestResult CIMProtocolCallback3::OnRequestUserAuthorization(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback3* pPrev3 = dynamic_cast<CIMProtocolCallback3*>(pPrev);

		if (pPrev3)
		{
			return pPrev3->OnRequestUserAuthorization(strUser, pRequestUserAuthorizationParam);
		}
	}

	return REQUEST_RESULT_ERROR;
}


void CIMProtocolCallback3::OnNotificationMessageReceived(const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback3* pPrev3 = dynamic_cast<CIMProtocolCallback3*>(pPrev);

		if (pPrev3)
		{
			pPrev3->OnNotificationMessageReceived(strUser, pNotificationMessageParam);
		}
	}
}




////////////////////////////////////////////////////
// CIMProtocolCallback4


CIMProtocolCallback4::~CIMProtocolCallback4()
{
}

RequestResult CIMProtocolCallback4::OnRequestAddContact(const std::string& strUser, CRequestUserAuthorizationParam* pRequestUserAuthorizationParam)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback4* pPrev4 = dynamic_cast<CIMProtocolCallback4*>(pPrev);
		
		if (pPrev4)
		{
			return pPrev4->OnRequestAddContact(strUser, pRequestUserAuthorizationParam);
		}
	}
	
	return REQUEST_RESULT_ERROR;
}

void CIMProtocolCallback4::OnNudgeReceived(const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback4* pPrev4 = dynamic_cast<CIMProtocolCallback4*>(pPrev);
		
		if (pPrev4)
		{
			pPrev4->OnNudgeReceived(strUser, strFromUser, strText);
		}
	}
}




////////////////////////////////////////////////////
// CIMProtocolCallback5


CIMProtocolCallback5::~CIMProtocolCallback5()
{
}

CustomEmoticonAddResult CIMProtocolCallback5::OnCustomSmileyAdd(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const int bRemote)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback5* pPrev5 = dynamic_cast<CIMProtocolCallback5*>(pPrev);
		
		if (pPrev5)
		{
			return pPrev5->OnCustomSmileyAdd(strUser, strContact, strSmiley, bRemote);
		}
	}
	
	return CUSTOM_EMOTICON_ADD_RESULT_ERROR;
}

void CIMProtocolCallback5::OnCustomSmileyWrite(const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback5* pPrev5 = dynamic_cast<CIMProtocolCallback5*>(pPrev);
		
		if (pPrev5)
		{
			pPrev5->OnCustomSmileyWrite(strUser, strContact, strSmiley, pSmileyData, nSmileyDataSize, strFileExtension);
		}
	}
}

void CIMProtocolCallback5::OnCustomSmileyClose(const std::string& strUser, const std::string& strContact, const std::string& strSmiley)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback5* pPrev5 = dynamic_cast<CIMProtocolCallback5*>(pPrev);
		
		if (pPrev5)
		{
			pPrev5->OnCustomSmileyClose(strUser, strContact, strSmiley);
		}
	}
}

void CIMProtocolCallback5::OnIMReceived(const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback5* pPrev5 = dynamic_cast<CIMProtocolCallback5*>(pPrev);
		
		if (pPrev5)
		{
			pPrev5->OnIMReceived(strUser, strContact, strText, strSmileyList);
		}
	}
}




////////////////////////////////////////////////////
// CIMProtocolCallback6


CIMProtocolCallback6::~CIMProtocolCallback6()
{
}

void CIMProtocolCallback6::OnChatAddUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatAddUser(strUser, nChatId, strChatContactName, strChatContactAlias, bIsContact);
		}
	}
}

void CIMProtocolCallback6::OnChatRenameUser(const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatRenameUser(strUser, nChatId, strChatContactOldName, strChatContactNewName, strChatContactNewAlias);
		}
	}
}

void CIMProtocolCallback6::OnChatRemoveUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatRemoveUser(strUser, nChatId, strChatContactName);
		}
	}
}

void CIMProtocolCallback6::OnChatUpdateUser(const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatUpdateUser(strUser, nChatId, strChatContactName, strChatContactAlias, bIsContact);
		}
	}
}

void CIMProtocolCallback6::OnChatReceived(const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatReceived(strUser, nChatId, strFromUser, strText, strSmileyList);
        }
	}
}

void CIMProtocolCallback6::OnChatSent(const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatSent(strFromUser, nChatId, strToUser, strText);
        }
	}
}

void CIMProtocolCallback6::OnChatSendError(const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnChatSendError(strFromUser, nChatId, strToUser, sendError);
        }
	}
}

RequestResult CIMProtocolCallback6::OnRequestAcceptFileAsync(const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();
	
	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			return pPrev6->OnRequestAcceptFileAsync(strUser, strContact, pFileTransferAsyncInfo);
        }
	}
	
	return REQUEST_RESULT_ERROR;
}

void CIMProtocolCallback6::OnContactCanRequestAuthorization(const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback6* pPrev6 = dynamic_cast<CIMProtocolCallback6*>(pPrev);
		
		if (pPrev6)
		{
			pPrev6->OnContactCanRequestAuthorization(strUser, strContact, bCanRequestAuthorization);
        }
	}
}




////////////////////////////////////////////////////
// CIMProtocolCallback7


CIMProtocolCallback7::~CIMProtocolCallback7()
{
}

void CIMProtocolCallback7::OnContactStatusChanged(const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		CIMProtocolCallback7* pPrev7 = dynamic_cast<CIMProtocolCallback7*>(pPrev);
		
		if (pPrev7)
		{
			pPrev7->OnContactStatusChanged(strUser, strContact, nStatus, strStatusMessage);
		}
	}
}
