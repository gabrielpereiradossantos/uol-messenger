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
#include ".\UOLFoneKeyboardWindow.h"

#include "../FacadeWrapper.h"
#include "../Resource.h"
#include "../UOLFonePluginTheme.h"
#include "UOLFoneController.h"



/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow positioning defines
/////////////////////////////////////////////////////////////////////


#define		DEFAULT_SHIFT						5

#define		LEFT_SHIFT							DEFAULT_SHIFT
#define		TOP_SHIFT							8
#define		RIGHT_SHIFT							DEFAULT_SHIFT
#define		BOTTOM_SHIFT						0

#define		INNER_COMPONENT_DEFAULT_HEIGHT				25
#define		INNER_COMPONENTS_HORIZONTAL_SEPARATION		5
#define		INNER_COMPONENTS_VERTICAL_SEPARATION		5

#define		VERTICAL_BLOCK_SEPARATION					5


/////////////////////////////////////////////////////////////////////
// CUOLFoneKeyboardWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneKeyboardWindow::CUOLFoneKeyboardWindow() : 
		m_destroyingWindow(FALSE)
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);
}


CUOLFoneKeyboardWindow::~CUOLFoneKeyboardWindow() 
{
	m_FoneKeyboardCtrl.UnregisterObserver(this);
	m_phoneNumberCombo.UnregisterObserver(this);
}


void CUOLFoneKeyboardWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneKeyboardWindow::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneKeyboardWindow::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneKeyboardWindow::SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo)
{
	// Do something with the information.
}


BOOL CUOLFoneKeyboardWindow::CanGetFocus()
{
	return TRUE;
}


void CUOLFoneKeyboardWindow::ResetWindow()
{
}


void CUOLFoneKeyboardWindow::GetPhoneNumberToCall(CString& phoneNumber)
{
	m_phoneNumberCombo.GetWindowText(phoneNumber);
	
	BOOL bValid = FALSE;
	bValid = CUOLFoneController::GetInstance()->IsValidPhoneNumber(phoneNumber);
	if ( FALSE == bValid )
	{
		phoneNumber.Empty();
	}
}


void CUOLFoneKeyboardWindow::GetDigitedText(CString& pDigitedText)
{
	m_phoneNumberCombo.GetWindowText(pDigitedText);
}


LRESULT CUOLFoneKeyboardWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls...
	//
	
	CString labelStr;
	CString verExemplosToolTip;
	
	m_smallFont.CreatePointFont(70, "Tahoma");
	m_normalFont.CreatePointFont(100, "Tahoma");
	
	labelStr.LoadString(IDS_BTN_LABEL_CALL_TO);
	m_makeCallTo.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_makeCallTo.SetFont(m_normalFont);
	
	CString comboDefaultText;
	comboDefaultText.LoadString(IDS_BTN_LABEL_CALL_TO);
	
	m_phoneNumberCombo.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_DROPDOWN | CBS_AUTOHSCROLL);
	m_phoneNumberCombo.GetComboCtrl().ModifyStyle(CBS_NOINTEGRALHEIGHT, NULL);
	m_phoneNumberCombo.SetFont(m_normalFont);
	m_phoneNumberCombo.SetInactiveTextColor(RGB(0x66, 0x66, 0x66));
	m_phoneNumberCombo.SetBkGrndColor(
			CUOLFonePluginTheme::GetInstance()->GetEditColor());
	m_phoneNumberCombo.SetComboDefaultText(comboDefaultText);
	m_phoneNumberCombo.RegisterObserver(this);
	
	verExemplosToolTip.LoadString(IDS_LABEL_CALL_INFO);
	labelStr.LoadString(IDS_LABEL_NATIONAL_EXAMPLE);
	verExemplosToolTip += " " + labelStr;
	labelStr.LoadString(IDS_LABEL_INTERNATIONAL_EXAMPLE);
	verExemplosToolTip += " " + labelStr;
	
	labelStr.LoadString(IDS_LABEL_CALL_INFO);
	m_howtoCallLine1.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_howtoCallLine1.SetFont(m_smallFont);

	labelStr.LoadString(IDS_LABEL_NATIONAL_EXAMPLE);
	m_howtoCallLine2.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_howtoCallLine2.SetFont(m_smallFont);

	labelStr.LoadString(IDS_LABEL_INTERNATIONAL_EXAMPLE);
	m_howtoCallLine3.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_howtoCallLine3.SetFont(m_smallFont);
	
	m_FoneKeyboardCtrl.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, WS_CHILDWINDOW | WS_VISIBLE);
	m_FoneKeyboardCtrl.RegisterObserver(this);
	m_FoneKeyboardCtrl.SetAlfaNumeric(FALSE);
	
	ATLASSERT(m_makeCallTo.IsWindow());
	ATLASSERT(m_phoneNumberCombo.IsWindow());
	ATLASSERT(m_howtoCallLine1.IsWindow());
	ATLASSERT(m_howtoCallLine2.IsWindow());
	ATLASSERT(m_howtoCallLine3.IsWindow());
	ATLASSERT(m_FoneKeyboardCtrl.IsWindow());
		
	//
	// Building layout...
	//
	
	// Build line 1.
	m_componentsOrganizer.AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_phoneNumberCombo, 
			m_phoneNumberCombo.GetWndClassName(), 
			163);
	
	// Add a separator line.
	m_componentsOrganizer.AddSeparatorLine(VERTICAL_BLOCK_SEPARATION);
	
	// Build how to call lines.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	/*m_componentsOrganizer.AddComponent(&m_howtoCallLine1, 
			m_howtoCallLine1.GetWndClassName(), 163, 12);*/
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_howtoCallLine2, 
			m_howtoCallLine2.GetWndClassName(), 163, 12);
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_howtoCallLine3, 
			m_howtoCallLine3.GetWndClassName(), 163, 12);
	
	// Add a separator line.
	//m_componentsOrganizer.AddSeparatorLine(VERTICAL_BLOCK_SEPARATION);
	
	// Build line 5.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_FoneKeyboardCtrl, 
			m_FoneKeyboardCtrl.GetWndClassName(), 
			m_FoneKeyboardCtrl.GetWidth(), 0, m_FoneKeyboardCtrl.GetHeight());
		
	return 0;
}


LRESULT CUOLFoneKeyboardWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	m_destroyingWindow = TRUE;
	
	//CFacadeWrapper::GetInstance()->StopMessageHook();
	
	m_makeCallTo.DestroyWindow();
	m_phoneNumberCombo.DestroyWindow();
	m_howtoCallLine1.DestroyWindow();
	m_howtoCallLine2.DestroyWindow();
	m_howtoCallLine3.DestroyWindow();
	m_FoneKeyboardCtrl.DestroyWindow();
	
	m_normalFont.DeleteObject();
	
	return 0;
}


LRESULT CUOLFoneKeyboardWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	return 0L;
}

void CUOLFoneKeyboardWindow::OnButtonClicked(UINT /*uCode*/, int nID, HWND hWndCtrl)
{
	if (nID == ID_BUTTON_CLEAN_CALL_NUMBER)
	{
		m_phoneNumberCombo.SetWindowText("");

		m_phoneNumberCombo.SimulateKillFocus();
	}
}


LRESULT CUOLFoneKeyboardWindow::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HBRUSH hBrush = NULL;
	bHandled = FALSE;
	
	if (WM_CTLCOLOREDIT == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetEditColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetEditBrush();
	}
	else if (WM_CTLCOLORLISTBOX == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetListBoxColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetListBoxBrush();
	}
	else if (WM_CTLCOLORBTN == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetButtonColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetButtonBrush();
	}
	else if (WM_CTLCOLORDLG == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetWindowColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetWindowBrush();
	}
	else if (WM_CTLCOLORSCROLLBAR == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetScrollBarColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetScrollBarBrush();
	}
	else if (WM_CTLCOLORSTATIC == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetStaticColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetStaticBrush();
	}
	
	return (LRESULT) hBrush;
}


LRESULT CUOLFoneKeyboardWindow::OnCtlColorStatic(HDC hDC, HWND hWnd)
{
	if ((hWnd == m_howtoCallLine2.m_hWnd) || (hWnd == m_howtoCallLine3.m_hWnd))
	{
		::SetTextColor(hDC, RGB(0x99, 0x99, 0x99));
	}

	return 0L;
}


LRESULT CUOLFoneKeyboardWindow::OnChar(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( this->IsWindowVisible() )
	{
		UINT nChar = wParam;

		if (nChar == VK_RETURN)
		{
			CFacadeWrapper::GetInstance()->ChooseStartCallType();
		}
		else
		{
			m_phoneNumberCombo.GetEditCtrl().SendMessage(WM_CHAR, wParam, 0);
			bHandled = TRUE;
		}
	}
	
	return 0L;
}


LRESULT CUOLFoneKeyboardWindow::OnCbnDropDown(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	COMBOBOXINFO comboboxInfo;

	CString strMsg;
	
	if (hWndCtl == m_phoneNumberCombo.m_hWnd)
	{
		CAtlList<CString> historyPhoneNumbers;
		CUOLFonePluginConfig::GetInstance()->GetCallHistory(historyPhoneNumbers);
		
		CString strCurNumber;
		m_phoneNumberCombo.GetWindowText(strCurNumber);
		
		m_phoneNumberCombo.ResetContent();
		
		comboboxInfo.cbSize = sizeof(COMBOBOXINFO);

		m_phoneNumberCombo.GetComboCtrl().GetComboBoxInfo(&comboboxInfo);

		CRect dropListRect;
		::GetWindowRect(comboboxInfo.hwndList, &dropListRect);

		if (dropListRect.Height() <= 25)
		{
			dropListRect.bottom += 200;

			::SetWindowPos(comboboxInfo.hwndList, NULL, dropListRect.left, dropListRect.top, 
				dropListRect.Width(), dropListRect.Height(), SWP_NOMOVE);
		}
		
		if (historyPhoneNumbers.GetCount() > 0)
		{
			for (POSITION pos = historyPhoneNumbers.GetHeadPosition(); pos; )
			{
				m_phoneNumberCombo.AddToHistory(historyPhoneNumbers.GetNext(pos));
			}
		}
		
		m_phoneNumberCombo.SetWindowText(strCurNumber);
	}
	
	return 0;
}


void CUOLFoneKeyboardWindow::OnFoneKeyboardPressed(UINT nChar, BOOL bCleanLast)
{
	if (bCleanLast) 
	{
		(m_phoneNumberCombo.GetEditCtrl()).SendMessage(WM_CHAR, (WPARAM)VK_BACK, 0);
	}
	(m_phoneNumberCombo.GetEditCtrl()).SendMessage(WM_CHAR, (WPARAM)nChar, 0);
}


void CUOLFoneKeyboardWindow::OnPhoneNumberComboBoxKeyPressed(TCHAR ch)
{
	UINT nChar = (UINT) ch;

	if (nChar == VK_RETURN)
	{
		CFacadeWrapper::GetInstance()->ChooseStartCallType();
	}
	else
	{
		m_FoneKeyboardCtrl.PressFoneKeyboardKey(nChar);
	}
}