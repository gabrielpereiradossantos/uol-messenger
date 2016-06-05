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


#pragma once

#include "../../resource.h"

#include <interfaces/IUOLMessengerAccountConnectionManager.h>
#include <interfaces/IUOLMessengerAuthenticationNotify.h>

#include "../../authentication/InternetAuthentication.h"
#include "UOLWizard.h"


#define		ID_ERROR_LOGIN		101



class CValidateAccountsDialog : 
	public CAuthenticationObserver, 
	public CUOLMessengerAccountConnectionManagerObserver,
    public CDialogImpl<CValidateAccountsDialog>
{
	enum
	{
		WM_USER_FINALIZE_DIALOG = WM_USER + 0x101,
	};

    BEGIN_MSG_MAP(CValidateAccountsDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_USER_FINALIZE_DIALOG, OnFinalizeDialog)
    END_MSG_MAP()

public:
    enum { IDD = IDD_DIALOG_WAIT_PROFILE };

	static enum IDAuthentication
	{
		ID_UOL_AUTH = 0,
		ID_UOL_SILENT_AUTH,
		ID_MSN_AUTH, 
		ID_ICQ_AUTH,
		ID_YAHOO_AUTH,
		ID_JABBER_AUTH,
		ID_JABBER_UOL_AUTH,
		ID_JABBER_UOL_SILENT_AUTH
	};

    CValidateAccountsDialog(UOLLib::ISettings* pWizardSettings);

	LRESULT OnInitDialog(HWND hwnd, LPARAM lParam);
	LRESULT OnShowWindow(WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	LRESULT OnCancel(WPARAM wParam, LPARAM lParam , HWND hwnd, BOOL& bHandled);
	LRESULT OnFinalizeDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	INT_PTR DoValidation(IDAuthentication enID, IUOLMessengerAccountPtr accountPtr);
	INT_PTR DoValidation(IDAuthentication enID, const CString& strLogin, const CString& strPassword);

	void DoAuthentication();
    void UOLAuthentication();
    void AccountAuthentication();	

	void Finalize(UINT uCode, BOOL bUnregister = TRUE);

	void UnRegisterObservers();

	//UOL callbacks
	//void OnAuthentication(LONG authenticated, CString cookie);
	void OnAuthenticate(const CString & cookie, LONG status);

    void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage);
	void OnConnected(IUOLMessengerAccountPtr pAccount);
	void OnDisconnected(IUOLMessengerAccountPtr pAccount);

    void TreatErrors(const CString& strId, const CString& strProtocol, ConnectionError error);

private:

	enum StateConnection
	{
		AUTH_OK,
		AUTH_REPORT_DISCONNECT,
		AUTH_OFFLINE,
		AUTH_CONNECTING,
		AUTH_CANCEL
	};

	int m_nID;

	CString m_strLogin;
	CString m_strPassword;

	BOOL m_bSilent;

	IUOLMessengerAccountPtr m_accountPtr;

    UINT m_idRet;

	StateConnection m_enStateConnection;
	ConnectionError m_enError;

	UOLLib::ISettings* m_pWizardSettings;
	
	CString m_strReportDisconnectionMsg;
};

MAKEAUTOPTR (CValidateAccountsDialog)