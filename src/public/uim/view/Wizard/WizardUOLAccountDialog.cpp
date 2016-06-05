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

#include "WizardUOLAccountDialog.h"
#include "WizardICQAccountDialog.h"
#include "WizardController.h"

#include <jabberconstants.h>

#include "../TextInputLimits.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "UOLWizard.h"

#include "../../controller/UIManager.h"


const CString CWizardUOLAccountDialog::m_strProtocolId = _T("prpl-uol");
const CString CWizardUOLAccountDialog::m_strDefaultPort = _T("5222");


CWizardUOLAccountDialog::CWizardUOLAccountDialog() : 
	m_bkg(RGB(255, 255, 255))
{
	m_pNextPage = NULL;

    m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

void CWizardUOLAccountDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardUOLAccountDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardUOLAccountDialog::GetName() const
{
	return "__WizardUOLInstance";
}

CWizardUOLAccountDialog::~CWizardUOLAccountDialog()
{
    m_pAccount.reset();
}

void CWizardUOLAccountDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}


BOOL CWizardUOLAccountDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	/*Init*/
	m_pProtocol = CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocol(m_strProtocolId);
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	
	m_pNextPage = CWizardController::GetInstance()->GetNextValidPage(
			ID_UOL_PROTOCOL_WIZARD_PAGE);
	
	ATLASSERT(NULL != m_pWizard);
	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	/**/

	CEdit edtUOLEmail (GetDlgItem(IDC_EDIT_WIZARD_UOLK_ID));

	m_staticHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_UOLK_HEADER));		
	m_staticHeader.SetColor(RGB(0, 0, 0));
	CString Header = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.UOL_HEADER");
	m_staticHeader.SetWindowText(Header);

	m_staticEmail.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_UOLK_EMAIL));		
	m_staticEmail.SetColor(RGB(0, 0, 0));
	m_staticPass.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_UOLK_PASS));		
	m_staticPass.SetColor(RGB(0, 0, 0));
	
	m_staticSubscribe.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_UOLK_SUBSCRIBE));
	m_staticSubscribe.SetColor(RGB(0, 0, 0));
	CString Subscribe = m_pWizard->GetWizardSettings()->GetValue("uol.resource.wizard.UOL_SUBSCRIBE");
	m_staticSubscribe.SetWindowText(Subscribe);

	m_strPort = m_strDefaultPort;

	DoDataExchange(FALSE);

	edtUOLEmail.SetFocus();
	edtUOLEmail.SetLimitText(TIL_ACCOUNT_USER);

	CEdit wndPass = GetDlgItem(IDC_EDIT_WIZARD_UOLK_PASSWORD);
	wndPass.SetLimitText(TIL_ACCOUNT_PASSWORD);

	return FALSE;
}

LRESULT CWizardUOLAccountDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


BOOL CWizardUOLAccountDialog::OnBtnNext()
{
	DoDataExchange(TRUE);

	if (m_strEmail.IsEmpty() && m_strPassword.IsEmpty())
	{
		return TRUE;
	}

	return ValidateData(TRUE);
}


BOOL CWizardUOLAccountDialog::HasNextPage()
{
	return TRUE;
}

CString CWizardUOLAccountDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardUOLAccountDialog::OnBtnCancel()
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

CUOLPropPagePtr CWizardUOLAccountDialog::GetNextPage(CUOLPropPagePtr pCurrentProp)
{
	return m_pNextPage;
}

void CWizardUOLAccountDialog::OnSubscribeButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CString strSubscriptionURL;

	strSubscriptionURL = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_STRING_BOL_SUBSCRIPTION_URL");

	::ShellExecute(NULL, _T("open"), strSubscriptionURL, NULL, NULL, SW_SHOWNORMAL);
}

void CWizardUOLAccountDialog::OnClickButton(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	ValidateData(FALSE);
}

void CWizardUOLAccountDialog::OnKillFocus(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(TRUE);

	LoadConnectServer(m_strEmail);

	DoDataExchange(FALSE);
}

BOOL CWizardUOLAccountDialog::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}

BOOL CWizardUOLAccountDialog::ValidateData(BOOL bIsNextButton)
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

					if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_JABBER_UOL_AUTH, m_pAccount) == IDOK)
					{
						m_pAccountsList->AddHead(m_pAccount);

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


void CWizardUOLAccountDialog::LoadConnectServer(const CString& strEmail)
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
				{ m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_EMAIL_DOMAIN"), m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_SERVER") },
				{ m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_BOL_EMAIL_DOMAIN"), m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_SERVER") }
			};

			int nCount = sizeof(domainServers) / sizeof(domainServers[0]);

			CString strDomain;

			for (int nInd = 0; nInd < nCount; nInd++)
			{
				strDomain = domainServers[nInd].nDomainResId;

				if (strEmail.Find(strDomain) != -1)
				{
					m_strServer = domainServers[nInd].nServerResId;
					break;
				}
			}
		}
	}
}