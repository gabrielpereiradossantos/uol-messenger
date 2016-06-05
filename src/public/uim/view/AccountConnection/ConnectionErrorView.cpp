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
#include "ConnectionErrorView.h"

#include "../../resource.h"
#include "AccountConnectionErrorEntry.h"
#include "AccountListElement.h"
#include "../skin/SolidBackgroundElement.h"
#include "../skin/FontElement.h"
#include "../../controller/UIManager.h"
#include "../../controller/UIMApplication.h"


CConnectionErrorView::CConnectionErrorView()
{
}


CConnectionErrorView::~CConnectionErrorView()
{
}


void CConnectionErrorView::AddErrorEvent(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CTime& time, const CString& strMessage)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountListView);

	if (m_wndAccountListView.IsWindow())
	{
		RemoveErrorEventImpl(pAccount);

		CAccountConnectionErrorEntryPtr pEntry = new CAccountConnectionErrorEntry(pAccount, error, time, strMessage);

		int nIndex = m_wndAccountListView.AddEntry(pEntry);

		if (nIndex >= 0)
		{
			int nCurSelection = m_wndAccountListView.GetSelectedIndex();
			
			if (nCurSelection == -1)
			{
				m_wndAccountListView.SelectItem(nIndex);
			}
		}
	}
}


void CConnectionErrorView::RemoveErrorEvent(IUOLMessengerAccountPtr pAccount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountListView);

	if (m_wndAccountListView.IsWindow())
	{
		int nRemoveIndex = RemoveErrorEventImpl(pAccount);

		if (m_wndAccountListView.GetItemCount() > 0)
		{
			m_wndAccountListView.SelectItem(0);
		}
	}
}


UINT CConnectionErrorView::GetErrorEventCount()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountListView);

	if (m_wndAccountListView.IsWindow())
	{
		return m_wndAccountListView.GetItemCount();
	}

	return 0;
}


void CConnectionErrorView::ReconnectAll()
{
	ATLTRACE(_T("%s \n"), __FUNCTION__);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	CComCritSecLock<CComAutoCriticalSection> lock(m_csAccountListView);

	int nItemCount = m_wndAccountListView.GetItemCount();

	for (int nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		IAccountListViewEntryPtr pListViewEntry = m_wndAccountListView.GetEntry(nIndex);

		if (pListViewEntry)
		{
			IUOLMessengerAccountPtr pAccount = pListViewEntry->GetAccount();

			DWORD dwOldStatus = pAccountConnectionManager->GetPreviousStatus(pAccount);

			pAccountConnectionManager->SetStatus(pAccount, dwOldStatus);

			ATLTRACE(_T("Reconnectiong %s with status = %d\n"), pAccount->GetUserId(), dwOldStatus);
			BOOL bConnect = pAccountConnectionManager->Connect(pAccount);

			ATLTRACE(_T("Reconnectiong result = %d\n"), bConnect);
		}
	}
}


CString CConnectionErrorView::GetName() const
{
	return "__ConnectionErrorViewInstance_" + m_strGUID;
}


void CConnectionErrorView::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strErrorIcon;
		GetParameterValue(strErrorIcon, pMapParams, _T("erroricon"));

		if (FALSE == strErrorIcon.IsEmpty())
		{
			IElementPtr pElement = pElementTable->GetElement(strErrorIcon);

			if (pElement)
			{
				m_pErrorIcon = ap_dynamic_cast<CStaticIconElementPtr>(pElement);
			}
		}
	}
	catch (const CSkinException&)
	{
	}

	try
	{
		CString strFontName;
		GetParameterValue(strFontName, pMapParams, _T("font"));

		m_pFontWindow = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontName));
		ATLASSERT(m_pFontWindow.get());

	}
	catch (const CSkinException&)
	{
		m_pFontWindow = new CFontElement();
		m_pFontWindow->SetFont(_T("MS Sans Serif"), 12, FALSE, FALSE);
	}

	try
	{
		CString strFontName;
		GetParameterValue(strFontName, pMapParams, _T("titlefont"));

		m_pFontTitle = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontName));
		ATLASSERT(m_pFontTitle.get());

	}
	catch (const CSkinException&)
	{
		m_pFontTitle = new CFontElement();
		m_pFontTitle->SetFont(_T("MS Sans Serif"), 12, TRUE, FALSE);
	}
}


void CConnectionErrorView::DoPaint(CDCHandle& dc)
{
	if (m_pErrorIcon)
	{
		IUOLMessengerImageDrawStrategyPtr pImageDrawStrategy = m_pErrorIcon->GetImageDrawStrategy();

		if (pImageDrawStrategy)
		{
			pImageDrawStrategy = CUIMApplication::GetApplication()->GetUIManager()->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		}

		IUOLMessengerImagePtr pImage = m_pErrorIcon->GetImage();

		if (pImage)
		{
			pImage->Draw(CDCHandle(dc), m_rectImage.left, m_rectImage.top, 
				m_rectImage.Width(), m_rectImage.Height(), pImageDrawStrategy);
		}
	}
}


void CConnectionErrorView::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);

	CSize size(rect.Width(), rect.Height());
	ResizeControls(size);
}


HWND CConnectionErrorView::CreateView(HWND hWndParent)
{
	HWND hWnd = Create(hWndParent);

	return hWnd;
}


void CConnectionErrorView::GetMinSize(CSize& size)
{
	size.cx = 420;
	size.cy = 310;
}


BOOL CConnectionErrorView::PreTranslateMessage(MSG* pMsg)
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


LRESULT CConnectionErrorView::OnCreate(LPCREATESTRUCT lpCs)
{
	CreateControls();

	return 0L;
}


void CConnectionErrorView::OnDestroy()
{
	SetMsgHandled(FALSE);
}


//void CConnectionErrorView::OnSize(UINT nType, const CSize& size)
//{
//	if (SIZE_MINIMIZED != nType)
//	{
//		ResizeControls(size);
//	}
//
//	return;
//}


//void CConnectionErrorView::OnPaint(HDC hDC)
//{
//
//}


LRESULT CConnectionErrorView::OnEraseBackground(HDC hDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	CDCHandle dc(hDC);

	dc.FillRect(rectClient, m_brushBackground);

	return 1L;
}


LRESULT CConnectionErrorView::OnCtlColorStatic(HDC hDc, HWND)
{
	::SetBkMode(hDc, TRANSPARENT);

	return (LRESULT) (HBRUSH) m_brushBackground;

	//return HOLLOW_BRUSH;
}


LRESULT CConnectionErrorView::OnCtlColorDialog(HDC hDc, HWND)
{
	::SetBkMode(hDc, TRANSPARENT);

	return HOLLOW_BRUSH;
}


void CConnectionErrorView::OnButtonClickedReconnectAll(UINT /*uCode*/, int /*nID*/, HWND hwndCtrl)
{
	CWindow wndButton = hwndCtrl;
	wndButton.EnableWindow(FALSE);

	ReconnectAll();

	CloseParentFrame();
}


void CConnectionErrorView::OnButtonClickedCancel(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CloseParentFrame();
}


LRESULT CConnectionErrorView::OnItemChangedAccountList(NMHDR *pNMHDR)
{
	OnItemChangedAccountListImpl();

	return 0L;
}


void CConnectionErrorView::CreateControls()
{
	CRect rectClient;
	GetClientRect(&rectClient);

	LOGFONTPtr pLogFont = m_pFontWindow->GetFont();
	ATLASSERT(pLogFont);

	m_fontNormal.CreateFontIndirect(pLogFont.get());

	pLogFont = m_pFontTitle->GetFont();
	ATLASSERT(pLogFont);

	m_fontTitle.CreateFontIndirect(pLogFont.get());

	if (m_pActiveBackground)
	{
		CSolidBackgroundElementPtr pSolidBackgroundElement = ap_dynamic_cast<CSolidBackgroundElementPtr>(m_pActiveBackground);

		if (pSolidBackgroundElement)
		{
			m_brushBackground.CreateSolidBrush(pSolidBackgroundElement->GetColor());
		}
	}

	if (NULL == m_brushBackground.m_hBrush)
	{
		m_brushBackground.CreateSolidBrush(RGB(193, 222, 240));
	}

	CString strWindowText;

	DWORD dwChildStyles = WS_CHILD | WS_VISIBLE;

	CRect rectImage;
	rectImage = CRect(CPoint(DM_INTERNAL_SPACE, DM_INTERNAL_SPACE), CSize(DM_IMAGE_WIDTH, DM_IMAGE_HEIGHT));
	m_rectImage = rectImage;

	CRect rectStatic;
	CStatic wndStatic;

	// Static controls...
	rectStatic = rectImage;

	rectStatic.left = rectImage.right + DM_INTERNAL_SPACE;
	rectStatic.right = rectClient.right - DM_INTERNAL_SPACE;
	rectStatic.top = rectStatic.bottom - DM_STATIC_BASE_HEIGHT;

	strWindowText.LoadString(IDS_CONNECTION_ERROR_DIALOG_STATIC_MESSAGE);
	wndStatic.Create(m_hWnd, rectStatic, strWindowText, dwChildStyles, 0, CConnectionErrorView::IDC_STATIC_MESSAGE);
	wndStatic.SetFont(m_fontNormal);
	wndStatic.Detach();
	
	//m_wndStaticMessage1.Create(m_hWnd, rectStatic, strWindowText, dwChildStyles, 0, CConnectionErrorView::IDC_STATIC_MESSAGE);
	//m_wndStaticMessage1.SetFont(m_fontNormal);
	//m_wndStaticMessage1.SetColor(RGB(255, 0, 0));

	rectStatic.bottom = rectStatic.top;
	rectStatic.top = rectStatic.bottom - DM_STATIC_BASE_HEIGHT;

	strWindowText.LoadString(IDS_CONNECTION_ERROR_DIALOG_STATIC_TITLE);
	wndStatic.Create(m_hWnd, rectStatic, strWindowText, dwChildStyles, 0, CConnectionErrorView::IDC_STATIC_TITLE);
	wndStatic.SetFont(m_fontTitle);
	wndStatic.Detach();

	// Account list...
	CRect rectList = rectImage;
	rectList.top = rectImage.bottom + (2 * DM_CONTROL_VERTICAL_SPACE);
	rectList.bottom = rectList.top + DM_ACCOUNT_LIST_HEIGHT;
	rectList.right = rectClient.right - DM_INTERNAL_SPACE;

	m_wndAccountListView.Create(m_hWnd, rectList, NULL, 
		dwChildStyles | LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS | LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING | WS_TABSTOP, 
		WS_EX_CLIENTEDGE, CConnectionErrorView::IDC_LIST_VIEW_ACCOUNTS);

	CAccountListElementPtr pAccountListElement = FindElement<CAccountListElementPtr>();

	ATLASSERT(pAccountListElement);
	if (pAccountListElement)
	{
		m_wndAccountListView.SetNormalBackground1(pAccountListElement->GetNormalBackground1());
		m_wndAccountListView.SetNormalBackground2(pAccountListElement->GetNormalBackground2());
		m_wndAccountListView.SetSelectedBackground(pAccountListElement->GetSelectedBackground());
		m_wndAccountListView.SetFocusBackground(pAccountListElement->GetFocusBackground());
		m_wndAccountListView.SetNormalFont(pAccountListElement->GetNormalFont());
		m_wndAccountListView.SetSelectedFont(pAccountListElement->GetSelectedFont());
	}
	else
	{
		m_wndAccountListView.SetNormalBackground1(new CSolidBackgroundElement(RGB(255, 255, 255)));
		m_wndAccountListView.SetNormalBackground2(new CSolidBackgroundElement(RGB(237, 243, 254)));
		m_wndAccountListView.SetSelectedBackground(new CSolidBackgroundElement(RGB(10, 36, 106)));
		m_wndAccountListView.SetFocusBackground(new CSolidBackgroundElement(RGB(212, 208, 200)));

		CFontElementPtr pFontNormal = new CFontElement();
		pFontNormal->SetFont(_T("MS Sans Serif"), 12, FALSE, FALSE);
		pFontNormal->SetColor(RGB(0, 0, 0));

		CFontElementPtr pFontSelected = new CFontElement();
		pFontSelected->SetFont(_T("MS Sans Serif"), 12, FALSE, FALSE);
		pFontSelected->SetColor(RGB(255, 255, 255));

		m_wndAccountListView.SetNormalFont(pFontNormal);
		m_wndAccountListView.SetSelectedFont(pFontSelected);
	}

	// Disconnection reason static controls...
	rectStatic = rectList;
	rectStatic.top = rectStatic.bottom + DM_CONTROL_VERTICAL_SPACE;
	rectStatic.bottom = rectStatic.top + DM_STATIC_BASE_HEIGHT;

	strWindowText.LoadString(IDS_CONNECTION_ERROR_DIALOG_STATIC_REASON);
	wndStatic.Create(m_hWnd, rectStatic, strWindowText, dwChildStyles | SS_WORDELLIPSIS, 0, CConnectionErrorView::IDC_STATIC_REASON_CAPTION);
	wndStatic.SetFont(m_fontNormal);
	wndStatic.Detach();

	rectStatic.top = rectStatic.bottom;
	rectStatic.bottom = rectStatic.top + (3 * DM_STATIC_BASE_HEIGHT);

	m_wndStaticReasonMessage.Create(m_hWnd, rectStatic, "", dwChildStyles);
	m_wndStaticReasonMessage.SetFont(m_fontNormal);

	CButton wndButton;
	CRect rectButton;

	// Reconnect button...
	rectButton = rectStatic;
	
	int nButtonsWidth = DM_BUTTON_RECONNECT_WIDTH + (2 * DM_CONTROL_HORIZONTAL_SPACE) + DM_BUTTON_CANCEL_WIDTH;
	int nLeft = ((rectClient.right - DM_INTERNAL_SPACE) - nButtonsWidth) / 2;
	
	rectButton.left = nLeft;
	rectButton.right = rectButton.left + DM_BUTTON_RECONNECT_WIDTH;
	rectButton.top = rectStatic.bottom + DM_CONTROL_VERTICAL_SPACE;
	rectButton.bottom = rectButton.top + DM_BUTTON_BASE_HEIGHT;

	strWindowText.LoadString(IDS_CONNECTION_ERROR_DIALOG_RECONNECT_BUTTON_CAPTION);

	wndButton.Create(m_hWnd, rectButton, strWindowText, 
			dwChildStyles | WS_TABSTOP | WS_GROUP, 0, CConnectionErrorView::IDC_BUTTON_RECONNECT_ALL);
	wndButton.SetFont(m_fontNormal);
	wndButton.Detach();

	// Cancel button...
	rectButton.left = rectButton.right + (2 * DM_CONTROL_HORIZONTAL_SPACE);
	rectButton.right = rectButton.left + DM_BUTTON_CANCEL_WIDTH;

	strWindowText.LoadString(IDS_CONNECTION_ERROR_DIALOG_CANCEL_BUTTON_CAPTION);

	wndButton.Create(m_hWnd, rectButton, strWindowText, 
			dwChildStyles | WS_TABSTOP, 0, CConnectionErrorView::IDC_BUTTON_CANCEL);
	wndButton.SetFont(m_fontNormal);
	wndButton.Detach();
}


void CConnectionErrorView::ResizeControls(const CSize& size)
{
	CRect rectClient(CPoint(0, 0), size);

	CRect rectImage;
	rectImage = CRect(CPoint(DM_INTERNAL_SPACE, DM_INTERNAL_SPACE), CSize(DM_IMAGE_WIDTH, DM_IMAGE_HEIGHT));
	m_rectImage = rectImage;

	CRect rectStatic;
	CWindow wndStatic;

	// Static controls...
	rectStatic = rectImage;

	rectStatic.left = rectImage.right + DM_INTERNAL_SPACE;
	rectStatic.right = rectClient.right - DM_INTERNAL_SPACE;
	rectStatic.top = rectStatic.bottom - DM_STATIC_BASE_HEIGHT;

	wndStatic = GetDlgItem(CConnectionErrorView::IDC_STATIC_MESSAGE);
	wndStatic.SetWindowPos(NULL, rectStatic.left, rectStatic.top, rectStatic.Width(), rectStatic.Height(), SWP_NOZORDER);
	wndStatic.Detach();

	rectStatic.bottom = rectStatic.top;
	rectStatic.top = rectStatic.bottom - DM_STATIC_BASE_HEIGHT;

	wndStatic = GetDlgItem(CConnectionErrorView::IDC_STATIC_TITLE);
	wndStatic.SetWindowPos(NULL, rectStatic.left, rectStatic.top, rectStatic.Width(), rectStatic.Height(), SWP_NOZORDER);
	wndStatic.Detach();

	// Account list...
	CRect rectList = rectImage;
	rectList.top = rectImage.bottom + (2 * DM_CONTROL_VERTICAL_SPACE);
	rectList.bottom = rectList.top + DM_ACCOUNT_LIST_HEIGHT;
	rectList.right = rectClient.right - DM_INTERNAL_SPACE;

	m_wndAccountListView.SetWindowPos(NULL, rectList.left, rectList.top, rectList.Width(), rectList.Height(), SWP_NOZORDER);

	// Disconnection reason static controls...
	rectStatic = rectList;
	rectStatic.top = rectStatic.bottom + DM_CONTROL_VERTICAL_SPACE;
	rectStatic.bottom = rectStatic.top + DM_STATIC_BASE_HEIGHT;

	wndStatic = GetDlgItem(CConnectionErrorView::IDC_STATIC_REASON_CAPTION);
	wndStatic.SetWindowPos(NULL, rectStatic.left, rectStatic.top, rectStatic.Width(), rectStatic.Height(), SWP_NOZORDER);
	wndStatic.Detach();

	rectStatic.top = rectStatic.bottom;
	rectStatic.bottom = rectStatic.top + (3 * DM_STATIC_BASE_HEIGHT);

	m_wndStaticReasonMessage.SetWindowPos(NULL, rectStatic.left, rectStatic.top, rectStatic.Width(), rectStatic.Height(), SWP_NOZORDER);

	CButton wndButton;
	CRect rectButton;

	// Reconnect button...
	rectButton = rectStatic;
	
	int nButtonsWidth = DM_BUTTON_RECONNECT_WIDTH + (2 * DM_CONTROL_HORIZONTAL_SPACE) + DM_BUTTON_CANCEL_WIDTH;
	int nLeft = ((rectClient.right - DM_INTERNAL_SPACE) - nButtonsWidth) / 2;
	
	rectButton.left = nLeft;
	rectButton.right = rectButton.left + DM_BUTTON_RECONNECT_WIDTH;
	rectButton.top = rectStatic.bottom + DM_CONTROL_VERTICAL_SPACE;
	rectButton.bottom = rectButton.top + DM_BUTTON_BASE_HEIGHT;

	wndButton = GetDlgItem(CConnectionErrorView::IDC_BUTTON_RECONNECT_ALL);
	wndButton.SetWindowPos(NULL, rectButton.left, rectButton.top, rectButton.Width(), rectButton.Height(), SWP_NOZORDER);
	wndButton.Detach();

	// Cancel button...
	rectButton.left = rectButton.right + (2 * DM_CONTROL_HORIZONTAL_SPACE);
	rectButton.right = rectButton.left + DM_BUTTON_CANCEL_WIDTH;

	wndButton = GetDlgItem(CConnectionErrorView::IDC_BUTTON_CANCEL);
	wndButton.SetWindowPos(NULL, rectButton.left, rectButton.top, rectButton.Width(), rectButton.Height(), SWP_NOZORDER);
	wndButton.Detach();
}


void CConnectionErrorView::CloseParentFrame()
{
	CWindow wndParent = GetParent();
	
	ATLASSERT(wndParent.IsWindow());
	if (wndParent.IsWindow())
	{
		wndParent.PostMessage(WM_CLOSE);
	}
}


int CConnectionErrorView::RemoveErrorEventImpl(IUOLMessengerAccountPtr pAccount)
{
	int nRemoveIndex = -1;

	int nItemCount = m_wndAccountListView.GetItemCount();

	for (int nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		IAccountListViewEntryPtr pListViewEntry = m_wndAccountListView.GetEntry(nIndex);

		if (pListViewEntry)
		{
			IUOLMessengerAccountPtr pEntryAccount = pListViewEntry->GetAccount();

			if (pEntryAccount.get() == pAccount.get())
			{
				nRemoveIndex = nIndex;
				break;
			}
		}
	}

	if (nRemoveIndex >= 0)
	{
		IAccountListViewEntryPtr pRemovedEntry = m_wndAccountListView.RemoveEntry(nRemoveIndex);
	}

	return nRemoveIndex;
}


void CConnectionErrorView::OnItemChangedAccountListImpl()
{
	IAccountListViewEntryPtr pListViewEntry;

	int nIndex = m_wndAccountListView.GetSelectedIndex();

	if (nIndex >= 0)
	{
		pListViewEntry = m_wndAccountListView.GetEntry(nIndex);
	}

	if (pListViewEntry)
	{
		CAccountConnectionErrorEntryPtr pErrorEntry = ap_dynamic_cast<CAccountConnectionErrorEntryPtr>(pListViewEntry);

		ConnectionError error = pErrorEntry->GetConnectionError();

		CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

		if (pUIManager != NULL)
		{
			CString strTextFormat;
			const CProtocolResourceTranslator* pTranslator = pUIManager->GetProtocolResourceTranslator();

			ATLASSERT(pTranslator != NULL);
			if (pTranslator->GetConnectionErrorTextFormat(error, strTextFormat))
			{
				CString strProtocolError(pErrorEntry->GetMessage());
				CString strMessage;
				
				if (error != CONNECTION_ERROR_UOL_SERVER_ERROR)
				{
					strMessage.Format(strTextFormat, pListViewEntry->GetAccount()->GetUserId(), pListViewEntry->GetAccount()->GetProtocolName());
				}
				else
				{
					strMessage.Format(strTextFormat, pListViewEntry->GetAccount()->GetUserId(), pListViewEntry->GetAccount()->GetProtocolName(), strProtocolError);
				}

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"Ocorreu o erro [%s] durante o estabelecimento da conexão.", 
						strMessage);
				
				m_wndStaticReasonMessage.SetWindowText(strMessage);
			}
		}
	}
}