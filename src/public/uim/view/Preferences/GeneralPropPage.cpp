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
#include "GeneralPropPage.h"
#include "MinDependencySpecialFolder.h"
#include "../UIMFileDialog.h"

#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include ".\generalproppage.h"
#include "../PasswordChangeDlg.h"
#include "../skin/ElementBuilder.h"
#include "../Skin/MainDialogElement.h"
#include <commands/OpenBrowserCommand.h>




// CMessagesPropPage dialog

CGeneralPropPage::CGeneralPropPage() :
	CPreferencesPropPage(IDD_DIALOG_GENERAL),
	m_dwToNotify(0)
{	
}

CGeneralPropPage::~CGeneralPropPage()
{
}

LRESULT CGeneralPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	DoDataExchange(FALSE);

	CString strDefaultProfile, strCurrentProfile;
	SetWindowText(GetTitle());

	m_pSettingManager = CUIMApplication::GetApplication()->GetSettingsManager();

	m_btnOpenIE = GetDlgItem(IDC_CHECK_OPEN_IE);

	m_btnOpenIE.SetCheck(m_pSettingManager->GetGeneralSettings()->IsOpenBrowserEnabled());
	m_btnAutoStartup.SetCheck(m_pSettingManager->GetGeneralSettings()->IsAutoStartupEnabled());

	strDefaultProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetDefaultProfile();
	strCurrentProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();
   
	if (strDefaultProfile.Compare(strCurrentProfile) == 0)
	{
        m_btnDefautProfile.SetCheck(TRUE);
	}
	else
	{
        m_btnDefautProfile.SetCheck(FALSE);
	}

	m_dwToNotify = 0;

	UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	if (pStringLoader)
	{
		CString strAutoStartupText = pStringLoader->GetValue(_T("uol.resource.IDS_CHECK_AUTO_STARTUP_FORMAT"));
		ATLASSERT(FALSE == strAutoStartupText.IsEmpty());

		m_btnAutoStartup.SetWindowText(strAutoStartupText);
	}
	
	return 0L;
}


LRESULT CGeneralPropPage::OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	switch (nID)
	{
        case IDC_CHECK_AUTO_STARTUP:
            //m_pSettingManager->GetGeneralSettings()->EnableAutoStartup(m_btnAutoStartup.GetCheck() ? TRUE : FALSE);
			m_dwToNotify |= NL_AUTO_STARTUP;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
			break;

        case IDC_CHECK_OPEN_IE:
            //m_pSettingManager->GetGeneralSettings()->EnableOpenBrowser(m_btnOpenIE.GetCheck() ? TRUE : FALSE);
			m_dwToNotify |= NL_OPEN_IE;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
			break;

        case IDC_CHECK_DEFAULT_PROFILE:	
			if(m_btnDefautProfile.GetCheck())
			{
				m_strProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();
			}
			else
			{
				m_strProfile.Empty();
			}
			//CUIMApplication::GetApplication()->GetProfileManager()->SetDefaultProfile(m_strProfile);
			m_dwToNotify |= NL_DEFAULT_PROFILE;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);

			break;

		default:
			break;
	}

	return 0L;
}


LRESULT CGeneralPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


void CGeneralPropPage::OnApply()
{
	if(	NL_AUTO_STARTUP	& m_dwToNotify )
	{
		m_pSettingManager->GetGeneralSettings()->EnableAutoStartup(m_btnAutoStartup.GetCheck() ? TRUE : FALSE);
	}

	if(	NL_OPEN_IE & m_dwToNotify )
	{
		m_pSettingManager->GetGeneralSettings()->EnableOpenBrowser(m_btnOpenIE.GetCheck() ? TRUE : FALSE);
	}

	if(	NL_DEFAULT_PROFILE & m_dwToNotify )
	{
		CUIMApplication::GetApplication()->GetProfileManager()->SetDefaultProfile(m_strProfile);
	}

	m_dwToNotify = 0;
}

LRESULT CGeneralPropPage::OnGeneralPasswordChange(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	CString strCurrentProfile = CUIMApplication::GetApplication()->GetProfileManager()->GetCurrentProfile();

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	CString strUrl, strMessage;
	
	CString strUserDomain = strCurrentProfile.Mid(strCurrentProfile.Find("@"));
	CString strUOLDomain, strBOLDomain;
	strUOLDomain.LoadString(IDS_JABBER_UOL_EMAIL_DOMAIN);
	strBOLDomain.LoadString(IDS_JABBER_BOL_EMAIL_DOMAIN);

	if ( strUserDomain.Compare(strBOLDomain ) == 0 )
	{
		strMessage.LoadString(IDS_PASSCHANGE_MSG_BOL);
        UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION);
		if (ret == IDYES)
		{
			strUrl.LoadString(IDS_PASSCHANGE_URL_BOL);
			CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strUrl));
		}
	}

	else if ( strUserDomain.Compare(strUOLDomain ) == 0 )
	{
		strMessage.LoadString(IDS_PASSCHANGE_MSG_UOL);
        UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION);
		if (ret == IDYES)
		{
			strUrl.LoadString(IDS_PASSCHANGE_URL_UOL);
			CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strUrl));
		}
	}

	else
	{
		CPasswordChangeDlgPtr pDialog;

		CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__PasswordChangeDlgBuilderInstance");			
		if (pElement)
		{
			CElementBuilderPtr pBuilder;				
			pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
			if (pBuilder)
			{
				pDialog = ap_dynamic_cast<CPasswordChangeDlgPtr>(pBuilder->CreateElement());			
				ATLASSERT(pDialog.get());
			}
		}

		pDialog->DoModal();
	}

	return 0;
}
