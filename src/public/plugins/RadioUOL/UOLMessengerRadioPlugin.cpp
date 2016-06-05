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
#include ".\uolmessengerRadioplugin.h"
#include "AppVersion.h"
#include "FacadeWrapper.h"
#include "RadioListEntry.h"
//#include "RadioPanel.h"
#include "core/RadioUOLWindow.h"
#include "core/RadioUOLWrapper.h"


// {3DFEE62B-828E-4fad-AB9B-F6C48A960237}
static const GUID LIBID_PluingModuleLib = 
{ 0x3dfee62b, 0x828e, 0x4fad, { 0xab, 0x9b, 0xf6, 0xc4, 0x8a, 0x96, 0x2, 0x37 } };


class CPluingModule : public CAtlDllModuleT<CPluingModule>
{
public :
	DECLARE_LIBID(LIBID_PluingModuleLib)
};


CPluingModule _AtlModule;

extern "C"
{
	// DLL entry points...

	// UOL Messenger will call this function to allocate new plugin
	__declspec(dllexport)
	CUOLMessengerPlugin* AllocUOLMessengerPlugin(IUOLMessengerFacade* pUOLMessengerFacade)
	{
		CUOLMessengerPlugin* pRadioUOLPlugin = 
				new CUOLMessengerRadioPlugin(pUOLMessengerFacade);
		
		CFacadeWrapper::GetInstance()->Init(pUOLMessengerFacade, pRadioUOLPlugin);
		
		return pRadioUOLPlugin;
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






CUOLMessengerRadioPlugin::CUOLMessengerRadioPlugin(IUOLMessengerFacade* pUOLMessengerFacade) : 
	CUOLMessengerPlugin(pUOLMessengerFacade)
{
	m_pUOLMessengerFacade = pUOLMessengerFacade;

	//IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();

	m_pIcon = pUOLMessengerFacade->GetUIManager()->LoadImage(_AtlBaseModule.GetModuleInstance(), IDB_RADIOUOL_PLUGIN);
}


CUOLMessengerRadioPlugin::~CUOLMessengerRadioPlugin(void)
{
}


BOOL CUOLMessengerRadioPlugin::Init(ULONGLONG llUOLMessengerVersion)
{
	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();

	int nError;

	if ((nError = m_config.LoadConfig())!=0)
	{
		CRadioUOLError error(nError);
		return FALSE;
	}

	// Check WMP version
	if (!CheckWMPVersion())
	{
		CRadioUOLError error(CRadioUOLError::INCOMPATIBLE_WMP_VWESION);
		return FALSE;
	}

	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager = pUOLMessengerFacade->GetUIManager();

		if (pUIManager)
		{
			//AddCustomListEntries(pUIManager);
			AddCustomPanel(pUIManager);
		}
	}

	return TRUE;
}


void CUOLMessengerRadioPlugin::Finalize()
{
	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();

	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager = pUOLMessengerFacade->GetUIManager();

		if (pUIManager)
		{
			//RemoveCustomListEntries(pUIManager);
			RemoveCustomPanel(pUIManager);
			
			CFacadeWrapper::GetInstance()->ClearRadioSystrayMenu();
		}
	}
}


CString CUOLMessengerRadioPlugin::GetName() const
{
	return _T("Rádio UOL");
}


ULONGLONG CUOLMessengerRadioPlugin::GetVersion() const
{
	return CAppVersion(ATL::_AtlBaseModule.GetModuleInstance()).GetVersion();
}


CString CUOLMessengerRadioPlugin::GetInternalName() const
{
	return PLUGIN_INTERNAL_NAME;
}


CString CUOLMessengerRadioPlugin::GetDesc() const
{
	CString strText;
	strText.LoadString(IDS_PLUGIN_DESC);
	return strText;
}


IUOLMessengerImagePtr CUOLMessengerRadioPlugin::GetIcon() const
{
	return m_pIcon;
}


BOOL CUOLMessengerRadioPlugin::HasConfigDialog()
{
	return FALSE;
}


void CUOLMessengerRadioPlugin::ShowConfigDialog()
{
}


void CUOLMessengerRadioPlugin::AddCustomListEntries(IUOLMessengerUIManagerPtr pUIManager)
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = pUIManager->GetPluginCustomList();

	if (pPluginCustomList)
	{
		m_pListEntry = new CRadioListEntry(_T("SampleListEntry"), GetInternalName());

		BOOL bAdd = pPluginCustomList->AddEntry(m_pListEntry);
		ATLASSERT(bAdd);
	}
}


void CUOLMessengerRadioPlugin::RemoveCustomListEntries(IUOLMessengerUIManagerPtr pUIManager)
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = pUIManager->GetPluginCustomList();

	if (pPluginCustomList && m_pListEntry)
	{
		pPluginCustomList->RemoveEntry(m_pListEntry->GetEntryName(), m_pListEntry->GetPluginInternalName());
	}
}


void CUOLMessengerRadioPlugin::AddCustomPanel(IUOLMessengerUIManagerPtr pUIManager)
{
	IUOLMessengerPluginCustomPanelContainerPtr pPluginCustomPanelContainer = pUIManager->GetPluginCustomPanelContainer();

	if (pPluginCustomPanelContainer)
	{
		m_pPanel = new CRadioUOLWindow(_T("Rádio UOL"), GetInternalName());
		m_pPanel->SetConfig(&m_config);
		CRadioUOLWrapper::GetInstance()->Init(m_pPanel);

		BOOL bAdd = pPluginCustomPanelContainer->AddPanel(m_pPanel);
		ATLASSERT(bAdd);
	}
}


void CUOLMessengerRadioPlugin::RemoveCustomPanel(IUOLMessengerUIManagerPtr pUIManager)
{
	IUOLMessengerPluginCustomPanelContainerPtr pPluginCustomPanelContainer = pUIManager->GetPluginCustomPanelContainer();

	if (pPluginCustomPanelContainer && m_pPanel)
	{
		pPluginCustomPanelContainer->RemovePanel(m_pPanel->GetPanelName(), m_pPanel->GetPluginInternalName());
		CRadioUOLWrapper::GetInstance()->Finalize();
	}
}


BOOL CUOLMessengerRadioPlugin::CheckWMPVersion()
{
	CRegKey regKey;

	if (ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\MediaPlayer\\PlayerUpgrade"), KEY_READ))
	{
		TCHAR szValue[MAX_PATH + 1] = {0};
		ULONG nChars = MAX_PATH;

		if (ERROR_SUCCESS == regKey.QueryStringValue(_T("PlayerVersion"), szValue, &nChars))
		{
			LONG nMajorVersion = ::_ttol(szValue);

			if (nMajorVersion >= 9)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}