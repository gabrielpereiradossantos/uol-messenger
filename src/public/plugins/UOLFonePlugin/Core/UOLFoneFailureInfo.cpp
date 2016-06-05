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
 

#pragma once

#include "Stdafx.h"
#include "UOLFoneFailureInfo.h"
#include "../HttpDownloader.h"


#define				NEW_LINE				"\r\n"


CUOLFoneFailureInfo::~CUOLFoneFailureInfo()
{
}


UINT CUOLFoneFailureInfo::GetFailureType()
{
	return m_nFailureType;
}


DWORD CUOLFoneFailureInfo::GetErrorCode()
{
	return m_dwErrorCode;
}


DWORD CUOLFoneFailureInfo::GetAdditionalErrorCode()
{
	return m_dwAdditionalErrorCode;
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::Clone()
{
	CUOLFoneFailureInfo* pFailureInfo = new CUOLFoneFailureInfo(
			this->m_nFailureType, this->m_dwErrorCode, 
			this->m_dwAdditionalErrorCode);
	
	if (pFailureInfo)
	{
		for (POSITION mapPos = m_mapExtraInfo.GetStartPosition(); mapPos; )
		{
			CExtraInfoMap::CPair* pPair = m_mapExtraInfo.GetNext(mapPos);
			
			if ( (pPair) && (pPair->m_value->iExtraInfoType == EXTRA_INFO_TYPE_STR) )
			{
				pFailureInfo->AddExtraFailureInfo(
						pPair->m_key, pPair->m_value->strExtraInfo);
			}
			else if ( (pPair) && (pPair->m_value->iExtraInfoType == EXTRA_INFO_TYPE_DWORD) )
			{
				pFailureInfo->AddExtraFailureInfo(
						pPair->m_key, pPair->m_value->dwExtraInfo);
			}
		}
	}
	
	return pFailureInfo;
}


void CUOLFoneFailureInfo::AddExtraFailureInfo(
		ExtraFailureInfoId extraInfoId, CString strExtraInfo)
{
	ExtraInfoPtr pExtraInfo = AllocExtraInfo();
	
	if (pExtraInfo)
	{
		pExtraInfo->iExtraInfoType = EXTRA_INFO_TYPE_STR;
		pExtraInfo->strExtraInfo = strExtraInfo;
		
		m_mapExtraInfo.SetAt(extraInfoId, pExtraInfo);
	}
}


void CUOLFoneFailureInfo::AddExtraFailureInfo(
		ExtraFailureInfoId extraInfoId, DWORD dwExtraInfo)
{
	ExtraInfoPtr pExtraInfo = AllocExtraInfo();
	
	if (pExtraInfo)
	{
		pExtraInfo->iExtraInfoType = EXTRA_INFO_TYPE_DWORD;
		pExtraInfo->dwExtraInfo = dwExtraInfo;
		
		m_mapExtraInfo.SetAt(extraInfoId, pExtraInfo);
	}
}


CString CUOLFoneFailureInfo::GetExtraFailureInfoStr(ExtraFailureInfoId extraInfoId)
{
	CExtraInfoMap::CPair* pPair = m_mapExtraInfo.Lookup(extraInfoId);
	
	if (pPair)
	{
		return pPair->m_value->strExtraInfo;
	}
	
	return CString();
}


DWORD CUOLFoneFailureInfo::GetExtraFailureInfoDWORD(ExtraFailureInfoId extraInfoId)
{
	CExtraInfoMap::CPair* pPair = m_mapExtraInfo.Lookup(extraInfoId);
	
	if (pPair)
	{
		return pPair->m_value->dwExtraInfo;
	}
	
	return DEFAULT_ERROR_CODE;
}


CString CUOLFoneFailureInfo::GetFailureInfoText()
{
	CString strFailureInfoText;
	CString strFailureCommonText;
	
	CString strErrorFile;
	DWORD dwHTTPStatusCode;
	CString strHTTPStatusMessage;
	CString strErrorPath;
	CString strMethodName;
	CString strParameterName;
	
	CString strCallUrl;
	CString strCallDisplayUrl;
	
	CString strReasonPhrase;
	CString strTextInfo;
	CString strStatusCode;
	CString strErrorSource;
	
	if (m_nFailureType != FAILURE_TYPE_INITIALIZATION)
	{
		strFailureCommonText.Format("ErrorCode = [%d], AdditionalErrorCode = [%d]", 
				m_dwErrorCode, m_dwAdditionalErrorCode);
	}
	else
	{
		strFailureCommonText.Format("ErrorCode = [%d]", m_dwErrorCode);
	}
	
	switch (m_nFailureType)
	{
		case FAILURE_TYPE_DOWNLOAD:
			
			strErrorFile = GetExtraFailureInfoStr(EXTRA_INFO_PATH);
			dwHTTPStatusCode = 
					GetExtraFailureInfoDWORD(EXTRA_INFO_HTTP_STATUS_CODE);
			strHTTPStatusMessage = 
					GetExtraFailureInfoStr(EXTRA_INFO_HTTP_STATUS_MESSAGE);
			
			if (m_dwErrorCode == CHttpDownloader::DE_HTTP_STATUS_CODE_ERROR)
			{
				strFailureInfoText = "[[[Download Failure]]] -> HTTP Error: ";
			}
			else if ( (m_dwErrorCode == CHttpDownloader::DE_FILE_CREATION_ERROR) || 
					(m_dwErrorCode == CHttpDownloader::DE_FILE_WRITING_ERROR) )
			{
				strFailureInfoText = "[[[Download Failure]]] -> FileSystem Error: ";
			}
			else
			{
				strFailureInfoText = "[[[Download Failure]]] -> General Error: ";
			}
			
			strFailureInfoText += strFailureCommonText;
			
			strFailureInfoText += ", Failed URL = [" + 
					GetExtraFailureInfoStr(EXTRA_INFO_URL) + "]";
			
			if (dwHTTPStatusCode != DEFAULT_ERROR_CODE)
			{
				CString strHTTPStatusCode;
				strHTTPStatusCode.Format("%d", dwHTTPStatusCode);
				
				strFailureInfoText += ", HTTP Status Code = [" + 
						strHTTPStatusCode + "]";
			}
			
			if (!strHTTPStatusMessage.IsEmpty())
			{
				strFailureInfoText += ", HTTP Status Message = [" + 
						strHTTPStatusMessage + "]";
			}
			
			if (!strErrorFile.IsEmpty())
			{
				strFailureInfoText += ", Failed File = [" + 
						strErrorFile + "]";
			}
			
			strFailureInfoText += ".";
			
		break;
		
		case FAILURE_TYPE_SYSTEM:
			
			strErrorPath = GetExtraFailureInfoStr(EXTRA_INFO_PATH);
			
			if (m_dwErrorCode == SYSTEM_PATH_CREATION_FAILURE)
			{
				strFailureInfoText = "[[[System Failure]]] -> Directory Creation Error: ";
			}
			else
			{
				strFailureInfoText = "[[[System Failure]]] -> General Error: ";
			}
			
			strFailureInfoText += strFailureCommonText;
			
			if (!strErrorPath.IsEmpty())
			{
				strFailureInfoText += ", Failed Path = [" + 
						strErrorPath + "]";
			}
			
			strFailureInfoText += ".";
			
		break;
		
		case FAILURE_TYPE_INITIALIZATION:
			
			strMethodName = GetExtraFailureInfoStr(EXTRA_INFO_FUNCTION_NAME);
			strParameterName = GetExtraFailureInfoStr(EXTRA_INFO_PARAMETER_NAME);
			
			if (m_dwErrorCode == INITIALIZATION_INVALID_PARAM_FAILURE)
			{
				strFailureInfoText = "[[[Initialization Failure]]] -> Invalid Parameter Error: ";
			}
			else
			{
				strFailureInfoText = "[[[Initialization Failure]]] -> General Error: ";
			}
			
			strFailureInfoText += strFailureCommonText;
			
			if (!strMethodName.IsEmpty())
			{
				strFailureInfoText += ", Method Name = [" + 
						strMethodName + "]";
			}
			
			if (!strParameterName.IsEmpty())
			{
				strFailureInfoText += ", Parameter Name = [" + 
						strParameterName + "]";
			}
			
			strFailureInfoText += ".";
			
		break;
		
		case FAILURE_TYPE_LIBRARY:
			
			strErrorFile = GetExtraFailureInfoStr(EXTRA_INFO_PATH);
			
			if ((m_dwErrorCode == LIBRARY_LOAD_UOLFONECLIENT_FAILURE) || 
				(m_dwErrorCode == LIBRARY_LOAD_UOLFONESERVERCONFIG_FAILURE))
			{
				strFailureInfoText = "[[[Library Failure]]] -> Loading Error: ";
			}
			else if ((m_dwErrorCode >= LIBRARY_COINITIALIZE_FAILURE) && 
					(m_dwErrorCode <= LIBRARY_INITIALIZE_PHONE_LINES_FAILURE))
			{
				strFailureInfoText = "[[[Library Failure]]] -> Instance/Initializing Error: ";
			}
			else if ((m_dwErrorCode >= LIBRARY_RUN_DOWNLOAD_PHONE_RULES_FAILURE) &&
					(m_dwErrorCode <= LIBRARY_CODECS_UPDATER_GET_ADDITIONAL_ERROR_FAILURE))
			{
				strFailureInfoText = "[[[Library Failure]]] -> Complements Update Error: ";
			}
			else if ((m_dwErrorCode >= LIBRARY_SET_OUTPUT_AUDIO_DEVICE_FAILURE) && 
					(m_dwErrorCode <= LIBRARY_MEDIA_NETWORK_CONFIG_RESET_HTTP_PROXY_FAILURE))
			{
				strFailureInfoText = "[[[Library Failure]]] -> Execution/Setup Error: ";
			}
			else if ((m_dwErrorCode >= LIBRARY_TEST_CONNECTION_GET_SERVER_INFO_FAILURE) && 
					(m_dwErrorCode <= LIBRARY_TEST_CONNECTION_NO_CONNECTION_FAILURE))
			{
				strFailureInfoText = "[[[Library Failure]]] -> Connection Testing Error: ";
			}
			else
			{
				strFailureInfoText = "[[[Library Failure]]] -> General Error: ";
			}
			
			strFailureInfoText += strFailureCommonText;
			
			if (!strErrorFile.IsEmpty())
			{
				strFailureInfoText += ", Failed File = [" + 
						strErrorFile + "]";
			}
			
			strFailureInfoText += ".";
			
		break;
		
		case FAILURE_TYPE_CALL:
			
			strCallUrl = GetExtraFailureInfoStr(EXTRA_INFO_URL);
			strCallDisplayUrl = GetExtraFailureInfoStr(EXTRA_INFO_ERROR_DISPLAY_URL);
			
			strReasonPhrase = GetExtraFailureInfoStr(EXTRA_INFO_REASON_PHRASE);
			strTextInfo = GetExtraFailureInfoStr(EXTRA_INFO_TEXT_INFO);
			strStatusCode = GetExtraFailureInfoStr(EXTRA_INFO_STATUS_CODE);
			strErrorSource = GetExtraFailureInfoStr(EXTRA_INFO_ERROR_SOURCE);
			
			if (m_dwErrorCode == CALL_ERROR_RECEIVED_FAILURE)
			{
				strFailureInfoText = "[[[Call Failure]]] -> Running Call Error: ";
			}
			else if (m_dwErrorCode == CALL_SERVER_ERROR_RECEIVED_FAILURE)
			{
				strFailureInfoText = "[[[Call Failure]]] -> Server Error: ";
			}
			else if (m_dwErrorCode == CALL_ERROR_UNDEFINED_FAILURE)
			{
				strFailureInfoText = "[[[Call Failure]]] -> Unidentified Error: ";
			}
			else
			{
				strFailureInfoText = "[[[Call Failure]]] -> Unknown Error Without Details.";
			}
			
			strFailureInfoText += strFailureCommonText;
			
			if (!strCallUrl.IsEmpty())
			{
				strFailureInfoText += ", Call URL = [" + 
						strCallUrl + "]";
			}
			
			if (!strCallDisplayUrl.IsEmpty())
			{
				strFailureInfoText += ", Call Display URL = [" + 
						strCallDisplayUrl + "]";
			}
			
			if (!strReasonPhrase.IsEmpty())
			{
				strFailureInfoText += ", Reason Phrase = [" + 
						strReasonPhrase + "]";
			}
			
			if (!strTextInfo.IsEmpty())
			{
				strFailureInfoText += ", Text Info = [" + 
						strTextInfo + "]";
			}
			
			if (!strStatusCode.IsEmpty())
			{
				strFailureInfoText += ", Status Code = [" + 
						strStatusCode + "]";
			}
			
			if (!strErrorSource.IsEmpty())
			{
				strFailureInfoText += ", Error Source = [" + 
						strErrorSource + "]";
			}
			
			strFailureInfoText += ".";
			
		break;
	}
	
	return strFailureInfoText;
}


BOOL CUOLFoneFailureInfo::IsFailureImmutable()
{
	BOOL bIsFailureImmutable = FALSE;
	
	switch (m_nFailureType)
	{
		case FAILURE_TYPE_DOWNLOAD:
			
			if ( (m_dwErrorCode == CHttpDownloader::DE_HTTP_STATUS_CODE_ERROR) && 
					(GetExtraFailureInfoStr(EXTRA_INFO_HTTP_STATUS_CODE) == 407) )
			{
				bIsFailureImmutable = TRUE;
			}
			else if ( (m_dwErrorCode == CHttpDownloader::DE_FILE_CREATION_ERROR) || 
					(m_dwErrorCode == CHttpDownloader::DE_FILE_WRITING_ERROR) )
			{
				bIsFailureImmutable = TRUE;
			}
			
		break;
		
		case FAILURE_TYPE_SYSTEM:
			
			bIsFailureImmutable = TRUE;
			
		break;
		
		case FAILURE_TYPE_INITIALIZATION:
			
			bIsFailureImmutable = TRUE;
			
		break;
		
		case FAILURE_TYPE_LIBRARY:
			
			// For now, we won't consider any of failures as immutable.
			//
			
		break;
		
		case FAILURE_TYPE_CALL:
			
			// For now, we won't consider any of failures as immutable.
			//
			
		break;
	}
	
	return bIsFailureImmutable;
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildDownloadFailure(
		DWORD dwErrorCode, DWORD dwAdditionalErrorCode, CString strDownloadUrl)
{
	CUOLFoneFailureInfo* pFailureInfo = new CUOLFoneFailureInfo(
			FAILURE_TYPE_DOWNLOAD, dwErrorCode, dwAdditionalErrorCode);
	
	if (pFailureInfo)
	{
		pFailureInfo->AddExtraFailureInfo(EXTRA_INFO_URL, strDownloadUrl);
	}
	
	return pFailureInfo;
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildDownloadFailure(
		DWORD dwErrorCode, DWORD dwAdditionalErrorCode, CString strDownloadUrl, 
		DWORD dwHTTPStatusCode, CString strHTTPStatusMessage)
{
	CUOLFoneFailureInfo* pFailureInfo = 
			BuildDownloadFailure(dwErrorCode, dwAdditionalErrorCode, strDownloadUrl);
	
	if (pFailureInfo)
	{
		pFailureInfo->AddExtraFailureInfo(
				EXTRA_INFO_HTTP_STATUS_CODE, dwHTTPStatusCode);
		
		pFailureInfo->AddExtraFailureInfo(
				EXTRA_INFO_HTTP_STATUS_MESSAGE, strHTTPStatusMessage);
	}
	
	return pFailureInfo;
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildSystemFailure(
		DWORD dwErrorCode, DWORD dwAdditionalErrorCode)
{
	return new CUOLFoneFailureInfo(FAILURE_TYPE_SYSTEM, 
			dwErrorCode, dwAdditionalErrorCode);
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildSystemFailure(
		DWORD dwErrorCode, DWORD dwAdditionalErrorCode, CString strFailedPath)
{
	CUOLFoneFailureInfo* pFailureInfo = 
			BuildSystemFailure(dwErrorCode, dwAdditionalErrorCode);
	
	if (pFailureInfo)
	{
		pFailureInfo->AddExtraFailureInfo(
				EXTRA_INFO_PATH, strFailedPath);
	}
	
	return pFailureInfo;
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildInitializationFailure(
		DWORD dwErrorCode)
{
	return new CUOLFoneFailureInfo(FAILURE_TYPE_INITIALIZATION, 
			dwErrorCode, DEFAULT_ERROR_CODE);
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildInitializationFailure(
		DWORD dwErrorCode, CString strFunction, CString strInvalidParameter)
{
	CUOLFoneFailureInfo* pFailureInfo = BuildInitializationFailure(dwErrorCode);
	
	if (pFailureInfo)
	{
		pFailureInfo->AddExtraFailureInfo(
				EXTRA_INFO_FUNCTION_NAME, strFunction);
		
		pFailureInfo->AddExtraFailureInfo(
				EXTRA_INFO_PARAMETER_NAME, strInvalidParameter);
	}
	
	return pFailureInfo;
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildLibraryFailure(
		DWORD dwErrorCode, DWORD dwAdditionalErrorCode)
{
	return new CUOLFoneFailureInfo(FAILURE_TYPE_LIBRARY, 
			dwErrorCode, dwAdditionalErrorCode);
}


CUOLFoneFailureInfo* CUOLFoneFailureInfo::BuildCallFailure(
		DWORD dwErrorCode, DWORD dwAdditionalErrorCode)
{
	return new CUOLFoneFailureInfo(FAILURE_TYPE_CALL, 
			dwErrorCode, dwAdditionalErrorCode);
}


CUOLFoneFailureInfo::CUOLFoneFailureInfo(UINT nFailureType, 
			DWORD dwErrorCode, DWORD dwAdditionalErrorCode) : 
		m_nFailureType(nFailureType), 
		m_dwErrorCode(dwErrorCode), 
		m_dwAdditionalErrorCode(dwAdditionalErrorCode)
{
}


CUOLFoneFailureInfo::ExtraInfoPtr CUOLFoneFailureInfo::AllocExtraInfo()
{
	ExtraInfoPtr pExtraInfo = new ExtraInfo;
	
	if (pExtraInfo)
	{
		pExtraInfo->dwExtraInfo = 0;
		pExtraInfo->iExtraInfoType = 0;
	}
	
	return pExtraInfo;
}

