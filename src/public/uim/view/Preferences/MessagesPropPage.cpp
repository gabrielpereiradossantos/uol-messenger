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
#include "MessagesPropPage.h"
#include "../BrowseFolder.h"

#include "../../controller/UIMApplication.h"


// CMessagesPropPage dialog

CMessagesPropPage::CMessagesPropPage() :
	CPreferencesPropPage(IDD_DIALOG_MESSAGES),
	m_dwToNotify(0),
	m_nFileTransferSaveFolderMode(IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK)
{	
	
}

CMessagesPropPage::~CMessagesPropPage()
{
}


LRESULT CMessagesPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	m_pSettingManager = CUIMApplication::GetApplication()->GetSettingsManager();	
	m_pSettings = ap_static_cast<IUOLMessengerMessageSettings3Ptr>(m_pSettingManager->GetMessageSettings());

	m_btnLogMessages = GetDlgItem(IDC_CHECK_LOG_MESSAGES);
	m_btnTrayNotification = GetDlgItem(IDC_CHECK_TRAY_NOTIFY);
	m_btnNewMsgWindow = GetDlgItem(IDC_CHECK_NEW_MSGWINDOW);
	m_btnUnreadMessagesAlert = GetDlgItem(IDC_CHECK_UNREAD_MESSAGES_ALERT);
	m_btnAllowNudges = GetDlgItem(IDC_CHK_ALLOW_NUDGES);
	m_btnAcceptCustomEmoticons = GetDlgItem(IDC_CHECK_ACCEPT_CUSTOM_EMOTICONS);
	m_btnShowRecentHistory = GetDlgItem(IDC_CHECK_SHOW_RECENT_HISTORY);

	m_btnLogMessages.SetCheck(m_pSettings->IsLogMessagesEnabled());
	m_btnTrayNotification.SetCheck(m_pSettings->IsTrayNotificationEnabled());
	m_btnNewMsgWindow.SetCheck(m_pSettings->IsOnNewMsgWindowEnabled());
	m_btnUnreadMessagesAlert.SetCheck(m_pSettings->IsShowUnreadMessagesAlertEnabled());
	m_btnAllowNudges.SetCheck(m_pSettings->GetAllowNudges());
	m_btnAcceptCustomEmoticons.SetCheck(m_pSettings->GetAcceptCustomEmoticons());
	m_btnShowRecentHistory.SetCheck(m_pSettings->IsShowRecentHistoryEnabled());

	if (!m_pSettings->IsLogMessagesEnabled())
	{
		m_btnShowRecentHistory.EnableWindow(FALSE);
	}
	
	m_dwToNotify = 0;

	// Get last received file folder
	CString strLastFolder;
	IUOLMessengerGeneralSettingsPtr pGeneralSettings;
	pGeneralSettings = m_pSettingManager->GetGeneralSettings();
	if (pGeneralSettings)
	{
		strLastFolder = pGeneralSettings->GetLastReceiveFileFolder();
	}
	
	// Get file transfer save folder mode
	m_nFileTransferSaveFolderMode = m_pSettings->GetFileTransferSaveFolderMode();
	
	CButton btnFileTransfer;
	CString strFolder;
	switch (m_nFileTransferSaveFolderMode)
	{
	case IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK:
		btnFileTransfer = GetDlgItem(IDC_RADIO_FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK);
		btnFileTransfer.SetCheck(TRUE);
		GetDlgItem(IDC_BUTTON_FILE_TRANSFER_FOLDER).EnableWindow(FALSE);
		break;

	case IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT:
		btnFileTransfer = GetDlgItem(IDC_RADIO_FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT);
		btnFileTransfer.SetCheck(TRUE);	

		strFolder = m_pSettings->GetDefaultFileTransferFolder();
		if (strFolder.IsEmpty())
		{
			strFolder = strLastFolder;
		}

		GetDlgItem(IDC_EDIT_FILE_TRANSFER_FOLDER).SetWindowText(strFolder);
		GetDlgItem(IDC_BUTTON_FILE_TRANSFER_FOLDER).EnableWindow(TRUE);
		break;
	}

	return 0L;
}

void CMessagesPropPage::OnCheckButtonClicked(UINT uCode, int nID, HWND hwndCtrl)
{
	if (hwndCtrl == m_btnNewMsgWindow)
	{
		m_dwToNotify |= NL_NEW_MSG_WINDOW_NOTIFICATION;
	}
	else if (hwndCtrl == m_btnLogMessages)
	{
		m_dwToNotify |= NL_LOG_MESSAGES;
		
		if (m_btnLogMessages.GetCheck())
		{
			m_btnShowRecentHistory.EnableWindow(TRUE);
		}
		else
		{
			m_btnShowRecentHistory.SetCheck(FALSE);
			m_btnShowRecentHistory.EnableWindow(FALSE);

			m_dwToNotify |= NL_SHOW_RECENT_HISTORY;
		}
	}
	else if (hwndCtrl == m_btnTrayNotification)
	{
		m_dwToNotify |= NL_TRAY_NOTIFICATION;
	}
	else if (hwndCtrl == m_btnUnreadMessagesAlert)
	{
		m_dwToNotify |= NL_SHOW_UNREAD_MESSAGES_ALERT;
	}
	else if (hwndCtrl == m_btnAllowNudges)
	{
		m_dwToNotify |= NL_ALLOW_NUDGES;
	}
	else if (hwndCtrl == m_btnAcceptCustomEmoticons)
	{
		m_dwToNotify |= NL_ACCEPT_CUSTOM_EMOTICONS;
	}
	else if (hwndCtrl == m_btnShowRecentHistory)
	{
		m_dwToNotify |= NL_SHOW_RECENT_HISTORY;
	}

	if ( 0!= m_dwToNotify )
	{
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
	}
}


LRESULT CMessagesPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


void CMessagesPropPage::OnApply()
{
	if ( NL_NEW_MSG_WINDOW_NOTIFICATION & m_dwToNotify )
	{
		m_pSettings->EnableOnNewMsgWindow(m_btnNewMsgWindow.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_NEW_MSG_WINDOW_NOTIFICATION);
	}
	
	if ( NL_LOG_MESSAGES & m_dwToNotify )
	{
		m_pSettings->EnableLogMessages(m_btnLogMessages.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_LOG_MESSAGES);
	}
	
	if ( NL_TRAY_NOTIFICATION & m_dwToNotify )
	{
		m_pSettings->EnableTrayNotification(m_btnTrayNotification.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_TRAY_NOTIFICATION);
	}

	if ( NL_SHOW_UNREAD_MESSAGES_ALERT & m_dwToNotify )
	{
		m_pSettings->EnableShowUnreadMessagesAlert(m_btnUnreadMessagesAlert.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_SHOW_UNREAD_MESSAGES_ALERT);
	}

	if ( NL_ALLOW_NUDGES & m_dwToNotify )
	{
		m_pSettings->SetAllowNudges(m_btnAllowNudges.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_ALLOW_NUDGES);
	}
	
	if ( NL_ACCEPT_CUSTOM_EMOTICONS & m_dwToNotify )
	{
		m_pSettings->SetAcceptCustomEmoticons(m_btnAcceptCustomEmoticons.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_ACCEPT_CUSTOM_EMOTICONS);
	}

	if (NL_FILE_TRANSFER_SAVE_FOLDER_MODE & m_dwToNotify)
	{
		m_pSettings->SetFileTransferSaveFolderMode(m_nFileTransferSaveFolderMode);

		if (m_nFileTransferSaveFolderMode == IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT)
		{
			CString strFolder;
			GetDlgItem(IDC_EDIT_FILE_TRANSFER_FOLDER).GetWindowText(strFolder);
			m_pSettings->SetDefaultFileTransferFolder(strFolder);
		}
		
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_FILE_TRANSFER_SAVE_FOLDER_MODE);
	}

	if ( NL_SHOW_RECENT_HISTORY & m_dwToNotify )
	{
		m_pSettings->EnableShowRecentHistory(m_btnShowRecentHistory.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerMessageSettings::MSG_SHOW_RECENT_HISTORY);
	}
	
	m_dwToNotify = 0;
}


void CMessagesPropPage::OnRadioFileTransferClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	// Get last received file folder
	CString strLastFolder;
	IUOLMessengerGeneralSettingsPtr pGeneralSettings;
	pGeneralSettings = m_pSettingManager->GetGeneralSettings();
	if (pGeneralSettings)
	{
		strLastFolder = pGeneralSettings->GetLastReceiveFileFolder();
	}
	
	CString strFolder;
	CButton btnFileTransfer;
	btnFileTransfer = hwndCtrl;

	if ( 1 == btnFileTransfer.GetCheck() )
	{
		switch (nID)
		{
		case IDC_RADIO_FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK:
			m_nFileTransferSaveFolderMode = IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK;
			GetDlgItem(IDC_EDIT_FILE_TRANSFER_FOLDER).SetWindowText("");
			GetDlgItem(IDC_BUTTON_FILE_TRANSFER_FOLDER).EnableWindow(FALSE);
			break;

		case IDC_RADIO_FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT:
			m_nFileTransferSaveFolderMode = IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT;
		
			strFolder = m_pSettings->GetDefaultFileTransferFolder();
			if (strFolder.IsEmpty())
			{
				strFolder = strLastFolder;
			}

			GetDlgItem(IDC_EDIT_FILE_TRANSFER_FOLDER).SetWindowText(strFolder);
			GetDlgItem(IDC_BUTTON_FILE_TRANSFER_FOLDER).EnableWindow(TRUE);
			break;
		}

		m_dwToNotify |= NL_FILE_TRANSFER_SAVE_FOLDER_MODE;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
	}
}


LRESULT CMessagesPropPage::OnFileTransferFolderChange(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	CBrowseFolder browseFolder(m_hWnd, "");

	if (IDOK == browseFolder.DoModal())
	{
		CString strFolder = browseFolder.GetFolderPath();
		GetDlgItem(IDC_EDIT_FILE_TRANSFER_FOLDER).SetWindowText(strFolder);
	}

	return 0L;
}