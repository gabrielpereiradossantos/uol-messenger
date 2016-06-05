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
#include "ICQAccountAdvancedPropertyPage.h"

#include <oscarconstants.h>

#include "../../controller/UIMApplication.h"


using namespace std;


CICQAccountAdvancedPropertyPage::CICQAccountAdvancedPropertyPage(IUOLMessengerAccountPtr pAccount, OperationMode mode) :
	m_pAccount(pAccount),
	m_mode(mode),
	m_bChkWebAware(FALSE),
	m_bChkRequestAuthorization(TRUE)
{
	m_strTitle.LoadString(IDS_ACCOUNT_EDIT_ADVANCED_PROPPAGE_CAPTION);
	SetTitle((LPCTSTR) m_strTitle);

	ATLASSERT(m_pAccount);
}


CICQAccountAdvancedPropertyPage::~CICQAccountAdvancedPropertyPage()
{
}


BOOL CICQAccountAdvancedPropertyPage::SetBoolean(const CString& strSettingId, const BOOL& bSettingValue)
{
	if (0 == strSettingId.CompareNoCase("web_aware"))
	{
		SetWebAware(bSettingValue);
		
		return TRUE;
	}
	else if (0 == strSettingId.CompareNoCase("authorization"))
	{
		SetRequestAuthorization(bSettingValue);
		
		return TRUE;
	}
	else
	{
		// TODO: implement other settings?!
		ATLASSERT(FALSE);
	}

	return FALSE;
}


BOOL CICQAccountAdvancedPropertyPage::GetBoolean(const CString& strSettingId, BOOL& bSettingValue)
{
	if (0 == strSettingId.CompareNoCase("web_aware"))
	{
		bSettingValue = GetWebAware();

		return TRUE;
	}
	else if (0 == strSettingId.CompareNoCase("authorization"))
	{
		bSettingValue = GetRequestAuthorization();

		return TRUE;
	}

	return FALSE;
}


void CICQAccountAdvancedPropertyPage::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateFields();
}


void CICQAccountAdvancedPropertyPage::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateFields();
}


void CICQAccountAdvancedPropertyPage::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	OnDisconnected(pAccount);
}


LRESULT CICQAccountAdvancedPropertyPage::OnInitDialog(HWND wnd, LPARAM lParam)
{
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(m_pAccount, this);
	}

	// Get protocol specific settings
	CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

	if (pProtocolSettings)
	{
		//Web Aware
		bool bWebAware = false;
		pProtocolSettings->GetBoolean("web_aware", bWebAware);
		m_bChkWebAware = bWebAware;

		//Request Authorization
		bool bRequestAuthorization = true;
		pProtocolSettings->GetBoolean("authorization", bRequestAuthorization);
		m_bChkRequestAuthorization = bRequestAuthorization;
	}

	DoDataExchange(FALSE);

	UpdateFields();

	return 0L;
}


void CICQAccountAdvancedPropertyPage::OnDestroy()
{
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(m_pAccount, this);
	}
}


int CICQAccountAdvancedPropertyPage::OnApply()
{
	DoDataExchange(TRUE);

	// Get protocol specific settings
	CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

	if (pProtocolSettings)
	{
		pProtocolSettings->SetBoolean("web_aware", (bool)m_bChkWebAware);
		pProtocolSettings->SetBoolean("authorization", (bool)m_bChkRequestAuthorization);
	}

	return PSNRET_NOERROR;

	//return PSNRET_INVALID_NOCHANGEPAGE;
}


CString CICQAccountAdvancedPropertyPage::GetParentTitle()
{
	CString strTitle;

	CWindow wndParent = GetPropertySheet();
	
	ATLASSERT(wndParent.IsWindow());
	wndParent.GetWindowText(strTitle);

	return strTitle;
}


void CICQAccountAdvancedPropertyPage::UpdateFields()
{
	DWORD dwConnectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(m_pAccount);

	BOOL bDisconnected = (dwConnectionState == IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);

	GetDlgItem(IDC_CHK_WEB_AVAILABLE).EnableWindow(bDisconnected);
	GetDlgItem(IDC_CHK_REQUEST_AUTHORIZATION).EnableWindow(bDisconnected);
}


void CICQAccountAdvancedPropertyPage::SetWebAware(const BOOL& bWebAware)
{
	if (::IsWindow(m_hWnd))
	{
		m_bChkWebAware = bWebAware;

		DoDataExchange(FALSE);
	}
	else
	{
		CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

		if (pProtocolSettings)
		{
			pProtocolSettings->SetBoolean("web_aware", (bool)bWebAware);
		}
	}
}


BOOL CICQAccountAdvancedPropertyPage::GetWebAware()
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
			bool bWebAware = false;
			pProtocolSettings->GetBoolean("web_aware", bWebAware);
			m_bChkWebAware = bWebAware;
		}
	}

	return m_bChkWebAware;
}

void CICQAccountAdvancedPropertyPage::SetRequestAuthorization(const BOOL& bRequestAuthorization)
{
	if (::IsWindow(m_hWnd))
	{
		m_bChkRequestAuthorization = bRequestAuthorization;

		DoDataExchange(FALSE);
	}
	else
	{
		CProtocolSettingsPtr pProtocolSettings = m_pAccount->GetProtocolSettings();

		if (pProtocolSettings)
		{
			pProtocolSettings->SetBoolean("authorization", (bool)bRequestAuthorization);
		}
	}
}


BOOL CICQAccountAdvancedPropertyPage::GetRequestAuthorization()
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
			bool bRequestAuthorization = true;
			pProtocolSettings->GetBoolean("authorization", bRequestAuthorization);
			m_bChkRequestAuthorization = bRequestAuthorization;
		}
	}

	return m_bChkRequestAuthorization;
}