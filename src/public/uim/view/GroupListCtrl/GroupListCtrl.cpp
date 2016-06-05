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
#include ".\grouplistctrl.h"


namespace GroupListCtrl {

struct EntryGroupHelper
{
	CEntryGroupPtr pEntryGroup;
};


class CDCGroupListItemMappingTraits : public CDCMappingTraits
{
public:
	CDCGroupListItemMappingTraits(const CPoint& pt) : m_pt(pt) {}

	BOOL GetViewportOrg(CPoint& pt) 
	{
		pt = m_pt;

		return TRUE;
	}	


private:
	CPoint m_pt;
};

struct SelectStateHelper : public CGroupListCtrl::Callback
{
	SelectStateHelper(BOOL bNewState, CGroupListCtrl* pGroupListCtrl) : 
		 m_bNewState(bNewState),
		 m_pGroupListCtrl(pGroupListCtrl) {}

	BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
	{
		if (bIsVisible)
		{			
			pItem->SetSelected(m_bNewState);
			pItem->Invalidate();
		}

		return FALSE;
	}

	BOOL			m_bNewState;
	CGroupListCtrl* m_pGroupListCtrl;
};

struct TargetStateHelper : public CGroupListCtrl::Callback
{
	TargetStateHelper(BOOL bNewState, CGroupListCtrl* pGroupListCtrl) : 
		 m_bNewState(bNewState),
		 m_pGroupListCtrl(pGroupListCtrl) {}

	BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
	{
		pItem->SetValidDropTarget(m_bNewState);
		return FALSE;
	}

	BOOL			m_bNewState;
	CGroupListCtrl* m_pGroupListCtrl;
};

struct IsGroupHelper : public CGroupListCtrl::Callback
{
	IsGroupHelper(CGroupListCtrlItemPtr pItem, BOOL* pIsGroup) : 
		m_pItem(pItem),
		m_pIsGroup(pIsGroup) {}

	BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
	{
		if (pItem == m_pItem)
		{
			*m_pIsGroup = TRUE;
			return TRUE;
		}

		return FALSE;
	}

	CGroupListCtrlItemPtr m_pItem;
	BOOL* m_pIsGroup;
};



const UINT CGroupListCtrl::SYSTEM_TIMER = 0xFFFF;
const UINT CGroupListCtrl::TIMER_UPDATE_WINDOW = 0x0310;


CGroupListCtrl::CGroupListCtrl(void) : 
	m_sizeCurrent(0, 0),
	m_nPendingWindowUpdate(0),
	m_bUpdateScrollBars(FALSE),
	m_bHover(FALSE),
	m_bIsDblClick(FALSE)
{
}


CGroupListCtrl::~CGroupListCtrl(void)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
}


void CGroupListCtrl::AddGroup(CEntryGroupPtr pGroup, BOOL nRedraw)
{
	int nNewHeight;

	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);
		
		AddGroupImpl(pGroup);

		nNewHeight= max(GetHeight(), 1);
	}

	//SendMessage(WM_USER_UPDATESCROLLSIZE, nNewHeight, FALSE);
	SetScrollSize(1, nNewHeight, FALSE, FALSE);
	//SetScrollSize(1, nNewHeight, nRedraw, FALSE);
	UpdateItensPosition();
		
	if (nRedraw)
	{
		RequestUpdateWindow();
	}
}


void CGroupListCtrl::RemoveGroup(CEntryGroupPtr pGroup, BOOL nRedraw)
{
	int nNewHeight;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		RemoveGroupImpl(pGroup);

		nNewHeight= max(GetHeight(), 1);
	}

	SetScrollSize(1, nNewHeight, nRedraw, FALSE);

	if (nRedraw)
	{
		RequestUpdateWindow();
	}
}


CEntryGroupPtr CGroupListCtrl::RemoveGroup(const CString& strGroupName, BOOL nRedraw)
{
	CEntryGroupPtr pGroup;
	int nNewHeight;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		pGroup = GetGroupImpl(strGroupName);

		RemoveGroupImpl(pGroup);

		nNewHeight= max(GetHeight(), 1);
	}

	SetScrollSize(1, nNewHeight, nRedraw, FALSE);

	if (nRedraw)
	{
		RequestUpdateWindow();
	}

	return pGroup;
}


void CGroupListCtrl::RemoveAll(BOOL nRedraw)
{
	struct RemoveAllEntriesHelper : public Callback
	{
		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL /*bIsVisible*/)
		{
			CEntryGroupPtr pEntryGroup = ap_dynamic_cast<CEntryGroupPtr>(pItem);

			if (pEntryGroup)
			{
				pEntryGroup->RemoveAllEntries();
			}

			return FALSE;
		}
	};

	RemoveAllEntriesHelper it;

	{ CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(it, CGroupListCtrl::ioGroups);

		m_listGroups.RemoveAll();
	}

	SetScrollSize(1, 1, nRedraw, FALSE);

	if (nRedraw)
	{
		RequestUpdateWindow();
	}
}

CEntryGroupPtr CGroupListCtrl::GetGroup(const CString& strGroupName)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	return GetGroupImpl(strGroupName);
}


void CGroupListCtrl::GetGroupList(CAtlList<CString>& listGroups)
{
	listGroups.RemoveAll();

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
		{
			CEntryGroupPtr pGroup = m_listGroups.GetNext(pos);
			listGroups.AddTail(pGroup->GetName());
		}
	}
}


void CGroupListCtrl::AddEntry(CEntryGroupPtr pGroup, CEntryPtr pEntry, BOOL bRedraw)
{
	ATLASSERT(pGroup);

	int nNewHeight;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		CEntryGroupPtr pGroupTmp = GetGroupImpl(pGroup->GetName());

		if (!pGroupTmp)
		{
			AddGroupImpl(pGroup);
		}

		InsertEntry(pGroup, pEntry);

		pEntry->SetWidth(m_sizeClient.cx);
		pEntry->SetGroupListCtrl(this);
	    
		nNewHeight= max(GetHeight(), 1);
	}

	//SendMessage(WM_USER_UPDATESCROLLSIZE, nNewHeight, FALSE);
	SetScrollSize(1, nNewHeight, FALSE, FALSE);
	//SetScrollSize(1, nNewHeight, bRedraw, FALSE);
	UpdateItensPosition();

	if (bRedraw)
	{
		RequestUpdateWindow();
	}
}

CEntryPtr CGroupListCtrl::GetEntryByName(const CString& strEntryName)
{
	struct FindEntryByNameHelper : public Callback
	{
		FindEntryByNameHelper(const CString& strEntryName) : m_strEntryName(strEntryName) {} 

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL /*bIsVisible*/)
		{
			CString strName = pItem->GetName();

			if (0 == strName.Compare(m_strEntryName))
			{
				m_pEntry = ap_dynamic_cast<CEntryPtr>(pItem);

				return TRUE;
			}

			return FALSE;
		}

		CEntryPtr m_pEntry;
		CString	  m_strEntryName;
	};

	FindEntryByNameHelper it(strEntryName);

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(it, CGroupListCtrl::ioEntries);
	}

	return it.m_pEntry;	
}


CEntryPtr CGroupListCtrl::GetEntryByName(const CString& strEntryName, const CString& strGroup)
{
	CEntryPtr pEntryRet;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	CEntryGroupPtr pEntryGroup = GetGroupImpl(strGroup);

	if (pEntryGroup != NULL)
	{
		CEntryListPtr plistEntries = pEntryGroup->GetEntryList();

		for (POSITION posEntry = plistEntries->GetHeadPosition(); posEntry; )
		{
			CEntryPtr pEntry = plistEntries->GetNext(posEntry);

			if (0 == strEntryName.CompareNoCase(pEntry->GetName()))
			{
				// Found it
				pEntryRet = pEntry;
				break;
			}
		}
	}

	return pEntryRet;
}


BOOL CGroupListCtrl::RemoveEntry(const CString& strEntryName, const CString& strGroupName, BOOL bRedraw)
{
	BOOL bOk = FALSE;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		CEntryGroupPtr pGroup = GetGroupImpl(strGroupName);

		if (pGroup != NULL)
		{
			CEntryListPtr plistEntries = pGroup->GetEntryList();

			for (POSITION posEntry = plistEntries->GetHeadPosition(); posEntry; )
			{
				CEntryPtr pEntry = plistEntries->GetAt(posEntry);

				if (0 == strEntryName.CompareNoCase(pEntry->GetName()))
				{
					// Found it
					plistEntries->RemoveAt(posEntry);

					bOk = TRUE;

					break;
				}

				plistEntries->GetNext(posEntry);
			}	
		}
	}

	int nHeight;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		nHeight = max(GetHeight(), 1);
	}

	SetScrollSize(1, nHeight, bRedraw, FALSE);

	if (bRedraw)
	{
		RequestUpdateWindow();
	}

	return bOk;
}


CGroupListCtrlItemPtr CGroupListCtrl::GetSelectedItem(IterateOptions io)
{
	struct GetSelectedItemHelper : public Callback
	{
		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if ((bIsVisible) && (pItem->GetSelected()))
			{
				m_pSelectedItem = pItem;

				return TRUE;
			}

			return FALSE;
		}

		CGroupListCtrlItemPtr m_pSelectedItem;
	};

	GetSelectedItemHelper it;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(it, io);
	}

	return it.m_pSelectedItem;
}

void CGroupListCtrl::GetSize(CSize& size)	
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);	

	GetSizeImpl(size);
}

int CGroupListCtrl::GetHeight()
{
	CSize size;

	GetSizeImpl(size);

	return size.cy;
}

void CGroupListCtrl::UpdateItensPosition()
{
	struct UpdateItensPositionHelper : public CGroupListCtrl::Callback
	{
		UpdateItensPositionHelper() : 			
			m_y(0) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			CGroupListCtrlItem* pRawItem = pItem.get();
			
			pRawItem->SetPosition(CPoint(0, m_y));

			if (bIsVisible)
			{
				m_y += pRawItem->GetHeight();
			}			

			return FALSE;
		}
		
		int m_y;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(UpdateItensPositionHelper());
	}

	//InvalidateRect(NULL, FALSE);
}

void CGroupListCtrl::Sort()
{
	{
		//CAutoTimer timer(__FUNCTION__);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		SortGroupsImpl();

		for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
		{
			CEntryGroupPtr pGroup = m_listGroups.GetNext(pos);
			pGroup->Sort();
		}
	}
	
	RequestUpdateWindowImpl(TRUE);
}

void CGroupListCtrl::SortGroups()
{
	{
		//CAutoTimer timer(__FUNCTION__);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		SortGroupsImpl();
	}

	RequestUpdateWindowImpl(TRUE);
}


void CGroupListCtrl::SortGroupEntries(const CString& strGroupName)
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		CEntryGroupPtr pGroup = GetGroupImpl(strGroupName);

		if (pGroup != NULL)
		{
			pGroup->Sort();
		}
	}

	RequestUpdateWindowImpl(TRUE);
}


CGroupListCtrlItemPtr CGroupListCtrl::Pt2Item(const CPoint& pt, IterateOptions io)
{
	struct Pt2ItemIterator : public Callback
	{
		Pt2ItemIterator(const CPoint& point, int yOffset) : cy(-yOffset), pt(point) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible) 
		{
			if (bIsVisible)
			{
				int nHeight = pItem->GetHeight();

				if ((cy <= pt.y) && ((cy + nHeight) > pt.y))
				{
					p = pItem;

					return TRUE;;
				}

				cy += nHeight;
			}

			return FALSE;
		}
		
		int cy;
		const CPoint& pt;
		CGroupListCtrlItemPtr p;
	};

	Pt2ItemIterator it(pt, m_ptOffset.y);

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(it, io);
	}

	return it.p;	
}

BOOL CGroupListCtrl::IsGroup(CGroupListCtrlItemPtr pItem)
{
	BOOL bIsGroup = FALSE;
	IterateThroughGroupList(IsGroupHelper(pItem, &bIsGroup), CGroupListCtrl::ioGroups);

	return bIsGroup;
}

LRESULT CGroupListCtrl::OnEraseBknd(HDC hDC)
{
	return 1;	
}

void CGroupListCtrl::OnLBtnDblClickMsg(UINT, CPoint& pt)
{
	m_bIsDblClick = TRUE;

	CGroupListCtrlItemPtr pItem = Pt2Item(pt);

	if (pItem.get())
	{
		pItem->OnLBtnDblClick(pt);

		UpdateScrollSize();

		UpdateWindow();
		OnLBtnDblClick(pItem, pt);
	}	
}

void CGroupListCtrl::OnRBtnDblClickMsg(UINT, CPoint& pt)
{
	CGroupListCtrlItemPtr pItem = Pt2Item(pt);

	if (pItem.get())
	{	
		pItem->OnRBtnDblClick(pt);

		UpdateScrollSize();

		UpdateWindow();
		OnRBtnDblClick(pItem, pt);
	}
}

void CGroupListCtrl::OnLButtonDownMsg(UINT, CPoint& pt)
{
	m_bIsDblClick = FALSE;

	SetFocus();

	CGroupListCtrlItemPtr pItem = Pt2Item(pt);

	if (pItem.get())
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

			IterateThroughGroupList(SelectStateHelper(FALSE, this));
			IterateThroughGroupList(TargetStateHelper(TRUE, this), CGroupListCtrl::ioGroups);
		}

		pItem->SetSelected(TRUE);
		pItem->Redraw();

		pItem->OnLButtonDown(pt);
		
		OnLButtonDown(pItem, pt);
	}
}

void CGroupListCtrl::OnLButtonUpMsg(UINT, CPoint& pt)
{
	if ( !m_bIsDblClick )
	{
		SetFocus();

		CGroupListCtrlItemPtr pItem = Pt2Item(pt);

		if (pItem.get())
		{
			{
				CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

				IterateThroughGroupList(SelectStateHelper(FALSE, this));
				IterateThroughGroupList(TargetStateHelper(TRUE, this), CGroupListCtrl::ioGroups);
			}

			pItem->SetSelected(TRUE);
			pItem->Redraw();

			pItem->OnLButtonUp(pt);

			OnLButtonUp(pItem, pt);
		}
	}
	else
	{
		m_bIsDblClick = FALSE;
	}
}

void CGroupListCtrl::OnRButtonDownMsg(UINT, CPoint& pt)
{
	SetFocus();

	CGroupListCtrlItemPtr pItem = Pt2Item(pt);

	if (pItem.get())
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

			IterateThroughGroupList(SelectStateHelper(FALSE, this));
		}

		pItem->SetSelected(TRUE);		
		pItem->Redraw();

		pItem->OnRButtonDown(pt);

		OnRButtonDown(pItem, pt);
	}
}

void CGroupListCtrl::OnLBtnDblClick(CGroupListCtrlItemPtr /*pItem*/, const CPoint& pt)
{
}

void CGroupListCtrl::OnRBtnDblClick(CGroupListCtrlItemPtr /*pItem*/, const CPoint& pt)
{
}

void CGroupListCtrl::OnLButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{	
}

void CGroupListCtrl::OnLButtonUp(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{	
}

void CGroupListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{	
}

void CGroupListCtrl::OnKeyDown(CGroupListCtrlItemPtr pItem, TCHAR ch)
{	
}

void CGroupListCtrl::OnKeydown(TCHAR vkey, UINT repeats, UINT code)
{
	// Arrow down
	if (vkey == VK_DOWN)
	{
		SelectNextItem();		
	}

	// Arrow up
	if (vkey == VK_UP)
	{
		SelectPrevItem();		
	}
}


void CGroupListCtrl::OnKeyUp(TCHAR vkey, UINT repeats, UINT code)
{
	// Home
	if (vkey == VK_HOME)
	{
        ScrollTop();
		SelectItem(1);
	}

	// End
	if (vkey == VK_END)
	{
        ScrollBottom();
		SelectLastItem();
	}
	
	// Page Up
	if (vkey == VK_PRIOR)
	{
		ScrollPageUp();
		SelectFirstItemVisible();		
	}

	// Page Down
	if (vkey == VK_NEXT)
	{
		ScrollPageDown();
		SelectLastItemVisible();		
	}
}

void CGroupListCtrl::OnSize(UINT, const CSize& size)
{	
	struct SetSizeHelper : public CGroupListCtrl::Callback
	{
		SetSizeHelper(int cx) : 
			m_cx(cx),
			m_y(0) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			CGroupListCtrlItem* pRawItem = pItem.get();

			pRawItem->SetWidth(m_cx);
			pRawItem->SetPosition(CPoint(0, m_y));

			if (bIsVisible)
			{
				m_y += pRawItem->GetHeight();
			}			

			return FALSE;
		}

		int m_cx;
		int m_y;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SetSizeHelper(size.cx));
	}

	SetMsgHandled(FALSE);

	InvalidateRect(NULL, FALSE);	

	m_sizeCurrent = size;
}

LRESULT CGroupListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetTimer(CGroupListCtrl::SYSTEM_TIMER, 1000);
	SetTimer(CGroupListCtrl::TIMER_UPDATE_WINDOW, 2000);

	return 0L;
}

void CGroupListCtrl::OnDestroy(void)
{
	KillTimer(CGroupListCtrl::SYSTEM_TIMER);
	KillTimer(CGroupListCtrl::TIMER_UPDATE_WINDOW);

	m_pCurrentTrackItem.reset();
	m_pCurrentOverItem.reset();
	m_pCurrentActiveDropTarget.reset();
}

void CGroupListCtrl::OnChar(TCHAR ch, UINT, UINT)
{
	CGroupListCtrlItemPtr pItem;

	switch (ch)
	{	
	case VK_RETURN:
		pItem = GetSelectedItem(CGroupListCtrl::ioALL);

		if (pItem)
		{
			pItem->OnReturnKeyPressed();
		}

		break;
	default:
		SetMsgHandled(FALSE);
	}
}


void CGroupListCtrl::OnTimer(UINT nId, TIMERPROC)
{
	if (nId == SYSTEM_TIMER)
	{
		struct OnTimerHelper : public Callback
		{
			OnTimerHelper(UINT nId) : 
				m_nId(nId) {}

			BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible) 
			{
				pItem->OnTimer(m_nId);

				return FALSE;
			}

			UINT m_nId;
		};

		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csList);
		
			IterateThroughGroupList(OnTimerHelper(nId));
		}
	}
	else if (nId == TIMER_UPDATE_WINDOW)
	{
		if (m_nPendingWindowUpdate > 0)
		{
			ATLTRACE(_T("m_nPendingWindowUpdate = %d\n"), m_nPendingWindowUpdate);

			::InterlockedExchange(&m_nPendingWindowUpdate, 0);

			if (m_bUpdateScrollBars)
			{
				::InterlockedExchange(&m_bUpdateScrollBars, FALSE);

				ATLTRACE(_T("Updating scrollbars\n"));

				UpdateScrollSize();

				//int nHeight;

				//{
				//	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

				//	nHeight = max(GetHeight(), 1);
				//}

				//m_sizeAll.cx = 1;
				//m_sizeAll.cy = nHeight;

				//// block: set horizontal scroll bar
				//{
				//	SCROLLINFO si = {0};
				//	si.cbSize = sizeof(SCROLLINFO);
				//	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				//	if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLH) != 0)
				//		si.fMask |= SIF_DISABLENOSCROLL;
				//	si.nMin = 0;
				//	si.nMax = m_sizeAll.cx - 1;
				//	si.nPage = m_sizeClient.cx;
				//	si.nPos = m_ptOffset.x;
				//	SetScrollInfo(SB_HORZ, &si, TRUE);
				//}

				//// block: set vertical scroll bar
				//{
				//	SCROLLINFO si = {0};
				//	si.cbSize = sizeof(SCROLLINFO);
				//	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				//	if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLV) != 0)
				//		si.fMask |= SIF_DISABLENOSCROLL;
				//	si.nMin = 0;
				//	si.nMax = m_sizeAll.cy - 1;
				//	si.nPage = m_sizeClient.cy;
				//	si.nPos = m_ptOffset.y;
				//	SetScrollInfo(SB_VERT, &si, TRUE);
				//}

				InvalidateRect(NULL);
				UpdateWindow();
			}
		}
	}
}

LRESULT CGroupListCtrl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam); 

	CPoint pt;
	pt.SetPoint(xPos, yPos);

	CGroupListCtrlItemPtr pItem  = Pt2Item(pt);

	if (pItem.get() != m_pCurrentOverItem.get())
	{
		if (m_pCurrentOverItem.get())
		{
			m_pCurrentOverItem->OnMouseLeave();
			m_pCurrentOverItem = NULL;
		}
	}

	if (pItem.get())
	{
		m_pCurrentOverItem = pItem;
		m_pCurrentOverItem->OnMouseMove(pt);
	}

	if(pItem.get() != m_pCurrentTrackItem.get())
	{
		if(m_pCurrentTrackItem.get())
		{
			m_pCurrentTrackItem->OnMouseLeave();
			m_pCurrentTrackItem = NULL;
		}

		m_bHover = FALSE;

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 400;
		BOOL bTracking = ::_TrackMouseEvent(&tme);
		ATLASSERT(bTracking);
	}


	return 0L;
}

LRESULT CGroupListCtrl::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLTRACE("%s\n", __FUNCTION__);
	if (!m_bHover)
	{
		Invalidate();
	}

	m_bHover=TRUE;

	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam); 

	CPoint pt;
	pt.SetPoint(xPos, yPos);

	m_pCurrentTrackItem = Pt2Item(pt);

	if ((m_pCurrentTrackItem.get()) &&
		(m_hWnd == ::GetFocus()))
	{
		m_pCurrentTrackItem->OnMouseHover(pt);
	}

	return 0L;
}

LRESULT CGroupListCtrl::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	if(m_pCurrentTrackItem.get())
	{
		m_pCurrentTrackItem->OnMouseLeave();
		m_pCurrentTrackItem = NULL;
	}

	m_bHover = FALSE;
	RedrawWindow();

	return 0L;
}


LRESULT CGroupListCtrl::OnUpdateScrollSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	int nNewHeight = (int) wParam;
	BOOL bRedraw = (BOOL) lParam;

	SetScrollSize(1, nNewHeight, bRedraw, FALSE);

	return 0L;
}


void CGroupListCtrl::DoPaint(HDC hDC)
{
	struct PaintIterator : public Callback
	{
		PaintIterator(CDCHandle* pDC, CRect& rectItem, CGroupListCtrl* pGroupListCtrl) : 
			m_pDC(pDC), 
			m_rectItem(rectItem),
			m_pGroupListCtrl(pGroupListCtrl) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible) 
		{
			if (bIsVisible)
			{				
				m_pGroupListCtrl->DrawItem(pItem.get(), CDCHandle(*m_pDC), m_rectItem);				
			}

			return FALSE;
		}
		
		CDCHandle*			m_pDC;		
		CRect&			m_rectItem;
		CGroupListCtrl* m_pGroupListCtrl;
	};

	//CAutoTimer at("CGroupListCtrl::DoPaint(HDC)");

	//int nHeight;

	//{
	//	CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

	//	nHeight = max(GetHeight(), 1);
	//}

	//SetScrollSize(1, nHeight, FALSE, FALSE);

	CRect rect;
	GetClientRect(&rect);		

	rect += m_ptOffset;

	CMemDC memDC(hDC, &rect);
	CDCHandle dcHandle(memDC);
	
	PaintBackground((CDCHandle)dcHandle, rect);

	HPEN oldPen = dcHandle.SelectPen((HPEN)::GetStockObject(WHITE_PEN));		

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);		
		
		CRect rectItem(0, 0, rect.Width(), 0);			
		PaintIterator it(&dcHandle, rectItem, this);

		IterateThroughGroupList(it);
	}

	dcHandle.SelectPen(oldPen);

	// debug code
#ifdef _DEBUG
	static int nCount = 0;

	nCount++;

	if (nCount % 10 == 0)
	{
		ATLTRACE(_T("=====> DoPaint() %d <===\n"), nCount);
	}
#endif
}


void CGroupListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
}


void CGroupListCtrl::RequestUpdateWindow()
{
	RequestUpdateWindowImpl(FALSE);
}

void CGroupListCtrl::SelectNextItem()
{
	struct SelectNextItemHelper : public CGroupListCtrl::Callback
	{
		SelectNextItemHelper(CGroupListCtrl* pGroupListCtrl) :
			m_bStopSearch(FALSE), 
			m_pGroupListCtrl(pGroupListCtrl), 
			m_pItemPrev() {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if (bIsVisible)
			{
				if (pItem->GetSelected())
				{
					m_bStopSearch = TRUE;
					m_pItemPrev = pItem;

					return FALSE;
				}

				if (m_bStopSearch)
				{
					if (m_pItemPrev)
					{
						m_pItemPrev->SetSelected(FALSE);
						m_pItemPrev->Invalidate();
					}

					pItem->SetSelected(TRUE);								
					pItem->Invalidate();
				}
			}
			else
			{
				return FALSE;
			}

			return m_bStopSearch;
		}		

	private:
		BOOL m_bStopSearch;
		CGroupListCtrl* m_pGroupListCtrl;
		CGroupListCtrlItemPtr m_pItemPrev;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SelectNextItemHelper(this), IterateOptions::ioALL);
	}

	ScrollView();
}

void CGroupListCtrl::SelectPrevItem()
{
	struct SelectPrevItemHelper : public CGroupListCtrl::Callback
	{
		SelectPrevItemHelper(CGroupListCtrl* pGroupListCtrl) :
			m_pGroupListCtrl(pGroupListCtrl), 
			m_pItemPrev() {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			//if (pItem->GetSelected())
			if (bIsVisible)
			{
				if (pItem->GetSelected())
				{
					if (m_pItemPrev)
					{
						m_pItemPrev->SetSelected(TRUE);
						m_pItemPrev->Invalidate();

						pItem->SetSelected(FALSE);								
						pItem->Invalidate();
					}			

					return TRUE;
				}		

				m_pItemPrev = pItem;			
			}

			return FALSE;
		}		

	private:
		CGroupListCtrl* m_pGroupListCtrl;
		CGroupListCtrlItemPtr m_pItemPrev;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SelectPrevItemHelper(this), IterateOptions::ioALL);
	}

	ScrollView();
}


void CGroupListCtrl::SelectItem(int itemIndex)
{
    struct SelectItemHelper : public CGroupListCtrl::Callback
	{
		SelectItemHelper(CGroupListCtrl* pGroupListCtrl, int itemIndex) :
			m_pGroupListCtrl(pGroupListCtrl), 
            m_idx(0),
            m_itemIndex(itemIndex) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if (bIsVisible)
			{
                if (m_idx == m_itemIndex)
				{
					if (pItem)
					{
						pItem->SetSelected(TRUE);
                        pItem->Invalidate();
					}			

					return TRUE;
				}		
                
                m_idx++;							
			}

			return FALSE;
		}		

	private:
		CGroupListCtrl* m_pGroupListCtrl;
        int m_idx;
        int m_itemIndex;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SelectStateHelper(FALSE, this));
        IterateThroughGroupList(SelectItemHelper(this, itemIndex), IterateOptions::ioALL);
	}

	ScrollView();
}


void CGroupListCtrl::SelectItem(CGroupListCtrlItemPtr pItem)
{
    struct SelectItemHelper : public CGroupListCtrl::Callback
	{
		SelectItemHelper(CGroupListCtrl* pGroupListCtrl, CGroupListCtrlItemPtr pItem) :
			m_pGroupListCtrl(pGroupListCtrl), 
            m_pItem(pItem) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if (bIsVisible)
			{
                if (m_pItem == pItem)
				{
					if (pItem)
					{
						pItem->SetSelected(TRUE);
                        pItem->Invalidate();
					}			

					return TRUE;
				}		
			}

			return FALSE;
		}		

	private:
		CGroupListCtrl* m_pGroupListCtrl;
		CGroupListCtrlItemPtr m_pItem;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SelectStateHelper(FALSE, this));
        IterateThroughGroupList(SelectItemHelper(this, pItem), IterateOptions::ioALL);
	}

	ScrollView();
}


void CGroupListCtrl::DrawItem(CGroupListCtrlItem* pItem, CDCHandle& dc, CRect& rectItem)
{
	int cy = pItem->GetHeight();

	rectItem.bottom += cy;

	pItem->SetRect(rectItem);

	CRect rWndClientRect;
	::GetClientRect(m_hWnd, &rWndClientRect);
	
	CPoint scrollOffset;
	GetScrollOffset(scrollOffset);
	
	BOOL bIsInsideClientRect = (rectItem.bottom > scrollOffset.y) && 
			( (rectItem.top - scrollOffset.y) < rWndClientRect.bottom );
	
	if (bIsInsideClientRect)
	{
		CDCMapping<CDCHandle, CDCGroupListItemMappingTraits> 
			dcMap(dc, CDCGroupListItemMappingTraits(CPoint(0, rectItem.top)));

		pItem->DrawBackground(dc);
		pItem->Paint(dc);			
		
		if (pItem->GetSelected())
		{
			pItem->DrawFocusRect(dc);				
		}
	}

	rectItem.top = rectItem.bottom;	
}


//void CGroupListCtrl::IterateThroughGroupList(Callback& callback, IterateOptions io) const
//{
//	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
//	{
//		CEntryGroupPtr pGroup = m_listGroups.GetNext(pos);						
//		CEntryListPtr plistEntries = pGroup->GetEntryList();
//
//		if ((io & CGroupListCtrl::ioGroups) && (callback(pGroup, pGroup->IsVisible())))
//		{
//			return;
//		}
//
//		for (POSITION posEntry = plistEntries->GetHeadPosition(); posEntry; )
//		{
//			CEntryPtr pEntry = plistEntries->GetNext(posEntry);
//
//			if ((io & CGroupListCtrl::ioEntries) && 
//				(callback(pEntry, (CEntryGroup::expanded == pGroup->GetGroupState()) && pGroup->IsVisible() && pEntry->IsVisible())))
//			{
//				return;
//			}
//		}
//	}
//}


void CGroupListCtrl::IterateThroughGroupList(Callback& callback, IterateOptions io) const
{
	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		CEntryGroupPtr pGroup = m_listGroups.GetNext(pos);						
		CEntryGroup* pRawGroup = pGroup.get();

		BOOL bGroupVisible = pRawGroup->IsVisible();
		CEntryListPtr plistEntries = pRawGroup->GetEntryList();


		if (io & CGroupListCtrl::ioGroups)
		{
			if (callback(pGroup, bGroupVisible))
			{
				return;
			}
		}

		if (io & CGroupListCtrl::ioEntries)
		{
			CEntryGroup::GroupState enumGroupState =  pRawGroup->GetGroupState();
			BOOL bGroupIsExpanded = (CEntryGroup::expanded == enumGroupState);

			for (POSITION posEntry = plistEntries->GetHeadPosition(); posEntry; )
			{
				CEntryPtr pEntry = plistEntries->GetNext(posEntry);				

				if (callback(pEntry, 
							 bGroupIsExpanded && bGroupVisible && pEntry->IsVisible()))
				{
					return;
				}
			}
		}
	}
}


void CGroupListCtrl::AddGroupImpl(CEntryGroupPtr pGroup)
{
	InsertGroup(pGroup);

	pGroup->SetWidth(m_sizeClient.cx);
	pGroup->SetGroupListCtrl(this);
}


void CGroupListCtrl::RemoveGroupImpl(CEntryGroupPtr pGroup)
{
	DeleteGroup(pGroup);
}


void CGroupListCtrl::InsertGroup(CEntryGroupPtr pNewGroup)
{
	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		POSITION posOld = pos;

		CEntryGroupPtr pGroup = m_listGroups.GetNext(pos);

		if (pGroup->Compare(pNewGroup.get()) >= 0)
		{
			m_listGroups.InsertBefore(posOld, pNewGroup);
			
			return;
		}
	}

	m_listGroups.AddTail(pNewGroup);
}


void CGroupListCtrl::DeleteGroup(CEntryGroupPtr pGroup)
{
	for (POSITION pos = m_listGroups.GetHeadPosition(); pos; )
	{
		POSITION posOld = pos;

		CEntryGroupPtr pDeleteGroup = m_listGroups.GetNext(pos);

		if (pDeleteGroup.get() == pGroup.get())
		{
			m_listGroups.RemoveAt(posOld);	
			return ;
		}
	}
}

void CGroupListCtrl::InsertEntry(CEntryGroupPtr pGroup, CEntryPtr pNewEntry)
{
	CEntryListPtr plistEntries = pGroup->GetEntryList();

	for (POSITION posEntry = plistEntries->GetHeadPosition(); posEntry; )
	{
		POSITION posOld = posEntry;

		CEntryPtr pEntry = plistEntries->GetNext(posEntry);

		if (pEntry->Compare(pNewEntry.get()) >= 0)
		{
			plistEntries->InsertBefore(posOld, pNewEntry);
			
			return ;
		}
	}

	plistEntries->AddTail(pNewEntry);
}


CEntryGroupPtr CGroupListCtrl::GetGroupImpl(const CString& strGroupName)
{
	for (POSITION pos =  m_listGroups.GetHeadPosition(); pos;)
	{
		CEntryGroupPtr pGroup = m_listGroups.GetNext(pos);

		if (strGroupName == pGroup->GetName())
		{
			return pGroup;
		}
	}

	return CEntryGroupPtr();
}


void CGroupListCtrl::SetActiveDropTargetImpl(CEntryGroup* pEntryGroup, BOOL bActivate)
{
	if (static_cast<CGroupListCtrlItem*>(pEntryGroup)->IsActiveDropTarget() != bActivate)
	{
		CEntryListPtr plistEntries = pEntryGroup->GetEntryList();

		for (POSITION posEntry = plistEntries->GetHeadPosition(); posEntry; )
		{
			CEntryPtr pEntry = plistEntries->GetNext(posEntry);

			pEntry->SetActiveDropTarget(bActivate);
		}

		static_cast<CGroupListCtrlItem*>(pEntryGroup)->SetActiveDropTarget(bActivate);
	}
}

void CGroupListCtrl::SortGroupsImpl()
{
	int nGroupCount = (int) m_listGroups.GetCount();

	if (nGroupCount > 0)
	{
		EntryGroupHelper* pEntryGroupHelper;
		int nInd = 0;
		EntryGroupHelper** ppGroups = new EntryGroupHelper* [nGroupCount];

		while (m_listGroups.GetCount() > 0)
		{
			pEntryGroupHelper = new EntryGroupHelper;
			pEntryGroupHelper->pEntryGroup = m_listGroups.RemoveHead();
			ppGroups[nInd] = pEntryGroupHelper;
			nInd++;
		}

		ATLASSERT(nInd == nGroupCount);

		::qsort(ppGroups, nGroupCount, sizeof(EntryGroupHelper*), CompareEntryGroups);

		for (nInd = 0; nInd < nGroupCount; nInd++)
		{
			pEntryGroupHelper = ppGroups[nInd];
			m_listGroups.AddTail(pEntryGroupHelper->pEntryGroup);
			delete pEntryGroupHelper;
		}

		ATLASSERT(nInd == nGroupCount);

		delete ppGroups;
	}
}


void CGroupListCtrl::GetSizeImpl(CSize& size)
{
	struct SizeIterator : public Callback
	{
		SizeIterator() : 
			m_size(0, 0)/*,
			m_nCount(0)*/
		{}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible) 
		{
			if (bIsVisible)
			{
				//++m_nCount;

				int nItemHeight = pItem->GetHeight();
				m_size.cy += nItemHeight;
				m_size.cx = max(m_size.cx, pItem->GetWidth());
				
				//ATLTRACE("%i - %s\n", m_cy, pItem->GetName());
				//CString strDebug;
				//strDebug.Format("%d - %d - %s\n", m_nCount, nItemHeight, pItem->GetName());

				//::OutputDebugString(strDebug);
			}			

			return FALSE;
		}
		
		CSize m_size;
		//int m_nCount;
	};

	SizeIterator it;

	IterateThroughGroupList(it);

	//ATLTRACE(_T("CGroupListCtrl::GetSize() - %d, %d\n"), it.m_size.cx, it.m_size.cy);

	size = it.m_size;
}


void CGroupListCtrl::RequestUpdateWindowImpl(BOOL bRedrawScrollbars)
{
	::InterlockedIncrement(&m_nPendingWindowUpdate);
	::InterlockedExchange(&m_bUpdateScrollBars, TRUE);
}



void CGroupListCtrl::SetHeight(int nHeight, IterateOptions enIterOptions)
{
	struct SetHeight : public CGroupListCtrl::Callback
	{
		int m_nHeight;

		SetHeight(int nHeight) :
			m_nHeight(nHeight){}
		
		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if (pItem)
			{
				pItem->SetHeight(m_nHeight);
			}

			return FALSE;
		}
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SetHeight(nHeight), enIterOptions);
	}
}


void CGroupListCtrl::SetActiveDropTarget(CEntryGroupPtr pGroup)
{
	if (m_pCurrentActiveDropTarget.get() != pGroup.get())
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		if (m_pCurrentActiveDropTarget.get())
		{
			SetActiveDropTargetImpl(m_pCurrentActiveDropTarget.get(), FALSE);
		}

		if (pGroup.get())
		{
			SetActiveDropTargetImpl(pGroup.get(), TRUE);
		}

		m_pCurrentActiveDropTarget = pGroup;

		InvalidateRect(NULL);
		UpdateWindow();
	}
}

void CGroupListCtrl::ClearActiveDropTarget()
{
	if (m_pCurrentActiveDropTarget.get())
	{
		SetActiveDropTargetImpl(m_pCurrentActiveDropTarget.get(), FALSE);

		m_pCurrentActiveDropTarget = CEntryGroupPtr();

		InvalidateRect(NULL);
		UpdateWindow();
	}
}


void CGroupListCtrl::UpdateItemSelection()
{
	struct UpdateSelection : public CGroupListCtrl::Callback
	{
		UpdateSelection() {}
		
		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			// Undo selection of invisible items
			if (!bIsVisible && pItem->GetSelected())
			{
				pItem->SetSelected(bIsVisible);
			}

			return FALSE;
		}
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(UpdateSelection(), CGroupListCtrl::ioEntries);
	}
}


void CGroupListCtrl::UpdateScrollSize()
{
	int nHeight;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		nHeight = max(GetHeight(), 1);
	}

	SetScrollSize(1, nHeight, TRUE, FALSE);
}


int CGroupListCtrl::CompareEntryGroups(const void* pEntryGroup1, const void* pEntryGroup2)
{
	EntryGroupHelper* pEntryGroupHelper1 = * ((EntryGroupHelper**) pEntryGroup1);
	EntryGroupHelper* pEntryGroupHelper2 = * ((EntryGroupHelper**) pEntryGroup2);

	return (pEntryGroupHelper1->pEntryGroup)->Compare((pEntryGroupHelper2->pEntryGroup).get());
}

void CGroupListCtrl::SelectFirstItemVisible()
{
	CRect rect;
	GetClientRect(&rect);

	CPoint pt;
	pt.SetPoint(rect.left, rect.top);

	CGroupListCtrlItemPtr pItem = Pt2Item(pt, CGroupListCtrl::ioEntries);

	if (pItem.get())
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

			IterateThroughGroupList(SelectStateHelper(FALSE, this));
		}

		pItem->SetSelected(TRUE);

		CRect pRect = pItem->GetRect();
		ScrollToView(pRect);
	}
}


void CGroupListCtrl::SelectLastItemVisible()
{
	CRect rect;
	GetClientRect(&rect);

	CPoint pt;
	pt.SetPoint(rect.left, rect.bottom - 15);

	CGroupListCtrlItemPtr pItem = Pt2Item(pt, CGroupListCtrl::ioEntries);

	if (pItem.get())
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

			IterateThroughGroupList(SelectStateHelper(FALSE, this));
		}

		pItem->SetSelected(TRUE);

		CRect pRect = pItem->GetRect();
		ScrollToView(pRect);
	}
}

void CGroupListCtrl::SelectLastItem()
{
	struct GetSelectedItemHelper : public Callback
	{
		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if (bIsVisible)
			{
				m_pSelectedItem = pItem;
			}

			return FALSE;
		}

		CGroupListCtrlItemPtr m_pSelectedItem;
	};

	GetSelectedItemHelper it;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		IterateThroughGroupList(SelectStateHelper(FALSE, this));
		IterateThroughGroupList(it, CGroupListCtrl::ioEntries);
	}

	CGroupListCtrlItemPtr pItem = it.m_pSelectedItem;
	if (pItem)
	{
		pItem->SetSelected(TRUE);
		pItem->Invalidate();
	}
}


void CGroupListCtrl::ScrollView()
{
	CGroupListCtrlItemPtr pItem = GetSelectedItem(CGroupListCtrl::ioALL);

	if (pItem.get())
	{
		CRect pRect = pItem->GetRect();
		ScrollToView(pRect);
	}
}

} //end namespace GroupListCtrl 
