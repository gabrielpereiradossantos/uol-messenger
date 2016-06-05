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
#include "UserIconCacheManager.h"

#include "../view/UserIcon.h"
#include "../view/image/imagebuilder.h"

#include <DirectoryEnumerator.h>
#include "../FileUtils.h"

#include "./UIMApplication.h"


using namespace Gdiplus;

const TCHAR* CUserIconCacheManager::ACCOUNT_ICON_FILE_NAME_PREFIX = _T("aci");
const TCHAR* CUserIconCacheManager::PROFILE_ICON_FILE_NAME_PREFIX = _T("pfi");
const TCHAR* CUserIconCacheManager::CONTACT_ICON_FILE_NAME_PREFIX = _T("cti");

const TCHAR* CUserIconCacheManager::CONTACT_ICON_FILE_NAME_SUFIX = _T("umc");

const LONG CUserIconCacheManager::ICON_MAX_WIDTH = 96;
const LONG CUserIconCacheManager::ICON_MAX_HEIGHT = 96;


CUserIconCacheManager::CUserIconCacheManager()
{
}


CUserIconCacheManager::~CUserIconCacheManager()
{
}


BOOL CUserIconCacheManager::Init(IUOLMessengerProfileManagerPtr pProfileManager)
{
	BOOL bInitOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (pProfileManager != NULL)
	{
		CString strProfileDir;
		
		IUOLMessengerProfileElementPtr pProfileElement;
		
		if (pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement))
		{
			strProfileDir = pProfileElement->GetProfilePath();

			strProfileDir += _T("\\Icons");

			SetAccountIconCacheDir(strProfileDir);
			SetProfileIconCacheDir(strProfileDir);
			SetContactIconBaseDir(strProfileDir);

			LoadContactIcons();

			bInitOk = TRUE;
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Failed to get profile element.");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"pProfileManager == NULL.");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bInitOk = [%d]", bInitOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bInitOk;
}


void CUserIconCacheManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IUOLMessengerUserIconPtr CUserIconCacheManager::RegisterCachedImage(IUOLMessengerAccount* pAccount, const CString& strCachedImagePath)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if (pAccount != NULL)
	{
		pUserIcon = CreateUserIcon(strCachedImagePath);

		if (pUserIcon != NULL)
		{
			m_mapAccountIcons.SetAt(pAccount, pUserIcon);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}

IUOLMessengerUserIconPtr CUserIconCacheManager::RegisterCachedImage(IUOLMessengerProfileElement* pProfileElement, const CString& strCachedImagePath)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if (pProfileElement != NULL)
	{
		pUserIcon = CreateUserIcon(strCachedImagePath);

		if (pUserIcon != NULL)
		{
			m_mapProfileIcons.SetAt(pProfileElement, pUserIcon);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}

IUOLMessengerUserIconPtr CUserIconCacheManager::RegisterCachedImage(IUOLMessengerContact* pContact, const CString& strCachedImagePath)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if (pContact != NULL)
	{
		pUserIcon = CreateUserIcon(strCachedImagePath);

		if (pUserIcon != NULL)
		{
			CString strKey = BuildContactUserIconKey(pContact);

			m_mapContactIcons.SetAt(strKey, pUserIcon);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


IUOLMessengerUserIconPtr CUserIconCacheManager::CacheIcon(IUOLMessengerAccount* pAccount, const CString& strOriginalPath, const CString& strNewImageFormat)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if ((pAccount == NULL) ||
		(!CFileUtils::CreateDirectory(m_strAccountIconCacheDir)))
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pUserIcon = [%x]", pUserIcon);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return pUserIcon;
	}

	TCHAR szTemp[MAX_PATH + 1] = {0};

	if (0 != ::GetTempFileName(m_strAccountIconCacheDir, CUserIconCacheManager::ACCOUNT_ICON_FILE_NAME_PREFIX, 0, szTemp))
	{
		CAtlStringW strwOriginalPath = strOriginalPath;

		Image* pOriginalIcon = Image::FromFile(strwOriginalPath);

		if (pOriginalIcon)
		{
			// Scale icon according to protocol icon specification...
			CIconSpec iconSpec;

			CIMProtocol* pProtocol = pAccount->GetProtocol();
			ATLASSERT(pProtocol);

			pProtocol->GetIconSpecification(iconSpec);

			CSize sizeCurrent, sizeMin, sizeMax, sizeNew;

			sizeCurrent.cx = pOriginalIcon->GetWidth();
			sizeCurrent.cy = pOriginalIcon->GetHeight();

			sizeMin.cx = iconSpec.GetMinWidth();
			sizeMin.cy = iconSpec.GetMinHeight();

			sizeMax.cx = ((iconSpec.GetMaxWidth())?iconSpec.GetMaxWidth():ICON_MAX_WIDTH);
			sizeMax.cy = ((iconSpec.GetMaxHeight())?iconSpec.GetMaxHeight():ICON_MAX_HEIGHT);

			CalculateImageSize(sizeCurrent, sizeMin, sizeMax, sizeNew);

			Image* pScaledIcon=NULL;
			Bitmap* pBitmap = (Bitmap*)pOriginalIcon->Clone();
			if (pBitmap!=NULL)
			{
				pScaledIcon = GetPhotoImage(pBitmap, sizeNew.cx, sizeNew.cy);
				delete pBitmap;
			}


			// Save scaled icon in appropriate format...
			if (pScaledIcon != NULL)
			{
				CAtlStringW strwDestinyPath = szTemp;

				CString strImageFormat;

				if (strNewImageFormat.IsEmpty())
				{
					// User protocol specific format
					CString strIconFormats = iconSpec.GetImageFormats().c_str();

					if (-1 != strIconFormats.Find("png"))
					{
						strImageFormat = IUOLMessengerUserIconCacheManager::IMG_FORMAT_PNG;
					}
					else if (-1 != strIconFormats.Find("jpeg"))
					{
						strImageFormat = IUOLMessengerUserIconCacheManager::IMG_FORMAT_JPEG;
					}
					else
					{
						strImageFormat = IUOLMessengerUserIconCacheManager::IMG_FORMAT_BMP;
					}
				}

				CLSID clsidEnconder;

				if (GetEncoderClsid(strImageFormat, &clsidEnconder) >= 0)
				{
					if (Ok == pScaledIcon->Save(strwDestinyPath, &clsidEnconder, NULL))
					{
						pUserIcon = CreateUserIcon(szTemp);

						if (pUserIcon)
						{
							CAccountUserIconMap::CPair* pPair = m_mapAccountIcons.Lookup(pAccount);

							if (pPair)
							{
								m_mapAccountIcons.RemoveKey(pAccount);
							}

							m_mapAccountIcons.SetAt(pAccount, pUserIcon);
						}
					}
				}

				delete pScaledIcon;
			}

			delete pOriginalIcon;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


IUOLMessengerUserIconPtr CUserIconCacheManager::CacheIcon(IUOLMessengerProfileElement* pProfileElement, const CString& strOriginalPath, const CString& strNewImageFormat)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if ((pProfileElement == NULL) ||
		(!CFileUtils::CreateDirectory(m_strProfileIconCacheDir)))
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pUserIcon = [%x]", pUserIcon);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return pUserIcon;
	}

	TCHAR szTemp[MAX_PATH + 1] = {0};

	if (0 != ::GetTempFileName(m_strProfileIconCacheDir, CUserIconCacheManager::PROFILE_ICON_FILE_NAME_PREFIX, 0, szTemp))
	{
		CAtlStringW strwOriginalPath = strOriginalPath;

		Image* pOriginalIcon = Image::FromFile(strwOriginalPath);

		if (pOriginalIcon)
		{
			CSize sizeCurrent, sizeMin, sizeMax, sizeNew;

			sizeCurrent.cx = pOriginalIcon->GetWidth();
			sizeCurrent.cy = pOriginalIcon->GetHeight();

			sizeMin.cx = 0;
			sizeMin.cy = 0;

			sizeMax.cx = ICON_MAX_WIDTH;
			sizeMax.cy = ICON_MAX_HEIGHT;

			CalculateImageSize(sizeCurrent, sizeMin, sizeMax, sizeNew);

			Image* pScaledIcon=NULL;
			Bitmap* pBitmap = (Bitmap*)pOriginalIcon->Clone();
			if (pBitmap!=NULL)
			{
				pScaledIcon = GetPhotoImage(pBitmap, sizeNew.cx, sizeNew.cy);
				delete pBitmap;
			}


			// Save scaled icon in appropriate format...
			if (pScaledIcon != NULL)
			{
				CAtlStringW strwDestinyPath = szTemp;

				CString strImageFormat;

				if (strNewImageFormat.IsEmpty())
				{
					strImageFormat = IUOLMessengerUserIconCacheManager::IMG_FORMAT_JPEG;
				}

				CLSID clsidEnconder;

				if (GetEncoderClsid(strImageFormat, &clsidEnconder) >= 0)
				{
					if (Ok == pScaledIcon->Save(strwDestinyPath, &clsidEnconder, NULL))
					{
						pUserIcon = CreateUserIcon(szTemp);

						if (pUserIcon)
						{
							CProfileUserIconMap::CPair* pPair = m_mapProfileIcons.Lookup(pProfileElement);

							if (pPair)
							{
								m_mapProfileIcons.RemoveKey(pProfileElement);
							}

							m_mapProfileIcons.SetAt(pProfileElement, pUserIcon);
						}
					}
				}

				delete pScaledIcon;
			}

			delete pOriginalIcon;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


IUOLMessengerUserIconPtr CUserIconCacheManager::CacheIcon(IUOLMessengerContact* pContact, const char* pIconBuffer, int nBufferLength, const CString& strNewImageFormat)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if (!pContact)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pUserIcon = [%x]", pUserIcon);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return pUserIcon;
	}

	CString strIconDir = GetContactIconDir(pContact);
	if (!CFileUtils::CreateDirectory(strIconDir))
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pUserIcon = [%x]", pUserIcon);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return pUserIcon;
	}

	CString strFileName = GetContactIconFileName(pContact);
	CString strFilePath = strIconDir + _T("\\") + strFileName;

	CString strImageFormat;
	if (strNewImageFormat.IsEmpty())
	{
		strImageFormat = IUOLMessengerUserIconCacheManager::IMG_FORMAT_JPEG;
	}
	else
	{
		strImageFormat = strNewImageFormat;
	}

	// Convert icon to new format...
	TCHAR szTemp[MAX_PATH + 1] = {0};

	if (0 != ::GetTempFileName(GetContactIconBaseDir(), CUserIconCacheManager::CONTACT_ICON_FILE_NAME_PREFIX, 0, szTemp))
	{
		if (SaveIcon(szTemp, pIconBuffer, nBufferLength))
		{
			CAtlStringW strwOriginalPath = szTemp;

			Image* pOriginalIcon = Image::FromFile(strwOriginalPath);

			if (pOriginalIcon != NULL)
			{
				// Scale icon according to protocol icon specification...
				CIconSpec iconSpec;

				CIMProtocol* pProtocol = pContact->GetAccount()->GetProtocol();
				ATLASSERT(pProtocol);

				pProtocol->GetIconSpecification(iconSpec);

				CSize sizeCurrent, sizeMin, sizeMax, sizeNew;

				sizeCurrent.cx = pOriginalIcon->GetWidth();
				sizeCurrent.cy = pOriginalIcon->GetHeight();

				sizeMin.cx = iconSpec.GetMinWidth();
				sizeMin.cy = iconSpec.GetMinHeight();

				sizeMax.cx = ((iconSpec.GetMaxWidth())?iconSpec.GetMaxWidth():ICON_MAX_WIDTH);
				sizeMax.cy = ((iconSpec.GetMaxHeight())?iconSpec.GetMaxHeight():ICON_MAX_HEIGHT);

				CalculateImageSize(sizeCurrent, sizeMin, sizeMax, sizeNew);

				Image* pScaledIcon=NULL;
				Bitmap* pBitmap = (Bitmap*)pOriginalIcon->Clone();
				if (pBitmap!=NULL)
				{
					pScaledIcon = GetPhotoImage(pBitmap, sizeNew.cx, sizeNew.cy);
					delete pBitmap;
				}

				if (pScaledIcon != NULL)
				{
					CAtlStringW strwDestinyPath = strFilePath;

					CLSID clsiEncoder;
					int nResult = GetEncoderClsid(strImageFormat, &clsiEncoder);
					ATLASSERT(nResult >= 0);

					if (nResult >= 0)
					{
						if (Ok == pScaledIcon->Save(strwDestinyPath, &clsiEncoder, NULL))
						{
							pUserIcon = CreateUserIcon(strFilePath);
						}
					}

					delete pScaledIcon;
				}

				delete pOriginalIcon;
			}
		}

		BOOL bDelete = ::DeleteFile(szTemp);
		ATLASSERT(bDelete);
	}

    // Register icon
	if (pUserIcon != NULL)
	{
		CString strKey = BuildContactUserIconKey(pContact);

		CContactUserIconMap::CPair* pPair = m_mapContactIcons.Lookup(strKey);

		if (pPair)
		{
			m_mapContactIcons.RemoveKey(strKey);
		}

		m_mapContactIcons.SetAt(strKey, pUserIcon); 
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


IUOLMessengerUserIconPtr CUserIconCacheManager::GetCachedIcon(IUOLMessengerAccount* pAccount)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	CAccountUserIconMap::CPair* pPair = m_mapAccountIcons.Lookup(pAccount);
	if (pPair != NULL)
	{
		pUserIcon = pPair->m_value;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


IUOLMessengerUserIconPtr CUserIconCacheManager::GetCachedIcon(IUOLMessengerProfileElement* pProfileElement)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	CProfileUserIconMap::CPair* pPair = m_mapProfileIcons.Lookup(pProfileElement);
	if (pPair != NULL)
	{
		pUserIcon = pPair->m_value;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


IUOLMessengerUserIconPtr CUserIconCacheManager::GetCachedIcon(IUOLMessengerContact* pContact)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerUserIconPtr pUserIcon;

	if (pContact != NULL)
	{
		CString strKey = BuildContactUserIconKey(pContact);

		CContactUserIconMap::CPair* pPair = m_mapContactIcons.Lookup(strKey);
		if (pPair != NULL)
		{
			pUserIcon = pPair->m_value;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pUserIcon = [%x]", pUserIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pUserIcon;
}


void CUserIconCacheManager::PurgeAccountIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFilter;

	// Create filter
 	strFilter = CUserIconCacheManager::ACCOUNT_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CAtlList<CString> listFiles;
	CFileUtils::GetDirectoryFiles(m_strAccountIconCacheDir, strFilter, TRUE, listFiles);

	// Filter out referenced icons
	CAccountUserIconMap::CPair* pPair;
	POSITION posFind = NULL;
	for (POSITION pos = m_mapAccountIcons.GetStartPosition(); pos; )
	{
		pPair = m_mapAccountIcons.GetNext(pos);

		if (pPair != NULL)
		{
			posFind = listFiles.Find(pPair->m_value->GetImagePath());
			if (posFind)
			{
				listFiles.RemoveAt(posFind);
			}
		}
	}

	// Delete everything else
	CFileUtils::DeleteFiles(listFiles);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CUserIconCacheManager::PurgeProfileIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFilter;

	// Create filter
 	strFilter = CUserIconCacheManager::PROFILE_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CAtlList<CString> listFiles;
	CFileUtils::GetDirectoryFiles(m_strProfileIconCacheDir, strFilter, TRUE, listFiles);

	// Filter out referenced icons
	CProfileUserIconMap::CPair* pPair;
	POSITION posFind = NULL;
	for (POSITION pos = m_mapProfileIcons.GetStartPosition(); pos; )
	{
		pPair = m_mapProfileIcons.GetNext(pos);

		if (pPair != NULL)
		{
			posFind = listFiles.Find(pPair->m_value->GetImagePath());
			if (posFind)
			{
				listFiles.RemoveAt(posFind);
			}
		}
	}

	// Delete everything else
	CFileUtils::DeleteFiles(listFiles);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CUserIconCacheManager::PurgeContactIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFilter;

	// Create filter
 	strFilter = CUserIconCacheManager::CONTACT_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CAtlList<CString> listFiles;
	CFileUtils::GetDirectoryFiles(m_strContactIconBaseDir, strFilter, TRUE, listFiles);

	// Filter out referenced icons
	CContactUserIconMap::CPair* pPair;
	POSITION posFind = NULL;
	for (POSITION pos = m_mapContactIcons.GetStartPosition(); pos; )
	{
		pPair = m_mapContactIcons.GetNext(pos);

		if (pPair != NULL)
		{
			posFind = listFiles.Find(pPair->m_value->GetImagePath());
			if (posFind)
			{
				listFiles.RemoveAt(posFind);
			}
		}
	}

	// Delete everything else
	CFileUtils::DeleteFiles(listFiles);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUserIconCacheManager::ClearAccountIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFilter;
 	
	strFilter = CUserIconCacheManager::ACCOUNT_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CFileUtils::DeleteDirectoryFiles(GetAccountIconCacheDir(), strFilter);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CUserIconCacheManager::ClearProfileIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFilter;
 	
	strFilter = CUserIconCacheManager::PROFILE_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CFileUtils::DeleteDirectoryFiles(GetProfileIconCacheDir(), strFilter);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CUserIconCacheManager::ClearContactIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFilter;
	
	strFilter = CUserIconCacheManager::CONTACT_ICON_FILE_NAME_PREFIX;
	strFilter += _T("*.tmp");

	CFileUtils::DeleteDirectoryFiles(GetContactIconBaseDir(), strFilter);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CString CUserIconCacheManager::GetPreferedIconFormat(CIMProtocol* pProtocol)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strNewFormat;

	if (!pProtocol)
	{
		return "";
	}

	CIconSpec iconSpec;
	pProtocol->GetIconSpecification(iconSpec);

	CString strSupportedFormats = iconSpec.GetImageFormats().c_str();

	CString strToken;
	int curPos= 0;
	const TCHAR pszToken[] = _T(",");

	// Use BMP format if protocol supports formats not supported by user icon cache manager
	strToken= strSupportedFormats.Tokenize(pszToken, curPos);
	while (strToken != "")
	{
		if ((0 != strToken.CompareNoCase(IUOLMessengerUserIconCacheManager::IMG_FORMAT_PNG)) &&
			(0 != strToken.CompareNoCase(IUOLMessengerUserIconCacheManager::IMG_FORMAT_JPEG)) &&
			(0 != strToken.CompareNoCase(IUOLMessengerUserIconCacheManager::IMG_FORMAT_BMP)))
		{
			strNewFormat = IUOLMessengerUserIconCacheManager::IMG_FORMAT_BMP;
			break;
		}

		strToken = strSupportedFormats.Tokenize(pszToken,curPos);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strNewFormat = [%s]", strNewFormat);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return strNewFormat;
}


CString CUserIconCacheManager::GetAccountIconCacheDir() const
{
	return m_strAccountIconCacheDir;
}


void CUserIconCacheManager::SetAccountIconCacheDir(const CString& strDirPath)
{
	InternalSetCacheDir(m_strAccountIconCacheDir, strDirPath);
}


CString CUserIconCacheManager::GetProfileIconCacheDir() const
{
	return m_strProfileIconCacheDir;
}


void CUserIconCacheManager::SetProfileIconCacheDir(const CString& strDirPath)
{
	InternalSetCacheDir(m_strProfileIconCacheDir, strDirPath);
}


CString CUserIconCacheManager::GetContactIconBaseDir() const
{
	return m_strContactIconBaseDir;
}


void CUserIconCacheManager::SetContactIconBaseDir(const CString& strDirPath)
{
	InternalSetCacheDir(m_strContactIconBaseDir, strDirPath);
}


void CUserIconCacheManager::InternalSetCacheDir(CString& strCurDir, const CString& strNewDir)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (strNewDir[strNewDir.GetLength() - 1] == '\\')
	{
		strCurDir.CopyChars(strCurDir.GetBuffer(), strNewDir, strNewDir.GetLength() - 1);
		strCurDir.ReleaseBuffer();
	}
	else
	{
		strCurDir = strNewDir;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IUOLMessengerUserIconPtr CUserIconCacheManager::CreateUserIcon(const CString& strCachedImagePath)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strCachedImagePath, FALSE);

	if (pImage != NULL)
	{
		IUOLMessengerUserIconPtr pUserIcon = new CUserIcon(pImage, strCachedImagePath);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pUserIcon = [%x]", pUserIcon);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return pUserIcon;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return IUOLMessengerUserIconPtr();
}


void CUserIconCacheManager::CalculateImageSize(const CSize& sizeCurrrent, const CSize& sizeMin, const CSize& sizeMax, CSize& sizeNew)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	ATLASSERT(sizeCurrrent != CSize(0, 0));
	ATLASSERT(sizeMin.cx <= sizeMax.cx);
	ATLASSERT(sizeMin.cy <= sizeMax.cy);

	if ((sizeCurrrent == CSize(0, 0)) ||
		(sizeMin.cx > sizeMax.cx) ||
		(sizeMin.cy > sizeMax.cy) )
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		return;
	}


	float fWidthScale = 1.0;
	float fHeightScale = 1.0;

	if (sizeCurrrent.cx > sizeMax.cx)
	{
		fWidthScale = sizeMax.cx / (float) sizeCurrrent.cx;
	}
	else if (sizeCurrrent.cx < sizeMin.cx)
	{
		fWidthScale = sizeMin.cx / (float) sizeCurrrent.cx;
	}

	if (sizeCurrrent.cy > sizeMax.cy)
	{
		fHeightScale = sizeMax.cy / (float) sizeCurrrent.cy;
	}
	else if (sizeCurrrent.cy < sizeMin.cy)
	{
		fHeightScale = sizeMin.cy / (float) sizeCurrrent.cy;
	}

	if (fWidthScale > fHeightScale)
	{
		if ((LONG) (fWidthScale * sizeCurrrent.cy) <= sizeMax.cy)
		{
			fHeightScale = fWidthScale;
		}
		else
		{
			fHeightScale = sizeMax.cy / (float) sizeCurrrent.cy;
		}
	}
	else
	{
		if ((LONG) (fHeightScale * sizeCurrrent.cx) <= sizeMax.cx)
		{
			fWidthScale = fHeightScale;
		}
		else
		{
			fWidthScale = sizeMax.cx / (float) sizeCurrrent.cx;
		}
	}

	sizeNew.cx = (LONG) (sizeCurrrent.cx * fWidthScale);
	sizeNew.cy = (LONG) (sizeCurrrent.cy * fHeightScale);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"sizeNew.cx = [%d], sizeNew.cy = [%d].", 
			sizeNew.cx, sizeNew.cy);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


int CUserIconCacheManager::GetEncoderClsid(const CString& strImageFormat, CLSID* pClsid)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	UINT nCount = 0;
	UINT nSize = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&nCount, &nSize);
	if (nSize == 0)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "ret = [-1]");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		// Failure
		return -1;
	}

	pImageCodecInfo = (ImageCodecInfo*) (malloc(nSize));
	if (pImageCodecInfo == NULL)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "ret = [-1]");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
		
		// Failure
		return -1;
	}

	GetImageEncoders(nCount, nSize, pImageCodecInfo);

	CAtlStringW wstrFormat;

	if (0 == strImageFormat.Compare(IUOLMessengerUserIconCacheManager::IMG_FORMAT_JPEG))
	{
		wstrFormat = L"image/jpeg";
	}
	else if (0 == strImageFormat.Compare(IUOLMessengerUserIconCacheManager::IMG_FORMAT_PNG))
	{
		wstrFormat = L"image/png";
	}
	else
	{
		// IUOLMessengerUserIconCacheManager::IMG_FORMAT_BMP
		wstrFormat = L"image/bmp";
	}

	for (UINT j = 0; j < nCount; ++j)
	{
		if (0 == wstrFormat.Compare(pImageCodecInfo[j].MimeType))
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);

			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, "ret = [%d].", j);
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogEndMethod(__FUNCTION__);
			
			return j;
		}    
	}

	// Failure
	free(pImageCodecInfo);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "ret = [-1]");
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return -1;
}


CString CUserIconCacheManager::BuildContactUserIconKey(IUOLMessengerContact* pContact)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strKey, strAccountUserId, strProtocolId;

	IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

	ATLASSERT(pAccount != NULL);
	if (pAccount != NULL)
	{
		strAccountUserId = pAccount->GetUserId();
		strProtocolId = pAccount->GetProtocolId();
	}
	
	CString strContactUserIconKey = 
			BuildContactUserIconKey(strAccountUserId, strProtocolId, pContact->GetName());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strContactUserIconKey = [%s]", strContactUserIconKey);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return strContactUserIconKey;
}


CString CUserIconCacheManager::BuildContactUserIconKey(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strName)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strKey;

	strKey.Format(_T("%s/%s/%s"), strName, strAccountUserId, strAccountProtocolId);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strKey = [%s]", strKey);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return strKey;
}


CString CUserIconCacheManager::GetContactIconDir(IUOLMessengerContact* pContact)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strDirPath;

	strDirPath.Format(_T("%s\\%s@%s"), 
			GetContactIconBaseDir(), 
			pContact->GetAccount()->GetUserId(), 
			pContact->GetAccount()->GetProtocolId());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strDirPath = [%s]", strDirPath);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return strDirPath;
}


CString CUserIconCacheManager::GetContactIconFileName(IUOLMessengerContact* pContact)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFileName;

	strFileName.Format(_T("%s.%s"), 
			pContact->GetName(),
			CUserIconCacheManager::CONTACT_ICON_FILE_NAME_SUFIX);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strFileName = [%s]", strFileName);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return strFileName;
}


BOOL CUserIconCacheManager::SaveIcon(LPCTSTR lpszFilePath, const char* pIconBuffer, int nBufferLength)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	DWORD dwBytesWritten = 0;

	CAtlFile fileIcon;

	HRESULT hr = fileIcon.Create(lpszFilePath, GENERIC_WRITE, 0, CREATE_ALWAYS); 
	
	if (SUCCEEDED(hr))
	{
		hr = fileIcon.Write(pIconBuffer, nBufferLength * sizeof(char), &dwBytesWritten);

		fileIcon.Close();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"nBufferLength = [%d], dwBytesWritten = [%d], hr = [%d].", 
			nBufferLength, dwBytesWritten, hr);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return (dwBytesWritten == nBufferLength);
}


void CUserIconCacheManager::LoadContactIcons()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strBaseDir = GetContactIconBaseDir();

	CAtlList<CString> listDirs;

	CDirectoryEnumerator::GetDirectories(listDirs, _T("*"), strBaseDir);

	CString strIconFilter;
	strIconFilter.Format(_T("*.%s"), CUserIconCacheManager::CONTACT_ICON_FILE_NAME_SUFIX);

	for (POSITION pos = listDirs.GetHeadPosition(); pos; )
	{
		CString strAccountDir = listDirs.GetNext(pos);
	
		CString strAccountUserId;
		CString strAccountProtocolId;

		ExtractAccount(strAccountDir, strAccountUserId, strAccountProtocolId);

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Loading icons from [%s] for user [%s] with protocol [%s] using filter [%s].", 
				strAccountDir, strAccountUserId, strAccountProtocolId, strIconFilter);
		
		if ((FALSE == strAccountUserId.IsEmpty()) && 
			(FALSE == strAccountProtocolId.IsEmpty()))
		{
			strAccountDir = strBaseDir + _T("\\") + strAccountDir;

			CAtlList<CString> listIcons;
			CFileUtils::GetDirectoryFiles(strAccountDir, strIconFilter, TRUE, listIcons);

			for (POSITION posIcon = listIcons.GetHeadPosition(); posIcon; )
			{
				CString strIconFilePath = listIcons.GetNext(posIcon);

				CString strContactName;
				ExtractContactName(strIconFilePath, strContactName);

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Processing icon [%s] that belongs to contact [%s].", 
						strIconFilePath, strContactName);
				
				if (FALSE == strContactName.IsEmpty())
				{
					IUOLMessengerUserIconPtr pUserIcon = CreateUserIcon(strIconFilePath);

					if (pUserIcon)
					{
						CString strKey = BuildContactUserIconKey(strAccountUserId, strAccountProtocolId, strContactName);

						static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
								GetLogger()->LogDebug(__FUNCTION__, 
								"Mapping item strKey = [%x] and pUserIcon = [%x].", 
								strKey, pUserIcon.get());
						
						m_mapContactIcons.SetAt(strKey, pUserIcon);
					}
					else
					{
						static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
								GetLogger()->LogDebug(__FUNCTION__, 
								"pUserIcon = [%x].", pUserIcon.get());
					}
				}
			}
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUserIconCacheManager::ExtractAccount(const CString& strDir, CString& strUserId, CString& strProtocolId)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	strUserId.Empty();
	strProtocolId.Empty();

	int nFind = strDir.ReverseFind('@');
	if (nFind >= 0)
	{
		strUserId = strDir.Left(nFind);
		strProtocolId = strDir.Right(strDir.GetLength() - nFind - 1);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strDir = [%s], strUserId = [%s], strProtocolId = [%s].", 
			strDir, strUserId, strProtocolId);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUserIconCacheManager::ExtractContactName(const CString& strIconFilePath, CString& strContactName)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	strContactName.Empty();

	int nFind = strIconFilePath.ReverseFind('\\');
	if (nFind >= 0)
	{
		int nFindExt = strIconFilePath.ReverseFind('.');
		if (nFindExt > nFind)
		{
			strContactName = strIconFilePath.Mid(nFind + 1, nFindExt - nFind - 1);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strIconFilePath = [%s], strContactName = [%s].", 
			strIconFilePath, strContactName);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


Image* CUserIconCacheManager::GetPhotoImage(Bitmap* pBitmap, int cx, int cy)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	ATLASSERT(pBitmap);
	ATLASSERT(cx);
	ATLASSERT(cy);

	if ((NULL == pBitmap) || 
		(0 == cx) || 
		(0 == cy))
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return NULL;
	}


	//Bitmap* pBitmapCrop = pBitmap->Clone(1,1,3,3,PixelFormatDontCare);
	//int offsetx, offsety;
	//int newcx, newcy;

	//Image* pScaledIcon = pBitmapCrop->GetThumbnailImage(sizeNew.cx, sizeNew.cy, NULL, NULL);
	
	int offsetx;
	int	offsety;
	int newcx;
	int	newcy;

	int bx = pBitmap->GetWidth();
	int by = pBitmap->GetHeight();

	float rx =  ( ((float) bx ) / ( (float) cx ) );
	float ry =  ( ((float) by ) / ( (float) cy ) );

	BOOL ycut=FALSE;


	// descubro quem sofrera o corte
	if ( ry > rx )
	{
		ycut = TRUE;
	}

	// se for a altura q sofre o corte
	if (ycut)
	{
		// x eh maximo
		newcx = bx;
		offsetx = 0;

		newcy = ( newcx * cy ) / cx;
		offsety = (by - newcy) / 2;
	}
	// se a largura sofre o corte
	else
	{
		newcy = by;
		offsety = 0;

		newcx = ( newcy * cx) / cy;
		offsetx = (bx - newcx) / 2;
	}     

	Bitmap* pBitmapCrop = pBitmap->Clone(offsetx,offsety,newcx,newcy,PixelFormatDontCare);

	//int xx,yy;
	//xx = pBitmapCrop->GetWidth();
	//yy = pBitmapCrop->GetHeight();

	if (pBitmapCrop!=NULL)
	{
		Image* pImage = pBitmapCrop->GetThumbnailImage(cx, cy, NULL, NULL);
		int xx = pImage->GetWidth();
		int yy = pImage->GetHeight();

		delete pBitmapCrop;
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pImage = [%x]", pImage);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);

		return pImage;

		//return pBitmapCrop->GetThumbnailImage(cx, cy, NULL, NULL);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return NULL;
}


//void CUserIconCacheManager::CalcPhotoCrop(Bitmap* pBitmap, int targex, int targexy, int& offsetx, int& offsety)
//{
//
//}