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

#include "WizardJabberAccountDialog.h"
#include "WizardPreferencesDialog.h"
#include "WizardController.h"

#include <jabberconstants.h>

#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "UOLWizard.h"


const CString CWizardJabberAccountDialog::m_strIdentitiesKey = _T("Software\\Google\\Google Talk\\Accounts");
const CString CWizardJabberAccountDialog::m_strProtocolId = _T("prpl-jabber");
const CString CWizardJabberAccountDialog::m_strDefaultPort = _T("5222");


CWizardJabberAccountDialog::CWizardJabberAccountDialog() :
	m_bkg(RGB(255, 255, 255))
{
	m_pNextPage = NULL;

    m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

CWizardJabberAccountDialog::~CWizardJabberAccountDialog()
{
    m_pAccount.reset();
}

void CWizardJabberAccountDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardJabberAccountDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardJabberAccountDialog::GetName() const
{
	return "__WizardJabberInstance";
}

void CWizardJabberAccountDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}


BOOL CWizardJabberAccountDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	/*Init*/
	m_pProtocol = CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocol(m_strProtocolId);
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	
	m_pNextPage = CWizardController::GetInstance()->GetNextValidPage(
			ID_JABBER_PROTOCOL_WIZARD_PAGE);

	CString strBtnText;

	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	/**/

	CEdit edtJabberEmail (GetDlgItem(IDC_EDIT_WIZARD_JABBER_EMAIL));
	CWindow grbAccounts (GetDlgItem(IDC_STATIC_WIZARD_JABBER_ACCOUNTS));

	m_lstbCaptured = GetDlgItem(IDC_LIST_WIZARD_JABBER_ACCOUNTS);

	m_staticEmail.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_JABBER_EMAIL));		
	m_staticEmail.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_JABBER_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));
	m_staticPort.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_JABBER_PORT));		
	m_staticPort.SetColor(RGB(0, 0, 0));
	m_staticServer.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_JABBER_SERVER));		
	m_staticServer.SetColor(RGB(0, 0, 0));
	
	m_staticSubHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_JABBER_SUBHEADER));		
	m_staticSubHeader.SetColor(RGB(0, 0, 0));
	CString Header = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.JABBER_HEADER");
	m_staticSubHeader.SetWindowText(Header);

	m_staticInfo.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_JABBER_INFO));		
	m_staticInfo.SetColor(RGB(0, 0, 0));
	CString Info = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.JABBER_INFO");
	m_staticInfo.SetWindowText(Info);

	if (CaptureJabberAccounts())
	{
		m_lstbCaptured.ShowWindow(TRUE);
 
		grbAccounts.ShowWindow(TRUE);
	}

	m_strPort = m_strDefaultPort;

	DoDataExchange(FALSE);

	edtJabberEmail.SetFocus();
	edtJabberEmail.SetLimitText(TIL_ACCOUNT_USER);

	CEdit wndPass = GetDlgItem(IDC_EDIT_WIZARD_JABBER_PASSWORD);
	wndPass.SetLimitText(TIL_ACCOUNT_PASSWORD);

	return FALSE;
}

LRESULT CWizardJabberAccountDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


BOOL CWizardJabberAccountDialog::OnBtnNext()
{
	DoDataExchange(TRUE);

	if (m_strEmail.IsEmpty() && m_strPassword.IsEmpty())
	{
		return TRUE;
	}

	return ValidateData(TRUE);
}


BOOL CWizardJabberAccountDialog::HasNextPage()
{
	return TRUE;
}

BOOL CWizardJabberAccountDialog::CaptureJabberAccounts()
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

CString CWizardJabberAccountDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardJabberAccountDialog::OnBtnCancel()
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

CUOLPropPagePtr CWizardJabberAccountDialog::GetNextPage(CUOLPropPagePtr pCurrentProp)
{
	return m_pNextPage;
}

void CWizardJabberAccountDialog::OnClickButton(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	ValidateData(FALSE);
}

void CWizardJabberAccountDialog::OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CString strSelected;

	DoDataExchange(TRUE);

	m_lstbCaptured.GetText(m_lstbCaptured.GetCurSel(), strSelected);

	m_strEmail = strSelected;

	LoadConnectServer(m_strEmail);

	DoDataExchange(FALSE);
}

void CWizardJabberAccountDialog::OnKillFocus(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(TRUE);

	LoadConnectServer(m_strEmail);

	DoDataExchange(FALSE);
}

BOOL CWizardJabberAccountDialog::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}

BOOL CWizardJabberAccountDialog::ValidateData(BOOL bIsNextButton)
{
	CString strMessage;
	BOOL bRet = FALSE;

	CProtocolSettingsPtr pProtocolSettings;
	int nPort;


	if (DoDataExchange(TRUE))
	{
		if (m_strEmail.IsEmpty())
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else if (m_strPassword.IsEmpty())
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_REQUIRED_PASSWORD");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else if (!m_pProtocol->IsValidUserName(m_strEmail.GetString()))
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_EMAIL");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		}
		else if (m_strPort.IsEmpty())
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_EMPTY_PORT");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else if ((m_strPort.GetLength() > MAX_LEN_PORT) || 
				 ((nPort = atoi(m_strPort)) == 0) ||
			     ((nPort = atoi(m_strPort)) > MAX_PORT))
		{
			strMessage = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_INVALID_PORT");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			m_pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(m_strEmail, m_strProtocolId);

			if (m_pAccount)
			{
				m_pAccount->SetPassword(m_strPassword);

				pProtocolSettings = m_pAccount->GetProtocolSettings();

				if (pProtocolSettings)
				{
					pProtocolSettings->SetString(JABBER_SETTING_CONNECT_SERVER, m_strServer.GetString());
					pProtocolSettings->SetInteger(JABBER_SETTING_CONNECT_PORT, nPort);
				}

				if (CheckDuplicateAccount(m_strEmail, m_strProtocolId, m_pAccount))
				{
					CString strTemplate = m_pWizard->GetWizardSettings()->GetValue("uol.resource.error.IDS_WIZARD_EXISTS_ACCOUNT");
					strMessage.Format(strTemplate, m_strEmail);
					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
				}
				else
				{
					CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(m_pAccount);

					if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_JABBER_AUTH, m_pAccount) == IDOK)
					{
						m_pAccountsList->AddHead(m_pAccount);

						int nIndex = m_lstbCaptured.FindString(0, m_strEmail);

						if (nIndex != -1)
						{
							m_lstbCaptured.DeleteString(nIndex);                                
						}

						if (!bIsNextButton)
						{
							CString strTemplate = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_ADD_ACCOUNT_SUCCESS");
							strMessage.Format(strTemplate, m_strEmail);
							CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONINFORMATION);
						}

						m_strEmail.Empty();
						m_strPassword.Empty();
						m_strServer.Empty();

						bRet = TRUE;
					}
					else
					{
						CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(m_pAccount);
					}
				}
			}
		}
	}

	DoDataExchange(FALSE);

	return bRet;
}


void CWizardJabberAccountDialog::LoadConnectServer(const CString& strEmail)
{
	m_strServer.Empty();

	if (m_pProtocol != NULL)
	{
		if (m_pProtocol->IsValidUserName((LPCTSTR) strEmail))
		{
			struct DomainServer
			{
				CString nDomainResId;
				CString nServerResId;
			};

			DomainServer domainServers[] = 
			{
				{ m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_GTALK_EMAIL_DOMAIN"), m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_GTALK_SERVER") },
				{ m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOLINC_EMAIL_DOMAIN"), m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOLINC_SERVER") }
			};

			int nCount = sizeof(domainServers) / sizeof(domainServers[0]);

			CString strDomain;

			for (int nInd = 0; nInd < nCount; nInd++)
			{
				strDomain = domainServers[nInd].nDomainResId;

				if (strEmail.Find(strDomain) != -1)
				{
					m_strServer= domainServers[nInd].nServerResId;
					break;
				}
			}
		}
	}
}