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
#include ".\uolmessengersampleplugin.h"

#include "FacadeWrapper.h"
#include "SampleListEntry.h"
#include "SamplePanel.h"
#include "AppVersion.h"


extern "C"
{
	// DLL entry points...

	// UOL Messenger will call this function to allocate new sample plugin
	__declspec(dllexport)
		CUOLMessengerPlugin* AllocUOLMessengerPlugin(IUOLMessengerFacade* pUOLMessengerFacade)
	{
		CFacadeWrapper::GetInstance()->Init(pUOLMessengerFacade);

		return new CUOLMessengerSamplePlugin(pUOLMessengerFacade);
	}


	// UOL Messenger will call this function to free sample plugin
	__declspec(dllexport)
		void FreeUOLMessengerPlugin(CUOLMessengerPlugin* pUOLMessengerPlugin)
	{
		ATLASSERT(FALSE == IsBadReadPtr(pUOLMessengerPlugin, sizeof(CUOLMessengerPlugin*)));

		CFacadeWrapper::GetInstance()->Finalize();

		delete pUOLMessengerPlugin;
	}
}






CUOLMessengerSamplePlugin::CUOLMessengerSamplePlugin(IUOLMessengerFacade* pUOLMessengerFacade) : 
CUOLMessengerPlugin(pUOLMessengerFacade)
{
	m_pUOLMessengerFacade = pUOLMessengerFacade;
}


CUOLMessengerSamplePlugin::~CUOLMessengerSamplePlugin(void)
{
}


BOOL CUOLMessengerSamplePlugin::Init(ULONGLONG llUOLMessengerVersion)
{
	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();

	if (pUOLMessengerFacade)
	{
		m_pUIManager = ap_static_cast<IUOLMessengerUIManager2Ptr>(pUOLMessengerFacade->GetUIManager());

		if (m_pUIManager)
		{
			m_pContactListCtrl = m_pUIManager->GetContactListCtrl();
			m_pContactListCtrl->RegisterObserver(this);

			//AddCustomListEntries();
			AddCustomPanel();
			AddCustomButtons();
		}
	}

	return TRUE;
}


void CUOLMessengerSamplePlugin::Finalize()
{
	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();

	if (pUOLMessengerFacade)
	{
		if (m_pUIManager)
		{
			//RemoveCustomListEntries();
			RemoveCustomPanel();
		}
	}

	if (m_pContactListCtrl)
	{
		m_pContactListCtrl->UnregisterObserver(this);
	}
}


CString CUOLMessengerSamplePlugin::GetName() const
{
	return "Custom Plugin";
}


ULONGLONG CUOLMessengerSamplePlugin::GetVersion() const
{
	return CAppVersion(ATL::_AtlBaseModule.GetModuleInstance()).GetVersion();
}


CString CUOLMessengerSamplePlugin::GetInternalName() const
{
	return PLUGIN_INTERNAL_NAME;
}


CString CUOLMessengerSamplePlugin::GetDesc() const
{
	return "Sample plugin";
}


IUOLMessengerImagePtr CUOLMessengerSamplePlugin::GetIcon() const
{
	return IUOLMessengerImagePtr();
}


BOOL CUOLMessengerSamplePlugin::HasConfigDialog()
{
	return TRUE;
}


void CUOLMessengerSamplePlugin::ShowConfigDialog()
{
}


void CUOLMessengerSamplePlugin::AddCustomListEntries()
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = m_pUIManager->GetPluginCustomList();

	if (pPluginCustomList)
	{
		m_pListEntry = new CSampleListEntry(_T("SampleListEntry"), GetInternalName());

		BOOL bAdd = pPluginCustomList->AddEntry(m_pListEntry);
		ATLASSERT(bAdd);
	}
}


void CUOLMessengerSamplePlugin::RemoveCustomListEntries()
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = m_pUIManager->GetPluginCustomList();

	if (pPluginCustomList && m_pListEntry)
	{
		pPluginCustomList->RemoveEntry(m_pListEntry->GetEntryName(), m_pListEntry->GetPluginInternalName());
	}
}


void CUOLMessengerSamplePlugin::AddCustomPanel()
{
	IUOLMessengerPluginCustomPanelContainerPtr pPluginCustomPanelContainer = m_pUIManager->GetPluginCustomPanelContainer();

	if (pPluginCustomPanelContainer)
	{
		m_pPanel = new CSamplePanel(_T("SamplePanel"), GetInternalName());

		BOOL bAdd = pPluginCustomPanelContainer->AddPanel(m_pPanel);
		ATLASSERT(bAdd);
	}
}


void CUOLMessengerSamplePlugin::RemoveCustomPanel()
{
	IUOLMessengerPluginCustomPanelContainerPtr pPluginCustomPanelContainer = m_pUIManager->GetPluginCustomPanelContainer();

	if (pPluginCustomPanelContainer && m_pPanel)
	{
		pPluginCustomPanelContainer->RemovePanel(m_pPanel->GetPanelName(), m_pPanel->GetPluginInternalName());
	}
}

void CUOLMessengerSamplePlugin::OnAddContact(IUOLMessengerContactNodePtr pContact)
{
	// TODO: craete a m_pUIManager
	//if (pContact)
	//{
	//	IUOLMessengerImagePtr pImage = pUIManager->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_SAMPLE_CONTACT_BUTTON);
	//	IUOLMessengerImageButton2Ptr pImageButton = m_pContactListCtrl->CreateContactButton();

	//	if (pImageButton)
	//	{
	//		pImageButton->SetImage(pImage);
	//		pImageButton->SetWidth(16);
	//		pImageButton->SetAuto(FALSE);
	//		pImageButton->SetDrawTransparent(TRUE);

	//		pContact->AddCustomButton(pImageButton);
	//	}
	//}
}

void CUOLMessengerSamplePlugin::OnRemoveContact(IUOLMessengerContactNodePtr pContact)
{
}

void CUOLMessengerSamplePlugin::OnRemoveContacts(CAtlList<IUOLMessengerContactNodePtr>& listContacts)
{
}

void CUOLMessengerSamplePlugin::AddCustomButtons()
{
	CAtlList<IUOLMessengerContactNodePtr> listContacts;

	m_pContactListCtrl->GetContacts(listContacts);

	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactNodePtr pContact = ap_dynamic_cast<IUOLMessengerContactNodePtr>(listContacts.GetNext(pos));

		if (pContact)
		{
			IUOLMessengerImagePtr pImage = m_pUIManager->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_SAMPLE_CONTACT_BUTTON);
			IUOLMessengerImageButton2Ptr pImageButton = m_pContactListCtrl->CreateContactButton();

			if (pImageButton)
			{
				pImageButton->SetImage(pImage);
				pImageButton->SetWidth(16);
				pImageButton->SetAuto(FALSE);
				pImageButton->SetDrawTransparent(TRUE);

				pContact->AddCustomButton(pImageButton);
			}
		}
	}
}

void CUOLMessengerSamplePlugin::RemoveCustomButtons()
{
}