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
#include "./ContactListMonitor.h"

#include "./resource.h"
#include "./FacadeWrapper.h"
#include "Core/UOLFoneController.h"


CContactListMonitor::CContactListMonitor(void) : 
		m_pUOLMessengerFacade(NULL), 
		m_pContactListCtrl(NULL), 
		m_pUOLFoneButtonImage(NULL)
{
	m_menuItemLabel.LoadString(IDS_MENU_LABEL_CHAMAR_CONTATO);
}


CContactListMonitor::~CContactListMonitor(void)
{
}


CContactListMonitor* CContactListMonitor::GetInstance()
{
	static CContactListMonitor contactListMonitor;
	
	return &contactListMonitor;
}


BOOL CContactListMonitor::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManager2Ptr pUIManager2 = 
				ap_static_cast<IUOLMessengerUIManager2Ptr>(pUOLMessengerFacade->GetUIManager());
		
		if (pUIManager2)
		{
			m_pContactListCtrl = pUIManager2->GetContactListCtrl();
			m_pContactListCtrl->RegisterObserver(this);
			
			m_pUOLFoneButtonImage = pUIManager2->LoadImage(
					ATL::_AtlBaseModule.GetResourceInstance(), 
					IDB_BTN_LISTA_CONTATOS);
			
			AddCustomButtons();
			
			m_pUOLMessengerFacade = pUOLMessengerFacade;
			
			return TRUE;
		}
	}
	
	return FALSE;
}


void CContactListMonitor::Finalize()
{
	if (m_pContactListCtrl)
	{
		m_pContactListCtrl->UnregisterObserver(this);
	}
	
	RemoveCustomButtons();
	
	m_pUOLFoneButtonImage.reset();
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_buttonsCS);

		m_mapContactNode.RemoveAll();
	}
}


void CContactListMonitor::UpdateContactList(BOOL bConnected)
{
	if ( (bConnected) || (CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall()) )
	{
		if (CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
		{
			// Force buttons refresh on this case.
			//
			RemoveCustomButtons();
			RemoveCustomMenuItems();
		}
		
		AddCustomButtons();
		AddCustomMenuItems();
	}
	else
	{
		RemoveCustomButtons();
		RemoveCustomMenuItems();
	}
}


void CContactListMonitor::OnAddContact(IUOLMessengerContactNodePtr pContact)
{
	AddCustomButton(pContact);
}


void CContactListMonitor::OnRemoveContact(IUOLMessengerContactNodePtr pContact)
{
	RemoveCustomButton(pContact, TRUE);
}


void CContactListMonitor::OnRemoveContacts(CAtlList<IUOLMessengerContactNodePtr>& listContacts)
{
	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactNodePtr pContact = ap_dynamic_cast<IUOLMessengerContactNodePtr>(listContacts.GetNext(pos));
		
		RemoveCustomButton(pContact, TRUE);
	}
}


void CContactListMonitor::OnShowContextMenu(IUOLMessengerContextMenuPtr pContactContextMenu, 
			IUOLMessengerContactNodePtr pContactNode)
{
	if ( (pContactContextMenu) && (pContactNode) && (!m_menuItemLabel.IsEmpty()) )
	{
		IUOLMessengerContactPtr pContact = pContactNode->GetContact();
		
		if (IsCompatible(pContact))
		{
			m_pCurrentMenuItem = 
				pContactContextMenu->AddCustomMenuItem(m_menuItemLabel, this, TRUE);
			
			m_pCurrentMenuItemContactNode = pContactNode;
		}
	}
}


void CContactListMonitor::OnLButtonUp(
		IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	ATLTRACE(_T("%s - Call contact\n"), __FUNCTION__);

	IUOLMessengerContactNodePtr pContactNode = FindContactNode(
			(IUOLMessengerImageButton2*)pButton);
	
	MakeCall(pContactNode);
}


void CContactListMonitor::OnCommand(IUOLMessengerContextMenuItem* pItem)
{
	if (m_pCurrentMenuItem.get() == pItem)
	{
		MakeCall(m_pCurrentMenuItemContactNode);
	}
}


void CContactListMonitor::Update(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}

void CContactListMonitor::UpdateAlias(IUOLMessengerContact* pContact)
{
	UpdateButtonCondition(pContact);
}


void CContactListMonitor::UpdateStatus(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}


void CContactListMonitor::UpdateIcon(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}


void CContactListMonitor::UpdateExtendedCapabilities(IUOLMessengerContact2* pContact)
{
	UpdateButtonCondition(pContact);
}


void CContactListMonitor::MakeCall(IUOLMessengerContactNodePtr pContactNode)
{
	IUOLMessengerContactPtr pContact = NULL;
	IUOLMessengerAccountPtr pAccount = NULL;
	CString strCaption;
	CString strMessage;
	BOOL failure = FALSE;
	
	strCaption.LoadString(IDS_PLUGIN_NAME);
	
	if (pContactNode != NULL)
	{
		pContact = pContactNode->GetContact();
		
		if (pContact != NULL)
		{
			pAccount = pContact->GetAccount();
		}
	}
	
	ATLASSERT(m_pUOLMessengerFacade);
	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = m_pUOLMessengerFacade->GetAccountConnectionManager();
	
	if (pContact == NULL)
	{
		strMessage.LoadString(IDS_ERROR_UNABLE_RETRIEVE_CONTACT);
		
		failure = TRUE;
	}
	else if (pAccount == NULL)
	{
		strMessage.LoadString(IDS_ERROR_UNABLE_RETRIEVE_ACCOUNT);
		
		failure = TRUE;
	}
	else if (pAccountConnectionManager == NULL)
	{
		strMessage.LoadString(IDS_ERROR_UNABLE_RETRIEVE_ACCOUNT_CONNECTION_MANAGER);
		
		failure = TRUE;
	}
	else if (!pAccountConnectionManager->IsConnected(pAccount))
	{
		strMessage.LoadString(IDS_ERROR_USER_NOT_CONNECTED);
		
		failure = TRUE;
	}
	
	if (!failure)
	{
		failure = !CFacadeWrapper::GetInstance()->StartCallToPC(pContact, FALSE);
		
		if ( failure )
		{
			strMessage = CFacadeWrapper::GetInstance()->GetLastCallErrorDescription();
		}
	}
	
	if (failure)
	{
		if ( strMessage.IsEmpty() )
		{
			strMessage.LoadString(IDS_ERROR_CALL);
		}

        ap_static_cast<IUOLMessengerUIManager4Ptr>(m_pUOLMessengerFacade->GetUIManager())->ShowMessageBox(NULL, strMessage, strCaption, MB_OK | MB_ICONERROR);
	}
}


void CContactListMonitor::AddCustomButtons()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_buttonsCS);
	
	CAtlList<IUOLMessengerContactNodePtr> listContacts;
	
	m_pContactListCtrl->GetContacts(listContacts);
	
	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactNodePtr pContact = 
				ap_dynamic_cast<IUOLMessengerContactNodePtr>(listContacts.GetNext(pos));
		
		AddCustomButton(pContact);
	}
}


void CContactListMonitor::RemoveCustomButtons()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_buttonsCS);
	
	if ( NULL != m_pContactListCtrl )
	{
		CAtlList<IUOLMessengerContactNodePtr> listContacts;

		m_pContactListCtrl->GetContacts(listContacts);

		for (POSITION pos = listContacts.GetHeadPosition(); pos;)
		{
			IUOLMessengerContactNodePtr pContact = 
				ap_dynamic_cast<IUOLMessengerContactNodePtr>(listContacts.GetNext(pos));

			RemoveCustomButton(pContact, TRUE);
		}
	}
}


void CContactListMonitor::AddCustomButton(IUOLMessengerContactNodePtr pContactNode)
{
	IUOLMessengerContactPtr pContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_buttonsCS);

		// The following check avoids the possibility to receive consecutive 
		// updates with positive CheckCapabilities, what leads to creation of 
		// buttons that were already created.
		CMapContactNode::CPair* pPair = m_mapContactNode.Lookup(pContactNode);

		if ( (pPair != NULL) && (pPair->m_value != NULL) )
		{
			return;
		}

		if ( (m_pContactListCtrl != NULL) && (m_pUOLFoneButtonImage != NULL) && 
			(pContactNode != NULL) )
		{
			pContact = pContactNode->GetContact();

			if (IsCompatible(pContact))
			{
				IUOLMessengerImageButton2Ptr pImageButton = m_pContactListCtrl->CreateContactButton();

				if (pImageButton)
				{
					pImageButton->SetImage(m_pUOLFoneButtonImage);
					pImageButton->SetWidth(16);
					pImageButton->SetAuto(FALSE);
					pImageButton->SetDrawTransparent(TRUE);

					m_mapContactNode.SetAt(pContactNode, pImageButton);

					pContactNode->AddCustomButton(pImageButton);

					pImageButton->AddEventListener(this);
				}

				//ATLTRACE(_T("%s - button added\n"), __FUNCTION__);
			}
			else
			{
				m_mapContactNode.SetAt(pContactNode, NULL);
			}


		}
	}
	
	if (pContact)
	{
		pContact->RegisterObserver(this);
	}
}


void CContactListMonitor::RemoveCustomButton(IUOLMessengerContactNodePtr pContactNode, 
											 BOOL unregisterContact)
{
	IUOLMessengerContactPtr pContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_buttonsCS);
		
		CMapContactNode::CPair* pPair = m_mapContactNode.Lookup(pContactNode);
		
		if ( (pContactNode != NULL) && (pPair != NULL) )
		{
			IUOLMessengerImageButton2Ptr pImageButton = pPair->m_value;
			
			if (pImageButton)
			{
				pImageButton->RemoveEventListener(this);
				
				pContactNode->RemoveCustomButton(pImageButton);
			}
			
			if (unregisterContact)
			{
				pContact = pContactNode->GetContact();
			}
			
			m_mapContactNode.SetAt(pContactNode, NULL);

			//ATLTRACE(_T("%s - button removed\n"), __FUNCTION__);
		}
	}

	if (pContact)
	{
		pContact->UnregisterObserver(this);
	}
}


void CContactListMonitor::UpdateButtonCondition(IUOLMessengerContact* pContact)
{
	IUOLMessengerContactNodePtr pContactNode = FindContactNode(pContact);
	
	if (pContactNode)
	{
		IUOLMessengerContactPtr pTempContact = pContactNode->GetContact();
		
		if (IsCompatible(pTempContact))
		{
			AddCustomButton(pContactNode);
		}
		else
		{
			RemoveCustomButton(pContactNode, FALSE);
		}
	}
}


void CContactListMonitor::AddCustomMenuItems()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_menuItemsCS);
	
	CAtlList<IUOLMessengerContactNodePtr> listContacts;
	
	m_pContactListCtrl->GetContacts(listContacts);
	
	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactNodePtr pContact = 
				ap_dynamic_cast<IUOLMessengerContactNodePtr>(listContacts.GetNext(pos));
		
		AddCustomMenuItem(pContact);
	}
}


void CContactListMonitor::RemoveCustomMenuItems()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_menuItemsCS);
	
	CAtlList<IUOLMessengerContactNodePtr> listContacts;
	
	m_pContactListCtrl->GetContacts(listContacts);
	
	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactNodePtr pContact = 
				ap_dynamic_cast<IUOLMessengerContactNodePtr>(listContacts.GetNext(pos));
		
		RemoveCustomMenuItem(pContact);
	}
}


void CContactListMonitor::AddCustomMenuItem(IUOLMessengerContactNodePtr pContactNode)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_menuItemsCS);
	
	if ( (m_pContactListCtrl != NULL) && (!m_menuItemLabel.IsEmpty()) && 
			(pContactNode != NULL) )
	{
		IUOLMessengerContactPtr pContact = pContactNode->GetContact();
		
		if (IsCompatible(pContact))
		{
			IUOLMessengerContextMenuItemPtr pMenuItem = 
					m_pContactListCtrl->CreateContextMenuItem(m_menuItemLabel, this);
			
			if (pMenuItem)
			{
				m_mapContactNodeMenuItem.SetAt(pContactNode, pMenuItem);
				
				pContactNode->AddCustomMenuItem(pMenuItem);

				//ATLTRACE(_T("%s - item added\n"), __FUNCTION__);
			}
		}
	}
}


void CContactListMonitor::RemoveCustomMenuItem(IUOLMessengerContactNodePtr pContactNode)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_menuItemsCS);
	
	CMapContactNodeMenuItem::CPair* pPair = 
			m_mapContactNodeMenuItem.Lookup(pContactNode);
	
	if ( (pContactNode != NULL) && (pPair != NULL) )
	{
		IUOLMessengerContextMenuItemPtr pMenuItem = pPair->m_value;
		
		pContactNode->RemoveCustomMenuItem(pMenuItem);
		
		m_mapContactNodeMenuItem.RemoveKey(pPair->m_key);

		//ATLTRACE(_T("%s - item removed\n"), __FUNCTION__);
	}
}


IUOLMessengerContextMenuItemPtr CContactListMonitor::CreateUOLFoneContextMenuItem(
			IUOLMessengerContactNodePtr pContactNode)
{
	if ( (m_pContactListCtrl != NULL) && (!m_menuItemLabel.IsEmpty()) && 
			(pContactNode != NULL) )
	{
		IUOLMessengerContactPtr pContact = pContactNode->GetContact();
		
		if (IsCompatible(pContact))
		{
			return m_pContactListCtrl->CreateContextMenuItem(m_menuItemLabel, this);
		}
	}
	
	return NULL;
}


IUOLMessengerContactNodePtr CContactListMonitor::FindContactNode(
		IUOLMessengerImageButton2* pButton)
{
	for (POSITION posI = m_mapContactNode.GetStartPosition(); posI; 
			m_mapContactNode.GetNextValue(posI))
	{
		IUOLMessengerImageButton2Ptr pTempButton = m_mapContactNode.GetValueAt(posI);
		
		if (pTempButton.get() == pButton)
		{
			return m_mapContactNode.GetKeyAt(posI);
		}
	}
	
	return NULL;
}


IUOLMessengerContactNodePtr CContactListMonitor::FindContactNode(
		IUOLMessengerContact* pContact)
{
	for (POSITION posI = m_mapContactNode.GetStartPosition(); posI; 
			m_mapContactNode.GetNextValue(posI))
	{
		IUOLMessengerContactNodePtr pTempContactNode = m_mapContactNode.GetKeyAt(posI);
		
		if (pTempContactNode->GetContact().get() == pContact)
		{
			return pTempContactNode;
		}
	}
	
	return NULL;
}


IUOLMessengerImageButtonPtr CContactListMonitor::FindContactNodeImageButton(
		IUOLMessengerContactNodePtr pContactNode)
{
	if (pContactNode)
	{
		CMapContactNode::CPair* pPair = m_mapContactNode.Lookup(pContactNode);
		
		if (pPair)
		{
			return pPair->m_value;
		}
	}
	
	return NULL;
}


IUOLMessengerContactNodePtr CContactListMonitor::FindContactNode(
		IUOLMessengerContextMenuItem* pMenuItem)
{
	for (POSITION posI = m_mapContactNodeMenuItem.GetStartPosition(); posI; 
			m_mapContactNodeMenuItem.GetNextValue(posI))
	{
		IUOLMessengerContextMenuItemPtr pTempMenuItem = 
				m_mapContactNodeMenuItem.GetValueAt(posI);
		
		if (pTempMenuItem.get() == pMenuItem)
		{
			return m_mapContactNodeMenuItem.GetKeyAt(posI);
		}
	}
	
	return NULL;
}


BOOL CContactListMonitor::IsCompatible(IUOLMessengerContactPtr pContact)
{
	if (pContact == NULL)
	{
		return FALSE;
	}
	
	return ( (pContact->GetAccount()) && 
			 (CUOLFoneController::GetInstance()->GetCurrentAccount()) &&
			 (CUOLFoneController::GetInstance()->GetCurrentAccount()->GetIMAccount()) && 
			 (pContact->GetAccount().get() == CUOLFoneController::GetInstance()->GetCurrentAccount()->GetIMAccount().get()) && 
			 ( (CUOLFoneController::GetInstance()->IsConnected()) || 
					(CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall()) ) &&
			 (m_capabilitiesHandler.CheckCapabilitiesCompatibility(pContact)));
}