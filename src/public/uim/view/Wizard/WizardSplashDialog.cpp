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

#include "WizardSplashDialog.h"
#include "ValidateAccountsDialog.h"
#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/imagebuilder.h"
#include "UOLWizard.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "WizardProfileDialog.h"


CWizardSplashDialog::CWizardSplashDialog()
{
}

void CWizardSplashDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardSplashDialog::GetName() const
{
	return "__WizardSplashInstance";
}

void CWizardSplashDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}

BOOL CWizardSplashDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	IUOLMessengerProfileElementPtr pProfileElement;
	CString strCurrentProfile, strProfileName, strBtnText;

	ATLASSERT(m_pWizard);

	if (FALSE == m_pWizard)
	{
		return FALSE;
	}

	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	CreateBtnCustom(165, 120);
	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_SPLASH_BTNCUSTOM");
	ShowBtnCustom(TRUE, strBtnText);

	m_pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();

	ATLASSERT(m_pProfileManager);

	m_staticEmail.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_SPLASH_EMAIL));		
	m_staticEmail.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_SPLASH_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));

	CEdit editPassword = GetDlgItem(IDC_EDIT_WIZARD_SPLASH_PASSWORD);
	editPassword.SetLimitText(TIL_PROFILE_PASSWORD);

	::SetFocus(editPassword);

	if (m_pProfileManager)
	{
		// Fill profile list
		CAtlList <CString> profileList;
		m_pProfileManager->GetProfileList(profileList);

		if (profileList.GetCount() > 0)
		{
			DoDataExchange(FALSE);

			m_wndComboProfile.ResetContent();

			for(POSITION p = profileList.GetHeadPosition(); p != NULL; profileList.GetNext(p))
			{
				m_wndComboProfile.AddString(profileList.GetAt(p));
			}

			strCurrentProfile = m_pProfileManager->GetCurrentProfile();

			if (strCurrentProfile.IsEmpty())
			{
				m_wndComboProfile.SetCurSel(0);
			}
			else
			{
				m_wndComboProfile.SelectString(-1, strCurrentProfile);
			}
            
			m_wndComboProfile.GetWindowText(strProfileName);
			m_pProfileManager->GetProfileElement(strProfileName, pProfileElement);
			
			m_strDefaultProfile = m_pProfileManager->GetDefaultProfile();

			BOOL bAutoOpen = (FALSE == m_strDefaultProfile.IsEmpty()) && 
							 (0 == m_strDefaultProfile.Compare(strCurrentProfile));
			
			m_btnAutoOpen.SetCheck(bAutoOpen ? BST_CHECKED : BST_UNCHECKED);

			SetOptions(pProfileElement);
			DoDataExchange(FALSE);

			if (bAutoOpen)
			{
				if (FALSE == m_strPassword.IsEmpty())
				{
					// Simulate next button click
					CWindow wndParent = GetParent();

					ATLASSERT(wndParent.IsWindow());
					wndParent.PostMessage(WM_COMMAND, ID_NEXT_PANE, 0L);
				}
				else
				{
					m_btnRememberPass.SetCheck(BST_CHECKED);
				}
			}
		}
	}

	// Register transparent controls
	RegisterControl(GetDlgItem(IDC_CHECK_SPLASH_REMEMBER_PASSWORD));
	RegisterControl(GetDlgItem(IDC_CHECK_SPLASH_OPEN_AUTO));
	SetParentWindow(GetParent());
	SetBitmapBackground(m_pActiveBackground);

	return FALSE;
}

BOOL CWizardSplashDialog::OnBtnNext()
{
	CString strMessage;
	CString strPerfil, strPass;
	IUOLMessengerProfileElementPtr pProfileElement;
    IUOLMessengerAuthenticationManagerPtr authManager;
	int nRet;
	BOOL bCheckPass = FALSE;

	if (DoDataExchange(TRUE))
	{
		m_wndComboProfile.GetWindowText(strPerfil);

		if (m_pProfileManager)
		{
            m_pProfileManager->GetProfileElement(strPerfil, pProfileElement);
            
			if(pProfileElement)
			{
				if (m_strPassword.IsEmpty())
				{
					strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");
					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

					return FALSE;
				}
				else if(pProfileElement->IsEnableAuthentication())
				{			
					nRet = m_pValidate->DoValidation(CValidateAccountsDialog::ID_UOL_SILENT_AUTH, strPerfil, m_strPassword); 

					if (nRet == IDOK)
					{
						pProfileElement->SetProfilePassword(m_strPassword);
					}
					else if (nRet == ID_ERROR_LOGIN)
					{
                        strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_LOGIN_PASS");
                        CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

						return FALSE;
					}
				}

                strPass = pProfileElement->GetProfilePassword();

				if (m_strPassword.Compare(strPass) == 0)
				{
                    m_pProfileManager->SetCurrentProfile(strPerfil);

					if (m_bSave)
					{
						m_pProfileManager->SetDefaultProfile(m_strDefaultProfile);
						pProfileElement->SetEnableRememberPass(m_btnRememberPass.GetCheck());

						m_pProfileManager->SaveSettings();
					}
					m_pWizard->EndDialog(IDOK);
				}
				else
				{
                    strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_LOGIN_PASS");
                    CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
				}
			}
		}
	}

	return FALSE;
}

CString CWizardSplashDialog::GetNextBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_SPLASH_BTNNEXT");

	return strBtnText;
}

BOOL CWizardSplashDialog::HasNextPage()
{
	return TRUE;
}

CUOLPropPagePtr CWizardSplashDialog::GetNextPage(CUOLPropPagePtr pCurrentProp)
{
	return m_pNextPage;
}

BOOL CWizardSplashDialog::OnBtnCancel()
{
	return TRUE;
}

void CWizardSplashDialog::OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	CString strMessage;

	m_bSave = TRUE;

	strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_REMEMBER_PASSWORD");

	if (DoDataExchange(TRUE))
	{
		if(nID == IDC_CHECK_SPLASH_REMEMBER_PASSWORD)
		{
			if (m_btnRememberPass.GetCheck())
			{
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
			}
			else
			{
				m_btnAutoOpen.SetCheck(FALSE);
			}
		}
		else if(nID == IDC_CHECK_SPLASH_OPEN_AUTO)
		{
			if (m_btnAutoOpen.GetCheck())
			{
                m_wndComboProfile.GetWindowText(m_strDefaultProfile);
				m_btnRememberPass.SetCheck(TRUE);
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
			}
			else
			{
                m_strDefaultProfile = _T("");
			}
		}
		DoDataExchange(FALSE);
	}
}

void CWizardSplashDialog::OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CString strProfileName, strPassword;
	IUOLMessengerProfileElementPtr pProfileElement;

	DoDataExchange(TRUE);

	m_wndComboProfile.GetWindowText(strProfileName);

	m_pProfileManager->GetProfileElement(strProfileName, pProfileElement);
	SetOptions(pProfileElement);

	DoDataExchange(FALSE);
}

void CWizardSplashDialog::OnBtnCustom(CUOLPropPagePtr pCurrentProp, UINT nCmdId)
{
	if (nCmdId == ID_BTNCUSTOM)
	{
        if(!m_pNextPage)
		{
			//m_pNextPage = new CWizardProfileDialog();
			CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());	
			CWizardProfileDialogPtr pWizardProfile = ap_dynamic_cast<CWizardProfileDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardProfileInstance"));
			m_pNextPage = pWizardProfile;
		}
        m_pWizard->SetPage(m_pNextPage);
	}
}

void CWizardSplashDialog::SetOptions(IUOLMessengerProfileElementPtr pProfileElement)
{
	CString strProfileName, strPassword;

	ATLASSERT(pProfileElement != NULL);

	if (pProfileElement)
	{
		m_pProfileManager->GetProfileElement(strProfileName, pProfileElement);

		if(pProfileElement->IsEnableRememberPass())
		{
			strPassword = pProfileElement->GetProfilePassword();
			m_btnRememberPass.SetCheck(TRUE);
			m_strPassword = strPassword;
		}
		else
		{
			m_strPassword = _T("");
			m_btnRememberPass.SetCheck(FALSE);
		}
	}
}