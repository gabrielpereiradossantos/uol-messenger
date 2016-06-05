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
#include ".\phonenumbercombobox.h"
#include "../FacadeWrapper.h"
#include "UOLFoneController.h"

#define COMBO_EDIT_RIGHT_MARGIN 16
#define COMBO_EDIT_MAX_LENGTH 128

CPhoneNumberComboBox::CPhoneNumberComboBox(UINT nMaxItems) : 
		m_nMaxItems(nMaxItems)
{
	// Tells "CWMEventNotifier" to not post new messages, but call 
	// the "OnCtlColorEventNotifier" overrided method.
	SetSendNewMessage(FALSE);
}


CPhoneNumberComboBox::~CPhoneNumberComboBox(void)
{

}


int CPhoneNumberComboBox::AddToHistory(LPCTSTR lpszString)
{
	// Insert on beginning
	COMBOBOXEXITEM cbNewItem = {0};
	cbNewItem.mask = CBEIF_TEXT;
	cbNewItem.iItem = 0;
	cbNewItem.pszText = (LPTSTR) (LPCTSTR) lpszString;
	cbNewItem.cchTextMax = ::_tcslen(lpszString);

	int nRetInd = InsertItem(&cbNewItem);

	// Delete from previous position, if any
	COMBOBOXEXITEM cbItem = {0};
	TCHAR szText[MAX_PATH + 1];
	int nCount = GetCount();
	for (int nInd = 1; nInd < nCount; nInd++)
	{
		cbItem.mask = CBEIF_TEXT;
		cbItem.iItem = nInd;
		cbItem.cchTextMax = MAX_PATH;
		cbItem.pszText = szText;

		if (GetItem(&cbItem) && (0 == ::_tcscmp(lpszString, cbItem.pszText)))
		{
			DeleteItem(nInd);
			break;
		}
	}

	// Limit strings count up to m_nMaxItems
	nCount = GetCount();
	for (int nInd = m_nMaxItems; nInd < nCount; nInd++)
	{
		DeleteItem(m_nMaxItems);
	}

	return nRetInd;
}


HWND CPhoneNumberComboBox::Create(HWND hWndParent, ATL::_U_RECT rect, 
								  LPCTSTR szWindowName, DWORD dwStyle, 
								  DWORD dwExStyle, ATL::_U_MENUorID MenuOrID, 
								  LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
	
	if (hWnd)
	{
		m_comboEdit.SubclassWindow(GetEditCtrl());		
		m_comboEdit.InitializeEditFeatures();
		m_comboEdit.ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		m_comboEdit.SetLimitText(COMBO_EDIT_MAX_LENGTH);
		m_comboEdit.SetParentWnd(m_hWnd);

		CString labelStr;
		labelStr.LoadString(IDS_BTN_LABEL_CLEAN);
		m_cleanNumber.Create(m_comboEdit.m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 0, CComboEdit::ID_BUTTON_CLEAN_CALL_NUMBER);
		
		m_multiStateCleanNumber.SubclassWindow(m_cleanNumber.m_hWnd);
		m_multiStateCleanNumber.SetMonoStateBitmap(IDB_BTN_LIMPAR);
		m_multiStateCleanNumber.SetCharForwardingWnd(m_hWnd); 
		m_multiStateCleanNumber.SetToolTipText(labelStr);
		m_multiStateCleanNumber.SetToolTipDelays(5000, 400, 0);
	}
	
	return hWnd;
}


BOOL CPhoneNumberComboBox::DestroyWindow()
{
	m_cleanNumber.DestroyWindow();
	m_comboEdit.UnsubclassWindow();	
	m_comboEdit.FinalizeEditFeatures();
	
	return __super::DestroyWindow();
}


void CPhoneNumberComboBox::SetComboDefaultText(const CString& comboDefaultText)
{
	CString windowText;
	
	m_comboEdit.m_comboDefaultText = comboDefaultText;
	
	GetWindowText(windowText);
	
	if (windowText.IsEmpty())
	{
		SetWindowText(m_comboEdit.m_comboDefaultText);
		
		if (m_comboEdit.m_actualFont)
		{
			m_comboEdit.SetFont(m_comboEdit.m_actualFont);
		}
	}
}


CString CPhoneNumberComboBox::GetComboDefaultText()
{
	return m_comboEdit.m_comboDefaultText;
}


void CPhoneNumberComboBox::SetInactiveTextColor(COLORREF textColor)
{
	if (textColor != NULL)
	{
		m_comboEdit.m_rgbInactiveText = textColor;
	}
}


void CPhoneNumberComboBox::SetBkGrndColor(COLORREF bkColor)
{
	if (bkColor != NULL)
	{
		m_comboEdit.m_rgbBkGrnd = bkColor;
		
		if (!m_comboEdit.m_pBrush.IsNull())
		{
			m_comboEdit.m_pBrush.DeleteObject();
		}
		m_comboEdit.m_bkGrndBrushHandle = 
				m_comboEdit.m_pBrush.CreateSolidBrush(m_comboEdit.m_rgbBkGrnd);
	}
}


void CPhoneNumberComboBox::SimulateKillFocus()
{
	ATLTRACE(_T("CPhoneNumberComboBox::SimulateKillFocus()\n"));
	
	m_comboEdit.UpdateEditLayout(FALSE, FALSE);

	::SetFocus(GetParent());
}


HBRUSH CPhoneNumberComboBox::OnCtlColorEventNotifier(UINT uMsg, HDC hdc, HWND hWnd)
{
	HBRUSH retBrush = NULL;
	
	ATLTRACE(_T("OnCtlColorEventNotifier(), hWnd = [%u], GetEditCtrl().m_hWnd = [%u]\n"), 
			hWnd, GetEditCtrl().m_hWnd);
	
	if ( (uMsg == WM_CTLCOLOREDIT) && (hWnd == GetEditCtrl().m_hWnd) )
	{
		if ( (m_comboEdit.m_searchTextActive) && 
				(m_comboEdit.m_backupInactiveTextColor == NULL) )
		{
			//ATLTRACE(_T("HandleOnCtlColor() - Using inactive text color.\n"));
			
			m_comboEdit.m_backupInactiveTextColor = 
					::SetTextColor(hdc, m_comboEdit.m_rgbInactiveText);
		}
		else if (m_comboEdit.m_backupInactiveTextColor)
		{
			//ATLTRACE(_T("HandleOnCtlColor() - Using normal color.\n"));
			
			::SetTextColor(hdc, m_comboEdit.m_backupInactiveTextColor);
			
			m_comboEdit.m_backupInactiveTextColor = NULL;
		}
		
		retBrush = m_comboEdit.m_pBrush;
	}
	
	//ATLTRACE(_T("HandleOnCtlColor(), retBrush = [%u]\n"), retBrush);
	
	return retBrush;
}


LRESULT CPhoneNumberComboBox::OnEraseBackground(HDC hDc)
{
	return 1L;
}


void CPhoneNumberComboBox::OnSize(UINT nType, const CSize& size)
{
	if ( (m_comboEdit) && (m_comboEdit.m_hWnd) )
	{
		CRect rect;
		m_comboEdit.GetClientRect(&rect);
		
		CRect rectEdit(rect.left, rect.top, rect.right - m_multiStateCleanNumber.GetWidth(), rect.bottom);
		
		//Edit control
		int x = rect.right - m_multiStateCleanNumber.GetWidth();
		int y = rect.top;
	    
		m_multiStateCleanNumber.SetWindowPos(NULL, x, y, 
			m_multiStateCleanNumber.GetWidth(), 
			m_multiStateCleanNumber.GetHeight(), SWP_NOZORDER);

		m_comboEdit.SetMargins(1, COMBO_EDIT_RIGHT_MARGIN);
	}
}


LRESULT CPhoneNumberComboBox::OnCleanButtonClicked(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_comboEdit.SetWindowText("");
	SimulateKillFocus();

	return 0L;
}


void CPhoneNumberComboBox::RegisterObserver(CPhoneNumberComboBoxObserver* pObserver)
{
	m_comboEdit.RegisterObserver(pObserver);
}


void CPhoneNumberComboBox::UnregisterObserver(CPhoneNumberComboBoxObserver* pObserver)
{
	m_comboEdit.UnregisterObserver(pObserver);
}


CPhoneNumberComboBox::CComboEdit::~CComboEdit(void)
{
	m_listObservers.RemoveAll();
}


void CPhoneNumberComboBox::CComboEdit::InitializeEditFeatures()
{
	SetSendNewMessage(FALSE);
	
	// Create the default font.
	m_fontSearchComboBox.CreatePointFont(100, "Tahoma");
	
	// Create italic font.
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));          // clear out structure
	m_fontSearchComboBox.GetLogFont(&lf);
	lf.lfItalic = TRUE;
	m_fontSearchComboBoxText = ::CreateFontIndirect(&lf);  // create the font
	
	SetFont(m_fontSearchComboBoxText, TRUE);
	
	m_actualFont = m_fontSearchComboBoxText;
	
	m_searchTextActive = TRUE;
	m_backupInactiveTextColor = NULL;
	
	m_rgbInactiveText = RGB(0x00, 0x00, 0x00);
	m_rgbBkGrnd = GetSysColor(COLOR_WINDOW);
	m_bkGrndBrushHandle = m_pBrush.CreateSolidBrush(m_rgbBkGrnd);
	
	UpdateEditLayout(FALSE, FALSE);
}


void CPhoneNumberComboBox::CComboEdit::FinalizeEditFeatures()
{
	m_fontSearchComboBox.DeleteObject();
	m_fontSearchComboBoxText.DeleteObject();
}


void CPhoneNumberComboBox::CComboEdit::OnChar(TCHAR ch, UINT, UINT)
{
	ATLTRACE(_T("OnChar()\n"));
	
	UpdateEditLayout(FALSE, TRUE);
	
	if (this->IsWindowVisible())
	{
		if (' ' == ch)
		{
			// ignore white space
			SetMsgHandled(TRUE);
		}
		else
		{
			NotifyAllKeyPressed(ch);
			SetMsgHandled(FALSE);
		}
	}
	else
	{
		NotifyAllKeyPressed(ch);
		SetMsgHandled(FALSE);
	}
}


void CPhoneNumberComboBox::CComboEdit::OnPaste()
{
	// Don't allow user paste text with white spaces
	int iSelStart = 0;
    int iSelEnd = 0; 

	CString strClipboardText;
	CString strEditText;

	GetWindowText(strEditText);
	GetSel(iSelStart, iSelEnd);
	
	if (::IsClipboardFormatAvailable(CF_TEXT))
	{
		if (::OpenClipboard(m_hWnd))
		{
			HANDLE hClipboardData = NULL;
			if ((hClipboardData = ::GetClipboardData(CF_TEXT)) != NULL)
			{
				LPTSTR lpClipboardText = (LPTSTR)::GlobalLock(hClipboardData);
				if (lpClipboardText)
				{
					strClipboardText = lpClipboardText;
					::GlobalUnlock(hClipboardData);
				}
			}
			::CloseClipboard();
		}
	}

	strClipboardText.Replace(" ", "");

	strEditText = strEditText.Left(iSelStart) 
				+ strClipboardText + strEditText.Mid(iSelEnd);
	iSelEnd = iSelStart + strClipboardText.GetLength();

	SetWindowText(strEditText);
	SetSel(iSelEnd, iSelEnd); 
}


void CPhoneNumberComboBox::CComboEdit::OnSetFocusEventNotifier(HWND hLostFocusWnd)
{
	ATLTRACE(_T("CComboEdit::OnSetFocusEventNotifier()\n"));
	
	UpdateEditLayout(TRUE, TRUE);
}


void CPhoneNumberComboBox::CComboEdit::OnKillFocusEventNotifier(HWND hGetFocusWnd)
{
	ATLTRACE(_T("CComboEdit::OnKillFocusEventNotifier()\n"));
	
	UpdateEditLayout(FALSE, FALSE);
}


void CPhoneNumberComboBox::CComboEdit::UpdateEditLayout(
		BOOL settingFocus, BOOL editing)
{
	ATLTRACE(_T("UpdateEditLayout(), settingFocus = [%d], editing = [%d]\n"), 
			settingFocus, editing);
	
	if ( (settingFocus) || (editing) )
	{
		// Get edit string.
		CString strText;
		GetWindowText(strText);
		
		ATLTRACE(_T("UpdateEditLayout(), strText = [%s]\n"), strText);
		
		// Is the default text active ?
		if (m_searchTextActive == TRUE)
		{   
			strText.SetString("", 0);
			SetWindowText(strText);
			
			SetFont(m_fontSearchComboBox, TRUE);
			m_actualFont = m_fontSearchComboBox;
			
			// Switch the flag off.
			m_searchTextActive = FALSE;
		}
	}
	else
	{
		// Get edit string.
		CString strText;
		GetWindowText(strText);
		
		ATLTRACE(_T("UpdateEditLayout(), strText = [%s]\n"), strText);
		
		// Does it contains text ?
		if (strText.GetLength() == 0)
		{
			SetWindowText(m_comboDefaultText);
			
			SetFont(m_fontSearchComboBoxText, TRUE);
			m_actualFont = m_fontSearchComboBoxText;
			
			// Switch the flag on.
			m_searchTextActive = TRUE;
		}
	}
	SetMargins(1, COMBO_EDIT_RIGHT_MARGIN);
}


void CPhoneNumberComboBox::CComboEdit::SetParentWnd(HWND hwnd)
{
	m_hwndParent = hwnd;
}


void CPhoneNumberComboBox::CComboEdit::OnCleanButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	::SendMessage(m_hwndParent, WM_BUTTON_CLEAN_CLICKED, NULL, NULL);
}


void CPhoneNumberComboBox::CComboEdit::RegisterObserver(CPhoneNumberComboBoxObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION posFind = m_listObservers.Find(pObserver);

		if (!posFind)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CPhoneNumberComboBox::CComboEdit::UnregisterObserver(CPhoneNumberComboBoxObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION posFind = m_listObservers.Find(pObserver);

		if (posFind != NULL)
		{
			m_listObservers.RemoveAt(posFind);
		}
	}
}


void CPhoneNumberComboBox::CComboEdit::NotifyAllKeyPressed(TCHAR ch)
{
	CPhoneNumberComboBoxObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);

		pObserver->OnPhoneNumberComboBoxKeyPressed(ch);
	}
}