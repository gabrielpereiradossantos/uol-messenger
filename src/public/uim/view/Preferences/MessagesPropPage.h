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

#include <interfaces/IUOLMessengerSettingsManager.h>
#include <interfaces/IUOLMessengerMessageSettings.h>


// CMessagesPropPage dialog

class CMessagesPropPage : public CPreferencesPropPage
{
public:
	CMessagesPropPage();
	virtual ~CMessagesPropPage();

	virtual void OnApply();

protected:
	BEGIN_MSG_MAP(CMessagesPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_OPEN_MESSAGE_WINDOW, OnCheckButtonClicked);
		COMMAND_ID_HANDLER_EX(IDC_CHECK_TRAY_NOTIFY, OnCheckButtonClicked);
		COMMAND_ID_HANDLER_EX(IDC_CHECK_LOG_MESSAGES, OnCheckButtonClicked);
		COMMAND_ID_HANDLER_EX(IDC_CHECK_NEW_MSGWINDOW, OnCheckButtonClicked);
		COMMAND_ID_HANDLER_EX(IDC_CHECK_UNREAD_MESSAGES_ALERT, OnCheckButtonClicked);
		COMMAND_ID_HANDLER_EX(IDC_CHECK_ALLOW_NUDGES, OnCheckButtonClicked)	
		COMMAND_ID_HANDLER_EX(IDC_CHECK_ACCEPT_CUSTOM_EMOTICONS, OnCheckButtonClicked)	
		COMMAND_ID_HANDLER_EX(IDC_CHECK_SHOW_RECENT_HISTORY, OnCheckButtonClicked)	
		COMMAND_HANDLER_EX(IDC_BUTTON_FILE_TRANSFER_FOLDER, BN_CLICKED, OnFileTransferFolderChange)
		COMMAND_RANGE_HANDLER_EX(IDC_RADIO_FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK, IDC_RADIO_FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT, OnRadioFileTransferClicked)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()

	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);

	LRESULT OnFileTransferFolderChange(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	void OnRadioFileTransferClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);

protected:
	IUOLMessengerSettingsManagerPtr		m_pSettingManager;
	IUOLMessengerMessageSettings3Ptr	m_pSettings;

	CButton		m_btnLogMessages;
	CButton		m_btnTrayNotification;
	CButton		m_btnWindowNotification;
	CButton		m_btnNewMsgWindow;
	CButton		m_btnUnreadMessagesAlert;
	CButton		m_btnAllowNudges;
	CButton		m_btnAcceptCustomEmoticons;
	CButton		m_btnShowRecentHistory;

	DWORD		m_nFileTransferSaveFolderMode;

	enum NotifyListener
	{
		NL_LOG_MESSAGES						= 1,
		NL_TRAY_NOTIFICATION				= 2,
		NL_NEW_MSG_WINDOW_NOTIFICATION		= 4,
		NL_SHOW_UNREAD_MESSAGES_ALERT		= 8,
		NL_ALLOW_NUDGES						= 16,
		NL_ACCEPT_CUSTOM_EMOTICONS			= 32,
		NL_FILE_TRANSFER_SAVE_FOLDER_MODE	= 64,
		NL_SHOW_RECENT_HISTORY				= 128
	};

	DWORD m_dwToNotify;

};
MAKEAUTOPTR(CMessagesPropPage);