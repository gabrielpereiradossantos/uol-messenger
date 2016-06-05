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
#include "PreferencesPropPage.h"
#include "../../controller/UIMApplication.h"
#include "TabbedWindow.h"


CPreferencesPropPage::CPreferencesPropPage(DWORD dwIDD, const CString& strTitle) : 
	IDD(dwIDD), m_strTitle(strTitle), m_clrbrushDialog(RGB(255, 255, 255)), m_pTabbedWindow(NULL)
{
}


CPreferencesPropPage::~CPreferencesPropPage()
{
}


HWND CPreferencesPropPage::Create(CTabbedWindow* pParent, LPARAM lParam)
{
	m_pTabbedWindow = pParent;

	HWND hWnd = __super::Create(pParent->m_hWnd, lParam);

	if (IsWindow())
	{
		DWORD dwRemove = DS_FIXEDSYS | WS_POPUP | WS_CLIPSIBLINGS | WS_SYSMENU;
		DWORD dwAdd = DS_CONTROL | WS_CHILDWINDOW;

		ModifyStyle(dwRemove, dwAdd);
		//SetParent(m_pTabbedWindow->m_hWnd);
	}

	return hWnd;
}


void CPreferencesPropPage::SetTitle(const CString& strTitle)
{
	m_strTitle = strTitle;

	if (IsWindow())
	{
		SetWindowText(m_strTitle);
	}
}


CString CPreferencesPropPage::GetTitle() const
{
	return m_strTitle;
}


void CPreferencesPropPage::SetIcon(IUOLMessengerImagePtr pIcon)
{
	m_pIcon = pIcon;
}


IUOLMessengerImagePtr CPreferencesPropPage::GetIcon() const
{
	return m_pIcon;
}


void CPreferencesPropPage::OnSetActive()
{
}


void CPreferencesPropPage::OnKillActive()
{
}

LRESULT CPreferencesPropPage::OnInitDialog(HWND, LPARAM)
{
	SetWindowText(GetTitle());

	return FALSE;
}

void CPreferencesPropPage::OnClose()
{
	DestroyWindow();
}

LRESULT CPreferencesPropPage::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	HBRUSH hBrush = NULL;

	if (WM_CTLCOLOREDIT == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushEdit);
		hBrush = m_brushEdit.m_hBrush;
		bHandled = FALSE;
	}
	else if (WM_CTLCOLORLISTBOX == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushListBox);
		hBrush = m_brushListBox.m_hBrush;
		bHandled = FALSE;
	}
	else if (WM_CTLCOLORBTN == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushButton);
		hBrush = m_brushButton.m_hBrush;
	}
	else if (WM_CTLCOLORDLG == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushDialog);
		hBrush = m_brushDialog.m_hBrush;
	}
	else if (WM_CTLCOLORSCROLLBAR == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushScrollBar);
		hBrush = m_brushScrollBar.m_hBrush;
	}
	else if (WM_CTLCOLORSTATIC == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushStatic);
		hBrush = m_brushStatic.m_hBrush;
	}

	// TODO: check hwnd?
//	::SetBkColor((HDC) wParam, m_clrbrushDialog);

	return (LRESULT) hBrush;
}


LRESULT CPreferencesPropPage::OnSetActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OnSetActive();
	return 0L;
}


LRESULT CPreferencesPropPage::OnKillActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OnKillActive();
	return 0L;
}


HWND CPreferencesPropPage::Create(LPCTSTR lpstrWndClass, HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
		DWORD dwStyle, DWORD dwExStyle,
		_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	ATLASSERT(FALSE);
	return NULL;
}



void CPreferencesPropPage::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "editColor");
		m_clrbrushEdit.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "editColor");
		m_clrbrushEdit.SetColor(_T("#e6e6e6"));
	}
	m_brushEdit.CreateSolidBrush(m_clrbrushEdit);

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "listBoxColor");
		m_clrbrushListBox.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "listBoxColor");
		m_clrbrushListBox.SetColor(_T("#e6e6e6"));
	}
	m_brushListBox.CreateSolidBrush(m_clrbrushListBox);

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "dialogColor");
		m_clrbrushDialog.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "dialogColor");
		m_clrbrushDialog.SetColor(_T("#e6e6e6"));
	}
	m_brushDialog.CreateSolidBrush(m_clrbrushDialog);

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "buttonColor");
		m_clrbrushButton.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "buttonColor");
		m_clrbrushButton.SetColor(_T("#e6e6e6"));
	}
	m_brushButton.CreateSolidBrush(m_clrbrushButton);

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "staticColor");
		m_clrbrushStatic.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "staticColor");
		m_clrbrushStatic.SetColor(_T("#e6e6e6"));
	}
	m_brushStatic.CreateSolidBrush(m_clrbrushStatic);

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "scrollBarColor");
		m_clrbrushScrollBar.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "scrollBarColor");
		m_clrbrushScrollBar.SetColor(_T("#e6e6e6"));
	}
	m_brushScrollBar.CreateSolidBrush(m_clrbrushScrollBar);

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "DlgBackgroundColor");
		m_clrbrushDialog.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "DlgBackgroundColor");
		m_clrbrushDialog.SetColor(_T("#e6e6e6"));
	}
	

	// Caption

	CString strCaption;
	GetParameterValue(strCaption, pMapParams, "caption");

	SetTitle(strCaption);	
}


LRESULT CPreferencesPropPage::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	//::SetBkColor(hdc, )
	return (LRESULT) m_brushStatic.m_hBrush;
}


void CPreferencesPropPage::SetTabbedWindow(CTabbedWindow* pTabbedWindow)
{
	m_pTabbedWindow = pTabbedWindow;
}


CTabbedWindow* CPreferencesPropPage::GetTabbedWindow() const
{
	return m_pTabbedWindow;
}
