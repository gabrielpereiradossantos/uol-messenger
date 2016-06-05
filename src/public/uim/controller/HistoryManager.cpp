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
 
#include "StdAfx.h"
#include "HistoryManager.h"
#include "UIMApplication.h"
#include "HistoryRecordMessage.h"

#include <interfaces/IUOLMessengerHistoryRecord.h>

using namespace std;
using namespace MSXML2;


#define CONVERSATION_LATENCY 5


CHistoryManager::CHistoryManager(void)
{
}


CHistoryManager::~CHistoryManager(void)
{
}


void CHistoryManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_listHistoryRecords.RemoveAll();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CHistoryManager::LogEvent(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	if (pHistoryRecord->CanLog())
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_lockFile);

			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

			CString strXMLFileName = GetFileNameForWrite(pHistoryRecord);

			MSXML::IXMLDOMDocumentPtr pXMLDoc;
			MSXML::IXMLDOMElementPtr pElement;
			MSXML::IXMLDOMElementPtr pChildElement;
			MSXML::IXMLDOMElementPtr pEventElement;
			MSXML::IXMLDOMNodePtr pNode;
			MSXML::IXMLDOMAttributePtr pAttribute;
			HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument));

			if (SUCCEEDED(hr))
			{
				pXMLDoc->Putasync(VARIANT_FALSE);
				pXMLDoc->PutpreserveWhiteSpace(VARIANT_TRUE);

				// History
				CString strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT");
				if (pXMLDoc->load(CComVariant(strXMLFileName)) == VARIANT_FALSE)
				{
					pElement = pXMLDoc->createElement(strXMLElement.AllocSysString());

					//Version
					strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_VERSION_ATT");
					pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
					pAttribute->Putvalue(pHistoryRecord->GetVersion().AllocSysString());
					pElement->setAttributeNode(pAttribute);

					//Service
					strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_SERVICE_ATT");
					pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
					pAttribute->Putvalue(pHistoryRecord->GetService().AllocSysString());
					pElement->setAttributeNode(pAttribute);

					// UserId
					strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_USER_ID_ATT");
					pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
					pAttribute->Putvalue(pHistoryRecord->GetAccountUserId().AllocSysString());
					pElement->setAttributeNode(pAttribute);

					// ProtocolId
					strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_PROTOCOL_ID_ATT");
					pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
					pAttribute->Putvalue(pHistoryRecord->GetProtocolId().AllocSysString());
					pElement->setAttributeNode(pAttribute);

					pXMLDoc->appendChild(pElement);
				}
				else
				{
					pElement = pXMLDoc->selectSingleNode(strXMLElement.AllocSysString());
				}

				if (NULL != pElement)
				{
					CAtlList<IUOLMessengerHistoryEventPtr> listEvents;
					pHistoryRecord->GetEvents(listEvents);
					for (POSITION pos = listEvents.GetHeadPosition(); pos;)
					{
						IUOLMessengerHistoryEventPtr pHistoryEvent = listEvents.GetNext(pos);

						// --------------------------------------------------
						// Event header line
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT");
						pNode = pXMLDoc->createElement(strXMLElement.AllocSysString());
						pEventElement = pElement->appendChild(pNode);

						//Date
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_DATE_ATT");
						pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
						pAttribute->Putvalue(pHistoryEvent->GetDate().AllocSysString());
						pEventElement->setAttributeNode(pAttribute);

						//Time
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_TIME_ATT");
						pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
						pAttribute->Putvalue(pHistoryEvent->GetTime().AllocSysString());
						pEventElement->setAttributeNode(pAttribute);

						//Status
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_STATUS_ATT");
						pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
						pAttribute->Putvalue(pHistoryEvent->GetStatus().AllocSysString());
						pEventElement->setAttributeNode(pAttribute);

						//Type
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_TYPE_ATT");
						pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
						pAttribute->Putvalue(pHistoryEvent->GetType().AllocSysString());
						pEventElement->setAttributeNode(pAttribute);

						// --------------------------------------------------
						// Event elements From
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_FROM_ELEMENT");
						pNode = pXMLDoc->createElement(strXMLElement.AllocSysString());
						pChildElement = pEventElement->appendChild(pNode);

						//Name
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_FROM_ID_ATT");
						pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
						pAttribute->Putvalue(pHistoryEvent->GetEventFrom().AllocSysString());
						pChildElement->setAttributeNode(pAttribute);

						// --------------------------------------------------
						// Event elements To
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_TO_ELEMENT");
						pNode = pXMLDoc->createElement(strXMLElement.AllocSysString());
						pChildElement = pEventElement->appendChild(pNode);

						//Name
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_TO_ID_ATT");
						pAttribute = pXMLDoc->createAttribute(strXMLElement.AllocSysString());
						pAttribute->Putvalue(pHistoryEvent->GetEventTo().AllocSysString());
						pChildElement->setAttributeNode(pAttribute);

						// --------------------------------------------------
						// Event elements Text
						strXMLElement = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_TEXT_ELEMENT");
						pNode = pXMLDoc->createElement(strXMLElement.AllocSysString());
						pChildElement = pEventElement->appendChild(pNode);

						MSXML::IXMLDOMCDATASectionPtr pCData;
						pCData = pXMLDoc->createCDATASection(pHistoryEvent->GetEventText().AllocSysString());
						if (NULL != pCData)
						{
							pNode = pNode->appendChild(pCData);
						}
					}
				}

				pXMLDoc->save(CComVariant(strXMLFileName));
			}
			CoUninitialize();
		}
	}
}


void CHistoryManager::GetEventsList(IUOLMessengerHistoryRecordPtr pHistoryRecord, CAtlList<CTime>& listHistoryEvents)
{
	// Check for older files
	// TODO: ConvertOlderFiles is temporally disabled because it is taking so long to convert big files
	//ConvertOlderFiles(pHistoryRecord);
	
	listHistoryEvents.RemoveAll();

	WTL::CFindFile finder;
	CString strPattern;
	strPattern.Format(_T("%s\\*.xml"), pHistoryRecord->GetHistoryPath());

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

			CTime FileTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
			listHistoryEvents.AddTail(FileTime);
		}
		while (finder.FindNextFile());
	}
	finder.Close();
}


void CHistoryManager::GetEventRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CString strFileName = GetFileNameForRead(pHistoryRecord);

	CHistoryGrammar historyGrammar;
	BOOL bDone = historyGrammar.Parse(strFileName, pHistoryRecord->GetTime());

	if (bDone)
	{
		pHistoryRecord->ConvertFrom(historyGrammar.GetHistoryRecord());
	}
}


void CHistoryManager::GetRecentEventRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CTime timeRecentEvent = CTime::GetCurrentTime();

	CAtlList<CTime> listEvents;
	GetEventsList(pHistoryRecord, listEvents);
	if (listEvents.GetCount() > 0)
	{
		timeRecentEvent = (CTime)listEvents.GetHead();

		for (POSITION pos = listEvents.GetHeadPosition(); pos;)
		{
			CTime timeEvent(listEvents.GetNext(pos));

			if (timeEvent > timeRecentEvent)
			{
				timeRecentEvent = timeEvent;
			}
		}
	}

	pHistoryRecord->SetTime(timeRecentEvent);
	GetEventRecord(pHistoryRecord);	
}


void CHistoryManager::DeleteHistory(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CString strFileName = GetFileNameForRead(pHistoryRecord);

	if (ATLPath::FileExists(strFileName) && !ATLPath::IsDirectory(strFileName))
	{
		::DeleteFile(strFileName);
	}
}


void CHistoryManager::DeleteAllHistoryFiles(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	WTL::CFindFile finder;
	CString strPattern;
	strPattern.Format(_T("%s\\*.xml"), pHistoryRecord->GetHistoryPath());

	CString strFileName;
	if (finder.FindFile(strPattern))
	{
		do
		{
			strFileName = finder.GetFilePath();
			if (ATLPath::FileExists(strFileName) && !ATLPath::IsDirectory(strFileName))
			{
				::DeleteFile(strFileName);
			}
		}
		while (finder.FindNextFile());
	}
	finder.Close();
}


void CHistoryManager::DeleteHistoryEvents(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_lockFile);

		CAtlList<IUOLMessengerHistoryEventPtr> listEventsToDelete;
		pHistoryRecord->GetEvents(listEventsToDelete);
		pHistoryRecord->ClearEvents();

		if (listEventsToDelete.GetCount() > 0)
		{
			GetEventRecord(pHistoryRecord);
			for (POSITION pos = listEventsToDelete.GetHeadPosition(); pos;)
			{
				IUOLMessengerHistoryEventPtr pHistoryEventToDelete = listEventsToDelete.GetNext(pos);
				if (pHistoryEventToDelete)
				{
					pHistoryRecord->RemoveEvent(pHistoryEventToDelete);
				}
			}
			
			// Delete old history xml
			CString strFileName = GetFileNameForWrite(pHistoryRecord);
			if (ATLPath::FileExists(strFileName) && !ATLPath::IsDirectory(strFileName))
			{
				::DeleteFile(strFileName);
			}

			listEventsToDelete.RemoveAll();

			// Save new History xml
			LogEvent(pHistoryRecord);			
		}
	}
}


CString CHistoryManager::GetFileNameForRead(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CString strFileName;
	strFileName.Format(_T("%s\\%s"), pHistoryRecord->GetHistoryPath(), 
		pHistoryRecord->GetFileName());
	
	return strFileName;
}


CString CHistoryManager::GetFileNameForWrite(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CString strFileName = pHistoryRecord->GetFileName();
	CString strHistoryPath = pHistoryRecord->GetHistoryPath();

	IUOLMessengerHistoryRecordPtr pHistoryRecordFound;
	pHistoryRecordFound = FindHistoryRecord(pHistoryRecord);
	if (pHistoryRecordFound)
	{
		CTimeSpan timeSpan = pHistoryRecord->GetTime() - pHistoryRecordFound->GetTime();
		if (timeSpan.GetTotalMinutes() < CONVERSATION_LATENCY)
		{
			strFileName = pHistoryRecordFound->GetFileName();
			strHistoryPath = pHistoryRecordFound->GetHistoryPath(); 
		}
		else
		{
			strFileName = pHistoryRecord->GetFileName();
			strHistoryPath = pHistoryRecord->GetHistoryPath();
			RemoveHistoryRecord(pHistoryRecordFound);
			AddHistoryRecord(pHistoryRecord);
		}
	}
	else
	{
		AddHistoryRecord(pHistoryRecord);
	}
	
	CString strFullFileName;
	strFullFileName.Format(_T("%s\\%s"), strHistoryPath, strFileName);
	
	return strFullFileName;
}


void CHistoryManager::AddHistoryRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	if (!FindHistoryRecord(pHistoryRecord))
	{
		m_listHistoryRecords.AddTail(pHistoryRecord);
	}
}


void CHistoryManager::RemoveHistoryRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	IUOLMessengerHistoryRecordPtr pHistoryRecordFound = FindHistoryRecord(pHistoryRecord);
	if (pHistoryRecordFound)
	{
		POSITION pos = m_listHistoryRecords.Find(pHistoryRecordFound);
		if (pos)
		{
			m_listHistoryRecords.RemoveAt(pos);
		}
	}
}


IUOLMessengerHistoryRecordPtr CHistoryManager::FindHistoryRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	for (POSITION pos = m_listHistoryRecords.GetHeadPosition(); pos;)
	{
		IUOLMessengerHistoryRecordPtr pHistoryRecordFound = m_listHistoryRecords.GetNext(pos);

		CString strAccountUserId = pHistoryRecordFound->GetAccountUserId();
		CString strProtocolId = pHistoryRecordFound->GetProtocolId();
		CString strContactName = pHistoryRecordFound->GetContactName();
		CString strService = pHistoryRecordFound->GetService();

		if ((0 == strAccountUserId.CompareNoCase(pHistoryRecord->GetAccountUserId())) &&
			(0 == strProtocolId.CompareNoCase(pHistoryRecord->GetProtocolId())) &&
			(0 == strContactName.CompareNoCase(pHistoryRecord->GetContactName())) &&
			(0 == strService.CompareNoCase(pHistoryRecord->GetService())))
		{
			return pHistoryRecordFound;
		}
	}

	return NULL;	
}


void CHistoryManager::ConvertOlderFiles(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	// TODO: ConvertOlderFiles is temporally disabled because it is taking so long to convert big files
	return;

	// Get prpl ProtocolId format
	CString strProtocolId;
	strProtocolId.Format(_T("prpl-%s"), pHistoryRecord->GetProtocolId());
	strProtocolId.MakeLower();

	// Get user account
	IUOLMessengerAccountPtr pAccount;
	pAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
		pHistoryRecord->GetAccountUserId(), strProtocolId);

	if (!pAccount)
	{
		return;
	}
	
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strHistoryFolder = pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_BASE_FOLDER_NAME");
	
	// Base profile path
	CString strBasePath;
	IUOLMessengerProfileElementPtr pProfileElement;
	CString strCurrentProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();
	CUIMApplication::GetApplication()->GetProfileManager()->GetProfileElement(strCurrentProfile, pProfileElement);
	strBasePath = pProfileElement->GetProfilePath();
	strBasePath.Append("\\");
	strBasePath.Append(strHistoryFolder);

	// Convert older files
	CString strOlderFileName;
	strOlderFileName.Format(_T("%s\\%s.%s"), strBasePath, pHistoryRecord->GetContactName(), strProtocolId);
	
	if (ATLPath::FileExists(strOlderFileName))
	{
		// Read file data
		FILE *file;
		DWORD dwSize;

		if (!(file = _tfopen(strOlderFileName, _T("rb"))))
		{
			ATLTRACE(_T("%s: Error opening file %s\n"), __FUNCTION__, strOlderFileName);
			return;
		};

		fseek(file, 0, SEEK_END);
		dwSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* pData = (char*) malloc (sizeof(char)*dwSize);

		size_t nReads = fread(pData, 1, dwSize, file);
		if (nReads != dwSize)
		{
			ATLTRACE(_T("%s: Error reading the file %s\n"), __FUNCTION__, strOlderFileName);
			free(pData);
			fclose(file);
			return;
		};
		
		CString strHistoryData = pData;
		fclose(file);
		free(pData);
		pData = NULL;
				
		// Preprocess history data to ensure expected layout
		int nPos = 0;
		while (nPos >= 0)
		{
			nPos = strHistoryData.Find("\n", nPos + 1);

			//0123456789012345678
			//(16/08/07 17:59:37)
			if (strHistoryData.GetLength() >= nPos + 20)
			{
				if (!IsLineStartMatch(strHistoryData.Mid(nPos + 1, 19)))
				{
					if (strHistoryData.Mid(nPos, 1) == "\n")
					{
						strHistoryData.SetAt(nPos, ' ');
					}
				}
			}

		}

		CString strHistoryLines = strHistoryData;

		if (strHistoryLines.IsEmpty())
		{
			return;
		}

		// Tokenize history lines
		CString strResToken;
		nPos = 0;
		strResToken = strHistoryLines.Tokenize("\n", nPos);
		while (strResToken != "")
		{
			//0123456789012345678901234590123456789012345
			//(16/08/07 17:59:37) fulano@gmail.com: teste
			CString strHistory = strResToken;

			// Get date and time
			int nYear = 2000;
			int nMonth = 1;
			int nDay = 1;
			int nHour = 0;
			int nMinute = 0;
			int nSecond = 0;

			if (strHistory.GetLength() > 18)
			{
				try
				{
					nYear = ::_tstoi(strHistory.Mid(7, 2)) + 2000;
					nMonth = ::_tstoi(strHistory.Mid(4, 2));
					nDay = ::_tstoi(strHistory.Mid(1, 2));
					nHour = ::_tstoi(strHistory.Mid(10, 2));
					nMinute = ::_tstoi(strHistory.Mid(13, 2));
					nSecond = ::_tstoi(strHistory.Mid(16, 2));
				}
				catch(char * strError)
				{
					ATLTRACE(_T("%s exception: %s\n"), __FUNCTION__, strError);
				}
				CTime timeEvent(nYear, nMonth, nDay, nHour, nMinute, nSecond);

				// Get sender (EventFrom)
				int nStart = strHistory.Find(')') + 2;
				int nFinish = strHistory.Find(':', nStart);
				int nLen = nFinish - nStart;
				CString strEventFrom = strHistory.Mid(nStart, nLen);
				nLen = strHistory.GetLength() - (nFinish + 2);
				CString strEventText = strHistory.Mid(nFinish + 2, nLen);

				IUOLMessengerConversation::MessageEntryPtr pMessageEntry;
				pMessageEntry = new IUOLMessengerConversation::MessageEntry();
				pMessageEntry->time = timeEvent;

				if ((0 == strEventFrom.CompareNoCase(pAccount->GetNickName())) ||
					(0 == strEventFrom.CompareNoCase(pAccount->GetUserId())))
				{
					pMessageEntry->nMessageSender = IUOLMessengerConversation::MT_ACCOUNT_USER;
				}
				else
				{
					pMessageEntry->nMessageSender = IUOLMessengerConversation::MT_CONTACT;
				}

				pMessageEntry->strText = strEventText;
				pMessageEntry->strAuthor = strEventFrom;

				CHistoryRecordMessagePtr pHistoryRecordMessage;
				pHistoryRecordMessage = new CHistoryRecordMessage(pAccount, 
					pHistoryRecord->GetContactName(), pMessageEntry);
				LogEvent(pHistoryRecordMessage);
			}

			strResToken = strHistoryLines.Tokenize("\n", nPos);
		}		

		::DeleteFile(strOlderFileName);
	}
}


BOOL CHistoryManager::IsLineStartMatch(const CString& strText) 
{
	//0123456789012345678
	//(16/08/07 17:59:37)

	return (('('  == strText.GetAt(0)) &&
			(isdigit(strText.GetAt(1))) &&
			(isdigit(strText.GetAt(2))) &&
			('/'  == strText.GetAt(3)) &&
			(isdigit(strText.GetAt(4))) &&
			(isdigit(strText.GetAt(5))) &&
			('/'  == strText.GetAt(6)) &&
			(isdigit(strText.GetAt(7))) &&
			(isdigit(strText.GetAt(8))) &&
			(' '  == strText.GetAt(9)) &&
			(isdigit(strText.GetAt(10))) &&
			(isdigit(strText.GetAt(11))) &&
			(':'  == strText.GetAt(12)) &&
			(isdigit(strText.GetAt(13))) &&
			(isdigit(strText.GetAt(14))) &&
			(':'  == strText.GetAt(15)) &&
			(isdigit(strText.GetAt(16))) &&
			(isdigit(strText.GetAt(17))) &&
			(')'  == strText.GetAt(18)));
}


//---------------------------------------------------------------------------\\
// DEPRECATED METHODS

// IUOLMessengerHistoryManager1 interface (deprecated)
BOOL CHistoryManager::SetMessageLog(IUOLMessengerContactPtr pContact, CString message, CString author)
{
	// Deprecated method, please use LogEvent() instead.
	ATLASSERT(FALSE);
	
	return FALSE;
}

	
// IUOLMessengerHistoryManager2 interface (deprecated)
BOOL CHistoryManager::ReadLogFilesFromFolder(CAtlMap<int, CString>& mapLog, IUOLMessengerContactPtr pContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
	
	return FALSE;
}


BOOL CHistoryManager::SetConversationWindowClosed(IUOLMessengerContactPtr pContact, CTime startTime)
{
	// Deprecated method.
	ATLASSERT(FALSE);
	
	return FALSE;
}


BOOL CHistoryManager::SetMessageLog(IUOLMessengerContactPtr pContact, const CString& message, const CString& author, CTime startTime)
{
	// Deprecated method, please use LogEvent() instead.
	ATLASSERT(FALSE);
	
	return FALSE;
}


// IUOLMessengerHistoryManager3 interface (deprecated)
BOOL CHistoryManager::ReadLogFilesFromFolder2(CAtlMap<int, CString>& mapLog, IUOLMessengerHistoryContactPtr pContact, const CString& strParameter, const CString& strCallStatus)
{
	// Deprecated method.
	ATLASSERT(FALSE);
	
	return FALSE;
}


BOOL CHistoryManager::SetConversationWindowClosed2(IUOLMessengerHistoryContactPtr pContact, CTime startTime)
{
	// Deprecated method.
	ATLASSERT(FALSE);
	
	return FALSE;
}


BOOL CHistoryManager::SetLog(IUOLMessengerHistoryContactPtr pContact, IUOLMessengerXMLManagerPtr pXMLManager, CTime startTime, const CString& strType)
{
	// Deprecated method, please use LogEvent() instead.
	ATLASSERT(FALSE);
	
	return FALSE;
}


IUOLMessengerHistoryContactPtr CHistoryManager::GetHistoryContact(const CString& strName, const CString& strAlias, const CString& strProtocol, const CString& strAccountName)
{
	// Deprecated method.
	ATLASSERT(FALSE);
	
	return NULL;
}


IUOLMessengerXMLManagerPtr CHistoryManager::GetXMLManager(const CString& strFrom, const CString& strTime,
														  const CString& strStatus, const CString& strContact, const CString& strId, 
														  const CString& strDate, const CString& strProtocol, const CString& strType, const CString& strMessage)
{
	// Deprecated method.
	ATLASSERT(FALSE);

	return NULL;
}