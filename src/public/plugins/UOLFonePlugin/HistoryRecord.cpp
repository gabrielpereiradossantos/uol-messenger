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
#include "HistoryRecord.h"
#include "FacadeWrapper.h"


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordListNotifier
///////////////////////////////////////////////////////////////////////////////
void CHistoryRecordListNotifier::NotifyAll(void (CUOLMessengerHistoryRecordListObserver::*pfnUpdate)(IUOLMessengerHistoryRecordPtr), IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CAtlList<CUOLMessengerHistoryRecordListObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	CUOLMessengerHistoryRecordListObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(pHistoryRecord);
	}
}


void CHistoryRecordListNotifier::NotifyAll(void (CUOLMessengerHistoryRecordListObserver::*pfnUpdate)())
{
	CAtlList<CUOLMessengerHistoryRecordListObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	CUOLMessengerHistoryRecordListObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)();
	}
}


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordEvent
///////////////////////////////////////////////////////////////////////////////
CHistoryRecordEvent::CHistoryRecordEvent(const CTime& timeEvent, const CString& strStatus, 
										 const CString& strType, const CString& strEventFrom,
										 const CString& strEventTo, const CString& strEventText) :
m_timeEvent(timeEvent),
m_strStatus(strStatus), 
m_strType(strType), 
m_strEventFrom(strEventFrom), 
m_strEventTo(strEventTo), 
m_strEventText(strEventText)
{
}


CHistoryRecordEvent::~CHistoryRecordEvent()
{
}


CString CHistoryRecordEvent::GetDate() const
{
	CString strDate = m_timeEvent.Format(_T("%d/%m/%Y"));
	
	return strDate;
}


CString CHistoryRecordEvent::GetTime() const
{
	CString strTime = m_timeEvent.Format(_T("%H:%M:%S"));
	
	return strTime;
}


CTime CHistoryRecordEvent::GetFullTime() const
{
	return m_timeEvent;
}


CString CHistoryRecordEvent::GetStatus() const
{
	return m_strStatus;
}


CString CHistoryRecordEvent::GetType() const
{
	return m_strType;
}


CString CHistoryRecordEvent::GetEventFrom() const
{
	return m_strEventFrom;
}


CString CHistoryRecordEvent::GetEventTo() const
{
	return m_strEventTo;
}


CString CHistoryRecordEvent::GetEventText() const
{
	return m_strEventText;
}


CString CHistoryRecordEvent::GetEventId() const
{
	CString strTime = m_timeEvent.Format(_T("%Y%m%d%H%M%S"));
	CString strEventId;
	strEventId.Format(_T("%s:%s:%s"), strTime, GetEventFrom(), GetEventTo());
	
	return strEventId;
}


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecord
///////////////////////////////////////////////////////////////////////////////
CHistoryRecord::CHistoryRecord()
{
	m_strVersion = HR_VERSION;
}


CHistoryRecord::~CHistoryRecord()
{
	m_listEvents.RemoveAll();
}


CString CHistoryRecord::GetVersion() const
{
	return m_strVersion;
}


CString CHistoryRecord::GetService() const
{
	return m_strService;
}


CString CHistoryRecord::GetAccountUserId() const
{
	return m_strAccountUserId;
}


CString CHistoryRecord::GetContactName() const
{
	return "";
}


CString CHistoryRecord::GetProtocolId() const
{
	return m_strProtocolId;
}


BOOL CHistoryRecord::CanLog() const
{
	return TRUE;
}


CTime CHistoryRecord::GetTime() const
{
	return m_timeHistory;
}


void CHistoryRecord::SetTime(const CTime& timeHistory)
{
	m_timeHistory = timeHistory;
}


CString CHistoryRecord::GetHistoryPath() const
{
	// Base profile path
	CString strBasePath;
	IUOLMessengerProfileElementPtr pProfileElement;
	CString strCurrentProfile = CFacadeWrapper::GetFacadeInstance()->GetProfileManager()->GetCurrentProfile();
	CFacadeWrapper::GetFacadeInstance()->GetProfileManager()->GetProfileElement(strCurrentProfile, pProfileElement);
	strBasePath = pProfileElement->GetProfilePath();
	strBasePath.Append("\\History");

	// History path	
	CString strHistoryPath;
	strHistoryPath.Format(_T("%s\\%s.%s"), strBasePath, GetAccountUserId(), 
		GetProtocolId());

	// Create directories if needed
	CreateDir(strHistoryPath);

	return strHistoryPath;
}


CString CHistoryRecord::GetFileName() const
{
	CString strFileName = m_timeHistory.Format(_T("%Y%m%d"));
	strFileName.Append(_T(".xml"));
	
	return strFileName;
}


BOOL CHistoryRecord::ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	return FALSE;
}


CString CHistoryRecord::GetHistoryText()
{
	return "";
}


CString CHistoryRecord::GetRecentHistoryText()
{
	return GetHistoryText();
}


void CHistoryRecord::GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
{
	GetEvents(listEvents);
}


void CHistoryRecord::GetEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
{
	listEvents.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		for (POSITION pos = m_listEvents.GetHeadPosition(); pos; )
		{
			IUOLMessengerHistoryEventPtr pHistoryEvent = m_listEvents.GetNext(pos);
			listEvents.AddTail(pHistoryEvent);
		}
	}
}


void CHistoryRecord::AddEvent(IUOLMessengerHistoryEventPtr pHistoryEvent)
{
	CHistoryRecordEventPtr pHistoryRecordEvent;
	pHistoryRecordEvent = ap_dynamic_cast<CHistoryRecordEventPtr>(pHistoryEvent);

	if (pHistoryRecordEvent)
	{
		m_listEvents.AddTail(pHistoryRecordEvent);
	}
}


void CHistoryRecord::RemoveEvent(IUOLMessengerHistoryEventPtr pHistoryEvent)
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		CHistoryRecordEventPtr pHistoryRecordEvent;
		pHistoryRecordEvent = ap_dynamic_cast<CHistoryRecordEventPtr>(pHistoryEvent);

		if (pHistoryRecordEvent)
		{
			for (POSITION pos = m_listEvents.GetHeadPosition(); pos;)
			{
				CHistoryRecordEventPtr pHistoryEventOriginal = m_listEvents.GetNext(pos);
				if (pHistoryEventOriginal)
				{
					CString strDeleteEventID = pHistoryEvent->GetEventId();
					CString strOriginalEventID = pHistoryEventOriginal->GetEventId();
					if (0 == strOriginalEventID.CompareNoCase(strDeleteEventID))
					{
						m_listEvents.RemoveAt(pos);
						break;
					}
				}
			}
		}
	}
}


void CHistoryRecord::ClearEvents()
{
	m_listEvents.RemoveAll();
}


void CHistoryRecord::SetVersion(const CString& strVersion)
{
	m_strVersion = strVersion;
}


void CHistoryRecord::SetService(const CString& strService)
{
	m_strService = strService;
}


void CHistoryRecord::SetAccountUserId(const CString& strAccountUserId)
{
	m_strAccountUserId = strAccountUserId;
}


void CHistoryRecord::SetProtocolId(const CString& strProtocolId)
{
	m_strProtocolId = strProtocolId;
}


BOOL CHistoryRecord::CreateDir(const CString& strDirPath) const
{
	int nFind;
	CString strDir;

	BOOL bPathExists = TRUE;

	nFind = strDirPath.Find('\\');
	while (-1 != nFind)
	{
		strDir = strDirPath.Left(nFind);

		bPathExists = ::PathFileExists(strDir);
		if (!bPathExists)
		{
			bPathExists = ::CreateDirectory(strDir, NULL);
		}
		nFind = strDirPath.Find('\\', nFind+1);
	}

	bPathExists = ::PathFileExists(strDirPath);
	if (!bPathExists)
	{
		bPathExists = ::CreateDirectory(strDirPath, NULL);
	}

	return bPathExists;
}


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordList Class
///////////////////////////////////////////////////////////////////////////////

CHistoryRecordList::CHistoryRecordList()
{
}

CHistoryRecordList::~CHistoryRecordList()
{
}

CString CHistoryRecordList::GetHistoryText(CTime eventTime)
{
	return "";
}

IUOLMessengerHistoryRecordPtr CHistoryRecordList::GetHistoryRecord(CTime eventTime)
{
	return NULL;
}

IUOLMessengerHistoryRecordPtr CHistoryRecordList::GetHistoryRecordByIndex(int nIndex)
{
	return NULL;
}

int CHistoryRecordList::GetCount()
{
	return 0;
}

CString CHistoryRecordList::GetProtocolId()
{
	return "";
}

BOOL CHistoryRecordList::GetShowTime()
{
	// Never show Time
	return FALSE;
}

BOOL CHistoryRecordList::StartLoad()
{
	return FALSE;
}

BOOL CHistoryRecordList::StopLoad()
{
	return FALSE;
}

void CHistoryRecordList::RegisterObserver(CUOLMessengerHistoryRecordListObserver* pObserver)
{
	m_ObserverNotifier.RegisterObserver(pObserver);
}


void CHistoryRecordList::UnregisterObserver(CUOLMessengerHistoryRecordListObserver* pObserver)
{
	m_ObserverNotifier.UnregisterObserver(pObserver);
}