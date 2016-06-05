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
#include "FileTransferFilter.h"

#include <fstream>
#include "../UIMApplication.h"
#include "../../resource.h"
#include "../../FileUtils.h"


static const CString EXTENSION_CONFIG = _T("filter.ext");


CFileTransferFilter::CFileTransferFilter()
{
}


CFileTransferFilter::~CFileTransferFilter()
{
}


BOOL CFileTransferFilter::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strConfigDir = GetConfigDir();

	BOOL bRet = CFileUtils::CreateDirectory(strConfigDir);

	if (bRet)
	{
		m_strBadExtensionsFile = strConfigDir + EXTENSION_CONFIG;

		LoadBadExtensionsConfig();
		UpdateBadExtensionsConfig();
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"CreateDirectory() failed. strConfigDir = [%s]", strConfigDir);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CFileTransferFilter::Finalize()
{
}


BOOL CFileTransferFilter::IsBadFileType(const CString& strFileName)
{
	BOOL bRet = FALSE;

	CString strExtension = strFileName;
	strExtension.MakeLower();

	int nFind = strExtension.ReverseFind('.');
	
	if (-1 != nFind)
	{
		strExtension = strExtension.Mid(nFind + 1);

		if (FALSE == strExtension.IsEmpty())
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csBadExtensions);

			POSITION pos = m_listBadExtensions.Find(strExtension);

			bRet = (pos != NULL);
		}
	}

	return bRet;
}


void CFileTransferFilter::OnItemFetched(IDiskCacheEntryPtr pEntry)
{
	if (::CopyFile(pEntry->GetLocalPath(), m_strBadExtensionsFile, FALSE))
	{
		LoadBadExtensionsConfig();
	}
}


void CFileTransferFilter::OnItemFetchError(const CString& strUrl, DWORD dwError)
{
}


CString CFileTransferFilter::GetConfigDir()
{
	CString strDir;
	CString strKey = "uolmessenger.general.configdir";

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


void CFileTransferFilter::LoadBadExtensionsConfig()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	std::fstream file;

	file.open(m_strBadExtensionsFile, std::ios_base::in);

	if (file.is_open())
	{
		std::string line;	

		while (std::getline(file, line))
		{
			CString strExtension = line.c_str();

			if (strExtension.IsEmpty())
			{
				continue;
			}

			strExtension = strExtension.MakeLower();

			{
				CComCritSecLock<CComAutoCriticalSection> lock(m_csBadExtensions);

				POSITION pos = m_listBadExtensions.Find(strExtension);

				if (!pos)
				{
					m_listBadExtensions.AddTail(strExtension);
				}
			}
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"file.is_open() failed. m_strBadExtensionsFile = [%s]", m_strBadExtensionsFile);
	}

	file.close();	
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

}


void CFileTransferFilter::UpdateBadExtensionsConfig()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strUrl;
	strUrl.LoadString(IDS_URL_FILE_TRANSFER_FILTER);

	IDiskCacheManagerPtr pDiskCacheManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetDiskCacheManager();

	if (pDiskCacheManager)
	{
		DWORD dwError = pDiskCacheManager->FetchUrl(strUrl, this);
		ATLASSERT(dwError == IDiskCacheManager::DCME_ERROR_SUCCESS);
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "pDiskCacheManager == NULL");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}