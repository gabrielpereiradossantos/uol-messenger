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

#include "WizardProfileDialog.h"
#include "ValidateAccountsDialog.h"
#include "MinDependencySpecialFolder.h"
#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/imagebuilder.h"
#include "UOLWizard.h"
#include "../../AntiSpamValidation.h"
#include "../../model/ProfileElement.h"
#include "WizardController.h"
#include "../../Controller/UIMApplication.h"
#include "../../Controller/UIManager.h"

HWND CWizardProfileDialog::hWndFinish = NULL;
int CWizardProfileDialog::m_nStep = 0;


CWizardProfileDialog::CWizardProfileDialog()
{
	m_pNextPage = NULL;
}

CWizardProfileDialog::~CWizardProfileDialog()
{
}

void CWizardProfileDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardProfileDialog::GetName() const
{
	return "__WizardProfileInstance";
}

void CWizardProfileDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}

BOOL CWizardProfileDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	/*Init*/
	CMinDependencySpecialFolder	minDSF;
	CString strBtnText, strCompany, strProduct, strProfile;

	strCompany = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_STRING_COMPANY_NAME");
	strProduct = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_STRING_PRODUCT_NAME");
	strProfile = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_STRING_PROFILE_DIR");
	m_strProfileBaseDir.Format("%s%s\\%s\\%s", minDSF.GetAppData(), strCompany, strProduct, strProfile);

    
    CString strUOLFoneMode;
	strUOLFoneMode = m_pWizard->GetWizardSettings()->GetValue("uol.resouce.appmode_uolfone");

	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);

	m_pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();
	ATLASSERT(NULL != m_pProfileManager);
	/**/

	DoDataExchange(FALSE);

    m_staticHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PROFILE_HEADER));
	m_staticHeader.SetColor(RGB(0, 0, 0));
	CString Header = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.PROFILE_HEADER");
	m_staticHeader.SetWindowText(Header);

    m_font.CreateFont(16, 0, 1,
			1, FW_NORMAL, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("MS Shell Dialog"));

    m_staticHeader.SetFont(m_font);

	m_staticInfo.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PROFILE_INFO));		
	m_staticInfo.SetColor(RGB(0, 0, 0));
	CString Info = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.PROFILE_INFO");
	m_staticInfo.SetWindowText(Info);


	m_staticEmail.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PROFILE_EMAIL));		
	m_staticEmail.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PROFILE_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));
	m_staticConfirm.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PROFILE_CONFIRM));		
	m_staticConfirm.SetColor(RGB(0, 0, 0));


    m_staticNetworks.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_NETWORKS));
	m_staticNetworks.SetColor(RGB(0, 0, 0));
    m_staticNetworks.SetFont(m_font);

	((CButton)GetDlgItem(IDC_CHK_MSN)).SetCheck(TRUE);
	((CButton)GetDlgItem(IDC_CHK_UOL)).SetCheck(TRUE);
	((CButton)GetDlgItem(IDC_CHK_ICQ)).SetCheck(TRUE);
	((CButton)GetDlgItem(IDC_CHK_YAHOO)).SetCheck(TRUE);
	((CButton)GetDlgItem(IDC_CHK_JABBER)).SetCheck(TRUE);

    if(0 == (CUIMApplication::GetApplication()->GetCommonSettings()->GetAppMode()).Compare(strUOLFoneMode)) {
        
        m_staticNetworks.ShowWindow(SW_HIDE);
        ((CButton)GetDlgItem(IDC_CHK_MSN)).ShowWindow(SW_HIDE);
	    ((CButton)GetDlgItem(IDC_CHK_UOL)).ShowWindow(SW_HIDE);
	    ((CButton)GetDlgItem(IDC_CHK_ICQ)).ShowWindow(SW_HIDE);
	    ((CButton)GetDlgItem(IDC_CHK_YAHOO)).ShowWindow(SW_HIDE);
	    ((CButton)GetDlgItem(IDC_CHK_JABBER)).ShowWindow(SW_HIDE);
    }
	
	ShowBtnCustom(FALSE, _T(""));
	
	CEdit editPassword = GetDlgItem(IDC_EDIT_WIZARD_PROFILE_PASSWORD);
	editPassword.LimitText(TIL_PROFILE_PASSWORD);

	CEdit editConfirm = GetDlgItem(IDC_EDIT_WIZARD_PROFILE_CONFIRM);
	editConfirm.LimitText(TIL_PROFILE_PASSWORD);

	CEdit editEmail = GetDlgItem(IDC_EDIT_WIZARD_PROFILE_EMAIL);
	editEmail.LimitText(TIL_PROFILE_USER);

	::SetFocus(editEmail);

	// Register transparent controls
	RegisterControl(GetDlgItem(IDC_CHK_MSN));
	RegisterControl(GetDlgItem(IDC_CHK_UOL));
	RegisterControl(GetDlgItem(IDC_CHK_ICQ));
	RegisterControl(GetDlgItem(IDC_CHK_YAHOO));
	RegisterControl(GetDlgItem(IDC_CHK_JABBER));
	SetParentWindow(GetParent());
	SetBitmapBackground(m_pActiveBackground);

	return TRUE;
}

BOOL CWizardProfileDialog::OnBtnNext()
{
	CString strMessage;
    int nRet = 0;

	if (DoDataExchange(TRUE))
	{
        if (CAntiSpamValidation::checkEMail(m_strEmail) == CAntiSpamValidation::AS_NO_ERROR)
		{
			if (m_strPassword.IsEmpty())
			{
				strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");

				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
			}
			else if(m_strPassword.Compare(m_strConfirm) == 0)
			{
				CString strUserDomain = m_strEmail.Mid(m_strEmail.Find("@"));
				CString strUOLDomain, strBOLDomain;
				strUOLDomain = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_EMAIL_DOMAIN");
				strBOLDomain = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_BOL_EMAIL_DOMAIN");

				IUOLMessengerFacade2* pUIMApp = CUIMApplication::GetApplication();
				CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(pUIMApp->GetUIManager());

				if(0 == (pUIMApp->GetCommonSettings()->GetAppMode()).Compare(IUOLMessengerCommonSettings::APP_MODE_DEFAULT_VALUE) )
				{
					if (strUserDomain.Compare(strUOLDomain ) == 0 ||
						strUserDomain.Compare(strBOLDomain ) == 0) 
					{					
						if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_UOL_AUTH, m_strEmail, m_strPassword) == IDOK)
						{
							if (SaveProfile(TRUE))
							{
								UpdateWizardController();
								
								m_pWizard->EndDialog(ID_CREATEPROFILE);
							}
						}
					}
					else // can be a visitor
					{
						if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_UOL_SILENT_AUTH, m_strEmail, m_strPassword) == IDOK)
						{
							if (SaveProfile(TRUE))
							{
								UpdateWizardController();
								
								m_pWizard->EndDialog(ID_CREATEPROFILE);
							}
						}
						else if (SaveProfile(FALSE))
						{
							UpdateWizardController();
							
							m_pWizard->EndDialog(ID_CREATEPROFILE);
						}
					}
				}
				else//Is UOLFone
				{
					if (strUserDomain.Compare(strUOLDomain ) == 0) 
					{
						if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_UOL_AUTH, m_strEmail, m_strPassword) == IDOK)
						{
                            if (SaveProfile(TRUE)) {

								UpdateWizardController();
								
								m_pWizard->EndDialog(ID_CREATEPROFILE);
							}
						}
					}
					else
					{
						strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
					}
				}
			}
			else
			{
				strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_DIFFERENT_PASSWORD");
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
			}
			
		}
		else
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return FALSE;
}

BOOL CWizardProfileDialog::HasNextPage()
{
	return TRUE;
}

BOOL CWizardProfileDialog::SaveProfile(BOOL bEnableAuthentication)
{
    CString strMessage;
    
	ATLASSERT(m_pProfileManager != NULL);
	ATLASSERT(FALSE == m_strEmail.IsEmpty());

	if (!m_pProfileManager->IsProfile(m_strEmail))
	{
		CProfileElementPtr pProfileElement = new CProfileElement(m_strEmail, m_strProfileBaseDir + "\\" + m_strEmail, FALSE, bEnableAuthentication);

		pProfileElement->SetProfilePassword(m_strPassword);

		if (!m_pProfileManager->CreateProfile(m_strEmail, ap_dynamic_cast<IUOLMessengerProfileElementPtr>(pProfileElement)))
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_DIALOG_PROFILE_INVALID");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

			return FALSE;
		}
		else
		{
			m_pProfileManager->GetProfileElement(m_strEmail, ap_dynamic_cast<IUOLMessengerProfileElementPtr>(pProfileElement));
			m_pProfileManager->SetCurrentProfile(m_strEmail);
		}
	}
	else
	{
		strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_DIALOG_PROFILE_EXISTS");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

		return FALSE;
	}

	return TRUE;
}

void CWizardProfileDialog::UpdateWizardController()
{
	CWizardController::GetInstance()->SetProtocolsToConfig(
			((CButton)GetDlgItem(IDC_CHK_MSN)).GetCheck(), 
			((CButton)GetDlgItem(IDC_CHK_UOL)).GetCheck(), 
			((CButton)GetDlgItem(IDC_CHK_ICQ)).GetCheck(), 
			((CButton)GetDlgItem(IDC_CHK_YAHOO)).GetCheck(), 
			((CButton)GetDlgItem(IDC_CHK_JABBER)).GetCheck());
}
