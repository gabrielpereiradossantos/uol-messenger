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
#include "JabberAccountAdvancedPropertyPage.h"

#include <jabberconstants.h>

#include "../../controller/UIMApplication.h"


using namespace std;


CJabberAccountAdvancedPropertyPage::CJabberAccountAdvancedPropertyPage(IUOLMessengerAccountPtr pAccount, OperationMode mode) :
	m_pAccount(pAccount),
	m_mode(mode),
	m_nPort(5222)
{
	m_strTitle.LoadString(IDS_ACCOUNT_EDIT_ADVANCED_PROPPAGE_CAPTION);
	SetTitle((LPCTSTR) m_strTitle);

	ATLASSERT(m_pAccount);
}


CJabberAccountAdvancedPropertyPage::~CJabberAccountAdvancedPropertyPage()
{
}


BOOL CJabberAccountAdvancedPropertyPage::SetString(const CString& strSettingId, const CString& strSettingValue)
{
	if (0 == strSettingId.CompareNoCase(JABBER_SETTING_CONNECT_SERVER))
	{
		SetServer(strSettingValue);
		
		return TRUE;
	}
	else
	{
		// TODO: implement other settings?!
		ATLASSERT(FALSE);
	}

	return FALSE;
}


BOOL CJabberAccountAdvancedPropertyPage::GetString(const CString& strSettingId, CString& strSettingValue)
{
	if (0 == strSettingId.CompareNoCase(JABBER_SETTING_CONNECT_SERVER))
	{
		strSettingValue = GetServer();

		return TRUE;
	}

	return FALSE;
}


void CJabberAccountAdvancedPropertyPage::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateFields();
}


void CJabberAccountAdvancedPropertyPage::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateFields();
}


void CJabberAccountAdvancedPropertyPage::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	OnDisconnected(pAccount);
}


LRESULT CJabberAccountAdvancedPropertyPage::OnInitDialog(HWND wnd, LPARAM lParam)
{
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(m_pAccount, this);
	}

	// Get protocol specific settings
	CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

	if (pProtocolSettings)
	{
		string strServer;

		pProtocolSettings->GetString(JABBER_SETTING_CONNECT_SERVER, strServer);

		m_strServer = strServer.c_str();

		pProtocolSettings->GetInteger(JABBER_SETTING_CONNECT_PORT, m_nPort);
	}

	DoDataExchange(FALSE);

	UpdateFields();

	return 0L;
}


void CJabberAccountAdvancedPropertyPage::OnDestroy()
{
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(m_pAccount, this);
	}
}


int CJabberAccountAdvancedPropertyPage::OnApply()
{
	DoDataExchange(TRUE);

	if (m_nPort <= 0)
	{
		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		CString strMessage;
		strMessage = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNT_EDIT_EMPTY_PORT");

		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

		GetDlgItem(IDC_EDIT_PORT).SetFocus();

		return PSNRET_INVALID_NOCHANGEPAGE;
	}

	// Get protocol specific settings
	CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

	if (pProtocolSettings)
	{
		pProtocolSettings->SetString(JABBER_SETTING_CONNECT_SERVER, (LPCTSTR) m_strServer);
		pProtocolSettings->SetInteger(JABBER_SETTING_CONNECT_PORT, m_nPort);
	}

	return PSNRET_NOERROR;

	//return PSNRET_INVALID_NOCHANGEPAGE;
}


CString CJabberAccountAdvancedPropertyPage::GetParentTitle()
{
	CString strTitle;

	CWindow wndParent = GetPropertySheet();
	
	ATLASSERT(wndParent.IsWindow());
	wndParent.GetWindowText(strTitle);

	return strTitle;
}


void CJabberAccountAdvancedPropertyPage::UpdateFields()
{
	DWORD dwConnectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(m_pAccount);

	BOOL bDisconnected = (dwConnectionState == IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);

	GetDlgItem(IDC_EDIT_SERVER).EnableWindow(bDisconnected);
	GetDlgItem(IDC_EDIT_PORT).EnableWindow(bDisconnected);
}


void CJabberAccountAdvancedPropertyPage::SetServer(const CString& strServer)
{
	if (::IsWindow(m_hWnd))
	{
		m_strServer = strServer;

		DoDataExchange(FALSE);
	}
	else
	{
		CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

		if (pProtocolSettings)
		{
			pProtocolSettings->SetString(JABBER_SETTING_CONNECT_SERVER, (LPCTSTR) strServer);
		}
	}
}


CString CJabberAccountAdvancedPropertyPage::GetServer()
{
	if (::IsWindow(m_hWnd))
	{
		DoDataExchange(TRUE);
	}
	else
	{
		CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

		if (pProtocolSettings)
		{
			string strServer;

			pProtocolSettings->GetString(JABBER_SETTING_CONNECT_SERVER, strServer);

			m_strServer = strServer.c_str();
		}
	}

	return m_strServer;
}