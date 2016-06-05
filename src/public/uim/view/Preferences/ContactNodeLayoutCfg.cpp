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
#include "../../resource.h"
#include "ContactNodeLayoutCfg.h"

/////////////////////////////////////////////////
// CContactNodeLayoutElement
CContactNodeLayoutElement::CContactNodeLayoutElement(void)
{
	m_caption = "";
	m_strBitmapPath = "";
	m_fixed = FALSE;
	m_id = 0;
	m_toolbarButton.fsState = TBSTATE_ENABLED; 
	m_toolbarButton.fsStyle = BTNS_BUTTON; 
	m_toolbarButton.idCommand = 0;
}

CContactNodeLayoutElement::~CContactNodeLayoutElement(void)
{
}

void CContactNodeLayoutElement::SetCaption(CString caption)
{
	m_caption = caption;
	LPSTR pCaption;
	pCaption = m_caption.GetBuffer(0);
	m_caption.ReleaseBuffer();
	m_toolbarButton.iString = (int) pCaption;
}

CString CContactNodeLayoutElement::GetCaption(void)
{
	return m_caption;
}

void CContactNodeLayoutElement::SetBitmapPath(CString bmpPath)
{
	m_strBitmapPath = bmpPath;
}

CString CContactNodeLayoutElement::GetBitmapPath(void)
{
	return m_strBitmapPath;
}

void CContactNodeLayoutElement::SetBitmapIndex(int bmpIndex)
{
    m_bitmapIndex = bmpIndex;
    m_toolbarButton.iBitmap = m_bitmapIndex;
}

int CContactNodeLayoutElement::GetBitmapIndex(void)
{
    return m_bitmapIndex;
}

void CContactNodeLayoutElement::SetID(int id)
{
	m_id = id;
	m_toolbarButton.dwData = m_id;
}

int CContactNodeLayoutElement::GetID(void)
{
	return m_id;
}

LPTBBUTTON CContactNodeLayoutElement::GetToolBartButton(void)
{
	return &m_toolbarButton;
}

void CContactNodeLayoutElement::SetFixed(BOOL fixed)
{
	m_fixed = fixed; 
}

BOOL CContactNodeLayoutElement::IsFixed(void)
{
	return m_fixed;
}

/////////////////////////////////////////////////
// CToolBarCtrlHelper
CToolBarCtrlHelper::CToolBarCtrlHelper(void)
{
	m_nDragButton = -1;
	m_bDragging = FALSE;
	m_bCanDelete = FALSE;
	m_lastRect = (0,0,0,0);
	m_hOrigCursor = 0;
	m_hDragCursor = 0;
	m_hNoDragCursor	= 0;
	m_hDeleteCursor = 0;
	m_bDragging	= FALSE;
	m_bDragCursor = FALSE;
	m_bDeleteCursor	= FALSE;
	m_bNoDragCursor	= FALSE;
}

CToolBarCtrlHelper::~CToolBarCtrlHelper(void)
{
    m_elements.RemoveAll();
}

void CToolBarCtrlHelper::BeginDrag(void)
{
	int nBtn = GetButtonCount();
	m_nLastButton = -1;
	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);

	m_nDragButton = -1;

	// have a look for whether the button is valid and
	// - if so - draw a dragging border around it.
	BOOL bHasCursor;
	for (int i = 0 ; i < nBtn ; ++i) 
	{
		CRect rc;
		GetItemRect(i, rc);

		bHasCursor = rc.PtInRect(pt);

		if (bHasCursor) 
		{
			m_nDragButton = i;
			CClientDC cdc(m_hWnd);
			cdc.DrawDragRect(rc, CSize(2,2), 0, CSize(0,0));
			break;
		}
	}

	if (m_nDragButton < 0)
	{
		return;		// nothing to drag ...
	}

	m_hDragCursor = ::CreateCursor(0, 4, 2, 32, 32, ANDmaskDrop, XORmaskDrop);
	m_hNoDragCursor = ::CreateCursor(0, 4, 2, 32, 32, ANDmaskNoDrop, XORmaskNoDrop);
	m_hDeleteCursor = ::CreateCursor(0, 4, 2, 32, 32, ANDmaskDelete, XORmaskDelete);

	// capture the mouse during the drag operation
	SetCapture();

	// make sure we receive keyboard-input
	SetFocus();

	m_hOrigCursor = ::SetCursor(m_hDragCursor);
	m_bCanDelete = FALSE;
	m_bDragging = TRUE;

	::GetCursorPos(&pt);
}

LRESULT CToolBarCtrlHelper::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndDrag(m_bDragging);
	m_bDragging = FALSE;
	return TRUE;
}

LRESULT CToolBarCtrlHelper::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BeginDrag();
	return TRUE;
}

void CToolBarCtrlHelper::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging) 
	{
		DragMove();
		return;
	}

	if (GetTopLevelParent().IsWindowEnabled())
	{
		int nBtn = GetButtonCount();
		int nLastBtn = m_nLastButton;
		m_nLastButton = -1;

		for (int i = 0 ; i < nBtn ; ++i)
		{
			CRect rc;
			GetItemRect(i, rc);

			BOOL bBtnEnabled = IsButtonEnabled(i);
			if(!bBtnEnabled)
				continue;

			BOOL bHasCursor = rc.PtInRect(point);

			if (bHasCursor && bBtnEnabled) 
			{
				if (nLastBtn != i)
				{
					// force a repaint of the button with the cursor on it
					InvalidateRect(rc, FALSE);
				}
				m_nLastButton = i;
			} 
			else if (!bHasCursor && i == nLastBtn)
			{
				// force a repaint of the last formed button
				InvalidateRect(rc, FALSE);
			}
		}
		// One problem occures with WM_MOUSEMOVE: we cannot detect
		// that the mouse leaves the window. If the mouse moves quick
		// enough, then the last formed button stays visible. To
		// resolve this problem, we set a timer and check, whether
		// the mouse is outside the window ...
		KillTimer(m_uTimerEvent);
		m_uTimerEvent = SetTimer(1, 250, 0);
	}
}

int CToolBarCtrlHelper::FindDropButton(CPoint& point)
{
	CPoint pt = point;
	ScreenToClient(&pt);

	CRect rc;
	// find the button which is closest to the cursor
	int nBtn = GetButtonCount();
	for (int i = 0 ; i < nBtn ; ++i)
	{
		GetItemRect(i, rc);
		if (rc.PtInRect(pt))
			// insert the button to drop before this button
			return (pt.x - rc.left < rc.right - pt.x)
					? i
					: ((i==nBtn-1) ? -1	: i+1);
	}
	
	// have to append the button
	return nBtn-1;
}

void CToolBarCtrlHelper::GetMarkerRect(int nButton, CRect& rc)
{
	int nBtn = GetButtonCount();
	if (nButton < 0 || nButton > nBtn) 
	{
		// set the marker behind the last button
		GetItemRect(nBtn-1, rc);
		rc.right += 3;
		rc.left = rc.right-6;
	}
	else
	{
		// set the marker before the given button
		GetItemRect(nButton, rc);
		rc.left -= 3;
		rc.right = rc.left+6;
	}
	rc.DeflateRect(0,1);
}

void CToolBarCtrlHelper::ShowMarker(CRect& rcMarker, CBitmap& bmpArea)
{
	CClientDC WinDC(m_hWnd);
	CDC MemDC; 
	MemDC.CreateCompatibleDC(WinDC.m_hDC);

	bmpArea.CreateCompatibleBitmap(WinDC.m_hDC, rcMarker.Width(), rcMarker.Height());
	CBitmap oldBmp = MemDC.SelectBitmap(bmpArea.m_hBitmap);

	HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN oldPen = WinDC.SelectPen(pen);

	// save original area:
	MemDC.BitBlt(0, 0, rcMarker.Width(), rcMarker.Height(),
				WinDC.m_hDC, rcMarker.left, rcMarker.top, SRCCOPY);

	WinDC.MoveTo(rcMarker.TopLeft());
	WinDC.LineTo(rcMarker.right, rcMarker.top);
	WinDC.MoveTo(rcMarker.left+1, rcMarker.top+1);
	WinDC.LineTo(rcMarker.right-1, rcMarker.top+1);

	WinDC.MoveTo(rcMarker.left+2, rcMarker.top+2);
	WinDC.LineTo(rcMarker.left+2, rcMarker.bottom-2);
	WinDC.MoveTo(rcMarker.left+3, rcMarker.top+2);
	WinDC.LineTo(rcMarker.left+3, rcMarker.bottom-2);

	WinDC.MoveTo(rcMarker.left, rcMarker.bottom-1);
	WinDC.LineTo(rcMarker.right, rcMarker.bottom-1);
	WinDC.MoveTo(rcMarker.left+1, rcMarker.bottom-2);
	WinDC.LineTo(rcMarker.right-1, rcMarker.bottom-2);

	MemDC.SelectBitmap(oldBmp);
	MemDC.DeleteDC();
	WinDC.SelectPen(oldPen);
}

void CToolBarCtrlHelper::RestoreMarker(CRect& rcArea, CBitmap& bmpArea) 
{
	CClientDC WinDC(m_hWnd);
	CDC MemDC; 
	MemDC.CreateCompatibleDC(WinDC.m_hDC);

	CBitmap oldBmp = MemDC.SelectBitmap(bmpArea.m_hBitmap);
	WinDC.BitBlt(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(),
				MemDC.m_hDC, 0, 0, SRCCOPY);

	MemDC.SelectBitmap(oldBmp.m_hBitmap);
	MemDC.DeleteDC();
}

void CToolBarCtrlHelper::SetMarker(CPoint& point)
{
	CBitmap bmpLastSavedArea;

	CRect rcMarker;

	// retrieve proposed rectangle for the marker
	int nDropBtn = FindDropButton(point);
	GetMarkerRect(nDropBtn, rcMarker);

	if(rcMarker == m_lastRect)
		return;		// don't need to erase/draw
		
	// draw the marker
	ShowMarker(rcMarker, bmpLastSavedArea);
	
	// restore the previously marked area:
	RestoreMarker(m_lastRect, bmpLastSavedArea);
	if (bmpLastSavedArea)
	{
		bmpLastSavedArea.DeleteObject();
	}

	m_lastRect = rcMarker;
}

void CToolBarCtrlHelper::DragMove(void) 
{
	CPoint pt;
	::GetCursorPos(&pt);
	CWindow aWnd = WindowFromPoint(pt);

	// is the cursor moving over a toolbar?
	BOOL bToolBar =	IsValidDropTarget(&aWnd);

	// is the cursor moving over a listbox?
	BOOL bListBox =	IsValidDeleteTarget(&aWnd);
	
	// check whether we have to switch the cursor
	if (bToolBar) 
	{
		if (!m_bDragCursor)
			::SetCursor(m_hDragCursor);
		m_bDragCursor = TRUE;
		m_bDeleteCursor	= FALSE;
		m_bNoDragCursor	= FALSE;
		m_bCanDelete = FALSE;
	}
	else if (bListBox && !IsElementFixed(GetButtonID(m_nDragButton))) 
	{
		if (!m_bDeleteCursor)
			::SetCursor(m_hDeleteCursor);
		m_bDragCursor = FALSE;
		m_bDeleteCursor	= TRUE;
		m_bNoDragCursor	= FALSE;
		m_bCanDelete = TRUE;
	}
	else
	{
		if (!m_bNoDragCursor)
			::SetCursor(m_hNoDragCursor);
		m_bDragCursor = FALSE;
		m_bDeleteCursor	= FALSE;
		m_bNoDragCursor	= TRUE;
		m_bCanDelete = FALSE;
	}

	SetMarker(m_ptDrop = pt);
}

void CToolBarCtrlHelper::EndDrag(BOOL bDoMove) 
{
	// remove the marker
	SetMarker(CPoint(0,0));

	::SetCursor(m_hOrigCursor);
	::DestroyCursor(m_hDragCursor);
	::DestroyCursor(m_hNoDragCursor);
	::DestroyCursor(m_hDeleteCursor);
	m_hDragCursor = 0;
	m_hNoDragCursor = 0;
	m_hDeleteCursor = 0;

	if (m_nDragButton >= 0) {
		int nBtn = GetButtonCount();

		if (bDoMove && !m_bCanDelete) 
		{
			// have a look for where to drop the button
			int nDropBtn = FindDropButton(m_ptDrop);
			
			// get button array index
			int idxDrag = GetButtonID(m_nDragButton);
			
			CWindow wndParent = GetParent();
			if (nDropBtn >= 0)
				// have to insert				
				::SendMessage(wndParent.m_hWnd, WM_TOOLBAR_INSERT, idxDrag, nDropBtn); 
			else
				// append the button
				::SendMessage(wndParent.m_hWnd, WM_TOOLBAR_ADD, idxDrag, NULL); 

			// delete the button at its original location
			DeleteButton(
				(m_nDragButton < nDropBtn || nDropBtn < 0)
				? m_nDragButton
				: m_nDragButton+1
				);
		}
		else
		{
			// remove the button from the toolbar
			if (bDoMove && m_bCanDelete)
			{
				// Get button array index
				int idxDrag = GetButtonID(m_nDragButton);

				// Can not delete alias button
				if (!IsElementFixed(idxDrag))
				{				
					DeleteButton(m_nDragButton);
					CWindow wndParent = GetParent();
					::SendMessage(wndParent.m_hWnd, WM_TOOLBAR_DROP, idxDrag, NULL); 
				}

				if (m_nDragButton == GetButtonCount())
					// remove trailing separators too
					RemoveSeparators();
			}
		}
	}
	else
	{
		// User has aborted the drag-operation.
		// Remove the drag-border from the button
		InvalidateButton(m_nDragButton);
	}

	m_bDragging = FALSE;

	// mouse capture is not longer needed
	ReleaseCapture();

	RemoveSeparators();
	Invalidate(TRUE);
}

BOOL CToolBarCtrlHelper::IsValidDropTarget(CWindow* pWnd)
{
	if (pWnd->IsWindow())
	{
		// Get target type
		char targetTypeBuffer[BUFSIZ];
		GetClassName(pWnd->m_hWnd, targetTypeBuffer, sizeof(targetTypeBuffer)); 
		
		// Is target a toolbar?
		return (strcmp(targetTypeBuffer, _T("CToolBarCtrlHelper")) == 0);
	}
	else
	{
		return FALSE;
	}
}

BOOL CToolBarCtrlHelper::IsValidDeleteTarget(CWindow* pWnd)
{
	if (pWnd->IsWindow())
	{
		// Get target type
		char targetTypeBuffer[BUFSIZ];
		GetClassName(pWnd->m_hWnd, targetTypeBuffer, sizeof(targetTypeBuffer)); 
		
		if (strcmp(targetTypeBuffer, _T("CListBoxCtrlHelper")) == 0)
		{
			return true;
		}
		
		// Is target a listbox?
		return (strcmp(targetTypeBuffer, _T("CListBoxCtrlHelper")) == 0);
	}
	else
	{
		return FALSE;
	}
}

BOOL CToolBarCtrlHelper::IsSeparator(int iBtn)
{
	TBBUTTON tbButton;
	GetButton(iBtn, &tbButton);
	return (tbButton.fsStyle == BTNS_SEP);
}

void CToolBarCtrlHelper::RemoveSeparators(void)
{
	int nBtn = GetButtonCount();
	for (int i = nBtn - 1; i >= 0; i--)
	{
		if (IsSeparator(i))
		{
			DeleteButton(i);
		}
	}
}

void CToolBarCtrlHelper::InvalidateButton(int nIndex) 
{
	if ((nIndex < 0) || (nIndex >= GetButtonCount()))
		return;
	CRect rc;
	GetItemRect(nIndex, rc);
	InvalidateRect(rc, FALSE);
}

int	CToolBarCtrlHelper::GetButtonID(int button)
{
	TBBUTTON tbButton;			
	// Get button struct
	if (GetButton(button, &tbButton))
    {	
	    return tbButton.dwData;
    }
    else
    {
        return -1;
    }
}

void CToolBarCtrlHelper::DeleteAllButtons(void)
{
	int nBtn = GetButtonCount();
	for (int i = nBtn - 1; i >= 0; i--)
	{
		DeleteButton(i);
	}
}

void CToolBarCtrlHelper::AddElement(int id, BOOL fixed)
{
    m_elements.SetAt(id, fixed);
}

BOOL CToolBarCtrlHelper::IsElementFixed(int id)
{
    BOOL fixed = TRUE;
    m_elements.Lookup(id, fixed);
    return fixed;
}

/////////////////////////////////////////////////
// CListBoxCtrlHelper
CListBoxCtrlHelper::CListBoxCtrlHelper(void)
{
	m_nDragItem = -1;
	m_bDragging = FALSE;
	m_bDragCursor = FALSE;
	m_lastRect = (0,0,0,0);
	m_hOrigCursor = 0;
	m_hDragCursor = 0;
	m_hNoDragCursor	= 0;
	m_bDragging	= FALSE;
	m_bDragCursor = FALSE;
}

CListBoxCtrlHelper::~CListBoxCtrlHelper(void)
{
}

void CListBoxCtrlHelper::OnLButtonUp(UINT nFlags, CPoint point)
{
	EndDrag(m_bDragging);
	m_bDragging = FALSE;
}

void CListBoxCtrlHelper::OnLButtonDown(UINT nFlags, CPoint point)
{
	BeginDrag();
}

void CListBoxCtrlHelper::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging) 
	{
		DragMove();
		return;
	}

	if (GetTopLevelParent().IsWindowEnabled())
	{
		int nItem = GetCount();
		int nLastItem = m_nLastItem;
		m_nLastItem = -1;

		for (int i = 0 ; i < nItem ; ++i)
		{
			CRect rc;
			GetItemRect(i, rc);

			BOOL bHasCursor = rc.PtInRect(point);
			if (bHasCursor) 
			{
				if (nLastItem != i)
				{
					// force a repaint of the Item with the cursor on it
					InvalidateRect(rc, FALSE);
				}
				m_nLastItem = i;
			} 
			else if (!bHasCursor && i == nLastItem)
			{
				// force a repaint of the last formed Item
				InvalidateRect(rc, FALSE);
			}
		}
		// One problem occures with WM_MOUSEMOVE: we cannot detect
		// that the mouse leaves the window. If the mouse moves quick
		// enough, then the last formed button stays visible. To
		// resolve this problem, we set a timer and check, whether
		// the mouse is outside the window ...
		KillTimer(m_uTimerEvent);
		m_uTimerEvent = SetTimer(1, 250, 0);
	}
}

void CListBoxCtrlHelper::BeginDrag(void)
{
	int nItem = GetCount();
	m_nLastItem = -1;
	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);

	m_nDragItem = -1;

	// have a look for whether the Item is valid and
	// - if so - draw a dragging border around it.
	BOOL bHasCursor;
	for (int i = 0 ; i < nItem ; ++i) 
	{
		CRect rc;
		GetItemRect(i, rc);

		bHasCursor = rc.PtInRect(pt);

		if (bHasCursor) 
		{
			m_nDragItem = i;
			CClientDC cdc(m_hWnd);
			cdc.DrawDragRect(rc, CSize(2,2), 0, CSize(0,0));
			break;
		}
	}

	if (m_nDragItem < 0)
	{
		return;		// nothing to drag ...
	}

	m_hDragCursor = ::CreateCursor(0, 4, 2, 32, 32, ANDmaskDrop, XORmaskDrop);
	m_hNoDragCursor = ::CreateCursor(0, 4, 2, 32, 32, ANDmaskNoDrop, XORmaskNoDrop);

	// capture the mouse during the drag operation
	SetCapture();

	// make sure we receive keyboard-input
	SetFocus();

	m_hOrigCursor = ::SetCursor(m_hDragCursor);
	m_bDragCursor = TRUE;
	m_bDragging = TRUE;

	::GetCursorPos(&pt);
}

void CListBoxCtrlHelper::EndDrag(BOOL bDoMove) 
{
	::SetCursor(m_hOrigCursor);
	::DestroyCursor(m_hDragCursor);
	::DestroyCursor(m_hNoDragCursor);
	m_hDragCursor = 0;
	m_hNoDragCursor = 0;

	if (m_nDragItem >= 0) 
	{
		if (m_bDragCursor && bDoMove) 
		{
			// delete the Item at its original location
			int idxDrag;
			idxDrag = GetItemData(m_nDragItem);

			DeleteString(m_nDragItem);
			CWindow wndParent = GetParent();
			::SendMessage(wndParent.m_hWnd, WM_LISTBOX_DROP, idxDrag, NULL); 
		}
	}

	m_bDragging = FALSE;

	// mouse capture is not longer needed
	ReleaseCapture();

	Invalidate(TRUE);
}

void CListBoxCtrlHelper::DragMove(void) 
{
	CPoint pt;
	::GetCursorPos(&pt);
	CWindow aWnd = WindowFromPoint(pt);

	// is the cursor moving over an adjustable toolbar ?
	BOOL bToolBar =	IsValidDropTarget(&aWnd);
	
	// If the window under the cursor is not a toolbar, then
	// check whether this window is a child of a toolbar.
	while (!bToolBar && (aWnd = aWnd.GetParent()) != 0)
		bToolBar = IsValidDropTarget(&aWnd);

	// check whether we have to switch the cursor
	if (bToolBar && ! m_bDragCursor) 
	{
		::SetCursor(m_hDragCursor);
		m_bDragCursor = TRUE;
	}
	else if (!bToolBar && m_bDragCursor) 
	{
		::SetCursor(m_hNoDragCursor);
		m_bDragCursor = FALSE;
	}
}

BOOL CListBoxCtrlHelper::IsValidDropTarget(CWindow* pWnd)
{
	if (pWnd->IsWindow())
	{
		// Get target type
		char targetTypeBuffer[BUFSIZ];
		GetClassName(pWnd->m_hWnd, targetTypeBuffer, sizeof(targetTypeBuffer)); 
		
		// Is target a toolbar?
		return (strcmp(targetTypeBuffer, _T("CToolBarCtrlHelper")) == 0);
	}
	else
	{
		return FALSE;
	}
}

/////////////////////////////////////////////////
// CContactNodeLayoutCfg
CContactNodeLayoutCfg::CContactNodeLayoutCfg(void)
{
    m_bitmapIndex = 0;
}

CContactNodeLayoutCfg::~CContactNodeLayoutCfg(void)
{
    m_elements.RemoveAll();
}

BOOL CContactNodeLayoutCfg::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CContactNodeLayoutCfg::CreateControls()
{
	ATLASSERT(m_elements.GetCount() > 0);
	
    // Get client rect
	CRect clientRect;
	GetClientRect(&clientRect);

	// Create Toolbar
	DWORD dwStyle;
	DWORD dwStyleEx;
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		CCS_NODIVIDER | TBSTYLE_AUTOSIZE | TBSTYLE_LIST;
	CRect* toolbarRect = new CRect(CPoint(0, 0), CSize(clientRect.right, 0));
	m_toolbar.Create(m_hWnd, toolbarRect, NULL, dwStyle);
	m_toolbar.SetButtonStructSize(sizeof(TBBUTTON));
    m_toolbar.SetBitmapSize(CSize(24, 24));
	m_toolbar.ShowWindow(SW_SHOW);

	// Get toolbar rect
	m_toolbar.GetClientRect(toolbarRect);

	// Create Listbox
	dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | 
		LBS_NOINTEGRALHEIGHT;
	dwStyleEx = WS_EX_LEFT;
	int lbHeight = clientRect.bottom - toolbarRect->bottom;
	CRect* listboxRect = new CRect(CPoint(0, toolbarRect->bottom), CSize(clientRect.right, 50));
	m_listbox.Create(m_hWnd, listboxRect, NULL, dwStyle, dwStyleEx);
	m_listbox.ShowWindow(SW_SHOW);

	// Create image list
    m_ilButtons.Create(24, 24, ILC_COLOR32 | ILC_MASK, 0, 1);
    
    //Add elements to toolbar and listbox	
    int bmpidx = 0;
	CContactNodeLayoutElementPtr element = NULL;
	for (POSITION pos = m_elements.GetHeadPosition(); pos;)
	{
		element = m_elements.GetNext(pos);
        
        // Set imagelist
        HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, element->GetBitmapPath(),
                          IMAGE_BITMAP, 0, 0, 
                          LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
        if (hBitmap)
        {
	        m_ilButtons.Add(hBitmap, RGB(255, 0, 255));
        }
        DeleteObject(hBitmap);
        
        // Add elements to toolbar
        m_toolbar.AddElement(element->GetID(), element->IsFixed());
        
        // Add elements to listbox
        int idx;
		idx = m_listbox.AddString(element->GetCaption());
		m_listbox.SetItemData(idx, element->GetID());
	}

    //Add imagelist to toolbar
	m_toolbar.SetImageList(m_ilButtons);
}

LRESULT CContactNodeLayoutCfg::OnToolBarDrop(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int id = (int)wParam;

	// Add removed button from toolbar to listbox
    CContactNodeLayoutElementPtr element;
	element = FindElement(id);
    if (!element)
    {
        return FALSE;
    }
	int idx;
    idx = m_listbox.AddString(element->GetCaption());
	m_listbox.SetItemData(idx, id);
    m_listbox.SetFocus();

    //Notify parent window
    CWindow wndParent = GetParent();
	::SendMessage(wndParent.m_hWnd, WM_CONTACT_NODE_LAYOUT_CHANGED, NULL, NULL);
	
	return TRUE;
}

LRESULT CContactNodeLayoutCfg::OnToolBarAdd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int id = (int)wParam;

	// Add button to toolbar
	CContactNodeLayoutElementPtr element;
	element = FindElement(id);
    if (!element)
    {
        return FALSE;
    }
	m_toolbar.AddButtons(1, element->GetToolBartButton());
    m_listbox.SetFocus();

    //Notify parent window
    CWindow wndParent = GetParent();
	::SendMessage(wndParent.m_hWnd, WM_CONTACT_NODE_LAYOUT_CHANGED, NULL, NULL);

	return TRUE;
}

LRESULT CContactNodeLayoutCfg::OnToolBarInsert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int id = (int)wParam;
	int after = (int) lParam;

	// Add button to toolbar
	CContactNodeLayoutElementPtr element;
	element = FindElement(id);
    if (!element)
    {
        return FALSE;
    }
	m_toolbar.InsertButton(after, element->GetToolBartButton());
    m_listbox.SetFocus();

    //Notify parent window
    CWindow wndParent = GetParent();
	::SendMessage(wndParent.m_hWnd, WM_CONTACT_NODE_LAYOUT_CHANGED, NULL, NULL);

	return TRUE;
}

LRESULT CContactNodeLayoutCfg::OnListBoxDrop(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int id = (int)wParam;

	// Add removed button from listbox to toolbar
	CContactNodeLayoutElementPtr element;
	element = FindElement(id);
    if (!element)
    {
        return FALSE;
    }
	m_toolbar.AddButtons(1, element->GetToolBartButton());
    m_listbox.SetFocus();

    //Notify parent window
    CWindow wndParent = GetParent();
	::SendMessage(wndParent.m_hWnd, WM_CONTACT_NODE_LAYOUT_CHANGED, NULL, NULL);
	
	return TRUE;
}

CString CContactNodeLayoutCfg::GetContactNodeLayout(void)
{
	int id;
    CString result = "";

    int icount = m_elements.GetCount();
    for (int i = 0 ; i < icount ; ++i) 
	{
    	id = m_toolbar.GetButtonID(i);
        if (id != -1)
		{
            char buffer[8];
            _itoa(id, buffer, 10);

            if (result.IsEmpty())
            {
                result = buffer;
            }
            else
            {
                result = result + "," + buffer;
            }
		}
	}

    return result;
}

void CContactNodeLayoutCfg::SetContactNodeLayout(CString layout)
{
    // Expected string is like "1,0,2"

    CString strlayout = layout;
    CString strID = "";
    CString strmid = "";
    int id;
    int len = strlayout.GetLength();

    for (int i = 0; i < len; i++)
    {
        strmid = strlayout.Mid(i, 1);

        if (strmid != ",")
        {
            strID = strID + strmid;
            
            // Check if isn't last iteration
            if (i + 1 < len)
                continue;
        }
        id = atoi(strID.GetBuffer(0));
        CContactNodeLayoutElementPtr element;
	    element = FindElement(id);
        if (element)
        {
            m_toolbar.AddButtons(1, element->GetToolBartButton());
            m_listbox.DeleteString(m_listbox.FindString(-1, element->GetCaption()));
        }
        strID.ReleaseBuffer();
        strID = "";
    }
}

void CContactNodeLayoutCfg::AddElement(int id, CString caption, CString bmpPath, BOOL fixed)
{
    CContactNodeLayoutElementPtr element = new CContactNodeLayoutElement();
	element->SetCaption(caption);
    element->SetBitmapPath(bmpPath);
    element->SetBitmapIndex(m_bitmapIndex);
	element->SetID(id);    
	element->SetFixed(fixed);

	m_elements.AddTail(element);
    m_bitmapIndex++;
}

CContactNodeLayoutElementPtr CContactNodeLayoutCfg::FindElement(int id)
{
	CContactNodeLayoutElementPtr element = NULL;
	for (POSITION pos = m_elements.GetHeadPosition(); pos;)
	{
		element = m_elements.GetNext(pos);
		if (element->GetID() == id)
		{
			return element;
			break;
		}
	}

	return NULL;
}
