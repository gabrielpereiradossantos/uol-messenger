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
#include "IconManager.h"

#include "../resource.h"
#include "MinDependencySpecialFolder.h"



using namespace Gdiplus;

const TCHAR* CIconManager::ACCOUNT_ICON_FILE_NAME_PREFIX = _T("aci");
const TCHAR* CIconManager::CONTACT_ICON_FILE_NAME_PREFIX = _T("cti");



CIconManager::CIconManager()
{
	// Initialize directories
	CMinDependencySpecialFolder mdsf;

	CString strCompany;
	strCompany.LoadString(IDS_STRING_COMPANY_NAME);

	CString strApplication;
	strApplication.LoadString(IDS_STRING_PRODUCT_NAME);

	CString strPath = mdsf.GetAppData() + strCompany + _T("\\") + strApplication  + _T("\\Icons");

	SetAccountIconBaseDir(strPath);
	SetContactIconBaseDir(strPath);
}


CIconManager::~CIconManager()
{
}


void CIconManager::SetAccountIconBaseDir(const CString& strDirPath)
{
	if (strDirPath[strDirPath.GetLength() - 1] == '\\')
	{
		m_strAccountIconBaseDir.CopyChars(m_strAccountIconBaseDir.GetBuffer(), strDirPath, strDirPath.GetLength() - 1);
		m_strAccountIconBaseDir.ReleaseBuffer();
	}
	else
	{
		m_strAccountIconBaseDir = strDirPath;
	}
}


void CIconManager::SetContactIconBaseDir(const CString& strDirPath)
{
	if (strDirPath[strDirPath.GetLength() - 1] == '\\')
	{
		m_strContactIconBaseDir.CopyChars(m_strContactIconBaseDir.GetBuffer(), strDirPath, strDirPath.GetLength() - 1);
		m_strContactIconBaseDir.ReleaseBuffer();
	}
	else
	{
		m_strContactIconBaseDir = strDirPath;
	}
}


void CIconManager::RegisterAccountIcon(IUOLMessengerAccount* pAccount)
{
	if (pAccount != NULL)
	{
		//m_mapAccountIcons.SetAt(pAccount, pAccount->GetIconPath());
	}
}



BOOL CIconManager::SaveAccountIcon(IUOLMessengerAccount* pAccount, const CString& strOriginalPath, CString& strIconPath)
{
	BOOL bRet = FALSE;

	if ((pAccount == NULL) ||
		(!CreateDirectory(m_strAccountIconBaseDir)))
	{
		return bRet;
	}

	TCHAR szTemp[MAX_PATH + 1] = {0};

	if (0 != ::GetTempFileName(m_strAccountIconBaseDir, CIconManager::ACCOUNT_ICON_FILE_NAME_PREFIX, 0, szTemp))
	{
		CAtlStringW strwOriginalPath = strOriginalPath;

		Image* pOriginalIcon = Image::FromFile(strwOriginalPath);

		if (pOriginalIcon)
		{
			// Scale icon according to protocol icon specification...
			UINT nIconWidth = pOriginalIcon->GetWidth();
			UINT nIconHeight = pOriginalIcon->GetHeight();

			CIconSpec iconSpec;
			pAccount->GetIconSpecification(iconSpec);

			float fWidthScale = 1.0;
			float fHeightScale = 1.0;

			if (nIconWidth > iconSpec.GetMaxWidth())
			{
				fWidthScale = iconSpec.GetMaxWidth() / (float) nIconWidth;
			}
			else if (nIconWidth < iconSpec.GetMinWidth())
			{
				fWidthScale = iconSpec.GetMinWidth() / (float) nIconWidth;
			}

			if (nIconHeight > iconSpec.GetMaxHeight())
			{
				fHeightScale = iconSpec.GetMaxHeight() / (float) nIconHeight;
			}
			else if (nIconHeight < iconSpec.GetMinHeight())
			{
				fHeightScale = iconSpec.GetMinHeight() / (float) nIconHeight;
			}

			if (fWidthScale > fHeightScale)
			{
				if ((UINT) (fWidthScale * nIconHeight) <= iconSpec.GetMaxHeight())
				{
					fHeightScale = fWidthScale;
				}
				else
				{
					fHeightScale = iconSpec.GetMaxHeight() / (float) nIconHeight;
				}
			}
			else
			{
				if ((UINT) (fHeightScale * nIconWidth) <= iconSpec.GetMaxWidth())
				{
					fWidthScale = fHeightScale;
				}
				else
				{
					fWidthScale = iconSpec.GetMaxWidth() / (float) nIconWidth;
				}
			}

			nIconWidth = (UINT) (nIconWidth * fWidthScale);
			nIconHeight = (UINT) (nIconHeight * fHeightScale);

			Image* pScaledIcon = pOriginalIcon->GetThumbnailImage(nIconWidth, nIconHeight, NULL, NULL);

			// Save scaled icon in appropriate format...
			if (pScaledIcon != NULL)
			{
				CAtlStringW strwDestinyPath = szTemp;

				CString strIconFormats = iconSpec.GetImageFormats().c_str();

				CLSID clsidEnconder;
				int nResult;
				if (-1 != strIconFormats.Find("jpeg"))
				{
					nResult = GetEncoderClsid(L"image/jpeg", &clsidEnconder);
				}
				else if (-1 != strIconFormats.Find("png"))
				{
					nResult = GetEncoderClsid(L"image/png", &clsidEnconder);
				}
				else
				{
					nResult = GetEncoderClsid(L"image/bmp", &clsidEnconder);
				}

				ATLASSERT(nResult >= 0);

				if (nResult >= 0)
				{
					bRet = (Ok == pScaledIcon->Save(strwDestinyPath, &clsidEnconder, NULL));

					if (bRet)
					{
						strIconPath = szTemp;

						m_mapAccountIcons.SetAt(pAccount, strIconPath);
					}
				}

				delete pScaledIcon;
			}

			delete pOriginalIcon;
		}
	}

	return bRet;
}


BOOL CIconManager::SaveContactIcon(IUOLMessengerContact* pContact, const char* pIconBuffer, int nBufferLength, CString& strIconPath, BOOL bConvert)
{
	BOOL bRet = FALSE;

	if (!CreateDirectory(m_strContactIconBaseDir))
	{
		return bRet;
	}

	TCHAR szTemp[MAX_PATH + 1] = {0};

	if (0 != ::GetTempFileName(m_strContactIconBaseDir, CIconManager::CONTACT_ICON_FILE_NAME_PREFIX, 0, szTemp))
	{
		CAtlFile fileIcon;

		HRESULT hr = fileIcon.Create(szTemp, GENERIC_WRITE, 0, CREATE_ALWAYS); 
		if (SUCCEEDED(hr))
		{
			DWORD dwBytesWritten = 0;
			hr = fileIcon.Write(pIconBuffer, nBufferLength * sizeof(char), &dwBytesWritten);

			fileIcon.Close();
			
			if (dwBytesWritten == nBufferLength)
			{
				if (bConvert)
				{
					// Convert icon to bmp
					CAtlStringW strwOriginalPath = szTemp;

					Image* pOriginalIcon = Image::FromFile(strwOriginalPath);

					if (pOriginalIcon != NULL)
					{
						// Get new temporary file name
						if (0 != ::GetTempFileName(m_strContactIconBaseDir, CIconManager::CONTACT_ICON_FILE_NAME_PREFIX, 0, szTemp))
						{
							CAtlStringW strwDestinyPath = szTemp;

							CLSID clsidBMP;
							int nResult = GetEncoderClsid(L"image/bmp", &clsidBMP);
							ATLASSERT(nResult >= 0);

							if (nResult >= 0)
							{
								bRet = (Ok == pOriginalIcon->Save(strwDestinyPath, &clsidBMP, NULL));

								if (bRet)
								{
									strIconPath = szTemp;

									m_mapContactIcons.SetAt(pContact, strIconPath);
								}
							}
						}

						delete pOriginalIcon;
					}
				}
				else
				{
					bRet = TRUE;
				
					strIconPath = szTemp;

					m_mapContactIcons.SetAt(pContact, strIconPath);
				}
			}			
		}
	}

	return bRet;
}


CString CIconManager::GetAccountIconPath(IUOLMessengerAccount* pAccount)
{
	CAtlMap<IUOLMessengerAccount*, CString>::CPair* pPair;

	pPair = m_mapAccountIcons.Lookup(pAccount);

	if (pPair != NULL)
	{
		return pPair->m_value;
	}

	return _T("");
}


CString CIconManager::GetContactIconPath(IUOLMessengerContact* pContact)
{
	CAtlMap<IUOLMessengerContact*, CString>::CPair* pPair;

	pPair = m_mapContactIcons.Lookup(pContact);

	if (pPair != NULL)
	{
		return pPair->m_value;
	}

	return _T("");
}


void CIconManager::ClearIcons()
{
	CString strFilter;

	// Remove unreferenced account icons...
 	strFilter = CIconManager::ACCOUNT_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CAtlList<CString> listFiles;
	GetDirectoryFiles(m_strAccountIconBaseDir, strFilter, TRUE, listFiles);

	// Filter out referenced icons
	CAtlMap<IUOLMessengerAccount*, CString>::CPair* pPair;
	POSITION posFind = NULL;
	for (POSITION pos = m_mapAccountIcons.GetStartPosition(); pos; )
	{
		pPair = m_mapAccountIcons.GetNext(pos);

		if (pPair != NULL)
		{
			posFind = listFiles.Find(pPair->m_value);
			if (posFind)
			{
				listFiles.RemoveAt(posFind);
			}
		}
	}

	// Remove files
	BOOL bOK;
	for (POSITION pos = listFiles.GetHeadPosition(); pos; )
	{
		bOK = DeleteFile(listFiles.GetNext(pos));
		ATLASSERT(bOK);
	}

	// Remove contact icons
	strFilter = CIconManager::CONTACT_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");
	DeleteDirectoryFiles(m_strContactIconBaseDir, strFilter);
}


BOOL CIconManager::CreateDirectory(const CString& strDirPath)
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


void CIconManager::DeleteDirectoryFiles(const CString& strDir, const CString& strFilter)
{
	CAtlList<CString> listFiles;
	GetDirectoryFiles(strDir, strFilter, TRUE, listFiles);

	BOOL bOK = FALSE;

	for (POSITION pos = listFiles.GetHeadPosition(); pos; )
	{
		bOK = DeleteFile(listFiles.GetNext(pos));
		ATLASSERT(bOK);
	}
}


void CIconManager::GetDirectoryFiles(const CString& strDir, const CString& strFilter, BOOL bAbsoultePath, CAtlList<CString>& listFiles)
{
	// Build find expression
	CString strFindFilter = strDir + _T("\\") + strFilter;

	// Find matching files...
	WIN32_FIND_DATA findFileData = {0};
	HANDLE hFind;

	hFind = ::FindFirstFile(strFindFilter, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		// No matching files
		return;
	}

	CString strFileName;

	do
	{
		if (!bAbsoultePath)
		{
			strFileName = findFileData.cFileName;
		}
		else
		{
			strFileName = strDir + _T("\\") + findFileData.cFileName;
		}

		listFiles.AddTail(strFileName);
	}
	while (::FindNextFile(hFind, &findFileData));

	::FindClose(hFind);
}


int CIconManager::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT nCount = 0; // number of image encoders
	UINT nSize = 0;  // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&nCount, &nSize);
	if (nSize == 0)
	{
		return -1;  // Failure
	}

	pImageCodecInfo = (ImageCodecInfo*) (malloc(nSize));
	if (pImageCodecInfo == NULL)
	{
		return -1;  // Failure
	}

	GetImageEncoders(nCount, nSize, pImageCodecInfo);

	for (UINT j = 0; j < nCount; ++j)
	{
		if (::wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}