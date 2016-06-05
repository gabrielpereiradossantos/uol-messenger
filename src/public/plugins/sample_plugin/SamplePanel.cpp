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
#include "SamplePanel.h"

#include "resource.h"
#include "FacadeWrapper.h"
#include "SampleNotifyPopUpHyperLinkHandler.h"


int CSamplePanel::TIMER_SHOW_NOTIFY_POPUP = 0x387;


CSamplePanel::CSamplePanel(const CString& strPanelName, const CString& strPluginInternalName) : 
m_strPanelName(strPanelName),
m_strPluginInternalName(strPluginInternalName),
m_nTimerNotifyPopUp(0)
{
	m_pTooltipProvider = new CSampleTooltipProvider(m_strPanelName);
}


CString CSamplePanel::GetPanelName() const
{
	return m_strPanelName;
}


CString CSamplePanel::GetPluginInternalName() const
{
	return m_strPluginInternalName;
}


void CSamplePanel::GetMinSize(CSize& size)
{
	size.cx = 0;
	size.cy = 80;
}


HWND CSamplePanel::CreatePanel(HWND hWndParent, const CRect& rectPanel, DWORD dwWindowStyle)
{
	CRect rectWindow = rectPanel;

	HWND hWnd = Create(hWndParent, rectWindow, NULL, dwWindowStyle);

	return hWnd;
}


void CSamplePanel::DestroyPanel(HWND hWndPanel)
{
	if (hWndPanel == m_hWnd)
	{
		DestroyWindow();
	}
}


IUOLMessengerTooltipProviderPtr CSamplePanel::GetTooltipProvider() const
{
	return m_pTooltipProvider;
}


LRESULT CSamplePanel::OnCreate(LPCREATESTRUCT lpCs)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	// Create controls
	m_btnTest1.Create(m_hWnd, CRect(0, 0, rectClient.Width(), rectClient.Height() / 2), "Enable notification", WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, ID_BUTTON_ENABLE_NOTIFY_POPUP);
	m_btnTest2.Create(m_hWnd, CRect(0, rectClient.Height() / 2, rectClient.Width(), rectClient.Height()), "Disable notification", WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, ID_BUTTON_DISABLE_NOTIFY_POPUP);

	ATLASSERT(m_btnTest1.IsWindow());
	ATLASSERT(m_btnTest2.IsWindow());

	// Load tooltip icon
	IUOLMessengerUIManagerPtr pUIManager = CFacadeWrapper::GetFacadeInstance()->GetUIManager();

	if (pUIManager)
	{
		m_pIcon = pUIManager->LoadImage(ATL::_AtlBaseModule.GetModuleInstance(), IDB_SAMPLE_TOOLTIP);

		m_pTooltipProvider->SetImage(m_pIcon);
	}

	return 0L;
}


void CSamplePanel::OnDestroy()
{
	if (m_nTimerNotifyPopUp != 0)
	{
		BOOL bKillTimer = KillTimer(m_nTimerNotifyPopUp);
		ATLASSERT(bKillTimer);
	}

	m_btnTest1.DestroyWindow();
	m_btnTest2.DestroyWindow();

	m_pIcon.reset();
}


void CSamplePanel::OnSize(UINT nType, const CSize& size)
{
	if (nType != SIZE_MINIMIZED)
	{
		// Reposition controls
		m_btnTest1.SetWindowPos(NULL, 0, 0, size.cx, 40, SWP_NOZORDER);
		m_btnTest2.SetWindowPos(NULL, 0, 40, size.cx, 40, SWP_NOZORDER);
	}
}


void CSamplePanel::OnTimer(UINT nTimerId, TIMERPROC /*timerProc*/)
{
	if (TIMER_SHOW_NOTIFY_POPUP == nTimerId)
	{
		IUOLMessengerFacade* pFacade = CFacadeWrapper::GetFacadeInstance();

		if (pFacade)
		{
			IUOLMessengerNotifyPopUpManagerPtr pNotifyPopUpManager = pFacade->GetNotifyPopUpManager();

			if (pNotifyPopUpManager)
			{
				static int nPopUpCount = 1;

				CString strText;
				strText.Format(_T("Notify pop-up test #%d"), nPopUpCount);
				++nPopUpCount;

				CAtlList<CString> listLines;
				listLines.AddTail(strText);

				pNotifyPopUpManager->ShowAlertWindow(_T("Sample Plugin Notification"), 
						listLines, m_pIcon, new CSampleNotifyPopUpHyperLinkHandler());
			}
		}
	}
}


void CSamplePanel::OnClickedButtonTest(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	CString strMessage;

	if (nID == ID_BUTTON_ENABLE_NOTIFY_POPUP)
	{
		if (0 == m_nTimerNotifyPopUp)
		{
			m_nTimerNotifyPopUp = SetTimer(TIMER_SHOW_NOTIFY_POPUP, 15 * 1000, NULL);
			ATLASSERT(m_nTimerNotifyPopUp != 0);
		}
	}
	else
	{
		if (0 != m_nTimerNotifyPopUp)
		{
			BOOL bKillTimer = KillTimer(m_nTimerNotifyPopUp);
			ATLASSERT(bKillTimer);
			m_nTimerNotifyPopUp = 0;
		}
	}
}