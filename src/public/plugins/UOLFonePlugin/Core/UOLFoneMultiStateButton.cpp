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
 

#include "StdAfx.h"
#include ".\UOLFoneMultiStateButton.h"


#define		CLICK_SIMULATION_TIMER_ID				1234

#define		CLICK_SIMULATION_TIMER_TIMEOUT			100


/////////////////////////////////////////////////////////////////////
// CUOLFoneMultiStateButton public members
/////////////////////////////////////////////////////////////////////


CUOLFoneMultiStateButton::CUOLFoneMultiStateButton(void) : 
		m_bIsMouseOver(FALSE),
		m_bIsMouseLButtonDown(FALSE),
		m_bIsMultiStateBitmap(TRUE), 
		m_clickSimulationTimer(NULL), 
		m_isSimulatingClick(FALSE), 
		m_charForwardingWnd(NULL),
		m_bButtonIsLink(FALSE)
{
}


CUOLFoneMultiStateButton::~CUOLFoneMultiStateButton(void)
{
	if (!m_buttonBitmap.IsNull())
	{
		m_buttonBitmap.DeleteObject();
	}
}


LPCTSTR CUOLFoneMultiStateButton::GetWndClassName()
{
	return _T("CUOLFoneMultiStateButton");
}


void CUOLFoneMultiStateButton::SetMonoStateBitmap(int bitmapResourceID)
{
	if (!m_buttonBitmap.IsNull())
	{
		m_buttonBitmap.DeleteObject();
	}
	
	m_buttonBitmap.LoadBitmap(bitmapResourceID);
	
	m_bIsMultiStateBitmap = FALSE;
	
	Invalidate();
}


void CUOLFoneMultiStateButton::SetMultiStateBitmap(int bitmapResourceID)
{
	if (!m_buttonBitmap.IsNull())
	{
		m_buttonBitmap.DeleteObject();
	}
	
	m_buttonBitmap.LoadBitmap(bitmapResourceID);
	
	m_bIsMultiStateBitmap = TRUE;
	
	Invalidate();
}


int CUOLFoneMultiStateButton::GetWidth()
{
	int width = 0;
	SIZE buttonSize;
	
	if (m_buttonBitmap.GetSize(buttonSize))
	{
		width = buttonSize.cx;
	}
	
	if (m_bIsMultiStateBitmap)
	{
		width /= 3;
	}
	
	return width;
}


int CUOLFoneMultiStateButton::GetHeight()
{
	int height = 0;
	SIZE buttonSize;
	
	if (m_buttonBitmap.GetSize(buttonSize))
	{
		height = buttonSize.cy;
	}
	
	return height;
}


void CUOLFoneMultiStateButton::SimulateButtonClick()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	
	HDC hDC = GetWindowDC();
	
	m_isSimulatingClick = TRUE;
	
	DrawBitmap(hDC, clientRect);
	
	ReleaseDC(hDC);
	
	if (m_clickSimulationTimer != NULL)
	{
		// This should never happen.
		KillTimer(m_clickSimulationTimer);
		m_clickSimulationTimer = NULL;
	}
	
	// Start timer
	m_clickSimulationTimer = (UINT_PTR) SetTimer(
			CLICK_SIMULATION_TIMER_ID, CLICK_SIMULATION_TIMER_TIMEOUT);
	
	ATLASSERT(m_clickSimulationTimer != NULL);
}


void CUOLFoneMultiStateButton::SetCharForwardingWnd(HWND charForwardingWnd)
{
	m_charForwardingWnd = charForwardingWnd;
}



void CUOLFoneMultiStateButton::SetButtonIsLink(BOOL bIsLink)
{
	m_bButtonIsLink = bIsLink;
}


/////////////////////////////////////////////////////////////////////
// CUOLFoneMultiStateButton protected members
/////////////////////////////////////////////////////////////////////


LRESULT CUOLFoneMultiStateButton::OnDrawItem(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	
	HDC hdcMem = ::CreateCompatibleDC(lpdis->hDC);
    HBITMAP hbmMem = ::CreateCompatibleBitmap(lpdis->hDC,
			lpdis->rcItem.right - lpdis->rcItem.left,
			lpdis->rcItem.bottom - lpdis->rcItem.top);
    
	HGDIOBJ hbmOld = ::SelectObject(hdcMem, (HGDIOBJ)hbmMem);
	
	HBRUSH buttonFace = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	
	RECT rect =
	{
		lpdis->rcItem.left, 
		lpdis->rcItem.top, 
		lpdis->rcItem.right, 
		lpdis->rcItem.bottom
	};
	
	FillRect(hdcMem , &rect, buttonFace);
	
	POINT mouse_position;
	if ( (m_bIsMouseLButtonDown) && (::GetCapture() == m_hWnd) && 
			(::GetCursorPos(&mouse_position)) )
	{
		if (::WindowFromPoint(mouse_position) == m_hWnd)
		{
			if ((GetState() & BST_PUSHED) != BST_PUSHED)
			{
				SetState(TRUE);
				return -1;
			}
		}
		else 
		{
			if ((GetState() & BST_PUSHED) == BST_PUSHED)
			{
				SetState(FALSE);
				return -1;
			}
		}
	}
	
	if (lpdis->itemState & ODS_SELECTED)
	{
		if (m_buttonBitmap.IsNull())
		{
			FillRect(hdcMem, &rect, buttonFace); 
		}
		else
		{
			DrawBitmap(hdcMem, rect); 
		}
	}
	else
	{
		if (m_buttonBitmap.IsNull())
		{
			FillRect(hdcMem, &rect, buttonFace); 
		}
		else
		{
			DrawBitmap(hdcMem, rect);
		}
	}
	
	int cx = rect.right  - rect.left;
	int cy = rect.bottom - rect.top;
	
	StretchBlt(lpdis->hDC, rect.left, rect.top, cx, cy, hdcMem, 0, 0, 
			GetWidth(), GetHeight(), SRCCOPY);
	
	::DeleteObject(buttonFace);
	::SelectObject(hdcMem, hbmOld);
    ::DeleteObject((HGDIOBJ)hbmMem);
    ::DeleteDC(hdcMem);
	
    return 0;
}


LRESULT CUOLFoneMultiStateButton::OnMouseMove(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( (!m_bIsMouseOver) || (::GetCapture() != m_hWnd) )
	{
		TRACKMOUSEEVENT t = 
		{
			sizeof(TRACKMOUSEEVENT),
				TME_LEAVE,
				m_hWnd,
				0
		};
		
		if (::_TrackMouseEvent(&t))
		{
			m_bIsMouseOver = TRUE;
			
			Invalidate();
		}
	}
	
	bHandled = FALSE;
	
	return 0L;
}


LRESULT CUOLFoneMultiStateButton::OnMouseLeave(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT(m_bIsMouseOver);
	
	m_bIsMouseOver = FALSE;
	
	Invalidate();
	
	return 0;
}


LRESULT CUOLFoneMultiStateButton::OnLButtonDown(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bIsMouseOver)
	{
		TRACKMOUSEEVENT t = 
		{
			sizeof(TRACKMOUSEEVENT),
			TME_CANCEL | TME_LEAVE,
			m_hWnd,
			0
		};
		
		if (::_TrackMouseEvent(&t))
		{
			m_bIsMouseOver = FALSE;
		}
	}
	
	
	m_bIsMouseLButtonDown = TRUE;
	
	bHandled = FALSE;
	
	return 0L;
}


LRESULT CUOLFoneMultiStateButton::OnLButtonUP(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bIsMouseLButtonDown = FALSE;
	
	bHandled = FALSE;
	
	return 0L;
}


LRESULT CUOLFoneMultiStateButton::OnEraseBkgnd(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1L;
}


LRESULT CUOLFoneMultiStateButton::OnTimer(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_clickSimulationTimer != NULL)
	{
		// This should never happen.
		KillTimer(m_clickSimulationTimer);
		m_clickSimulationTimer = NULL;
	}
	
	CRect clientRect;
	GetClientRect(&clientRect);
	
	HDC hDC = GetWindowDC();
	
	m_isSimulatingClick = FALSE;
	
	DrawBitmap(hDC, clientRect);
	
	ReleaseDC(hDC);
	
	return 0L;
}


LRESULT CUOLFoneMultiStateButton::OnChar(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_charForwardingWnd)
	{
		::SendMessage(m_charForwardingWnd, uMsg, wParam, lParam);
	}
	
	return 0L;
}


LRESULT CUOLFoneMultiStateButton::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HCURSOR hCursor = LoadCursor(0, IDC_ARROW);

	if (m_bButtonIsLink)
	{
		hCursor = LoadCursor(0, IDC_HAND);
	}
	
	if(hCursor)
	{
		::SetCursor(hCursor);
	}
	
	return 0L;
}


void CUOLFoneMultiStateButton::DrawBitmap(HDC hdc, RECT& rc)
{
	int xShift = 0;
	
	int cx = rc.right  - rc.left;
	int cy = rc.bottom - rc.top;
	
	if (m_bIsMultiStateBitmap)
	{
		if (m_isSimulatingClick)
		{
			xShift += GetWidth();
		}
		else if (m_bIsMouseLButtonDown)
		{
			xShift += GetWidth();
		}
		else if (m_bIsMouseOver)
		{
			xShift += ( 2 * GetWidth() );
		}
	}
	
	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HGDIOBJ hbmOld = ::SelectObject(hdcMem, m_buttonBitmap.m_hBitmap);
	
	StretchBlt(hdc, rc.left, rc.top, cx, cy, hdcMem, xShift, 0, GetWidth(), GetHeight(), SRCCOPY);
	
	::SelectObject(hdcMem, hbmOld);
	::DeleteDC(hdcMem);
}


UINT CUOLFoneMultiStateButton::GetState() const
{
	ATLASSERT(::IsWindow(m_hWnd));
	
	return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
}


void CUOLFoneMultiStateButton::SetState(BOOL bPushed)
{
	ATLASSERT(::IsWindow(m_hWnd));
	
	::SendMessage(m_hWnd, BM_SETSTATE, bPushed, 0L);
}