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
 

#include "DummyCallbackSerializer.h"
#include "PurpleService.h"

using namespace std;


CDummyCallbackSerializer::CDummyCallbackSerializer() : 
		m_bInitialized(TRUE)
{
}


CDummyCallbackSerializer::~CDummyCallbackSerializer()
{
}


BOOL CDummyCallbackSerializer::Init()
{
	return TRUE;
}


BOOL CDummyCallbackSerializer::Finalize()
{
	return TRUE;
}


void CDummyCallbackSerializer::HandleOnConnected(CIMProtocolCallback* pCallback, const std::string& strUser)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnConnected(strUser);
	}
}


void CDummyCallbackSerializer::HandleOnDisconnected(CIMProtocolCallback* pCallback, const std::string& strUser)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnDisconnected(strUser);
	}
}


void CDummyCallbackSerializer::HandleOnReportDisconnect(CIMProtocolCallback* pCallback, const std::string& strUser, ConnectionError errorCode, const std::string& strMessage)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnReportDisconnect(strUser, errorCode, strMessage);
	}
}


void CDummyCallbackSerializer::HandleOnNotifyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnNotifyAdded(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactListUpdate(CIMProtocolCallback* pCallback, const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactListUpdate(strUser, pContactInfo, strGroup);
	}
}


void CDummyCallbackSerializer::HandleOnContactRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactRemoved(strUser, strContact, strGroup);
	}
}


void CDummyCallbackSerializer::HandleOnContactMoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactMoved(strUser, strContact, strOldGroup, strNewGroup);
	}
}


void CDummyCallbackSerializer::HandleOnContactGotInfo(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactGotInfo(strUser, strContact, strInfo);
	}
}


void CDummyCallbackSerializer::HandleOnContactIconReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactIconReceived(strUser, strContact, pIconData, nLen);
	}
}


void CDummyCallbackSerializer::HandleOnContactAliasReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactAliasReceived(strUser, strContact, strAlias);
	}
}


void CDummyCallbackSerializer::HandleOnGroupAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnGroupAdded(strUser, strGroup);
	}
}


void CDummyCallbackSerializer::HandleOnGroupRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnGroupRemoved(strUser, strGroup);
	}
}


void CDummyCallbackSerializer::HandleOnGroupRenamed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnGroupRenamed(strUser, strOldGroup, strNewGroup);
	}
}


void CDummyCallbackSerializer::HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnIMReceived(strUser, strFromUser, strText);
	}
}


void CDummyCallbackSerializer::HandleOnIMSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnIMSent(strFromUser, strToUser, strText);
	}
}


void CDummyCallbackSerializer::HandleOnIMSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnIMSendError(strFromUser, strToUser, sendError);
	}
}


void CDummyCallbackSerializer::HandleOnContactSignedOn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactSignedOn(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactSignedOff(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactSignedOff(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactAway(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactAway(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactAwayReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactAwayReturn(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactIdle(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactIdle(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactIdleReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactIdleReturn(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnContactStatusChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nStatus)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactStatusChanged(strUser, strContact, nStatus);
	}
}


void CDummyCallbackSerializer::HandleOnContactStatusChanged(CIMProtocolCallback7* pCallback, const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactStatusChanged(strUser, strContact, nStatus, strStatusMessage);
	}
}


void CDummyCallbackSerializer::HandleOnAddContactDenied(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strMessage)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnAddContactDenied(strUser, strContact, strMessage);
	}
}


void CDummyCallbackSerializer::HandleOnContactTyping(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, TypingState state)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnContactTyping(strUser, strContact, state);
	}
}


void CDummyCallbackSerializer::HandleOnConversationWindowClosed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnConversationWindowClosed(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnConversationTimeout(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnConversationTimeout(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnPermitAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnPermitAdded(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnDenyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnDenyAdded(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnPermitRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnPermitRemoved(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnDenyRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnDenyRemoved(strUser, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnNotifyContactAction(CIMProtocolCallback* pCallback, const std::string& strUser, NotifyReason reason, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnNotifyContactAction(strUser, reason, strContact);
	}
}


void CDummyCallbackSerializer::HandleOnFileTransferInit(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnFileTransferInit(strUser, strContact, pFileTransferInfo);
	}
}


void CDummyCallbackSerializer::HandleOnFileTransferUpdateProgress(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnFileTransferUpdateProgress(strUser, strContact, pFileTransferInfo);
	}
}


void CDummyCallbackSerializer::HandleOnFileTransferCanceled(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnFileTransferCanceled(strUser, strContact, pFileTransferInfo);
	}
}


void CDummyCallbackSerializer::HandleOnFileTransferError(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nError)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		pCallback->OnFileTransferError(strUser, strContact, nError);
	}
}


void CDummyCallbackSerializer::HandleOnContactExtendedCapabilitiesChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback2* pCallback2 = dynamic_cast<CIMProtocolCallback2*>(pCallback);
		
		if (pCallback2)
		{
			pCallback2->OnContactExtendedCapabilitiesChanged(strUser, strContact, pExtendedCapabilities);
		}
	}
}


void CDummyCallbackSerializer::HandleOnNotificationMessageReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback3* pCallback3 = dynamic_cast<CIMProtocolCallback3*>(pCallback);
		
		if (pCallback3)
		{
			pCallback3->OnNotificationMessageReceived(strUser, pNotificationMessageParam);
		}
	}
}


void CDummyCallbackSerializer::HandleOnNudgeReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback4* pCallback4 = dynamic_cast<CIMProtocolCallback4*>(pCallback);
		
		if (pCallback4)
		{
			pCallback4->OnNudgeReceived(strUser, strFromUser, strText);
		}
	}
}


void CDummyCallbackSerializer::HandleOnCustomSmileyWrite(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			pCallback5->OnCustomSmileyWrite(strUser, strContact, strSmiley, pSmileyData, nSmileyDataSize, strFileExtension);
		}
	}
}


void CDummyCallbackSerializer::HandleOnCustomSmileyClose(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			pCallback5->OnCustomSmileyClose(strUser, strContact, strSmiley);
		}
	}
}


void CDummyCallbackSerializer::HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			pCallback5->OnIMReceived(strUser, strContact, strText, strSmileyList);
		}
	}
}


void CDummyCallbackSerializer::HandleOnChatAddUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatAddUser(strUser, nChatId, strChatContactName, strChatContactAlias, bIsContact);
		}
	}
}


void CDummyCallbackSerializer::HandleOnChatRenameUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatRenameUser(strUser, nChatId, strChatContactOldName, strChatContactNewName, strChatContactNewAlias);
		}
	}
}


void CDummyCallbackSerializer::HandleOnChatRemoveUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatRemoveUser(strUser, nChatId, strChatContactName);
		}
	}
}


void CDummyCallbackSerializer::HandleOnChatUpdateUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatUpdateUser(strUser, nChatId, strChatContactName, strChatContactAlias, bIsContact);
		}
	}
}


void CDummyCallbackSerializer::HandleOnChatReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatReceived(strUser, nChatId, strFromUser, strText, strSmileyList);
        }
	}
}


void CDummyCallbackSerializer::HandleOnChatSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatSent(strFromUser, nChatId, strToUser, strText);
        }
	}
}


void CDummyCallbackSerializer::HandleOnChatSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnChatSendError(strFromUser, nChatId, strToUser, sendError);
        }
	}
}


RequestResult CDummyCallbackSerializer::HandleOnRequestAcceptFileAsync(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			return pCallback6->OnRequestAcceptFileAsync(strUser, strContact, pFileTransferAsyncInfo);
        }
	}
	
	return REQUEST_RESULT_ERROR;
}


void CDummyCallbackSerializer::HandleOnContactCanRequestAuthorization(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			pCallback6->OnContactCanRequestAuthorization(strUser, strContact, bCanRequestAuthorization);
        }
	}
}

