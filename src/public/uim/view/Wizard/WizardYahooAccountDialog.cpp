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

#include "WizardYahooAccountDialog.h"
#include "WizardJabberAccountDialog.h"
#include "WizardController.h"

#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "UOLWizard.h"


const CString CWizardYahooAccountDialog::m_strIdentitiesKey = _T("Software\\Yahoo\\Profiles");
const CString CWizardYahooAccountDialog::m_strProtocolId = _T("prpl-yahoo");


CWizardYahooAccountDialog::CWizardYahooAccountDialog() :
	m_bkg(RGB(255, 255, 255))
{
	m_pNextPage = NULL;

    m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

CWizardYahooAccountDialog::~CWizardYahooAccountDialog()
{
    m_pAccount.reset();
}

void CWizardYahooAccountDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardYahooAccountDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardYahooAccountDialog::GetName() const
{
	return "__WizardYahooInstance";
}

void CWizardYahooAccountDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}


BOOL CWizardYahooAccountDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	/*Init*/
	m_pProtocol = CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocol(m_strProtocolId);
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	
	m_pNextPage = CWizardController::GetInstance()->GetNextValidPage(
			ID_YAHOO_PROTOCOL_WIZARD_PAGE);
	
	ATLASSERT(NULL != m_pWizard);
	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	/**/
	
	CEdit edtYahooId (GetDlgItem(IDC_EDIT_WIZARD_YAHOO_ID));
	CWindow grbAccounts (GetDlgItem(IDC_STATIC_WIZARD_YAHOO_ACCOUNTS));

	m_lstbCaptured = GetDlgItem(IDC_LIST_WIZARD_YAHOO_ACCOUNTS);

	m_staticID.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_YAHOO_ID));		
	m_staticID.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_YAHOO_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));
	
	m_staticSubHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_YAHOO_SUBHEADER));		
	m_staticSubHeader.SetColor(RGB(0, 0, 0));
	CString Header = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.YAHOO_HEADER");
	m_staticSubHeader.SetWindowText(Header);

	m_staticInfo.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_YAHOO_INFO));		
	m_staticInfo.SetColor(RGB(0, 0, 0));
	CString Info = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.YAHOO_INFO");
	m_staticInfo.SetWindowText(Info);

    if (CaptureYahooAccounts())
	{
		m_lstbCaptured.ShowWindow(TRUE);
 
		grbAccounts.ShowWindow(TRUE);
	}

	edtYahooId.SetFocus();
	edtYahooId.SetLimitText(TIL_ACCOUNT_USER);

	CEdit wndPass = GetDlgItem(IDC_EDIT_WIZARD_YAHOO_PASSWORD);
	wndPass.SetLimitText(TIL_ACCOUNT_PASSWORD);

	return FALSE;
}

LRESULT CWizardYahooAccountDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


BOOL CWizardYahooAccountDialog::OnBtnNext()
{
	DoDataExchange(TRUE);

	if (m_strID.IsEmpty() && m_strPassword.IsEmpty())
	{
		return TRUE;
	}

	return ValidateData(TRUE);
}

BOOL CWizardYahooAccountDialog::HasNextPage()
{
	return TRUE;
}

BOOL CWizardYahooAccountDialog::CaptureYahooAccounts()
{
	int nIndex = 0;
	BOOL bRet = FALSE;
	CRegKey reg;
    DWORD cbName = KEY_LEN;
    TCHAR szSubKeyName[KEY_LEN];
	CString strInvalidID("Skin");


	if (ERROR_SUCCESS == reg.Open(HKEY_CURRENT_USER, m_strIdentitiesKey))
	{
		while (reg.EnumKey(nIndex++, szSubKeyName, &cbName, NULL) != ERROR_NO_MORE_ITEMS)
		{
            if (m_pProtocol->IsValidUserName(szSubKeyName) && strInvalidID.Compare(szSubKeyName) != 0)
			{
                m_lstbCaptured.AddString(szSubKeyName); 
               
				bRet = TRUE;
			}

            cbName = KEY_LEN;
		}
	}
	
	return bRet;
}

CString CWizardYahooAccountDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardYahooAccountDialog::OnBtnCancel()
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

CUOLPropPagePtr CWizardYahooAccountDialog::GetNextPage(CUOLPropPagePtr pCurrentProp)
{
	return m_pNextPage;
}

void CWizardYahooAccountDialog::OnClickButton(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	ValidateData(FALSE);
}

void CWizardYahooAccountDialog::OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CString strSelected;

	DoDataExchange(TRUE);

	m_lstbCaptured.GetText(m_lstbCaptured.GetCurSel(), strSelected);

	m_strID = strSelected;

	DoDataExchange(FALSE);
}

BOOL CWizardYahooAccountDialog::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}

BOOL CWizardYahooAccountDialog::ValidateData(BOOL bIsButtonNext)
{
	BOOL bRet = FALSE;
	CString strMessage;

	if (DoDataExchange(TRUE))
	{
		if (m_strID.IsEmpty())
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_ID");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else if (m_strPassword.IsEmpty())
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else if (!m_pProtocol->IsValidUserName(m_strID.GetString()))
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_ID");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		}
		else
		{
			m_pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(m_strID, m_strProtocolId);

			if (m_pAccount)
			{
				m_pAccount->SetPassword(m_strPassword);
			}

			if (!CheckDuplicateAccount(m_strID, m_strProtocolId, m_pAccount))
			{
				CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(m_pAccount);

				if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_YAHOO_AUTH, m_pAccount) == IDOK)
				{
					m_pAccountsList->AddHead(m_pAccount);

					int nIndex = m_lstbCaptured.FindString(0, m_strID);
					if (nIndex != -1)
					{
						m_lstbCaptured.DeleteString(nIndex);                                
					}

					if (!bIsButtonNext)
					{
						CString strTemplate = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_ADD_ACCOUNT_SUCCESS");
                        strMessage.Format(strTemplate, m_strID);
                        CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONINFORMATION);
					}

					m_strID.Empty();
					m_strPassword.Empty();

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
				strMessage.Format(strTemplate, m_strID);
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
			}
		}
	}

	DoDataExchange(FALSE);

	return bRet;
}