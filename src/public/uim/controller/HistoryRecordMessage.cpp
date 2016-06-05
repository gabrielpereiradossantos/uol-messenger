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
#include "HistoryRecordMessage.h"
#include "HistoryGrammar.h"
#include "../HTMLFilter.h"
#include "../controller/UIMApplication.h"

#include <interfaces/IUOLMessengerHistoryRecord.h>
#include <interfaces/IUOLMessengerContact.h>


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordMessage
///////////////////////////////////////////////////////////////////////////////
CHistoryRecordMessage::CHistoryRecordMessage(IUOLMessengerAccountPtr pAccount, const CString& strContact) :
m_pAccount(pAccount),
m_strContact(strContact)
{
	m_strHistoryText = "";
	m_strRecentHistoryText = "";

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


CHistoryRecordMessage::CHistoryRecordMessage(IUOLMessengerAccountPtr pAccount, 
											 const CString& strContact,
                                             IUOLMessengerConversation::MessageEntryPtr pMessageEntry) : 
m_pAccount(pAccount),
m_strContact(strContact),
m_pMessageEntry(pMessageEntry)
{
	m_strHistoryText = "";
	m_strRecentHistoryText = "";

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


CHistoryRecordMessage::~CHistoryRecordMessage()
{
}


BOOL CHistoryRecordMessage::CanLog() const
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


BOOL CHistoryRecordMessage::ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	m_strHistoryText = "";
	m_strRecentHistoryText = "";

	//----------------------------------------------------------------------------
	// History
	// Time
	SetTime(pHistoryRecord->GetTime());

	// Service    
	CString strService = pHistoryRecord->GetService();
	if (!strService.IsEmpty())
	{
		SetService(strService);
	}

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


CString CHistoryRecordMessage::GetHistoryText()
{
	CString strText;

	if (m_strHistoryText.IsEmpty())
	{
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

		m_strHistoryText = strText;
	}
	else
	{
		strText = m_strHistoryText;
	}

	return strText;
}


CString CHistoryRecordMessage::GetRecentHistoryText()
{
	CString strText;

	if (m_strRecentHistoryText.IsEmpty())
	{
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

		m_strRecentHistoryText = strText;
	}
	else
	{
		strText = m_strRecentHistoryText;
	}

	return strText;
}


void CHistoryRecordMessage::GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
{
	listEvents.RemoveAll();
	int nEventsCount = 0;

	CAtlList<IUOLMessengerHistoryEventPtr> listAllEvents;
	GetEvents(listAllEvents);
	for (POSITION pos = listAllEvents.GetTailPosition(); pos;)
	{
		IUOLMessengerHistoryEventPtr pHistoryEvent = listAllEvents.GetPrev(pos);
		listEvents.AddHead(pHistoryEvent);

		nEventsCount++;

		if (nEventsCount >= HR_RECENT_HISTORY_MAX_EVENTS)
		{
			break;
		}
	}
}


IUOLMessengerAccountPtr CHistoryRecordMessage::GetAccount()
{
	return m_pAccount;
}


CString CHistoryRecordMessage::GetContactName() const
{
	return m_strContact;
}


CString CHistoryRecordMessage::GetType() const
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


CString CHistoryRecordMessage::GetEventFrom() const
{
	ATLASSERT(m_pMessageEntry);
	
	CString strEventFrom;

	switch (m_pMessageEntry->nMessageSender)
	{
	case IUOLMessengerConversation::MT_ACCOUNT_USER:
		strEventFrom = m_pAccount->GetUserId();
		break;
	case IUOLMessengerConversation::MT_CONTACT:
		strEventFrom = GetContactName();
		break;
	case IUOLMessengerConversation::MT_SYSTEM:
		break;
	case IUOLMessengerConversation::MT_FILE_TRANSFER:
		break;
	}

	return strEventFrom;
}


CString CHistoryRecordMessage::GetEventTo() const
{
	ATLASSERT(m_pMessageEntry);
	
	CString strEventTo;

	switch (m_pMessageEntry->nMessageSender)
	{
	case IUOLMessengerConversation::MT_ACCOUNT_USER:
		strEventTo = GetContactName();
		break;
	case IUOLMessengerConversation::MT_CONTACT:
	case IUOLMessengerConversation::MT_SYSTEM:	
	case IUOLMessengerConversation::MT_FILE_TRANSFER:	
		strEventTo = m_pAccount->GetUserId();
		break;
	}

	return strEventTo;
}

CString CHistoryRecordMessage::FindAlias(const CString& strName)
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


CString CHistoryRecordMessage::GetPrplProtocolId() const
{
	CString strProtocolId;
	strProtocolId.Format(_T("prpl-%s"), GetProtocolId());
	strProtocolId.MakeLower();	

	return strProtocolId;
}

///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordMessageList Class
///////////////////////////////////////////////////////////////////////////////

CHistoryRecordMessageList::CHistoryRecordMessageList(CHistoryRecordMessagePtr pHistoryRecordMessage) :
	m_bStopFlag(FALSE),
	m_thread(this)
{
	m_pHistoryRecordMessage = pHistoryRecordMessage;
}

CHistoryRecordMessageList::~CHistoryRecordMessageList()
{
	StopLoad();
	RemoveAll();
}


CString CHistoryRecordMessageList::GetHistoryText(CTime eventTime)
{
	CString strHistoryText;

	CHistoryRecordMessagePtr pHistoryRecordMessage;
	pHistoryRecordMessage = ap_dynamic_cast<CHistoryRecordMessagePtr>(GetHistoryRecord(eventTime));

	if (pHistoryRecordMessage)
	{
		strHistoryText = pHistoryRecordMessage->GetHistoryText();
	}

	return strHistoryText;
}


IUOLMessengerHistoryRecordPtr CHistoryRecordMessageList::GetHistoryRecord(CTime eventTime)
{
	CHistoryRecordMessagePtr pHistoryRecordMessage = NULL;
	
	for (POSITION pos = m_listHistoryRecordMessage.GetHeadPosition(); pos; )
	{
		pHistoryRecordMessage = m_listHistoryRecordMessage.GetNext(pos);
		CTime recordTime = pHistoryRecordMessage->GetTime();
		if (recordTime == eventTime)
		{
			break;
		}
	}
	ATLASSERT(pHistoryRecordMessage);
	
	return pHistoryRecordMessage;
}


IUOLMessengerHistoryRecordPtr CHistoryRecordMessageList::GetHistoryRecordByIndex(int nIndex)
{
	CHistoryRecordMessagePtr pHistoryRecordMessage = NULL;
	POSITION pos = m_listHistoryRecordMessage.FindIndex(nIndex);
	
	if (pos)
	{
		pHistoryRecordMessage = m_listHistoryRecordMessage.GetAt(pos);
	}

	return pHistoryRecordMessage;
}


int CHistoryRecordMessageList::GetCount()
{
	return m_listHistoryRecordMessage.GetCount();
}


void CHistoryRecordMessageList::DeleteHistory(CTime eventTime)
{
	CHistoryRecordMessagePtr pHistoryRecordMessage = NULL;
	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	for (POSITION pos = m_listHistoryRecordMessage.GetHeadPosition(); pos; m_listHistoryRecordMessage.GetNext(pos))
	{
		CHistoryRecordMessagePtr pHistoryRecordMessage = m_listHistoryRecordMessage.GetAt(pos);
		if (pHistoryRecordMessage->GetTime() == eventTime)
		{
			IUOLMessengerHistoryManager4Ptr pHistoryManager5;
			pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(CUIMApplication::GetApplication()->GetHistoryManager());
			if (pHistoryManager5)
			{
				pHistoryManager5->DeleteHistory(pHistoryRecordMessage);
				m_listHistoryRecordMessage.RemoveAt(pos);
				
				break;
			}
		}
	}
}


void CHistoryRecordMessageList::DeleteAllHistory()
{
	IUOLMessengerHistoryManager4Ptr pHistoryManager5;
	pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(CUIMApplication::GetApplication()->GetHistoryManager());
	if (pHistoryManager5)
	{
		pHistoryManager5->DeleteAllHistoryFiles(m_pHistoryRecordMessage);
		RemoveAll();
	}
}


void CHistoryRecordMessageList::RemoveAll()
{
	m_listHistoryRecordMessage.RemoveAll();
}


CString CHistoryRecordMessageList::GetProtocolId()
{
	return m_pHistoryRecordMessage->GetProtocolId();
}


void CHistoryRecordMessageList::GetHistoryEvents(CAtlList<CTime>& listHistoryEvents)
{
	listHistoryEvents.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		for (POSITION pos = m_listHistoryRecordMessage.GetHeadPosition(); pos; )
		{
			CHistoryRecordMessagePtr pHistoryRecordMessage = m_listHistoryRecordMessage.GetNext(pos);
			listHistoryEvents.AddTail(pHistoryRecordMessage->GetTime());
		}
	}
}


void CHistoryRecordMessageList::GetEventRecord(CHistoryRecordMessagePtr pHistoryRecordMessage)
{
	IUOLMessengerHistoryManager4Ptr pHistoryManager5;
	pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(CUIMApplication::GetApplication()->GetHistoryManager());
	if (pHistoryManager5)
	{
		pHistoryManager5->GetEventRecord(pHistoryRecordMessage);
	}
}


BOOL CHistoryRecordMessageList::StartLoad()
{
	if (!m_thread.IsAlive())
	{
		m_bStopFlag = FALSE;
		return m_thread.Start();
	}

	return FALSE;
}


BOOL CHistoryRecordMessageList::StopLoad()
{
	if (m_thread.IsAlive())
	{
		m_bStopFlag = TRUE;
		m_thread.MessageLoopWait();
	}

	return TRUE;
}


void CHistoryRecordMessageList::Run()
{
	CAtlList<CTime> listEvents;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	m_listHistoryRecordMessage.RemoveAll();

	WTL::CFindFile finder;
	CString strPattern;
	strPattern.Format(_T("%s\\*.xml"), m_pHistoryRecordMessage->GetHistoryPath());

	CString strLastFile;
	CString strFileFound;
	if (finder.FindFile(strPattern))
	{
		do
		{
			strFileFound = finder.GetFilePath();
			ATLPath::StripPath(strFileFound.GetBuffer());
			strFileFound.ReleaseBuffer();

			int nYear = 2000;
			int nMonth = 1;
			int nDay = 1;
			int nHour = 0;
			int nMinute = 0;
			int nSecond = 0;

			if (strFileFound.GetLength() > 8)
			{
				//index...: 0123_45_67_89_01_23_4567
				//filename: 2008_04_22_10_58_45_.xml

				try
				{
					nYear = ::_tstoi(strFileFound.Mid(0, 4));
					nMonth = ::_tstoi(strFileFound.Mid(4, 2));
					nDay = ::_tstoi(strFileFound.Mid(6, 2));
					if (strFileFound.GetLength() > 14)
					{
						nHour = ::_tstoi(strFileFound.Mid(8, 2));
						nMinute = ::_tstoi(strFileFound.Mid(10, 2));
						nSecond = ::_tstoi(strFileFound.Mid(12, 2));
					}
				}
				catch(char * strError)
				{
					ATLTRACE(_T("%s exception: %s\n"), __FUNCTION__, strError);
				}
			}

			CTime timeHistory(nYear, nMonth, nDay, nHour, nMinute, nSecond);

			if (listEvents.GetCount() == 0)
			{
				listEvents.AddHead(timeHistory);
			}
			else
			{
				for (POSITION pos = listEvents.GetHeadPosition(); pos; listEvents.GetNext(pos))
				{
					CTime listItem = listEvents.GetAt(pos);
					
					if (timeHistory > listItem)
					{
						listEvents.InsertBefore(pos, timeHistory);
						break;
					}
					else
					{
						listEvents.InsertAfter(pos, timeHistory);
						break;
					}
				}
			}
		}
		while (finder.FindNextFile() && !m_bStopFlag);
	}
	finder.Close();	

	// Create each History Record from list
	for (POSITION pos = listEvents.GetHeadPosition(); pos;)
	{
		if (m_bStopFlag)
		{
			break;
		}

		CTime timeHistory = listEvents.GetNext(pos);

		CHistoryRecordMessagePtr pHistoryRecordMessage;
		pHistoryRecordMessage = new CHistoryRecordMessage(
			m_pHistoryRecordMessage->GetAccount(), m_pHistoryRecordMessage->GetContactName());

		pHistoryRecordMessage->SetTime(timeHistory);
		GetEventRecord(pHistoryRecordMessage);

		m_listHistoryRecordMessage.AddTail(pHistoryRecordMessage);

		m_ObserverNotifier.NotifyAll(&CUOLMessengerHistoryRecordListObserver::OnUpdateHistoryRecordListStart, 
			pHistoryRecordMessage);
	}

	if (!m_bStopFlag)
	{
		m_ObserverNotifier.NotifyAll(&CUOLMessengerHistoryRecordListObserver::OnUpdateHistoryRecordListFinish);
	}
}