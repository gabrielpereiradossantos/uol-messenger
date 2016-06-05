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
#include "CryptoProtocolController.h"



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
			m_pImageEstablished = pUIManager->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_BITMAP_CRYPTO_SESSION_ESTABLISHED);
			m_pImageStarted = pUIManager->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_BITMAP_CRYPTO_SESSION_STARTED);
			m_pImageEnded = pUIManager->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_BITMAP_CRYPTO_SESSION_ENDED);

			pUIManager->GetMessageWindowManager()->RegisterObserver(this);

			CAtlList<IUOLMessengerMessageWindowPtr> listMessageWindow;

			pUIManager->GetMessageWindowManager()->GetMessageWindowList(listMessageWindow);

			AddButtons(listMessageWindow);

			m_pUOLMessengerFacade = pUOLMessengerFacade;

			m_pCryptoPluginConfig = CCryptoPluginConfig::GetInstance();

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

			m_pImageEstablished.reset();
			m_pImageStarted.reset();
			m_pImageEnded.reset();
		}
	}
}


void CMessageWindowMonitor::ShowCryptoSessionState(const CString& strProtocolId, const CString& strUser, const CString& strContact, CMessageWindowMonitor::CryptoSessionState state)
{
	IUOLMessengerImageButtonPtr pImageButton = FindMessageWindowImageButton(strProtocolId, strUser, strContact);

	if (pImageButton)
	{
		IUOLMessengerImagePtr pImage;
		
		CString strTooltipText;

		if (state == CSS_SESSION_STARTED)
		{
			pImage = m_pImageStarted;
			strTooltipText.LoadString(IDS_TOOLTIP_ESTABLISHING_SESSION);
		}
		else if (state == CSS_SESSION_ESTABLISHED)
		{
			pImage = m_pImageEstablished;
			strTooltipText.LoadString(IDS_TOOLTIP_END_SESSION);
		}
		else
		{
			pImage = m_pImageEnded;
			strTooltipText.LoadString(IDS_TOOLTIP_START_SESSION);
		}

		if (pImage)
		{
			pImageButton->SetImage(pImage);
			pImageButton->SetTip(strTooltipText);
		}
	}

	// Get ConversationWindow
	IUOLMessengerMessageWindow2* pMessageWindow;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		pMessageWindow = dynamic_cast<IUOLMessengerMessageWindow2*>(FindMessageWindow(pImageButton.get()));
	}
	if (pMessageWindow)
	{
		IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
		IUOLMessengerConversationWindow2* pConversationWindow2;
		pConversationWindow2 = pMessageWindow->FindConversationWindow(pConversation);
		
		if (pConversationWindow2)
		{
			if (state == CSS_SESSION_STARTED)
			{
				// Session is estabilished, disable/enable message log
				pConversationWindow2->SetIsLogMessageEnabled(m_pCryptoPluginConfig->IsLogMessageEnabled());
			}
			else if (state == CSS_SESSION_ENDED)
			{
				// Session is ended, enable message log
				pConversationWindow2->SetIsLogMessageEnabled(TRUE);
			}
		}
	}
}


void CMessageWindowMonitor::ShowCryptoSessionError(const CString& strProtocolId, const CString& strUser, const CString& strContact, CCryptoSession::ErrorReason reason)
{
	IUOLMessengerConversationPtr pConversation = FindConversation(strProtocolId, strUser, strContact);

	if (pConversation)
	{
		CString strMessage;

		if (reason == CCryptoSession::reasonAcceptTimeout)
		{
			strMessage.LoadString(IDS_CANTSTARTCRYPTOSESSION);
		}
		else if (reason == CCryptoSession::reasonSessionNotFound)
		{
			strMessage.LoadString(IDS_CRYPTO_SESSION_NOT_FOUND);
		}

		if (FALSE == strMessage.IsEmpty())
		{
			pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
		}
	}

	IUOLMessengerImageButtonPtr pImageButton = FindMessageWindowImageButton(strProtocolId, strUser, strContact);

	if (pImageButton)
	{
		if (m_pImageEnded)
		{
			pImageButton->SetImage(m_pImageEnded);

			CString strTooltipText;
			strTooltipText.LoadString(IDS_TOOLTIP_START_SESSION);
			pImageButton->SetTip(strTooltipText);
		}
	}

	// Get ConversationWindow
	IUOLMessengerMessageWindow2* pMessageWindow;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		pMessageWindow = dynamic_cast<IUOLMessengerMessageWindow2*>(FindMessageWindow(pImageButton.get()));
	}
	if (pMessageWindow)
	{
		IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
		IUOLMessengerConversationWindow2* pConversationWindow2;
		pConversationWindow2 = pMessageWindow->FindConversationWindow(pConversation);
		
		if (pConversationWindow2)
		{
			pConversationWindow2->SetIsLogMessageEnabled(TRUE);
		}
	}
}


CMessageWindowMonitor::CMessageWindowMonitor() : 
	m_pUOLMessengerFacade(NULL)
{
}


void CMessageWindowMonitor::OnCreateMessageWindow(IUOLMessengerMessageWindow* pMessageWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (FALSE == ::IsBadReadPtr(pMessageWindow, sizeof(IUOLMessengerMessageWindow*)))
	{
		AddButton(pMessageWindow);
	}
}


void CMessageWindowMonitor::OnDestroyMessageWindow(IUOLMessengerMessageWindow* pMessageWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (FALSE == ::IsBadReadPtr(pMessageWindow, sizeof(IUOLMessengerMessageWindow*)))
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pMessageWindow->GetToolbar();

		if (pImageButtonContainer)
		{
			IUOLMessengerImageButtonPtr pImageButton = FindMessageWindowImageButton(pMessageWindow);

			if (pImageButton)
			{
				pImageButtonContainer->RemoveExternalButton(pImageButton);

				bool bRemoved = m_mapMessageWindow.RemoveKey(pMessageWindow);
				ATLASSERT(bRemoved);
			}
		}

		pMessageWindow->UnregisterObserver(this);
	}
}


//void CMessageWindowMonitor::OnCreateConversationWindow(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
//{
//	ATLTRACE(_T("%s\n"), __FUNCTION__);
//}
//
//
//void CMessageWindowMonitor::OnDestroyConversationWindow(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
//{
//	ATLTRACE(_T("%s\n"), __FUNCTION__);
//}


void CMessageWindowMonitor::OnActivateConversationWindow(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	UpdateButton(pMessageWindow, pConversationWindow);
}


void CMessageWindowMonitor::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	IUOLMessengerMessageWindow2* pMessageWindow;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		pMessageWindow = dynamic_cast<IUOLMessengerMessageWindow2*>(FindMessageWindow(pButton));
	}

	if (!pMessageWindow)
	{
		return;
	}

	IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();

	IUOLMessengerAccountPtr pAccount = pConversation->GetAccount();
	
	ATLASSERT(m_pUOLMessengerFacade);
	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = m_pUOLMessengerFacade->GetAccountConnectionManager();

	if ((pAccountConnectionManager) && 
		(pAccountConnectionManager->IsConnected(pAccount)))
	{
		CString strProtocolId = pAccount->GetProtocolId();

		CCryptoProtocol* pCryptoProtocol = CCryptoProtocolController::GetInstance()->GetProtocol(strProtocolId);

		if (pCryptoProtocol)
		{
			CString strAccount = pAccount->GetUserId();
			CString strContact = pConversation->GetContact()->GetName();

			IUOLMessengerImagePtr pImage = pButton->GetImage();

			if (pImage.get() == m_pImageEnded.get())		
			{				
				pCryptoProtocol->StartCryptoSession((LPCTSTR)strAccount, (LPCTSTR)strContact);
			}
			else
			{
				pCryptoProtocol->EndCryptoSession((LPCTSTR)strAccount, (LPCTSTR)strContact, false);
			}
		}
	}
	else
	{
		CString strCaption;
		CString strMessage;

		strCaption.LoadString(IDS_CRYPTO_PLUGIN_NAME);
		strMessage.LoadString(IDS_ACCOUNT_NOT_CONNECTED_MSG);

		::MessageBox(NULL, strMessage, strCaption, MB_OK | MB_ICONERROR);
	}
}


void CMessageWindowMonitor::AddButton(IUOLMessengerMessageWindow* pMessageWindow)
{
	IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pMessageWindow->GetToolbar();

	if (pImageButtonContainer)
	{
		IUOLMessengerImageButtonPtr pImageButton = pImageButtonContainer->CreateExternalButton();

		if (pImageButton)
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

			CString strTooltipText;
			strTooltipText.LoadString(IDS_TOOLTIP_START_SESSION);

			pImageButton->SetTip(strTooltipText);
			pImageButton->SetImage(m_pImageEnded);
			pImageButton->SetWidth(32);
			//pImageButton->SetPushButton(TRUE);
			pImageButton->SetAuto(FALSE);
			pImageButton->SetDrawTransparent(TRUE);
			//pImageButton->SetPressed(FALSE);
            pImageButton->SetCommandName("UOLCrypto");

			m_mapMessageWindow.SetAt(pMessageWindow, pImageButton);

			pImageButtonContainer->AddExternalButton(pImageButton);

			pImageButton->AddEventListener(this);
		}
	}

	pMessageWindow->RegisterObserver(this);
}


void CMessageWindowMonitor::AddButtons(CAtlList<IUOLMessengerMessageWindowPtr>& listMessageWindows)
{
	for (POSITION pos = listMessageWindows.GetHeadPosition(); pos; )
	{
		IUOLMessengerMessageWindowPtr pMessageWindow = listMessageWindows.GetNext(pos);

		AddButton(pMessageWindow.get());
	}
}


void CMessageWindowMonitor::RemoveAllButtons()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI;)
	{
		CMapMessageWindow::CPair* pPair = m_mapMessageWindow.GetNext(posI);

		ATLASSERT(pPair->m_key);

		IUOLMessengerMessageWindow* pMessageWindow = pPair->m_key;

		IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pMessageWindow->GetToolbar();

		pImageButtonContainer->RemoveExternalButton(pPair->m_value);

		pMessageWindow->UnregisterObserver(this);
	}

	m_mapMessageWindow.RemoveAll();
}


void CMessageWindowMonitor::UpdateButton(IUOLMessengerMessageWindow* pMessageWindow, IUOLMessengerConversationWindow* pConversationWindow)
{
	IUOLMessengerConversationWindow2* pConversationWindow2;
	pConversationWindow2 = dynamic_cast<IUOLMessengerConversationWindow2*>(pConversationWindow);
	
	IUOLMessengerImageButtonPtr pImageButton;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
	
		pImageButton = FindMessageWindowImageButton(pMessageWindow);
	}

	ATLASSERT(pImageButton);
	if (pImageButton)
	{
		IUOLMessengerConversationPtr pConversation;

		if (pConversationWindow)
		{
			pConversation = pConversationWindow->GetConversation();
		}
		else
		{
			pConversation = pMessageWindow->GetActiveConversation();
		}

		CString strProtocolId = pConversation->GetAccount()->GetProtocolId();

		CCryptoProtocol* pProtocol = CCryptoProtocolController::GetInstance()->GetProtocol(strProtocolId);

		if (pProtocol != NULL)
		{
			CString strUserId = pConversation->GetAccount()->GetUserId();
            CString strContact;
            
            IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
            if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
			    strContact = pConversation->GetContact()->GetName();
            }

			CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

			//pImageButton->SetPressed(pProtocol->GetSession(static_cast<LPCTSTR>(strUserId), static_cast<LPCTSTR>(strContact)) != NULL);

			CString strTooltipText;

			if (pProtocol->GetSession(static_cast<LPCTSTR>(strUserId), static_cast<LPCTSTR>(strContact)) != NULL)
			{
				strTooltipText.LoadString(IDS_TOOLTIP_END_SESSION);
				pImageButton->SetImage(m_pImageEstablished);

				// Session is estabilished, disable/enable message log
				if (pConversationWindow2)
				{
					pConversationWindow2->SetIsLogMessageEnabled(m_pCryptoPluginConfig->IsLogMessageEnabled());
				}
			}
			else
			{
				strTooltipText.LoadString(IDS_TOOLTIP_START_SESSION);
				pImageButton->SetImage(m_pImageEnded);

				// Session is ended, enable message log
				if (pConversationWindow2)
				{
					pConversationWindow2->SetIsLogMessageEnabled(TRUE);
				}
			}

			pImageButton->SetTip(strTooltipText);
		}
	}
}


IUOLMessengerMessageWindow* CMessageWindowMonitor::FindMessageWindow(IUOLMessengerImageButton* pButton)
{
	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI; m_mapMessageWindow.GetNextValue(posI))
	{
		IUOLMessengerImageButtonPtr pTempButton = m_mapMessageWindow.GetValueAt(posI);

		if (pTempButton.get() == pButton)
		{
			return m_mapMessageWindow.GetKeyAt(posI);
		}
	}

	return NULL;
}


IUOLMessengerImageButtonPtr CMessageWindowMonitor::FindMessageWindowImageButton(const CString& strProtocolId, const CString& strUser, const CString& strContact)
{
	CMapMessageWindow::CPair* pPair;

	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI;)
	{
		pPair = m_mapMessageWindow.GetNext(posI);

		if (pPair)
		{
			IUOLMessengerMessageWindow* pMessageWindow = pPair->m_key;

			if (pMessageWindow)
			{
				IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();

				if (pConversation)
				{
					CString strConvProtocolId = pConversation->GetAccount()->GetProtocolId();
					CString strConvUser = pConversation->GetAccount()->GetUserId();
					CString strConvContact = pConversation->GetContact()->GetName();

					if ((0 == strProtocolId.Compare(strConvProtocolId)) &&
						(0 == strUser.Compare(strConvUser)) &&
						(0 == strContact.Compare(strConvContact)))
					{
						return pPair->m_value;
					}
				}
			}
		}
	}

	return IUOLMessengerImageButtonPtr();
}


IUOLMessengerImageButtonPtr CMessageWindowMonitor::FindMessageWindowImageButton(IUOLMessengerMessageWindow* pMessageWindow)
{
	if (pMessageWindow)
	{
		CMapMessageWindow::CPair* pPair = m_mapMessageWindow.Lookup(pMessageWindow);

		if (pPair)
		{
			return pPair->m_value;
		}
	}

	return IUOLMessengerImageButtonPtr();
}


IUOLMessengerConversationPtr CMessageWindowMonitor::FindConversation(const CString& strProtocolId, const CString& strUser, const CString& strContact)
{
	IUOLMessengerConversationPtr pConversation;

	if (m_pUOLMessengerFacade)
	{
		IUOLMessengerAccountPtr pAccount = m_pUOLMessengerFacade->GetAccountManager()->FindAccount(strUser, strProtocolId);
		
		if (pAccount)
		{
			IUOLMessengerUIManagerPtr pUIManager = m_pUOLMessengerFacade->GetUIManager();

			if (pUIManager)
			{
				pUIManager->GetMessageWindowManager()->FindConversationMessageWindow(pAccount, strContact, pConversation);
			}
		}
	}

	return pConversation;
}

void CMessageWindowMonitor::UpdateCryptoLogMessageConfig()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	for (POSITION posI = m_mapMessageWindow.GetStartPosition(); posI;)
	{
		CMapMessageWindow::CPair* pPair = m_mapMessageWindow.GetNext(posI);
		ATLASSERT(pPair->m_key);

		IUOLMessengerMessageWindow2* pMessageWindow = dynamic_cast<IUOLMessengerMessageWindow2*>(pPair->m_key);
		if (pMessageWindow)
		{
			IUOLMessengerConversationPtr pConversation = pMessageWindow->GetActiveConversation();
			IUOLMessengerConversationWindow2* pConversationWindow2;
			pConversationWindow2 = pMessageWindow->FindConversationWindow(pConversation);

			if (pConversationWindow2)
			{
				CString strProtocolId = pConversation->GetAccount()->GetProtocolId();
				CCryptoProtocol* pProtocol = CCryptoProtocolController::GetInstance()->GetProtocol(strProtocolId);

				if (pProtocol != NULL)
				{
					CString strUserId = pConversation->GetAccount()->GetUserId();
					CString strContact;

					IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
					if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
					{
						strContact = pConversation->GetContact()->GetName();
					}

					CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

					if (pProtocol->GetSession(static_cast<LPCTSTR>(strUserId), static_cast<LPCTSTR>(strContact)) != NULL)
					{
						// Session is estabilished, disable/enable message log
						pConversationWindow2->SetIsLogMessageEnabled(m_pCryptoPluginConfig->IsLogMessageEnabled());
					}
					else
					{
						// Session is ended, enable message log
						pConversationWindow2->SetIsLogMessageEnabled(TRUE);
					}
				}
			}
		}
	}
}
