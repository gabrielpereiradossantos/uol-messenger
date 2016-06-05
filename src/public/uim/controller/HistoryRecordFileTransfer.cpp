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
#include "HistoryRecordFileTransfer.h"
#include "../controller/UIMApplication.h"
#include "../FileUtils.h"

#include <interfaces/IUOLMessengerHistoryRecord.h>


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordFileTransferEvent
///////////////////////////////////////////////////////////////////////////////
CHistoryRecordFileTransferEvent::CHistoryRecordFileTransferEvent(const CTime& timeEvent, const CString& strStatus, 
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


CHistoryRecordFileTransferEvent::~CHistoryRecordFileTransferEvent()
{
}


CString CHistoryRecordFileTransferEvent::GetDate() const
{
	CString strDate = m_timeEvent.Format(_T("%d/%m/%Y"));
	
	return strDate;
}


CString CHistoryRecordFileTransferEvent::GetTime() const
{
	CString strTime = m_timeEvent.Format(_T("%H:%M:%S"));
	
	return strTime;
}


CTime CHistoryRecordFileTransferEvent::GetFullTime() const
{
	return m_timeEvent;
}


CString CHistoryRecordFileTransferEvent::GetStatus() const
{
	return m_strStatus;
}


CString CHistoryRecordFileTransferEvent::GetType() const
{
	return m_strType;
}


CString CHistoryRecordFileTransferEvent::GetEventFrom() const
{
	return m_strEventFrom;
}


CString CHistoryRecordFileTransferEvent::GetEventTo() const
{
	return m_strEventTo;
}


CString CHistoryRecordFileTransferEvent::GetEventText() const
{
	return m_strEventText;
}


CString CHistoryRecordFileTransferEvent::GetEventId() const
{
	CString strEventId;
	strEventId.Format(_T("%s:%s"), GetFileTransferContactName(), GetEventText());
	
	return strEventId;
}


DWORD CHistoryRecordFileTransferEvent::GetFileTransferOperation() const
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	DWORD dwTransferOperation;
	CString strType = GetType();
	
	if (0 == strType.CompareNoCase(pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_FILE_TRANSFER_SEND")))
	{
		dwTransferOperation = IUOLMessengerFileTransfer::FTO_SEND;
	}
	else if (0 == strType.CompareNoCase(pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_FILE_TRANSFER_RECEIVE")))
	{
		dwTransferOperation = IUOLMessengerFileTransfer::FTO_RECEIVE;
	}

	return dwTransferOperation;
}


FileTransferStatus CHistoryRecordFileTransferEvent::GetFileTransferStatus()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	FileTransferStatus ftStatus;
	CString strStatus = GetStatus();
	
	if (0 == strStatus.CompareNoCase(pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_FILE_TRANSFER_STATUS_CANCELED")))
	{
		ftStatus = FileTransferStatus::FILE_TRANSFER_STATUS_CANCEL_LOCAL;
	}
	else if (0 == strStatus.CompareNoCase(pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_FILE_TRANSFER_STATUS_DONE")))
	{
		ftStatus = FileTransferStatus::FILE_TRANSFER_STATUS_DONE;
	}

	return ftStatus;
}


CString CHistoryRecordFileTransferEvent::GetFileTransferContactName() const
{
	CString strContactName;
	
	DWORD dwTransferOperation = GetFileTransferOperation();
	if (dwTransferOperation == IUOLMessengerFileTransfer::FTO_SEND)
	{
		strContactName = GetEventTo();
	}
	else if (dwTransferOperation == IUOLMessengerFileTransfer::FTO_RECEIVE)
	{
		strContactName = GetEventFrom();
	}

	return strContactName;
}


CString CHistoryRecordFileTransferEvent::GetFileTransferFileName()
{
	CString strFileName = GetEventText();
	ATLPath::StripPath(strFileName.GetBuffer());
	strFileName.ReleaseBuffer();

	return strFileName;
}


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordFileTransfer
///////////////////////////////////////////////////////////////////////////////
CHistoryRecordFileTransfer::CHistoryRecordFileTransfer(IUOLMessengerFileTransferPtr pFileTransfer) : 
m_pFileTransfer(pFileTransfer)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	//----------------------------------------------------------------------------
	// History
	// Time
	SetTime(CTime::GetCurrentTime());

	// Service    
	CString strService = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_SERVICE_FILE_TRANSFER");
	SetService(strService);
	
	// Account UserId
	SetAccountUserId(pFileTransfer->GetAccount()->GetUserId());

	// ProtocolId
	CString strProtocolId = pFileTransfer->GetAccount()->GetProtocolId();
	strProtocolId.Replace("prpl-", "");
	strProtocolId.MakeUpper();
	SetProtocolId(strProtocolId);

	// Version
	m_strVersion = HR_VERSION;

	//----------------------------------------------------------------------------
	// Event
	if (CanLog())
	{
		CString strStatus = GetFileTransferStatus();
		CString strType = GetFileTransferType();
		CString strEventFrom;
		CString strEventTo;
		GetFileTransferOperation(strEventFrom, strEventTo);
		CString strEventText = m_pFileTransfer->GetLocalFilePath();

		CHistoryRecordFileTransferEventPtr pHistoryEvent = new CHistoryRecordFileTransferEvent(GetTime(), 
			strStatus, strType, strEventFrom, strEventTo, strEventText);

		if (pHistoryEvent)
		{
			AddEvent(pHistoryEvent);
		}
	}
}


CHistoryRecordFileTransfer::CHistoryRecordFileTransfer()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	//----------------------------------------------------------------------------
	// History
	// Time
	CTime timeHistory = CTime::GetCurrentTime();
	SetTime(timeHistory);

	// Service    
	CString strService = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_SERVICE_FILE_TRANSFER");
	SetService(strService);
}


CHistoryRecordFileTransfer::~CHistoryRecordFileTransfer()
{
	m_listEvents.RemoveAll();
}


CString CHistoryRecordFileTransfer::GetVersion() const
{
	return m_strVersion;
}


CString CHistoryRecordFileTransfer::GetService() const
{
	return m_strService;
}


CString CHistoryRecordFileTransfer::GetAccountUserId() const
{
	return m_strAccountUserId;
}


CString CHistoryRecordFileTransfer::GetContactName() const
{
	return m_pFileTransfer->GetContactName();
}


CString CHistoryRecordFileTransfer::GetProtocolId() const
{
	return m_strProtocolId;
}


BOOL CHistoryRecordFileTransfer::CanLog() const
{
	return TRUE;
}


CTime CHistoryRecordFileTransfer::GetTime() const
{
	return m_timeHistory;
}


void CHistoryRecordFileTransfer::SetTime(const CTime& timeHistory)
{
	m_timeHistory = timeHistory;
}


CString CHistoryRecordFileTransfer::GetHistoryPath() const
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strHistoryFolder = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_BASE_FOLDER_NAME");

	// Base profile path
	CString strHistoryPath;
	IUOLMessengerProfileElementPtr pProfileElement;
	CString strCurrentProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();
	CUIMApplication::GetApplication()->GetProfileManager()->GetProfileElement(strCurrentProfile, pProfileElement);
	strHistoryPath = pProfileElement->GetProfilePath();
	strHistoryPath.Append("\\");
	strHistoryPath.Append(strHistoryFolder);

	// Create directories if needed
	CFileUtils::CreateDirectory(strHistoryPath);

	return strHistoryPath;
}


CString CHistoryRecordFileTransfer::GetFileName() const
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strFileName = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_FILE_TRANSFER_FILE_NAME");
	
	return strFileName;
}


BOOL CHistoryRecordFileTransfer::ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

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
	CString strAccount = pHistoryRecord->GetAccountUserId();
	if (!strAccount.IsEmpty())
	{
		SetAccountUserId(strAccount);
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

		CHistoryRecordFileTransferEventPtr pHistoryEvent = new CHistoryRecordFileTransferEvent(pHistoryEventItem->GetFullTime(),
			pHistoryEventItem->GetStatus(), pHistoryEventItem->GetType(), pHistoryEventItem->GetEventFrom(), 
			pHistoryEventItem->GetEventTo(), pHistoryEventItem->GetEventText());

		if (pHistoryEvent)
		{
			AddEvent(pHistoryEvent);
		}
	}
	return TRUE;
}


CString CHistoryRecordFileTransfer::GetHistoryText()
{
	// not applicable on file transfer event
	return "";
}


CString CHistoryRecordFileTransfer::GetRecentHistoryText()
{
	// not applicable on file transfer event
	return "";
}


void CHistoryRecordFileTransfer::GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
{
	GetEvents(listEvents);
}


void CHistoryRecordFileTransfer::GetEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents)
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


void CHistoryRecordFileTransfer::AddEvent(IUOLMessengerHistoryEventPtr pHistoryEvent)
{
	CHistoryRecordFileTransferEventPtr pHistoryRecordEvent;
	pHistoryRecordEvent = ap_dynamic_cast<CHistoryRecordFileTransferEventPtr>(pHistoryEvent);

	if (pHistoryRecordEvent)
	{
		m_listEvents.AddTail(pHistoryRecordEvent);
	}
}


void CHistoryRecordFileTransfer::RemoveEvent(IUOLMessengerHistoryEventPtr pHistoryEvent)
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		CHistoryRecordFileTransferEventPtr pHistoryRecordEvent;
		pHistoryRecordEvent = ap_dynamic_cast<CHistoryRecordFileTransferEventPtr>(pHistoryEvent);

		if (pHistoryRecordEvent)
		{
			for (POSITION pos = m_listEvents.GetHeadPosition(); pos; m_listEvents.GetNext(pos))
			{
				CHistoryRecordFileTransferEventPtr pHistoryEventOriginal = m_listEvents.GetAt(pos);
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


void CHistoryRecordFileTransfer::ClearEvents()
{
	m_listEvents.RemoveAll();
}


void CHistoryRecordFileTransfer::SetVersion(const CString& strVersion)
{
	m_strVersion = strVersion;
}


void CHistoryRecordFileTransfer::SetService(const CString& strService)
{
	m_strService = strService;
}


void CHistoryRecordFileTransfer::SetAccountUserId(const CString& strAccountUserId)
{
	m_strAccountUserId = strAccountUserId;
}


void CHistoryRecordFileTransfer::SetProtocolId(const CString& strProtocolId)
{
	m_strProtocolId = strProtocolId;
}


CString CHistoryRecordFileTransfer::GetFileTransferStatus() const
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	CString strStatus;
	FileTransferStatus ftStatus = m_pFileTransfer->GetStatus();
	if (ftStatus == FILE_TRANSFER_STATUS_DONE)
	{
		strStatus = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_FILE_TRANSFER_STATUS_DONE");
	}
	else if ((ftStatus == FILE_TRANSFER_STATUS_CANCEL_REMOTE) || 
			 (ftStatus == FILE_TRANSFER_STATUS_CANCEL_LOCAL))
	{
		strStatus = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_FILE_TRANSFER_STATUS_CANCELED");
	}
	else if ((ftStatus == FILE_TRANSFER_STATUS_ACCEPTED) || 
			 (ftStatus == FILE_TRANSFER_STATUS_NOT_STARTED))
	{
		strStatus = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_FILE_TRANSFER_STATUS_WAITING");
	}

	return strStatus;
}


void CHistoryRecordFileTransfer::GetFileTransferOperation(CString& strFrom, CString& strTo)
{
	switch(m_pFileTransfer->GetTransferOperation())
	{
	case IUOLMessengerFileTransfer::FTO_SEND:
		strFrom = m_pFileTransfer->GetAccount()->GetUserId();
		strTo = m_pFileTransfer->GetContactName();
		break;
	case IUOLMessengerFileTransfer::FTO_RECEIVE:
		strFrom = m_pFileTransfer->GetContactName();
		strTo = m_pFileTransfer->GetAccount()->GetUserId();
		break;
	}
}


CString CHistoryRecordFileTransfer::FindAlias(const CString& strName) const
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


CString CHistoryRecordFileTransfer::GetPrplProtocolId() const
{
	CString strProtocolId;
	strProtocolId.Format(_T("prpl-%s"), GetProtocolId());
	strProtocolId.MakeLower();	

	return strProtocolId;
}


CString CHistoryRecordFileTransfer::GetFileTransferType() const
{
	CString strType;

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	switch(m_pFileTransfer->GetTransferOperation())
	{
	case IUOLMessengerFileTransfer::FTO_SEND:
		strType = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_FILE_TRANSFER_SEND");
		break;
	case IUOLMessengerFileTransfer::FTO_RECEIVE:
		strType = pStringLoader->GetValue("uol.resource.IDS_HISTORY_RECORD_TYPE_FILE_TRANSFER_RECEIVE");
		break;
	}

	return strType;
}


void CHistoryRecordFileTransfer::GetFileTransferEvents(CAtlList<CFileTransferPtr>& listFileTransferEvents)
{
	listFileTransferEvents.RemoveAll();

	IUOLMessengerAccountPtr pAccount;
	pAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
		GetAccountUserId(), GetPrplProtocolId());

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		for (POSITION pos = m_listEvents.GetHeadPosition(); pos; )
		{
			CHistoryRecordFileTransferEventPtr pHistoryEvent = m_listEvents.GetNext(pos);
			
			// Create fake file transfer structure
			long lTransferId = atol(pHistoryEvent->GetFullTime().Format("%Y%H%M%S"));
			CFileTransferPtr pFileTransfer;
			pFileTransfer = new CFileTransfer(pAccount, pHistoryEvent->GetFileTransferContactName(), 
				lTransferId, pHistoryEvent->GetFullTime());
			pFileTransfer->SetTransferOperation(pHistoryEvent->GetFileTransferOperation());
			pFileTransfer->SetFileName(pHistoryEvent->GetFileTransferFileName());
			pFileTransfer->SetLocalFilePath(pHistoryEvent->GetEventText());
			pFileTransfer->SetStatus(pHistoryEvent->GetFileTransferStatus());
			
			listFileTransferEvents.AddTail(pFileTransfer);
		}
	}
}


/*
IUOLMessengerFileTransferPtr CHistoryRecordFileTransfer::GetFileTransfer()
{
	IUOLMessengerFileTransferPtr pFileTransfer = m_pFileTransfer;
	
	// Several timer we've got a 
	if ( (m_pFileTransfer == NULL) && (m_listEvents.GetCount() > 0) )
	{
		CHistoryRecordFileTransferEventPtr pHistoryEvent = m_listEvents.GetNext(pos);
		
		pFileTransfer = ap_dynamic_cast<
	}
}


CString CHistoryRecordFileTransfer::GetFileTransferLocalFilePath()
{
}


CString CHistoryRecordFileTransfer::GetFileTransferContactName()
{
}


FileTransferStatus CHistoryRecordFileTransfer::GetFileTransferStatus()
{
}


DWORD CHistoryRecordFileTransfer::GetFileTransferOperation()
{
}


CString CHistoryRecordFileTransfer::GetFileTransferUserId()
{
}
*/
