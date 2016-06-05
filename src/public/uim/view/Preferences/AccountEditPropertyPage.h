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

#include <interfaces/IUOLMessengerAccountConnectionManager.h>
#include "AccountEditConsts.h"
#include "../../resource.h"
#include "../UOLOwnerDraw.h"

class CAccountEditPropertyPage : 
	public CPropertyPageImpl<CAccountEditPropertyPage>,
	public CWinDataExchange<CAccountEditPropertyPage>,
	public CUOLMessengerAccountConnectionManagerObserver
{
public:
	enum { IDD = IDD_DIALOG_EDIT_ACCOUNT };

	CAccountEditPropertyPage(IUOLMessengerAccountPtr pAccount, OperationMode mode);
	virtual ~CAccountEditPropertyPage();

	void OnConnected(IUOLMessengerAccountPtr pAccount);
	void OnDisconnected(IUOLMessengerAccountPtr pAccount);
	void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage);

protected:
	BEGIN_MSG_MAP(CAccountEditPropertyPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_PAINT(OnPaint)
		COMMAND_HANDLER_EX(IDC_COMBO_NET, LBN_SELCHANGE, OnSelChangeProtocolComboBox)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CHANGE_AVATAR, OnButtonChangeAvatar)
		COMMAND_HANDLER_EX(IDC_EDIT_USER, EN_KILLFOCUS, OnKillFocus)
		CHAIN_MSG_MAP(CPropertyPageImpl<CAccountEditPropertyPage>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND wnd, LPARAM lParam);
	void OnDestroy();
	void OnPaint(HDC hDC);
	void OnSelChangeProtocolComboBox(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnButtonChangeAvatar(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
    void OnKillFocus(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);

	BEGIN_DDX_MAP(CAccountEditPropertyPage)
		DDX_TEXT(IDC_EDIT_USER, m_strUser)
		DDX_TEXT(IDC_EDIT_PASS, m_strPass)
		DDX_TEXT(IDC_EDIT_NICK, m_strNick)
		DDX_CHECK(IDC_CHECK_CONNECT_ON_STARTUP, m_nConnectOnStartUp);
		DDX_CHECK(IDC_CHECK_SAVE_PASSWORD, m_nSavePassword);
	END_DDX_MAP()

	int OnApply();
	BOOL OnQueryCancel();

private:
	void InitProtocolList();

	void SplitUserId(const CString& strUserId, const CString& strSplitter, CString& strSuffix, CString& strPrefix);

	void UpdateFields();
	BOOL ValidateFields();

	CIMProtocol* GetComboProtocol();

	void OnSelChangeProtocolComboBoxImpl();

	BOOL CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount);

	BOOL IsGmailAccount(const CString& strEmail);

	CString GetParentTitle();

	CString GetAccountConnectServer(const CString& strEmail, BOOL& bForce);

	CString GetAccountSettingsConnectServer();
	void SetAccountSettingsConnectServer(const CString& strServer);
	void UpdateAccountSettingsConnectServer();

	BOOL GetAccountSettingsWebAware();
	void SetAccountSettingsWebAware(const BOOL& bWebAware);
	BOOL GetAccountSettingsRequestAuthorization();
	void SetAccountSettingsRequestAuthorization(const BOOL& bRequestAuthorization);

private:
	CString		m_strTitle;

	IUOLMessengerAccountPtr		m_pAccount;
	OperationMode				m_mode;

	CUOLComboBox				m_wndComboBoxProtocols;
	CString						m_strUser;
	CString						m_strPass;
	CString						m_strNick;
	int							m_nConnectOnStartUp;
	int							m_nSavePassword;
	IUOLMessengerUserIconPtr	m_pUserIcon;

	CString						m_strOldServer;
	BOOL						m_bOldWebAware;
	BOOL						m_bOldRequestAuthorization;
};