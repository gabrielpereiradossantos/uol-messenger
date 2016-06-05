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

#include "stdafx.h"
#include <interfaces/IUOLMessengerFrameWindow.h>
#include "../resource.h"
#include "wtlext.h"
#include "UOLFoneRatesWindowFrame.h"
#include "../FacadeWrapper.h"


CUOLFoneRatesWindowFrame::CUOLFoneRatesWindowFrame(){}
CUOLFoneRatesWindowFrame::~CUOLFoneRatesWindowFrame() {/*m_tarifasWnd.DestroyWindow();*/}
HWND CUOLFoneRatesWindowFrame::CreateView(HWND hWndParent)
{
	HWND hWnd = Create(hWndParent);
	return hWnd;
}
void CUOLFoneRatesWindowFrame::GetMinSize(CSize& size) 
{
	//IUOLMessengerFacade*	pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	//IUOLMessengerSettingsManagerPtr pSettingsManager = 
	//		ap_static_cast<IUOLMessengerSettingsManagerPtr>(pFacade->GetSettingsManager());
	//
	//int nCordX = pSettingsManager->GetGeneralSettings()->GetCordX();
	//int nCordY = pSettingsManager->GetGeneralSettings()->GetCordY();
	//int nSizeX = pSettingsManager->GetGeneralSettings()->GetSizeX();
	//int nSizeY = pSettingsManager->GetGeneralSettings()->GetSizeY();

	//CRect rectWindow = CRect(CPoint(nCordX, nCordY), CSize(nSizeX, nSizeY));
	//
	//HMONITOR  hMonitor = ::MonitorFromRect(rectWindow, MONITOR_DEFAULTTONEAREST);
	//
	//MONITORINFO mInfo = {0};
	//mInfo.cbSize = sizeof(MONITORINFO);

	//if (0 != GetMonitorInfo(hMonitor, &mInfo))
	//{
	//	CRect rectMonitorArea(mInfo.rcMonitor); // Monitor Area

	//	if ((rectMonitorArea.right == 800)&&(rectMonitorArea.bottom == 600))
	//	{
	//		size.cx = 796-210;
	//		size.cy = 424;
	//	}
	//	else
	//	{
	//		size.cx = 796;
	//		size.cy = 424;
	//	}
	//}
	//else
	//{
	//	size.cx = 796;
	//	size.cy = 424;
	//}

	size.cx = 400;
	size.cy = 424;

	

}
BOOL CUOLFoneRatesWindowFrame::PreTranslateMessage(MSG* pMsg) 
{
	return FALSE;
}

BOOL CUOLFoneRatesWindowFrame::ShouldCancelNavigation(CString strURL)
{
	if ((0 != strURL.Find(_T("res://"))) && 
			(0 != strURL.Compare(_T("about:blank"))) && 
			(0 != strURL.IsEmpty()) )
	{
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);
		
		return TRUE;
	}
	
	return FALSE;
}



BOOL CUOLFoneRatesWindowFrame::SetUrl(const CString& strUrl)
{	
	m_tarifasWnd.ShowWindow(SW_SHOW);

	return m_tarifasWnd.SetUrl(strUrl);
}

LRESULT CUOLFoneRatesWindowFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls ...
	//

	m_tarifasWnd.Create(m_hWnd, CRect(0, 0, 796, 424), NULL, WS_CHILDWINDOW | WS_VSCROLL | /*WS_SIZEBOX |*/ WS_CLIPSIBLINGS | /*WS_VISIBLE |*/ BS_OWNERDRAW);
	m_tarifasWnd.SetWindowHolder(this);
	
	ATLASSERT(m_tarifasWnd.IsWindow());
	
	return 0L;
}


LRESULT CUOLFoneRatesWindowFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_tarifasWnd)
	{
		m_tarifasWnd.DestroyWindow();
	}

	
	return 0L;
}

LRESULT CUOLFoneRatesWindowFrame::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int x = LOWORD(lParam); 
	int y = HIWORD(lParam);

	if (m_tarifasWnd)
	{
		m_tarifasWnd.ResizeClient(x, y, TRUE);
	}
	
	return 0L;
}