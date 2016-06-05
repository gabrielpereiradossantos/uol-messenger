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
#include <interfaces/IUOLMessengerStatusSettings.h>


// CStatusPropPage dialog

class CStatusPropPage : public CPreferencesPropPage
{
public:
	CStatusPropPage();
	virtual ~CStatusPropPage();

	virtual void OnApply();

protected:
	BEGIN_MSG_MAP(CStatusPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		//MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_AUTO_AWAY, OnCheckButtonClicked)
		COMMAND_HANDLER_EX(IDC_EDIT_INACTIVITY_MINUTES, EN_CHANGE, OnChangeInactivityMinutes)
		COMMAND_HANDLER_EX(IDC_EDIT_AWAY_MESSAGE, EN_CHANGE, OnChangeEditAwayMessage)
		//COMMAND_HANDLER_EX(IDC_SPIN_INACTIVITY_MINUTES, 
		NOTIFY_HANDLER_EX(IDC_SPIN_INACTIVITY_MINUTES, NM_RELEASEDCAPTURE, OnReleasedCaptureTime)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()

	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnChangeInactivityMinutes(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnChangeEditAwayMessage(UINT /*uCode*/, int nID, HWND hwndCtrl);
	LRESULT OnReleasedCaptureTime(LPNMHDR pNMHDR);

protected:
	enum TextInputLimit
	{
		TIL_INACTIVE_MINUTES = 2,
		TIL_AWAY_MESSAGE = 1000
	};

	IUOLMessengerStatusSettingsPtr m_pSettings;

	CButton		m_btnAutoAway;
	CEdit		m_editInactivityMinutes;
	CUpDownCtrl	m_updownTime;
	CEdit		m_wndAwayMessage;

};
MAKEAUTOPTR(CStatusPropPage);