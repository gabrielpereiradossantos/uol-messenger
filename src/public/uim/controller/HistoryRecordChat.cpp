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
#include "HistoryRecordChat.h"
#include "../HTMLFilter.h"
#include "../controller/UIMApplication.h"

#include <interfaces/IUOLMessengerHistoryRecord.h>
#include <interfaces/IUOLMessengerContact.h>


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordChat
///////////////////////////////////////////////////////////////////////////////
CHistoryRecordChat::CHistoryRecordChat(IUOLMessengerAccountPtr pAccount, 
                                       const CString& strContact,
                                       const CString& strEventFrom,
									   const CString& strEventTo,
                                       IUOLMessengerConversation::MessageEntryPtr pMessageEntry) : 
m_pAccount(pAccount),
m_strContact(strContact),
m_strEventFrom(strEventFrom),
m_strEventTo(strEventTo),
m_pMessageEntry(pMessageEntry)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	//----------------------------------------------------------------------------
	// History
	// Time
	CTime timeHistory = m_pMessageEntry->time;
	SetTime(timeHistory);

	// Service    
	CString strService = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_SERVICE_MESSAGE");
	SetService(strService);
	
	// Account UserId
	SetAccountUserId(m_pAccount->GetUserId());

	// ProtocolId
	CString strProtocolId = m_pAccount->GetProtocolId();
	strProtocolId.Replace("prpl-", "");
	strProtocolId.MakeUpper();
	SetProtocolId(strProtocolId);

	//----------------------------------------------------------------------------
	// Event
	if (CanLog())
	{
		CString strStatus = ""; // Status is not used when log Messages
		CString strType = GetType();
		CString strEventFrom = GetEventFrom();
		CString strEventTo = GetEventTo();
		CString strEventText = CHTMLFilter::HTMLToText(m_pMessageEntry->strText);

		CHistoryRecordEventPtr pHistoryEvent = new CHistoryRecordEvent(GetTime(), 
			strStatus, strType, strEventFrom, strEventTo, strEventText);

		if (pHistoryEvent)
		{
			AddEvent(pHistoryEvent);
		}
	}
}


CHistoryRecordChat::CHistoryRecordChat(IUOLMessengerAccountPtr pAccount, const CString& strContact) :
m_pAccount(pAccount),
m_strContact(strContact)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	//----------------------------------------------------------------------------
	// History
	// Time
	CTime timeHistory = CTime::GetCurrentTime();
	SetTime(timeHistory);

	// Service    
	CString strService = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_SERVICE_MESSAGE");
	SetService(strService);
	
	// Account UserId
	SetAccountUserId(m_pAccount->GetUserId());

	// ProtocolId
	CString strProtocolId = m_pAccount->GetProtocolId();
	strProtocolId.Replace("prpl-", "");
	strProtocolId.MakeUpper();
	SetProtocolId(strProtocolId);
}


CHistoryRecordChat::~CHistoryRecordChat()
{
}


BOOL CHistoryRecordChat::CanLog() const
{
	ATLASSERT(m_pMessageEntry);
	
	BOOL bCanLog = FALSE;

	switch (m_pMessageEntry->nMessageSender)
	{
	case IUOLMessengerConversation::MT_ACCOUNT_USER:
	case IUOLMessengerConversation::MT_CONTACT:
		bCanLog = TRUE;
		break;
	case IUOLMessengerConversation::MT_SYSTEM:
	case IUOLMessengerConversation::MT_FILE_TRANSFER:
		bCanLog = FALSE;
		break;
	}

	return bCanLog;
}


BOOL CHistoryRecordChat::ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	//----------------------------------------------------------------------------
	// History
	// Time
	SetTime(pHistoryRecord->GetTime());

	// Service    
	CString strService = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_SERVICE_MESSAGE");
	
	ATLASSERT(0 == strService.CompareNoCase(pHistoryRecord->GetService()));
	if (0 == strService.CompareNoCase(pHistoryRecord->GetService()))
	{
		SetService(pHistoryRecord->GetService());

		// Account UserId
		CString strAccountUserId = pHistoryRecord->GetAccountUserId();
		if (!strAccountUserId.IsEmpty())
		{
			SetAccountUserId(strAccountUserId);
		}

		// ProtocolId
		CString strProtocolId = pHistoryRecord->GetProtocolId();
		if (!strProtocolId.IsEmpty())
		{
			SetProtocolId(strProtocolId);
		}

		//----------------------------------------------------------------------------
		// Events
		ClearEvents();
		CAtlList<IUOLMessengerHistoryEventPtr> listEvents;
		pHistoryRecord->GetEvents(listEvents);
		for (POSITION pos = listEvents.GetHeadPosition(); pos;)
		{
			IUOLMessengerHistoryEventPtr pHistoryEventItem = listEvents.GetNext(pos);

			CHistoryRecordEventPtr pHistoryEvent = new CHistoryRecordEvent(pHistoryEventItem->GetFullTime(),
				pHistoryEventItem->GetStatus(), pHistoryEventItem->GetType(), pHistoryEventItem->GetEventFrom(), 
				pHistoryEventItem->GetEventTo(), pHistoryEventItem->GetEventText());

			if (pHistoryEvent)
			{
				AddEvent(pHistoryEvent);
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


CString CHistoryRecordChat::GetHistoryText()
{
	CString strText;

	CAtlList<IUOLMessengerHistoryEventPtr> listEvents;
	GetEvents(listEvents);
	for (POSITION pos = listEvents.GetHeadPosition(); pos;)
	{
		IUOLMessengerHistoryEventPtr pHistoryEvent = listEvents.GetNext(pos);

		// build text block
		strText.Append("(");
		strText.Append(pHistoryEvent->GetTime());
		strText.Append(") ");
		strText.Append(FindAlias(pHistoryEvent->GetEventFrom()));
		strText.Append(": ");
		strText.Append(pHistoryEvent->GetEventText());
		strText.Append("\n\n");
	}

	return strText;
}


CString CHistoryRecordChat::GetRecentHistoryText()
{
	CString strText;
	int nEventsCount = 0;

	CAtlList<IUOLMessengerHistoryEventPtr> listEvents;
	GetEvents(listEvents);
	for (POSITION pos = listEvents.GetTailPosition(); pos;)
	{
		IUOLMessengerHistoryEventPtr pHistoryEvent = listEvents.GetPrev(pos);

		CString strLine;
		strLine.Append("(");
		strLine.Append(pHistoryEvent->GetTime());
		strLine.Append(") ");
		strLine.Append(FindAlias(pHistoryEvent->GetEventFrom()));
		strLine.Append(": ");
		strLine.Append(pHistoryEvent->GetEventText());
		strLine.Append("\n\n");

		strText.Insert(0, strLine);
		nEventsCount++;

		if (nEventsCount >= HR_RECENT_HISTORY_MAX_EVENTS)
		{
			break;
		}
	}

	return strText;
}


void CHistoryRecordChat::GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
{
	GetEvents(listEvents);
}


CString CHistoryRecordChat::GetType() const
{
	ATLASSERT(m_pMessageEntry);
	
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(
		CUIMApplication::GetApplication())->GetStringLoader();
    
	CString strType;

	switch (m_pMessageEntry->nMessageSender)
	{
	case IUOLMessengerConversation::MT_ACCOUNT_USER:
		strType = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_ACCOUNT");
		break;
	case IUOLMessengerConversation::MT_CONTACT:
		strType = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_CONTACT");
		break;
	case IUOLMessengerConversation::MT_SYSTEM:
		strType = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_SYSTEM");
		break;
	case IUOLMessengerConversation::MT_FILE_TRANSFER:
		strType = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_FILE_TRANSFER");
		break;
	}

	return strType;
}


CString CHistoryRecordChat::GetEventFrom() const
{
	return m_strEventFrom;
}


CString CHistoryRecordChat::GetEventTo() const
{
	return m_strEventTo;
}

CString CHistoryRecordChat::GetContactName() const
{
	return m_strContact;
}

CString CHistoryRecordChat::FindAlias(const CString& strName)
{
	// Find User/Contact alias
	CString strAcountUserId = GetAccountUserId();
	CString strAlias = strName;

	// is sender (EventFrom) account user?
	if (0 == strAcountUserId.CompareNoCase(strName))
	{
		IUOLMessengerAccountPtr pAccount;
		pAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
			GetAccountUserId(), GetPrplProtocolId());
		if (pAccount)
		{
			strAlias = pAccount->GetNickName();
		}
	}
	else
	{
		IUOLMessengerContactPtr pContact;
		pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(
			GetAccountUserId(), GetPrplProtocolId(), strName);
		if (pContact)
		{
			strAlias = pContact->GetAlias();
		}
	}

	return strAlias;
}


CString CHistoryRecordChat::GetPrplProtocolId() const
{
	CString strProtocolId;
	strProtocolId.Format(_T("prpl-%s"), GetProtocolId());
	strProtocolId.MakeLower();	

	return strProtocolId;
}