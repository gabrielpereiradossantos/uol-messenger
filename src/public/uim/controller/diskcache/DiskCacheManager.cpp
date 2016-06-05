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
#include "DiskCacheManager.h"

#include <sys/stat.h>

#include "../UIMApplication.h"
#include "../../FileUtils.h"
#include "../../model/SettingsTraits.h"



static const CString FILE_NAME_PREFIX = _T("uim");
static const CString CACHE_INDEX = _T("index.dat");
static const CString INDEX_ROOT_NODE = _T("Entries");
static const CString ENTRY_NODE = _T("Entry");
static const CString ENTRY_NODE_URL = _T("URL");
static const CString ENTRY_NODE_FILE = _T("File");


CDiskCacheManager::CDiskCacheManager()
{
}


CDiskCacheManager::~CDiskCacheManager()
{
}


BOOL CDiskCacheManager::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bRet;

	m_strCacheDir = GetCacheDir();

	bRet = CFileUtils::CreateDirectory(m_strCacheDir);

	if (bRet)
	{
		bRet = m_downloadScheduler.Init();

		if (bRet)
		{
			m_downloadScheduler.RegisterObserver(this);

			LoadCacheEntries();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"m_downloadScheduler.Init() failed.");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"CreateDirectory() failed. m_strCacheDir = [%s]", m_strCacheDir);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CDiskCacheManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_downloadScheduler.UnregisterObserver(this);
	m_downloadScheduler.Finalize();

	SaveCacheEntries();

	ClearOrphanFiles();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IDiskCacheEntryPtr CDiskCacheManager::FetchUrl(const CString& strUrl, DWORD& dwError)
{
	IDiskCacheEntryPtr pDiskCacheEntry;

	if (FALSE == strUrl.IsEmpty())
	{
		CString strLocalPath = GetLocalPath();

		dwError = m_downloadScheduler.DownloadNow(strUrl, strLocalPath);

		if (dwError == DCME_ERROR_SUCCESS)
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

			pDiskCacheEntry = AddCacheEntry(strUrl, strLocalPath);
		}
	}
	else
	{
		dwError = IDiskCacheManager::DCME_INVALID_PARAM;
	}

	return pDiskCacheEntry;
}


DWORD CDiskCacheManager::FetchUrl(const CString& strUrl, IDiskCacheEntryObserver* pObserver)
{
	if ((strUrl.IsEmpty()) ||
		(!pObserver))
	{
		return IDiskCacheManager::DCME_INVALID_PARAM;
	}

	CString strLocalPath = GetLocalPath();

	DWORD dwSchedulerError = m_downloadScheduler.ScheduleDownload(strUrl, strLocalPath);

	DWORD dwRetError = GetErrorCode(dwSchedulerError);

	if (dwRetError == IDiskCacheManager::DCME_ERROR_SUCCESS)
	{
		m_mapEntryObservers.SetAt(strUrl, pObserver);
	}

	return dwRetError;
}


IDiskCacheEntryPtr CDiskCacheManager::GetCacheEntry(const CString& strURL)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

	CDiskCacheEntryMap::CPair* pPair = m_mapEntries.Lookup(strURL);

	if (pPair != NULL)
	{
		return pPair->m_value;
	}

	return IDiskCacheEntryPtr();
}


void CDiskCacheManager::Clear()
{
	CFileUtils::DeleteDirectoryFiles(GetCacheDir(), _T("*.tmp"));
}


void CDiskCacheManager::OnDownloadComplete(IDownloadInfo* pDownloadInfo)
{
	CString strUrl = pDownloadInfo->GetUrl();

	CDiskCacheEntryObserverMap::CPair* pPair = m_mapEntryObservers.Lookup(strUrl);

	if (pPair)
	{
		DWORD dwError = GetErrorCode(pDownloadInfo->GetStatus());

		if (dwError == IDiskCacheManager::DCME_ERROR_SUCCESS)
		{
			CDiskCacheEntryPtr pEntry = AddCacheEntry(strUrl, pDownloadInfo->GetLocalPath());

			if (pEntry)
			{
				(pPair->m_value)->OnItemFetched(pEntry);
			}
		}
		else
		{

			(pPair->m_value)->OnItemFetchError(strUrl, dwError);
		}

		m_mapEntryObservers.RemoveKey(strUrl);
	}
}


void CDiskCacheManager::LoadCacheEntries()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr))
	{
		MSXML::IXMLDOMDocumentPtr pXMLDoc;

		hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument));

		if (SUCCEEDED(hr))
		{
			pXMLDoc->Putasync(VARIANT_FALSE);
			pXMLDoc->PutpreserveWhiteSpace(VARIANT_TRUE);

			CString strFilePath = m_strCacheDir + CACHE_INDEX;

			VARIANT_BOOL bResult = pXMLDoc->load(CComVariant(strFilePath)); 

			if (bResult == VARIANT_TRUE)
			{
				MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(INDEX_ROOT_NODE.AllocSysString());

				if (pNode)
				{
					if (pNode->hasChildNodes())
					{
						MSXML::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();

						if (NULL != pNodeList)
						{
							long lSize = pNodeList->length;

							MSXML::IXMLDOMNodePtr pChild;

							for (long lInd = 0; lInd < lSize; lInd++)
							{
								pChild = pNodeList->Getitem(lInd);

								CComBSTR bstrNodeName;
								pChild->get_nodeName(&bstrNodeName);

								CString strNodeName = bstrNodeName;

								if (0 == strNodeName.CompareNoCase(ENTRY_NODE))
								{
									CDiskCacheEntryPtr pDiskCacheEntry = LoadCacheEntry(pChild);
									if (pDiskCacheEntry)
									{
										m_mapEntries.SetAt(pDiskCacheEntry->GetUrl(), pDiskCacheEntry);
									}
								}
							}

							pNodeList.Release();
						}
					}
				}
			}

			pXMLDoc.Release();
		}

		::CoUninitialize();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hr = [%x]", hr);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CDiskCacheEntryPtr CDiskCacheManager::LoadCacheEntry(MSXML::IXMLDOMNodePtr pNode)
{
	CDiskCacheEntryPtr pCacheEntry;

	if (pNode->hasChildNodes())
	{
		MSXML::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
		if (NULL != pNodeList)
		{
			long lSize = pNodeList->length;

			MSXML::IXMLDOMNodePtr pChild;

			CString strUrl;
			CString strLocalPath;

			for (long lInd = 0; lInd < lSize; lInd++)
			{
				pChild = pNodeList->Getitem(lInd);

				CComBSTR bstrNodeName;
				pChild->get_nodeName(&bstrNodeName);

				CString strNodeName = bstrNodeName;

				if (0 == strNodeName.CompareNoCase(ENTRY_NODE_URL))
				{
					CSettingsTraits::GetStringValue(pChild, ENTRY_NODE_URL, strUrl);
				}
				else if (0 == strNodeName.CompareNoCase(ENTRY_NODE_FILE))
				{
					CSettingsTraits::GetStringValue(pChild, ENTRY_NODE_FILE, strLocalPath);
				}
			}

			if ((FALSE == strUrl.IsEmpty()) &&
				(FALSE == strLocalPath.IsEmpty()))
			{
				strLocalPath = m_strCacheDir + strLocalPath;

				struct _stat st = {0};

				if (0 == ::_tstat(strLocalPath, &st))
				{
					pCacheEntry = new CDiskCacheEntry();
					pCacheEntry->SetUrl(strUrl);
					pCacheEntry->SetLocalPath(strLocalPath);
				}
				else
				{
					ATLTRACE(_T("%s - file not found: %s\n"), __FUNCTION__, strLocalPath);
				}
			}
		}

		pNodeList.Release();
	}

	return pCacheEntry;
}


void CDiskCacheManager::SaveCacheEntries()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr))
	{
		MSXML::IXMLDOMDocumentPtr pXMLDoc;

		hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument));

		if (SUCCEEDED(hr))
		{
			MSXML::IXMLDOMNodePtr pRootNode;

			pRootNode = pXMLDoc->createElement(INDEX_ROOT_NODE.AllocSysString());

			if (NULL != pRootNode)
			{
				CDiskCacheEntryMap::CPair* pPair;

				for (POSITION pos = m_mapEntries.GetStartPosition(); pos; )
				{
					pPair = m_mapEntries.GetNext(pos);
					if (pPair)
					{
						SaveCacheEntry(pXMLDoc, pRootNode, pPair->m_value);
					}
				}
			
				pXMLDoc->appendChild(pRootNode);
				pRootNode.Release();
			}

			CString strFilePath = m_strCacheDir + CACHE_INDEX;

			hr = pXMLDoc->save(CComVariant(strFilePath)); 

			ATLASSERT(hr == S_OK);

			pXMLDoc.Release();
		}

		::CoUninitialize();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CDiskCacheManager::SaveCacheEntry(MSXML::IXMLDOMDocumentPtr pDoc, MSXML::IXMLDOMNodePtr pNode, CDiskCacheEntryPtr pDiskCacheEntry)
{
	MSXML::IXMLDOMNodePtr pEntryNode;

	if (pDoc)
	{
		pEntryNode = pDoc->createElement(ENTRY_NODE.AllocSysString());

		if (NULL != pEntryNode)
		{
			CSettingsTraits::SetStringValue(pEntryNode, ENTRY_NODE_URL, pDiskCacheEntry->GetUrl());

			CString strLocalPath = pDiskCacheEntry->GetLocalPath();

			int nInd = strLocalPath.ReverseFind('\\');

			if (nInd != -1)
			{
				strLocalPath = strLocalPath.Right(strLocalPath.GetLength() - nInd - 1);
			}

			CSettingsTraits::SetStringValue(pEntryNode, ENTRY_NODE_FILE, strLocalPath);

			pNode->appendChild(pEntryNode);
		}
	}
}



CDiskCacheEntryPtr CDiskCacheManager::AddCacheEntry(const CString& strURL, const CString& strLocalPath)
{
	CDiskCacheEntryPtr pEntry;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

	CDiskCacheEntryMap::CPair* pPair = m_mapEntries.Lookup(strURL);

	if (!pPair)
	{
		pEntry = new CDiskCacheEntry(strURL, strLocalPath);
		m_mapEntries.SetAt(strURL, pEntry);	
	}
	else
	{
		pEntry = pPair->m_value;
		pEntry->SetLocalPath(strLocalPath);
	}

	return pEntry;
}


CString CDiskCacheManager::GetCacheDir()
{
	CString strDir;
	CString strKey = "uolmessenger.general.cachedir";

	if (CUIMApplication::GetApplication()->GetSettingsManager()->GetValue(strKey, strDir))
	{
		if (strDir.Right(1) != '\\')
		{
			strDir += '\\';
		}
	}
	else
	{
		ATLASSERT(FALSE);
	}

	return strDir;
}


CString CDiskCacheManager::GetLocalPath()
{
	TCHAR szTemp[MAX_PATH + 1] = {0};

	if (0 != ::GetTempFileName(m_strCacheDir, FILE_NAME_PREFIX, 0, szTemp))
	{
		return CString(szTemp);
	}

	return "";
}


void CDiskCacheManager::ClearOrphanFiles()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CAtlList<CString> listFiles;
	CFileUtils::GetDirectoryFiles(m_strCacheDir, _T("*.tmp"), TRUE, listFiles);

	CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

	CDiskCacheEntryMap::CPair* pPair;
	POSITION posFind = NULL;
	for (POSITION pos = m_mapEntries.GetStartPosition(); pos; )
	{
		pPair = m_mapEntries.GetNext(pos);

		if (pPair != NULL)
		{
			posFind = listFiles.Find(pPair->m_value->GetLocalPath());
			if (posFind)
			{
				listFiles.RemoveAt(posFind);
			}
		}
	}

	CFileUtils::DeleteFiles(listFiles);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


DWORD CDiskCacheManager::GetErrorCode(DWORD dwDownloadError)
{
	DWORD dwErrorCode = IDiskCacheManager::DCME_ERROR_FAILED;
	
	switch (dwDownloadError)
	{
		case CHttpDownloader::DE_ERROR_SUCCESS:
			
			dwErrorCode = IDiskCacheManager::DCME_ERROR_SUCCESS;
			
		break;
		
		// This actualy doesn't means that the URL is not found, but leads 
		// to not retrieve the file, so...
		//
		case CHttpDownloader::DE_HTTP_SEND_REQUEST_ERROR:
		case CHttpDownloader::DE_HTTP_QUERY_STATUS_ERROR:
		case CHttpDownloader::DE_HTTP_QUERY_CONTENT_LENGTH_ERROR:
		case CHttpDownloader::DE_INTERNET_QUERY_DATA_AVAILABLE_ERROR:
		case CHttpDownloader::DE_INTERNET_READ_ERROR:
			
			dwErrorCode = IDiskCacheManager::DCME_URL_NOT_FOUND;
			
		break;
		
		case CHttpDownloader::DE_FILE_CREATION_ERROR:
		case CHttpDownloader::DE_FILE_WRITING_ERROR:
			
			dwErrorCode = IDiskCacheManager::DCME_DISK_WRITE_ERROR;
			
		break;
	}
	
	return dwErrorCode;
}