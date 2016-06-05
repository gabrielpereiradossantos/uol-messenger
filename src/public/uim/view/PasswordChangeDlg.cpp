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
 
// PasswordChangeDlg.cpp : implementation of the CPasswordChangeDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PasswordChangeDlg.h"
#include "../resource.h"
#include "../controller/UIMApplication.h"



CPasswordChangeDlg::CPasswordChangeDlg(void) :
		CMainDialog(IDD_DIALOG_PASSWORD_CHANGE)
{
}

CPasswordChangeDlg::~CPasswordChangeDlg(void)
{
}

LRESULT CPasswordChangeDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString strTitle;
	GetWindowText(strTitle);

	__super::OnInitDialog(NULL, NULL);

	//Set title and text
	SetTitle(strTitle);

	m_editActualPass = GetDlgItem(IDC_EDIT_ACTUAL_PASSWORD);
	m_editNewPass = GetDlgItem(IDC_EDIT_NEW_PASSWORD);
	m_editConfirmPass = GetDlgItem(IDC_EDIT_NEW_PASSWORD_CONFIRM);

	m_labelOldTransparencyApplier.SubclassWindow(GetDlgItem(IDC_CHANGE_PWD_STATIC_OLD));
	m_labelNewTransparencyApplier.SubclassWindow(GetDlgItem(IDC_CHANGE_PWD_STATIC_NEW));
	m_labelConfTransparencyApplier.SubclassWindow(GetDlgItem(IDC_CHANGE_PWD_STATIC_CONFIRM));
	m_labelInfoTransparencyApplier.SubclassWindow(GetDlgItem(IDC_CHANGE_PWD_STATIC_INFO));
	
	CenterWindow();

	return TRUE;
}

LRESULT CPasswordChangeDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
    UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	CString strMessage;
	
	CString strCurPass, strNewPass, strConfirm;
	
	m_editActualPass.GetWindowText(strCurPass);
	m_editNewPass.GetWindowText(strNewPass);
	m_editConfirmPass.GetWindowText(strConfirm);

	if ( strNewPass.IsEmpty() )
	{
		strMessage = pStringLoader->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
		
		m_editNewPass.SetFocus();

		return 0;
	}
	
	IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();
	CString strCurrentProfile = pProfileManager->GetCurrentProfile();
	IUOLMessengerProfileElementPtr pProfileElement;
	if ( pProfileManager->GetProfileElement(strCurrentProfile, pProfileElement) )
	{		
		if ( pProfileElement->GetProfilePassword() != strCurPass )
		{
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_INCORRECT_PASSWORD");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);

			m_editActualPass.SetFocus();
			m_editActualPass.SetSel(0, -1);

			return 0;
		}

		if ( strNewPass != strConfirm )
		{
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_WIZARD_DIFFERENT_PASSWORD");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);

			m_editNewPass.SetWindowText(_T(""));
			m_editConfirmPass.SetWindowText(_T(""));
			m_editNewPass.SetFocus();

			return 0;
		}

		pProfileElement->SetProfilePassword(strNewPass);
		pProfileManager->SaveSettings();

		EndDialog(wID);
	}

	return 0;
}

LRESULT CPasswordChangeDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	EndDialog(wID);

	return 0;
}