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

#include "TabbedWindow.h"
#include "ButtonTabCtrl.h"

#include "../skin/SolidBackgroundElement.h" 
#include "../skin/PropPageElement.h"

#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"

#include "../image/image.h"
#include "../image/imagebuilder.h"
#include "../skin/ElementBuilder.h"
#include "../UOLToolbarBuilder.h"

#include "AccountsPropPage.h"
#include "ContactsPropPage.h"
#include "MessagesPropPage.h"
#include "StatusPropPage.h"
#include "SoundsPropPage.h"
#include "GeneralPropPage.h"
#include "EmoticonsPropPage.h"
#include "NetworkPropPage.h"
#include "PluginsPropPage.h"
#include "ButtonTabCtrlBuilder.h"


CTabbedWindow::CTabbedWindow(LPCTSTR lpszTitle, int nActivePage, HWND hWndParent) :
	m_strTitle(lpszTitle),
	m_wndParent(hWndParent),
	m_nActivePage(nActivePage),
	m_pActiveButton(NULL)
{
	m_fontButton.CreateFont(12, 0, 1,
		1, FW_NORMAL, FALSE, FALSE,
		FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("MS Shell Dlg"));

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	if (pUIManager)
	{
		m_pPluginPreferencesPageContainer = ap_dynamic_cast<CPluginPreferencesPageContainerPtr>(pUIManager->GetPluginPreferencesPageContainer());
		if (m_pPluginPreferencesPageContainer)
		{
			m_pPluginPreferencesPageContainer->RegisterObserver(this);
		}
	}
}


CTabbedWindow::~CTabbedWindow()
{
	if (m_pPluginPreferencesPageContainer)
	{
		m_pPluginPreferencesPageContainer->UnregisterObserver(this);
	}
	
	m_arrPages.RemoveAll();

	if ( m_btnOk.IsWindow() )
	{
		m_btnOk.DestroyWindow();
	}

	if ( m_btnCancel.IsWindow() )
	{
		m_btnCancel.DestroyWindow();
	}

	if ( m_btnApply.IsWindow() )
	{
		m_btnApply.DestroyWindow();
	}
}


BOOL CTabbedWindow::AddPage(HWND hWndPage)
{
	BOOL bRet = FALSE;	

	if ( ::IsWindow(hWndPage) )
	{
		PageInfoPtr pPageInfo = new PageInfo;

		pPageInfo->wndPage = hWndPage;
		
		bRet = m_arrPages.Add(pPageInfo);

		if (IsWindow())
		{
			CRect rect;

			// Resize page
			GetPageRect(rect);
			pPageInfo->wndPage.MoveWindow(rect, TRUE);

			pPageInfo->wndPage.SetParent(m_hWnd);

			DWORD dwStyle = pPageInfo->wndPage.GetStyle();
			pPageInfo->wndPage.ModifyStyle(0, WS_OVERLAPPED);

			// Hide, if not first page
			if (m_arrPages.GetSize() > 1)
			{
				pPageInfo->wndPage.ShowWindow(SW_HIDE);

				pPageInfo->wndPage.SetWindowPos(HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE /*| SWP_HIDEWINDOW*/);
			}

			CString strTitle;
			pPageInfo->wndPage.GetWindowText(strTitle);
			CImageButtonBasePtr pImageButton = m_pToolbar->FindImageButton(strTitle);
			ATLASSERT(pImageButton.get());
			if ( pImageButton.get() )
			{		
				pImageButton->SetAttachment(pPageInfo->wndPage.m_hWnd);
			}
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}

	return bRet;
}


BOOL CTabbedWindow::RemovePage(HWND hWndPage)
{
	int nIndex = GetPageIndex(hWndPage);

	if (nIndex != TW_INDEX_NOT_FOUND)
	{
		return RemovePage(nIndex);
	}

	return FALSE;
}


BOOL CTabbedWindow::RemovePage(int nIndex)
{
	int nPageCount = m_arrPages.GetSize();

	if ((nIndex < 0) || (nIndex >= nPageCount))
	{
		// Invalid index
		return FALSE;
	}

	PageInfoPtr pPageInfo = m_arrPages[nIndex];

	BOOL bRet = m_arrPages.RemoveAt(nIndex);
	
	if (bRet)
	{
		// Adjust active page index...
		if (1 == nPageCount)
		{
			// No more pages
			m_nActivePage = 0;
		}
		else if ((nIndex <= m_nActivePage) && (m_nActivePage > 0))
		{
			m_nActivePage--;
		}

		ATLASSERT(m_nActivePage >= 0);

		// UI update...
		if (IsWindow())
		{
			// Hide removed page
			pPageInfo->wndPage.ShowWindow(SW_HIDE);
			
			CString strTitle;
			pPageInfo->wndPage.GetWindowText(strTitle);
			CImageButtonBasePtr pImageButton = m_pToolbar->FindImageButton(strTitle);
			ATLASSERT(pImageButton.get());
			if ( pImageButton.get() )
			{		
				m_pToolbar->RemoveExternalButton(pImageButton);
			}

			POSITION pos = m_listPropPages.GetHeadPosition();
			while (pos)
			{
				IUOLMessengerPreferencesPagePtr& pPrefsPage = m_listPropPages.GetAt(pos);
				ATLASSERT( pPrefsPage.get() );
				if ( pPrefsPage->GetHWND() == pPageInfo->wndPage )
				{
					m_listPropPages.RemoveAt(pos);
					break;
				}

				m_listPropPages.GetNext(pos);
			}

			SetActivePageIndex(m_nActivePage);
		}

		ATLTRACE(_T("Removed page index: %d\n"), nIndex);
	}

	return bRet;
}


void CTabbedWindow::RemoveAll()
{
	// UI update...
	int nPageCount = m_arrPages.GetSize();
	for (int nIndex = 0; nIndex < nPageCount; nIndex++)
	{
		if (m_arrPages[nIndex]->wndPage.IsWindow())
		{
			m_arrPages[nIndex]->wndPage.ShowWindow(SW_HIDE);
			m_arrPages[nIndex]->wndPage.SendMessage(WM_CLOSE);
		}
	}

	m_arrPages.RemoveAll();
	m_nActivePage = 0;

	if (IsWindow())
	{
		UpdateWindow();
	}
}


int CTabbedWindow::GetPageCount()
{
	return m_arrPages.GetSize();
}


int CTabbedWindow::GetActivePageIndex()
{
	if (m_arrPages.GetSize() > 0)
	{
		return m_nActivePage;
	}

	return TW_INDEX_NOT_FOUND;
}


void CTabbedWindow::SetActivePageIndex(int nIndex)
{
	if ((nIndex > TW_INDEX_NOT_FOUND) && (nIndex < m_arrPages.GetSize()))
	{
		if ( IsWindow() )
		{
			UpdateActivePageIndex(nIndex);
		}
	}
}


void CTabbedWindow::SetActivePage(HWND hWndPage)
{
	int nIndex = GetPageIndex(hWndPage);

	if (nIndex != TW_INDEX_NOT_FOUND)
	{
		SetActivePageIndex(nIndex);
	}
}


BOOL CTabbedWindow::HandlePropPageCommand(
		IUOLMessengerPropertyPageCommandPtr pPropPageCommand)
{
	BOOL result = m_arrPages[m_nActivePage]->wndPage.SendMessage(
			CAccountsPropPage::WM_NEW_ACCOUNT, 0, 
			reinterpret_cast<LPARAM>(pPropPageCommand.get()));
	
	return result;
}


void CTabbedWindow::SetTitle(LPCTSTR lpszTitle)
{
	if (lpszTitle != NULL)
	{
		m_strTitle = lpszTitle;

		// UI update
		CWindowImpl<CTabbedWindow>::SetWindowText(m_strTitle);
	}
}


BOOL CTabbedWindow::PreTranslateMessage(MSG* pMsg)
{
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
	return IsDialogMessage(pMsg);
}


BOOL CTabbedWindow::CreatePages()
{
	CElementBuilderPtr pBuilder;	

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
	{
		pBuilder = ap_dynamic_cast<CPreferencesPropPageBuilderPtr> (m_listChilds.GetNext(pos));

		if ( pBuilder ) 
		{
			CPreferencesPropPagePtr pPropPage = ap_dynamic_cast<CPreferencesPropPagePtr>(pBuilder->CreateElement());			
			ATLASSERT(pPropPage.get());
			
			if ( pPropPage.get() )
			{
				m_listPropPages.AddTail(pPropPage);
				pPropPage->Create(this, NULL);
				AddPage(*pPropPage);
			}
		}		
	}

	SetActivePageIndex(0);

//////////////// DEBUG	
	//SetActivePageIndex(m_listPropPages.GetCount() - 1);
//////////////// END DEBUG	

	return TRUE;
}

int CTabbedWindow::GetPageIndex(HWND hWndPage)
{
	for (int nIndex = 0; nIndex < m_arrPages.GetSize(); nIndex++)
	{
		if (m_arrPages[nIndex]->wndPage == hWndPage)
		{
			return nIndex;
		}
	}

	return TW_INDEX_NOT_FOUND;
}


int CTabbedWindow::GetPageIndex(const CString& strPageTitle)
{
	for (int nIndex = 0; nIndex < m_arrPages.GetSize(); nIndex++)
	{
		CString str;

		m_arrPages[nIndex]->wndPage.GetWindowText(str);

		if (strPageTitle == str)
		{
			return nIndex;
		}
	}

	return TW_INDEX_NOT_FOUND;
}


void CTabbedWindow::GetPageRect(CRect& rect)
{
	GetClientRect(rect);

	if ( m_pToolbar.get() )
	{
		CSize sizeToolbar;
		m_pToolbar->GetMinSize(sizeToolbar);

		rect.top += sizeToolbar.cy;
		rect.bottom -= 45;
	}
}


LRESULT CTabbedWindow::OnCreate(LPCREATESTRUCT lpCs)
{
	// Create button tab control
	CRect rect = CRect(CPoint(0, 0), CSize(110, 110));
	

	CMessageLoop* pLoop = _Module.GetMessageLoop();

    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
	
	CreateToolbar();
	
	CreatePages();

	AddAllPluginsPreferencesPage();

	CString strCaption;

	m_btnOk.Create(m_hWnd, CRect(0, 0, 0, 0),  NULL, 
		WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_PUSHBUTTON , 0, ID_PREF_OK);
	m_btnOk.SetFont(m_fontButton);
	strCaption.LoadString(IDS_ACCOUNT_EDIT_DIALOG_OK_BUTTON);
	m_btnOk.SetWindowText(strCaption);

	m_btnCancel.Create(m_hWnd, CRect(0, 0, 0, 0),  NULL, 
		WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_PUSHBUTTON, 0, ID_PREF_CANCEL);
	m_btnCancel.SetFont(m_fontButton);
	strCaption.LoadString(IDS_ACCOUNT_EDIT_DIALOG_CANCEL_BUTTON);
	m_btnCancel.SetWindowText(strCaption);

	m_btnApply.Create(m_hWnd, CRect(0, 0, 0, 0),  NULL, 
		WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_PUSHBUTTON | WS_DISABLED, 0, ID_PREF_APPLY);
	m_btnApply.SetFont(m_fontButton);
	strCaption.LoadString(IDS_APPLY);
	m_btnApply.SetWindowText(strCaption);

	UpdateWindow();

	return 0;
}


void CTabbedWindow::OnDestroy()
{
	for (int nIndex = 0; nIndex < m_arrPages.GetSize(); nIndex++)
	{
		m_arrPages[nIndex]->wndPage.SendMessage(WM_TAB_KILLACTIVE);
	}

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	if (pLoop != NULL)
	{
		pLoop->RemoveMessageFilter(this);
	}
		
	m_listPropPages.RemoveAll();
	m_listTempPluginPropPages.RemoveAll();
}


void CTabbedWindow::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());

	// Resize toolbar

	CSize sizeToolbar;
	if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);

		m_pToolbar->SetWindowPos(NULL, 
								 0, 0, 
								 size.cx, sizeToolbar.cy, 
								 SWP_NOMOVE | SWP_NOZORDER); 			
	}

	// Resize pages
	CRect rectPage;
	GetPageRect(rectPage);

	int nPageCount = m_arrPages.GetSize();
	for (int nIndex = 0; nIndex < nPageCount; nIndex++)
	{
		m_arrPages[nIndex]->wndPage.MoveWindow(rectPage, TRUE);
	}

	rect.top += sizeToolbar.cy + rectPage.Height();

	CPoint centerPt = rect.CenterPoint();

	m_btnOk.SetWindowPos(0, centerPt.x - 170, centerPt.y - 12, 100, 24, SWP_NOZORDER);
	m_btnCancel.SetWindowPos(0, centerPt.x - 50, centerPt.y - 12, 100, 24, SWP_NOZORDER);	
	m_btnApply.SetWindowPos(0, centerPt.x + 70, centerPt.y -12, 100, 24, SWP_NOZORDER);
}


void CTabbedWindow::UpdateActivePageIndex(int nIndex)
{
	// Show/hide pages...
	m_arrPages[nIndex]->wndPage.ShowWindow(SW_SHOW);
	//m_arrPages[nIndex]->wndPage.SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	if (nIndex != m_nActivePage)
	{
		//m_arrPages[m_nActivePage]->wndPage.SetWindowPos(HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		m_arrPages[m_nActivePage]->wndPage.ShowWindow(SW_HIDE);
		m_arrPages[m_nActivePage]->wndPage.SendMessage(WM_TAB_KILLACTIVE);

		m_arrPages[nIndex]->wndPage.SetFocus();
	}

	m_arrPages[nIndex]->wndPage.SendMessage(WM_TAB_SETACTIVE);

	
	if ( NULL != m_pActiveButton )
	{
		m_pActiveButton->SetPressed(FALSE);
	}

	CString strTitle;
	m_arrPages[nIndex]->wndPage.GetWindowText(strTitle);
	CImageButtonBasePtr pImageButton = m_pToolbar->FindImageButton(strTitle);
	ATLASSERT(pImageButton.get());
	if ( pImageButton.get() )
	{
		pImageButton->SetPressed(TRUE);	
	}

	UpdateWindow();

	m_nActivePage = nIndex;
	m_pActiveButton = pImageButton.get();
}


LRESULT CTabbedWindow::OnOkBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if ( m_btnApply.IsWindowEnabled() )
	{
		ApplyChanges();
	}

	GetParent().SendMessage(WM_CLOSE);

	return 0;
}


LRESULT CTabbedWindow::OnCancelBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	GetParent().SendMessage(WM_CLOSE);

	return 0;
}


LRESULT CTabbedWindow::OnApplyBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_btnApply.EnableWindow(FALSE);
	
	ApplyChanges();

	return 0;
}


void CTabbedWindow::ApplyChanges()
{
	POSITION pos = m_listPropPages.GetHeadPosition();
	while(pos)
	{
		m_listPropPages.GetNext(pos)->OnApply();
	}

	IUOLMessengerSettingsManagerPtr pSettingsManager = CUIMApplication::GetApplication()->GetSettingsManager();
	if (pSettingsManager != NULL)
	{
		pSettingsManager->SaveSettings();
	}
}


LRESULT CTabbedWindow::OnPrefsChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( !m_btnApply.IsWindowEnabled() )
	{
		m_btnApply.EnableWindow(TRUE);
	}

	return 0L;
}


void CTabbedWindow::CreateToolbar()
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


void CTabbedWindow::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	IUOLMessengerImageButton2* pButton2 = static_cast<IUOLMessengerImageButton2*>(pButton);
	ATLASSERT(pButton2);
	HWND hwndPage = (HWND)pButton2->GetAttachment();
	ATLASSERT(hwndPage);
	SetActivePage(hwndPage);
}


void CTabbedWindow::RemovePluginPage(const CString& strPluginName)
{
	CImageButtonBasePtr pImageButton = m_pToolbar->FindImageButton(strPluginName);
	if ( pImageButton.get() )
	{
		RemovePage((HWND)pImageButton->GetAttachment());
	}
}


IUOLMessengerImageButtonPtr CTabbedWindow::CreateToolbarButton()
{
	return m_pToolbar->CreateExternalButton();
}


void CTabbedWindow::OnAddPage(IUOLMessengerPluginPreferencesPagePtr pPluginPage)
{
	AddPluginsPreferencesPage(pPluginPage);
}


void CTabbedWindow::OnRemovePage(IUOLMessengerPluginPreferencesPagePtr pPluginPage)
{
	RemovePluginsPreferencesPage(pPluginPage);
}


void CTabbedWindow::AddAllPluginsPreferencesPage()
{
	if (m_pPluginPreferencesPageContainer)
	{
		CAtlList<IUOLMessengerPluginPreferencesPagePtr> listPages;
		m_pPluginPreferencesPageContainer->GetPages(listPages);

		for (POSITION pos = listPages.GetHeadPosition(); pos; )
		{
			IUOLMessengerPluginPreferencesPagePtr pPluginPage = listPages.GetNext(pos);
			AddPluginsPreferencesPage(pPluginPage);
		}
	}
}


void CTabbedWindow::AddPluginsPreferencesPage(IUOLMessengerPluginPreferencesPagePtr pPluginPage)
{
	IUOLMessengerPreferencesPage2Ptr pPage = pPluginPage->GetPage();
	IUOLMessengerImagePtr pImage = pPluginPage->GetImage();
	ATLASSERT(pPage.get());
	ATLASSERT(pImage.get());

	IUOLMessengerImageButton2Ptr pButton2 = ap_static_cast<IUOLMessengerImageButton2Ptr>(CreateToolbarButton());
	ATLASSERT(pButton2.get());

	if (pPage.get() && pButton2.get())
	{
		pPage->Create(m_hWnd);
		
		pButton2->SetTip(pPluginPage->GetName());
		pButton2->SetImage(pImage);
		pButton2->SetWidth(44);
		pButton2->SetAuto(FALSE);
		pButton2->SetDrawTransparent(TRUE);
		pButton2->SetCaption(pPluginPage->GetName());
		pButton2->SetCommandName(pPluginPage->GetName());
		pButton2->SetAttachment(pPage->GetHWND());
		pButton2->AddEventListener(this);

		m_pToolbar->AddExternalButton(pButton2);

		m_listPropPages.AddTail(pPage);
		AddPage(pPage->GetHWND());
	}
}


void CTabbedWindow::RemovePluginsPreferencesPage(IUOLMessengerPluginPreferencesPagePtr pPluginPage)
{
	RemovePluginPage(pPluginPage->GetName());
}
