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
#include "AccountEditDialog.h"

#include "../../resource.h"


CAccountEditDialog::CAccountEditDialog(IUOLMessengerAccountPtr pAccount, OperationMode mode) :
	m_pAccount(pAccount),
	m_mode(mode),
	m_accountEditPropPage(pAccount, mode),
	m_jabberAccountAdvancedPropPage(pAccount, mode),
	m_icqAccountAdvancedPropPage(pAccount, mode)
{
	m_psh.dwFlags |= PSH_NOCONTEXTHELP | PSH_NOAPPLYNOW;

	AddPropertyPages();
}


CAccountEditDialog::~CAccountEditDialog()
{
}


LRESULT CAccountEditDialog::OnShowWindow(BOOL bShow, int nStatus)
{
	// Set window title...
	CString strTitle;
	
	if (m_mode == OM_ADD_ACCOUNT)
	{
		strTitle.LoadString(IDS_ACCOUNTS_TITLE_ADD);
	}
	else
	{
		strTitle.LoadString(IDS_ACCOUNTS_TITLE_EDIT);
	}

	SetWindowText(strTitle);

	// Set buttons title...
    CWindow wndOK, wndCancel;

	wndOK = GetDlgItem(IDOK);
	wndCancel = GetDlgItem(IDCANCEL);

	strTitle.LoadString(IDS_ACCOUNT_EDIT_DIALOG_OK_BUTTON);
	wndOK.SetWindowText(strTitle);

	strTitle.LoadString(IDS_ACCOUNT_EDIT_DIALOG_CANCEL_BUTTON);
	wndCancel.SetWindowText(strTitle);

	CenterWindow(GetParent());

	return 0L;
}


LRESULT CAccountEditDialog::OnOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	SetMsgHandled(FALSE);

	return 0L;
}


LRESULT CAccountEditDialog::OnCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	SetMsgHandled(FALSE);

	return 0L;
}


LRESULT CAccountEditDialog::OnChangeAdvancedPropertyPage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	LPCHANGEADVANCEDPROPPAGEPARAM pcap = (LPCHANGEADVANCEDPROPPAGEPARAM) wParam;

	if (GetPageCount() > 1)
	{
		RemovePage(1);
	}
	
	if (0 == (pcap->strProtocolId).Compare("prpl-jabber"))
	{
		AddPage(m_jabberAccountAdvancedPropPage);
	}
	else if (0 == (pcap->strProtocolId).Compare("prpl-icq"))
	{
		AddPage(m_icqAccountAdvancedPropPage);
	}

	return 0L;
}


LRESULT CAccountEditDialog::OnGetAdvancedSettingString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = FALSE;

	if (wParam && lParam)
	{
		CString strSettingId = (LPCTSTR) wParam;
		CString* pstrSettingValue = (CString*) lParam;

		CString strValue;
		bResult = m_jabberAccountAdvancedPropPage.GetString(strSettingId, strValue);

		if (bResult)
		{
			*pstrSettingValue = strValue;
		}
	}

	return (LRESULT) bResult;
}


LRESULT CAccountEditDialog::OnSetAdvancedSettingString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = FALSE;

	if (wParam && lParam)
	{
		CString strSettingId = (LPCTSTR) wParam;
		CString strSettingValue = (LPCTSTR) lParam;

		bResult = m_jabberAccountAdvancedPropPage.SetString(strSettingId, strSettingValue);
	}

	return (LRESULT) bResult;
}

LRESULT CAccountEditDialog::OnGetAdvancedSettingBoolean(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = FALSE;

	if (wParam && lParam)
	{
		CString strSettingId = (LPCTSTR) wParam;
		BOOL* pbSettingValue = (BOOL*) lParam;

		BOOL bValue = FALSE;
		bResult = m_icqAccountAdvancedPropPage.GetBoolean(strSettingId, bValue);

		if (bResult)
		{
			*pbSettingValue = bValue;
		}
	}

	return (LRESULT) bResult;
}


LRESULT CAccountEditDialog::OnSetAdvancedSettingBoolean(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = FALSE;

	if (wParam && lParam)
	{
		CString strSettingId = (LPCTSTR) wParam;
		BOOL bSettingValue = (BOOL) lParam;

		bResult = m_icqAccountAdvancedPropPage.SetBoolean(strSettingId, bSettingValue);
	}

	return (LRESULT) bResult;
}


void CAccountEditDialog::AddPropertyPages()
{
	AddPage(m_accountEditPropPage);
}