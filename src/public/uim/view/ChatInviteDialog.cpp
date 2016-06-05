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
 
// ChatInviteDialog.cpp : implementation of the CChatInviteDialog class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ChatInviteDialog.h"
#include "DialogManager.h"
#include "../controller/UIMApplication.h"
#include "../controller/uimanager.h"
#include "../controller/Conversation/ConversationManager.h"

#include <commands/InviteForChatCommand.h>

#define	TOP_SHIFT_SPACE		5
#define	LEFT_SHIFT_SPACE	10
#define	RIGHT_SHIFT_SPACE	10
#define	LINE_SHIFT_SPACE	10
#define	TITLE_LINE_SPACE	25
#define BUTTONS_SPACE		100
#define BUTTONS_WIDTH		75
#define BUTTONS_HEIGHT		23
#define BUTTON_OK_TOP		307
#define BUTTON_OK_LEFT		107
#define BUTTON_CANCEL_TOP	307
#define BUTTON_CANCEL_LEFT	212


CChatInviteDialog::CChatInviteDialog(void) : CMainDialog(IDD_DIALOG_CHAT_INVITE)
{
}


CChatInviteDialog::~CChatInviteDialog(void)
{
 	m_pFindContactPanel->UnregisterObserver(this);
	
	if (IsWindow())
    {
		CDialogManager::GetInstance()->UnregisterDialog(m_hWnd);
        EndDialog(IDCANCEL);
    }
}


void CChatInviteDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
    __super::SetParameters(pMapParams, pElementTable);
    
    try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "font");

		m_pFontText = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFontText.get());

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "font");
		// use default system font
		m_pFontText = new CFontElement();
	}
}


void CChatInviteDialog::DoPaint(CDCHandle& dc)
{
	__super::DoPaint(dc);

	// Draw text
	int nLeft = m_rectView.left + LEFT_SHIFT_SPACE;
	int nTop = m_rectView.top + TOP_SHIFT_SPACE;
	int nRight = m_rectView.right - RIGHT_SHIFT_SPACE;
	int nBottom = nTop + TITLE_LINE_SPACE;
	CRect rectText(nLeft, nTop, nRight, nBottom);
	COLORREF clrText = RGB(0, 0, 0);
	m_pFontText->DrawColoredText(dc, rectText, m_strText, clrText, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
}


void CChatInviteDialog::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
    m_pConversation = pConversation;
}


void CChatInviteDialog::UpdateLayout()
{
	__super::UpdateLayout();

	// Update controls layout
	CSize sizeFindContactPanel;
	CSize size = m_rectView.Size();
	int nLeft = m_rectView.left;
	int nTop = m_rectView.top + TOP_SHIFT_SPACE + TITLE_LINE_SPACE;
	int nBottom = size.cy - BUTTONS_SPACE - LINE_SHIFT_SPACE;
	
	if ((m_pFindContactPanel) && (m_pFindContactPanel->IsWindow()))
	{
        m_pFindContactPanel->RegisterObserver(this);
		m_pFindContactPanel->GetMinSize(sizeFindContactPanel);

        m_pFindContactPanel->SetWindowPos(NULL, nLeft, nTop, size.cx, sizeFindContactPanel.cy,  SWP_NOZORDER);
		m_pFindContactPanel->UpdateLayout();
        nTop += sizeFindContactPanel.cy;
	}

    if ((m_pChatInviteListCtrl) && (m_pChatInviteListCtrl->IsWindow()))
	{
        m_pChatInviteListCtrl->SetWindowPos(NULL, nLeft, nTop, size.cx, nBottom,  SWP_NOZORDER);
		m_pChatInviteListCtrl->UpdateLayout();
    }

	if (m_btnOk.IsWindow())
	{
		m_btnOk.SetWindowPos(NULL, BUTTON_OK_LEFT, BUTTON_OK_TOP, BUTTONS_WIDTH, BUTTONS_HEIGHT, SWP_NOZORDER);
	}

	if (m_btnCancel.IsWindow())
	{
		m_btnCancel.SetWindowPos(NULL, BUTTON_CANCEL_LEFT, BUTTON_CANCEL_TOP, BUTTONS_WIDTH, BUTTONS_HEIGHT, SWP_NOZORDER);
	}
}


LRESULT CChatInviteDialog::OnInitDialog(HWND, LPARAM)
{
    __super::OnInitDialog(NULL, NULL);

	CenterWindow(GetParent());

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	m_strText = pStringLoader->GetValue("uol.resource.IDS_CHAT_INVITE_DLG_TEXT");

	CString strAppName;
	strAppName = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
	SetTitle(strAppName);
	
	CreateFindContactPanel();
	CreateChatInviteListCtrl();
	m_pChatInviteListCtrl->SetFindContactPanel(m_pFindContactPanel);

    CDialogManager::GetInstance()->RegisterDialog(m_hWnd);

	// Create buttons
	CRect buttonRect;
	buttonRect = CRect(CPoint(0, 0), CSize(BUTTONS_WIDTH, BUTTONS_HEIGHT));
	m_btnOk.Create(m_hWnd, buttonRect, _T("Ok"), WS_CHILDWINDOW | WS_VISIBLE);
	m_btnOk.SetNotifyingWindow(m_hWnd);
	m_btnOk.SetSendNewMessage(TRUE);

	m_btnCancel.Create(m_hWnd, buttonRect, _T("Cancelar"), WS_CHILDWINDOW | WS_VISIBLE);
	m_btnCancel.SetNotifyingWindow(m_hWnd);
	m_btnCancel.SetSendNewMessage(TRUE);

	UpdateLayout();

	PopulateChatInviteList();

	//m_pFindContactPanel->SetFocus();
	m_pChatInviteListCtrl->SelectItem(0);
    return FALSE;
}


LRESULT CChatInviteDialog::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	__super::OnDestroy();
	
	if (m_pChatInviteListCtrl)
	{
		m_pChatInviteListCtrl->Finalize();
	}

	return 0L;
}


void CChatInviteDialog::DoOK()
{	
	if (HasAtLeastOneContactChecked())
	{	
		IUOLMessengerCommandPtr pInviteForChatCommand;

		// Store last chat invited contact name, so we can focus chat conversation window
		CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(
			CUIMApplication::GetApplication()->GetConversationManager());
		pConversationManager->SetChatInvitedContact(m_pConversation->GetContact()->GetName());

		// invite for chat conversation window contact
		pInviteForChatCommand = new CInviteForChatCommand(m_pConversation, m_pConversation->GetContact(), TRUE);
		CUIMApplication::GetApplication()->ExecuteCommand(pInviteForChatCommand);

		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csLock);

			// Insert each selected contact into list
			CAtlList<IUOLMessengerContactPtr>* pListChatInviteContacts;
			pListChatInviteContacts = new CAtlList<IUOLMessengerContactPtr>;

			CAtlList<IUOLMessengerChatInviteNodePtr> listContacts;
			m_pChatInviteListCtrl->GetContacts(listContacts);
			for (POSITION pos = listContacts.GetHeadPosition(); pos;)
			{
				IUOLMessengerChatInviteNodePtr pChatInviteNode = listContacts.GetNext(pos);
				if (pChatInviteNode)
				{
					if (pChatInviteNode->IsChecked())
					{
						pListChatInviteContacts->AddHead(pChatInviteNode->GetContact());
					}
				}
			}

			// Set chat invite contacts to be used when receive AddChatParticipant
			CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
				CUIMApplication::GetApplication()->GetUIManager());
			if (pUIManager)
			{
				pUIManager->AddChatInviteContact(m_pConversation->GetAccount(), 
					m_pConversation->GetContact(), pListChatInviteContacts);
			}
		}

		EndDialog(IDOK);
	}
	else
	{
		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		ATLASSERT(pStringLoader);

		CString strMessage = pStringLoader->GetValue("uol.resource.IDS_CHAT_INVITE_DLG_WARNING");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONWARNING);
	}
}


void CChatInviteDialog::DoCancel()
{	
	EndDialog(IDCANCEL);
}


void CChatInviteDialog::CreateFindContactPanel()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__FindContactPanelBuilderInstance");

	if (pElement)
	{
		CFindContactPanelBuilderPtr pBuilder;
		pBuilder = ap_dynamic_cast<CFindContactPanelBuilderPtr>(pElement);
		if (pBuilder)
		{
			m_pFindContactPanel = ap_dynamic_cast<CFindContactPanelPtr>(pBuilder->CreateElement());
			if (m_pFindContactPanel != NULL)
			{
				CRect rectPanel = CRect(CPoint(0, 0), CSize(0, 0));
				DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
				m_pFindContactPanel->Create(m_hWnd, rectPanel, NULL, dwStyle);
			}
		}
	}
}


void CChatInviteDialog::CreateChatInviteListCtrl()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__ChatInviteListCtrlBuilderInstance");

	if (pElement)
	{
		CChatInviteListCtrlBuilderPtr pBuilder;
		pBuilder = ap_dynamic_cast<CChatInviteListCtrlBuilderPtr>(pElement);
		if (pBuilder)
		{
			m_pChatInviteListCtrl = ap_dynamic_cast<CChatInviteListCtrlPtr>(pBuilder->CreateElement());
		}

		if (m_pChatInviteListCtrl.get())
		{
			IUOLMessengerFacade* pApp = CUIMApplication::GetApplication();
			m_pChatInviteListCtrl->SetConversation(m_pConversation);
			m_pChatInviteListCtrl->Init(pApp->GetContactList());

			CRect rect = CRect(0, 0, 0, 0);
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			m_pChatInviteListCtrl->Create(m_hWnd, rect, NULL, dwStyle);
			m_pChatInviteListCtrl->SetNotifyingWindow(m_hWnd);
			m_pChatInviteListCtrl->SetSendNewMessage(TRUE);
		}
	}
}


void CChatInviteDialog::PopulateChatInviteList()
{
    if (m_pChatInviteListCtrl)
    {
        IUOLMessengerAccountPtr pAccount;
        pAccount = m_pConversation->GetAccount();
        CString strAccountUserId;
        if (pAccount)
        {
            strAccountUserId = pAccount->GetUserId();

            IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();
            CAtlList<IUOLMessengerContactPtr> listContacts;
            if (pContactList->FindContacts(pAccount, listContacts))
            {
                for (POSITION pos = listContacts.GetHeadPosition(); pos; )
                {
                    IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);
                    if (pContact)
                    {
                        CString strContactName = pContact->GetName();
						if (0 != strContactName.CompareNoCase(m_pConversation->GetContactName()))
						{
							m_pChatInviteListCtrl->AddChatContact(pContact);
						}
                    }
                }
            }
        }
    }
}


void CChatInviteDialog::OnFindPanelTextChanged(CString strFindContactText)
{
	ATLTRACE(_T("%s - %s\n"), __FUNCTION__, strFindContactText);
	m_pChatInviteListCtrl->Sort();
	m_pChatInviteListCtrl->UpdateItemSelection();
	m_pChatInviteListCtrl->UpdateScrollSize();
}


void CChatInviteDialog::OnFindPanelArrowKeyPressed()
{
	if (m_pChatInviteListCtrl.get() && m_pChatInviteListCtrl->IsWindow() && m_pChatInviteListCtrl->IsWindowVisible())
	{
		m_pChatInviteListCtrl->SetFocus();
		m_pChatInviteListCtrl->SelectItem(1);
	}
}


void CChatInviteDialog::OnFindPanelEscPressed()
{
	if (m_pChatInviteListCtrl.get() && m_pChatInviteListCtrl->IsWindow() && m_pChatInviteListCtrl->IsWindowVisible())
	{
		m_pChatInviteListCtrl->SetFocus();
		m_pChatInviteListCtrl->SelectItem(1);
	}
}


BOOL CChatInviteDialog::HasAtLeastOneContactChecked()
{
	BOOL bIsChecked = FALSE;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csLock);

		CAtlList<IUOLMessengerChatInviteNodePtr> listContacts;
		m_pChatInviteListCtrl->GetContacts(listContacts);
		for (POSITION pos = listContacts.GetHeadPosition(); pos;)
		{
			IUOLMessengerChatInviteNodePtr pChatInviteNode = listContacts.GetNext(pos);
			if (pChatInviteNode)
			{
				if (pChatInviteNode->IsChecked())
				{
					bIsChecked = TRUE;
					break;
				}
			}
		}
	}

	return bIsChecked;
}


void CChatInviteDialog::OnCommand(UINT nId, int nCode, HWND hWnd)
{
	if (hWnd == m_btnOk)
	{
		DoOK();
	}
	else if (hWnd == m_btnCancel)
	{
		DoCancel();
	}
}


LRESULT CChatInviteDialog::OnKeyDownEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_CONTROL)
	{
		m_bIsCtrlKeyPressed = TRUE;
	}
	
	return 0L;
}


LRESULT CChatInviteDialog::OnKeyUpEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((UINT)wParam == VK_ESCAPE)
	{
		m_pFindContactPanel->SendMessage(CFindContactPanel::WM_FIND_PANEL_ESC_PRESSED, 0, 0);
		return 0L;
	}
	
	if (wParam == VK_CONTROL)
	{
		m_bIsCtrlKeyPressed = FALSE;
		return 0L;
	}
	else if ( (wParam == (int)'F') && (m_bIsCtrlKeyPressed) )
	{
        if (m_pFindContactPanel->IsWindow())
        {
            m_pFindContactPanel->SetFocus();
        }
		return 0L;
	}
	
	return 0L;
}


LRESULT CChatInviteDialog::OnCharEventNotifier(
		UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR ch = wParam;
	char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*()[{}]/?\\|";
	
	if (strchr(validChars, ch))
	{
		CString strText(ch);
		
        if (m_pFindContactPanel->IsWindow())
        {
            m_pFindContactPanel->SetFocus();
			m_pFindContactPanel->SetText(strText);
        }
	}
	
	return 0L;
}


LRESULT CChatInviteDialog::OnAcceleratorFindContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_pFindContactPanel->SetFocus();

    return S_OK;
}
