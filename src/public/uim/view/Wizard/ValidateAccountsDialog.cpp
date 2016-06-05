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

#include "ValidateAccountsDialog.h"
#include "../../controller/uimanager.h"
#include "../../controller/UIMApplication.h"

#include <commands/RegisterUOLKCommand.h>

CValidateAccountsDialog::CValidateAccountsDialog(UOLLib::ISettings* pWizardSettings)
{
	m_enStateConnection = AUTH_OFFLINE;

	m_bSilent = FALSE;

	m_pWizardSettings = pWizardSettings;
}

LRESULT CValidateAccountsDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	CenterWindow();
 
	return TRUE;
}

LRESULT CValidateAccountsDialog::OnShowWindow(WPARAM wParam, LPARAM lParam)
{
	DoAuthentication();

	return TRUE;
}


void CValidateAccountsDialog::OnDestroy()
{

}

LRESULT CValidateAccountsDialog::OnCancel(WPARAM wParam, LPARAM lParam , HWND hwnd, BOOL& bHandled)
{
	if (m_enStateConnection == AUTH_CONNECTING)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->Disconnect(m_accountPtr);
        
		m_enStateConnection = AUTH_CANCEL;
	}
	else
	{
        Finalize(IDCANCEL);
	}

	return 0;
}


LRESULT CValidateAccountsDialog::OnFinalizeDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Finalize((UINT) wParam, (BOOL) lParam);

	return 0L;
}


void CValidateAccountsDialog::Finalize(UINT uCode, BOOL bUnregister)
{
	if (bUnregister)
	{
		UnRegisterObservers();
	}

	if (::IsWindow(m_hWnd))
	{
		EndDialog(uCode);
	}
}

INT_PTR CValidateAccountsDialog::DoValidation(IDAuthentication enID, IUOLMessengerAccountPtr accountPtr)
{
	m_accountPtr = accountPtr;

    m_nID = enID;
    
	return DoModal();
}

INT_PTR CValidateAccountsDialog::DoValidation(IDAuthentication enID, const CString& strLogin, const CString& strPassword)
{
	m_strLogin = strLogin;
	m_strPassword = strPassword;

	m_nID = enID;

	return DoModal();
}

void CValidateAccountsDialog::DoAuthentication()
{
	CString strMessage;


	switch(m_nID)
	{
	case IDAuthentication::ID_UOL_SILENT_AUTH:
		m_bSilent = TRUE;

	case IDAuthentication::ID_UOL_AUTH:
		strMessage = m_pWizardSettings->GetValue("uol.resource.IDS_VALIDATE_ACCOUNTS_UOL_AUTH_LABEL");
		SetDlgItemText(IDC_STATIC_WAIT, strMessage);
		UOLAuthentication();
		break;

	case IDAuthentication::ID_MSN_AUTH:
        strMessage = m_pWizardSettings->GetValue("uol.resource.IDS_VALIDATE_ACCOUNTS_MSN_AUTH_LABEL");
		SetDlgItemText(IDC_STATIC_WAIT, strMessage);
		AccountAuthentication();
		break;

	case IDAuthentication::ID_ICQ_AUTH:
        strMessage = m_pWizardSettings->GetValue("uol.resource.IDS_VALIDATE_ACCOUNTS_ICQ_AUTH_LABEL");
		SetDlgItemText(IDC_STATIC_WAIT, strMessage);
		AccountAuthentication();
		break;

	case IDAuthentication::ID_YAHOO_AUTH:
        strMessage = m_pWizardSettings->GetValue("uol.resource.IDS_VALIDATE_ACCOUNTS_YAHOO_AUTH_LABEL");
		SetDlgItemText(IDC_STATIC_WAIT, strMessage);
		AccountAuthentication();
		break;

	case IDAuthentication::ID_JABBER_AUTH:
        strMessage = m_pWizardSettings->GetValue("uol.resource.IDS_VALIDATE_ACCOUNTS_JABBER_AUTH_LABEL");
		SetDlgItemText(IDC_STATIC_WAIT, strMessage);
		AccountAuthentication();
		break;

	case IDAuthentication::ID_JABBER_UOL_SILENT_AUTH:
		m_bSilent = TRUE;

	case IDAuthentication::ID_JABBER_UOL_AUTH:
        strMessage = m_pWizardSettings->GetValue("uol.resource.IDS_VALIDATE_ACCOUNTS_JABBER_AUTH_LABEL");
		SetDlgItemText(IDC_STATIC_WAIT, strMessage);
		AccountAuthentication();
		break;
	}
}

void CValidateAccountsDialog::UOLAuthentication()
{
	CString strMessage;
    IUOLMessengerAuthenticationManagerPtr authManager;

	authManager = CUIMApplication::GetApplication()->GetAuthenticationManager();
	authManager->RegisterObserver(this);

	if (!authManager->DoAuthentication(m_strLogin, m_strPassword, FALSE))
	{
		if (m_bSilent == FALSE)
		{
            strMessage = m_pWizardSettings->GetValue("uol.resource.error.IDS_WIZARD_ERROR_SERVER");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		}

		Finalize(IDCANCEL);
	}
}

void CValidateAccountsDialog::AccountAuthentication()
{
	if (m_accountPtr)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(m_accountPtr, this);

 		if(!CUIMApplication::GetApplication()->GetAccountConnectionManager()->Connect(m_accountPtr))
		{
            TreatErrors(m_accountPtr->GetUserId(), m_accountPtr->GetProtocolName(), CONNECTION_ERROR_UNKNOWN);

			Finalize(IDCANCEL);
		}
		else
		{
            InterlockedExchange((LONG *) &m_enStateConnection, AUTH_CONNECTING);
		}
	}
}

void CValidateAccountsDialog::UnRegisterObservers()
{
	if (m_nID == IDAuthentication::ID_UOL_SILENT_AUTH ||
		m_nID == IDAuthentication::ID_UOL_AUTH)
	{
		IUOLMessengerAuthenticationManagerPtr authManager;
		authManager = CUIMApplication::GetApplication()->GetAuthenticationManager();
		authManager->UnregisterObserver(this);
	}
	else
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(m_accountPtr, this);
	}
}

void CValidateAccountsDialog::OnAuthenticate(const CString & cookie, LONG status)
{
    CString strMessage;
	UINT idRet = IDCANCEL;
	IUOLMessengerAuthenticationManagerPtr authManager;

	UnRegisterObservers();

	if (CInternetAuthentication::AUTH_OK == status)
	{
		idRet = IDOK;
	}
	else
	{
		if (CInternetAuthentication::AUTH_PASS_LOGIN_ERROR == status)
		{
			if (m_bSilent == TRUE)
			{
				idRet = ID_ERROR_LOGIN;
			}
            else
			{
				strMessage = m_pWizardSettings->GetValue("uol.resource.error.IDS_WIZARD_INVALID_LOGIN_PASS");
				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
			}
		}
        else if (m_bSilent == FALSE)
		{
			strMessage = m_pWizardSettings->GetValue("uol.resource.error.IDS_WIZARD_ERROR_SERVER");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		}
	}

    Finalize(idRet, FALSE);
}

void CValidateAccountsDialog::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	ATLASSERT(m_accountPtr != NULL);
    
	InterlockedExchange((LONG *) &m_enStateConnection, AUTH_REPORT_DISCONNECT);
	InterlockedExchange((LONG *) &m_enError, error);

	// Must this setting be protected by a secure block like the members above ?
	m_strReportDisconnectionMsg = strMessage;
	
	if (0 == strcmp(_T("prpl-uol"), pAccount->GetProtocol()->GetId().c_str()))
	{
		if (error == CONNECTION_ERROR_REGISTRATION_REQUIRED)
		{
			BOOL bRegisterOk = TRUE;
			
			m_enStateConnection = AUTH_OK;

			IUOLMessengerCommandPtr pRegisterUOLKCommand = 
					new CRegisterUOLKCommand(pAccount, TRUE);
			
			CUIMApplication::GetApplication()->ExecuteCommand(pRegisterUOLKCommand);
			
			bRegisterOk = dynamic_cast<CRegisterUOLKCommand*>(
					pRegisterUOLKCommand.get())->RegisterSuccessfull();
			
			if (bRegisterOk)
			{
				CUIMApplication::GetApplication()->
						GetAccountConnectionManager()->Connect(pAccount);
			}
		}
	}
}

void CValidateAccountsDialog::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	m_enStateConnection = AUTH_OK;

	CUIMApplication::GetApplication()->GetAccountConnectionManager()->Disconnect(m_accountPtr);
}

void CValidateAccountsDialog::OnDisconnected(IUOLMessengerAccountPtr pAccount)
 {
	if (m_enStateConnection == AUTH_REPORT_DISCONNECT)
	{
        TreatErrors(m_accountPtr->GetUserId(), m_accountPtr->GetProtocolName(), m_enError);

		InterlockedExchange((LONG *) &m_enStateConnection, AUTH_OFFLINE);
  
		PostMessage(WM_USER_FINALIZE_DIALOG, (WPARAM) IDCANCEL, (LPARAM) TRUE);
	}
	else if (m_enStateConnection == AUTH_OK)
	{
		PostMessage(WM_USER_FINALIZE_DIALOG, (WPARAM) IDOK, (LPARAM) TRUE);
	}
	else
	{
		PostMessage(WM_USER_FINALIZE_DIALOG, (WPARAM) IDCANCEL, (LPARAM) TRUE);
	}
}

void CValidateAccountsDialog::TreatErrors(const CString& strId, const CString& strProtocol, ConnectionError error)
{
	CString strMessage;
    CString strTextFormat;
	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	const CProtocolResourceTranslator* pTranslator = pUIManager->GetProtocolResourceTranslator();

	ATLASSERT(pTranslator != NULL);

	if (pTranslator->GetConnectionErrorTextFormat(error, strTextFormat))
	{
		if (m_bSilent == FALSE)
		{
			if (error != CONNECTION_ERROR_UOL_SERVER_ERROR)
			{
				strMessage.Format(strTextFormat, strId, strProtocol);
			}
			else
			{
				// CONNECTION_ERROR_UOL_SERVER_ERROR demands 3 str params "%s"...
				//
				strMessage.Format(strTextFormat, strId, strProtocol, m_strReportDisconnectionMsg);
			}
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"O acorreu o erro [%s] durante o estabelecimento da conexão.", 
					strMessage);
			
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		}
	}
}
