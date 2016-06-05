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
 

#include "WMCallbackSerializer.h"
#include "WMCallbackSerializerParams.h"

#include "PurpleRequestFileTransferAsyncInfo.h"
#include "PurpleService.h"


// Public methods...
//

CWMCallbackSerializer::CWMCallbackSerializer() : 
		m_bInitialized(FALSE), 
		m_bIsTerminating(FALSE)
{
}


CWMCallbackSerializer::~CWMCallbackSerializer()
{
}


BOOL CWMCallbackSerializer::Init()
{
	HWND hWnd = Create(NULL, NULL, "DummyCallbackSerializer", WS_POPUP);
	
	if (hWnd != NULL)
	{
		m_bInitialized = TRUE;
	}
	
	return m_bInitialized;
}


BOOL CWMCallbackSerializer::Finalize()
{
	m_bIsTerminating = TRUE;
	
	return DestroyWindow();
}


// Interface ILibPurpleToUIMThreadSerializer methods...
//

void CWMCallbackSerializer::HandleOnConnected(CIMProtocolCallback* pCallback, const std::string& strUser)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_PARAM* pWMSParam = new WMS_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnConnected;
			
			PostMessage(WM_WMS_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnDisconnected(CIMProtocolCallback* pCallback, const std::string& strUser)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_PARAM* pWMSParam = new WMS_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnDisconnected;
			
			PostMessage(WM_WMS_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnReportDisconnect(CIMProtocolCallback* pCallback, const std::string& strUser, ConnectionError errorCode, const std::string& strMessage)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_CONNECTIONERROR_STR_PARAM* pWMSParam = new WMS_STR_CONNECTIONERROR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strMessage;
			pWMSParam->connectionError = errorCode;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnReportDisconnect;
			
			PostMessage(WM_WMS_STR_CONNECTIONERROR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnNotifyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnNotifyAdded;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactListUpdate(CIMProtocolCallback* pCallback, const std::string& strUser, const CContactInfo* pContactInfo, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_CONTACTINFO_STR_PARAM* pWMSParam = new WMS_STR_CONTACTINFO_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strGroup;
			pWMSParam->pContactInfo = CloneContactInfo(pContactInfo);
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactListUpdate;
			
			PostMessage(WM_WMS_STR_CONTACTINFO_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->strParam3 = strGroup;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactRemoved;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactMoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->strParam3 = strOldGroup;
			pWMSParam->strParam4 = strNewGroup;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactMoved;
			
			PostMessage(WM_WMS_STR_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactGotInfo(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->strParam3 = strInfo;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactGotInfo;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactIconReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const char* pIconData, int nLen)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PCHAR_INT_PARAM* pWMSParam = new WMS_STR_STR_PCHAR_INT_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			//pWMSParam->pData = DuplicateData(pIconData, nLen, FALSE);
			pWMSParam->pData = (char *)DuplicateData((void *)pIconData, nLen);
			pWMSParam->intParam1 = nLen;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactIconReceived;
			
			PostMessage(WM_WMS_STR_STR_PCHAR_INT, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactAliasReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->strParam3 = strAlias;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactAliasReceived;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnGroupAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strGroup;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnGroupAdded;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnGroupRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strGroup;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnGroupRemoved;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnGroupRenamed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strOldGroup, const std::string& strNewGroup)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strOldGroup;
			pWMSParam->strParam3 = strNewGroup;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnGroupRenamed;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strFromUser;
			pWMSParam->strParam3 = strText;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnIMReceived;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnIMSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strFromUser;
			pWMSParam->strParam2 = strToUser;
			pWMSParam->strParam3 = strText;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnIMSent;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnIMSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const std::string& strToUser, IMProtocolError sendError)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_IMPROTOCOLERROR_PARAM* pWMSParam = new WMS_STR_STR_IMPROTOCOLERROR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strFromUser;
			pWMSParam->strParam2 = strToUser;
			pWMSParam->imProtocolError = sendError;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnIMSendError;
			
			PostMessage(WM_WMS_STR_STR_IMPROTOCOLERROR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactSignedOn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactSignedOn;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactSignedOff(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactSignedOff;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactAway(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactAway;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactAwayReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactAwayReturn;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactIdle(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactIdle;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactIdleReturn(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactIdleReturn;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactStatusChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nStatus)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_INT_PARAM* pWMSParam = new WMS_STR_STR_INT_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->intParam1 = nStatus;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactStatusChanged;
			
			PostMessage(WM_WMS_STR_STR_INT, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactStatusChanged(CIMProtocolCallback7* pCallback, const std::string& strUser, const std::string& strContact, int nStatus, const std::string& strStatusMessage)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_CBK7_STR_STR_INT_STR_PARAM* pWMSParam = new WMS_CBK7_STR_STR_INT_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback7 = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->strParam3 = strStatusMessage;
			pWMSParam->intParam1 = nStatus;
			pWMSParam->pfnHandler = &CIMProtocolCallback7::OnContactStatusChanged;
			
			PostMessage(WM_WMS_CBK7_STR_STR_INT_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnAddContactDenied(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strMessage)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->strParam3 = strMessage;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnAddContactDenied;
			
			PostMessage(WM_WMS_STR_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactTyping(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, TypingState state)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_TYPINGSTATE_PARAM* pWMSParam = new WMS_STR_STR_TYPINGSTATE_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->typingState = state;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnContactTyping;
			
			PostMessage(WM_WMS_STR_STR_TYPINGSTATE, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnConversationWindowClosed(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnConversationWindowClosed;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnConversationTimeout(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnConversationTimeout;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnPermitAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnPermitAdded;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnDenyAdded(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnDenyAdded;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnPermitRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnPermitRemoved;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnDenyRemoved(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PARAM* pWMSParam = new WMS_STR_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnDenyRemoved;
			
			PostMessage(WM_WMS_STR_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnNotifyContactAction(CIMProtocolCallback* pCallback, const std::string& strUser, NotifyReason reason, const std::string& strContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_NOTIFYREASON_STR_PARAM* pWMSParam = new WMS_STR_NOTIFYREASON_STR_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->notifyReason = reason;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnNotifyContactAction;
			
			PostMessage(WM_WMS_STR_NOTIFYREASON_STR, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnFileTransferInit(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PFILETRANSFERINFO_PARAM* pWMSParam = new WMS_STR_STR_PFILETRANSFERINFO_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pFileTransferInfo = pFileTransferInfo->Clone();
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnFileTransferInit;
			
			PostMessage(WM_WMS_STR_STR_PFILETRANSFERINFO, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnFileTransferUpdateProgress(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PFILETRANSFERINFO_PARAM* pWMSParam = new WMS_STR_STR_PFILETRANSFERINFO_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pFileTransferInfo = pFileTransferInfo->Clone();
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnFileTransferUpdateProgress;
			
			PostMessage(WM_WMS_STR_STR_PFILETRANSFERINFO, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnFileTransferCanceled(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_PFILETRANSFERINFO_PARAM* pWMSParam = new WMS_STR_STR_PFILETRANSFERINFO_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->pFileTransferInfo = pFileTransferInfo->Clone();
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnFileTransferCanceled;
			
			PostMessage(WM_WMS_STR_STR_PFILETRANSFERINFO, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnFileTransferError(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, int nError)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		WMS_STR_STR_INT_PARAM* pWMSParam = new WMS_STR_STR_INT_PARAM;
		
		if (pWMSParam)
		{
			pWMSParam->pCallback = pCallback;
			pWMSParam->strParam1 = strUser;
			pWMSParam->strParam2 = strContact;
			pWMSParam->intParam1 = nError;
			pWMSParam->pfnHandler = &CIMProtocolCallback::OnFileTransferError;
			
			PostMessage(WM_WMS_STR_STR_INT, 0, (LPARAM) pWMSParam);
		}
	}
}


void CWMCallbackSerializer::HandleOnContactExtendedCapabilitiesChanged(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const CProtocolSettings2* pExtendedCapabilities)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback2* pCallback2 = dynamic_cast<CIMProtocolCallback2*>(pCallback);
		
		if (pCallback2)
		{
			WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2_PARAM* pWMSParam = new WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback2 = pCallback2;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strContact;
				
				if (pExtendedCapabilities)
				{
					pWMSParam->pProtocolSettings2 = pExtendedCapabilities->Clone();
				}
				else
				{
					pWMSParam->pProtocolSettings2 = NULL;
				}
				
				pWMSParam->pfnHandler = &CIMProtocolCallback2::OnContactExtendedCapabilitiesChanged;
				
				PostMessage(WM_WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnNotificationMessageReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const CNotificationMessageParam* pNotificationMessageParam)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback3* pCallback3 = dynamic_cast<CIMProtocolCallback3*>(pCallback);
		
		if (pCallback3)
		{
			WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM_PARAM* pWMSParam = new WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback3 = pCallback3;
				pWMSParam->strParam1 = strUser;
				pWMSParam->pNotificationMessageParam = pNotificationMessageParam->Clone();
				pWMSParam->pfnHandler = &CIMProtocolCallback3::OnNotificationMessageReceived;
				
				PostMessage(WM_WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnNudgeReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strFromUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback4* pCallback4 = dynamic_cast<CIMProtocolCallback4*>(pCallback);
		
		if (pCallback4)
		{
			WMS_CBK4_STR_STR_STR_PARAM* pWMSParam = new WMS_CBK4_STR_STR_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback4 = pCallback4;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strFromUser;
				pWMSParam->strParam3 = strText;
				pWMSParam->pfnHandler = &CIMProtocolCallback4::OnNudgeReceived;
				
				PostMessage(WM_WMS_CBK4_STR_STR_STR, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnCustomSmileyWrite(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const std::string& strFileExtension)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR_PARAM* pWMSParam = new WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback5 = pCallback5;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strContact;
				pWMSParam->strParam3 = strSmiley;
				pWMSParam->strParam4 = strFileExtension;
				//pWMSParam->pData = DuplicateData(pSmileyData, nSmileyDataSize, TRUE);
				pWMSParam->pData = (unsigned char*)DuplicateData((void*)pSmileyData, nSmileyDataSize);
				pWMSParam->intParam1 = nSmileyDataSize;
				pWMSParam->pfnHandler = &CIMProtocolCallback5::OnCustomSmileyWrite;
				
				PostMessage(WM_WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnCustomSmileyClose(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strSmiley)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			WMS_CBK5_STR_STR_STR_PARAM* pWMSParam = new WMS_CBK5_STR_STR_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback5 = pCallback5;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strContact;
				pWMSParam->strParam3 = strSmiley;
				pWMSParam->pfnHandler = &CIMProtocolCallback5::OnCustomSmileyClose;
				
				PostMessage(WM_WMS_CBK5_STR_STR_STR, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const std::string& strText, const std::string& strSmileyList)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
		
		if (pCallback5)
		{
			WMS_CBK5_STR_STR_STR_STR_PARAM* pWMSParam = new WMS_CBK5_STR_STR_STR_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback5 = pCallback5;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strContact;
				pWMSParam->strParam3 = strText;
				pWMSParam->strParam4 = strSmileyList;
				pWMSParam->pfnHandler = &CIMProtocolCallback5::OnIMReceived;
				
				PostMessage(WM_WMS_CBK5_STR_STR_STR_STR, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnChatAddUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_STR_INT_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_STR_INT_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strChatContactName;
				pWMSParam->strParam3 = strChatContactAlias;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->intParam2 = bIsContact;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatAddUser;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR_STR_INT, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnChatRenameUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactOldName, const std::string& strChatContactNewName, const std::string& strChatContactNewAlias)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_STR_STR_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_STR_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strChatContactOldName;
				pWMSParam->strParam3 = strChatContactNewName;
				pWMSParam->strParam4 = strChatContactNewAlias;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatRenameUser;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR_STR_STR, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnChatRemoveUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strChatContactName;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatRemoveUser;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnChatUpdateUser(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strChatContactName, const std::string& strChatContactAlias, const int bIsContact)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_STR_INT_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_STR_INT_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strChatContactName;
				pWMSParam->strParam3 = strChatContactAlias;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->intParam2 = bIsContact;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatUpdateUser;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR_STR_INT, 0, (LPARAM) pWMSParam);
			}
		}
	}
}


void CWMCallbackSerializer::HandleOnChatReceived(CIMProtocolCallback* pCallback, const std::string& strUser, const int nChatId, const std::string& strFromUser, const std::string& strText, const std::string& strSmileyList)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_STR_STR_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_STR_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strFromUser;
				pWMSParam->strParam3 = strText;
				pWMSParam->strParam4 = strSmileyList;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatReceived;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR_STR_STR, 0, (LPARAM) pWMSParam);
			}
        }
	}
}


void CWMCallbackSerializer::HandleOnChatSent(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, const std::string& strText)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_STR_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_STR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strFromUser;
				pWMSParam->strParam2 = strToUser;
				pWMSParam->strParam3 = strText;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatSent;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR_STR, 0, (LPARAM) pWMSParam);
			}
        }
	}
}


void CWMCallbackSerializer::HandleOnChatSendError(CIMProtocolCallback* pCallback, const std::string& strFromUser, const int nChatId, const std::string& strToUser, IMProtocolError sendError)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR_PARAM* pWMSParam = new WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strFromUser;
				pWMSParam->strParam2 = strToUser;
				pWMSParam->imProtocolError = sendError;
				pWMSParam->intParam1 = nChatId;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnChatSendError;
				
				PostMessage(WM_WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR, 0, (LPARAM) pWMSParam);
			}
        }
	}
}


RequestResult CWMCallbackSerializer::HandleOnRequestAcceptFileAsync(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC_PARAM* pWMSParam = 
					new WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strContact;
				pWMSParam->pFileTransferAsyncInfo = pFileTransferAsyncInfo;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnRequestAcceptFileAsync;
				
				PostMessage(WM_WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC, 0, (LPARAM) pWMSParam);
				
				return REQUEST_RESULT_OK;
			}
        }
	}
	
	return REQUEST_RESULT_ERROR;
}


void CWMCallbackSerializer::HandleOnContactCanRequestAuthorization(CIMProtocolCallback* pCallback, const std::string& strUser, const std::string& strContact, const int bCanRequestAuthorization)
{
	if ( (m_bInitialized) && (pCallback) && 
			(!CPurpleService::GetInstance()->PreFinalizeDone()) )
	{
		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		if (pCallback6)
		{
			WMS_CBK6_STR_STR_INT_PARAM* pWMSParam = new WMS_CBK6_STR_STR_INT_PARAM;
			
			if (pWMSParam)
			{
				pWMSParam->pCallback6 = pCallback6;
				pWMSParam->strParam1 = strUser;
				pWMSParam->strParam2 = strContact;
				pWMSParam->intParam1 = bCanRequestAuthorization;
				pWMSParam->pfnHandler = &CIMProtocolCallback6::OnContactCanRequestAuthorization;
				
				PostMessage(WM_WMS_CBK6_STR_STR_INT, 0, (LPARAM) pWMSParam);
			}
        }
	}
}



// CWindowImpl message handling methods...
//

LRESULT CWMCallbackSerializer::OnWmsStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_PARAM* pWMSParam = (WMS_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrConnectionErrorStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_CONNECTIONERROR_STR_PARAM* pWMSParam = (WMS_STR_CONNECTIONERROR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->connectionError, pWMSParam->strParam2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_PARAM* pWMSParam = (WMS_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrContactInfoStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_CONTACTINFO_STR_PARAM* pWMSParam = (WMS_STR_CONTACTINFO_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->pContactInfo, pWMSParam->strParam2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam->pContactInfo;
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_STR_PARAM* pWMSParam = (WMS_STR_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->strParam3);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_STR_STR_PARAM* pWMSParam = (WMS_STR_STR_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->strParam3, pWMSParam->strParam4);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrPcharInt(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_PCHAR_INT_PARAM* pWMSParam = (WMS_STR_STR_PCHAR_INT_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->pData, pWMSParam->intParam1);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		free(pWMSParam->pData);
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrIMProtocolError(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_IMPROTOCOLERROR_PARAM* pWMSParam = (WMS_STR_STR_IMPROTOCOLERROR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->imProtocolError);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrInt(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_INT_PARAM* pWMSParam = (WMS_STR_STR_INT_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->intParam1);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrTypingState(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_TYPINGSTATE_PARAM* pWMSParam = (WMS_STR_STR_TYPINGSTATE_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->typingState);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrNotifyReasonStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_NOTIFYREASON_STR_PARAM* pWMSParam = (WMS_STR_NOTIFYREASON_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->notifyReason, pWMSParam->strParam2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsStrStrPFileTransferInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_STR_STR_PFILETRANSFERINFO_PARAM* pWMSParam = (WMS_STR_STR_PFILETRANSFERINFO_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->pFileTransferInfo);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam->pFileTransferInfo;
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk2StrStrPProtocolSettings2(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2_PARAM* pWMSParam = (WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback2 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback2)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->pProtocolSettings2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam->pProtocolSettings2;
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk3StrNotificationMessageParam(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM_PARAM* pWMSParam = (WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback3 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback3)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->pNotificationMessageParam);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam->pNotificationMessageParam;
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk4StrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK4_STR_STR_STR_PARAM* pWMSParam = (WMS_CBK4_STR_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback4 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback4)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->strParam3);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk5StrStrStrPucharIntStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR_PARAM* pWMSParam = (WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback5 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback5)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->strParam3, pWMSParam->pData, 
							pWMSParam->intParam1, pWMSParam->strParam4);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		free(pWMSParam->pData);
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk5StrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK5_STR_STR_STR_PARAM* pWMSParam = (WMS_CBK5_STR_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback5 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback5)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->strParam3);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk5StrStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK5_STR_STR_STR_STR_PARAM* pWMSParam = (WMS_CBK5_STR_STR_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback5 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback5)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->strParam3, pWMSParam->strParam4);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrIntStrStrInt(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_INT_STR_STR_INT_PARAM* pWMSParam = (WMS_CBK6_STR_INT_STR_STR_INT_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->intParam1, pWMSParam->strParam2, pWMSParam->strParam3, 
							pWMSParam->intParam2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrIntStrStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_INT_STR_STR_STR_PARAM* pWMSParam = (WMS_CBK6_STR_INT_STR_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->intParam1, pWMSParam->strParam2, pWMSParam->strParam3, 
							pWMSParam->strParam4);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrIntStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_INT_STR_PARAM* pWMSParam = (WMS_CBK6_STR_INT_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->intParam1, pWMSParam->strParam2);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrIntStrStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_INT_STR_STR_PARAM* pWMSParam = (WMS_CBK6_STR_INT_STR_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->intParam1, pWMSParam->strParam2, pWMSParam->strParam3);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrIntStrIMProtocolError(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR_PARAM* pWMSParam = (WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->intParam1, pWMSParam->strParam2, 
							pWMSParam->imProtocolError);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrStrRequestFileTransferAsync(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC_PARAM* pWMSParam = 
			(WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->pFileTransferAsyncInfo);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		// Asynchronous operation... this must be released on the response...
		//delete pWMSParam->pFileTransferAsyncInfo;
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk6StrStrInt(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK6_STR_STR_INT_PARAM* pWMSParam = (WMS_CBK6_STR_STR_INT_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback6 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback6)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->intParam1);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


LRESULT CWMCallbackSerializer::OnWmsCbk7StrStrIntStr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WMS_CBK7_STR_STR_INT_STR_PARAM* pWMSParam = (WMS_CBK7_STR_STR_INT_STR_PARAM*)lParam;
	
	LogMethodInfo(pWMSParam, __FUNCTION__);
	
	if (pWMSParam)
	{
		if (!CPurpleService::GetInstance()->PreFinalizeDone())
		{
			if (!m_bIsTerminating)
			{
				if ( (pWMSParam->pCallback7 != NULL) && (pWMSParam->pfnHandler != NULL) )
				{
					((pWMSParam->pCallback7)->*(pWMSParam->pfnHandler))(pWMSParam->strParam1, 
							pWMSParam->strParam2, pWMSParam->intParam1, pWMSParam->strParam3);
				}
			}
		}
		else
		{
			ATLTRACE(_T("%s -> PreFinalizeDone == TRUE.\n"), __FUNCTION__);
		}
		
		delete pWMSParam;
	}
	
	return 0L;
}


// Utilitary methods...
//

CContactInfo* CWMCallbackSerializer::CloneContactInfo(const CContactInfo* pOriginalContactInfo)
{
	CContactInfo* pNewContactInfo = new CContactInfo();
	
	if (pNewContactInfo)
	{
		pNewContactInfo->SetUserName(pOriginalContactInfo->GetUser());
		pNewContactInfo->SetAlias(pOriginalContactInfo->GetAlias());
	}
	
	return pNewContactInfo;
}


void* CWMCallbackSerializer::DuplicateData(void* pOriginalData, int nOriginalDataLength)
{
	if ( (!pOriginalData) || (!nOriginalDataLength) )
	{
		return NULL;
	}
	
	int nAllocLength = nOriginalDataLength + 1;
	
	void *pNewData = malloc(nAllocLength);
	
	if (pNewData)
	{
		memset(pNewData, 0, nAllocLength);
		memcpy(pNewData, pOriginalData, nOriginalDataLength);
	}
	
	return pNewData;
}


void CWMCallbackSerializer::LogMethodInfo(void* pWMSParam, string methodName)
{
	if (pWMSParam)
	{
		ATLTRACE(_T("%s -> LPARAM casting successfull. = %x.\n"), 
				methodName.c_str(), pWMSParam);
		
		if (m_bIsTerminating)
		{
			ATLTRACE(_T("%s -> Terminating on the way... don't process.\n"), 
					methodName.c_str());
		}
	}
	else
	{
		ATLTRACE(_T("%s -> Failed to cast LPARAM.\n"), methodName.c_str());
	}
}

