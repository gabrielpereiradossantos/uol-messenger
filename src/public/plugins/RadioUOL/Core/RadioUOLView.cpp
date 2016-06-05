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
 

// RadioUOLView.cpp : implementation of the CRadioUOLView class
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RadioUOLView.h"
#include "radiouolview.h"
#include "windows.h"
#include "membm.h"
#include "..\resource.h"

CRadioUOLView::CRadioUOLView():
	m_lastMouseOver(NULL),
	m_lastMouseClicked(FALSE),
	m_bShowBuy(FALSE),
	m_bMouseCap(FALSE),
	m_bkColor(RGB(123,160,209))
{
}

CRadioUOLView::~CRadioUOLView() 
{
}


BOOL CRadioUOLView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CRadioUOLView::ShowBuyButton(BOOL enable)
{
	if (enable!=m_bShowBuy)
	{
		m_bShowBuy = enable;
		m_widBuy->SetVisible(enable);
		UpdateWidgets();
		Invalidate();
	}
}

void CRadioUOLView::EnablePopup(BOOL enable)
{
	m_widSyles->SetEnabled(enable);
	Invalidate();
}


void CRadioUOLView::UpdateWidgets()
{
	CRect m_rect;
	GetClientRect(m_rect);
	//m_rect.Width

	m_rectTop =  CRect(0,1,m_rect.Width(),21);
	m_rectMiddle = CRect(0,22,m_rect.Width(),46);
	m_rectBottom = CRect(0,47,m_rect.Width(),75);


	int dx;
	dx = 0;

	//m_widSyles->SetRect(CRect(m_rectTop.right-11, m_rectTop.top+6, m_rectTop.right-2,m_rectTop.top+15));
	
	m_widSearch->SetRect  (CRect(CPoint(m_rectTop.right-15-41-37, 1), CSize(37, 21)));
	m_widChannels->SetRect(CRect(CPoint(m_rectTop.right-15-41, 1), CSize(41, 21)));
	m_widSyles->SetRect   (CRect(CPoint(m_rectTop.right-15, 1), CSize(15, 21)));
	

	if (m_widBuy->GetVisible()==TRUE) 
	{
		dx = 67;
		m_widSong->SetRect(CRect(0, m_rectMiddle.top,m_rectMiddle.right-dx,m_rectMiddle.bottom));
		m_widBuy->SetRect(CRect(m_rectMiddle.right-dx,m_rectMiddle.top+1,m_rectMiddle.right,m_rectMiddle.bottom-1));
	}
	else
	{
		m_widSong->SetRect(CRect(1, m_rectMiddle.top,m_rectMiddle.right,m_rectMiddle.bottom));
	}
}


LRESULT CRadioUOLView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	CRect rect;
	GetClientRect(rect);
	CMemoryDC mem(dc,rect);
	CDCHandle hCDC(mem);
	mem.FillSolidRect(rect,m_bkColor);
	m_pFillTopBitmap->Draw(hCDC, 0, 1, rect.Width(), 21,  m_pDrawStrategyOpaque);
	m_pRadioUOLBitmap->Draw(hCDC, 0, 1, m_pRadioUOLBitmap->GetWidth(), m_pRadioUOLBitmap->GetHeight(), m_pDrawStrategyTrans);
	DrawWidgets(hCDC);
	return 0;
}

LRESULT CRadioUOLView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint m_StartPoint;
	BOOL find=FALSE;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 
	CRect rect;
	GetClientRect(rect);

	//ATLTRACE("\n%d %d - %d %d - %d %d",m_StartPoint.x, m_StartPoint.y,rect.bottom, rect.right, m_bMouseCap,rect.PtInRect(m_StartPoint));

	POSITION pos = m_widgets.GetHeadPosition();
	CWidget * tmpWidget = NULL;
	while (pos!=NULL) 
	{
		tmpWidget = m_widgets.GetNext(pos);
		if (tmpWidget->PtInRect(m_StartPoint))
		{
			if ((m_lastMouseOver!=NULL) && (m_lastMouseOver!=tmpWidget))
			{
				m_lastMouseOver->OnMouseLeave();
			}
			find = TRUE;
			tmpWidget->OnMouseOver();
			m_lastMouseOver = tmpWidget;
			Invalidate(FALSE);

			TRACKMOUSEEVENT mouseevent;
			mouseevent.cbSize=sizeof(TRACKMOUSEEVENT);
			mouseevent.dwFlags=TME_LEAVE;
			mouseevent.dwHoverTime=0;
			mouseevent.hwndTrack=m_hWnd;
			TrackMouseEvent(&mouseevent);
			
			break;
		}
	}

	if ((find==FALSE)&&(m_lastMouseOver!=NULL))
	{
		m_lastMouseOver->OnMouseLeave();
		m_lastMouseOver = NULL;
		Invalidate(FALSE);
	}

	
	return 0;
}


LRESULT CRadioUOLView::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE("%s\n\n",__FUNCTION__);
	CPoint m_StartPoint;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 

	POSITION pos = m_widgets.GetHeadPosition();
	CWidget * tmpWidget;
	while (pos!=NULL) 
	{
		tmpWidget = m_widgets.GetNext(pos);
		if (tmpWidget->PtInRect(m_StartPoint))
		{
			tmpWidget->OnMouseDown();
			m_lastMouseClicked = tmpWidget;
			SetCapture();
			Invalidate(FALSE);
			break;
		}

	}
	
	return 0;
}

LRESULT CRadioUOLView::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE("%s\n\n",__FUNCTION__);
	CPoint m_StartPoint;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 
	BOOL find=FALSE;
	POSITION pos = m_widgets.GetHeadPosition();
	CWidget * tmpWidget;
	while (pos!=NULL) 
	{
		tmpWidget = m_widgets.GetNext(pos);
		if (tmpWidget->PtInRect(m_StartPoint))
		{
			find = TRUE;
			tmpWidget->OnMouseUp();
			if (tmpWidget==m_lastMouseClicked)
			{
				tmpWidget->OnMouseClick(m_StartPoint);
			}
			break;
		}
	}
	
	if (m_lastMouseClicked)
	{
		if ((tmpWidget!=m_lastMouseClicked)||(!find))
		{
			m_lastMouseClicked->OnMouseCancelClick();
		} 
	}
	ReleaseCapture();

	Invalidate(FALSE);
	return 0;
}

LRESULT CRadioUOLView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	CreateWidgets();
	return 0;
}

LRESULT CRadioUOLView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POSITION pos = m_widgets.GetHeadPosition();
	CWidget * tmpWidget;
	while (pos!=NULL) 
	{
		tmpWidget = m_widgets.GetNext(pos);
		delete tmpWidget;
	}
	m_widgets.RemoveAll();

	return 0;
}

void CRadioUOLView::DrawWidgets(CDCHandle& dc)
{
	POSITION pos = m_widgets.GetHeadPosition();
	CWidget * tmpWidget;
	while (pos!=NULL) 
	{
		tmpWidget = m_widgets.GetNext(pos);
		tmpWidget->Draw(dc);
	}
}


void CRadioUOLView::CreateWidgets()
{
	IUOLMessengerUIManagerPtr pUIManager = CFacadeWrapper::GetFacadeInstance()->GetUIManager();

	if (pUIManager)
	{
		HINSTANCE hInst = ATL::_AtlBaseModule.GetModuleInstance();

		m_pSplitterBitmap = pUIManager->LoadImage(hInst, IDB_SPLITTER);
		m_pFillTopBitmap  = pUIManager->LoadImage(hInst, IDB_FILLTOP);
		m_pRadioUOLBitmap = pUIManager->LoadImage(hInst, IDB_RADIO_UOL_BAR);
		//m_pDrawStrategy   = pUIManager->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		m_pDrawStrategyTrans  = pUIManager->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		m_pDrawStrategyOpaque = pUIManager->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_OPAQUE_STRETCHED);
		


		// Song Title
		m_widSong = new CWidgetSong();
		m_widSong->SetRect(CRect(0,100,400,200));
		m_widSong->SetTitle(_T(""),"");
		m_widgets.AddTail(m_widSong);

		// Channels
		m_widChannels = new CWidgetButton( 
			pUIManager->LoadImage(hInst, IDB_CHANNELS),
			pUIManager->LoadImage(hInst, IDB_CHANNELS_OVER),
			pUIManager->LoadImage(hInst, IDB_CHANNELS_CLICKED),
			pUIManager->LoadImage(hInst, IDB_CHANNELS_OFF), 
			WMPChannels, FALSE
			);

		m_widChannels->SetRect(CRect(210,1,250,50));
		m_widChannels->SetHelp("Canais");
		m_widgets.AddTail(m_widChannels);

		// Search
		m_widSearch = new CWidgetButton( 
			pUIManager->LoadImage(hInst, IDB_SEARCH),
			pUIManager->LoadImage(hInst, IDB_SEARCH_OVER),
			pUIManager->LoadImage(hInst, IDB_SEARCH_CLICKED),
			pUIManager->LoadImage(hInst, IDB_SEARCH_OFF), 
			WMPSearch, FALSE
			);

		m_widSearch->SetRect(CRect(260,1,300,50));
		m_widSearch->SetHelp("Busca");
		m_widgets.AddTail(m_widSearch);

		// Styles
		m_widSyles = new CWidgetButton(
			pUIManager->LoadImage(hInst, IDB_POPUP),
			pUIManager->LoadImage(hInst, IDB_POPUP_OVER),
			pUIManager->LoadImage(hInst, IDB_POPUP_CLICKED),
			pUIManager->LoadImage(hInst, IDB_POPUP_OFF), 
			WMPStyles, FALSE);

		m_widSyles->SetRect(CRect(260,1,300,50));
		m_widSyles->SetHelp("Styles");
		m_widSyles->SetEnabled(TRUE);
		m_widgets.AddTail(m_widSyles);

		//// Buy button
		m_widBuy = new CWidgetButton(
			pUIManager->LoadImage(hInst, IDB_BUY),
			pUIManager->LoadImage(hInst, IDB_BUY_OVER),
			pUIManager->LoadImage(hInst, IDB_BUY_CLICKED),
			pUIManager->LoadImage(hInst, IDB_BUY_OFF), 

			WMPBuy);

		m_widBuy->SetVisible(FALSE);
		m_widBuy->SetRect(CRect(400,300,450,350));
		m_widBuy->SetHelp("Comprar");
		m_widgets.AddTail(m_widBuy);
	}

}
LRESULT CRadioUOLView::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//ATLTRACE("%s\n\n",__FUNCTION__);
	return 0;
}



LRESULT CRadioUOLView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UpdateWidgets();

	return 0;
}

LRESULT CRadioUOLView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default

	return 0;
}



//void CRadioUOLView::OnFinalMessage(HWND /*hWnd*/)
//{
//   //::PostQuitMessage(0);
//}


void CRadioUOLView::SetSongTitle(const CString & title, const CString & duration)
{
	m_widSong->SetTitle(title, duration);
}

LRESULT CRadioUOLView::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_lastMouseOver!=NULL) 
	{
		m_lastMouseOver->OnMouseLeave();
		m_lastMouseOver=NULL;
		Invalidate(FALSE);
	}
	bHandled = FALSE;
	return 0;
}