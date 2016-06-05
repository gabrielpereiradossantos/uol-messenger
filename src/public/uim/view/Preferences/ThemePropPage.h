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
#include <interfaces/IUOLMessengerThemeManager.h>
#include <interfaces/IUOLMessengerSettingsManager.h>


// CMessagesPropPage dialog

class CThemePropPage : public CPreferencesPropPage,
					   public CWinDataExchange<CThemePropPage>

{
public:
	CThemePropPage();
	virtual ~CThemePropPage();

	void FillSkinCombo();
	void FillThemeCombo();

protected:
	BEGIN_MSG_MAP(CThemePropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER(IDC_COMBO_THEME, CBN_SELCHANGE, OnComboThemeChange)
		COMMAND_HANDLER(IDC_COMBO_SKIN, CBN_SELCHANGE, OnComboSkinChange)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CProfileDlg)
		DDX_CONTROL_HANDLE(IDC_COMBO_THEME, m_cmbTheme)
		DDX_CONTROL_HANDLE(IDC_COMBO_SKIN, m_cmbSkin)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnComboThemeChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnComboSkinChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
	IUOLMessengerThemeManagerPtr	m_pThemeManager;
	IUOLMessengerSettingsManagerPtr	m_pSettingManager;

	CComboBox		m_cmbTheme;
	CComboBox		m_cmbSkin;
};
