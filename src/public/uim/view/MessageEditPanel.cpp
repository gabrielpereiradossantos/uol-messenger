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
#include "MessageEditPanel.h"

#include "../resource.h"
#include "../urlbuilder.h"
#include "../controller/UIMApplication.h"
#include "image/image.h"
#include "image/imagebuilder.h"
#include "UOLToolbarBuilder.h"
#include "MessageEdit/EmoticonsFrameBuilder.h"
#include "skin/SolidBackgroundElement.h"
#include "ImageButton.h"
#include "../controller/UIManager.h"
#include "../controller/EmoticonParserManager.h"


//
// CMessageEditPanel implementation
//
CMessageEditPanel::CMessageEditPanel() //: 
{
	COLORREF bkg;
	bkg = RGB(255, 255, 255);

	CSolidBackgroundElementPtr pBkg = new CSolidBackgroundElement();		
	pBkg->SetColor(bkg);	

	m_pActiveBackground = pBkg;
	m_pInactiveBackground = pBkg;	
	m_pCurrentBackground = m_pActiveBackground.get();

	/*
	IUOLMessengerSettingsManager3Ptr pSettingsManager3 = ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager3);
	if (pSettingsManager3)
	{
		pSettingsManager3->RegisterObserver(this);
	}
	*/
}


CMessageEditPanel::~CMessageEditPanel()
{
	/*
	IUOLMessengerSettingsManager3Ptr pSettingsManager3 = ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager3);
	if (pSettingsManager3)
	{
		pSettingsManager3->UnregisterObserver(this);
	}
	*/
}


int CMessageEditPanel::GetEditTextLength()
{
	return m_editRichMessage.GetTextLength();
}


CString CMessageEditPanel::GetEditText()
{
	return m_editRichMessage.GetEditHTMLText();
}

void CMessageEditPanel::ClearEditText()
{
	m_editRichMessage.ClearEditText();
}

void CMessageEditPanel::SetRichListener(CRichTextEventsEventListener * listener)
{
	//ATLASSERT(listener!=NULL);
	m_editRichMessage.SetListener(listener);
}

void CMessageEditPanel::ShowInputFocus()
{
	if (this->IsWindow())
		m_editRichMessage.SetFocus();
	
}

CString CMessageEditPanel::GetName() const
{
	return "__UOLMessageEditInstance_" ;
}

CString CMessageEditPanel::GetEditPlainText()
{
	return m_editRichMessage.GetEditPlainText();
}

void CMessageEditPanel::SetEditPlainText(CString strPlainText)
{
	m_editRichMessage.SetEditPlainText(strPlainText);
}

LRESULT CMessageEditPanel::OnCreate(LPCREATESTRUCT lpCs)
{
	CreateToolbar();

	m_strSendButtonText.LoadString(IDS_MESSAGE_WINDOW_SEND_BUTTON);

	CRect rectClient;
	GetClientRect(&rectClient);

	CRect buttonRect;
	CRect richRect;
	CRect menuRect;

	buttonRect = CRect(CPoint(0, 0), CSize(70, 0));
	richRect = CRect(CPoint(CONTROL_PADDING, CONTROL_PADDING), CSize(0, 0));

	m_btnSend.Create(m_hWnd, buttonRect , m_strSendButtonText, WS_CHILDWINDOW | WS_VISIBLE);
	m_btnSend.SetNotifyingWindow(m_hWnd);
	
	m_editRichMessage.Create(m_hWnd, richRect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE /*| ES_WANTRETURN*/ | ES_NOHIDESEL | ES_SAVESEL, 0);
	m_editRichMessage.LoadFontSettings();
	ATLASSERT(m_editRichMessage.SetOleCallback(&m_richCallBack)!=0);
	UpdateToolbar(); 


	m_editRichMessage.ShowWindow(SW_NORMAL);
	m_editRichMessage.LimitText(MAX_CHARACTER_MESSAGE_UOL);
	long eventmask = m_editRichMessage.GetEventMask();
	m_editRichMessage.SetEventMask(ENM_LINK|ENM_CHANGE);
	m_editRichMessage.SetAutoURLDetect(TRUE);
	m_editRichMessage.SetTextMode(TM_PLAINTEXT);

	return 0L;
}


void CMessageEditPanel::OnDestroy()
{
}


void CMessageEditPanel::OnCommand(UINT nId, int nCode, HWND hWnd)
{
	if (hWnd == m_btnSend)
	{
		ATLTRACE(_T("CMessageEditPanel::OnCommand() - MEP_EDIT_RETURN\n"));
		GetParent().SendMessage(WM_COMMAND, MAKEWPARAM(0, MEP_EDIT_RETURN), (LPARAM) m_hWnd);
	}
}


LRESULT CMessageEditPanel::OnCharEventNotifier(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((UINT)wParam == VK_RETURN)
	{
		ATLTRACE(_T("CMessageEditPanel::OnSendMessage()\n"));
		GetParent().SendMessage(WM_COMMAND, MAKEWPARAM(0, MEP_EDIT_RETURN), (LPARAM) m_hWnd);
	}
	
	return 0L;
}


void CMessageEditPanel::CreateToolbar()
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
	}

	if (m_pToolbar.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pToolbar->Create(m_hWnd, rect);			
		ATLASSERT(m_pToolbar.get());
		m_pToolbar->AddButtonsEventListener(this); 

	}
}


void CMessageEditPanel::UpdateToolbar()
{
	if (m_pToolbar.get())
	{
		CImageButtonBasePtr button;
		button = m_pToolbar->FindImageButton(_T("bold"));
		if (button.get())
			button->SetPressed(m_editRichMessage.IsBold());
	
		button = m_pToolbar->FindImageButton(_T("italic"));
		if (button.get())
			button->SetPressed(m_editRichMessage.IsItalic());

		button = m_pToolbar->FindImageButton(_T("underline"));
		if (button.get())
			button->SetPressed(m_editRichMessage.IsUnderline());

		button = m_pToolbar->FindImageButton(_T("Strikeout"));
		if (button.get())
			button->SetPressed(m_editRichMessage.IsStrikeout());

		m_pToolbar->InvalidateRect(NULL);
		m_pToolbar->UpdateWindow();
	}
}


void CMessageEditPanel::UpdateLayout(BOOL bResizeBars)
{
	// TODO: O tamanho do botão pode variar?
	const int button_max_cy=40;
	const int button_min_cy=40;
	int button_cy;
	__super::UpdateLayout(bResizeBars);
	CRect rect;
	int toolbarHeight;
	CSize sizeToolbar;
	GetClientRect(&rect);

	CRect rectButton;
	m_btnSend.GetWindowRect(&rectButton);

	int cx = max((rect.Width() - rectButton.Width() - (3 * CONTROL_PADDING)), CONTROL_PADDING);
	int cy = rect.Height() - (2 * CONTROL_PADDING);

	if (cy < 0)
		cy = 0;

	toolbarHeight = 0;
	if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);
		toolbarHeight = sizeToolbar.cy;
		m_pToolbar->SetWindowPos(NULL, 
			0, 0, 
			rect.Width(), sizeToolbar.cy, 
			SWP_NOZORDER); 			
	}

	m_editRichMessage.SetWindowPos(NULL,
		0 , toolbarHeight, 
		cx, rect.Height()-toolbarHeight,  
		SWP_NOZORDER);
	button_cy = rect.Height() - sizeToolbar.cy - CONTROL_PADDING*2;
	if (button_cy<button_min_cy)
		button_cy = button_min_cy;
/*	m_btnSend.SetWindowPos(NULL, 
		cx + (2 * CONTROL_PADDING), (toolbarHeight+rect.Height()-button_cy)/2, 
		rectButton.Width(), button_cy, 
		SWP_NOZORDER);*/
	m_btnSend.SetWindowPos(NULL, 
		cx + (2 * CONTROL_PADDING), (toolbarHeight+CONTROL_PADDING), 
		rectButton.Width(), button_cy, 
		SWP_NOZORDER);

}


void CMessageEditPanel::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	CString cmdName = pButton->GetCommandName();
	if (cmdName.CompareNoCase(_T("Bold"))==0) {
		m_editRichMessage.SetBold(pButton->IsPressed());
	} else if (cmdName.CompareNoCase(_T("Italic"))==0) {
		m_editRichMessage.SetItalic(pButton->IsPressed());
	} else if (cmdName.CompareNoCase(_T("Strikeout"))==0) {
		m_editRichMessage.SetStrikeout(pButton->IsPressed());
	} else if (cmdName.CompareNoCase(_T("Underline"))==0) {
		m_editRichMessage.SetUnderline(pButton->IsPressed());
	} else if (cmdName.CompareNoCase(_T("TextColor"))==0) {
		m_editRichMessage.ChangeFontTextColor();
	} else if (cmdName.CompareNoCase(_T("TextBkColor"))==0) {
		m_editRichMessage.ChangeFontTextBkColor();
	} else if (cmdName.CompareNoCase(_T("Font"))==0) {
		m_editRichMessage.ChooseFont();
		UpdateToolbar();
	} else if (cmdName.CompareNoCase(_T("Emoticons"))==0) {

		CPoint ptScreen = ptClick;
		::ClientToScreen(m_hWnd, &ptScreen);

		ShowEmoticonsWindow(ptScreen);
	} else if (cmdName.CompareNoCase(_T("NudgeContact"))==0) {
		
		SendMessage(GetParent(), WM_NUDGE_BUTTON_CLICK, 0, 0);
	}
}

void CMessageEditPanel::OnConnected()
{
	m_btnSend.EnableWindow(TRUE);
}

void CMessageEditPanel::OnDisconnected()
{
	m_btnSend.EnableWindow(FALSE);
}

void CMessageEditPanel::OnReportDisconnect()
{
	m_btnSend.EnableWindow(FALSE);
}

void CMessageEditPanel::AddEmoticon(CString textTag)
{
	m_editRichMessage.InsertTagText(textTag);
	m_editRichMessage.Parse();

}

IUOLMessengerImageButtonContainerPtr CMessageEditPanel::GetToolbar() const
{
	return m_pToolbar;
}


void CMessageEditPanel::OnEmoticonSettingsChanged()
{
	//CEmoticonParserManager::GetInstance()->UpdateTags();
}


void CMessageEditPanel::SelectAllText()
{
	m_editRichMessage.SetSel(0, -1);
}


HWND CMessageEditPanel::GetRichEditCtrlHandle()
{
	return m_editRichMessage.m_hWnd;
}


HWND CMessageEditPanel::GetSendButtonCtrlHandle()
{
	return m_btnSend.m_hWnd;
}


void CMessageEditPanel::ShowEmoticonsWindow(const CPoint& ptClick)
{
	CPoint ptOrigin = ptClick;

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	CEmoticonsFrameBuilderPtr pBuilder = ap_dynamic_cast<CEmoticonsFrameBuilderPtr>(pUIManager->GetWindowManager()->GetElement("__EmoticonsWindowBuilderInstance"));

	m_pEmoticonsFrame = ap_dynamic_cast<CEmoticonsFramePtr>(pBuilder->CreateElement());

	CSize sizeBorder;

	m_pEmoticonsFrame->GetBorderSize(sizeBorder);

	CSize sizeWindow(321 + sizeBorder.cx, 246 + sizeBorder.cy);

	ptOrigin.y -= sizeWindow.cy;

	HMONITOR hMonitor = ::MonitorFromPoint(ptOrigin, MONITOR_DEFAULTTONEAREST);

	CRect rectDesktop;
	if (NULL != hMonitor)
	{
		MONITORINFO mi;
		memset(&mi, 0, sizeof(MONITORINFO));
		mi.cbSize = sizeof(MONITORINFO);
		::GetMonitorInfo(hMonitor, &mi);
		rectDesktop = mi.rcWork;
	}
	else
	{
		::GetClientRect(::GetDesktopWindow(), &rectDesktop);
	}
	
	if ((ptOrigin.x + sizeWindow.cx) > rectDesktop.right)
	{
		ptOrigin.x = rectDesktop.right - sizeWindow.cx - 5;
	}

	m_pEmoticonsFrame->SetConversation(m_pConversation);
	m_pEmoticonsFrame->Create(m_hWnd, CRect(ptOrigin, sizeWindow),_T("Emoticons"));

	m_pEmoticonsFrame->ShowWindow(SW_SHOW);
	m_pEmoticonsFrame->RegisterMessageEditPanel(this);
}

void CMessageEditPanel::UpdateMessageEditToolbarButtonVisible(const CString& strButtonCommand, BOOL bVisible)
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

void CMessageEditPanel::SetConversation(IUOLMessengerConversationPtr pConversation)
{
	m_pConversation = pConversation;
	
	m_editRichMessage.SetConversation(m_pConversation);
}

