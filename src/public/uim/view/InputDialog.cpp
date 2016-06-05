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
#include "InputDialog.h"


CInputDialog::CInputDialog() :
		CMainDialog(IDD_INPUT_DIALOG)
{
}


CInputDialog::~CInputDialog(void)
{
}


void CInputDialog::Initialize(const CString& strTitle, const CString& strLabel, UINT nInputTextLimit, BOOL bPasswordInput, const CString& strText)
{
	m_strTitle = strTitle;
	m_strLabel = strLabel;
	m_bPasswordInput = bPasswordInput;
	m_nInputTextLimit = nInputTextLimit;
	m_strText = strText;
}


void CInputDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
    __super::SetParameters(pMapParams, pElementTable);
}


LRESULT CInputDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	__super::OnInitDialog(NULL, NULL);

	//Set title and text
	SetTitle(m_strTitle);
	
	DoDataExchange(FALSE);

	m_wndEditText = GetDlgItem(IDC_EDIT_INPUT_TEXT);
	m_wndRadioSavePassword = GetDlgItem(IDC_INPUT_CHECK_SAVE_PASSWORD);

	if (FALSE == m_bPasswordInput)
	{
		m_wndEditText.SetPasswordChar(NULL);
		m_wndRadioSavePassword.ShowWindow(SW_HIDE);
	}

	m_wndEditText.SetFocus();
	m_wndEditText.SetSel(0, -1);
	m_wndEditText.SetLimitText(m_nInputTextLimit);
	m_wndEditText.SetWindowText(m_strText);
	m_wndRadioSavePassword.SetCheck(m_bSavePassword);

	m_labelTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_INPUT_LABEL));
	
	CenterWindow();

	// Register transparent controls
	RegisterControl(GetDlgItem(IDC_INPUT_CHECK_SAVE_PASSWORD));
	SetParentWindow(m_hWnd);
	SetBitmapBackground(m_pActiveBackground);

	return 0L;
}


LRESULT CInputDialog::OnClose()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	EndDialog(IDCANCEL);
	return 0L;
}

void CInputDialog::OnBnClickedButtonOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DoDataExchange(TRUE);
	m_bSavePassword = m_wndRadioSavePassword.GetCheck();
	EndDialog(IDOK);
}

void CInputDialog::OnBnClickedButtonCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	EndDialog(IDCANCEL);
}

CString CInputDialog::GetText() const
{
	return m_strText;
}

void CInputDialog::SetText(const CString& strText)
{
	m_strText = strText;
}

void CInputDialog::SetInputTextLimit(UINT nCharacters)
{
	m_nInputTextLimit = nCharacters;

	if (m_wndEditText.IsWindow())
	{
		m_wndEditText.SetLimitText(nCharacters);
	}
}

UINT CInputDialog::GetInputTextLimit()
{
	return m_nInputTextLimit;
}

BOOL CInputDialog::IsSavePassword()
{
	return m_bSavePassword;
}

void CInputDialog::SetSavePassword(BOOL bSavePassword)
{
	m_bSavePassword = bSavePassword;
}