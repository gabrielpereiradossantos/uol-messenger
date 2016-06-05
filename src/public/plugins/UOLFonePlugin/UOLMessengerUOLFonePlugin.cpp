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
#include "UOLMessengerUOLFonePlugin.h"
#include "AppVersion.h"
#include "FacadeWrapper.h"
#include "MessageWindowMonitor.h"
#include "ContactListMonitor.h"
#include "HistoryWindowMonitor.h"
#include "Core/UOLFoneController.h"


#define UMSDK_FILE_MAJOR		2
#define UMSDK_FILE_MINOR		0
#define UMSDK_BUILD_MAJOR		1
#define UMSDK_BUILD_MINOR		0


// {61C7A80A-D53A-4c47-AC6B-86827D716BF6}
static const GUID LIBID_PluginModuleLib = 
{ 0x61c7a80a, 0xd53a, 0x4c47, { 0xac, 0x6b, 0x86, 0x82, 0x7d, 0x71, 0x6b, 0xf6 } };


class CPluginModule : public CAtlDllModuleT<CPluginModule>
{
public :
	DECLARE_LIBID(LIBID_PluginModuleLib)
};


CPluginModule _AtlModule;




extern "C"
{
	// DLL entry points...

	// UOL Messenger will call this function to allocate new plugin
	__declspec(dllexport)
	CUOLMessengerPlugin* AllocUOLMessengerPlugin(IUOLMessengerFacade* pUOLMessengerFacade)
	{
		CUOLMessengerPlugin* pUOLMessengerPlugin = 
				new CUOLMessengerUOLFonePlugin(pUOLMessengerFacade);
		
		CFacadeWrapper::GetInstance()->Init(pUOLMessengerFacade, pUOLMessengerPlugin);
		
		return pUOLMessengerPlugin;
	}
	
	
	// UOL Messenger will call this function to free plugin
	__declspec(dllexport)
	void FreeUOLMessengerPlugin(CUOLMessengerPlugin* pUOLMessengerPlugin)
	{
		ATLASSERT(FALSE == IsBadReadPtr(pUOLMessengerPlugin, sizeof(CUOLMessengerPlugin*)));
		
		CFacadeWrapper::GetInstance()->Finalize();
		
		delete pUOLMessengerPlugin;
	}
}






CUOLMessengerUOLFonePlugin::CUOLMessengerUOLFonePlugin(IUOLMessengerFacade* pUOLMessengerFacade) : 
		CUOLMessengerPlugin(pUOLMessengerFacade)
{
	m_pUOLMessengerFacade = pUOLMessengerFacade;
	
	m_pIcon = pUOLMessengerFacade->GetUIManager()->LoadImage(_AtlBaseModule.GetModuleInstance(), IDB_UOLFONE_PLUGIN_ICON);
	
	m_pluginName.LoadString(IDS_PLUGIN_NAME);
	m_pluginDescription.LoadString(IDS_PLUGIN_DESC);
}


CUOLMessengerUOLFonePlugin::~CUOLMessengerUOLFonePlugin(void)
{
}


BOOL CUOLMessengerUOLFonePlugin::Init(ULONGLONG llUOLMessengerVersion)
{
	ULONG* pVersion = reinterpret_cast<ULONG*>(&llUOLMessengerVersion);

	ULONG lFileVersion = MAKELONG(UMSDK_FILE_MINOR, UMSDK_FILE_MAJOR);
	ULONG lBuildVersion = MAKELONG(UMSDK_BUILD_MINOR, UMSDK_BUILD_MAJOR);

	if ((pVersion[1] < lFileVersion) ||
		((pVersion[1] == lFileVersion) && (pVersion[0] < lBuildVersion)))
	{
		return FALSE;
	}

	CFacadeWrapper::GetInstance()->PluginInit();

	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();
	BOOL success = FALSE;
	
	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManager5Ptr pUIManager5 = ap_static_cast<IUOLMessengerUIManager5Ptr>(pUOLMessengerFacade->GetUIManager());
		
		if (pUIManager5)
		{
			AddCustomPanel(pUIManager5);
			AddPreferencesPage(pUIManager5);
			
			CString strUIMLibDir = pUOLMessengerFacade->GetSettingsManager()->GetGeneralSettings()->GetPluginLibFolder();
			
			success = CUOLFoneController::GetInstance()->Initialize(strUIMLibDir, CFacadeWrapper::GetInstance());
		}
	}
	
	if (success)
	{
		if ((success = CMessageWindowMonitor::GetInstance()->Init(
				CFacadeWrapper::GetFacadeInstance())) == TRUE)
		{
			if (success = CContactListMonitor::GetInstance()->Init(
				CFacadeWrapper::GetFacadeInstance()) == TRUE)
			{
				success = CHistoryWindowMonitor::GetInstance()->Init(CFacadeWrapper::GetFacadeInstance());
			}
		}
	}

	// Add Sound Events
	CFacadeWrapper::GetInstance()->AddSoundEvents();
	
	return success;
}


void CUOLMessengerUOLFonePlugin::Finalize()
{
	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManager5Ptr pUIManager5 = ap_static_cast<IUOLMessengerUIManager5Ptr>(pUOLMessengerFacade->GetUIManager());
		
		if (pUIManager5)
		{
			//RemoveCustomListEntries(pUIManager2);
			RemoveCustomPanel(pUIManager5);
			RemovePreferencesPage(pUIManager5);

			if (m_pPrefsPage.get())
			{
				HWND hWndPrefsDlg = m_pPrefsPage->GetHWND();

				if (::IsWindow(hWndPrefsDlg))
				{
					pUIManager5->RemovePreferencesPage(hWndPrefsDlg);

					::DestroyWindow(hWndPrefsDlg);
				}
			}

			// Remove Sound Events
			CFacadeWrapper::GetInstance()->RemoveSoundEvents();
		}
	}

	CUOLFoneController::GetInstance()->Finalize();
	CMessageWindowMonitor::GetInstance()->Finalize();
	CContactListMonitor::GetInstance()->Finalize();
	CHistoryWindowMonitor::GetInstance()->Finalize();
	CFacadeWrapper::GetInstance()->PluginFinalize();
	
	// Clear the plugin properties published on main application.
	// """" MUST """" be done here or we will have a crash.
	//
	IUOLMessengerFacade4* pFacade4 = 
			dynamic_cast<IUOLMessengerFacade4*>(
					CFacadeWrapper::GetInstance()->GetFacadeInstance());
	
	IUOLMessengerPluginPropertiesRegisterPtr pPluginPropertiesRegister;
	
	if (pFacade4 != NULL)
	{
		pPluginPropertiesRegister = 
				pFacade4->GetPluginPropertiesRegister();
	}
	
	if (pPluginPropertiesRegister != NULL)
	{
		pPluginPropertiesRegister->UnregisterAllPluginProperties(PLUGIN_INTERNAL_NAME);
	}
}


CString CUOLMessengerUOLFonePlugin::GetName() const
{
	return m_pluginName;
}


ULONGLONG CUOLMessengerUOLFonePlugin::GetVersion() const
{
	return CAppVersion(ATL::_AtlBaseModule.GetModuleInstance()).GetVersion();
}


CString CUOLMessengerUOLFonePlugin::GetInternalName() const
{
	return PLUGIN_INTERNAL_NAME;
}


CString CUOLMessengerUOLFonePlugin::GetDesc() const
{
	return m_pluginDescription;
}


IUOLMessengerImagePtr CUOLMessengerUOLFonePlugin::GetIcon() const
{
	return m_pIcon;
}


BOOL CUOLMessengerUOLFonePlugin::HasConfigDialog()
{
	return FALSE;
}


void CUOLMessengerUOLFonePlugin::ShowConfigDialog()
{
	IUOLFoneControllerCallback::InitStatusCode initStatusCode = 
		CFacadeWrapper::GetInstance()->GetInitStatusCode();
	
	if ( IUOLFoneControllerCallback::InitStatusCodeLibraryInitializationError == initStatusCode ||
		 IUOLFoneControllerCallback::InitStatusCodeLibraryDownloadError		  == initStatusCode )
	{
		CString strCaption;
		CString strMessage;
		strCaption.LoadString(IDS_PLUGIN_NAME);
		strMessage.LoadString(IDS_ERROR_PLUGIN_NOT_INITIALIZED);

		IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();
        ap_static_cast<IUOLMessengerUIManager4Ptr>(pUOLMessengerFacade->GetUIManager())->ShowMessageBox(NULL, strMessage, strCaption, MB_OK | MB_ICONERROR);
	}
	else
	{
		IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();
		IUOLMessengerUIManager3Ptr pUIManager3 = ap_static_cast<IUOLMessengerUIManager3Ptr>(pUOLMessengerFacade->GetUIManager());

		if ( pUIManager3.get() )
		{
			CString strTitle;
			strTitle.LoadString(IDS_PLUGIN_NAME);
			
			pUIManager3->ShowPreferences(strTitle);
		}
	}
}


void CUOLMessengerUOLFonePlugin::AddPreferencesPage(IUOLMessengerUIManager5Ptr pUIManager5)
{
	IUOLMessengerImagePtr pImage;
	pImage = pUIManager5->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_PREFS_TOOLBAR_BTN_UOLFONE);

	IUOLMessengerPluginPreferencesPageContainerPtr pPluginPreferencesPageContainer = pUIManager5->GetPluginPreferencesPageContainer();
	
	if (pPluginPreferencesPageContainer)
	{
		m_pPrefsPage = new CUOLFoneConfigDlg(GetName(), GetInternalName(), pImage);
		if (m_pPrefsPage)
		{
			m_pPrefsPage->SetTabButtonIndex(
					CFacadeWrapper::GetInstance()->AddTabButton());
			
			BOOL bAdd = pPluginPreferencesPageContainer->AddPage(GetName(), m_pPrefsPage, pImage);
			ATLASSERT(bAdd);
		}
	}
}


void CUOLMessengerUOLFonePlugin::RemovePreferencesPage(IUOLMessengerUIManager5Ptr pUIManager5)
{
	IUOLMessengerPluginPreferencesPageContainerPtr pPluginPreferencesPageContainer = pUIManager5->GetPluginPreferencesPageContainer();

	if (pPluginPreferencesPageContainer && m_pPrefsPage)
	{
		pPluginPreferencesPageContainer->RemovePage(GetName());
	}
}


void CUOLMessengerUOLFonePlugin::AddCustomListEntries(IUOLMessengerUIManager5Ptr pUIManager5)
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = pUIManager5->GetPluginCustomList();
	
	if (pPluginCustomList)
	{
		//m_pListEntry = new CRadioListEntry(_T("SampleListEntry"), GetInternalName());
		
		BOOL bAdd = pPluginCustomList->AddEntry(m_pListEntry);
		ATLASSERT(bAdd);
	}
}


void CUOLMessengerUOLFonePlugin::RemoveCustomListEntries(IUOLMessengerUIManager5Ptr pUIManager5)
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = pUIManager5->GetPluginCustomList();
	
	if (pPluginCustomList && m_pListEntry)
	{
		pPluginCustomList->RemoveEntry(m_pListEntry->GetEntryName(), m_pListEntry->GetPluginInternalName());
	}
}


void CUOLMessengerUOLFonePlugin::AddCustomPanel(IUOLMessengerUIManager5Ptr pUIManager5)
{
	IUOLMessengerPluginCustomTabContainerPtr pPluginCustomTabContainer = pUIManager5->GetPluginCustomTabContainer();
	
	if (pPluginCustomTabContainer)
	{
		m_pTab = new CUOLFoneWindow(GetName(), GetInternalName());
		
		if (m_pTab)
		{
			m_pTab->SetTabButtonIndex(
					CFacadeWrapper::GetInstance()->AddTabButton());
			
			BOOL bAdd = pPluginCustomTabContainer->AddTab(m_pTab);
			ATLASSERT(bAdd);
		}
	}
}


void CUOLMessengerUOLFonePlugin::RemoveCustomPanel(IUOLMessengerUIManager5Ptr pUIManager5)
{
	IUOLMessengerPluginCustomTabContainerPtr pPluginCustomTabContainer = pUIManager5->GetPluginCustomTabContainer();
	
	if (pPluginCustomTabContainer && m_pTab)
	{
		// Firstly we remove the UOLFone tab button.
		CFacadeWrapper::GetInstance()->RemoveTabButton();
		
		// After, we remove the UOLFone panel.
		pPluginCustomTabContainer->RemoveTab(m_pTab->GetTabName(), m_pTab->GetPluginInternalName());
	}
}
