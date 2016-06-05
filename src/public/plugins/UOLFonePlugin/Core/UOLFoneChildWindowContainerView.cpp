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
#include ".\UOLFoneChildWindowContainerView.h"
#include "../FacadeWrapper.h"


CUOLFoneChildWindowContainerView::CUOLFoneChildWindowContainerView(void) : 
		m_showingChildWindow(NULL), 
		m_showingWindow(FALSE)
{
	CFacadeWrapper::GetInstance()->SetChildContainerPtr(this);
	
	m_componentsOrganizer.Initialize(this);
}


CUOLFoneChildWindowContainerView::~CUOLFoneChildWindowContainerView(void)
{
}


IUOLMessengerPluginCustomTabChildWindow* CUOLFoneChildWindowContainerView::ShowChildWindow(
		int childWindowID, CUOLFoneChildWindowInfo* pChildWindowInfo)
{
	CWindowMap::CPair* pPair = NULL;
	
	pPair = m_uolfoneChildWindowsMap.Lookup(childWindowID);
	
	if (pPair != NULL)
	{
		if ( (m_showingChildWindow != NULL) && 
				(m_showingChildWindow != pPair->m_value) )
		{
			m_showingChildWindow->pChildCWindow->ShowWindow(SW_HIDE);
			m_showingChildWindow->pTabChildWindow->ResetWindow();
		}
		
		m_showingChildWindow = pPair->m_value;
		
		m_showingChildWindow->pTabChildWindow->SetChildWindowInfo(pChildWindowInfo);
		m_showingChildWindow->pChildCWindow->ShowWindow(SW_SHOW);
		
		HandleChildWindowFocus();
		
		return m_showingChildWindow->pTabChildWindow;
	}
	
	return NULL;
}


void CUOLFoneChildWindowContainerView::GetMinSize(CSize& size)
{
	CSize childWindowSize;
	int cx = 0;
	int cy = 0;
	ChildWindowInfo* childWindow;
	
	for (POSITION pos = m_uolfoneChildWindowsList.GetHeadPosition(); pos; )
	{
		childWindow = m_uolfoneChildWindowsList.GetNext(pos);
		
		childWindow->pTabChildWindow->GetMinSize(childWindowSize);
		
		cx = __max(cx, childWindowSize.cx);
		cy = __max(cy, childWindowSize.cy);
	}
	
	size.cx = cx;
	size.cy = cy;
}


void CUOLFoneChildWindowContainerView::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneChildWindowContainerView::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneChildWindowContainerView::OnSetFocus(HWND lostFocusWnd)
{
	//ATLTRACE(_T("%s - lostFocusWnd = %u\n"), __FUNCTION__, (DWORD)lostFocusWnd);
	
	if ( (m_showingWindow) && (m_showingChildWindow != NULL) && 
			(m_showingChildWindow->pTabChildWindow != NULL) && 
			(m_showingChildWindow->pTabChildWindow->CanGetFocus()) )
	{
		::SetFocus(m_showingChildWindow->pChildCWindow->m_hWnd);
	}
}


void CUOLFoneChildWindowContainerView::OnKillFocus(HWND newFocusWnd)
{
	//ATLTRACE(_T("%s - newFocusWnd = %u\n"), __FUNCTION__, (DWORD)newFocusWnd);
	
	// Do nothing.
}


CWindow* CUOLFoneChildWindowContainerView::GetChildWindowPtr(int childWindowID)
{
	CWindowMap::CPair* pPair = NULL;
	ChildWindowInfo* childWindowInfo = NULL;
	CWindow* childWindowPtr = NULL;
	
	pPair = m_uolfoneChildWindowsMap.Lookup(childWindowID);
	
	if (pPair != NULL)
	{
		childWindowInfo = pPair->m_value;
		
		childWindowPtr = childWindowInfo->pChildCWindow;
	}
	
	return childWindowPtr;
}


LRESULT CUOLFoneChildWindowContainerView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Create child windows.
	//
	
	m_initializingWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	m_keyboardWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	m_callInProgressWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	m_acceptCallWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	m_callInPlaceWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	m_callErrorWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	m_informationWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL);
	
	AddChildWindowInfoItem(UOLFONE_INITIALIZING_WINDOW, &m_initializingWnd, &m_initializingWnd);
	AddChildWindowInfoItem(UOLFONE_KEYBOARD_WINDOW,	&m_keyboardWnd, &m_keyboardWnd);
	AddChildWindowInfoItem(UOLFONE_CALL_IN_PROGRESS_WINDOW, &m_callInProgressWnd, &m_callInProgressWnd);
	AddChildWindowInfoItem(UOLFONE_ACCEPT_CALL_WINDOW, &m_acceptCallWnd, &m_acceptCallWnd);
	AddChildWindowInfoItem(UOLFONE_CALL_IN_PLACE_WINDOW, &m_callInPlaceWnd, &m_callInPlaceWnd);
	AddChildWindowInfoItem(UOLFONE_CALL_ERROR_WINDOW, &m_callErrorWnd, &m_callErrorWnd);
	AddChildWindowInfoItem(UOLFONE_INFORMATION_WINDOW, &m_informationWnd, &m_informationWnd);
	
	CFacadeWrapper::GetInstance()->RegisterAsFocusObserver(this);
	
	return 0L;
}


LRESULT CUOLFoneChildWindowContainerView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFacadeWrapper::GetInstance()->UnregisterAsFocusObserver(this);
	
	m_showingChildWindow = NULL;
	m_showingWindow = FALSE;
	
	ChildWindowInfo* childWindow;
	
	for (POSITION pos = m_uolfoneChildWindowsList.GetHeadPosition(); pos; )
	{
		childWindow = m_uolfoneChildWindowsList.GetNext(pos);
		delete childWindow;
	}
	
	m_uolfoneChildWindowsList.RemoveAll();
	m_uolfoneChildWindowsMap.RemoveAll();
	
	return 0L;
}


LRESULT CUOLFoneChildWindowContainerView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rectClient;
	::GetClientRect(m_hWnd, &rectClient);
	
	ChildWindowInfo* childWindow;
	
	for (POSITION pos = m_uolfoneChildWindowsList.GetHeadPosition(); pos; )
	{
		childWindow = m_uolfoneChildWindowsList.GetNext(pos);
		
		childWindow->pChildCWindow->MoveWindow(rectClient);
	}
	
	return 0L;
}


LRESULT CUOLFoneChildWindowContainerView::OnShowWindow(UINT /*uMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_showingWindow = (BOOL)wShow;
	
	if (!m_showingWindow)
	{
		if (m_showingChildWindow)
		{
			m_showingChildWindow->pChildCWindow->ShowWindow(SW_HIDE);
		}
	}
	else if (m_showingChildWindow == NULL)
	{
		CFacadeWrapper::GetInstance()->ShowHomeWindow();
	}
	else
	{
		m_showingChildWindow->pChildCWindow->ShowWindow(SW_SHOW);
		
		HandleChildWindowFocus();
	}
	
	return 0L;
}


LRESULT CUOLFoneChildWindowContainerView::OnEraseBackground(HDC hDc)
{
	return 1L;
}


void CUOLFoneChildWindowContainerView::AddChildWindowInfoItem(int nItem, IUOLMessengerPluginCustomTabChildWindow* pTabChildWindow, CWindow* pChildCWindow)
{
	ChildWindowInfo* newChildWindowInfo = NULL;
	
	newChildWindowInfo = CreateChildWindowInfoItem(pTabChildWindow, pChildCWindow);
	
	if (newChildWindowInfo != NULL) {
		
		m_uolfoneChildWindowsMap.SetAt(	nItem, newChildWindowInfo);
		m_uolfoneChildWindowsList.AddHead(newChildWindowInfo);
	}
}


ChildWindowInfo* CUOLFoneChildWindowContainerView::CreateChildWindowInfoItem(
		IUOLMessengerPluginCustomTabChildWindow* pTabChildWindow, 
		CWindow* pChildCWindow)
{
	ChildWindowInfo* newChildInfoItem = new ChildWindowInfo();
	
	if (newChildInfoItem != NULL) {
		
		newChildInfoItem->pTabChildWindow = pTabChildWindow;
		newChildInfoItem->pChildCWindow = pChildCWindow;
	}
	
	return newChildInfoItem;
}


void CUOLFoneChildWindowContainerView::HandleChildWindowFocus()
{
	if (m_showingChildWindow->pTabChildWindow->CanGetFocus())
	{
		::SetFocus(m_showingChildWindow->pChildCWindow->m_hWnd);
	}
	else
	{
		CFacadeWrapper::GetInstance()->SetFocusToMainWindow();
	}
}


IUOLMessengerPluginCustomTabChildWindow* CUOLFoneChildWindowContainerView::GetCurrentChildWindow()
{
	if (m_showingChildWindow)
	{
		return m_showingChildWindow->pTabChildWindow;
	}
	
	return NULL;
}

