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
#include "ThemeManager.h"

//#include <MinDependencySpecialFolder.h>
//#include <directoryenumerator.h>
//#include "../resource.h"
//#include "UIMApplication.h"
//#include <unzip.h>
//#include <zip.h>


CThemeManager::CThemeManager(IUOLMessengerCommonSettingsPtr pCommonSettings)
{
	//m_pCommonSettings = pCommonSettings;

	//CMinDependencySpecialFolder		minDSF;
	//CString							strProduct;

	//strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	//m_strSelectedThemeTempFolder.Format("%s%s", minDSF.GetTemp(), strProduct);
	//m_strSelectedTheme = GetDefaultThemePath();

	//m_pAppTheme = new CThemeSettings(m_strSelectedTheme);
	//m_bThemeChanged = FALSE;
}


CThemeManager::~CThemeManager()
{
}


//IUOLMessengerThemePtr CThemeManager::GetCurrentTheme()
//{
//	return m_pAppTheme->GetDefaultTheme();
//}


IUOLMessengerThemeManager::EnLoadTheme CThemeManager::LoadTheme()
{
	return LT_DEFAULT_THEME_INVALID;
/** Deprecated function
	if (!InitTheme() || !m_pAppTheme->Load())
	{
		SetDefaultTheme();

		if (!InitTheme() || !m_pAppTheme->Load())
		{
			return LT_DEFAULT_THEME_INVALID;
		}
		else
		{
			return LT_CURRENT_THEME_INVALID;
		}
	}

	return LT_CURRENT_THEME_OK;
*/
}


BOOL CThemeManager::SaveTheme()
{
	/** Deprecated function
	BOOL bSaveReturn;

	bSaveReturn =  m_pAppTheme->Save();

	if (bSaveReturn)
		if (m_bThemeChanged)
			this->ZipFromTemp();

	return bSaveReturn;
	*/

	return TRUE;
}


BOOL CThemeManager::InitTheme()
{
	/** Deprecated function
	if (UnzipToTemp())
	{
		m_pAppTheme->SetThemeFolder(m_strSelectedThemeTempFolder);

		return TRUE;
	}
	*/

	return FALSE;
}


IUOLMessengerSoundSettingsPtr CThemeManager::GetSoundSettings() const
{
	/** Deprecated function
	return m_pAppTheme->GetSoundSettings();
	*/

	return IUOLMessengerSoundSettingsPtr();
}


IUOLMessengerEmoticonSettingsPtr CThemeManager::GetEmoticonSettings() const
{
	/** Deprecated function
	return m_pAppTheme->GetEmoticonSettings();
	*/

	return IUOLMessengerEmoticonSettingsPtr();
}


void CThemeManager::GetThemeList(CAtlMap <CString, CString>& mapTheme)
{
	/** Deprecated function
	CMinDependencySpecialFolder	minDSF;

	CString						strCompany;
	CString						strProduct;
	CString						strTheme;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strTheme.LoadString(IDS_STRING_THEME_DIR);

	CString						strGlobalDir;
	CString						strLocalDir;

	strGlobalDir.Format("%s%s\\%s\\%s", minDSF.GetCommonAppData(), strCompany, strProduct, strTheme);
//	strLocalDir.Format("%s%s\\%s\\%s", minDSF.GetAppData(), strCompany, strProduct, strTheme);

	CString strProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();
	CString strPath;

	IUOLMessengerProfileElementPtr pProfileElement;
	CUIMApplication::GetApplication()->GetProfileManager()->GetProfileElement(strProfile, pProfileElement);
	strPath = pProfileElement->GetProfilePath();

	strLocalDir.Format("%s\\%s", strPath, strTheme);

	//Retrieve Local (changed) Themes
	CAtlList<CString> listFiles;
	CDirectoryEnumerator::GetFiles(listFiles, "*.zip", strLocalDir);	
	for (POSITION pos = listFiles.GetHeadPosition(); pos; )
	{

		if(!mapTheme.Lookup(listFiles.GetAt(pos)))
			mapTheme.SetAt(listFiles.GetAt(pos), strLocalDir);

		listFiles.GetNext(pos);
	}

	//Retrieve Global Themes
	listFiles.RemoveAll();
	CDirectoryEnumerator::GetFiles(listFiles, "*.zip", strGlobalDir);	
	for (POSITION pos = listFiles.GetHeadPosition(); pos; )
	{
		if(!mapTheme.Lookup(listFiles.GetAt(pos)))
			mapTheme.SetAt(listFiles.GetAt(pos), strGlobalDir);

		listFiles.GetNext(pos);
	}
	*/
}

BOOL CThemeManager::ImportFile(int nDestFolder, CString strSourcePath, CString & strRelativeDestPath)
{
	return FALSE;

	/** Deprecated function
	strRelativeDestPath = "";

	CString strDestFolder = "";

	if(CThemeManager::IF_SOUNDS == nDestFolder)
		strDestFolder.LoadString(IDS_FOLDER_SOUNDS);
	else if(CThemeManager::IF_EMOTICONS == nDestFolder)
		strDestFolder.LoadString(IDS_FOLDER_EMOTICONS);
	else if(CThemeManager::IF_ICONS == nDestFolder)
		strDestFolder.LoadString(IDS_FOLDER_ICONS);

	// Verify if nDestFolder is valid
	if (!(strDestFolder.GetLength() > 0))
		return FALSE;
	else
	{
		// Verify if file is on the right folder, else import the file.
		if (strSourcePath.Find(m_strSelectedThemeTempFolder + "\\" + strDestFolder) >= 0)
		{
			strRelativeDestPath = strDestFolder + "\\" + PathFindFileName(strSourcePath);
			return TRUE;
		}
		else
		{
			// Verify if file to be imported exists, if exists prompt for overwrite or cancel
			if (PathFileExists(m_strSelectedThemeTempFolder + "\\" + strDestFolder + "\\" + PathFindFileName(strSourcePath)))
			{
				CString strMessage, strCaption;

				UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
				strCaption = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
				strMessage = pStringLoader->GetValue("uol.resource.IDS_THEME_MANAGER_WARNING_FILE_EXISTS");
				if (IDNO == MessageBox(NULL, strMessage, strCaption, MB_YESNO | MB_ICONQUESTION))
					return FALSE;
			}

			if (CopyFile(strSourcePath, m_strSelectedThemeTempFolder + "\\" + strDestFolder + "\\" + PathFindFileName(strSourcePath), FALSE))
			{
				strRelativeDestPath = strDestFolder + "\\" + PathFindFileName(strSourcePath);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	*/
}


CString CThemeManager::GetSelectedThemeFolder()
{
	/** Deprecated function
	return m_strSelectedThemeTempFolder;
	*/

	return _T("");
}


CString CThemeManager::GetSelectedThemePath()
{
	/** Deprecated function
	return GetSelectedThemeFolder() + _T("\\skin.xml");
	*/

	return _T("");
}


void CThemeManager::SetDefaultTheme()
{
	/** Deprecated function
	m_strSelectedTheme = GetDefaultThemePath();
	
	//CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetSelectedTheme(m_strSelectedTheme);
	*/
}


void CThemeManager::SetThemeChanged(BOOL bThemeChanged)
{
	/** Deprecated function
	m_bThemeChanged = bThemeChanged;
	*/
}


BOOL CThemeManager::IsThemeChanged()
{
	/** Deprecated function
	return m_bThemeChanged;
	*/

	return FALSE;
}


/** Deprecated function
CString CThemeManager::GetDefaultThemePath()
{
	CString strDefaultThemePath;

	CMinDependencySpecialFolder		minDSF;
	CString						strCompany;
	CString						strProduct;
	CString						strThemesDir;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strThemesDir.LoadString(IDS_STRING_THEME_DIR);

	strDefaultThemePath.Format(_T("%s%s\\%s\\%s\\%s.zip"), 
			minDSF.GetCommonAppData(), 
			strCompany, strProduct, 
			strThemesDir, m_pCommonSettings->GetDefaultTheme());

	return strDefaultThemePath;
}
*/


/** Deprecated function
BOOL CThemeManager::UnzipToTemp()
{
	RemoveTempThemeFolder();

	HZIP hz = OpenZip(m_strSelectedTheme,0);
	
	if(hz)
	{
		ZIPENTRY ze;
		GetZipItem(hz,-1,&ze);

		SetUnzipBaseDir(hz, m_strSelectedThemeTempFolder);

		int nCountZip = ze.index;

		for (int i=0; i<nCountZip; i++)
		{
			GetZipItem(hz,i,&ze);
			UnzipItem(hz,i,ze.name);
		}

		CloseZip(hz);

		return TRUE;
	}

	return FALSE;

}
*/

/** Deprecated function
BOOL CThemeManager::ZipFromTemp()
{	
	CMinDependencySpecialFolder	minDSF;

	CString						strCompany;
	CString						strProduct;
	CString						strTheme;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strTheme.LoadString(IDS_STRING_THEME_DIR);

	CString						strLocalDir;
	CString						strGlobalDir;

	strGlobalDir.Format("%s%s\\%s\\%s", minDSF.GetCommonAppData(), strCompany, strProduct, strTheme);

	CString strProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();
	CString strPath;

	IUOLMessengerProfileElementPtr pProfileElement;
	CUIMApplication::GetApplication()->GetProfileManager()->GetProfileElement(strProfile, pProfileElement);
	strPath = pProfileElement->GetProfilePath();

	strLocalDir.Format("%s\\%s", strPath, strTheme);

	if (!PathFileExists(strLocalDir))
		CreateDirectory(strLocalDir, NULL);

	CAtlList<CString> listThemeAllFiles;
	ListThemeFolder(m_strSelectedThemeTempFolder, listThemeAllFiles);

	if (listThemeAllFiles.GetCount() > 0)
	{
		if(m_strSelectedTheme.Find(strGlobalDir) == 0 )
		{
			int nIndexExtension = m_strSelectedTheme.ReverseFind('.');
			m_strSelectedTheme.Insert(nIndexExtension, " (" + strProfile + ")");
		}

		CString strThemeFileName = strLocalDir + '\\' + PathFindFileName(m_strSelectedTheme);
		HZIP hz = CreateZip(strThemeFileName, 0);

		for (POSITION pos = listThemeAllFiles.GetHeadPosition(); pos; )
		{
			CString strFileSrc = listThemeAllFiles.GetNext(pos);
			CString strFileDst = strFileSrc;

			strFileDst.Delete(0, m_strSelectedThemeTempFolder.GetLength() + 1);

			ZipAdd(hz,strFileDst, strFileSrc);		
		}

		CloseZip(hz);

		CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetSelectedTheme(strThemeFileName);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
*/


/** Deprecated function
BOOL CThemeManager::RemoveTempThemeFolder()
{
	MAKEAUTOPTR(TCHAR);
	int nBufferSize = m_strSelectedThemeTempFolder.GetLength() + 2;

	
	TCHARPtr pBuffer(new TCHAR[nBufferSize]);
	::ZeroMemory(pBuffer.get(), nBufferSize);

	::_tcsncpy(pBuffer.get(), (LPCTSTR)m_strSelectedThemeTempFolder, nBufferSize - 1);

	SHFILEOPSTRUCT shFileOp;
	::ZeroMemory(&shFileOp, sizeof(SHFILEOPSTRUCT));
	shFileOp.hwnd = NULL;
	shFileOp.wFunc = FO_DELETE;
	shFileOp.pFrom = pBuffer.get();
	shFileOp.pTo = NULL;
	shFileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
	
	if (SHFileOperation(&shFileOp) == 0)
	{
		return TRUE;
	}

	return FALSE;
}
*/


/** Deprecated function
void CThemeManager::ListThemeFolder(const CString& strFolder, CAtlList <CString>& listThemeAllFiles)
{
	CAtlList<CString> listThemeFolders;
	CDirectoryEnumerator::GetDirectories(listThemeFolders, _T("*.*"), strFolder);	

	for (POSITION pos = listThemeFolders.GetHeadPosition(); pos; )
	{
		CString dir = listThemeFolders.GetNext(pos);

		ListThemeFolder(strFolder + '\\' + dir, listThemeAllFiles); 
	}
	
	CAtlList<CString> listThemeFiles;
	CDirectoryEnumerator::GetFiles(listThemeFiles, _T("*.*"), strFolder);	

	for (POSITION pos = listThemeFiles.GetHeadPosition(); pos; )
	{
		CString strFile = strFolder + '\\' + listThemeFiles.GetNext(pos);

		listThemeAllFiles.AddHead(strFile);

		ATLTRACE("%s\n", strFile);
	}
}
*/