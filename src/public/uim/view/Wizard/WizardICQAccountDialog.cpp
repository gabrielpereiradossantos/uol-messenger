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
#include "../../Controller/UIManager.h"
#include "ValidateAccountsDialog.h"
#include "WizardICQAccountDialog.h"
#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "WizardYahooAccountDialog.h"
#include "UOLWizard.h"
#include "WizardController.h"


const CString CWizardICQAccountDialog::m_strIdentitiesKey = _T("Software\\Mirabilis\\ICQ\\NewOwners");
const CString CWizardICQAccountDialog::m_strProtocolId = _T("prpl-icq");


CWizardICQAccountDialog::CWizardICQAccountDialog() :
m_bkg(RGB(255, 255, 255))

{
	m_pNextPage = NULL;

    m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

void CWizardICQAccountDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardICQAccountDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardICQAccountDialog::GetName() const
{
	return "__WizardICQInstance";
}


void CWizardICQAccountDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}

BOOL CWizardICQAccountDialog::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	/*Init*/
	m_pProtocol = CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocol("prpl-icq");
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	
	m_pNextPage = CWizardController::GetInstance()->GetNextValidPage(
			ID_ICQ_PROTOCOL_WIZARD_PAGE);

	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	/**/

	CEdit edtICQId (GetDlgItem(IDC_EDIT_WIZARD_ICQ_ID));
	CWindow grbAccounts (GetDlgItem(IDC_STATIC_WIZARD_ICQ_ACCOUNTS));

	m_lstbCaptured = GetDlgItem(IDC_LIST_WIZARD_ICQ_ACCOUNTS);

	m_staticEmail.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_ICQ_UIN));		
	m_staticEmail.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_ICQ_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));

	m_staticInfo.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_ICQ_INFO));
	m_staticInfo.SetColor(RGB(0, 0, 0));
	CString Info = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.ICQ_INFO");
	m_staticInfo.SetWindowText(Info);
	
	m_staticSubHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_ICQ_SUBHEADER));		
	m_staticSubHeader.SetColor(RGB(0, 0, 0));
	CString Header = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.ICQ_HEADER");
	m_staticSubHeader.SetWindowText(Header);

	if (CaptureICQAccounts())
	{
		m_lstbCaptured.ShowWindow(TRUE);
		grbAccounts.ShowWindow(TRUE);

	}

	edtICQId.SetFocus();
	edtICQId.SetLimitText(TIL_ACCOUNT_USER);

	CEdit wndPass = GetDlgItem(IDC_EDIT_WIZARD_ICQ_PASSWORD);
	wndPass.SetLimitText(TIL_ACCOUNT_PASSWORD);

    return TRUE;
}

LRESULT CWizardICQAccountDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}

BOOL CWizardICQAccountDialog::OnBtnNext()
{
	BOOL bRet = FALSE;
    CString strCaption, strMessage;

	strCaption = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_APP_NAME");

	if (DoDataExchange(true))
	{
		if (!m_strUIN.IsEmpty())
		{
			if (!m_strPassword.IsEmpty())
			{
				if (m_pProtocol->IsValidUserName(m_strUIN.GetString()))
				{
                    m_pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(m_strUIN, m_strProtocolId);
					
					if (m_pAccount)
					{
						m_pAccount->SetPassword(m_strPassword);
					}

					if (!CheckDuplicateAccount(m_strUIN, m_strProtocolId, m_pAccount))
					{	
						CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(m_pAccount);

						if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_ICQ_AUTH, m_pAccount) == IDOK)
						{
							m_pAccountsList->AddTail(m_pAccount); 

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
						strMessage.Format(strTemplate, m_strUIN);
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
					}
				}
				else
				{
					strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_UIN");
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
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_UIN");
            CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CWizardICQAccountDialog::HasNextPage()
{
	return TRUE;
}

CString CWizardICQAccountDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardICQAccountDialog::OnBtnCancel()
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

CUOLPropPagePtr CWizardICQAccountDialog::GetNextPage(CUOLPropPagePtr pCurrentProp)
{
	return m_pNextPage;
}

void CWizardICQAccountDialog::OnClickButton(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
    CString strMessage;

	if (DoDataExchange(true))
	{
		if (!m_strUIN.IsEmpty())
		{
			if (!m_strPassword.IsEmpty())
			{
				if (m_pProtocol->IsValidUserName(m_strUIN.GetString()))
				{
                    m_pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(m_strUIN, m_strProtocolId);
					
					if (m_pAccount)
					{
						m_pAccount->SetPassword(m_strPassword);
					}

					if (!CheckDuplicateAccount(m_strUIN, m_strProtocolId, m_pAccount))
					{
						CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(m_pAccount);

						if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_ICQ_AUTH, m_pAccount) == IDOK)
						{
							m_pAccountsList->AddTail(m_pAccount); 

							DoDataExchange(FALSE);

							int nIndex = m_lstbCaptured.FindString(0, m_strUIN);
							if (nIndex != -1)
							{
								m_lstbCaptured.DeleteString(nIndex);                                
							}
							//TODO: Mudar para DoDataExchange.
							strMessage.Format(IDS_WIZARD_ADD_ACCOUNT_SUCCESS, m_strUIN);

							CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONINFORMATION);
                            
							(GetDlgItem(IDC_EDIT_WIZARD_ICQ_ID)).SetWindowText(_T(""));
							(GetDlgItem(IDC_EDIT_WIZARD_ICQ_PASSWORD)).SetWindowText(_T(""));
						}
						else
						{
							CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(m_pAccount);
						}
					}
					else
					{
						CString strFormat;
						strFormat = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_EXISTS_ACCOUNT");
                        strMessage.Format(strFormat, m_strUIN);
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
					}
				}
				else
				{
					strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_UIN");
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
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_UIN");
            CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CWizardICQAccountDialog::OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(TRUE);
	CString strSelected;

	m_lstbCaptured.GetText(m_lstbCaptured.GetCurSel(), strSelected);

	m_strUIN = strSelected;

	DoDataExchange(FALSE);
}

BOOL CWizardICQAccountDialog::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}

BOOL CWizardICQAccountDialog::CaptureICQAccounts()
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
