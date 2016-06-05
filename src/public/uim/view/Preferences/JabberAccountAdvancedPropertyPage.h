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
#include "AccountEditConsts.h"
#include <interfaces/IUOLMessengerAccountConnectionManager.h>


class CJabberAccountAdvancedPropertyPage : 
	public CPropertyPageImpl<CJabberAccountAdvancedPropertyPage>,
	public CWinDataExchange<CJabberAccountAdvancedPropertyPage>,
	public CUOLMessengerAccountConnectionManagerObserver
{
public:
	enum { IDD = IDD_DIALOG_EDIT_JABBER_ACCOUNT_ADVANCED };

	CJabberAccountAdvancedPropertyPage(IUOLMessengerAccountPtr pAccount, OperationMode mode);
	virtual ~CJabberAccountAdvancedPropertyPage();

	BOOL SetString(const CString& strSettingId, const CString& strSettingValue);
	BOOL GetString(const CString& strSettingId, CString& strSettingValue);

	void OnConnected(IUOLMessengerAccountPtr pAccount);
	void OnDisconnected(IUOLMessengerAccountPtr pAccount);
	void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage);

protected:
	BEGIN_MSG_MAP(CJabberAccountAdvancedPropertyPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(CPropertyPageImpl<CJabberAccountAdvancedPropertyPage>)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND wnd, LPARAM lParam);
	void OnDestroy();

	BEGIN_DDX_MAP(CJabberAccountAdvancedPropertyPage)
		DDX_TEXT(IDC_EDIT_SERVER, m_strServer)
		DDX_INT(IDC_EDIT_PORT, m_nPort)
	END_DDX_MAP()

	int OnApply();

	CString GetParentTitle();

private:
	void UpdateFields();

	void SetServer(const CString& strServer);
	CString GetServer();

private:
	CString		m_strTitle;
	IUOLMessengerAccountPtr		m_pAccount;
	OperationMode				m_mode;

	CString		m_strServer;
	int			m_nPort;
};
