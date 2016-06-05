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
 
// ProfileDlg.cpp : Implementation of CProfileDlg

#include "stdafx.h"
#include "ProfileDlg.h"
#include "MinDependencySpecialFolder.h"

#include "skin/SolidBackgroundElement.h"
#include "../resource.h"

BOOL CProfileDlg::ProfileDlgExecute(CProfileManager* profileManagerPtr)
{
	m_profileManagerPtr = profileManagerPtr;

	if (this->DoModal() > 0)
		return TRUE;
	else
		return FALSE;
}

LRESULT CProfileDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow(GetParent());

	CMinDependencySpecialFolder	minDSF;
	CString	strCompany;
	CString	strProduct;
	CString	strProfile;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strProfile.LoadString(IDS_STRING_PROFILE_DIR);
	m_strProfileBaseDir.Format("%s%s\\%s\\%s", minDSF.GetAppData(), strCompany, strProduct, strProfile);

	InitDialog();

	m_bkg = RGB(3, 70, 183);	
	m_brushBkg.CreateSolidBrush(m_bkg);	

	CSolidBackgroundElementPtr pBkg = new CSolidBackgroundElement();		
	pBkg->SetColor(m_bkg);	
	
	m_pActiveBackground = pBkg;
	m_pInactiveBackground = pBkg;

	m_staticTip.SubclassWindow(GetDlgItem(IDC_STATIC_PROFILE_DLG_TIP));		
	m_staticTip.SetColor(RGB(255, 255, 255));
	

	return 1;  // Let the system set the focus
}

LRESULT CProfileDlg::OnCloseDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndDialog(0);
	return 1;  // Let the system set the focus
}

LRESULT CProfileDlg::OnCtlColorStatic(HDC hDC, HWND hwnd)
{	
	if ((hwnd == GetDlgItem(IDC_RADIO_PROFILE_0)) ||
		(hwnd == GetDlgItem(IDC_RADIO_PROFILE_1)))
	{	
		CDCHandle dc(hDC);
		
		dc.SetTextColor(RGB(255, 255, 255));	
		dc.SetBkColor(m_bkg);
	}

	return reinterpret_cast<LRESULT>(m_brushBkg.m_hBrush);
}

LRESULT CProfileDlg::OnCtlColorBtn(HDC hDC, HWND hwnd)
{
	return reinterpret_cast<LRESULT>(m_brushBkg.m_hBrush);
}

LRESULT CProfileDlg::OnRadioCustomDraw(LPNMHDR lpNmhdr)
{
	LPNMCUSTOMDRAW pNMCustomDraw = reinterpret_cast<LPNMCUSTOMDRAW>(lpNmhdr);

	switch(pNMCustomDraw->dwDrawStage)
	{
	case CDDS_PREPAINT:
		CDCHandle dc(pNMCustomDraw->hdc);
		DWORD dwText = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	
		CRect client_rect = pNMCustomDraw->rc;				
	
		CString szText;
		CWindow(lpNmhdr->hwndFrom).GetWindowText(szText);

		int nOldBkMode = dc.SetBkMode(TRANSPARENT);
		COLORREF oldColor = dc.SetTextColor(RGB(255, 255, 255));		

		client_rect.left += 8;
		dc.DrawText(szText, szText.GetLength(), client_rect, dwText);		

		dc.SetBkMode(nOldBkMode);
		dc.SetTextColor(oldColor);		

	return CDRF_SKIPDEFAULT;
	}


	SetMsgHandled(FALSE);

	return CDRF_DODEFAULT;
}

LRESULT CProfileDlg::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoDataExchange(TRUE);

	IUOLMessengerProfileElementPtr pProfileElement;

	CString strPerfil = "";
	CString strPath = "";

	if (this->m_nRadioProfile == 0)
	{
		this->m_wndComboProfile.GetWindowText(strPerfil);
		m_profileManagerPtr->GetProfileElement(strPerfil, pProfileElement);
		strPath = pProfileElement->GetProfilePath();
		m_profileManagerPtr->SetCurrentProfile(strPerfil);
		EndDialog(wID);
	}
	else
	{
		this->m_wndEditProfile.GetWindowText(strPerfil);

		strPerfil.Trim();

		if (strPerfil.GetLength() > 0)
		{
			if (!m_profileManagerPtr->IsProfile(strPerfil))
			{
				if (!m_profileManagerPtr->CreateProfile(strPerfil, pProfileElement))
				{
					CString strAppName;
					CString strMessage;
					strAppName = m_pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
					strMessage.LoadString(IDS_DIALOG_PROFILE_INVALID);
					MessageBox(strMessage, strAppName, MB_OK | MB_ICONERROR);

					this->m_wndEditProfile.SetWindowText("");
					this->m_wndEditProfile.SetFocus();

					return 1;
				}
				else
				{
					m_profileManagerPtr->GetProfileElement(strPerfil, pProfileElement);
					strPath = pProfileElement->GetProfilePath();
					m_profileManagerPtr->SetCurrentProfile(strPerfil);
					EndDialog(wID);
				}

			}
			else
			{
				CString strAppName;
				CString strMessage;
				strAppName = m_pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
				strMessage.LoadString(IDS_DIALOG_PROFILE_EXISTS);
				MessageBox(strMessage, strAppName, MB_OK | MB_ICONERROR);

				this->m_wndEditProfile.SetWindowText("");
				this->m_wndEditProfile.SetFocus();

				return 1;
			}
		}
		else
		{
			CString strAppName;
			CString strMessage;
			strAppName = m_pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
			strMessage.LoadString(IDS_DIALOG_PROFILE_NEW_ERROR);
			MessageBox(strMessage, strAppName, MB_OK | MB_ICONERROR);
			return 1;
		}
	}

	return 0;
}

LRESULT CProfileDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(0);

	return 0;
}

LRESULT CProfileDlg::OnClickedRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CString strMessage;
	CString strAppName;
	strMessage.LoadString(IDS_DIALOG_PROFILE_REMOVE_MESSAGE);
	strAppName = m_pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

	if (IDYES == MessageBox(strMessage, strAppName, MB_YESNO | MB_ICONQUESTION))
	{
		DoDataExchange(TRUE);

		CString csPerfil = "";

		this->m_wndComboProfile.GetWindowText(csPerfil);
		this->m_wndComboProfile.DeleteString(this->m_wndComboProfile.GetCurSel());
		this->m_profileManagerPtr->RemoveProfile(csPerfil);
		InitDialog();
	}

	return 0;
}

LRESULT CProfileDlg::OnRadioProfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoDataExchange(TRUE);

	if (this->m_nRadioProfile == 0)
	{
		if (this->m_wndComboProfile.GetCount() > 0)
		{
			DoDataExchange(FALSE);
			SelectEnabled(TRUE);
			NewEnabled(FALSE);
		}
		else
		{
			this->m_nRadioProfile = 1;
			this->m_wndEditProfile.SetFocus();
			DoDataExchange(FALSE);
		}

	}
	else if (this->m_nRadioProfile == 1)
	{
		DoDataExchange(FALSE);
		SelectEnabled(FALSE);
		NewEnabled(TRUE);
	}

	return 0;
}
void CProfileDlg::InitDialog()
{
	CAtlList <CString> profileList;
	m_profileManagerPtr->GetProfileList(profileList);

	if (profileList.GetCount() > 0)
	{
		this->m_nRadioProfile = 0;
		DoDataExchange(FALSE);
		this->m_wndComboProfile.ResetContent();

		for(POSITION p = profileList.GetHeadPosition(); p != NULL; profileList.GetNext(p))
		{
			this->m_wndComboProfile.AddString(profileList.GetAt(p));
		}

		SelectEnabled(TRUE);
		NewEnabled(FALSE);

		if (m_profileManagerPtr->GetCurrentProfile() == "")
			this->m_wndComboProfile.SetCurSel(0);
		else
			this->m_wndComboProfile.SelectString(-1, m_profileManagerPtr->GetCurrentProfile());

	}
	else
	{
		this->m_nRadioProfile = 1;
		DoDataExchange(FALSE);

		SelectEnabled(FALSE);
		NewEnabled(TRUE);
	}
}

void CProfileDlg::SelectEnabled(BOOL bEnabled)
{
	if(bEnabled)
	{
		this->m_wndComboProfile.EnableWindow(1);
		this->m_wndButtonRemoveProfile.EnableWindow(1);
	}
	else
	{
		this->m_wndComboProfile.EnableWindow(0);
		this->m_wndButtonRemoveProfile.EnableWindow(0);
	}

}

void CProfileDlg::NewEnabled(BOOL bEnabled)
{
	if(bEnabled)
	{
		this->m_wndEditProfile.SetWindowText("");
		this->m_wndEditProfile.EnableWindow(1);
		this->m_wndButtonChangeDir.EnableWindow(1);
	}
	else
	{
		this->m_wndEditProfile.SetWindowText("");
		this->m_wndEditProfile.EnableWindow(0);
		this->m_wndButtonChangeDir.EnableWindow(0);
	}
}

LRESULT CProfileDlg::OnChangeDir(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BROWSEINFO bi = { 0 };
//    bi.lpszTitle = _T("Selecione um diretório");
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

	if ( pidl != 0 )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if ( SHGetPathFromIDList ( pidl, path ) )
        {
			m_strProfileBaseDir.Format("%s", path);
        }
    }

	return 0;
}
// CProfileDlg
