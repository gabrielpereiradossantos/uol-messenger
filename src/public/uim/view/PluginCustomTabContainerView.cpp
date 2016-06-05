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
#include "PluginCustomTabContainerView.h"

#include "usermessages.h"
#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"


CPluginCustomTabContainerView::CPluginCustomTabContainerView() :
	m_pTooltipTabInfo(NULL),
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


CPluginCustomTabContainerView::~CPluginCustomTabContainerView()
{
}


void CPluginCustomTabContainerView::Init(IUOLMessengerPluginCustomTabContainerPtr pTabContainer, CTabViewPtr pTabViewCtrl)
{	
	m_pTabContainer = ap_dynamic_cast<CPluginCustomTabContainerPtr>(pTabContainer);
	ATLASSERT(m_pTabContainer);

	m_pTabContainer->RegisterObserver(this);

	ATLASSERT(pTabViewCtrl);
	m_pTabViewCtrl = pTabViewCtrl;
}


void CPluginCustomTabContainerView::Finalize()
{
	m_pTabContainer->UnregisterObserver(this);
}


void CPluginCustomTabContainerView::GetMinSize(CSize& size)
{
	int cx = 0;
	int cy = 0;

	CAtlList<IUOLMessengerPluginCustomTabPtr> listTabs;
	m_pTabContainer->GetTabs(listTabs);
	
	for (POSITION pos = listTabs.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomTabPtr pTab = listTabs.GetNext(pos);
		
		CSize sizeMin;
		pTab->GetMinSize(sizeMin);

		cy = __max(cy, sizeMin.cy);
		
		cx = __max(cx, sizeMin.cx);
	}

	size.cx = cx;
	size.cy = cy;
}


UINT CPluginCustomTabContainerView::GetTabsCount()
{
	return m_listTabs.GetCount();
}


LRESULT CPluginCustomTabContainerView::OnCreate(LPCREATESTRUCT lpCs)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	CreateTabs();

	return 0L;
}


void CPluginCustomTabContainerView::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (m_wndTrackingWindow.IsWindow())
	{
		HWND hWndOld  = m_wndTrackingWindow.UnsubclassWindow();
		ATLASSERT(hWndOld != NULL);
	}

	DestroyTabs();
}


void CPluginCustomTabContainerView::OnSize(UINT nType, const CSize& size)
{
	OnSizeImpl(size);
}


LRESULT CPluginCustomTabContainerView::OnEraseBackground(HDC hDc)
{
	return 1;
}


LRESULT CPluginCustomTabContainerView::OnSetCursor(HWND hWnd, UINT nHitTest, UINT uMsg)
{
	//ATLTRACE(_T("%s: hWnd = 0x%x, uMsg = 0x%x\n"), __FUNCTION__, hWnd, uMsg);

	if (uMsg == WM_MOUSEMOVE)
	{
		TrackMouseMove(hWnd);
	}

	return FALSE;
}


void CPluginCustomTabContainerView::OnMouseHover(UINT nFlags, CPoint pt)
{
	//ATLTRACE(_T("%s: pt(%d,%d)\n"), __FUNCTION__, pt.x, pt.y);

	if (!m_pPluginTooltip)
	{
		return;
	}

	m_bHoverOn = TRUE;

	// Show Tab tooltip...
	ATLTRACE(_T("Show tooltip for %s\n"), GetTabId(m_pTooltipTabInfo->pTab));

	if (m_pPluginTooltip->IsWindow())
	{
		m_pPluginTooltip->DestroyWindow();
	}

	IUOLMessengerTooltipProviderPtr pTooltipProvider = m_pTooltipTabInfo->pTab->GetTooltipProvider();

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


void CPluginCustomTabContainerView::OnMouseLeave()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	// Hide tooltip
	if (m_pTooltipTabInfo)
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


void CPluginCustomTabContainerView::OnAddTab(IUOLMessengerPluginCustomTabPtr pTab)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	
	CSize sizeMin;
	pTab->GetMinSize(sizeMin);
	
	CWindow wndTab = pTab->CreateTab(m_hWnd, 
			CRect(rectClient.left, rectClient.top, rectClient.right, rectClient.bottom), 
					WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	
	ATLASSERT(wndTab.IsWindow());
	if (wndTab.IsWindow())
	{
		//wndTab.ShowWindow(SW_SHOWNORMAL);
		
		TabInfo* pTabInfo = new TabInfo();
		pTabInfo->pTab = pTab;
		pTabInfo->hWndTab = static_cast<HWND>(wndTab);
		
		m_listTabs.AddTail(pTabInfo);
		
		m_mapTabs.SetAt(static_cast<HWND>(wndTab), pTabInfo);
		
		CWindow wndParent = GetParent();
		wndParent.SendMessage(WM_USER_UPDATE_LAYOUT);
	}
}


void CPluginCustomTabContainerView::OnRemoveTab(IUOLMessengerPluginCustomTabPtr pTab)
{
	BOOL bRemoved = FALSE;
	TabInfo* pTabInfo = NULL;
	CString strRemovedId = GetTabId(pTab);

	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; m_listTabs.GetNext(pos))
	{
		pTabInfo = m_listTabs.GetAt(pos);
		
		CString strTabId = GetTabId(pTabInfo->pTab);
		
		if (0 == strRemovedId.Compare(strTabId))
		{
			CWindow wndTab = pTabInfo->hWndTab;

			if ((m_wndTrackingWindow.IsWindow()) &&
				(wndTab.IsChild(m_wndTrackingWindow)))
			{
			
				HWND hWndOld  = m_wndTrackingWindow.UnsubclassWindow();
				ATLASSERT(hWndOld != NULL);

				if (m_pPluginTooltip)
				{
					m_pPluginTooltip->SetTooltipProvider(IUOLMessengerTooltipProviderPtr());
				}
			}

			pTab->DestroyTab(pTabInfo->hWndTab);
			
			m_listTabs.RemoveAt(pos);
			m_mapTabs.RemoveKey(pTabInfo->hWndTab);
			delete pTabInfo;
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


void CPluginCustomTabContainerView::OnShowTab(IUOLMessengerPluginCustomTabPtr pTab)
{
	TabInfo* pTabInfo = NULL;
	CWindow wndTab;
	CImageButtonPtr pButton = NULL;
	int nTabIndex = 1;	// Starts with 1 because the contactlisttab is always the "0" index tab.
	
	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; m_listTabs.GetNext(pos))
	{
		pTabInfo = m_listTabs.GetAt(pos);
		wndTab = pTabInfo->hWndTab;

		if (wndTab == pTab->GetTabHWND()) {
			
			ATLASSERT(wndTab.IsWindow());
			
			if (wndTab.IsWindow())
			{
				if (this->IsWindow()) {
					
					this->ShowWindow(SW_SHOW);
				}
				
				wndTab.ShowWindow(SW_HIDE); //To force focus treatement
				wndTab.ShowWindow(SW_SHOWNORMAL);
				
				if ((m_pTabViewCtrl.get()) && (m_pTabViewCtrl->IsWindow()))
				{
					m_pTabViewCtrl->SetActiveTab(nTabIndex);
				}
				
				CWindow wndParent = GetParent();
				wndParent.SendMessage(WM_USER_UPDATE_LAYOUT);

			}
		}
		else {
			
			ATLASSERT(wndTab.IsWindow());
			
			if (wndTab.IsWindow())
			{
				wndTab.ShowWindow(SW_HIDE);
			}
		}
		
		nTabIndex++;
	}
}


void CPluginCustomTabContainerView::CreateTabs()
{
	CRect rectClient;
	GetClientRect(&rectClient);
	
	CAtlList<IUOLMessengerPluginCustomTabPtr> listTabs;
	m_pTabContainer->GetTabs(listTabs);
	
	for (POSITION pos = listTabs.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomTabPtr pTab = listTabs.GetNext(pos);

		CSize sizeMin;
		pTab->GetMinSize(sizeMin);

		CWindow wndTab = pTab->CreateTab(m_hWnd, 
				CRect(rectClient.left, rectClient.top, rectClient.right, rectClient.bottom), 
						WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

		ATLASSERT(wndTab.IsWindow());
		
		if (wndTab.IsWindow())
		{
			//wndTab.ShowWindow(SW_SHOWNORMAL);
			
			TabInfo* pTabInfo = new TabInfo();
			pTabInfo->pTab = pTab;
			pTabInfo->hWndTab = static_cast<HWND>(wndTab);

			m_listTabs.AddTail(pTabInfo);
			m_mapTabs.SetAt(static_cast<HWND>(wndTab), pTabInfo);
		}
	}
}


void CPluginCustomTabContainerView::DestroyTabs()
{
	TabInfo* pTabInfo;

	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; )
	{
		pTabInfo = m_listTabs.GetNext(pos);

		IUOLMessengerPluginCustomTabPtr pTab = pTabInfo->pTab;
		pTab->DestroyTab(pTabInfo->hWndTab);
		delete pTabInfo;
	}

	m_listTabs.RemoveAll();
	m_mapTabs.RemoveAll();

	m_pPluginTooltip->SetTooltipProvider(IUOLMessengerTooltipProviderPtr());
}


CString CPluginCustomTabContainerView::GetTabId(IUOLMessengerPluginCustomTabPtr pTab)
{
	CString strId;

	strId.Format(_T("%s/%s"), pTab->GetPluginInternalName(), pTab->GetTabName());

	return strId;
}


void CPluginCustomTabContainerView::OnSizeImpl(const CSize& size)
{
	TabInfo* pTabInfo;

	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; )
	{
		pTabInfo = m_listTabs.GetNext(pos);
		
		CWindow wndTab = pTabInfo->hWndTab;
		
		ATLASSERT(wndTab.IsWindow());
		wndTab.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER);
	}
}


void CPluginCustomTabContainerView::TrackMouseMove(HWND hWndChild)
{
	//ATLTRACE(_T("%s: pt(%d,%d)\n"), __FUNCTION__, pt.x, pt.y);

	if (hWndChild != m_wndTrackingWindow)
	{
		HWND hWndTab = hWndChild;

		while ((hWndTab != NULL) && (::GetParent(hWndTab) != m_hWnd))
		{
			hWndTab = ::GetParent(hWndTab);
		}

		CTabMap::CPair* pPair = m_mapTabs.Lookup(hWndTab);
		if (pPair)
		{
			TabInfo* pTabInfo = pPair->m_value;

			if (pTabInfo != m_pTooltipTabInfo)
			{
				// Tooltip changed...
				m_pTooltipTabInfo = pTabInfo;

				//ATLTRACE(_T("%s New tooltip = %s\n"), __FUNCTION__, GetTabId(pTabInfo->pTab));
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


void CPluginCustomTabContainerView::HideAllTabs()
{
	TabInfo* pTabInfo = NULL;
	CImageButtonPtr pButton = NULL;
	
	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; )
	{
		pTabInfo = m_listTabs.GetNext(pos);
		
		CWindow wndTab = pTabInfo->hWndTab;
		
		ATLASSERT(wndTab.IsWindow());
		
		wndTab.ShowWindow(SW_HIDE);
	}
}

