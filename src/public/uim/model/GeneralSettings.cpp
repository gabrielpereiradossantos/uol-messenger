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

#include "GeneralSettings.h"
#include "SettingsTraits.h"
#include "MinDependencySpecialFolder.h"
#include "../resource.h"

#include "../controller/UIMApplication.h"


CGeneralSettings::CGeneralSettings() :
	m_bOpenBrowser(FALSE),
	m_bAutoStartup(TRUE),
	m_bSound(TRUE), 
	m_bMuteWhileAway(FALSE), 
	m_nCordX(400),
	m_nCordY(100),
	m_nSizeX(216),
	m_nSizeY(554),
	m_nUIMVersion(INSTALL_SEQUENCE),
	m_strLastAvatarFolder(""),
	m_bDontShowHideAppWarning(FALSE),
	m_bAutoReconnect(TRUE),
	m_nListSplitterPosition(-1)
{
	CMinDependencySpecialFolder	minDSF;

	CString						strCompany;
	CString						strProduct;
	CString						strTheme;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strTheme.LoadString(IDS_STRING_THEME_DIR);

	m_strDefaultTheme.Format("%s%s\\%s\\%s\\%s", minDSF.GetCommonAppData(), strCompany, strProduct, strTheme, "Default.zip");

	m_strSelectedTheme = m_strDefaultTheme;

	m_settingEventSource.SetEventSource(this);

	InitTagProcessMap();
}

CGeneralSettings::~CGeneralSettings(void)
{
}

BOOL CGeneralSettings::IsEnableSound()
{
	return m_bSound;
}

BOOL CGeneralSettings::IsMuteWhileAwayEnabled()
{
	return m_bMuteWhileAway;
}

void CGeneralSettings::EnableSound(BOOL bEnableSound)
{
	m_bSound = bEnableSound;
}

void CGeneralSettings::EnableMuteWhileAway(BOOL bEnableMuteWhileAway)
{
	m_bMuteWhileAway = bEnableMuteWhileAway;
}

void CGeneralSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"OpenBrowser", NEW_SETTINGS_CALLBACK(OpenBrowser, this)},
		{"Sound", NEW_SETTINGS_CALLBACK(Sound, this)},
		{"MuteWhileAway", NEW_SETTINGS_CALLBACK(MuteWhileAway, this)},
		{"SelectedTheme", NEW_SETTINGS_CALLBACK(SelectedTheme, this)},
		{"CordX", NEW_SETTINGS_CALLBACK(CordX, this)},
		{"CordY", NEW_SETTINGS_CALLBACK(CordY, this)},
		{"SizeX", NEW_SETTINGS_CALLBACK(SizeX, this)},
		{"SizeY", NEW_SETTINGS_CALLBACK(SizeY, this)},
		{"UIMVersion", NEW_SETTINGS_CALLBACK(UIMVersion, this)},
		{"ListSplitterPosition", NEW_SETTINGS_CALLBACK(ListSplitterPosition, this)},
		{"LastAvatarFolder", NEW_SETTINGS_CALLBACK(LastAvatarFolder, this)},
		{"DontShowHideAppWarning", NEW_SETTINGS_CALLBACK(DontShowHideAppWarning, this)},
		{"AutoReconnect", NEW_SETTINGS_CALLBACK(AutoReconnect, this)},
		{"LastSendFileFolder", NEW_SETTINGS_CALLBACK(LastSendFileFolder, this)},
		{"LastReceiveFileFolder", NEW_SETTINGS_CALLBACK(LastReceiveFileFolder, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CGeneralSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
	LoadAutoStartUp();
}

void CGeneralSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
	SaveAutoStartUp();
}

BOOL CGeneralSettings::IsAutoStartupEnabled()
{
	return m_bAutoStartup;
}

void CGeneralSettings::EnableAutoStartup(BOOL bAutoStartup)
{
	m_bAutoStartup = bAutoStartup;
	//NotifyEventListeners(IUOLMessengerGeneralSettings::CL_SHOW_OFFLINE);
}

BOOL CGeneralSettings::IsOpenBrowserEnabled()
{
	return m_bOpenBrowser;
}

void CGeneralSettings::EnableOpenBrowser(BOOL bEnable)
{
	m_bOpenBrowser = bEnable;
}

void CGeneralSettings::LoadAutoStartUp()
{
	CMinDependencySpecialFolder minFolder;
	CString strStartUpFolder;
	CString strAppName, strFilename;
	
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	strStartUpFolder = minFolder.GetStartUpFolder();
	strAppName = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
	strFilename = strStartUpFolder + strAppName + ".lnk";
	
	if (ATLPath::FileExists(strFilename))
	{
		m_bAutoStartup = TRUE;
	}
	else
	{
		m_bAutoStartup = FALSE;
	}
}

BOOL CGeneralSettings::SaveAutoStartUp()
{
	BOOL bOk = FALSE;
	CMinDependencySpecialFolder minFolder;
	CString strStartUpFolder, strWorkingDir, strPath;
	CString strAppName, strFilename;

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	strStartUpFolder = minFolder.GetStartUpFolder();
	strAppName = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
	strFilename = strStartUpFolder + strAppName + ".lnk";

	if (m_bAutoStartup)
	{
		// Create link file if it doesn't exist

		if (ATLPath::FileExists(strFilename))
		{
			bOk = TRUE;
		}
		else
		{
			char pszModulePath[MAX_PATH + 1];
			HMODULE hModule;

			hModule = _Module.GetModuleInstance();
			GetModuleFileName(hModule, pszModulePath, MAX_PATH);
			strPath.SetString(pszModulePath);
			strWorkingDir = strPath.Mid(0, strPath.ReverseFind('\\'));

			CString strAppIcon;
			int nIconIndex = 0;

			IUOLMessengerCommonSettingsPtr pCommonSettings = CUIMApplication::GetApplication()->GetCommonSettings();

			if (pCommonSettings)
			{
				pCommonSettings->GetValue(IUOLMessengerCommonSettings::APP_LINK_ICON_SETTING, strAppIcon);
				pCommonSettings->GetValue(IUOLMessengerCommonSettings::APP_LINK_ICON_INDEX_SETTING, nIconIndex);

				if (FALSE == strAppIcon.IsEmpty())
				{
					strAppIcon = strWorkingDir + _T("\\") + strAppIcon;
				}
			}

			if (SUCCEEDED(CreateLink(pszModulePath, strFilename, strAppName, strWorkingDir, 
									 ((FALSE == strAppIcon.IsEmpty()) ? (LPCTSTR) strAppIcon : NULL), nIconIndex)))
			{
				bOk = TRUE;
			}
		}
	}
	else 
	{
		// Delete link file
		if (::DeleteFile(strFilename))
		{
			bOk = TRUE;
		}
	}

	return bOk;
}

HRESULT CGeneralSettings::CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink,
                   LPCSTR lpszDesc, LPCSTR lpszWorkingDir, 
				   LPCSTR lpszIconLocation, int nIconIndex) 
{ 
	HRESULT hres; 
	IShellLink* psl; 

	// Get a pointer to the IShellLink interface. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, 
		CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl); 

	if (SUCCEEDED(hres))
	{ 
		IPersistFile* ppf; 

		// Set the path to the shortcut target and add the 
		// description. 
		psl->SetPath(lpszPathObj); 
		psl->SetDescription(lpszDesc); 
		psl->SetWorkingDirectory(lpszWorkingDir);

		if (lpszIconLocation)
		{
			psl->SetIconLocation(lpszIconLocation, nIconIndex);
		}

		// Query IShellLink for the IPersistFile interface for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf); 

		if (SUCCEEDED(hres))
		{ 
			WCHAR wsz[MAX_PATH]; 

			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, 
				wsz, MAX_PATH); 

			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(wsz, TRUE); 
			ppf->Release(); 
		} 

		psl->Release(); 
	} 

	return hres; 
} 

CString CGeneralSettings::GetSelectedTheme() const
{
	return m_strSelectedTheme;
}

void CGeneralSettings::SetSelectedTheme(const CString& strSelectedTheme)
{
	m_strSelectedTheme = strSelectedTheme;
}

CString CGeneralSettings::GetDefaultTheme() const
{
	return m_strDefaultTheme;
}

CString CGeneralSettings::GetLastAvatarFolder() const
{
	return m_strLastAvatarFolder;
}

void CGeneralSettings::SetLastAvatarFolder(const CString& strLastAvatarFolder)
{
	m_strLastAvatarFolder = strLastAvatarFolder;
}

DWORD CGeneralSettings::GetCordX()
{
	return m_nCordX;
}

void CGeneralSettings::SetCordX(DWORD nCordX)
{
	m_nCordX = nCordX;
}

DWORD CGeneralSettings::GetCordY()
{
	return m_nCordY;
}

void CGeneralSettings::SetCordY(DWORD nCordY)
{
	m_nCordY = nCordY;
}

DWORD CGeneralSettings::GetSizeX()
{
	return m_nSizeX;
}

void CGeneralSettings::SetSizeX(DWORD nSizeX)
{
	m_nSizeX = nSizeX;
}

DWORD CGeneralSettings::GetSizeY()
{
	return m_nSizeY;
}

void CGeneralSettings::SetSizeY(DWORD nSizeY)
{
	m_nSizeY = nSizeY;
}

__declspec(deprecated) DWORD CGeneralSettings::GetUIMVersion()
{
	ATLASSERT(FALSE);

	return 0xffffffff;
}

__declspec(deprecated) void CGeneralSettings::SetUIMVersion(DWORD nVersion)
{
	m_nUIMVersion = nVersion;
}


DWORD CGeneralSettings::GetListSplitterPosition()
{
	return m_nListSplitterPosition;
}

void CGeneralSettings::SetListSplitterPosition(DWORD nPosition)
{
	m_nListSplitterPosition = nPosition;
}

void CGeneralSettings::SetDontShowHideAppWarning()
{
	m_bDontShowHideAppWarning = TRUE;
}

BOOL  CGeneralSettings::GetDontShowHideAppWarning() const
{
	return m_bDontShowHideAppWarning;
}

__declspec(deprecated) BOOL CGeneralSettings::IsAutoReconnectEnabled()
{
	ATLASSERT(FALSE);

	return FALSE;
}

__declspec(deprecated) void CGeneralSettings::EnableAutoReconnect(BOOL bEnable)
{
	m_bAutoReconnect = bEnable;
}

void CGeneralSettings::SetLastSendFileFolder(const CString& strFolder)
{
	m_strLastSendFileFolder = strFolder;
}

CString CGeneralSettings::GetLastSendFileFolder() const
{
	return m_strLastSendFileFolder;
}

void CGeneralSettings::SetLastReceiveFileFolder(const CString& strFolder)
{
	m_strLastReceiveFileFolder = strFolder;
}

CString CGeneralSettings::GetLastReceiveFileFolder() const
{
	return m_strLastReceiveFileFolder;
}

void CGeneralSettings::AddEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.AddEventListener(pListener);
}
void CGeneralSettings::RemoveEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.RemoveEventListener(pListener);
}
void CGeneralSettings::NotifyEventListeners(UINT nSettingId)
{
	m_settingEventSource.NotifyEventListeners(nSettingId);
}


CString CGeneralSettings::GetPluginLibFolder() const
{
	CString strDir;
	CString strKey = "uolmessenger.general.libdir";
	
	if (FALSE == CUIMApplication::GetApplication()->GetSettingsManager()->GetValue(strKey, strDir))
	{
		ATLASSERT(FALSE);
	}

	if (strDir.Right(1) != '\\')
	{
		strDir += '\\';
	}

	return strDir;
}




///////////////////////////////////////////////////////////////////////////////
// Callbacks
IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, Sound)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bSound);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bSound);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, MuteWhileAway)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bMuteWhileAway);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bMuteWhileAway);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, OpenBrowser)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bOpenBrowser);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bOpenBrowser);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, SelectedTheme)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strSelectedTheme);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strSelectedTheme);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, LastAvatarFolder)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strLastAvatarFolder);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strLastAvatarFolder);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, CordX)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nCordX);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nCordX);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, CordY)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nCordY);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nCordY);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, SizeX)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nSizeX);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nSizeX);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, SizeY)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nSizeY);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nSizeY);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, UIMVersion)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nUIMVersion);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nUIMVersion);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, ListSplitterPosition)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nListSplitterPosition);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nListSplitterPosition);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, DontShowHideAppWarning)
{
	if (!bSave)
	{
		DWORD dw = 0;
		if (CSettingsTraits::GetDWORDValue(pNode, strKey, dw))
		{
			m_pThis->m_bDontShowHideAppWarning = dw;
		}
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_bDontShowHideAppWarning);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, AutoReconnect)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bAutoReconnect);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bAutoReconnect);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, LastSendFileFolder)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strLastSendFileFolder);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strLastSendFileFolder);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CGeneralSettings, LastReceiveFileFolder)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strLastReceiveFileFolder);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strLastReceiveFileFolder);
	}
}