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

#include "PreferencesPropPage.h"
#include "../../resource.h"
#include <interfaces/IUOLMessengerNetworkSettings.h>


class CNetworkPropPage : public CPreferencesPropPage,
						 public CWinDataExchange<CNetworkPropPage>
{
public:
	CNetworkPropPage();
	virtual ~CNetworkPropPage();

	virtual void OnApply();

protected:
	BEGIN_MSG_MAP(CNetworkPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		//MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		COMMAND_HANDLER_EX(IDC_COMBO_PROXY_TYPE, LBN_SELCHANGE, OnSelChangeProxyTypeComboBox)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_HOST, EN_KILLFOCUS, OnKillFocusProxyHost)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_PORT, EN_KILLFOCUS, OnKillFocusProxyPort)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_USERNAME, EN_KILLFOCUS, OnKillFocusProxyUsername)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_PASSWORD, EN_KILLFOCUS, OnKillFocusProxyPassword)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CNetworkPropPage)
		DDX_TEXT(IDC_EDIT_PROXY_HOST, m_strProxyHost)
		DDX_TEXT(IDC_EDIT_PROXY_PORT, m_strProxyPort)
		DDX_TEXT(IDC_EDIT_PROXY_USERNAME, m_strProxyUsername)
		DDX_TEXT(IDC_EDIT_PROXY_PASSWORD, m_strProxyPassword)
	END_DDX_MAP()

	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnSelChangeProxyTypeComboBox(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnKillFocusProxyHost(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnKillFocusProxyPort(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnKillFocusProxyUsername(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnKillFocusProxyPassword(UINT /*uCode*/, int nID, HWND hwndCtrl);

private:
	void OnSelChangeProxyTypeComboBoxImpl();
	int ConvertProxyPort(const CString& strPort);

private:
	enum TextInputLimit
	{
		TIL_PROXY_HOST = 256,
		TIL_PROXY_PORT = 5,
		TIL_PROXY_USERNAME = TIL_PROXY_HOST,
		TIL_PROXY_PASSWORD = TIL_PROXY_HOST
	};

	IUOLMessengerNetworkSettingsPtr m_pSettings;

	CComboBox	m_cmbProxyType;

	CString		m_strProxyHost;
	CString		m_strProxyPort;
	CString		m_strProxyUsername;
	CString		m_strProxyPassword;

protected:	
	enum NotifyListener
	{
		NL_PROXY_TYPE = 1,
		NL_PROXY_HOST = 2,
		NL_PROXY_PORT = 4,
		NL_PROXY_USERNAME = 8,
		NL_PROXY_PASSWORD = 16
	};

	DWORD m_dwToNotify;
};
MAKEAUTOPTR(CNetworkPropPage);
