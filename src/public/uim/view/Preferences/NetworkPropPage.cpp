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
#include "NetworkPropPage.h"

#include "../../controller/UIMApplication.h"


CNetworkPropPage::CNetworkPropPage() :
	CPreferencesPropPage(IDD_DIALOG_NETWORK),
	m_dwToNotify(0)
{
}


CNetworkPropPage::~CNetworkPropPage()
{
}


LRESULT CNetworkPropPage::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SetWindowText(GetTitle());

	m_pSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetNetworkSettings();

	ATLASSERT(m_pSettings.get());

	// Init proxy type combobox...
	m_cmbProxyType = GetDlgItem(IDC_COMBO_PROXY_TYPE);

	struct ProxyTypeInit
	{
		DWORD dwId;
		int nProxyType;
	};

	ProxyTypeInit proxyTypes[] = 
	{
		{ IDS_NETWORK_PROPPAGE_PROXY_TYPE_NONE, IUOLMessengerNetworkSettings::PT_PROXY_NONE },
		{ IDS_NETWORK_PROPPAGE_PROXY_TYPE_HTTP, IUOLMessengerNetworkSettings::PT_PROXY_HTTP },
		{ IDS_NETWORK_PROPPAGE_PROXY_TYPE_SOCKS4, IUOLMessengerNetworkSettings::PT_PROXY_SOCKS4 },
		{ IDS_NETWORK_PROPPAGE_PROXY_TYPE_SOCKS5, IUOLMessengerNetworkSettings::PT_PROXY_SOCKS5 }
	};

	CString strItem;
	int nNewItemIndex;
	int nCurSel = 0;
	for (int nInd = 0; nInd < sizeof(proxyTypes) / sizeof(proxyTypes[0]); nInd++)
	{
		strItem.LoadString(proxyTypes[nInd].dwId);
		nNewItemIndex = m_cmbProxyType.AddString(strItem);
		m_cmbProxyType.SetItemData(nNewItemIndex, proxyTypes[nInd].nProxyType);

		if (proxyTypes[nInd].nProxyType == m_pSettings->GetProxyType())
		{
			nCurSel = nNewItemIndex;
		}
	}

	m_cmbProxyType.SetCurSel(nCurSel);

	// Init edit controls text limit...
	struct TextInputLimitInit
	{
		DWORD dwId;
		UINT nLimit;
	};

	TextInputLimitInit limits[] = 
	{
		{ IDC_EDIT_PROXY_HOST, CNetworkPropPage::TIL_PROXY_HOST },
		{ IDC_EDIT_PROXY_PORT, CNetworkPropPage::TIL_PROXY_PORT },
		{ IDC_EDIT_PROXY_USERNAME, CNetworkPropPage::TIL_PROXY_USERNAME },
		{ IDC_EDIT_PROXY_PASSWORD, CNetworkPropPage::TIL_PROXY_PASSWORD }
	};

	for (int nInd = 0; nInd < sizeof(limits) / sizeof(limits[0]); nInd++)
	{
		CEdit wndEdit = GetDlgItem(limits[nInd].dwId);
		wndEdit.SetLimitText(limits[nInd].nLimit);
	}

	m_strProxyHost = m_pSettings->GetProxyHost();
	int nProxyPort = m_pSettings->GetProxyPort();
	if (nProxyPort >= 0)
	{
		m_strProxyPort.Format("%d", nProxyPort);
	}
	else
	{
		m_strProxyPort = _T("");
	}

	m_strProxyUsername = m_pSettings->GetProxyUsername();
	m_strProxyPassword = m_pSettings->GetProxyPassword();

	DoDataExchange(FALSE);

	OnSelChangeProxyTypeComboBoxImpl();

	m_dwToNotify = 0;

	return 0L;
}


void CNetworkPropPage::OnSelChangeProxyTypeComboBox(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	OnSelChangeProxyTypeComboBoxImpl();
}


void CNetworkPropPage::OnKillFocusProxyHost(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CString strOldHost = m_strProxyHost;

	DoDataExchange(TRUE, IDC_EDIT_PROXY_HOST);

	if (0 != strOldHost.CompareNoCase(m_strProxyHost))
	{
		//m_pSettings->SetProxyHost(m_strProxyHost);

		m_dwToNotify |= NL_PROXY_HOST;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_HOST);
	}
}


void CNetworkPropPage::OnKillFocusProxyPort(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	int nOldProxyPort = ConvertProxyPort(m_strProxyPort);

	DoDataExchange(TRUE, IDC_EDIT_PROXY_PORT);

	int nProxyPort = ConvertProxyPort(m_strProxyPort);

	if (nOldProxyPort != nProxyPort)
	{
		//m_pSettings->SetProxyPort(nProxyPort);

		m_dwToNotify |= NL_PROXY_PORT;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_PORT);
	}
}


void CNetworkPropPage::OnKillFocusProxyUsername(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CString strOldUsername = m_strProxyUsername;

	DoDataExchange(TRUE, IDC_EDIT_PROXY_USERNAME);

	if (0 != strOldUsername.CompareNoCase(m_strProxyUsername))
	{
		//m_pSettings->SetProxyUsername(m_strProxyUsername);

		m_dwToNotify |= NL_PROXY_USERNAME;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_USERNAME);
	}
}


void CNetworkPropPage::OnKillFocusProxyPassword(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CString strOldPassword = m_strProxyPassword;

	DoDataExchange(TRUE, IDC_EDIT_PROXY_PASSWORD);

	if (0 != strOldPassword.CompareNoCase(m_strProxyPassword))
	{
		//m_pSettings->SetProxyPassword(m_strProxyPassword);

		m_dwToNotify |= NL_PROXY_PASSWORD;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_PASSWORD);
	}
}


void CNetworkPropPage::OnSelChangeProxyTypeComboBoxImpl()
{
	int nCurSel = m_cmbProxyType.GetCurSel();

	if (nCurSel != -1)
	{
		int nType = m_cmbProxyType.GetItemData(nCurSel);

		if (nType != m_pSettings->GetProxyType())
		{
			//m_pSettings->SetProxyType(nType);

			m_dwToNotify |= NL_PROXY_TYPE;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
			//m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_TYPE);
		}

		BOOL bEnableEdit = (IUOLMessengerNetworkSettings::PT_PROXY_NONE != nType);

		DWORD pdwEdits[] = 
		{
			IDC_EDIT_PROXY_HOST,
			IDC_EDIT_PROXY_PORT,
			IDC_EDIT_PROXY_USERNAME,
			IDC_EDIT_PROXY_PASSWORD
		};

		int nCount = sizeof(pdwEdits) / sizeof(pdwEdits[0]);
		for (int nInd = 0; nInd < nCount; nInd++)
		{
			CWindow wndEdit = GetDlgItem(pdwEdits[nInd]);
			wndEdit.EnableWindow(bEnableEdit);
		}
	}
}


int CNetworkPropPage::ConvertProxyPort(const CString& strPort)
{
	int nProxyPort = -1;
	
	if (!strPort.IsEmpty())
	{
		nProxyPort = ::_ttoi(strPort);
	}

	return nProxyPort;
} 


LRESULT CNetworkPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


void CNetworkPropPage::OnApply()
{
	if ( NL_PROXY_TYPE & m_dwToNotify )
	{
		int nCurSel = m_cmbProxyType.GetCurSel();

		if (nCurSel != -1)
		{
			int nType = m_cmbProxyType.GetItemData(nCurSel);

			if (nType != m_pSettings->GetProxyType())
			{
				m_pSettings->SetProxyType(nType);
				m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_TYPE);
			}
		}		
	}

	if ( NL_PROXY_HOST & m_dwToNotify )
	{
		m_pSettings->SetProxyHost(m_strProxyHost);
		m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_HOST);
	}

	if ( NL_PROXY_PORT & m_dwToNotify )
	{
		int nProxyPort = ConvertProxyPort(m_strProxyPort);
		m_pSettings->SetProxyPort(nProxyPort);
		m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_PORT);
	}

	if ( NL_PROXY_USERNAME & m_dwToNotify )
	{
		m_pSettings->SetProxyUsername(m_strProxyUsername);		
		m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_USERNAME);
	}

	if ( NL_PROXY_PASSWORD & m_dwToNotify )
	{
		m_pSettings->SetProxyPassword(m_strProxyPassword);		
		m_pSettings->NotifyEventListeners(IUOLMessengerNetworkSettings::NT_PROXY_PASSWORD);
	}

	m_dwToNotify = 0;
}
