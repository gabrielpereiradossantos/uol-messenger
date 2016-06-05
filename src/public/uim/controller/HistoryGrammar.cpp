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
#include "HistoryGrammar.h"
#include "UIMApplication.h"


using namespace MSXML2;

CHistoryGrammar::CHistoryGrammar(void)
{
	m_pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(m_pStringLoader);
	
	m_pHistoryRecord = new CHistoryRecord();
	
	::CoInitialize(NULL); 	

	HRESULT hr = m_pSAXXMLReader.CreateInstance(__uuidof(SAXXMLReader), NULL, CLSCTX_ALL);
	if ( SUCCEEDED(hr) && NULL != m_pSAXXMLReader )
	{
		m_pSAXXMLReader->putContentHandler(this);	
		m_pSAXXMLReader->putErrorHandler(this);
	}
}


CHistoryGrammar::~CHistoryGrammar(void)
{
	m_pSAXXMLReader = NULL;

	::CoUninitialize();
}


BOOL CHistoryGrammar::Parse(const CString& strFileName, const CTime& timeHistory)
{
	m_pHistoryRecord->SetTime(timeHistory);
	
	if ( NULL == m_pSAXXMLReader )
	{
		return FALSE;
	}
	
	wchar_t URL[1000] = {0};
	::mbstowcs(URL, "file://" + strFileName, 999);	

	HRESULT hResult = S_OK;

	if (ATLPath::FileExists(strFileName) && !ATLPath::IsDirectory(strFileName))
	{
		hResult = m_pSAXXMLReader->parseURL(URL);
	}
	else
	{
		hResult = E_FILE_INVALID;
	}

	return (hResult == S_OK);
}


HRESULT STDMETHODCALLTYPE CHistoryGrammar::startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)

{
	HRESULT hr = S_OK;
	int l = 0;

	IUOLMessengerHistoryRecordPtr pHistoryRecord;

	// Get Element	
	m_strElementName = CString(CW2A(CStringW(pwchLocalName, cchLocalName)));

	// Get Attibutes
	pAttributes->getLength(&l);
	for (int i = 0; i < l; i++)
	{
		wchar_t* ln;
		wchar_t* vl; 
		int lnl;
		int vll;

		pAttributes->getLocalName(i, &ln, &lnl); 
		CStringW strName(ln, lnl);
		CString strParsedAttributeName(strName);

		pAttributes->getValue(i, &vl, &vll);
		CStringW strValue(vl, vll);
		CString strParsedAttributeValue(strValue);

		// Get History Version
		CString strAttribute = m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_VERSION_ATT");
		if (0 == strParsedAttributeName.CompareNoCase(strAttribute))
		{
			m_strVersion = strParsedAttributeValue;
		}

		if (0 == m_strVersion.Compare("1.0"))
		{
			ParseVersion1(strParsedAttributeName, strParsedAttributeValue);
		}
		else
		{
			ParseVersion0(strParsedAttributeName, strParsedAttributeValue);
		}
	}
	
	return hr;
}
       

HRESULT STDMETHODCALLTYPE CHistoryGrammar::endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName)
{
	CString strParsedElement;
	strParsedElement = CString(CW2A(CStringW(pwchLocalName, cchLocalName)));

	// Add history event
	CString strElementName = m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT");
	if (0 == strParsedElement.CompareNoCase(strElementName))
	{
		CTime timeEvent = EncodeTime();
		
		CHistoryRecordEventPtr pHistoryEvent;
		pHistoryEvent = new CHistoryRecordEvent(timeEvent, m_strStatus,
            m_strType, m_strEventFrom, m_strEventTo, m_strEventText);
		m_pHistoryRecord->AddEvent(pHistoryEvent);

		m_strEventText.Empty();
	}	

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CHistoryGrammar::startDocument()
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE CHistoryGrammar::characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars)
{
	CStringW strTextW(pwchChars, cchChars);
	CString strText(strTextW);

	if (0 == m_strVersion.Compare("1.0"))
	{
		// Event Text
		if (0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_TEXT_ELEMENT")))
		{
			m_strEventText.Append(strText);
		}
	}
	else
	{
		// Message (Event Text)
		if (0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_TEXT_ELEMENT")))
		{
			m_strEventText.Append(strText);
		}
		else if (0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_HISTORY_ELEMENT_PROTOCOL_ID_ATT")))
		{
			// ProtocolId
			m_pHistoryRecord->SetProtocolId(strText);
		}
		else if (0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_ELEMENT_TYPE_ATT")))
		{
			// Event Type
			m_strType = strText;
		}
		else if (0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_ELEMENT_DATE_ATT")))
		{
			// Event Date
			m_strDate = strText;
		}
		else if (0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_TO_ID_ATT")))
		{
			// Event To
			m_strEventTo = strText;
		}
	}

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CHistoryGrammar::error( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);
	
	CString strPath(pwchPath);
	CString strErrorMessage(pwchErrorMessage);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogError(__FUNCTION__, 
			"Error processing history file: Path = [%s], Message = [%s], "
			"Line = [%d], Column = [%d] and ErrorCode = [%x].", 
			strPath, strErrorMessage, nLine, nCol, errCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

    return S_OK;
}


HRESULT STDMETHODCALLTYPE CHistoryGrammar::fatalError( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);
	
	CString strPath(pwchPath);
	CString strErrorMessage(pwchErrorMessage);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogError(__FUNCTION__, 
			"Fatal error processing history file: Path = [%s], Message = [%s], "
			"Line = [%d], Column = [%d] and ErrorCode = [%x].", 
			strPath, strErrorMessage, nLine, nCol, errCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return S_OK;
}
    

HRESULT STDMETHODCALLTYPE CHistoryGrammar::ignorableWarning( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);
	
	CString strPath(pwchPath);
	CString strErrorMessage(pwchErrorMessage);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogWarning(__FUNCTION__, 
			"Ignorable warning processing history file: Path = [%s], Message = [%s], "
			"Line = [%d], Column = [%d] and ErrorCode = [%x].", 
			strPath, strErrorMessage, nLine, nCol, errCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return S_OK;
}


void CHistoryGrammar::ParseVersion0(const CString& strAttributeName, const CString& strAttributeValue)
{
	m_pHistoryRecord->SetVersion("0.0");
	if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_ELEMENT_TIME_ATT")))
	{
		// Event Time
		m_strTime = strAttributeValue;
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_ELEMENT_STATUS_ATT")))
	{
		// Event Status
		m_strStatus = strAttributeValue;
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_OLD_XML_EVENT_FROM_ID_ATT")))
	{
		// Event From
		m_strEventFrom = strAttributeValue;
	}
}


void CHistoryGrammar::ParseVersion1(const CString& strAttributeName, const CString& strAttributeValue)
{
	if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_VERSION_ATT")))
	{
		//Version
		m_pHistoryRecord->SetVersion(strAttributeValue);
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_SERVICE_ATT")))
	{
		// Service
		m_pHistoryRecord->SetService(strAttributeValue);
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_USER_ID_ATT")))
	{
		// UserId
		m_pHistoryRecord->SetAccountUserId(strAttributeValue);
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_HISTORY_ELEMENT_PROTOCOL_ID_ATT")))
	{
		// ProtocolId
		m_pHistoryRecord->SetProtocolId(strAttributeValue);
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_DATE_ATT")))
	{
		// Event Date
		m_strDate = strAttributeValue;
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_TIME_ATT")))
	{
		// Event Time
		m_strTime = strAttributeValue;
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_STATUS_ATT")))
	{
		// Event Status
		m_strStatus = strAttributeValue;
	}
	else if (0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_ELEMENT_TYPE_ATT")))
	{
		// Event Type
		m_strType = strAttributeValue;
	}
	else if ((0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_FROM_ID_ATT"))) &&
		(0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_FROM_ELEMENT"))))
	{
		// Event From
		m_strEventFrom = strAttributeValue;
	}
	else if ((0 == strAttributeName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_TO_ID_ATT")))  &&
		(0 == m_strElementName.CompareNoCase(m_pStringLoader->GetValue("uol.resource.IDS_HISTORY_XML_EVENT_TO_ELEMENT"))))
	{
		// Event To
		m_strEventTo = strAttributeValue;
	}
}

CTime CHistoryGrammar::EncodeTime() 
{
	int nYear = 2000;
	int nMonth = 1;
	int nDay = 1;
	int nHour = 0;
	int nMinute = 0;
	int nSecond = 0;

	if ((m_strDate.GetLength() >= 8) && (m_strTime.GetLength() == 8))
	{
		//Index....: 0123456789 
		//m_strDate: 28/04/2008
		//m_strTime: 10:54:03

		try
		{
			if (0 == m_strVersion.Compare("1.0"))
			{
				nYear = ::_tstoi(m_strDate.Mid(6, 4));
			}
			else
			{
				nYear = ::_tstoi(m_strDate.Mid(6, 2));
				// set century
				nYear += 2000;
			}
			nMonth = ::_tstoi(m_strDate.Mid(3, 2));
			nDay = ::_tstoi(m_strDate.Mid(0, 2));
			nHour = ::_tstoi(m_strTime.Mid(0, 2));
			nMinute = ::_tstoi(m_strTime.Mid(3, 2));
			nSecond = ::_tstoi(m_strTime.Mid(6, 2));
		}
		catch(char * strError)
		{
			ATLTRACE(_T("%s exception: %s\n"), __FUNCTION__, strError);
		}
	}

	CTime timeEvent(nYear, nMonth, nDay, nHour, nMinute, nSecond);

	return timeEvent;
}


CHistoryRecordPtr CHistoryGrammar::GetHistoryRecord()
{
	return m_pHistoryRecord;
}
