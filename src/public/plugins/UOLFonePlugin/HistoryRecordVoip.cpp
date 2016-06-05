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
#include "HistoryRecordVoip.h"
#include "resource.h"
#include "FacadeWrapper.h"
#include "Core/UOLFoneAccountManager.h"
#include "Core/UOLFoneController.h"


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordVoip
///////////////////////////////////////////////////////////////////////////////
CHistoryRecordVoip::CHistoryRecordVoip(IUOLMessengerAccountPtr pAccount,
                                       const CString& strEventFrom, 
									   const CString& strEventTo, 
									   const CString& strStatus,
									   const CString& strType,
									   const CString& strText):
m_pAccount(pAccount)
{
	m_strHistoryText = "";
	
	//----------------------------------------------------------------------------
	// History
	// Time
	SetTime(CTime::GetCurrentTime());

	// Service 
	CString strService;
	strService.LoadString(IDS_HISTORY_RECORD_SERVICE_VOIP);
	SetService(strService);

	// Account UserId
	SetAccountUserId(pAccount->GetUserId());

	// ProtocolId
	CString strProtocolId;
	strProtocolId.LoadString(IDS_VOIP_PROTOCOL_ID);
	SetProtocolId(strProtocolId);
	
	//----------------------------------------------------------------------------
	// Event
	CString strEventText = strText;

	CHistoryRecordEventPtr pVoipEvent= new CHistoryRecordEvent(GetTime(), 
		strStatus, strType, strEventFrom, strEventTo, strEventText);

	if (pVoipEvent)
	{
		AddEvent(pVoipEvent);
	}
}


CHistoryRecordVoip::CHistoryRecordVoip(IUOLMessengerAccountPtr pAccount, const CString& strTypeFilter) :
m_strTypeFilter(strTypeFilter),
m_pAccount(pAccount)
{
	m_strHistoryText = "";

	//----------------------------------------------------------------------------
	// History
	// Time
	SetTime(CTime::GetCurrentTime());

	// Service 
	CString strService;
	strService.LoadString(IDS_HISTORY_RECORD_SERVICE_VOIP);
	SetService(strService);

	// Account UserId
	SetAccountUserId(pAccount->GetUserId());

	// ProtocolId
	CString strProtocolId;
	strProtocolId.LoadString(IDS_VOIP_PROTOCOL_ID);
	SetProtocolId(strProtocolId);
}


CHistoryRecordVoip::~CHistoryRecordVoip()
{
}


BOOL CHistoryRecordVoip::CanLog() const
{
	return TRUE;
}


BOOL CHistoryRecordVoip::ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	m_strHistoryText = "";

	//----------------------------------------------------------------------------
	// History
	// Time
	SetTime(pHistoryRecord->GetTime());

	// Service    
	CString strService;
	strService.LoadString(IDS_HISTORY_RECORD_SERVICE_VOIP);

	// Version
	CString strVersion = pHistoryRecord->GetVersion();

	if (0 != strVersion.Compare("1.0"))
	{
		pHistoryRecord->SetService(strService);
	}

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


CString CHistoryRecordVoip::GetHistoryText()
{
	CString strText;

	if (m_strHistoryText.IsEmpty())
	{
		CAtlList<IUOLMessengerHistoryEventPtr> listEvents;
		GetEvents(listEvents);
		for (POSITION pos = listEvents.GetHeadPosition(); pos;)
		{
			IUOLMessengerHistoryEventPtr pHistoryEvent = listEvents.GetNext(pos);
			CString strEventWho = GetEventFromByType(pHistoryEvent);

			// All events
			CString strTypeAll;
			strTypeAll.LoadString(IDS_HISTORY_CALL_STATUS_ALL);
			if (0 == m_strTypeFilter.Compare(strTypeAll))
			{
				strText.Append("(");
				strText.Append(pHistoryEvent->GetTime());
				strText.Append(") ");
				strText.Append(FindAlias(strEventWho));
				strText.Append(": ");
				strText.Append(pHistoryEvent->GetEventText());
				strText.Append("\n\n");
			}
			else if (0 == m_strTypeFilter.Compare(pHistoryEvent->GetType()))
			{
				strText.Append("(");
				strText.Append(pHistoryEvent->GetTime());
				strText.Append(") ");
				strText.Append(FindAlias(strEventWho));
				strText.Append(": ");
				strText.Append(pHistoryEvent->GetEventText());
				strText.Append("\n\n");
			}
		}

		m_strHistoryText = strText;
	}
	else
	{
		strText = m_strHistoryText;
	}

	return strText;
}


CString CHistoryRecordVoip::GetRecentHistoryText()
{
	return GetHistoryText();
}


void CHistoryRecordVoip::GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
{
	GetEvents(listEvents);
}


IUOLMessengerAccountPtr CHistoryRecordVoip::GetAccount()
{
	return m_pAccount;
}


CString CHistoryRecordVoip::GetTypeFilter()
{
	return m_strTypeFilter;
}


CString CHistoryRecordVoip::FindAlias(const CString& strName)
{
	CUOLFoneAccountManagerPtr pAccountManager = CUOLFoneController::GetInstance()->GetUOLFoneAccountManager();
	
	// Find User/Contact alias
	CString strAcountUserId = GetAccountUserId();
	CString strAlias = strName;

	// Get account
	IUOLMessengerAccountPtr pAccount;
	pAccount = pAccountManager->GetIMAccount(strAcountUserId);
	if (!pAccount)
	{
		return strAlias;
	}

	// is sender (EventFrom) account user?
	if (0 == strAcountUserId.CompareNoCase(strName))
	{
		strAlias = pAccount->GetNickName();
	}
	else
	{
		IUOLMessengerContactPtr pContact;
		pContact = CFacadeWrapper::GetInstance()->FindContact(pAccount, strName);
		if (pContact)
		{
			strAlias = pContact->GetAlias();
		}
	}

	return strAlias;
}

CString CHistoryRecordVoip::GetEventFromByType(IUOLMessengerHistoryEventPtr pHistoryEvent)
{
	//IDS_HISTORY_CALL_STATUS_DIALED
	//IDS_HISTORY_CALL_STATUS_MISSED
	//IDS_HISTORY_CALL_STATUS_RECEIVED
	
	CString strType;

	// Compare with IDS_HISTORY_CALL_STATUS_DIALED
	strType.LoadString(IDS_HISTORY_CALL_STATUS_DIALED);
	if (0 == strType.Compare(pHistoryEvent->GetType()))
	{
		return pHistoryEvent->GetEventTo();
	}
	
	// Compare with IDS_HISTORY_CALL_STATUS_MISSED
	strType.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);
	if (0 == strType.Compare(pHistoryEvent->GetType()))
	{
		return pHistoryEvent->GetEventFrom();
	}

	// Compare with IDS_HISTORY_CALL_STATUS_RECEIVED
	strType.LoadString(IDS_HISTORY_CALL_STATUS_RECEIVED);
	if (0 == strType.Compare(pHistoryEvent->GetType()))
	{
		return pHistoryEvent->GetEventFrom();
	}

	return pHistoryEvent->GetEventFrom();
}


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordVoipList Class
///////////////////////////////////////////////////////////////////////////////

CHistoryRecordVoipList::CHistoryRecordVoipList(CHistoryRecordVoipPtr pHistoryRecordVoip) :
	m_bStopFlag(FALSE),
	m_thread(this)
{
	m_pHistoryRecordVoip = pHistoryRecordVoip;
}

CHistoryRecordVoipList::~CHistoryRecordVoipList()
{
	RemoveAll();
}


CString CHistoryRecordVoipList::GetHistoryText(CTime eventTime)
{
	CString strHistoryText;

	CHistoryRecordVoipPtr pHistoryRecordVoip;
	pHistoryRecordVoip = ap_dynamic_cast<CHistoryRecordVoipPtr>(GetHistoryRecord(eventTime));

	if (pHistoryRecordVoip)
	{
		strHistoryText = pHistoryRecordVoip->GetHistoryText();
	}

	return strHistoryText;
}


IUOLMessengerHistoryRecordPtr CHistoryRecordVoipList::GetHistoryRecord(CTime eventTime)
{
	CHistoryRecordVoipPtr pHistoryRecordVoip = NULL;
	
	for (POSITION pos = m_listHistoryRecordVoip.GetHeadPosition(); pos; )
	{
		pHistoryRecordVoip = m_listHistoryRecordVoip.GetNext(pos);
		if (pHistoryRecordVoip->GetTime() == eventTime)
		{
			break;
		}
	}
	
	return pHistoryRecordVoip;
}


IUOLMessengerHistoryRecordPtr CHistoryRecordVoipList::GetHistoryRecordByIndex(int nIndex)
{
	CHistoryRecordVoipPtr pHistoryRecordVoip = NULL;
	POSITION pos = m_listHistoryRecordVoip.FindIndex(nIndex);
	
	if (pos)
	{
		pHistoryRecordVoip = m_listHistoryRecordVoip.GetAt(pos);
	}

	return pHistoryRecordVoip;
}


int CHistoryRecordVoipList::GetCount()
{
	return m_listHistoryRecordVoip.GetCount();
}


void CHistoryRecordVoipList::DeleteHistory(CTime eventTime)
{
	CHistoryRecordVoipPtr pHistoryRecordVoip = NULL;
	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	for (POSITION pos = m_listHistoryRecordVoip.GetHeadPosition(); pos; m_listHistoryRecordVoip.GetNext(pos))
	{
		CHistoryRecordVoipPtr pHistoryRecordVoip = m_listHistoryRecordVoip.GetAt(pos);
		if (pHistoryRecordVoip->GetTime() == eventTime)
		{
			IUOLMessengerUIManager7Ptr pUIManager7 = 
				ap_static_cast<IUOLMessengerUIManager7Ptr>(CFacadeWrapper::GetInstance()->GetUIManager5());
			if (pUIManager7)
			{	
				IUOLMessengerHistoryManager4Ptr pHistoryManager5;
				pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(pUIManager7->GetHistoryManager());
				if (pHistoryManager5)
				{
					pHistoryManager5->DeleteHistory(pHistoryRecordVoip);
					m_listHistoryRecordVoip.RemoveAt(pos);

					break;
				}
			}
		}
	}
}


void CHistoryRecordVoipList::DeleteAllHistory()
{
	IUOLMessengerUIManager7Ptr pUIManager7 = 
		ap_static_cast<IUOLMessengerUIManager7Ptr>(CFacadeWrapper::GetInstance()->GetUIManager5());
	if (pUIManager7)
	{	
		IUOLMessengerHistoryManager4Ptr pHistoryManager5;
		pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(pUIManager7->GetHistoryManager());
		if (pHistoryManager5)
		{
			pHistoryManager5->DeleteAllHistoryFiles(m_pHistoryRecordVoip);
			RemoveAll();
		}
	}
}


void CHistoryRecordVoipList::RemoveAll()
{
	m_listHistoryRecordVoip.RemoveAll();
}


CString CHistoryRecordVoipList::GetProtocolId()
{
	return m_pHistoryRecordVoip->GetProtocolId();
}


void CHistoryRecordVoipList::GetHistoryEvents(CAtlList<CTime>& listHistoryEvents)
{
	listHistoryEvents.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		for (POSITION pos = m_listHistoryRecordVoip.GetHeadPosition(); pos; )
		{
			CHistoryRecordVoipPtr pHistoryRecordVoip = m_listHistoryRecordVoip.GetNext(pos);
			listHistoryEvents.AddTail(pHistoryRecordVoip->GetTime());
		}
	}
}


void CHistoryRecordVoipList::GetEventRecord(CHistoryRecordVoipPtr pHistoryRecordVoip)
{
	IUOLMessengerUIManager7Ptr pUIManager7 = 
		ap_static_cast<IUOLMessengerUIManager7Ptr>(CFacadeWrapper::GetInstance()->GetUIManager5());
	if (pUIManager7)
	{	
		IUOLMessengerHistoryManager4Ptr pHistoryManager5;
		pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(pUIManager7->GetHistoryManager());
		if (pHistoryManager5)
		{
			pHistoryManager5->GetEventRecord(pHistoryRecordVoip);
		}
	}
}


BOOL CHistoryRecordVoipList::StartLoad()
{
	if (!m_thread.IsAlive())
	{
		m_bStopFlag = FALSE;
		return m_thread.Start();
	}

	return FALSE;
}


BOOL CHistoryRecordVoipList::StopLoad()
{
	if (m_thread.IsAlive())
	{
		m_bStopFlag = TRUE;
		m_thread.MessageLoopWait();
	}

	return TRUE;
}


void CHistoryRecordVoipList::Run()
{
	CAtlList<CTime> listEvents;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	m_listHistoryRecordVoip.RemoveAll();

	WTL::CFindFile finder;
	CString strPattern;
	strPattern.Format(_T("%s\\*.xml"), m_pHistoryRecordVoip->GetHistoryPath());

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

		CHistoryRecordVoipPtr pHistoryRecordVoip;
		pHistoryRecordVoip = new CHistoryRecordVoip(
			m_pHistoryRecordVoip->GetAccount(), m_pHistoryRecordVoip->GetTypeFilter());

		pHistoryRecordVoip->SetTime(timeHistory);
		GetEventRecord(pHistoryRecordVoip);

		m_listHistoryRecordVoip.AddTail(pHistoryRecordVoip);

		m_ObserverNotifier.NotifyAll(&CUOLMessengerHistoryRecordListObserver::OnUpdateHistoryRecordListStart, 
			pHistoryRecordVoip);
	}

	if (!m_bStopFlag)
	{
		m_ObserverNotifier.NotifyAll(&CUOLMessengerHistoryRecordListObserver::OnUpdateHistoryRecordListFinish);
	}
}
