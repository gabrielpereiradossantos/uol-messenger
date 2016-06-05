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

#include "../../Controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "WizardMSNAccountDialog.h"
#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "WizardUOLAccountDialog.h"
#include "WizardICQAccountDialog.h"
#include "UOLWizard.h"
#include "WizardController.h"



const CString CWizardMSNAccountDialog::m_strIdentitiesKey = _T("Software\\Microsoft\\IdentityCRL\\Creds");
const CString CWizardMSNAccountDialog::m_strProtocolId = _T("prpl-msn");


CWizardMSNAccountDialog::CWizardMSNAccountDialog() :
	m_bkg(RGB(255, 255, 255))
{
	m_pNextPage = NULL;
	
    m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

CWizardMSNAccountDialog::~CWizardMSNAccountDialog()
{
    m_pAccount.reset();
}

void CWizardMSNAccountDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardMSNAccountDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardMSNAccountDialog::GetName() const
{
	return "__WizardMSNInstance";
}

void CWizardMSNAccountDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}


BOOL CWizardMSNAccountDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	/*Init*/
	m_pProtocol = CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocol("prpl-msn");
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	m_pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();

	ATLASSERT(NULL != m_pProfileManager);
	
	ATLASSERT(NULL != m_pWizard);
	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	/**/

	m_pNextPage = CWizardController::GetInstance()->GetNextValidPage(
			ID_MSN_PROTOCOL_WIZARD_PAGE);


	CEdit edtMSNId (GetDlgItem(IDC_EDIT_WIZARD_MSN_ID));
	CWindow grbAccounts (GetDlgItem(IDC_STATIC_WIZARD_MSN_ACCOUNTS));

	m_lstbCaptured = GetDlgItem(IDC_LIST_WIZARD_MSN_ACCOUNTS);

	m_staticEmail.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_MSN_EMAIL));		
	m_staticEmail.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_MSN_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));
	
	m_staticSubHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_MSN_SUBHEADER));		
	m_staticSubHeader.SetColor(RGB(0, 0, 0));
	CString Header = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.MSN_HEADER");
	m_staticSubHeader.SetWindowText(Header);

	m_staticInfo.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_MSN_INFO));		
	m_staticInfo.SetColor(RGB(0, 0, 0));
	CString Info = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.MSN_INFO");
	m_staticInfo.SetWindowText(Info);

	if (CaptureMSNAccounts())
	{
		m_lstbCaptured.ShowWindow(TRUE);
 
		grbAccounts.ShowWindow(TRUE);
	}

	edtMSNId.SetFocus();
	edtMSNId.SetLimitText(TIL_ACCOUNT_USER);

	CEdit wndPass = GetDlgItem(IDC_EDIT_WIZARD_MSN_PASSWORD);
	wndPass.SetLimitText(TIL_ACCOUNT_PASSWORD);

	return FALSE;
}

LRESULT CWizardMSNAccountDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


BOOL CWizardMSNAccountDialog::OnBtnNext()
{
	BOOL bRet = FALSE;
    CString strMessage;

	if (DoDataExchange(true))
	{
		if (!m_strEmail.IsEmpty())
		{
			if (!m_strPassword.IsEmpty())
			{
				if (m_pProtocol->IsValidUserName(m_strEmail.GetString()))
				{
                    m_pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(m_strEmail, m_strProtocolId);

					if (m_pAccount)
					{
                        //m_pAccount->SetProtocol(m_pProtocol);
                        //m_pAccount->SetUserId(m_strEmail);
                        m_pAccount->SetPassword(m_strPassword);
                        //m_pAccount->SetUserStatus(IUOLMessengerAccount::AS_OFFLINE);
					}
					if (!CheckDuplicateAccount(m_strEmail, m_strProtocolId, m_pAccount))
					{
						CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(m_pAccount);

                        if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_MSN_AUTH, m_pAccount) == IDOK)
						{
							m_pAccountsList->AddHead(m_pAccount);

							bRet = TRUE;
						}
						else
						{
							CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(m_pAccount);
						}
					}
					else
					{
						CString strTemplate = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_EXISTS_ACCOUNT");
						strMessage.Format(strTemplate, m_strEmail);
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
					}
				}
				else
				{
					strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
				}
			}
			else
			{
				strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		else if (!m_strPassword.IsEmpty())
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
            CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CWizardMSNAccountDialog::HasNextPage()
{
	return TRUE;
}

BOOL CWizardMSNAccountDialog::CaptureMSNAccounts()
{
	int nIndex = 0;
	BOOL bRet = FALSE;
	CRegKey reg;
    DWORD cbName = KEY_LEN;
    TCHAR szSubKeyName[KEY_LEN];


	if (ERROR_SUCCESS == reg.Open(HKEY_CURRENT_USER, m_strIdentitiesKey))
	{
		while (reg.EnumKey(nIndex++, szSubKeyName, &cbName, NULL) != ERROR_NO_MORE_ITEMS)
		{
            if (m_pProtocol->IsValidUserName(szSubKeyName))
			{
                m_lstbCaptured.AddString(szSubKeyName); 
               
				bRet = TRUE;
			}

            cbName = KEY_LEN;
		}
	}
	
	return bRet;
}

CString CWizardMSNAccountDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardMSNAccountDialog::OnBtnCancel()
{
	IUOLMessengerAccountManagerPtr pAccountManager = CUIMApplication::GetApplication()->GetAccountManager();

	if (pAccountManager != NULL)
	{
		pAccountManager->SaveAccounts();
	}

	m_pWizard->EndDialog(IDOK);

	return FALSE;
	//return TRUE;
}

CUOLPropPagePtr CWizardMSNAccountDialog::GetNextPage(CUOLPropPagePtr pCurrentProp)
{
	return m_pNextPage;
}

void CWizardMSNAccountDialog::OnClickButton(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
    CString strMessage;

	if (DoDataExchange(true))
	{
		if (!m_strEmail.IsEmpty())
		{
			if (!m_strPassword.IsEmpty())
			{
				if (m_pProtocol->IsValidUserName(m_strEmail.GetString()))
				{
                    m_pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(m_strEmail, m_strProtocolId);

					if (m_pAccount)
					{
                        m_pAccount->SetPassword(m_strPassword);
					}

					if (!CheckDuplicateAccount(m_strEmail, m_strProtocolId, m_pAccount))
					{
						CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(m_pAccount);

                        if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_MSN_AUTH, m_pAccount) == IDOK)
						{
							m_pAccountsList->AddHead(m_pAccount);

							DoDataExchange(FALSE);

							int nIndex = m_lstbCaptured.FindString(0, m_strEmail);
							if (nIndex != -1)
							{
								m_lstbCaptured.DeleteString(nIndex);                                
							}

							CString strTemplate = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_ADD_ACCOUNT_SUCCESS");
							strMessage.Format(strTemplate, m_strEmail);

							CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONINFORMATION);
							 //TODO: Mudar para DoDataExchange

							(GetDlgItem(IDC_EDIT_WIZARD_MSN_ID)).SetWindowText(_T(""));
							(GetDlgItem(IDC_EDIT_WIZARD_MSN_PASSWORD)).SetWindowText(_T(""));
						}
						else
						{
							CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(m_pAccount);
						}
					}
					else
					{
						CString strTemplate = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_EXISTS_ACCOUNT");
						strMessage.Format(strTemplate, m_strEmail);
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
					}
				}
				else
				{
					strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
				}
			}
			else
			{
				strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		else
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
            CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CWizardMSNAccountDialog::OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(TRUE);
	CString strSelected;

	m_lstbCaptured.GetText(m_lstbCaptured.GetCurSel(), strSelected);

	m_strEmail = strSelected;

	DoDataExchange(FALSE);
}

BOOL CWizardMSNAccountDialog::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}
