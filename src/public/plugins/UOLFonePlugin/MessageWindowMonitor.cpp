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
#include "MessageWindowMonitor.h"

#include "resource.h"
#include "Core/UOLFoneController.h"



CMessageWindowMonitor::CMessageWindowMonitor() : 
	m_pUOLMessengerFacade(NULL)
{
}


CMessageWindowMonitor::~CMessageWindowMonitor()
{
}


CMessageWindowMonitor* CMessageWindowMonitor::GetInstance()
{
	static CMessageWindowMonitor messageWindowMonitor;
	
	return &messageWindowMonitor;
}


BOOL CMessageWindowMonitor::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager = pUOLMessengerFacade->GetUIManager();
		
		if (pUIManager)
		{
			m_pUOLFoneButtonImage = pUIManager->LoadImage(
					ATL::_AtlBaseModule.GetResourceInstance(), 
					IDB_BTN_JANELA_CONVERSA);
			
			pUIManager->GetMessageWindowManager()->RegisterObserver(this);
			
			m_pUOLMessengerFacade = pUOLMessengerFacade;
			
			AddAllButtons();
			
			return TRUE;
		}
	}
	
	return FALSE;
}


void CMessageWindowMonitor::Finalize()
{
	if (m_pUOLMessengerFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager = m_pUOLMessengerFacade->GetUIManager();
		
		if (pUIManager)
		{
			pUIManager->GetMessageWindowManager()->UnregisterObserver(this);
			
			RemoveAllButtons();
			
			m_pUOLFoneButtonImage.reset();
		}
	}
	
	
		
	{ CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		m_mapMessageWindow.RemoveAll();
	}
}


void CMessageWindowMonitor::UpdateMessageWindows(BOOL bConnected)
{
	if ( (bConnected) || (CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall()) )
	{
		if (CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
		{
			// Force buttons refresh on this case.
			//
			RemoveAllButtons();
		}
		
		AddAllButtons();
	}
	else
	{
		RemoveAllButtons();
	}
}


void CMessageWindowMonitor::OnCreateMessageWindow(IUOLMessengerMessageWindow* pMessageWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if (FALSE == ::IsBadReadPtr(pMessageWindow, sizeof(IUOLMessengerMessageWindow*)))
	{
		pMessageWindow->RegisterObserver(this);
	}
}


void CMessageWindowMonitor::OnDestroyMessageWindow(IUOLMessengerMessageWindow* pMessageWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if (FALSE == ::IsBadReadPtr(pMessageWindow, sizeof(IUOLMessengerMessageWindow*)))
	{
		RemoveButton(pMessageWindow, TRUE);
	}
}


//void CMessageWindowMonitor::OnCreateConversationWindow(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
//{
//	ATLTRACE(_T("%s\n"), __FUNCTION__);
//	IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
//}


//void CMessageWindowMonitor::OnDestroyConversationWindow(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
//{
//	ATLTRACE(_T("%s\n"), __FUNCTION__);
//	IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
//}


void CMessageWindowMonitor::OnActivateConversationWindow(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if (FALSE == ::IsBadReadPtr(pMessageWindow, sizeof(IUOLMessengerMessageWindow*)))
	{
		IUOLMessengerImageButtonPtr pImageButton = FindMessageWindowImageButton(pMessageWindow);
		
		if (pImageButton)
		{
			UpdateButton(pMessageWindow, pImageButton);
		}
		else
		{
			AddButton(pMessageWindow);
		}
	}
}


void CMessageWindowMonitor::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	CString strCaption;
	CString strMessage;
	BOOL failure = FALSE;
	
	strCaption.LoadString(IDS_PLUGIN_NAME);

	IUOLMessengerMessageWindow* pMessageWindow;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
		
		pMessageWindow = FindMessageWindow(pButton);
	}
	
	if (!pMessageWindow)
	{
		return;
	}
	
	IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
	
	IUOLMessengerContactPtr pContact = pConversation->GetContact();
	IUOLMessengerAccountPtr pAccount = pConversation->GetAccount();
	
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


void CMessageWindowMonitor::Update(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}

void CMessageWindowMonitor::UpdateAlias(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}


void CMessageWindowMonitor::UpdateStatus(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}


void CMessageWindowMonitor::UpdateIcon(IUOLMessengerContact* pContact)
{
	// Anything to do for now.
}


void CMessageWindowMonitor::UpdateExtendedCapabilities(IUOLMessengerContact2* pContact)
{
	IUOLMessengerMessageWindow* pMessageWindow = FindMessageWindow(pContact);
	
	if (pMessageWindow)
	{
		IUOLMessengerImageButtonPtr pImageButton = FindMessageWindowImageButton(pMessageWindow);
		
		UpdateButton(pMessageWindow, pImageButton);
	}
}


void CMessageWindowMonitor::AddAllButtons()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
	
	if (m_pUOLMessengerFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager = m_pUOLMessengerFacade->GetUIManager();
		
		CAtlList<IUOLMessengerMessageWindowPtr> listMessageWindows;
		
		pUIManager->GetMessageWindowManager()->GetMessageWindowList(listMessageWindows);
		
		for (POSITION pos = listMessageWindows.GetHeadPosition(); pos; )
		{
			IUOLMessengerMessageWindowPtr pMessageWindow = listMessageWindows.GetNext(pos);
			
			AddButton(pMessageWindow.get());
		}
	}
}


void CMessageWindowMonitor::RemoveAllButtons()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
	
	if (m_pUOLMessengerFacade)
	{
		IUOLMessengerUIManagerPtr pUIManager = m_pUOLMessengerFacade->GetUIManager();
		
		CAtlList<IUOLMessengerMessageWindowPtr> listMessageWindows;
		
		pUIManager->GetMessageWindowManager()->GetMessageWindowList(listMessageWindows);
		
		for (POSITION pos = listMessageWindows.GetHeadPosition(); pos; )
		{
			IUOLMessengerMessageWindowPtr pMessageWindow = listMessageWindows.GetNext(pos);
			
			RemoveButton(pMessageWindow.get(), TRUE);
		}
	}
	
	/*
	// Considering just the map, whether any condition occurs that avoid the addition 
	// of the map element, we will have problems.
	// So... let's run all the opened windows, that it won't result in problems anyway.
	//
	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI;)
	{
		CMapMessageWindow::CPair* pPair = m_mapMessageWindow.GetNext(posI);
		
		ATLASSERT(pPair->m_key);
		
		RemoveButton(pPair->m_key, TRUE);
	}
	*/
}


void CMessageWindowMonitor::AddButton(IUOLMessengerMessageWindow* pMessageWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	IUOLMessengerContactPtr pContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pMessageWindow->GetToolbar();
		IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();

		if ( (pImageButtonContainer) && (pConversation) )
		{
			pContact = pConversation->GetContact();

			if (IsCompatible(pContact))
			{
				IUOLMessengerImageButtonPtr pImageButton = pImageButtonContainer->CreateExternalButton();

				if (pImageButton)
				{
					CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

					CString strTooltipText;
					strTooltipText.LoadString(IDS_MENU_LABEL_CHAMAR_CONTATO); //Chamar contato

					pImageButton->SetTip(strTooltipText);
					pImageButton->SetImage(m_pUOLFoneButtonImage);
					pImageButton->SetWidth(32);
					//pImageButton->SetPushButton(TRUE);
					pImageButton->SetAuto(FALSE);
					pImageButton->SetDrawTransparent(TRUE);
					//pImageButton->SetPressed(FALSE);

					m_mapMessageWindow.SetAt(pMessageWindow, pImageButton);

					pImageButtonContainer->AddExternalButton(pImageButton);

					pImageButton->AddEventListener(this);
				}
			}
			else
			{
				m_mapMessageWindow.RemoveKey(pMessageWindow);
			}
		}
	}

	if (pContact)
	{
		pContact->RegisterObserver(this);
	}
}


void CMessageWindowMonitor::RemoveButton(IUOLMessengerMessageWindow* pMessageWindow, 
										 BOOL unregisterContact)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	IUOLMessengerContactPtr pContact;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pMessageWindow->GetToolbar();

		if (pImageButtonContainer)
		{
			IUOLMessengerImageButtonPtr pImageButton = FindMessageWindowImageButton(pMessageWindow);

			if (pImageButton)
			{
				pImageButton->RemoveEventListener(this);

				pImageButtonContainer->RemoveExternalButton(pImageButton);
			}

			if (unregisterContact)
			{
				IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
				pContact = pConversation->GetContact();
			}

			m_mapMessageWindow.RemoveKey(pMessageWindow);
		}

		pMessageWindow->UnregisterObserver(this);
	}

	if (pContact)
	{
		pContact->UnregisterObserver(this);
	}
}


void CMessageWindowMonitor::UpdateButton(IUOLMessengerMessageWindow* pMessageWindow, 
										 IUOLMessengerImageButtonPtr pImageButton)
{
	IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
	
	if ( (pConversation) && (pImageButton) )
	{
		IUOLMessengerContactPtr pContact = pConversation->GetContact();
		
		if (IsCompatible(pContact))
		{
			pImageButton->SetVisible(TRUE);
		}
		else
		{
			pImageButton->SetVisible(FALSE);
		}
	}
}


IUOLMessengerMessageWindow* CMessageWindowMonitor::FindMessageWindow(IUOLMessengerImageButton* pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI; 
			m_mapMessageWindow.GetNextValue(posI))
	{
		IUOLMessengerImageButtonPtr pTempButton = m_mapMessageWindow.GetValueAt(posI);
		
		if (pTempButton.get() == pButton)
		{
			return m_mapMessageWindow.GetKeyAt(posI);
		}
	}
	
	return NULL;
}


IUOLMessengerMessageWindow* CMessageWindowMonitor::FindMessageWindow(
		IUOLMessengerContact2* pContact2)
{
	IUOLMessengerMessageWindow* pFoundMessageWindow = NULL;

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	IUOLMessengerAccountPtr pAccount = pContact2->GetAccount();
	CString strContactName = pContact2->GetName();

	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI; 
			m_mapMessageWindow.GetNextValue(posI))
	{
		IUOLMessengerMessageWindow* pTempMessageWindow = m_mapMessageWindow.GetKeyAt(posI);

		if (pTempMessageWindow)
		{
			IUOLMessengerConversationPtr pConversation = 
				pTempMessageWindow->FindConversation(pAccount, strContactName);
		
			if (pConversation)
			{
				pFoundMessageWindow = pTempMessageWindow;
				break;
			}
		}
	}
	
	return pFoundMessageWindow;
}


IUOLMessengerImageButtonPtr CMessageWindowMonitor::FindMessageWindowImageButton(IUOLMessengerMessageWindow* pMessageWindow)
{
	if (pMessageWindow)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		CMapMessageWindow::CPair* pPair = m_mapMessageWindow.Lookup(pMessageWindow);
		
		if (pPair)
		{
			return pPair->m_value;
		}
	}
	
	return NULL;
}


BOOL CMessageWindowMonitor::IsCompatible(IUOLMessengerContactPtr pContact)
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