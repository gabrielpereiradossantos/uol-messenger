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
#include "GroupListCtrlItem.h"

#include ".\grouplistctrl.h"

namespace GroupListCtrl {


CGroupListCtrlItem::CGroupListCtrlItem() :		
	m_size(0, 0), 
	m_bSelected(FALSE), 
	m_bActiveDropTarget(FALSE),
	m_rect(0, 0, 0, 0), 
	m_ptPosition(0, 0),
	m_pGroupListCtrl(NULL)
{
}

CGroupListCtrlItem::~CGroupListCtrlItem()
{
}


void CGroupListCtrlItem::SetGroupListCtrl(CGroupListCtrl* pGroupListCtrl)
{
	m_pGroupListCtrl = pGroupListCtrl;
}

void CGroupListCtrlItem::SetHeight(int cy)
{
	m_size.cy = cy;
}

int CGroupListCtrlItem::GetHeight() const
{
	return m_size.cy;
}

void CGroupListCtrlItem::SetWidth(int cx)
{
	m_size.cx = cx;
}

int CGroupListCtrlItem::GetWidth() const
{
	return m_size.cx;
}

void  CGroupListCtrlItem::SetSelected(BOOL bSelected)
{
	m_bSelected = bSelected;
}

BOOL  CGroupListCtrlItem::GetSelected() const
{
	return m_bSelected;
}

void CGroupListCtrlItem::SetActiveDropTarget(BOOL bActive)
{
	m_bActiveDropTarget = bActive;
}

BOOL CGroupListCtrlItem::IsActiveDropTarget() const
{
	return m_bActiveDropTarget;
}

void  CGroupListCtrlItem::SetRect(const CRect& rect)
{
	m_rect = rect;
}

CRect CGroupListCtrlItem::GetRect() const
{
	return m_rect;
}

void CGroupListCtrlItem::SetPosition(const CPoint& ptPosition)
{
	m_ptPosition = ptPosition;
}

CPoint CGroupListCtrlItem::GetPosition() const
{
	return m_ptPosition;
}

CSize CGroupListCtrlItem::GetSize() const
{
	return m_size;
}

void CGroupListCtrlItem::Invalidate()
{
	ATLASSERT(m_pGroupListCtrl);
	
	if ((m_pGroupListCtrl) && (m_pGroupListCtrl->IsWindow()))
	{
		//CRect rectClient;
		//m_pGroupListCtrl->GetClientRect(&rectClient);

		//CRect rectItem = GetRect();	
		//rectItem.bottom = rectClient.bottom;
		
		//CRect rectItem = GetRect();
		//rectItem.MoveToXY(GetPosition());

		// TODO: Otimizar!
		//m_pGroupListCtrl->InvalidateRect(rectItem, FALSE);
		m_pGroupListCtrl->InvalidateRect(NULL, FALSE);
	}
}

void CGroupListCtrlItem::Redraw(BOOL bNow)
{
	ATLASSERT(m_pGroupListCtrl);
	
	if ((m_pGroupListCtrl) && (m_pGroupListCtrl->IsWindow()))
	{
		if (bNow)
		{
			m_pGroupListCtrl->UpdateWindow();
		}
		else
		{
			m_pGroupListCtrl->RequestUpdateWindow();
		}
	}
}

void CGroupListCtrlItem::DrawFocusRect(CDCHandle& dc)
{
	CRect rect(CPoint(0, 0), m_size);
	
	HBRUSH hOldBrush = dc.SelectStockBrush(NULL_BRUSH);
	HPEN hOldPen = dc.SelectStockPen(BLACK_PEN);

	dc.Rectangle(&rect);

	dc.SelectBrush(hOldBrush);
	dc.SelectPen(hOldPen);
}

void CGroupListCtrlItem::DrawBackground(CDCHandle& dc)
{
}

void CGroupListCtrlItem::OnLBtnDblClick(const CPoint& /*pt*/)
{	
}

void CGroupListCtrlItem::OnRBtnDblClick(const CPoint& /*pt*/)
{	
}

void CGroupListCtrlItem::OnLButtonDown(const CPoint& /*pt*/)
{
}

void CGroupListCtrlItem::OnLButtonUp(const CPoint& /*pt*/)
{
}

void CGroupListCtrlItem::OnRButtonDown(const CPoint& /*pt*/)
{
}

void CGroupListCtrlItem::OnReturnKeyPressed()
{
}

void CGroupListCtrlItem::OnKeyDown(TCHAR ch)
{
	switch (ch)
	{
	case VK_UP:
	case VK_LEFT:
		m_pGroupListCtrl->SelectPrevItem();
		break;
	
	case VK_DOWN:
	case VK_RIGHT:
		m_pGroupListCtrl->SelectNextItem();
		break;
	}	
}

void CGroupListCtrlItem::OnTimer(UINT /*nId*/)
{
}

void CGroupListCtrlItem::OnMouseHover(const CPoint& /*pt*/)
{
}

void CGroupListCtrlItem::OnMouseLeave()
{
}

void CGroupListCtrlItem::OnMouseMove(const CPoint& /*pt*/)
{
}

BOOL CGroupListCtrlItem::IsValidDropTarget()
{
	return m_bValidDropTarget;
}

void CGroupListCtrlItem::SetValidDropTarget(BOOL bValid)
{
	m_bValidDropTarget = bValid;
}

}