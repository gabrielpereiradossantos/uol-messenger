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
#include "VerifyDllsThread.h"
#include <atlpath.h>
#include <util/DirectoryEnumerator.h>

CVerifyDllsThread::CVerifyDllsThread() : 
	m_nStopped(FALSE),
	m_dwErrorCode(1), //generic error
	m_pCallback(NULL),
	m_dwCurItem(0)
{
}


CVerifyDllsThread::~CVerifyDllsThread()
{
	CDownloadDllItem* pDllItem = NULL;
	POSITION pos = m_DownloadItems.GetHeadPosition();
	while (pos)
	{
		pDllItem = m_DownloadItems.GetNext(pos);
		ATLASSERT(NULL != pDllItem);
		delete pDllItem;
	}
}


void CVerifyDllsThread::SetCallback(IVerifyDllsThreadCallback* pCallback)
{
	m_pCallback = pCallback;
}

DWORD CVerifyDllsThread::GetInstallationError()
{
	return m_dwErrorCode;
}


void CVerifyDllsThread::CancelVerification()
{
	::InterlockedExchange(&m_nStopped, TRUE);
}


void CVerifyDllsThread::Run()
{
	::InterlockedExchange(&m_nStopped, FALSE);

	HRESULT hr = S_OK;

	CString strFolder;
	CHttpDownloader httpDownloader;
	httpDownloader.SetCallback(this);
	httpDownloader.SetStopFlag(this);
	
	do
	{
		if ( NULL == m_pCallback )
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildInitializationFailure(
					CUOLFoneFailureInfo::INITIALIZATION_INVALID_PARAM_FAILURE, 
					__FUNCTION__, "m_pCallback");
			
			ATLTRACE(_T("CVerifyDllsThread::Run callback is null\n"));
			break;
		}
	
		CDownloadDllItem* pDllItem = NULL;
		m_dwCurItem = 0;
		POSITION pos = m_DownloadItems.GetHeadPosition();
		while (pos)
		{
			pDllItem = m_DownloadItems.GetNext(pos);
			ATLASSERT(NULL != pDllItem);
			if ( ATLPath::FileExists( pDllItem->m_strLocalFilePath ) == FALSE ) 
			{
				
				int nPos = pDllItem->m_strLocalFilePath.ReverseFind(_TCHAR('\\'));
				ATLASSERT(nPos > 0);
				strFolder = pDllItem->m_strLocalFilePath.Left(nPos);

				if (FALSE == ATLPath::FileExists(strFolder))
				{
					if ( FALSE == ::CreateDirectory(strFolder, NULL) )
					{
						m_pLastFailureInfo = CUOLFoneFailureInfo::BuildSystemFailure(
								CUOLFoneFailureInfo::SYSTEM_PATH_CREATION_FAILURE, 
								GetLastError(), strFolder);
						
						ATLTRACE(_T("CVerifyDllsThread::Run - CreateDirectory %s failed\n"), strFolder);
						hr = E_FAIL;
						break;
					}
				}
				
				httpDownloader.SetUrl(pDllItem->m_strFromUrl);
				httpDownloader.SetLocalFilePath(pDllItem->m_strLocalFilePath);
				m_dwErrorCode = httpDownloader.DownloadFile();

				if (CHttpDownloader::DE_ERROR_SUCCESS !=  m_dwErrorCode)
				{
					if ( (CHttpDownloader::DE_FILE_CREATION_ERROR == m_dwErrorCode) || 
							(CHttpDownloader::DE_FILE_WRITING_ERROR == m_dwErrorCode) )
					{
						m_pLastFailureInfo = CUOLFoneFailureInfo::BuildDownloadFailure(
								m_dwErrorCode, 
								httpDownloader.GetAdditionalErrorCode(), 
								httpDownloader.GetUrl());
						
						if (m_pLastFailureInfo)
						{
							m_pLastFailureInfo->AddExtraFailureInfo(
									CUOLFoneFailureInfo::EXTRA_INFO_PATH, 
									httpDownloader.GetLocalFilePath());
						}
					}
					else if (CHttpDownloader::DE_HTTP_STATUS_CODE_ERROR == m_dwErrorCode)
					{
						m_pLastFailureInfo = 
								CUOLFoneFailureInfo::BuildDownloadFailure(
										m_dwErrorCode, 
										httpDownloader.GetAdditionalErrorCode(), 
										httpDownloader.GetUrl(), 
										httpDownloader.GetHTTPStatusErrorCode(), 
										httpDownloader.GetHTTPStatusErrorMessage());
					}
					else
					{
						m_pLastFailureInfo = 
								CUOLFoneFailureInfo::BuildDownloadFailure(
										m_dwErrorCode, 
										httpDownloader.GetAdditionalErrorCode(), 
										httpDownloader.GetUrl());
					}
					
					hr = E_FAIL;
					break;
				}
			}

			RemoveOldDlls(pDllItem->m_strLocalFilePath);

			m_dwCurItem++;
		}

	} while(FALSE);
	
	if (m_pCallback)
	{
		m_pCallback->OnVerifyDllsFinished(hr);
	}
}


void CVerifyDllsThread::OnDownloadBegin()
{
	ATLASSERT(NULL != m_pCallback);
	
	m_pCallback->OnDownloadBegin(m_dwCurItem);
}


void CVerifyDllsThread::OnProgressChange(ULONG nProgress, ULONG nProgressMax)
{
	ATLASSERT(NULL != m_pCallback);

	m_pCallback->OnProgressChange(m_dwCurItem, nProgress, nProgressMax);
}


void CVerifyDllsThread::OnDownloadComplete(DWORD dwError)
{
	ATLASSERT(NULL != m_pCallback);

	m_pCallback->OnDownloadComplete(m_dwCurItem, dwError);
}


BOOL CVerifyDllsThread::GetStopFlag()
{
	return m_nStopped;
}

void CVerifyDllsThread::AddItem(const CString& strFromUrl, const CString& strLocalFilePath)
{
	ATLASSERT(strFromUrl.GetLength()>0);
	ATLASSERT(strLocalFilePath.GetLength()>0);
	
	m_DownloadItems.AddTail(new CDownloadDllItem(strFromUrl, strLocalFilePath));
}


CUOLFoneFailureInfoPtr CVerifyDllsThread::GetLastFailureInfo()
{
	return m_pLastFailureInfo;
}


void CVerifyDllsThread::RemoveOldDlls(const CString& strLatestDllLocalFilePath)
{
	/// Test code
	//int nCompareTest;
	//nCompareTest = CompareVersions(_T("1.2.3.4"), _T("1.2.3.4"));
	//ATLASSERT(nCompareTest == 0);
	//
	//nCompareTest = CompareVersions(_T("9.2.3.4"), _T("1.2.3.4"));
	//ATLASSERT(nCompareTest > 0);

	//nCompareTest = CompareVersions(_T("1.9.3.4"), _T("1.2.3.4"));
	//ATLASSERT(nCompareTest > 0);

	//nCompareTest = CompareVersions(_T("1.2.9.4"), _T("1.2.3.4"));
	//ATLASSERT(nCompareTest > 0);

	//nCompareTest = CompareVersions(_T("1.2.3.9"), _T("1.2.3.4"));
	//ATLASSERT(nCompareTest > 0);

	//nCompareTest = CompareVersions(_T("1.2.3.4"), _T("9.2.3.4"));
	//ATLASSERT(nCompareTest < 0);

	//nCompareTest = CompareVersions(_T("1.2.3.4"), _T("1.9.3.4"));
	//ATLASSERT(nCompareTest < 0);

	//nCompareTest = CompareVersions(_T("1.2.3.4"), _T("1.2.9.4"));
	//ATLASSERT(nCompareTest < 0);

	//nCompareTest = CompareVersions(_T("1.2.3.4"), _T("1.2.3.9"));
	//ATLASSERT(nCompareTest < 0);

	//nCompareTest = CompareVersions(_T("1.2.3"), _T("1.2.3.4"));
	//ATLASSERT(nCompareTest == 0);

	//nCompareTest = CompareVersions(_T("1.2.3.4"), _T("1.2.3"));
	//ATLASSERT(nCompareTest == 0);
	/// Test code

	int nFind = -1;

	// Extract base directory and file name...
	CString strBaseDir;
	CString strLatestFileName;

	nFind = strLatestDllLocalFilePath.ReverseFind('\\');

	if (nFind != -1)
	{
		strBaseDir = strLatestDllLocalFilePath.Left(nFind);
		strLatestFileName = strLatestDllLocalFilePath.Mid(nFind + 1);
	}
	else
	{
		strBaseDir = _T(".\\");
		strLatestFileName = strLatestDllLocalFilePath;
	}

	int nFindVersionSep = strLatestFileName.ReverseFind('-');

	if (nFindVersionSep != -1)
	{
		// Extract base file name / extension...
		CString strBaseFileName;
		CString strBaseFileExtension;
		CString strLatestVersion;

		strBaseFileName = strLatestFileName.Left(nFindVersionSep + 1);

		int nFindExtensionSep = strLatestFileName.ReverseFind('.');

		if (nFindExtensionSep != -1)
		{
			strBaseFileExtension = strLatestFileName.Mid(nFindExtensionSep);
			strLatestVersion = strLatestFileName.Mid(nFindVersionSep + 1, nFindExtensionSep - nFindVersionSep - 1);
		}
		else
		{
			strBaseFileExtension = _T("");
			strLatestVersion = strLatestFileName.Mid(nFindVersionSep + 1);
		}

		// List files with similar name...
		CAtlList<CString> listFiles;
		CString strSearchCriteria;

		strSearchCriteria = strBaseFileName + _T("*") + strBaseFileExtension;

		CDirectoryEnumerator::GetFiles(listFiles, strSearchCriteria, strBaseDir);
		
		for (POSITION pos = listFiles.GetHeadPosition(); pos; )
		{
			CString strDeleteFilePath = listFiles.GetNext(pos);

			// Compare file versions by file name and delete older versions...
			CString strVersion;

			nFind = strDeleteFilePath.Find(strBaseFileName);

			ATLASSERT(nFind >= 0);
			if (nFind >= 0)
			{
				nFind += strBaseFileName.GetLength();

				if (nFindExtensionSep != -1)
				{
					int nFindExtension = strDeleteFilePath.ReverseFind('.');

					strVersion = strDeleteFilePath.Mid(nFind, nFindExtension - nFind);
				}
				else
				{
					strVersion = strDeleteFilePath.Mid(nFind);
				}

				if (CompareVersions(strVersion, strLatestVersion) < 0)
				{
					BOOL bDeleted = ::DeleteFile(strBaseDir + _T("\\") + strDeleteFilePath);
					ATLASSERT(bDeleted);
				}
			}
		}
	}
}


int CVerifyDllsThread::CompareVersions(const CString& strVersion1, const CString& strVersion2)
{
	const TCHAR VERSION_SEP = _T('.');
	int nRet = -1;
	int nFindL1 = 0;
	int nFindL2 = 0;
	int nFindR1, nFindR2;
	LONG lVersionComp1, lVersionComp2;
	CString strVersionComp1, strVersionComp2;

	do
	{
		nFindR1 = strVersion1.Find(VERSION_SEP, nFindL1);

		if (nFindR1 != -1)
		{
			strVersionComp1 = strVersion1.Mid(nFindL1, nFindR1 - nFindL1);
			nFindL1 = nFindR1 + 1;
		}
		else
		{
			strVersionComp1 = strVersion1.Mid(nFindL1);
		}

		nFindR2 = strVersion2.Find(VERSION_SEP, nFindL2);

		if (nFindR2 != -1)
		{
			strVersionComp2 = strVersion2.Mid(nFindL2, nFindR2 - nFindL2);
			nFindL2 = nFindR2 + 1;
		}
		else
		{
			strVersionComp2 = strVersion2.Mid(nFindL2);
		}

		lVersionComp1 = ::_ttol(strVersionComp1);
		lVersionComp2 = ::_ttol(strVersionComp2);

		nRet = lVersionComp1 - lVersionComp2;

		if (nRet != 0)
		{
			break;
		}
	}
	while ((nFindR1 != -1) && (nFindR2 != -1));

	return nRet;
}
