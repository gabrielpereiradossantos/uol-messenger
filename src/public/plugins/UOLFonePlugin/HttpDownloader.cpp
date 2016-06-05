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
#include "HttpDownloader.h"

//#include "utils.h"
#include <util/InternetHandle.h>


CHttpDownloader::CHttpDownloader() : 
	m_pCallback(NULL),
	m_pStopFlag(NULL),
	m_bDisableCache(TRUE),
	m_dwAdditionalError(-1),	// Must be -1.
	m_dwHTTPStatusCode(-1)		// Must be -1.
{
}


void CHttpDownloader::SetUrl(const CString& strUrl)
{
	m_strUrl = strUrl;
}


void CHttpDownloader::SetLocalFilePath(const CString& strLocalFilePath)
{
	m_strLocalFilePath = strLocalFilePath;
}


void CHttpDownloader::SetCallback(CHttpDownloaderCallback* pCallback)
{
	m_pCallback = pCallback;
}


void CHttpDownloader::SetStopFlag(CHttpDownloaderStopFlag* pStopFlag)
{
	m_pStopFlag = pStopFlag;
}


CString CHttpDownloader::GetUrl()
{
	return m_strUrl;
}


CString CHttpDownloader::GetLocalFilePath()
{
	return m_strLocalFilePath;
}


void CHttpDownloader::DisableCache(BOOL bDisable)
{
	m_bDisableCache = bDisable;
}


BOOL CHttpDownloader::IsCacheDisabled()
{
	return m_bDisableCache;
}


DWORD CHttpDownloader::DownloadFile()
{
	DWORD dwError = CHttpDownloader::DE_INVALID_PARAM;
	
	if ((FALSE == m_strUrl.IsEmpty()) &&
		(FALSE == m_strLocalFilePath.IsEmpty()))
	{
		dwError = DownloadFileImpl(m_strUrl, m_strLocalFilePath, m_pCallback, m_pStopFlag);
	}

	return dwError;
}


DWORD CHttpDownloader::GetAdditionalErrorCode()
{
	return m_dwAdditionalError;
}


DWORD CHttpDownloader::GetHTTPStatusErrorCode()
{
	return m_dwHTTPStatusCode;
}


CString CHttpDownloader::GetHTTPStatusErrorMessage()
{
	return m_strHTTPStatusMessage;
}


DWORD CHttpDownloader::DownloadFileImpl(const CString& strUrl, const CString& strLocalFilePath, CHttpDownloaderCallback* pCallback, CHttpDownloaderStopFlag* pStopFlag)
{
	DWORD dwError = CHttpDownloader::DE_ERROR_FAILED;
	
	m_dwAdditionalError = -1;	// Must be -1.
	m_dwHTTPStatusCode = -1;	// Must be -1.
	m_strHTTPStatusMessage.Empty();

	CInternetHandle hInet = ::InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInet)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_INTERNET_OPEN_ERROR;
		return dwError;
	}

	// Set receive timeout...
	ULONG ulTimeOut = 20000;
	DWORD dwBufferSize = sizeof(ulTimeOut);

	BOOL bRet = ::InternetSetOption(hInet, INTERNET_OPTION_RECEIVE_TIMEOUT, &ulTimeOut, dwBufferSize);
	ATLASSERT(bRet);

	if (FALSE == bRet)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_INTERNET_SET_OPTION_ERROR;
		return dwError;
	}

	// Set send timeout...
	ulTimeOut = 20000;
	dwBufferSize = sizeof(ulTimeOut);

	bRet = ::InternetSetOption(hInet, INTERNET_OPTION_SEND_TIMEOUT, &ulTimeOut, dwBufferSize);
	ATLASSERT(bRet);

	if (FALSE == bRet)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_INTERNET_SET_OPTION_ERROR;
		return dwError;
	}

	// Set connect timeout...
	ulTimeOut = 60000;
	dwBufferSize = sizeof(ulTimeOut);

	bRet = ::InternetSetOption(hInet, INTERNET_OPTION_CONNECT_TIMEOUT, &ulTimeOut, dwBufferSize);
	ATLASSERT(bRet);

	if (FALSE == bRet)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_INTERNET_SET_OPTION_ERROR;
		return dwError;
	}

	CUrl url;
	if (FALSE == url.CrackUrl(strUrl))
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_CRACK_URL_ERROR;
		return dwError;
	}

	// Send request...
	DWORD_PTR pContext = NULL;
	CInternetHandle hConnect = ::InternetConnect(hInet, 
		url.GetHostName(), 
		url.GetPortNumber(), 
		NULL, 
		NULL, 
		INTERNET_SERVICE_HTTP, 
		0, 
		pContext);
	
	if (!hConnect)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_INTERNET_CONNECT_ERROR;
		return dwError;
	}

	const TCHAR* pszAccept[] = {"*/*", NULL};

	CString strObject;
	
	strObject = url.GetUrlPath();
	strObject += url.GetExtraInfo();

	DWORD dwOpenRequestFlags = INTERNET_FLAG_NO_AUTO_REDIRECT;

	if (IsCacheDisabled())
	{
		dwOpenRequestFlags |= (INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE);
	}

	CInternetHandle hRequest = ::HttpOpenRequest(hConnect, 
		"GET",
		strObject, 
		NULL, 
		NULL, 
		pszAccept, 
		dwOpenRequestFlags, 
		pContext);
	if (!hRequest)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_HTTP_OPEN_REQUEST_ERROR;
		return dwError;
	}

	// Check stop flag
	if ((pStopFlag != NULL) &&
		(pStopFlag->GetStopFlag()))
	{
		return CHttpDownloader::DE_STOPPED;
	}

	NotifyDownloadBegin(pCallback);

	if (FALSE == ::HttpSendRequest(hRequest, NULL, 0, NULL, 0))
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_HTTP_SEND_REQUEST_ERROR;
		NotifyDownloadComplete(pCallback, dwError);
		return dwError;
	}

	// Check status code...
	TCHAR szStatusCode[20] = {0};
	DWORD dwStatusCodeSize = sizeof(szStatusCode) / sizeof(TCHAR);

	if (FALSE == ::HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwStatusCodeSize, NULL))
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_HTTP_QUERY_STATUS_ERROR;
		NotifyDownloadComplete(pCallback, dwError);
		return dwError;
	}

	DWORD dwStatusCode = (DWORD) _ttol(szStatusCode);
	if (dwStatusCode != 200)
	{
		dwError = CHttpDownloader::DE_HTTP_STATUS_CODE_ERROR;
		m_dwHTTPStatusCode = dwStatusCode;
		
		// Check status text...
		TCHAR szStatusMessage[512] = {0};
		DWORD dwStatusMessageSize = sizeof(szStatusMessage) / sizeof(TCHAR);
		if (TRUE == ::HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_TEXT, szStatusMessage, &dwStatusMessageSize, NULL))
		{
			m_strHTTPStatusMessage = szStatusMessage;
		}
		
		NotifyDownloadComplete(pCallback, dwError);
		return dwError;
	}

	TCHAR szContentLength[20] = {0};
	DWORD dwContentLengthSize = sizeof(szContentLength) / sizeof(TCHAR);

	if (FALSE == ::HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwContentLengthSize, NULL))
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_HTTP_QUERY_CONTENT_LENGTH_ERROR;
		NotifyDownloadComplete(pCallback, dwError);
		return dwError;
	}

	DWORD dwContentLength = (DWORD) _ttol(szContentLength);

	CAtlFile file;

	HRESULT hr = file.Create(strLocalFilePath, GENERIC_WRITE, 0, CREATE_ALWAYS);

	if (FAILED(hr))
	{
		m_dwAdditionalError = hr;
		dwError = CHttpDownloader::DE_FILE_CREATION_ERROR;
		NotifyDownloadComplete(pCallback, dwError);
		return dwError;
	}

	// Read data and write to output file...
	DWORD dwRead;
	DWORD dwTotalWrittenBytes = 0;
	DWORD dwBytesInBuffer = 0;
	TCHAR* pBuffer = NULL;
	BOOL bDataAvailable;

	bDataAvailable = ::InternetQueryDataAvailable(hRequest, &dwBytesInBuffer, NULL, NULL);

	if (FALSE == bDataAvailable)
	{
		m_dwAdditionalError = GetLastError();
		dwError = CHttpDownloader::DE_INTERNET_QUERY_DATA_AVAILABLE_ERROR;
		return dwError;
	}

	while (bDataAvailable && (dwBytesInBuffer > 0))
	{
		// Check stop flag
		if ((pStopFlag != NULL) &&
			(pStopFlag->GetStopFlag()))
		{
			return CHttpDownloader::DE_STOPPED;
		}

		pBuffer = new TCHAR[dwBytesInBuffer + 1];
		::ZeroMemory((PVOID) pBuffer, (SIZE_T) dwBytesInBuffer + 1);

		if (::InternetReadFile(hRequest, pBuffer, dwBytesInBuffer, &dwRead))
		{
			DWORD dwBytesWritten;

			hr = file.Write(pBuffer, dwBytesInBuffer, &dwBytesWritten);

			if (SUCCEEDED(hr))
			{
				dwTotalWrittenBytes += dwBytesWritten;

				NotifyProgressChange(pCallback, dwTotalWrittenBytes, dwContentLength);
				
				dwBytesInBuffer = 0;
				bDataAvailable = ::InternetQueryDataAvailable(hRequest, &dwBytesInBuffer, 0, 0);
			}
			else
			{
				m_dwAdditionalError = hr;
				dwError = CHttpDownloader::DE_FILE_WRITING_ERROR;
				bDataAvailable = FALSE;
			}
		}
		else
		{
			m_dwAdditionalError = GetLastError();
			dwError = CHttpDownloader::DE_INTERNET_READ_ERROR;
			bDataAvailable = FALSE;
		}

		if (pBuffer)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}

	file.Close();

	// Is result complete?
	if ((bDataAvailable) && (dwContentLength == dwTotalWrittenBytes))
	{
		dwError = CHttpDownloader::DE_ERROR_SUCCESS;
	}
	else
	{
		BOOL bDeleted = ::DeleteFile(strLocalFilePath);
		ATLASSERT(bDeleted);
	}

	NotifyDownloadComplete(pCallback, dwError);

	return dwError;
}


void CHttpDownloader::NotifyDownloadBegin(CHttpDownloaderCallback* pCallback)
{
	if (pCallback != NULL)
	{
		pCallback->OnDownloadBegin();
	}
}


void CHttpDownloader::NotifyProgressChange(CHttpDownloaderCallback* pCallback, ULONG nProgress, ULONG nProgressMax)
{
	if (pCallback != NULL)
	{
		pCallback->OnProgressChange(nProgress, nProgressMax);
	}
}


void CHttpDownloader::NotifyDownloadComplete(CHttpDownloaderCallback* pCallback, DWORD dwError)
{
	if (pCallback != NULL)
	{
		pCallback->OnDownloadComplete(dwError);
	}
}