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
#include "ThemePropPage.h"

#include "../../controller/UIMApplication.h"


// CMessagesPropPage dialog

CThemePropPage::CThemePropPage() :
	CPreferencesPropPage(IDD_DIALOG_THEME)
{
	CString strTitle;
	strTitle.LoadString(IDS_DIALOG_THEME);

	SetTitle(strTitle);
}

CThemePropPage::~CThemePropPage()
{
}

LRESULT CThemePropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	m_pThemeManager = CUIMApplication::GetApplication()->GetThemeManager();
	m_pSettingManager = CUIMApplication::GetApplication()->GetSettingsManager();

	FillSkinCombo();
	FillThemeCombo();

	return 0L;
}

LRESULT CThemePropPage::OnComboThemeChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CString strSelectecTheme;

	m_cmbTheme.GetLBText(m_cmbTheme.GetCurSel(), strSelectecTheme);

	m_pSettingManager->GetGeneralSettings()->SetSelectedTheme(strSelectecTheme);

	return 0L;
}

LRESULT CThemePropPage::OnComboSkinChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CString strSelectecSkin;

	m_cmbSkin.GetLBText(m_cmbSkin.GetCurSel(), strSelectecSkin);

	m_pThemeManager->GetSkinSettings()->SetSelected(strSelectecSkin);

	return 0L;
}

void CThemePropPage::FillSkinCombo()
{
	CAtlList <CString> skinList;
	m_pThemeManager->GetSkinSettings()->GetSkinList(skinList);

	if (skinList.GetCount() > 0)
	{
		DoDataExchange(FALSE);
		m_cmbSkin.ResetContent();

		for(POSITION p = skinList.GetHeadPosition(); p != NULL; skinList.GetNext(p))
		{
			m_cmbSkin.AddString(skinList.GetAt(p));
		}

		if (m_pThemeManager->GetSkinSettings()->GetSelected() == "")
			m_cmbSkin.SetCurSel(0);
		else
			m_cmbSkin.SelectString(-1, m_pThemeManager->GetSkinSettings()->GetSelected());
	}
}

void CThemePropPage::FillThemeCombo()
{
	CAtlMap <CString, CString> themeList;
	m_pThemeManager->GetThemeList(themeList);

	if (themeList.GetCount() > 0)
	{
		DoDataExchange(FALSE);
		m_cmbTheme.ResetContent();

		for(POSITION p = themeList.GetStartPosition(); p != NULL; p)
		{
			m_cmbTheme.AddString(themeList.GetNextKey(p));
		}

		if (m_pSettingManager->GetGeneralSettings()->GetSelectedTheme() == "")
			m_cmbTheme.SetCurSel(0);
		else
			m_cmbTheme.SelectString(-1, m_pSettingManager->GetGeneralSettings()->GetSelectedTheme());
	}
}
