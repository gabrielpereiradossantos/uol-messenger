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
#include "PluginCustomPanelContainerView.h"

#include "usermessages.h"
#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"


CPluginCustomPanelContainerView::CPluginCustomPanelContainerView() :
	m_pTooltipPanelInfo(NULL),
	m_bHoverOn(FALSE),
	m_wndTrackingWindow(this, 1)
{
	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());
	ATLASSERT(pUIManager != NULL);

	IElementPtr pElement = const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__PluginTooltipBuilderInstance");

	if (pElement)
	{
		CElementBuilderPtr pPluginTooltipBuilder = ap_dynamic_cast<CElementBuilderPtr>(pElement);

		if (pPluginTooltipBuilder)
		{
			m_pPluginTooltip = ap_dynamic_cast<CPluginTooltipPtr>(pPluginTooltipBuilder->CreateElement());
		}
	}
}


CPluginCustomPanelContainerView::~CPluginCustomPanelContainerView()
{
}


void CPluginCustomPanelContainerView::Init(IUOLMessengerPluginCustomPanelContainerPtr pPanelContainer)
{
	m_pPanelContainer = ap_dynamic_cast<CPluginCustomPanelContainerPtr>(pPanelContainer);
	ATLASSERT(m_pPanelContainer);

	m_pPanelContainer->RegisterObserver(this);
}


void CPluginCustomPanelContainerView::Finalize()
{
	m_pPanelContainer->UnregisterObserver(this);
}


void CPluginCustomPanelContainerView::GetMinSize(CSize& size)
{
	int cx = 0;
	int cy = 0;

	CAtlList<IUOLMessengerPluginCustomPanelPtr> listPanels;
	m_pPanelContainer->GetPanels(listPanels);
	
	for (POSITION pos = listPanels.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomPanelPtr pPanel = listPanels.GetNext(pos);
		
		CSize sizeMin;
		pPanel->GetMinSize(sizeMin);

		if (sizeMin.cy > 0)
		{
			cy += sizeMin.cy;
		}

		if ((sizeMin.cx > 0) && (sizeMin.cx < cx))
		{
			cx = sizeMin.cx;
		}
	}

	size.cx = cx;
	size.cy = cy;
}


LRESULT CPluginCustomPanelContainerView::OnCreate(LPCREATESTRUCT lpCs)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	CreatePanels();

	return 0L;
}


void CPluginCustomPanelContainerView::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (m_wndTrackingWindow.IsWindow())
	{
		HWND hWndOld  = m_wndTrackingWindow.UnsubclassWindow();
		ATLASSERT(hWndOld != NULL);
	}

	DestroyPanels();
}


void CPluginCustomPanelContainerView::OnSize(UINT nType, const CSize& size)
{
	OnSizeImpl(size);
}


LRESULT CPluginCustomPanelContainerView::OnEraseBackground(HDC hDc)
{
	return 1;
}


LRESULT CPluginCustomPanelContainerView::OnSetCursor(HWND hWnd, UINT nHitTest, UINT uMsg)
{
	//ATLTRACE(_T("%s: hWnd = 0x%x, uMsg = 0x%x\n"), __FUNCTION__, hWnd, uMsg);

	if (uMsg == WM_MOUSEMOVE)
	{
		TrackMouseMove(hWnd);
	}

	return FALSE;
}


void CPluginCustomPanelContainerView::OnMouseHover(UINT nFlags, CPoint pt)
{
	//ATLTRACE(_T("%s: pt(%d,%d)\n"), __FUNCTION__, pt.x, pt.y);

	if (!m_pPluginTooltip)
	{
		return;
	}

	m_bHoverOn = TRUE;

	// Show panel tooltip...
	ATLTRACE(_T("Show tooltip for %s\n"), GetPanelId(m_pTooltipPanelInfo->pPanel));

	if (m_pPluginTooltip->IsWindow())
	{
		m_pPluginTooltip->DestroyWindow();
	}

	IUOLMessengerTooltipProviderPtr pTooltipProvider = m_pTooltipPanelInfo->pPanel->GetTooltipProvider();

	m_pPluginTooltip->SetTooltipProvider(pTooltipProvider);

	if (pTooltipProvider)
	{
		m_pPluginTooltip->Create(m_hWnd, NULL, "", WS_POPUP);

		if (m_pPluginTooltip->IsWindow())
		{
			CPoint ptTooltipPos;
			ptTooltipPos.x = pt.x;
			ptTooltipPos.y = pt.y;

			::ClientToScreen(m_wndTrackingWindow, &ptTooltipPos);

			m_pPluginTooltip->Show(m_hWnd, ptTooltipPos);
		}
	}

	// Let child do default message handling
	SetMsgHandled(FALSE);
}


void CPluginCustomPanelContainerView::OnMouseLeave()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	// Hide tooltip
	if (m_pTooltipPanelInfo)
	{
		//ATLTRACE(_T("%s - Hidding tooltip\n"), __FUNCTION__);

		if (m_pPluginTooltip && (m_pPluginTooltip->IsWindow()))
		{
			m_pPluginTooltip->DestroyWindow();

			m_pPluginTooltip->SetTooltipProvider(IUOLMessengerTooltipProviderPtr());
		}
	}

	m_bHoverOn = FALSE;

	// Let child do default message handling
	SetMsgHandled(FALSE);
}


void CPluginCustomPanelContainerView::OnAddPanel(IUOLMessengerPluginCustomPanelPtr pPanel)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	CSize sizeMin;
	pPanel->GetMinSize(sizeMin);

	CWindow wndPanel = pPanel->CreatePanel(m_hWnd, 
												  CRect(0, rectClient.Height(), rectClient.Width(), rectClient.Height() + sizeMin.cy), 
												  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	ATLASSERT(wndPanel.IsWindow());
	if (wndPanel.IsWindow())
	{
		wndPanel.ShowWindow(SW_SHOWNORMAL);

		PanelInfo* pPanelInfo = new PanelInfo();
		pPanelInfo->pPanel = pPanel;
		pPanelInfo->hWndPanel = static_cast<HWND>(wndPanel);

		m_listPanels.AddTail(pPanelInfo);

		m_mapPanels.SetAt(static_cast<HWND>(wndPanel), pPanelInfo);

		CWindow wndParent = GetParent();
		wndParent.SendMessage(WM_USER_UPDATE_LAYOUT);
	}
}


void CPluginCustomPanelContainerView::OnRemovePanel(IUOLMessengerPluginCustomPanelPtr pPanel)
{
	BOOL bRemoved = FALSE;
	PanelInfo* pPanelInfo = NULL;
	CString strRemovedId = GetPanelId(pPanel);

	for (POSITION pos = m_listPanels.GetHeadPosition(); pos; m_listPanels.GetNext(pos))
	{
		pPanelInfo = m_listPanels.GetAt(pos);
		
		CString strPanelId = GetPanelId(pPanelInfo->pPanel);
		
		if (0 == strRemovedId.Compare(strPanelId))
		{
			CWindow wndPanel = pPanelInfo->hWndPanel;

			if ((m_wndTrackingWindow.IsWindow()) &&
				(wndPanel.IsChild(m_wndTrackingWindow)))
			{
			
				HWND hWndOld  = m_wndTrackingWindow.UnsubclassWindow();
				ATLASSERT(hWndOld != NULL);

				if (m_pPluginTooltip)
				{
					m_pPluginTooltip->SetTooltipProvider(IUOLMessengerTooltipProviderPtr());
				}
			}

			pPanel->DestroyPanel(pPanelInfo->hWndPanel);
			
			m_listPanels.RemoveAt(pos);
			m_mapPanels.RemoveKey(pPanelInfo->hWndPanel);
			delete pPanelInfo;
			bRemoved = TRUE;
			
			break;
		}
	}

	if (bRemoved)
	{
		CWindow wndParent = GetParent();
		wndParent.SendMessage(WM_USER_UPDATE_LAYOUT);
	}
}


void CPluginCustomPanelContainerView::CreatePanels()
{
	CRect rectClient;
	GetClientRect(&rectClient);

	CAtlList<IUOLMessengerPluginCustomPanelPtr> listPanels;
	m_pPanelContainer->GetPanels(listPanels);
	
	int x = 0;
	int y = 0;

	for (POSITION pos = listPanels.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomPanelPtr pPanel = listPanels.GetNext(pos);

		CSize sizeMin;
		pPanel->GetMinSize(sizeMin);

		CWindow wndPanel = pPanel->CreatePanel(m_hWnd, 
												CRect(x, y, rectClient.Width(), y + sizeMin.cy), 
												WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

		ATLASSERT(wndPanel.IsWindow());
		if (wndPanel.IsWindow())
		{
			wndPanel.ShowWindow(SW_SHOWNORMAL);

			y += sizeMin.cy;

			PanelInfo* pPanelInfo = new PanelInfo();
			pPanelInfo->pPanel = pPanel;
			pPanelInfo->hWndPanel = static_cast<HWND>(wndPanel);

			m_listPanels.AddTail(pPanelInfo);
			m_mapPanels.SetAt(static_cast<HWND>(wndPanel), pPanelInfo);
		}
	}
}


void CPluginCustomPanelContainerView::DestroyPanels()
{
	PanelInfo* pPanelInfo;

	for (POSITION pos = m_listPanels.GetHeadPosition(); pos; )
	{
		pPanelInfo = m_listPanels.GetNext(pos);

		IUOLMessengerPluginCustomPanelPtr pPanel = pPanelInfo->pPanel;
		pPanel->DestroyPanel(pPanelInfo->hWndPanel);
		delete pPanelInfo;
	}

	m_listPanels.RemoveAll();
	m_mapPanels.RemoveAll();

	m_pPluginTooltip->SetTooltipProvider(IUOLMessengerTooltipProviderPtr());
}


CString CPluginCustomPanelContainerView::GetPanelId(IUOLMessengerPluginCustomPanelPtr pPanel)
{
	CString strId;

	strId.Format(_T("%s/%s"), pPanel->GetPluginInternalName(), pPanel->GetPanelName());

	return strId;
}


void CPluginCustomPanelContainerView::OnSizeImpl(const CSize& size)
{
	int y = 0;
	PanelInfo* pPanelInfo;

	for (POSITION pos = m_listPanels.GetHeadPosition(); pos; )
	{
		pPanelInfo = m_listPanels.GetNext(pos);

		CSize sizeMin;
		(pPanelInfo->pPanel)->GetMinSize(sizeMin);

		CWindow wndPanel = pPanelInfo->hWndPanel;

		ATLASSERT(wndPanel.IsWindow());
		wndPanel.SetWindowPos(NULL, 0, y, size.cx, sizeMin.cy, SWP_NOZORDER);
		y += sizeMin.cy;
	}
}


void CPluginCustomPanelContainerView::TrackMouseMove(HWND hWndChild)
{
	//ATLTRACE(_T("%s: pt(%d,%d)\n"), __FUNCTION__, pt.x, pt.y);

	if (hWndChild != m_wndTrackingWindow)
	{
		HWND hWndPanel = hWndChild;

		while ((hWndPanel != NULL) && (::GetParent(hWndPanel) != m_hWnd))
		{
			hWndPanel = ::GetParent(hWndPanel);
		}

		CPanelMap::CPair* pPair = m_mapPanels.Lookup(hWndPanel);
		if (pPair)
		{
			PanelInfo* pPanelInfo = pPair->m_value;

			if (pPanelInfo != m_pTooltipPanelInfo)
			{
				// Tooltip changed...
				m_pTooltipPanelInfo = pPanelInfo;

				//ATLTRACE(_T("%s New tooltip = %s\n"), __FUNCTION__, GetPanelId(pPanelInfo->pPanel));
			}

			if (m_pPluginTooltip && (m_pPluginTooltip->IsWindow()))
			{
				m_pPluginTooltip->DestroyWindow();
				m_pPluginTooltip->SetTooltipProvider(IUOLMessengerTooltipProviderPtr());
			}

			// Track mouse events from child window...
			m_bHoverOn = FALSE;

			if (m_wndTrackingWindow.IsWindow())
			{
				HWND hWndOld  = m_wndTrackingWindow.UnsubclassWindow();
				ATLASSERT(hWndOld != NULL);
			}

			BOOL bSubclass = m_wndTrackingWindow.SubclassWindow(hWndChild);
			ATLASSERT(bSubclass);

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWndChild;
			tme.dwFlags = TME_LEAVE|TME_HOVER;
			tme.dwHoverTime = 400;

			BOOL bTracking = ::_TrackMouseEvent(&tme);
			ATLASSERT(bTracking);

			//ATLTRACE(_T("Tracking window: 0x%x\n"), hWndChild);
		}
	}
	else
	{
		// Same window, just restart tracking
		if (!m_bHoverOn)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWndChild;
			tme.dwFlags = TME_LEAVE|TME_HOVER;
			tme.dwHoverTime = 400;

			BOOL bTracking = ::_TrackMouseEvent(&tme);
			ATLASSERT(bTracking);

			//ATLTRACE(_T("Tracking again window: 0x%x\n"), hWndChild);
		}
	}
}