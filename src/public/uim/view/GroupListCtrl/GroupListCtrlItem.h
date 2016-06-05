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


namespace GroupListCtrl {

class CGroupListCtrl;

class CGroupListCtrlItem
{
public:	
	CGroupListCtrlItem();
	virtual ~CGroupListCtrlItem();

	virtual void SetGroupListCtrl(CGroupListCtrl* pGroupListCtrl);

	virtual void SetName(const CString& strName) = 0;
	virtual CString GetName() const = 0;

	virtual BOOL IsVisible() const = 0;
	
	void  SetHeight(int cy);
	int	  GetHeight() const;	

	void SetWidth(int cx);
	int	 GetWidth() const;

	virtual void  SetSelected(BOOL bSelected);
	virtual BOOL  GetSelected() const;

	virtual void  SetActiveDropTarget(BOOL bActive);
	virtual BOOL  IsActiveDropTarget() const;

	void  SetRect(const CRect& rect);
	CRect GetRect() const;	

	virtual void SetPosition(const CPoint& ptPosition);
	virtual CPoint GetPosition() const;

	CSize GetSize() const;

	virtual void Paint(CDCHandle& dc) = 0;
	virtual void DrawFocusRect(CDCHandle& dc);	
	virtual void DrawBackground(CDCHandle& dc);	

	virtual void Invalidate();
	virtual void Redraw(BOOL bNow = FALSE);

	virtual void OnLBtnDblClick(const CPoint& pt);	
	virtual void OnLButtonDown(const CPoint& pt);
	virtual void OnLButtonUp(const CPoint& pt);
	virtual void OnRButtonDown(const CPoint& pt);
	virtual void OnRBtnDblClick(const CPoint& pt);
	virtual void OnReturnKeyPressed();
	virtual void OnKeyDown(TCHAR ch);
	virtual void OnTimer(UINT nId);
	virtual void OnMouseHover(const CPoint& pt);
	virtual void OnMouseLeave();
	virtual void OnMouseMove(const CPoint& pt);

	virtual BOOL IsValidDropTarget();
	virtual void SetValidDropTarget(BOOL bValid);

protected:
	CGroupListCtrl*		m_pGroupListCtrl;
	BOOL				m_bValidDropTarget;

private:
	CSize				m_size;	
	BOOL				m_bSelected;
	BOOL				m_bActiveDropTarget;
	CRect				m_rect;
	CPoint				m_ptPosition;

}; MAKEAUTOPTR(CGroupListCtrlItem)

};


