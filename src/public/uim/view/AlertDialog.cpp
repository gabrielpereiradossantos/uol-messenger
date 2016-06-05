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


#include "stdafx.h"
#include "AlertDialog.h"

#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"
#include "skin/AppDefaultIconElement.h"


#define SKIN_TOP_MARGIN 26


CAlertDialog::CAlertDialog() : 
	CMainDialog(IDD_DIALOG_MAINFRAME_CLOSED_ALERT),
	m_bCheck(FALSE)
{
	::ZeroMemory(&m_ovi, sizeof(OSVERSIONINFO));
}


CAlertDialog::~CAlertDialog()
{
}


LRESULT CAlertDialog::OnInitDialog(HWND, LPARAM lParam)
{	
	__super::OnInitDialog(NULL, NULL);
	
	UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	if (pStringLoader)
	{
		// Set window text
		CString strTitle;

		strTitle = pStringLoader->GetValue(_T("uol.resource.IDS_APP_NAME"));
		ATLASSERT(FALSE == strTitle.IsEmpty());

		//Set title and text
		SetTitle(strTitle);

		// Set message 1 text
		CString strAlertMessage1;
		CString strAlertMessage1Format;
		CString strAppName;

		strAlertMessage1Format = pStringLoader->GetValue(_T("uol.resource.IDS_ALERT_WINDOW_MESSAGE1_FORMAT"));
		strAppName = pStringLoader->GetValue(_T("uol.resource.IDS_APP_NAME"));

		ATLASSERT(FALSE == strAlertMessage1Format.IsEmpty());
		ATLASSERT(FALSE == strAppName.IsEmpty());

		strAlertMessage1.Format(strAlertMessage1Format, strAppName);

		CWindow wndStatic1 = GetDlgItem(IDC_STATIC_MESSAGE1);
		ATLASSERT(wndStatic1.IsWindow());

		wndStatic1.SetWindowText(strAlertMessage1);
	}	

	// Get windows version
	::ZeroMemory(&m_ovi, sizeof(OSVERSIONINFO));	
	m_ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&m_ovi);

	// Get skin properties
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager)
	{
		m_pAppCloseAlertElement = ap_dynamic_cast<CAppCloseAlertElementPtr>(pUIManager->GetWindowManager()->GetElement("__AppCloseAlertInstance"));
	}
	
	m_label1TransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_MESSAGE1));
	m_label2TransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_MESSAGE2));

	DoDataExchange(FALSE);

	::SetForegroundWindow(m_hWnd);

	CenterWindow(::GetDesktopWindow());

	// Register transparent controls
	RegisterControl(GetDlgItem(IDC_CHECK_CONFIRM));
	SetParentWindow(m_hWnd);
	SetBitmapBackground(m_pActiveBackground);

	return 0L;
}


void CAlertDialog::OnDestroy()
{
	DoDataExchange(TRUE);

	SetMsgHandled(FALSE);
}


void CAlertDialog::DoPaint(CDCHandle& dc)
{
	__super::DoPaint(dc);

	if (m_pAppCloseAlertElement)
	{
		// Draw window icon
		CStaticIconElementPtr pStaticIconAlert = m_pAppCloseAlertElement->GetWindowStaticIcon();

		if (pStaticIconAlert)
		{
			IUOLMessengerImagePtr pImageAlert = pStaticIconAlert->GetImage();
			IUOLMessengerImageDrawStrategyPtr pImageDrawStrategyAlert = pStaticIconAlert->GetImageDrawStrategy();

			if (pImageAlert.get() && pImageDrawStrategyAlert.get())
			{
				pImageAlert->Draw(dc, 14, 14 + SKIN_TOP_MARGIN, pImageAlert->GetWidth(), 
								  pImageAlert->GetHeight(), pImageDrawStrategyAlert);
			}
		}

		// Draw tray sample image
		CStaticIconElementPtr pStaticIconTray = GetTrayIconSample();

		if (pStaticIconTray)
		{
			IUOLMessengerImagePtr pImageTray = pStaticIconTray->GetImage();
			IUOLMessengerImageDrawStrategyPtr pImageDrawStrategyTray = pStaticIconTray->GetImageDrawStrategy();

			if (pImageTray.get() && pImageDrawStrategyTray.get())
			{
				CRect rectClient;
				GetClientRect(&rectClient);

				int cx = pImageTray->GetWidth();
				int cy = pImageTray->GetHeight();
				int x = (rectClient.Width() - cx) / 2;
				const int y = 110 + SKIN_TOP_MARGIN;

				pImageTray->Draw(dc, x, y, cx, cy, pImageDrawStrategyTray);
			}
		}
	}	
}


CStaticIconElementPtr CAlertDialog::GetTrayIconSample()
{
	CStaticIconElementPtr pStaticIconTraySample;

	if (m_ovi.dwMajorVersion >= 5)
	{
		// Vista or Later
		if (m_ovi.dwMajorVersion > 5)
		{
			pStaticIconTraySample = m_pAppCloseAlertElement->GetVistaTraySampleStaticIcon();
		}

		if (!pStaticIconTraySample)
		{
			pStaticIconTraySample = m_pAppCloseAlertElement->GetXPTraySampleStaticIcon();
		}
	}

	// Default icon
	if (!pStaticIconTraySample)
	{
		pStaticIconTraySample = m_pAppCloseAlertElement->Get2kTraySampleStaticIcon(); 
	}
	
	return pStaticIconTraySample;
}


LRESULT CAlertDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}