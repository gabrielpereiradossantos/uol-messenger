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


#include "StdAfx.h"
#include ".\UOLFoneConfigDlg.h"
#include "FacadeWrapper.h"
#include <winuser.h>


CUOLFoneConfigDlg::CUOLFoneConfigDlg(const CString& strTabName, const CString& strPluginInternalName,
									 const IUOLMessengerImagePtr& pImage) :
	m_strTabName(strTabName),
	m_strPluginInternalName(strPluginInternalName),
	m_pImage(pImage),
	m_nTabIndex(-1)
{
	CreateColors();
}


CUOLFoneConfigDlg::~CUOLFoneConfigDlg()
{
	if (IsWindow())
	{
		DestroyWindow();
	}
}


LRESULT CUOLFoneConfigDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	IUOLMessengerFacade* pUOLMessengerFacade = CFacadeWrapper::GetFacadeInstance();
	
	if (pUOLMessengerFacade)
	{
		m_pUOLMessengerFacade = pUOLMessengerFacade;
	}    
    
	m_callAccountCombo = GetDlgItem(IDC_CMB_CONTAS);
	m_lineInDeviceCombo = GetDlgItem(IDC_CMB_ENTRADA_AUDIO);
	m_lineOutDeviceCombo = GetDlgItem(IDC_CMB_SAIDA_AUDIO);
	m_forceTunnelButton = GetDlgItem(IDC_CHECK_FORCE_TUNNEL);
	m_editPurgeInterval = GetDlgItem(IDC_EDIT_PURGE_INTERVAL);
	m_updownPurgeInterval = GetDlgItem(IDC_SPIN_PURGE_INTERVAL);
	m_registerOnCallButton = GetDlgItem(IDC_CHECK_REGISTER_ON_CALL);
	
	m_registerOnCallTooltipButton.SubclassWindow(m_registerOnCallButton);

	CString strRegisterOnCallHint;
	strRegisterOnCallHint.LoadString(IDS_REGISTER_ON_CALL_HINT);

	m_registerOnCallTooltipButton.SetToolTipText(strRegisterOnCallHint);
	m_registerOnCallTooltipButton.SetTooltipMaxWidth(500);
	
	DoDataExchange(FALSE);
	
	CString strTitle;
	strTitle.LoadString(IDS_PLUGIN_NAME);
	this->SetWindowText(strTitle);

	// Center the dialog on the screen.
	CenterWindow();

	// Load config and show data.
	m_pUOLFoneController = CUOLFoneController::GetInstance();
	m_pUOLFonePluginConfig = CUOLFonePluginConfig::GetInstance();

	CString strInterval;
	strInterval.Format("%d", m_pUOLFonePluginConfig->GetPurgeInterval());
	m_editPurgeInterval.SetWindowText(strInterval);
	
	m_updownPurgeInterval.SetRange32(1, 30);
	m_updownPurgeInterval.SetBuddy(m_editPurgeInterval);
	
	m_dwToNotify = 0;

	return TRUE;
}


LRESULT CUOLFoneConfigDlg::OnClearHistory(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString strMessage;
	strMessage.LoadString(IDS_CLEAR_HISTORY_CONFIRM_MESSAGE);

    CString strCaption;
	strCaption.LoadString(IDS_PLUGIN_NAME);
    
    UINT ret = ap_static_cast<IUOLMessengerUIManager4Ptr>(m_pUOLMessengerFacade->GetUIManager())->ShowMessageBox(m_hWnd, strMessage, strCaption, MB_YESNO | MB_ICONQUESTION);
    if (ret == IDYES)
	{
		m_pUOLFonePluginConfig->ClearCallHistory();
	}

	return 0L;
}


LRESULT CUOLFoneConfigDlg::OnComboDropDown(int /*HiWordWParam*/, int /*LowWordWParam*/, HWND lParam, BOOL& /*bHandled*/)
{
	RECT windowRect = {0};

	COMBOBOXINFO comboBoxInfo;
	comboBoxInfo.cbSize = sizeof(COMBOBOXINFO);

	if (::GetComboBoxInfo(lParam, &comboBoxInfo))
	{
		if (::GetWindowRect(lParam, &windowRect))
		{
			if (::SetWindowPos(comboBoxInfo.hwndList, 
				0, 
				0, 0, windowRect.right - windowRect.left, 20, 
				SWP_NOMOVE | SWP_NOZORDER))
			{
				int blah = 0;
			}
		}
	}

	return 0L;
}


LRESULT CUOLFoneConfigDlg::OnDestroy()
{
	int nAccount = m_callAccountCombo.GetCount();
	AccountHelper* pAH;

	for (int i = 0; i < nAccount; i++)
	{
		pAH = (AccountHelper*) m_callAccountCombo.GetItemData(i);

		if (pAH)
		{
			delete pAH;
		}
	}

	return 0L;
}


void CUOLFoneConfigDlg::FillAccountListCombo()
{
	ATLASSERT(m_pUOLFoneController);

	m_callAccountCombo.ResetContent();

	CUOLFoneAccountManagerPtr pAccountManager = m_pUOLFoneController->GetUOLFoneAccountManager();

	CAtlList<IUOLMessengerAccountPtr> listAccounts;

	pAccountManager->GetIMAccountList(listAccounts);

	CString strUserId;
	AccountHelper* pAH = NULL;
	IUOLMessengerAccountPtr pAccount = NULL;
	int nIndex = -1;

	for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
	{
		pAccount = ap_dynamic_cast<IUOLMessengerAccountPtr>(listAccounts.GetNext(pos));

		if (pAccount)
		{
			pAH = new AccountHelper;
			pAH->pAccount = pAccount;

			strUserId = pAccount->GetUserId();
			nIndex = m_callAccountCombo.AddString(strUserId);
			m_callAccountCombo.SetItemData(nIndex, (DWORD_PTR)pAH);
		}
	}

	m_strCallAccount = m_pUOLFonePluginConfig->GetCallAccount();

	if (CB_ERR == m_callAccountCombo.SelectString(-1, m_strCallAccount))
	{
		if ((m_callAccountCombo.GetCount() > 0) &&
			(CB_ERR != m_callAccountCombo.SetCurSel(0)))
		{
			int nTextLen = m_callAccountCombo.GetLBTextLen(0);
			m_callAccountCombo.GetLBText(0, m_strCallAccount.GetBuffer(nTextLen));
			m_strCallAccount.ReleaseBuffer();

			m_pUOLFonePluginConfig->SetCallAccount(m_strCallAccount);
		}
	}
}

void CUOLFoneConfigDlg::FillInputDevicesCombo()
{
	ATLASSERT(m_pUOLFoneController);

	m_lineInDeviceCombo.ResetContent();

	CAtlList<CString> listDevices;

	m_pUOLFoneController->GetInputDevices(listDevices);

	for (POSITION pos = listDevices.GetHeadPosition(); pos;)
	{
		CString strDevice = listDevices.GetNext(pos);

		m_lineInDeviceCombo.AddString(strDevice);
	}

	// Select config device or first valid device...
	m_strLineInDevice = m_pUOLFonePluginConfig->GetLineInDevice();

	if (CB_ERR == m_lineInDeviceCombo.SelectString(-1, m_strLineInDevice))
	{
		if ((m_lineInDeviceCombo.GetCount() > 0) &&
			(CB_ERR != m_lineInDeviceCombo.SetCurSel(0)))
		{
			int nTextLen = m_lineInDeviceCombo.GetLBTextLen(0);
			m_lineInDeviceCombo.GetLBText(0, m_strLineInDevice.GetBuffer(nTextLen));
			m_strLineInDevice.ReleaseBuffer();
		}
	}
}

void CUOLFoneConfigDlg::FillOutputDevicesCombo()
{
	ATLASSERT(m_pUOLFoneController);

	m_lineOutDeviceCombo.ResetContent();

	CAtlList<CString> listDevices;

	m_pUOLFoneController->GetOutputDevices(listDevices);

	for (POSITION pos = listDevices.GetHeadPosition(); pos;)
	{
		CString strDevice = listDevices.GetNext(pos);

		m_lineOutDeviceCombo.AddString(strDevice);
	}

	// Select config device or first valid device...
	m_strLineOutDevice = m_pUOLFonePluginConfig->GetLineOutDevice();

	if (CB_ERR == m_lineOutDeviceCombo.SelectString(-1, m_strLineOutDevice))
	{
		if ((m_lineOutDeviceCombo.GetCount() > 0) &&
			(CB_ERR != m_lineOutDeviceCombo.SetCurSel(0)))
		{
			int nTextLen = m_lineOutDeviceCombo.GetLBTextLen(0);
			m_lineOutDeviceCombo.GetLBText(0, m_strLineOutDevice.GetBuffer(nTextLen));
			m_strLineOutDevice.ReleaseBuffer();
		}
	}
}


void CUOLFoneConfigDlg::OnApply()
{
	if ( 0 != m_dwToNotify )
	{
		if ( m_pUOLFoneController->IsCallInProgress() )
		{
			CString strCaption;
			CString strMessage;

			strCaption.LoadString(IDS_PLUGIN_NAME);
			strMessage.LoadString(IDS_ERROR_APPLY_CONFIG);

			ap_static_cast<IUOLMessengerUIManager4Ptr>(m_pUOLMessengerFacade->GetUIManager())->ShowMessageBox(m_hWnd, strMessage, strCaption, MB_OK | MB_ICONERROR);
		}
		else
		{
			DoDataExchange(TRUE);

			if(	NL_CALL_ACCOUNT	& m_dwToNotify )
			{
				m_pUOLFonePluginConfig->SetCallAccount(m_strCallAccount);
				
				int nIndex = m_callAccountCombo.FindStringExact(0, m_strCallAccount);
				if ( nIndex > -1 )
				{
					AccountHelper* pAH = NULL;
					pAH = (AccountHelper*)m_callAccountCombo.GetItemData(nIndex);
					ATLASSERT( pAH->pAccount );

					m_pUOLFoneController->GetUOLFoneAccountManager()->SetCurrentAccount(pAH->pAccount, TRUE);
				}

			}

			if(	NL_LINE_IN_DEVICE	& m_dwToNotify )
			{
				m_pUOLFonePluginConfig->SetLineInDevice(m_strLineInDevice);
				m_pUOLFoneController->SetCurrentInputDevice(m_strLineInDevice);
			}

			if(	NL_LINE_OUT_DEVICE	& m_dwToNotify )
			{
				m_pUOLFonePluginConfig->SetLineOutDevice(m_strLineOutDevice);
				m_pUOLFoneController->SetCurrentOutputDevice(m_strLineOutDevice);
			}

			if(	NL_FORCE_TUNNEL	& m_dwToNotify )
			{
				m_pUOLFonePluginConfig->SetForceTunnel(m_bForceTunnel);
				m_pUOLFoneController->UpdateNetworkConfig();
			}

			if(	NL_PURGE_INTERVAL & m_dwToNotify )
			{
				CString strInterval;
				m_editPurgeInterval.GetWindowText(strInterval);
				
				m_pUOLFonePluginConfig->SetPurgeInterval(::_tstoi(strInterval));
			}

			if(	NL_REGISTER_ON_CALL	& m_dwToNotify )
			{
				m_pUOLFonePluginConfig->SetRegisterOnlyOnCall(m_bRegisterOnCall);
				
				if ( (m_bRegisterOnCall) && (m_pUOLFoneController->IsConnected()) && 
						(!m_pUOLFoneController->IsCallInProgress()) )
				{
					m_pUOLFoneController->Disconnect();
				}
				else if ( (!m_bRegisterOnCall) && (!m_pUOLFoneController->IsConnected()) && 
						(m_pUOLFoneController->IsIMAccountConnected()) )
				{
					m_pUOLFoneController->Connect();
				}
			}

			m_pUOLFonePluginConfig->SaveUOLFoneConfig();
		}
	}

	m_dwToNotify = 0;
}


HWND CUOLFoneConfigDlg::GetHWND()
{
	return m_hWnd;
}


HWND CUOLFoneConfigDlg::Create(HWND hWndParent)
{
	HWND hWnd = __super::Create(hWndParent, 0);

	if (IsWindow())
	{
		DWORD dwRemove = DS_FIXEDSYS | WS_POPUP | WS_CLIPSIBLINGS | WS_SYSMENU;
		DWORD dwAdd = DS_CONTROL | WS_CHILDWINDOW;

		ModifyStyle(dwRemove, dwAdd);
	}

	return hWnd;
}


LRESULT CUOLFoneConfigDlg::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	HBRUSH hBrush = NULL;

	if (WM_CTLCOLOREDIT == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushEdit);
		hBrush = m_brushEdit.m_hBrush;
		bHandled = FALSE;
	}
	else if (WM_CTLCOLORLISTBOX == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushListBox);
		hBrush = m_brushListBox.m_hBrush;
		bHandled = FALSE;
	}
	else if (WM_CTLCOLORBTN == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushButton);
		hBrush = m_brushButton.m_hBrush;
	}
	else if (WM_CTLCOLORDLG == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushDialog);
		hBrush = m_brushDialog.m_hBrush;
	}
	else if (WM_CTLCOLORSCROLLBAR == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushScrollBar);
		hBrush = m_brushScrollBar.m_hBrush;
	}
	else if (WM_CTLCOLORSTATIC == nMsg)
	{
		::SetBkColor((HDC) wParam, m_clrbrushStatic);
		hBrush = m_brushStatic.m_hBrush;
	}

	return (LRESULT) hBrush;
}


LRESULT CUOLFoneConfigDlg::OnCbnSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	switch (wID)
	{
	case IDC_CMB_CONTAS:
		m_dwToNotify |= NL_CALL_ACCOUNT;
		break;

	case IDC_CMB_ENTRADA_AUDIO:
		m_dwToNotify |= NL_LINE_IN_DEVICE;
		break;

	case IDC_CMB_SAIDA_AUDIO:
		m_dwToNotify |= NL_LINE_OUT_DEVICE;
		break;
	}
	
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);

	return 0;
}


void CUOLFoneConfigDlg::CreateColors()
{
	m_clrbrushEdit.SetColor(_T("#fefefe"));
	m_brushEdit.CreateSolidBrush(m_clrbrushEdit);

	m_clrbrushListBox.SetColor(_T("#fefefe"));
	m_brushListBox.CreateSolidBrush(m_clrbrushListBox);

	m_clrbrushDialog.SetColor(_T("#fefefe"));
	m_brushDialog.CreateSolidBrush(m_clrbrushDialog);

	m_clrbrushButton.SetColor(_T("#fefefe"));
	m_brushButton.CreateSolidBrush(m_clrbrushButton);

	m_clrbrushStatic.SetColor(_T("#fefefe"));
	m_brushStatic.CreateSolidBrush(m_clrbrushStatic);

	m_clrbrushScrollBar.SetColor(_T("#fefefe"));
	m_brushScrollBar.CreateSolidBrush(m_clrbrushScrollBar);
}


LRESULT CUOLFoneConfigDlg::OnSetActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	FillAccountListCombo();
	FillInputDevicesCombo();
	FillOutputDevicesCombo();

	m_forceTunnelButton.SetCheck(m_pUOLFonePluginConfig->GetForceTunnel());
	m_registerOnCallButton.SetCheck(m_pUOLFonePluginConfig->RegisterOnlyOnCall());
	
	//BOOL bIsCallInProgress = m_pUOLFoneController->IsCallInProgress();

	//m_callAccountCombo.EnableWindow  ( ! bIsCallInProgress );
	//m_lineInDeviceCombo.EnableWindow ( ! bIsCallInProgress );
	//m_lineOutDeviceCombo.EnableWindow( ! bIsCallInProgress );

	return 0L;
}


void CUOLFoneConfigDlg::OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	if (m_forceTunnelButton.m_hWnd == hwndCtrl)
	{
		m_dwToNotify |= NL_FORCE_TUNNEL;
	}
	else if (m_registerOnCallButton.m_hWnd == hwndCtrl)
	{
		m_dwToNotify |= NL_REGISTER_ON_CALL;
	}
	
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);
}


void CUOLFoneConfigDlg::OnChangePurgeInterval(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	if (m_editPurgeInterval.m_hWnd == hwndCtrl)
	{
		CString strInterval;
		m_editPurgeInterval.GetWindowText(strInterval);
		
		UINT nDays = ::_tstoi(strInterval);
		
		if ((nDays >= m_pUOLFonePluginConfig->GetMinimmumPurgeInterval()) && 
				(nDays != m_pUOLFonePluginConfig->GetPurgeInterval()))
		{
			m_dwToNotify |= NL_PURGE_INTERVAL;
			
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		}
		/*
		else if (nDays < m_pUOLFonePluginConfig->GetMinimmumPurgeInterval())
		{
			strInterval.Format("%d", m_pUOLFonePluginConfig->GetMinimmumPurgeInterval());
			m_editPurgeInterval.SetWindowText(strInterval);
		}
		*/
	}
}

CString CUOLFoneConfigDlg::GetTabName() const
{
	return m_strTabName;
}


CString CUOLFoneConfigDlg::GetPluginInternalName() const
{
	return m_strPluginInternalName;
}


void CUOLFoneConfigDlg::GetMinSize(CSize& size)
{

}


HWND CUOLFoneConfigDlg::CreateTab(HWND hWndParent, const CRect& rectTab, DWORD dwWindowStyle)
{
	return NULL;
}


void CUOLFoneConfigDlg::DestroyTab(HWND hWndTab)
{
}


HWND CUOLFoneConfigDlg::GetTabHWND()
{
	return m_hWnd;
}


IUOLMessengerImagePtr CUOLFoneConfigDlg::GetTabImage()
{
	return m_pImage;
}


void CUOLFoneConfigDlg::SetTabButtonIndex(int nTabIndex)
{
	m_nTabIndex = nTabIndex;
}


int  CUOLFoneConfigDlg::GetTabButtonIndex()
{
	return m_nTabIndex;
}

void CUOLFoneConfigDlg::Show(int nCmdShow)
{
	if (IsWindow())
	{
		ShowWindow(nCmdShow);
	}
}