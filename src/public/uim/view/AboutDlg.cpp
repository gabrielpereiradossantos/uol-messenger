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
 
// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "aboutdlg.h"

#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"


#define SKIN_TOP_MARGIN 26


CAboutDlg::CAboutDlg(void) : 
		CMainDialog(IDD_ABOUTBOX),
		m_clrBackground(RGB(0, 0, 0))
{
}

CAboutDlg::~CAboutDlg(void)
{
}

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	__super::OnInitDialog(NULL, NULL);

	TCHAR szPath[MAX_PATH];
	CString strPath;	

	HINSTANCE hInst = ATL::_AtlBaseModule.GetModuleInstance();
	if (::GetModuleFileName(hInst, szPath, MAX_PATH))
	{
		strPath = szPath;
	}

	CFileVersion fv(strPath);
	ULONGLONG llVersion = fv.GetVersion();
	USHORT* pVersion = reinterpret_cast<USHORT*>(&llVersion);

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strVersionInfo, strVersionFormat;
	strVersionFormat = pStringLoader->GetValue("uol.resource.IDS_VERSION_FORMAT");
	strVersionInfo.Format(strVersionFormat, pVersion[3], pVersion[2], pVersion[1], pVersion[0]);

	GetDlgItem(IDC_STATIC_VERSION_INFO).SetWindowText(strVersionInfo);

	CString strTitle;
	CString strTitleFormat;
	CString strTitleDefault;
	CString strResName;
	
	strResName.Format("#%u", IDR_LICENSE);
	strTitle = pStringLoader->GetValue("uol.resource.IDS_ABOUT_PROPPAGE_TITLE");
	strTitleFormat = pStringLoader->GetValue("uol.resource.IDS_TITLE_WINDOW_DEFAULT");
	strTitleDefault.Format(strTitleFormat, strTitle);
	
	//Set title and text
	SetTitle(strTitleDefault);
	
	HRSRC hrsrc = ::FindResource(ATL::_AtlBaseModule.GetResourceInstance(), strResName, "LICENSE");		
	
	if (hrsrc) 
	{
		HGLOBAL hGlobal = ::LoadResource(ATL::_AtlBaseModule.GetResourceInstance(), hrsrc);
		
		if (hGlobal)
		{

			CString strText;
			CEdit edit = GetDlgItem(IDC_EDIT_ABOUT_CONTRACT);
			CString strBuffer((LPTSTR) ::LockResource(hGlobal)); 
			DWORD dwSize = ::SizeofResource(ATL::_AtlBaseModule.GetResourceInstance(), hrsrc);

			edit.SetWindowText(strBuffer);			
			edit.SetSelNone();
		}
	}

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager)
	{
		m_pAppAboutBoxElement = ap_dynamic_cast<CAppAboutBoxElementPtr>(pUIManager->GetWindowManager()->GetElement("__AppAboutBoxInstance"));
	}

	CenterWindow(GetParent());

	m_labelTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_VERSION_INFO));

	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

void CAboutDlg::DoPaint(CDCHandle& dc)
{
	__super::DoPaint(dc);

	m_clrBackground = dc.GetPixel(30, 30);

	if (m_pAppAboutBoxElement)
	{
		// Draw window icon
		CStaticIconElementPtr pStaticIconAlert = m_pAppAboutBoxElement->GetWindowStaticIcon();

		if (pStaticIconAlert)
		{
			IUOLMessengerImagePtr pImageAlert = pStaticIconAlert->GetImage();
			IUOLMessengerImageDrawStrategyPtr pImageDrawStrategyAlert = pStaticIconAlert->GetImageDrawStrategy();

			if (pImageAlert.get() && pImageDrawStrategyAlert.get())
			{
				pImageAlert->Draw(dc, 32, 26 + SKIN_TOP_MARGIN, pImageAlert->GetWidth(), pImageAlert->GetHeight(), pImageDrawStrategyAlert);
			}
		}
	}
}


LRESULT CAboutDlg::OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL& /* bHandled */)
{
	// Set CEdit background color
	HDC hDC = (HDC) wParam;
	SetBkColor(hDC, m_clrBackground);
	return (LRESULT) (HBRUSH) GetStockObject(HOLLOW_BRUSH);
}