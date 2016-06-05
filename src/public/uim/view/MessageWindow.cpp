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
#include "MessageWindow.h"

#include "../resource.h"
#include "../controller/UIMApplication.h"
#include "../controller/uimanager.h"
#include "../controller/Conversation/ConversationManager.h"
#include "../controller/HistoryRecordMessage.h"
#include "tooltip/ContactTooltipBuilder.h"
#include "UOLToolbarBuilder.h"
#include "Skin/MainDialogElement.h"
#include "ChatInviteDialog.h"
#include "Skin/ElementBuilder.h"
#include "Skin/MainDialogElement.h"

#include <interfaces/IUOLMessengerMultiStateImageButton.h>

#include <commands/showprivacydialogcommand.h>
#include <commands/sendfilecommand.h>
#include <commands/minimizewindowcommand.h>
#include <commands/closewindowcommand.h>
#include <commands/launchinfowindowcommand.h>
#include <commands/blockcontactcommand.h>
#include <commands/toggleblockedstatecommand.h>
#include <commands/ShowContactDialogCommand.h>
#include <commands/NudgeContactCommand.h>
#include <commands/InviteForChatCommand.h>


#define		TOOLBAR_HEIGHT  		32
#define		TABVIEW_HEIGHT  		24



////////////////////////////////////// 
// CConversationWindowObserverNotifier
//////////////////////////////////////

CConversationWindowObserverNotifier::CConversationWindowObserverNotifier(IUOLMessengerMessageWindow* pMessageWindow) :
	m_pMessageWindow(pMessageWindow)
{
}


void CConversationWindowObserverNotifier::NotifyAll(void (CUOLMessengerMessageWindowObserver::*pfnUpdate)(IUOLMessengerMessageWindow*, IUOLMessengerConversationWindow*), IUOLMessengerConversationWindow* pConversationWindow)
{
	CAtlList<CUOLMessengerMessageWindowObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
	
		listObservers.AddTailList(&m_listObservers);
	}

	CUOLMessengerMessageWindowObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(m_pMessageWindow, pConversationWindow);
	}
}










/////////////////
// CMessageWindow
/////////////////


typedef struct _CONVERSATIONWINDOWPARAM
{
	CConversationWindowPtr pConvWindow;

	_CONVERSATIONWINDOWPARAM& operator=(const _CONVERSATIONWINDOWPARAM& cwp)
	{
		pConvWindow = cwp.pConvWindow;

		return (*this);
	}

} CONVERSATIONWINDOWPARAM, *LPCONVERSATIONWINDOWPARAM;



const int CMessageWindow::MAX_TABS_COUNT = 10;


CMessageWindow::CMessageWindow(CMessageWindowFrame* pParentFrame) : 
	m_pParentFrame(pParentFrame),
	m_ObserverNotifier(pParentFrame),
	m_nActiveConversationIndex(-1),
	m_nHistoryWindowTabIndex(-1),
	m_nRichEditWindowTabIndex(-1),
	m_nSendButtonWindowTabIndex(-1)
{
	ATLASSERT(m_pParentFrame != NULL);
	m_bIsTabVisible = TRUE;
    m_bIsToolbarVisible = TRUE;
	
	//These lines is for Tooltip implementation
	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	IUOLMessengerUIManager* pUIManagerInterface = dynamic_cast<IUOLMessengerUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	ATLASSERT(pUIManager);

	IElementPtr pElement = const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__ContactTooltipBuilderInstance");
	CElementBuilderPtr pContactTooltipBuilderPtr = ap_dynamic_cast<CElementBuilderPtr>(pElement);

	m_pContactTooltip = ap_dynamic_cast<CContactTooltipPtr>(pContactTooltipBuilderPtr->CreateElement());
	//m_pTabView = new CTabView();
	//m_pTabView->SetMaxTabs(max_tabs_in_message);
	//m_pTabView->RegisterObserver(this);
}


CMessageWindow::~CMessageWindow()
{
	//ATLTRACE(_T("CMessageWindow::~CMessageWindow()\n"));
	if (m_pTabView.get())
		m_pTabView->UnregisterObserver(this);
}


int CMessageWindow::AddConversation(IUOLMessengerConversationPtr pConversation)
{
	int nIndexRet = -1;

	BOOL bEnableLog = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsLogMessagesEnabled();
	pConversation->EnableLogging(bEnableLog);

	// Create conversation window
    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
    ATLASSERT(pConversation2);
	CConversationWindowPtr pConversationWindow = new CConversationWindow(pConversation2);
	
	pConversationWindow->RegisterObserver(this);
	
    // This is applicable only on non chat conversation
    if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        IUOLMessengerContactPtr pActiveContact = pConversation->GetContact();
		
        if (pActiveContact)
        {
            pActiveContact->RegisterObserver(this);
        }
    }
	

	//Pass to conversation Window Toolbar Builder
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	CMessageEditPanelBuilderPtr pBuilder;

	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pBuilder.get() == NULL)); )
	{
		pBuilder = ap_dynamic_cast<CMessageEditPanelBuilderPtr>(m_listChilds.GetNext(pos));
	}	

	if (pBuilder)
	{
		pConversationWindow->SetMessageEditPanelBuilder(pBuilder);
	}

	// Create conversation window in the same thread this window was created
	CONVERSATIONWINDOWPARAM cwp;
	cwp.pConvWindow = pConversationWindow;

	SendMessage(WM_USER_CREATE_CONVERSATION_WINDOW, (WPARAM) &cwp, NULL);

	if (FALSE == pConversationWindow->IsWindow())
	{
		return nIndexRet;
	}
	
	// Add new conversation window

    if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        m_pTabView->AddTab(pConversation, GetContactAlias(pConversation), CWidget::Normal);
    }
    else
    {
        UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
        CString strCaption = pStringLoader->GetValue("uol.resource.IDS_CHAT_TAB_CAPTION");
        
        m_pTabView->AddTab(pConversation, strCaption, CWidget::Normal);
    }
	m_pTabView->Invalidate();
	m_pTabView->UpdateWindow();

	m_arConversationWindow.Add(pConversationWindow);

	nIndexRet = (int) m_arConversationWindow.GetCount() - 1;

	// Initialize conversation window...
	UpdateLayout();

	pConversation->RegisterObserver(this);

	return nIndexRet;
}


IUOLMessengerConversationPtr CMessageWindow::RemoveConversation(int nIndex)
{
	if ((nIndex >= 0) && (nIndex < (int) m_arConversationWindow.GetCount()))
	{
		CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nIndex);

		if (pConversationWindow != NULL)
		{
			m_arConversationWindow.RemoveAt(nIndex);

			IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();

			CONVERSATIONWINDOWPARAM cwp;
			cwp.pConvWindow = pConversationWindow;

			SendMessage(WM_USER_DESTROY_CONVERSATION_WINDOW, (WPARAM) &cwp, NULL);

			ATLASSERT(FALSE == pConversationWindow->IsWindow());

			pConversationWindow->UnregisterObserver(this);
			
			m_pTabView->RemoveTab(pConversation);

			int nElem = m_arConversationWindow.GetCount();
			if (nElem<=0) {
				// must close the window
				CWindow tmpWindow = GetParent();
				if (tmpWindow.IsWindow())
					tmpWindow.PostMessage(WM_CLOSE);
			} else {
				if (m_pActiveConversationWindow.get() == pConversationWindow.get()) {

					m_pActiveConversationWindow = CConversationWindowPtr();
					m_nActiveConversationIndex = -1;
					
					if (nIndex>=nElem) 
						nIndex=nElem-1; 
					
					SetActiveConversation(nIndex);
				}
			}

			UnregisterContactObserver(pConversationWindow);
			
			pConversation->UnregisterObserver(this);

			return pConversation;
		}
	}

	return IUOLMessengerConversationPtr();
}


IUOLMessengerConversationPtr CMessageWindow::GetConversation(int nIndex) const
{
	if ((nIndex >= 0) && (nIndex < (int) m_arConversationWindow.GetCount()))
	{
		CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nIndex);

		return pConversationWindow->GetConversation();
	}

	return IUOLMessengerConversationPtr();
}


int CMessageWindow::GetConversationCount() const
{
	return (int) m_arConversationWindow.GetCount();
}


BOOL CMessageWindow::CanAddMoreConversation()
{
	BOOL ret;
	IUOLMessengerMessageSettingsPtr pSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings();
	ret = pSettings->IsOnNewMsgWindowEnabled();
	return (ret && (GetConversationCount() < MAX_TABS_COUNT));
}


IUOLMessengerConversationPtr CMessageWindow::FindConversation(IUOLMessengerAccountPtr pAccount, const CString& strContactName)
{
	if (pAccount != NULL)
	{
		int nCount = (int) m_arConversationWindow.GetCount();

		for (int nInd = 0; nInd < nCount; nInd++)
		{   
			CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nInd);

			IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();

			if (MatchConversationContact(pConversation, pAccount, strContactName))
			{
				return pConversation;
			}
		}
	}

	return IUOLMessengerConversationPtr();
}


IUOLMessengerConversationPtr CMessageWindow::FindConversation(IUOLMessengerContactPtr pContact)
{
	return FindConversation(pContact->GetAccount(), pContact->GetName());
}

IUOLMessengerConversation2Ptr CMessageWindow::FindConversation(const CString& strChatKey)
{
    int nCount = (int) m_arConversationWindow.GetCount();

    for (int nInd = 0; nInd < nCount; nInd++)
    {   
        CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nInd);

        IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversationWindow->GetConversation());
        if (pConversation2)
        {
            if (0 == strChatKey.Compare(pConversation2->GetChatKey()))
            {
                return pConversation2;
            }
        }
    }
	
	return IUOLMessengerConversation2Ptr();
}

void CMessageWindow::SetActiveNextConversation()
{
	int newIndex ;
	newIndex = ( m_nActiveConversationIndex + 1) % m_arConversationWindow.GetCount();
	SetActiveConversation(newIndex);
}

void CMessageWindow::SetActivePriorConversation()
{
	int newIndex = m_nActiveConversationIndex - 1;
	
	if (newIndex < 0)
	{
		newIndex = m_arConversationWindow.GetCount() - 1;
	}
	
	SetActiveConversation(newIndex);
}

void CMessageWindow::SetActiveConversation(int nIndex)
{
	SetActiveConversationImpl(nIndex);
}


void CMessageWindow::SetActiveConversation(IUOLMessengerConversationPtr pConversation)
{
	int nIndex = GetConversationIndex(pConversation);

	SetActiveConversation(nIndex);
}


void CMessageWindow::SetActiveConversation(IUOLMessengerConversationPtr pConversation, BOOL bSetFocus)
{
	int nIndex = GetConversationIndex(pConversation);
	
	SetActiveConversationImpl(nIndex, bSetFocus);
}


IUOLMessengerConversationPtr CMessageWindow::GetActiveConversation() const
{
	if (m_pActiveConversationWindow)
	{
		return m_pActiveConversationWindow->GetConversation();
	}

	return IUOLMessengerConversationPtr();
}


int CMessageWindow::GetActiveConversationIndex() const
{
	return m_nActiveConversationIndex;
}


int CMessageWindow::GetConversationIndex(IUOLMessengerConversationPtr pConversation) const
{	
	CString strChatKey;
    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
    if (pConversation2)
    {
        strChatKey = pConversation2->GetChatKey();
    }
        
    IUOLMessengerAccountPtr pAccount = pConversation->GetAccount();
	CString strContactName = pConversation->GetContactName();

	int nCount = (int) m_arConversationWindow.GetCount();
	for (int nInd = 0; nInd < nCount; nInd++)
	{   
		CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nInd);

		IUOLMessengerConversationPtr pTempConversation = pConversationWindow->GetConversation();

	    IUOLMessengerConversation2Ptr pTempConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pTempConversation);
        if (pTempConversation2)
        {
            if (pTempConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                if (MatchConversationChat(pTempConversation2, pAccount, strChatKey))
                {
                    return nInd;
                }         
            }
            else
            {
                if (MatchConversationContact(pTempConversation, pAccount, strContactName))
                {
                    return nInd;
                }
            }
        }
        else
        {
            if (MatchConversationContact(pTempConversation, pAccount, strContactName))
            {
                return nInd;
            }
        }
	}

	return -1;
}


void CMessageWindow::FlashConversation(IUOLMessengerConversationPtr pConversation)
{
	if (m_pTabView->IsWindow())
	{
		m_pTabView->SetBlinking(pConversation);
		m_pTabView->Invalidate();
		m_pTabView->UpdateWindow();
	}
}


void CMessageWindow::ShowInputFocus()
{
	DWORD nAppThreadId = CUIMApplication::GetApplication()->GetApplicationThreadId();
	DWORD nCurrentThreadId = ::GetCurrentThreadId();
	
	ATLTRACE("CMessageWindow::ShowInputFocus() - nAppThreadId = %d and nCurrentThreadId = %d\n", 
			nAppThreadId, nCurrentThreadId);
	
	if (nAppThreadId == nCurrentThreadId)
	{
		ATLASSERT(m_pActiveConversationWindow != NULL);
		m_pTabView->SetActiveConversation(m_pActiveConversationWindow->GetConversation());

		if (m_pActiveConversationWindow != NULL)
		{
			m_pActiveConversationWindow->ShowInputFocus();
		}
	}
	else
	{
		// Called by libpurple thread.
		//
		::SendMessage(m_hWnd, WM_USER_SHOW_INPUT_FOCUS, 0, 0);
	}
}


IUOLMessengerImageButtonContainerPtr CMessageWindow::GetToolbar() const
{
	return m_pToolbar;
}


BOOL CMessageWindow::HasUnreadMessages()
{
	if (m_pTabView)
	{
		int nBlinkingCount = m_pTabView->GetTabCount(CWidget::Blinking);

		return (nBlinkingCount > 0);
	}

	return FALSE;
}


void CMessageWindow::RegisterObserver(CUOLMessengerMessageWindowObserver* pObserver)
{
	m_ObserverNotifier.RegisterObserver(pObserver);
}


void CMessageWindow::UnregisterObserver(CUOLMessengerMessageWindowObserver* pObserver)
{
	m_ObserverNotifier.UnregisterObserver(pObserver);
}


void CMessageWindow::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
	if (pSource == CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings().get())
	{
		if (nSettingId == IUOLMessengerMessageSettings::MSG_NEW_MSG_WINDOW_NOTIFICATION)
		{
			BOOL bTabView = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsOnNewMsgWindowEnabled();
			if (m_arConversationWindow.GetCount()<=1)
				m_bIsTabVisible = bTabView;
			else
				m_bIsTabVisible = TRUE;

			UpdateLayout();
		}
		if (nSettingId == IUOLMessengerMessageSettings::MSG_LOG_MESSAGES)
		{
			BOOL bEnableLog = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsLogMessagesEnabled();
			int nCount = (int) m_arConversationWindow.GetCount();

			for (int nInd = 0; nInd < nCount; nInd++)
			{   
				CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nInd);		
				IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();

				pConversation->EnableLogging(bEnableLog);
			}
		}
	}
}


void CMessageWindow::OnCreate(IUOLMessengerConversationWindow* pConversationWindow)
{
	m_ObserverNotifier.NotifyAll(&CUOLMessengerMessageWindowObserver::OnCreateConversationWindow, pConversationWindow);
}


void CMessageWindow::OnDestroy(IUOLMessengerConversationWindow* pConversationWindow)
{
	m_ObserverNotifier.NotifyAll(&CUOLMessengerMessageWindowObserver::OnDestroyConversationWindow, pConversationWindow);
}


void CMessageWindow::OnUpdateChatTitle(IUOLMessengerConversationWindow2* pConversationWindow)
{
	CString strCaption = pConversationWindow->GetConversationTitle();

	IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();
    if (pConversation)
    {
		if (pConversation == GetActiveConversation())
		{
			m_pParentFrame->SetCaptionText(strCaption);
			m_pParentFrame->Invalidate();
			m_pParentFrame->UpdateWindow();
		}

		m_pTabView->SetCaption(pConversation, strCaption);
		m_pTabView->Invalidate();
		m_pTabView->UpdateWindow();
	}
}


void CMessageWindow::OnConversationTabSelected(IUOLMessengerTabViewCtrl* pTabView, 
		IUOLMessengerConversationPtr pConversation, int nTabIndex)
{
	SetActiveConversation(pConversation);
	ShowInputFocus();
}

void CMessageWindow::OnConversationTabMouseHover(IUOLMessengerTabViewCtrl* pTabView, 
		IUOLMessengerConversationPtr pConversation, CPoint& pt, HWND wHandle)
{
    CPoint ptTooltipPos;
    ptTooltipPos.x = pt.x;
    ptTooltipPos.y = pt.y;

    ::ClientToScreen(wHandle, &ptTooltipPos);
    
    // Chat tab shows simple tooltip
    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
    if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        CString strCaption = pConversation2->GetChatTitle();
        m_ChatTooltip.ShowTooltip(wHandle, strCaption, ptTooltipPos);
    }
    else
    {    
        m_pContactTooltip->SetContact(pConversation->GetContact());
        m_pContactTooltip->Create(wHandle, NULL, "", WS_POPUP);

        if (m_pContactTooltip->IsWindow())
        {
            m_pContactTooltip->Show(wHandle, ptTooltipPos);
        }
    }
}

void CMessageWindow::OnConversationTabMouseLeave()
{
	if(m_pContactTooltip->IsWindow())
	{
		m_pContactTooltip->DestroyWindow();
	}

    m_ChatTooltip.HideTooltip();
}

void CMessageWindow::OnContactTyping(IUOLMessengerConversationPtr pConversation, TypingState state)
{
	// Not applicable on chat, so return
    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
    if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        return;
    }

    // Update status text if conversation window is active
	CConversationWindowPtr pConversationWindow = FindConversationWindow(pConversation->GetAccount(), pConversation->GetContact()->GetName());

	if ((pConversationWindow != NULL) &&
		(pConversationWindow == m_pActiveConversationWindow))
	{
		IUOLMessengerContactPtr pContact = pConversationWindow->GetConversation()->GetContact();

		CString strStatusBarText = _T("");
		if (state == TYPING)
		{
			CString strContactTypingTemplate;
			strContactTypingTemplate.LoadString(IDS_MESSAGE_WINDOW_CONTACT_TYPING_FORMAT);

			strStatusBarText.Format(strContactTypingTemplate, pContact->GetAlias());
		}

		m_pParentFrame->SetStatusBarText(strStatusBarText);
	}
}


void CMessageWindow::OnDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	// Update button if conversation window is active
	CConversationWindowPtr pConversationWindow = FindConversationWindow(pAccount, strContact);

	if ((pConversationWindow != NULL) &&
		(pConversationWindow == m_pActiveConversationWindow))
	{
		UpdateToolbarButtonState("ToggleBlockedState", "blocked");
	}
}


void CMessageWindow::OnDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	// Update button if conversation window is active
	CConversationWindowPtr pConversationWindow = FindConversationWindow(pAccount, strContact);

	if ((pConversationWindow != NULL) &&
		(pConversationWindow == m_pActiveConversationWindow))
	{
		UpdateToolbarButtonState("ToggleBlockedState", "unblocked");
	}
}

void CMessageWindow::OnContactAdded(IUOLMessengerAccount* pAccount, const CContactInfo* pContactInfo, const CString& strGroup)
{
	ATLTRACE("%s\n",__FUNCTION__);

	PostMessage(WM_USER_UPDATE_TOOLBAR);

	//IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount, pContactInfo->GetUser().c_str());

	//UpdateToolbarButtonVisible("ShowAddContactDialog",pContact==NULL?TRUE:FALSE);
}

void CMessageWindow::Update(IUOLMessengerContact* pContact)
{
}


void CMessageWindow::UpdateAlias(IUOLMessengerContact* pContact)
{
	if (pContact == NULL)
	{
		return;
	}
	
	CConversationWindowPtr pConversationWindow = 
			FindConversationWindow(pContact->GetAccount(), pContact->GetName());
	
	if (pConversationWindow == NULL)
	{
		return;
	}
	
	CString strCaption = pConversationWindow->GetConversationTitle();
	IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();
    
	if (pConversation != NULL)
	{
		if (pConversation == GetActiveConversation())
		{
			m_pParentFrame->SetCaptionText(strCaption);
			m_pParentFrame->Invalidate();
			m_pParentFrame->UpdateWindow();
		}
		
		m_pTabView->SetCaption(pConversation, GetContactAlias(pConversation));
		m_pTabView->Invalidate();
		m_pTabView->UpdateWindow();
	}
}


void CMessageWindow::UpdateStatus(IUOLMessengerContact* pContact)
{
	if (pContact == NULL)
	{
		return;
	}
	
	CConversationWindowPtr pConversationWindow = 
			FindConversationWindow(pContact->GetAccount(), pContact->GetName());
	
	if ((pConversationWindow != NULL) &&
		(pConversationWindow == m_pActiveConversationWindow))
	{
		UpdateWindowCaptionIcon(m_pActiveConversationWindow);
	}
}


void CMessageWindow::UpdateIcon(IUOLMessengerContact* pContact)
{
}


LRESULT CMessageWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	//m_haccelerator = AtlLoadAccelerators(IDR_MAINFRAME);
	ATL::_U_STRINGorID table = IDR_MAINFRAME;
	m_haccelerator = ::LoadAccelerators(ATL::_AtlBaseModule.GetResourceInstance(), table.m_lpstr);

	// Register as CMessageSettings event listener
	IUOLMessengerMessageSettingsPtr pSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings();
	ATLASSERT(pSettings.get() != NULL);
	pSettings->AddEventListener(this);

	m_bIsTabVisible = pSettings->IsOnNewMsgWindowEnabled();

	CUIMApplication::GetApplication()->GetConversationManager()->RegisterObserver(this);
	CUIMApplication::GetApplication()->GetPrivacyManager()->RegisterObserver(this);

	CreateToolbar();
	CreateTabView();

	SetWindowText(_T(""));
	return 0L;
}


void CMessageWindow::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);

	if (rect.IsRectEmpty())
	{
		return;
	}

    CSize size(rect.Width(), rect.Height());
    CRect convRect;
    CRect tabRect;

    int y = 0;
    int convesationHeight = size.cy;

    if (m_pToolbar.get() && m_bIsToolbarVisible)
    {
        CSize sizeToolbar;
        m_pToolbar->GetMinSize(sizeToolbar);

        m_pToolbar->SetWindowPos(NULL, 
            0, 0, 
            size.cx, sizeToolbar.cy, 
            SWP_NOZORDER); 			

        y += sizeToolbar.cy;

        convesationHeight -= (TOOLBAR_HEIGHT + TABVIEW_HEIGHT);
    }
    else
    {
        convesationHeight -= TABVIEW_HEIGHT;
    }

    if (m_bIsTabVisible) {
        convRect = CRect(0, y, size.cx, y + convesationHeight);
        tabRect = CRect(0, y + convesationHeight, size.cx, size.cy);
        m_pTabView->ShowWindow(SW_SHOW);
    } 
    else
    {
        convRect = CRect(0, y, size.cx, size.cy);
        tabRect = CRect(0, y + convesationHeight, size.cx, size.cy - (y + convesationHeight));
        m_pTabView->ShowWindow(SW_HIDE);
    }

    int nCount = (int) m_arConversationWindow.GetCount();

	for (int nInd = 0; nInd < nCount; nInd++)
	{   
		CConversationWindowPtr pConversationWindow = m_arConversationWindow.GetAt(nInd);

		pConversationWindow->SetWindowPos(NULL, 
										  convRect.left, convRect.top ,  
										  convRect.Width(), convRect.Height(), 
										  SWP_NOZORDER); 			
	}

	if (m_pTabView.get())
		m_pTabView->SetWindowPos(NULL, 
							 tabRect.left, tabRect.top,
							 tabRect.Width(), tabRect.Height(),
							 SWP_NOZORDER);
}


void CMessageWindow::GetMinSize(CSize& size)
{
	CSize sizeToolbar(0, 0);

	if (m_pToolbar && m_bIsToolbarVisible)
	{
		m_pToolbar->GetMinSize(sizeToolbar);
	}

	CSize sizeTabView(0, 0);

	if (m_pTabView)
	{
		m_pTabView->GetMinSize(sizeTabView);
	}

	size.cx = max(sizeToolbar.cx, sizeTabView.cx);
	size.cy = sizeToolbar.cy + sizeTabView.cy;
}


void CMessageWindow::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

    // Unregister as CMessageSettings event listener
    CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->RemoveEventListener(this);
    CUIMApplication::GetApplication()->GetConversationManager()->UnregisterObserver(this);
    CUIMApplication::GetApplication()->GetPrivacyManager()->UnregisterObserver(this);

    DestroyAllConversationWindows();

    SetMsgHandled(FALSE);
}


LRESULT CMessageWindow::OnCreateConversationWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPCONVERSATIONWINDOWPARAM pCwp = (LPCONVERSATIONWINDOWPARAM) wParam;

	if (pCwp->pConvWindow)
	{
		(pCwp->pConvWindow)->Create(m_hWnd);
	}

	return 0L;
}


LRESULT CMessageWindow::OnDestroyConversationWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPCONVERSATIONWINDOWPARAM pCwp = (LPCONVERSATIONWINDOWPARAM) wParam;

	if ((pCwp->pConvWindow) &&
		((pCwp->pConvWindow)->IsWindow()))
	{
		(pCwp->pConvWindow)->DestroyWindow();
	}

	return 0L;
}


LRESULT CMessageWindow::OnActivateConversationWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetActiveConversationImpl((int) wParam);

	return 0L;
}


LRESULT CMessageWindow::OnShowInputFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ShowInputFocus();
	
	return 0L;
}


IUOLMessengerCommandPtr CMessageWindow::GetCommand(const CString& strCommand)
{
	return CMessageWindowCommandFactory::GetCommand(strCommand, this);
}


void CMessageWindow::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	IUOLMessengerCommandPtr pCommand = GetCommand(pButton->GetCommandName());
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();

	if (pCommand)
	{
		pApplication->ExecuteCommand(pCommand);
	}
}

LRESULT CMessageWindow::OnUpdateToolbar(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetActiveConversation(GetActiveConversation());
	return 0L;
}

CConversationWindowPtr CMessageWindow::FindConversationWindow(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	if (pAccount != NULL)
	{
		int nCount = (int) m_arConversationWindow.GetCount();
		for (int nInd = 0; nInd < nCount; nInd++)
		{
			CConversationWindowPtr pConversationWindow = m_arConversationWindow[nInd];

			IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();

			if (MatchConversationContact(pConversation, pAccount, strContact))
			{
				return pConversationWindow;
			}
		}
	}

	return CConversationWindowPtr();
}


void CMessageWindow::DestroyAllConversationWindows()
{
	int nCount = (UINT) m_arConversationWindow.GetCount();
	for (int nInd = 0; nInd < nCount; nInd++)
	{
		CConversationWindowPtr pConversationWindow = m_arConversationWindow[nInd];

		UnregisterContactObserver(pConversationWindow);
		
		CONVERSATIONWINDOWPARAM cwp;
		cwp.pConvWindow = pConversationWindow;

		SendMessage(WM_USER_DESTROY_CONVERSATION_WINDOW, (WPARAM) &cwp, NULL);

		ATLASSERT(FALSE == pConversationWindow->IsWindow());
	}
}


void CMessageWindow::SetActiveConversationImpl(int nIndex, BOOL bSetFocus)
{
	if (((nIndex >= 0) && (nIndex < (int) m_arConversationWindow.GetCount())))/* &&
		(nIndex != m_nActiveConversationIndex))*/
	{
		CConversationWindowPtr pConversationWindow = m_arConversationWindow[nIndex];

		if (pConversationWindow != NULL)
		{
			// Switch conversation windows...
			if (m_pActiveConversationWindow != NULL)
			{
				m_pActiveConversationWindow->ShowWindow(SW_HIDE);
			}

			m_nActiveConversationIndex = nIndex;
			m_pActiveConversationWindow = pConversationWindow;

			m_pTabView->SetActiveConversation(pConversationWindow->GetConversation());
			m_pTabView->Invalidate();

            pConversationWindow->ShowWindow(SW_SHOW);
			pConversationWindow->BringWindowToTop();
            pConversationWindow->SetActiveWindow();
			
			if (bSetFocus)
			{
				//pConversationWindow->SetFocus();
			}

			IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();
			ATLASSERT(pConversation);

			// Update parent caption...
			CString strTitle = pConversationWindow->GetConversationTitle();
			pConversationWindow->SetWindowText(strTitle);

			m_pParentFrame->SetCaptionText(strTitle);

            UpdateWindowCaptionIcon(pConversationWindow);
            
			// Update status bar
			m_pParentFrame->SetStatusBarText(_T(""));
			
		    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
            
            // Hide toolbar when Chat is active
			if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                m_bIsToolbarVisible = FALSE;
                m_pToolbar->ShowWindow(SW_HIDE);
                UpdateLayout(TRUE);

                // Update splitters position
                pConversationWindow->UpdateLayout();
            }
            else
            {
                m_bIsToolbarVisible = TRUE;
                m_pToolbar->ShowWindow(SW_SHOWNORMAL);
                UpdateLayout(TRUE);
            }
            
            // Update toolbar buttons...
			IUOLMessengerAccountPtr pAccount = pConversation->GetAccount();

			IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount, pConversation->GetContactName());

			// Add Contact Button
            BOOL bShowAddContactButton = FALSE;
			
            // Check if:
            // 1) protocol has contact list support
            // 2) contact is not on list
            // 3) account ist online

            CIMProtocol* pProtocol = pAccount->GetProtocol();
            ATLASSERT(pProtocol);

            if (pProtocol)
            {
                CIMProtocol3* pProtocol3 = dynamic_cast<CIMProtocol3*>(pProtocol);

                if (pProtocol3)
                {
                    bShowAddContactButton = 
                        ((pProtocol3->HasContactListSupport()) &&
                        (!pContact) && 
                        (pConversationWindow->isConnected()));
                }
            }

			UpdateToolbarButtonVisible("ShowAddContactDialog", bShowAddContactButton);

            // Chat button
            BOOL bShowStartButton = FALSE;
			
			if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                // Check if protocol has chat support
                
                CIMProtocol* pProtocol = pAccount->GetProtocol();
                ATLASSERT(pProtocol);

                if (pProtocol)
                {
                    CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocol);

                    if (pProtocol5)
                    {
                        bShowStartButton = pProtocol5->HasChatSupport();
                    }
                }
            }

			UpdateToolbarButtonVisible("StartChat", bShowStartButton);

            //////////////////////////////////////////////////////////////////////////

            BOOL bShowNudgeContactButton = FALSE;

            if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                // Check if:
                // 1) protocol has nudge suport
                // 2) nudge receiving option is enabled ?

                CIMProtocol* pProtocol = pAccount->GetProtocol();
                ATLASSERT(pProtocol);

                if (pProtocol)
                {
                    CIMProtocol4* pProtocol4 = dynamic_cast<CIMProtocol4*>(pProtocol);

                    if (pProtocol4)
                    {
                        bShowNudgeContactButton = 
                            ((pProtocol4->HasCommandSupport(NUDGE_COMMAND_ID)) &&
                            (TRUE));
                    }
                }
            }

            pConversationWindow->UpdateMessageEditToolbarButtonVisible("NudgeContact", bShowNudgeContactButton);

            //////////////////////////////////////////////////////////////////////////

            if (pConversation2->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                ATLTRACE("%s pContact %d %s\n",__FUNCTION__, (pContact == NULL) ? TRUE : FALSE, pConversation->GetContactName());


                BOOL bDenied = CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(pAccount, pConversation->GetContactName());
                UpdateToolbarButtonState("ToggleBlockedState", bDenied ? "blocked" : "unblocked");
            }

            m_ObserverNotifier.NotifyAll(&CUOLMessengerMessageWindowObserver::OnActivateConversationWindow, m_pActiveConversationWindow.get());

            //////////////////////////////////////////////////////////////////////////

			if (m_nHistoryWindowTabIndex == -1)
			{
				m_nRichEditWindowTabIndex = AddTabTargetCtrl(
						m_pActiveConversationWindow->GetRichEditCtrlHandle(), FALSE);
				m_nSendButtonWindowTabIndex = AddTabTargetCtrl(
						m_pActiveConversationWindow->GetSendButtonCtrlHandle(), TRUE);
				m_nHistoryWindowTabIndex = AddTabTargetCtrl(
						m_pActiveConversationWindow->GetHistoryCtrlHandle(), FALSE, TRUE);
				
				//SetPreFocusActivateWindowHandle(m_pActiveConversationWindow->m_hWnd);
			}
			else
			{
				UpdateTabTargetCtrlHandle(m_nRichEditWindowTabIndex, 
						m_pActiveConversationWindow->GetRichEditCtrlHandle());
				UpdateTabTargetCtrlHandle(m_nSendButtonWindowTabIndex, 
						m_pActiveConversationWindow->GetSendButtonCtrlHandle());
				UpdateTabTargetCtrlHandle(m_nHistoryWindowTabIndex, 
						m_pActiveConversationWindow->GetHistoryCtrlHandle());
			}
		}
	}
}


void CMessageWindow::UnregisterContactObserver(CConversationWindowPtr pConversationWindow)
{
	if (pConversationWindow != NULL)
	{
		IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();

		if (pConversation)
		{
            // Not applicable on chat, so return
            IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
            if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                return;
            }

			IUOLMessengerContactPtr pContact = pConversation->GetContact();
					
			if (pContact)
			{
				pContact->UnregisterObserver(this);
			}
		}
	}
}


void CMessageWindow::CreateTabView()
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	CTabViewBuilderPtr pBuilder;

	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pBuilder.get() == NULL)); )
	{
		pBuilder = ap_dynamic_cast<CTabViewBuilderPtr>(m_listChilds.GetNext(pos));
	}	

	if (pBuilder)
	{
		m_pTabView = ap_dynamic_cast<CTabViewPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pTabView);
	}

	if (m_pTabView.get())
	{
		m_pTabView->Create(m_hWnd,  rcDefault);
		m_pTabView->SetMaxTabs(MAX_TABS_COUNT);
		m_pTabView->RegisterObserver(this);

        // Enables auto width tabs
        //m_pTabView->AutoCalculateTabsWidths(TRUE);
	}
}

void CMessageWindow::CreateToolbar()
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	CUOLToolBarBuilderPtr pBuilder;
	
	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pBuilder.get() == NULL)); )
	{
		pBuilder = ap_dynamic_cast<CUOLToolBarBuilderPtr>(m_listChilds.GetNext(pos));
	}	

	if (pBuilder)
	{
		m_pToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pToolbar);

		if (m_pToolbar)
		{
			CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
			m_pToolbar->Create(m_hWnd, rect);			

			m_pToolbar->AddButtonsEventListener(this);
		}
	}
}


BOOL CMessageWindow::MatchConversationContact(IUOLMessengerConversationPtr pConversation, IUOLMessengerAccountPtr pAccount, const CString& strContact) const
{
	ATLASSERT(pConversation != NULL);
	ATLASSERT(pAccount != NULL);

	CString strTargetUserId = pAccount->GetUserId();
	CString strTargetProtocolId = pAccount->GetProtocolId();

	IUOLMessengerAccountPtr pConvAccount = pConversation->GetAccount();

	return ((0 == strContact.Compare(pConversation->GetContactName())) &&
			(0 == strTargetUserId.Compare(pConvAccount->GetUserId())) &&
			(0 == strTargetProtocolId.Compare(pConvAccount->GetProtocolId())));
}


BOOL CMessageWindow::MatchConversationChat(IUOLMessengerConversation2Ptr pConversation, IUOLMessengerAccountPtr pAccount, const CString& strChatKey) const
{
	ATLASSERT(pConversation != NULL);
	ATLASSERT(pAccount != NULL);

	CString strTargetUserId = pAccount->GetUserId();
	CString strTargetProtocolId = pAccount->GetProtocolId();

	IUOLMessengerAccountPtr pConvAccount = pConversation->GetAccount();

	return ((0 == strChatKey.Compare(pConversation->GetChatKey())) &&
			(0 == strTargetUserId.Compare(pConvAccount->GetUserId())) &&
			(0 == strTargetProtocolId.Compare(pConvAccount->GetProtocolId())));
}


CString CMessageWindow::GetContactAlias(IUOLMessengerConversationPtr pConversation)
{
	CString strAlias;
    CString strTitle;
    
    // Not applicable on chat, so return
    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
    if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        return strAlias;
    }    

	IUOLMessengerContactPtr pContact = pConversation->GetContact();

	strAlias = pContact->GetAlias();

	if (strAlias.IsEmpty())
	{
		strTitle = pContact->GetName();
	}
    else
    {
        strTitle = pContact->GetAlias();
    }

	return strTitle;
}

void CMessageWindow::UpdateToolbarButtonVisible(const CString& strButtonCommand, BOOL bVisible)
{
	ATLTRACE("%s visible %d\n",__FUNCTION__, bVisible);
	if (m_pToolbar.get())
	{
		IUOLMessengerImageButtonPtr pImageButtonTmp = m_pToolbar->FindImageButton(strButtonCommand);
		if (pImageButtonTmp)
		{
			pImageButtonTmp->SetVisible(bVisible);
		}
	}
}

void CMessageWindow::UpdateWindowCaptionIcon(CConversationWindowPtr pConversationWindow)
{
	IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();
	ATLASSERT(pConversation);

    IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
    
    CWindowCaptionPtr pWindowCaption = m_pParentFrame->GetWindowCaption();

	if (pWindowCaption)
	{
		IUOLMessengerImagesProtocolPtr pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
				
		if (pImagesProtocol)
		{
			IUOLMessengerImagePtr pImage;
            
            if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                pImage = pImagesProtocol->GetUserStatusImage(pConversation->GetAccount()->GetProtocolId(), IUOLMessengerAccount::AS_ONLINE);
            }
            else
            {
                pImage = pImagesProtocol->GetContactStatusImage(pConversation->GetContact());
            }
					
			if (pImage)
			{
				pWindowCaption->SetDrawTransparentImage(TRUE);
				pWindowCaption->SetActiveImage(pImage);
				pWindowCaption->SetInactiveImage(pImage);
			}
		}
	}
}


void CMessageWindow::UpdateToolbarButtonState(const CString& strButtonCommand, const CString& strButtonState)
{
	if (m_pToolbar.get())
	{
		IUOLMessengerImageButtonPtr pImageButton = m_pToolbar->FindImageButton(strButtonCommand);

		//////////////////////////////////////////////////////////////////////////
		//IUOLMessengerImageButtonPtr pImageButtonTmp = m_pToolbar->FindImageButton(_T("ShowHistory"));
		//if (pImageButtonTmp)
		//{
		//	pImageButtonTmp->SetVisible(FALSE);
		//}
		//////////////////////////////////////////////////////////////////////////
		

		if (pImageButton)
		{
			IUOLMessengerMultiStateImageButtonPtr pMultiStateImageButton = ap_dynamic_cast<IUOLMessengerMultiStateImageButtonPtr>(pImageButton);

			if (pMultiStateImageButton)
			{
				pMultiStateImageButton->SetActiveState(strButtonState);
			}
		}
	}
}

BOOL CMessageWindow::PreTranslateMessage(MSG* pMsg)
{
	// << debug code
	//return FALSE;
	// >> debug code
	// Pass keyboard messages along to the child window that has the focus.
	// When the browser has the focus, the message is passed to its containing
	// CAxWindow, which lets the control handle the message.
	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
	{
		return FALSE;
	}

	HWND hWndCtl = ::GetFocus();

	if (IsChild(hWndCtl))
	{
	    if(::TranslateAccelerator(m_hWnd, m_haccelerator, pMsg))
		    return TRUE;

		// Find a direct child of the dialog from the window that has focus
		while (::GetParent(hWndCtl) != m_hWnd)
		{
			hWndCtl = ::GetParent(hWndCtl);
		}

		// Give control a chance to translate this message
		if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
		{
			return TRUE;
		}
	}

	// A normal control has the focus, so call IsDialogMessage() so that
	// the dialog shortcut keys work (TAB, etc.)
	//return IsDialogMessage(pMsg);
	return FALSE;
}



LRESULT CMessageWindow::OnAccelaratorHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	IUOLMessengerCommandPtr pCommand = GetCommand(_T("ShowHistory"));
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();

	if (pCommand)
	{
		pApplication->ExecuteCommand(pCommand);
	}
	return S_OK;
}

LRESULT CMessageWindow::OnAccelaratorInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	IUOLMessengerCommandPtr pCommand = GetCommand(_T("ShowUserInfo"));
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();

	if (pCommand)
	{
		pApplication->ExecuteCommand(pCommand);
	}	
	return S_OK;
}

LRESULT CMessageWindow::OnAccelaratorEscape(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	GetParent().PostMessage(WM_CLOSE);
	return S_OK;
}

LRESULT CMessageWindow::OnAccelaratorNextTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	SetActiveNextConversation();
	ShowInputFocus();

	return S_OK;
}

LRESULT CMessageWindow::OnAcceleratorPriorTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	
	SetActivePriorConversation();
	ShowInputFocus();
	
	return S_OK;
}

LRESULT CMessageWindow::OnAcceleratorCloseTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	
	RemoveConversation(m_nActiveConversationIndex);
	ShowInputFocus();
	
	return S_OK;
}

LRESULT CMessageWindow::OnAcceleratorSelectAll(
		WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLTRACE("%s",__FUNCTION__);
	
	m_pActiveConversationWindow->SelectAllText();
	
	return S_OK;
}


LRESULT CMessageWindow::OnEditCopy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ATLTRACE("%s",__FUNCTION__);
	
	m_pActiveConversationWindow->HandleEditCopy();
	
	return 0L;
}


BOOL CMessageWindow::CanCloseChatWindow()
{
    // Search for a chat window, if have ask user for close it
    BOOL bCanClose = TRUE;

    int nCount = (UINT) m_arConversationWindow.GetCount();
	for (int nInd = 0; nInd < nCount; nInd++)
	{
		CConversationWindowPtr pConversationWindow = m_arConversationWindow[nInd];
        IUOLMessengerConversationPtr pConversation = pConversationWindow->GetConversation();

        // If conversation is chat alert closing window
        IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
        if (pConversation2)
        {
            if (pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
            {
                IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

				if ((pConversation2->GetChatParticipantCount() > 2) &&
					(TRUE == pAccountConnectionManager->IsConnected(pConversation->GetAccount())))
				{
					UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
					ATLASSERT(pStringLoader);

					CString strMessage = pStringLoader->GetValue("uol.resource.IDS_CHAT_CONFIRM_CLOSE_WND");
					bCanClose = (CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES);

					if (!bCanClose)
					{
						break;
					}
				}
            }
        }
	}

    return bCanClose;
}


CString CMessageWindow::GetConversationTitle()
{
	CString strTile;

	if (m_pActiveConversationWindow)
	{
		strTile = m_pActiveConversationWindow->GetConversationTitle();
	}

	return strTile;
}


IUOLMessengerConversationWindow2* CMessageWindow::FindConversationWindow(IUOLMessengerConversationPtr pConversation)
{
	CConversationWindowPtr pConversationWindow;
	pConversationWindow = FindConversationWindow(pConversation->GetAccount(), pConversation->GetContact()->GetName());

	return pConversationWindow.get();
}


void CMessageWindow::OnSwitchContact(IUOLMessengerConversation* pConversation, IUOLMessengerContactPtr pRemovedContact, IUOLMessengerContactPtr pAddedContact)
{
	CConversationWindowPtr pConversationWindow;
	pConversationWindow = FindConversationWindow(pConversation->GetAccount(), pRemovedContact->GetName());

	if (pConversationWindow)
	{
		pRemovedContact->UnregisterObserver(this);
		pAddedContact->RegisterObserver(this);

		PostMessage(WM_USER_UPDATE_TOOLBAR);
	}
}


//LRESULT CMessageWindow::OnAccelaratorPreference(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		pUOLMessengerFacade->ExecuteCommand(new CShowPreferencesCommand());
//	}
//	return S_OK;
//}
//
//LRESULT CMessageWindow::OnAccelaratorAddContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowAddContactDialog"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//	return S_OK;
//}
//
//LRESULT CMessageWindow::OnAccelaratorPrivacy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		pUOLMessengerFacade->ExecuteCommand(new CShowPrivacyDialogCommand(pUOLMessengerFacade->GetUIManager()->GetAppWindow()));
//	}
//	return S_OK;
//}
//
//LRESULT CMessageWindow::OnAccelaratorHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowHelp"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//	return S_OK;
//}
//
//LRESULT CMessageWindow::OnAccelaratorOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("DisconnectAllAccounts"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//
//	return S_OK;
//}
//
//LRESULT CMessageWindow::OnAccelaratorOffline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("DisconnectAllAccounts"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//
//	return S_OK;
//}

//////////////////////////////////////
// CMessageWindowFrmaeObserverNotifier
//////////////////////////////////////

CMessageWindowFrameObserverNotifier::CMessageWindowFrameObserverNotifier(CMessageWindowFrame* pFrame) :
	m_pFrame(pFrame)
{
}

void CMessageWindowFrameObserverNotifier::NotifyAll(void (CMessageWindowFrameObserver::*pfnUpdate)(CMessageWindowFrame*))
{
	CAtlList<CMessageWindowFrameObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
	
		listObservers.AddTailList(&m_listObservers);
	}

	CMessageWindowFrameObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(m_pFrame);
	}
}









//////////////////////
// CMessageWindowFrame
//////////////////////

CMessageWindowFrame::CMessageWindowFrame() :
	m_observerNotifier(this),
	m_bMinimized(FALSE)
{
	m_pView = new CMessageWindow(this);
	m_nIDFlashTimer = 0;
}


CMessageWindowFrame::~CMessageWindowFrame()
{
}


int CMessageWindowFrame::AddConversation(IUOLMessengerConversationPtr pConversation)
{
	int nResult = static_cast<CMessageWindow*>(m_pView.get())->AddConversation(pConversation);
	if (m_bMinimized || !HasFocus()) 
	{
		int nConversations = static_cast<CMessageWindow*>(m_pView.get())->GetConversationCount();
		if (nConversations>1)
		{
			m_bMinimized= TRUE;
			CString strText;
			strText.Format(_T("%d Conversas"), nConversations);
			SetCaptionText(strText);
		}
	}
	return nResult;
}


IUOLMessengerConversationPtr CMessageWindowFrame::RemoveConversation(int nIndex)
{
	return static_cast<CMessageWindow*>(m_pView.get())->RemoveConversation(nIndex);
}


IUOLMessengerConversationPtr CMessageWindowFrame::GetConversation(int nIndex) const
{
	return static_cast<const CMessageWindow*>(m_pView.get())->GetConversation(nIndex);
}


int CMessageWindowFrame::GetConversationCount() const
{
	return static_cast<const CMessageWindow*>(m_pView.get())->GetConversationCount();
}


BOOL CMessageWindowFrame::CanAddMoreConversation()
{
	return static_cast<CMessageWindow*>(m_pView.get())->CanAddMoreConversation();
}


IUOLMessengerConversationPtr CMessageWindowFrame::FindConversation(IUOLMessengerAccountPtr pAccount, const CString& strContactName)
{
	return static_cast<CMessageWindow*>(m_pView.get())->FindConversation(pAccount, strContactName);
}


IUOLMessengerConversationPtr CMessageWindowFrame::FindConversation(IUOLMessengerContactPtr pContact)
{
	return static_cast<CMessageWindow*>(m_pView.get())->FindConversation(pContact);
}


IUOLMessengerConversation2Ptr CMessageWindowFrame::FindConversation(const CString& strChatKey)
{
	return static_cast<CMessageWindow*>(m_pView.get())->FindConversation(strChatKey);
}


void CMessageWindowFrame::SetActiveNextConversation(void)
{
	static_cast<CMessageWindow*>(m_pView.get())->SetActiveNextConversation();
}

void CMessageWindowFrame::SetActivePriorConversation(void)
{
	static_cast<CMessageWindow*>(m_pView.get())->SetActivePriorConversation();
}

void CMessageWindowFrame::SetActiveConversation(int nIndex)
{
	static_cast<CMessageWindow*>(m_pView.get())->SetActiveConversation(nIndex);
}


void CMessageWindowFrame::SetActiveConversation(IUOLMessengerConversationPtr pConversation)
{
	static_cast<CMessageWindow*>(m_pView.get())->SetActiveConversation(pConversation);
}


IUOLMessengerConversationPtr CMessageWindowFrame::GetActiveConversation() const
{
	return static_cast<const CMessageWindow*>(m_pView.get())->GetActiveConversation();
}


int CMessageWindowFrame::GetActiveConversationIndex() const
{
	return static_cast<const CMessageWindow*>(m_pView.get())->GetActiveConversationIndex();
}


int CMessageWindowFrame::GetConversationIndex(IUOLMessengerConversationPtr pConversation) const
{
	return static_cast<const CMessageWindow*>(m_pView.get())->GetConversationIndex(pConversation);
}


void CMessageWindowFrame::Show(int nCmdShow)
{
	ShowWindow(nCmdShow);

	//static_cast<CMessageWindow*>(m_pView.get())->Show();
}


void CMessageWindowFrame::Hide()
{
	ShowWindow(SW_HIDE);

	//static_cast<CMessageWindow*>(m_pView.get())->Hide();
}


void CMessageWindowFrame::Raise()
{
	BringWindowToTop();
}


void CMessageWindowFrame::Flash()
{
	if (!m_nIDFlashTimer)
	{
		m_nIDFlashTimer = SetTimer(1, 1000, NULL);
	}
}

void CMessageWindowFrame::GetMinSize(CSize& size)
{
	__super::GetMinSize(size);
	
	//size.cx = 200;
	size.cy = 380;
}

void CMessageWindowFrame::FlashConversation(IUOLMessengerConversationPtr pConversation)
{
	static_cast<CMessageWindow*>(m_pView.get())->FlashConversation(pConversation);
}


BOOL CMessageWindowFrame::HasFocus() const
{
	HWND hWnd = GetForegroundWindow();

	return (m_hWnd == hWnd) || (IsChild(hWnd));
}


BOOL CMessageWindowFrame::IsVisible() const
{
	return IsWindowVisible();
}


void CMessageWindowFrame::ShowInputFocus()
{
	static_cast<CMessageWindow*>(m_pView.get())->ShowInputFocus();
}


IUOLMessengerImageButtonContainerPtr CMessageWindowFrame::GetToolbar() const
{
	return static_cast<CMessageWindow*>(m_pView.get())->GetToolbar();
}


BOOL CMessageWindowFrame::HasUnreadMessages()
{
	return static_cast<CMessageWindow*>(m_pView.get())->HasUnreadMessages();
}


BOOL CMessageWindowFrame::CanCloseChatWindow()
{
	return static_cast<CMessageWindow*>(m_pView.get())->CanCloseChatWindow();
}


IUOLMessengerConversationWindow2* CMessageWindowFrame::FindConversationWindow(IUOLMessengerConversationPtr pConversation)
{
	return static_cast<CMessageWindow*>(m_pView.get())->FindConversationWindow(pConversation);
}


void CMessageWindowFrame::OnFinalMessage(HWND /*hWnd*/)
{
	CUIMApplication::GetApplication()->GetUIManager()->GetMessageWindowManager()->DestroyMessageWindow(this);
}


LRESULT CMessageWindowFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	__super::OnCreate(lpCreateStruct);

	m_observerNotifier.NotifyAll(&CMessageWindowFrameObserver::OnCreateWindow);

	return 0L;
}


void CMessageWindowFrame::OnSize(UINT type, const CSize& size)
{
	__super::OnSize(type, size);

	if (type == SIZE_MINIMIZED )
	{
		m_bMinimized= TRUE;
		int nConversations = static_cast<CMessageWindow*>(m_pView.get())->GetConversationCount();
		if (nConversations > 1)
		{
			//m_bMinimized= TRUE;
			CString strText;
			strText.Format(_T("%d Conversas"), nConversations);
			SetCaptionText(strText);
		}
	}
	else
	{
		if (m_bMinimized)
		{
			int nConversations = static_cast<CMessageWindow*>(m_pView.get())->GetConversationCount();
			if (nConversations > 1)
			{
				SetActiveConversation(GetActiveConversation());
			}
			m_bMinimized = FALSE;
		}
	}
}


void CMessageWindowFrame::OnDestroy()
{
	m_observerNotifier.NotifyAll(&CMessageWindowFrameObserver::OnDestroyWindow);

	SetMsgHandled(FALSE);
}


void CMessageWindowFrame::OnClose()
{
	// Do not automatically close if there are unread messages
	if (static_cast<CMessageWindow*>(m_pView.get())->HasUnreadMessages())
	{
		if (CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsShowUnreadMessagesAlertEnabled())
		{
			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			CConfirmDlgPtr pConfirmDlg = GetConfirmDialog();

			pConfirmDlg->m_bCheck = FALSE;
			pConfirmDlg->m_strCaption = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
			pConfirmDlg->m_strText = pStringLoader->GetValue("uol.resource.IDS_UNREAD_MESSAGES_ALERT");
			pConfirmDlg->m_strCheckText = pStringLoader->GetValue("uol.resource.IDS_DONT_DISPLAY_AGAIN");
			pConfirmDlg->m_strOKButton = pStringLoader->GetValue("uol.resource.IDS_YES");
			pConfirmDlg->m_strCancelButton = pStringLoader->GetValue("uol.resource.IDS_NO");

			BOOL bRet = pConfirmDlg->DoModal();
			
			CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->EnableShowUnreadMessagesAlert(FALSE == pConfirmDlg->m_bCheck);

			if (!bRet)
			{
				CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_SHOW_UNREAD_MESSAGES_ALERT);
				return;
			}
		}	
	}

    // Do not automatically close if there are chat window
    if (!CanCloseChatWindow())
	{
        return;
    }

	DestroyWindow();
}


LRESULT CMessageWindowFrame::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return FlashWindow(TRUE);
}


LRESULT CMessageWindowFrame::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_nIDFlashTimer != 0)
	{
        if (KillTimer(m_nIDFlashTimer))
		{
			m_nIDFlashTimer = 0;
		}
	}

	ShowInputFocus();

	return TRUE;
}

void CMessageWindowFrame::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
	static_cast<CMessageWindow*>(m_pView.get())->OnSettingChanged(pSource, nSettingId);
}

void CMessageWindowFrame::RegisterObserver(CUOLMessengerMessageWindowObserver* pObserver)
{
	static_cast<CMessageWindow*>(m_pView.get())->RegisterObserver(pObserver);
}

void CMessageWindowFrame::SetActiveConversation(IUOLMessengerConversationPtr pConversation, BOOL bSetFocus)
{
	static_cast<CMessageWindow*>(m_pView.get())->SetActiveConversation(pConversation, bSetFocus);
}

void CMessageWindowFrame::UnregisterObserver(CUOLMessengerMessageWindowObserver* pObserver)
{
	static_cast<CMessageWindow*>(m_pView.get())->UnregisterObserver(pObserver);
}

void CMessageWindowFrame::RegisterFrameObserver(CMessageWindowFrameObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}

void CMessageWindowFrame::UnregisterFrameObserver(CMessageWindowFrameObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


CString CMessageWindowFrame::GetConversationTitle()
{
	return static_cast<CMessageWindow*>(m_pView.get())->GetConversationTitle();
}


CConfirmDlgPtr CMessageWindowFrame::GetConfirmDialog()
{
	CConfirmDlgPtr pConfirmDlg;

	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__ConfirmDialogBuilderInstance");			
	if (pElement)
	{
		CElementBuilderPtr pBuilder;				
		pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
		if (pBuilder)
		{
			pConfirmDlg = ap_dynamic_cast<CConfirmDlgPtr>(pBuilder->CreateElement());			
			ATLASSERT(pConfirmDlg.get());
		}
	}

	return pConfirmDlg;
}








///////////////////////////////
// CMessageWindowCommandFactory
///////////////////////////////

IUOLMessengerCommandPtr CMessageWindowCommandFactory::GetCommand(const CString& strCommand, CMessageWindow* pMessageWindow)
{
	IUOLMessengerContactPtr pContact;
	IUOLMessengerConversationPtr pConversation;

	CString strContactName;
	CString strprotocolName;
	CString strAccountID;

	pConversation = pMessageWindow->GetActiveConversation();
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	ATLASSERT(pConversation.get());

	if (pConversation)
	{
		pContact = pConversation->GetContact();

		if (pContact)
		{
			strContactName = pContact->GetName();
			strprotocolName = pContact->GetAccount()->GetProtocolId();
			strAccountID = pContact->GetAccount()->GetUserId();
		}
	}

	if (strCommand.CompareNoCase(_T("ShowUserInfo")) == 0)

		
	{
		if (pContact)
		{
			return new CLaunchInfoWindowCommand(pContact, *pMessageWindow);
		}
	}
	else if (strCommand.CompareNoCase(_T("ShowAddContactDialog")) == 0)
	{
		return new CShowContactDialogCommand(pConversation->GetAccount(), pContact->GetName());
	}
	else if (strCommand.CompareNoCase(_T("ShowPrivacy")) == 0)
	{
        return new CShowPrivacyDialogCommand(*pMessageWindow);
	}
	else if (strCommand.CompareNoCase(_T("SendFile")) == 0)
	{
		if (pContact)
		{
			return new CSendFileCommand(pContact, *pMessageWindow);
		}
		else if (0 == CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount())
		{
			CString strMessage;
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_SEND_FILE_NO_ONLINE_ACCOUNTS_MSG");

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(*pMessageWindow, strMessage, MB_OK | MB_ICONERROR);
		}
	}
	else if (strCommand.CompareNoCase(_T("ShowHistory")) == 0)
	{
		CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		if (pUIManager)
		{
			CString strCaption;
			CString strFormat;
			strFormat = pStringLoader->GetValue("uol.resource.IDS_HISTORY_WINDOW_MESSAGES_TITLE");
			strCaption.Format(strFormat, pConversation->GetContact()->GetAlias());

			CHistoryRecordMessagePtr pHistoryRecordMessage;
			pHistoryRecordMessage = new CHistoryRecordMessage(pConversation->GetAccount(), pConversation->GetContactName());

			CHistoryRecordMessageListPtr pHistoryRecordMessageList;
			pHistoryRecordMessageList = new CHistoryRecordMessageList(pHistoryRecordMessage);
			pUIManager->ShowHistoryWindow(pHistoryRecordMessage, pHistoryRecordMessageList, strCaption);
		}
	}
	else if (strCommand.CompareNoCase(_T("BlockContact")) == 0)
	{
		if (pContact)
		{
			return new CBlockContactCommand(pContact, *pMessageWindow);
		}
	}
	else if (strCommand.CompareNoCase(_T("ToggleBlockedState")) == 0)
	{
		if (pContact)
		{
			return new CToggleBlockedStateCommand(pContact, *pMessageWindow);
		}
	}
	else if (strCommand.CompareNoCase(_T("MinimizeWindow")) == 0)
	{
		return new CMinimizeWindowCommand(pMessageWindow->GetParent());
	}
	else if (strCommand.CompareNoCase(_T("CloseWindow")) == 0)
	{
		return new CCloseWindowCommand(pMessageWindow->GetParent());
	}
    else if (strCommand.CompareNoCase(_T("StartChat")) == 0)
	{
		if (pContact)
		{
			UINT ret = IDCANCEL;
			
			// Show Chat Invite Dialog
			CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());
			IElementPtr pElement = const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__ChatInviteDialogBuilderInstance");
			if (pElement)
			{
				CElementBuilderPtr pBuilder;	

				pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);
				if (pBuilder)
				{
					CChatInviteDialogPtr pChatInviteDialog = ap_dynamic_cast<CChatInviteDialogPtr>(pBuilder->CreateElement());			
					ATLASSERT(pChatInviteDialog.get());
					
					if (pChatInviteDialog.get())
					{
						IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
						ATLASSERT(pConversation2);

						pChatInviteDialog->SetConversation(pConversation2);
						ret = pChatInviteDialog->DoModal(pMessageWindow->m_hWnd);
					}
				}		
			}
		}
	}
	/*
	// Moved to MessageEditPanel.
	else if (strCommand.CompareNoCase(_T("NudgeContact")) == 0)
	{
		if ( (pContact) && (pConversation) )
		{
			return new CNudgeContactCommand(pConversation);
		}
	}
	*/
	else
	{
		return CUIMApplication::GetApplication()->GetCommandFactory()->GetCommand(strCommand);
	}

	return IUOLMessengerCommandPtr();
}