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
 
// ContactPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "StatusPropPage.h"
#include "../../controller/UIMApplication.h"


// CStatusPropPage dialog

CStatusPropPage::CStatusPropPage() :
	CPreferencesPropPage(IDD_DIALOG_STATUS)
{
}

CStatusPropPage::~CStatusPropPage()
{
}


LRESULT CStatusPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	m_pSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetStatusSettings();

	// Init controls...
	m_btnAutoAway = GetDlgItem(IDC_CHECK_AUTO_AWAY);
	m_editInactivityMinutes = GetDlgItem(IDC_EDIT_INACTIVITY_MINUTES);
	m_updownTime = GetDlgItem(IDC_SPIN_INACTIVITY_MINUTES);
	m_wndAwayMessage = GetDlgItem(IDC_EDIT_AWAY_MESSAGE);

	// Copy settings to controls...
	m_btnAutoAway.SetCheck(m_pSettings->IsAutoAwayEnabled());

	UINT nTime = m_pSettings->GetTimeout();

	CString strTime;
	strTime.Format(_T("%u\n"),  nTime);

	//m_editInactivityMinutes.SetWindowText(strTime);
	m_editInactivityMinutes.LimitText(TIL_INACTIVE_MINUTES);

	// Replace '\n' with "\r\n"...
	LPTSTR lpszCurrentMessage = (LPTSTR) (LPCTSTR) m_pSettings->GetAwayMessage();
	CString strAwayMessage;
	LPTSTR lpszNewMessage = strAwayMessage.GetBuffer(2 * (::_tcslen(lpszCurrentMessage) + 1));

	LPTSTR lpszLastChar = NULL;
	while (*lpszCurrentMessage != '\0')
	{
		if ((*lpszCurrentMessage == '\n') &&
			(((lpszLastChar != NULL) &&	(*lpszLastChar != '\r')) ||
			 (lpszLastChar == NULL)))
		{
			*lpszNewMessage = '\r';
			lpszNewMessage++;
			*lpszNewMessage = '\n';
			lpszNewMessage++;
		}
		else
		{
			*lpszNewMessage = *lpszCurrentMessage;
			lpszNewMessage++;
		}

		lpszLastChar = lpszCurrentMessage;
		lpszCurrentMessage++;
	}
	*lpszNewMessage = '\0';
	strAwayMessage.ReleaseBuffer();
	if (strAwayMessage.GetLength() > TIL_AWAY_MESSAGE)
	{
		strAwayMessage = strAwayMessage.Left(TIL_AWAY_MESSAGE);
	}

	m_wndAwayMessage.SetWindowText(strAwayMessage);
	m_wndAwayMessage.LimitText(TIL_AWAY_MESSAGE);

	m_updownTime.SetRange32(1, 60);
	m_updownTime.SetBuddy(m_editInactivityMinutes);
	m_updownTime.SetPos(nTime);

	return 0L;
}


void CStatusPropPage::OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	if (hwndCtrl == m_btnAutoAway)
	{
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->EnableAutoAway(m_btnAutoAway.GetCheck() == BST_CHECKED ? TRUE : FALSE);
	}
}


void CStatusPropPage::OnChangeInactivityMinutes(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CString strTime;
	m_editInactivityMinutes.GetWindowText(strTime);

	UINT nMinutes = ::_tstoi(strTime);

	if ((nMinutes > 0) && (nMinutes != m_pSettings->GetTimeout()))
	{
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->SetTimeout(nMinutes);
	}
}

void CStatusPropPage::OnChangeEditAwayMessage(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	//CString strAwayMessage;
	//m_wndAwayMessage.GetWindowText(strAwayMessage);
	//m_pSettings->SetAwayMessage(strAwayMessage);
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);
}

LRESULT CStatusPropPage::OnReleasedCaptureTime(LPNMHDR pNMHDR)
{
	UINT nPos = m_updownTime.GetPos();

	return 0L;
}

LRESULT CStatusPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}

void CStatusPropPage::OnApply()
{
	CString str;
	m_wndAwayMessage.GetWindowText(str);
	m_pSettings->SetAwayMessage(str);

	m_editInactivityMinutes.GetWindowText(str);

	UINT nMinutes = ::_tstoi(str);

	if ((nMinutes > 0) && (nMinutes != m_pSettings->GetTimeout()))
	{
		m_pSettings->SetTimeout(nMinutes);
	}

	m_pSettings->EnableAutoAway(m_btnAutoAway.GetCheck() == BST_CHECKED ? TRUE : FALSE);
}
