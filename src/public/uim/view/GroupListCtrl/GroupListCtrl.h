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

#include "EntryGroup.h"


namespace GroupListCtrl {

class CGroupListCtrl;

typedef CAtlList<CEntryGroupPtr> CEntryGroupList;
MAKEAUTOPTR(CEntryGroupList);	


class CGroupListCtrl : public CScrollWindowImpl<CGroupListCtrl>
{
public:
	static const UINT SYSTEM_TIMER;

	struct Callback
	{
		virtual BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible) = 0;
		virtual BOOL GetExpanded() {return TRUE;}
	};	

	DECLARE_WND_CLASS_EX("GroupListCtrl", CS_DBLCLKS, COLOR_WINDOW);

	// public Methods
	CGroupListCtrl(void);
	virtual ~CGroupListCtrl(void);		

	void			AddGroup(CEntryGroupPtr pGroup, BOOL nRedraw = TRUE);
	void			RemoveGroup(CEntryGroupPtr pGroup, BOOL nRedraw = TRUE);
	CEntryGroupPtr	RemoveGroup(const CString& strGroupName, BOOL nRedraw = TRUE);
	void			RemoveAll(BOOL nRedraw = TRUE);
	CEntryGroupPtr	GetGroup(const CString& strGroupName);
	

	void		GetGroupList(CAtlList<CString>& listGroups);

	void		AddEntry(CEntryGroupPtr pGroup, CEntryPtr pEntry, BOOL bRedraw = TRUE);
	CEntryPtr	GetEntryByName(const CString& strEntryName);
	CEntryPtr	GetEntryByName(const CString& strEntryName, const CString& strGroup);
	BOOL		RemoveEntry(const CString& strEntryName, const CString& strGroupName, BOOL bRedraw = TRUE);

	enum IterateOptions {ioGroups = 1, 
						 ioEntries = 2, 
						 ioALL = ioGroups | ioEntries}; 

	CGroupListCtrlItemPtr GetSelectedItem(IterateOptions io = ioEntries);

	void GetSize(CSize& size);
	int GetHeight();

	void UpdateItensPosition();

	void Sort();
	void SortGroups();
	void SortGroupEntries(const CString& strGroupName);

	void RequestUpdateWindow();

	void SelectNextItem();
	void SelectPrevItem();
    void SelectItem(int itemIndex = 0);	
	void SelectItem(CGroupListCtrlItemPtr pItem);
	void SelectFirstItemVisible();
	void SelectLastItemVisible();
	void SelectLastItem();

protected:
	enum
	{
		WM_USER_UPDATESCROLLSIZE = WM_USER + 0x21
	};

	BEGIN_MSG_MAP(CGroupListCtrl)		
		MSG_WM_ERASEBKGND(OnEraseBknd)
		MSG_WM_LBUTTONDOWN(OnLButtonDownMsg)
		MSG_WM_LBUTTONUP(OnLButtonUpMsg)
		MSG_WM_RBUTTONDOWN(OnRButtonDownMsg)		
		MSG_WM_LBUTTONDBLCLK(OnLBtnDblClickMsg)	
		MSG_WM_RBUTTONDBLCLK(OnRBtnDblClickMsg)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CHAR(OnChar)
		MSG_WM_KEYDOWN(OnKeydown)
		MSG_WM_KEYUP(OnKeyUp)
		MSG_WM_TIMER(OnTimer)		
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_USER_UPDATESCROLLSIZE, OnUpdateScrollSize)
		CHAIN_MSG_MAP(CScrollWindowImpl<CGroupListCtrl>)
	END_MSG_MAP()	

	CGroupListCtrlItemPtr Pt2Item(const CPoint& pt, IterateOptions io = CGroupListCtrl::ioALL);
	BOOL IsGroup(CGroupListCtrlItemPtr pItem);

	LRESULT OnEraseBknd(HDC hDC);
	void OnLButtonDownMsg(UINT, CPoint& pt);
	void OnLButtonUpMsg(UINT, CPoint& pt);
	void OnRButtonDownMsg(UINT, CPoint& pt);
	void OnLBtnDblClickMsg(UINT, CPoint& pt);	
	void OnRBtnDblClickMsg(UINT, CPoint& pt);	

	void OnSize(UINT, const CSize& size);
	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy(void);

	virtual void OnChar(TCHAR, UINT, UINT);	
	void OnKeydown(TCHAR vkey, UINT repeats, UINT code);
	void OnKeyUp(TCHAR vkey, UINT repeats, UINT code);
	void OnTimer(UINT, TIMERPROC);

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnUpdateScrollSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void DoPaint(HDC hDC);
	
	virtual void PaintBackground(CDCHandle& dc, const CRect& rect);

	virtual void OnLBtnDblClick(CGroupListCtrlItemPtr pItem, const CPoint& pt);
	virtual void OnLButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt);
	virtual void OnLButtonUp(CGroupListCtrlItemPtr pItem, const CPoint& pt);
	virtual void OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt);
	virtual void OnRBtnDblClick(CGroupListCtrlItemPtr pItem, const CPoint& pt);
	virtual void OnKeyDown(CGroupListCtrlItemPtr pItem, TCHAR ch);
	
	void SetHeight(int nHeight, IterateOptions enIterOptios);

	void SetActiveDropTarget(CEntryGroupPtr pGroup);
	void ClearActiveDropTarget();

	void UpdateItemSelection();
	void UpdateScrollSize();

	void IterateThroughGroupList(Callback& callback, IterateOptions io = ioALL) const;

private:	
	void DrawItem(CGroupListCtrlItem* pItem, CDCHandle& dc, CRect& rectItem);	   	

	void AddGroupImpl(CEntryGroupPtr pGroup);
	void RemoveGroupImpl(CEntryGroupPtr pGroup);
	void InsertGroup(CEntryGroupPtr pNewGroup);
	void DeleteGroup(CEntryGroupPtr pGroup);
	void InsertEntry(CEntryGroupPtr pGroup, CEntryPtr pNewEntry);
	CEntryGroupPtr GetGroupImpl(const CString& strGroupName);
	void SetActiveDropTargetImpl(CEntryGroup* pEntryGroup, BOOL bActivate);
	void SortGroupsImpl();
	void GetSizeImpl(CSize& size);
	void RequestUpdateWindowImpl(BOOL bRedrawScrollbars = FALSE);
	void ScrollView();

	static int CompareEntryGroups(const void* pEntryGroup1, const void* pEntryGroup2);

protected:
	CComAutoCriticalSection		m_csList;

private:
	static const UINT TIMER_UPDATE_WINDOW;

	CEntryGroupList		m_listGroups;	
	CSize				m_sizeCurrent;

	CGroupListCtrlItemPtr	m_pCurrentTrackItem;
	CGroupListCtrlItemPtr	m_pCurrentOverItem;
	CEntryGroupPtr			m_pCurrentActiveDropTarget;

	BOOL				m_bHover;
	volatile LONG		m_nPendingWindowUpdate;
	volatile LONG		m_bUpdateScrollBars;

	BOOL				m_bIsDblClick;
};

MAKEAUTOPTR(CGroupListCtrl);

}; // end namespace GroupListCtrl

