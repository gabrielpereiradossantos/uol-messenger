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
#include "HistoryWindow.h"

#include "../resource.h"
#include "../controller/UIMApplication.h"
#include "../controller/CommandFactory.h"
#include "../controller/UIManager.h"
#include "skin/ProtocolImageElement.h"
#include "ImageButtonContainer.h"
#include "UOLToolbarBuilder.h"
#include "ATLComTime.h"
#include "./FindPanelButton.h"


#define TREEVIEW_DATE_NODE 1
#define TREEVIEW_MONTH_NODE 2
#define MIN_HEIGHT 300


///////////////////////////////////////////////////////////////////
//  CHistoryWindowObserverNotifier
///////////////////////////////////////////////////////////////////
void CHistoryWindowObserverNotifier::NotifyAll(void (CUOLMessengerHistoryWindowObserver::*pfnUpdate)(IUOLMessengerHistoryWindow2*), IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	CAtlList<CUOLMessengerHistoryWindowObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	CUOLMessengerHistoryWindowObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(pHistoryWindow);
	}
}


///////////////////////////////////////////////////////////////////
//  CHistoryTreeView
///////////////////////////////////////////////////////////////////
template<class T>
CHistoryTreeView<T>::CHistoryTreeView() 
{
}

template<class T>
LRESULT CHistoryTreeView<T>::OnTreeItemChanged( int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	LPNMTREEVIEW selChanged;
	selChanged = (LPNMTREEVIEW) pnmh;
	
	GetParent().SendMessage(WM_COMMAND, MAKEWPARAM(0, HISTORY_ITEM_SELECTED), (LPARAM) m_hWnd);
	return 0L ;
}

template<class T>
LRESULT CHistoryTreeView<T>::OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pnmt=(LPNMTREEVIEW)pnmh;
	HTREEITEM hItem = pnmt->itemNew.hItem;
	//ATLASSERT(hItem);
	if (hItem)	// Delete attached structure
		CTreeViewCtrl::DeleteItem(hItem);
	return 0;
}


///////////////////////////////////////////////////////////////////
//  CHistoryWindow
///////////////////////////////////////////////////////////////////
CHistoryWindow::CHistoryWindow()
{
	m_bSplitterPositioned = FALSE;
	m_bIsShowingHistoryText = FALSE;
	m_bIsCtrlKeyPressed = FALSE;

	m_strFindText = "";
	m_strLastMonthYearItem = "";
	m_strLastDateItem = "";
	
	m_hLastMonthYearItem = NULL;
	m_hLastDateItem = NULL;
}


CHistoryWindow::~CHistoryWindow()
{
	if (m_pHistoryRecordList)
	{
		m_pHistoryRecordList->StopLoad();
		m_pHistoryRecordList->UnregisterObserver(this);
		m_pHistoryRecordList->RemoveAll();
	}

	m_listFoundTextPos.RemoveAll();
}


void CHistoryWindow::CreateToolbars()
{
	CUOLToolBarBuilderPtr pBuilder1;
	CUOLToolBarBuilderPtr pBuilder2;
	
	for (POSITION pos = m_listChilds.GetHeadPosition(); 
			( (pos) && ( (pBuilder1.get() == NULL) || (pBuilder2.get() == NULL) ) ); )
	{
		if (pBuilder1 == NULL)
		{
			pBuilder1 = ap_dynamic_cast<CUOLToolBarBuilderPtr>(m_listChilds.GetNext(pos));
		}
		else if (pBuilder2 == NULL)
		{
			pBuilder2 = ap_dynamic_cast<CUOLToolBarBuilderPtr>(m_listChilds.GetNext(pos));
		}
	}	
	
	if (pBuilder1)
	{
		m_pToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder1->CreateElement());
		ATLASSERT(m_pToolbar);
	}
	
	if (pBuilder2)
	{
		m_pFinderToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder2->CreateElement());
		ATLASSERT(m_pFinderToolbar);
	}
	
	CRect rect = CRect(CPoint(0, 0), CSize(0, 0)); 
	
	if (m_pToolbar.get())
	{
		m_pToolbar->Create(m_hWnd, rect);
		
		m_pToolbar->AddButtonsEventListener(this);
	}
	
	if (m_pFinderToolbar.get())
	{
		m_pFinderToolbar->Create(m_hWnd, rect);
		
		m_pFinderToolbar->AddButtonsEventListener(this);
	}
}


LRESULT CHistoryWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	CreateToolbars();
	SetWindowText(_T(""));

	// Create Splitter
	if (!(m_wndSplitter.IsWindow()))
	{
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        CRect rect = CRect(0, 0, 0, 0);

		m_wndSplitter.Create(m_hWnd, rect, NULL, dwStyle);
	}

	m_wndSplitter.m_bUpdateProportionalPos = FALSE;
	m_wndSplitter.m_nProportionalPos = 8500;
	m_wndSplitter.m_cxyBarEdge = 0;
	m_wndSplitter.m_cxyMin = 150;
	m_wndSplitter.m_bFullDrag = TRUE;
	m_wndSplitter.SetSplitterRect();
	
	// Create TreeViewCtrl and RichEditCtrl
	CRect rectClient;
	m_wndSplitter.GetClientRect(&rectClient);

	int nRectHeight = rectClient.Height();
	int nRectWidth  = rectClient.Width();

	DWORD dwRichStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS	| WS_CLIPCHILDREN | 
		WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_SAVESEL;
	m_RichEdit.Create(m_wndSplitter.m_hWnd, CRect(CPoint(0, 150), CSize(nRectWidth - 150, nRectHeight)), NULL, dwRichStyle, 0);
	m_RichEdit.SetReadOnly(TRUE);
    m_RichEdit.SetWindowText(_T(""));
	m_RichEdit.SetNotifyingWindow(m_hWnd);
	m_RichEdit.SetSendNewMessage(TRUE);
	
	DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |  WS_CLIPCHILDREN | 
		WS_BORDER | WS_TABSTOP| TVS_HASLINES | TVS_HASBUTTONS |	TVS_LINESATROOT | 
		TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP;
	m_HistoryTreeView.Create(m_wndSplitter.m_hWnd, CRect(CPoint(0, 0), CSize(150, nRectHeight)), NULL, dwTreeStyle, WS_EX_CLIENTEDGE);
	m_HistoryTreeView.SetNotifyingWindow(m_hWnd);
	m_HistoryTreeView.SetSendNewMessage(TRUE);
	
	m_wndSplitter.SetSplitterPanes(m_HistoryTreeView, m_RichEdit);
	m_wndSplitter.SetSplitterPos(150);

	// Set Font
	if (m_pFont.get())
	{
		m_pToolbar->SetFont(m_pFont);
		m_pFinderToolbar->SetFont(m_pFont);

		CFont font;		
		if (NULL != font.CreateFontIndirect(&(*m_pFont->GetFont())))
		{
			m_RichEdit.SetFont(font);
		}
	}

	m_ObserverNotifier.NotifyAll(&CUOLMessengerHistoryWindowObserver::OnCreateHistoryWindow, this);
	
	// Hide Find Panel and navigation buttons while populating treeview
	CFindPanelButtonPtr pFindPanelButton = 
			ap_dynamic_cast<CFindPanelButtonPtr>(
					m_pFinderToolbar->FindImageButton("SearchHistory"));
	
	if (pFindPanelButton)
	{
		pFindPanelButton->RegisterObserver(this);
		pFindPanelButton->SetVisible(FALSE);
	}

	IUOLMessengerImageButtonPtr pImageNextButton = 
			m_pFinderToolbar->FindImageButton("FindNextHistory");
	if (pImageNextButton)
	{
		pImageNextButton->SetVisible(FALSE);
	}
	
	IUOLMessengerImageButtonPtr pImagePriorButton = 
			m_pFinderToolbar->FindImageButton("FindPriorHistory");
	if (pImagePriorButton)
	{
		pImagePriorButton->SetVisible(FALSE);
	}

	return 0L;
}


void CHistoryWindow::OnDestroy()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	m_HistoryTreeView.DestroyWindow();
	m_RichEdit.DestroyWindow();
	m_wndSplitter.DestroyWindow();
	
	m_ObserverNotifier.NotifyAll(&CUOLMessengerHistoryWindowObserver::OnDestroyHistoryWindow, this);
	
	CFindPanelButtonPtr pFindPanelButton = 
			ap_dynamic_cast<CFindPanelButtonPtr>(
					m_pFinderToolbar->FindImageButton("SearchHistory"));
	
	if (pFindPanelButton)
	{
		pFindPanelButton->UnregisterObserver(this);
	}
	
	SetMsgHandled(FALSE);
}


void CHistoryWindow::OnClose()
{
	if (m_pHistoryRecordList)
	{
		m_pHistoryRecordList->StopLoad();
		m_pHistoryRecordList->UnregisterObserver(this);
		m_pHistoryRecordList->RemoveAll();
	}

	m_listFoundTextPos.RemoveAll();
}


void CHistoryWindow::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);
	CSize sizeToolbar;
	CSize sizeFinderToolbar;
	
	CSize size(rect.Width(), rect.Height());
	
	if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);
		m_pToolbar->SetWindowPos(NULL, 0, 0, size.cx, sizeToolbar.cy, SWP_NOZORDER);
	}
	
	if (m_pFinderToolbar.get())
	{
		m_pFinderToolbar->GetMinSize(sizeFinderToolbar);
		m_pFinderToolbar->SetWindowPos(NULL, 0, size.cy - sizeFinderToolbar.cy,
				size.cx, sizeFinderToolbar.cy, SWP_NOZORDER);
		m_pFinderToolbar->InvalidateRect(NULL);
		m_pFinderToolbar->UpdateWindow();
	}
	
	m_wndSplitter.SetWindowPos(NULL, 0, sizeToolbar.cy,  
			size.cx, size.cy - sizeToolbar.cy - sizeFinderToolbar.cy,  SWP_NOZORDER);
	
	// Update splitter position
	if (!m_bSplitterPositioned)
	{
		if (rect.Height() > 0)
		{
			m_bSplitterPositioned = TRUE;
			m_wndSplitter.SetSplitterPos(150);
		}
	}
}


void CHistoryWindow::GetMinSize(CSize& size)
{
	CSize sizeToolbar(0, 0);
	if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);
	}

	CSize sizeFinderToolbar(0, 0);
	if (m_pFinderToolbar.get())
	{
		m_pFinderToolbar->GetMinSize(sizeFinderToolbar);
	}

	size.SetSize(max(sizeToolbar.cx, sizeFinderToolbar.cx), MIN_HEIGHT);
}


BOOL CHistoryWindow::IsItem (const CString& strText, HTREEITEM& hFather)
{
	CString strAux, strAux2;
	HTREEITEM hMonthYear = hFather;

	while(hMonthYear)
	{
		m_HistoryTreeView.GetItemText(hMonthYear, strAux);
		if (strText == strAux)
		{
			hFather = hMonthYear;
			return TRUE;
		}
		hMonthYear = m_HistoryTreeView.GetNextItem(hMonthYear, TVGN_NEXT);
	}
	return FALSE;
}


void CHistoryWindow::ClearTreeItens()
{
	m_HistoryTreeView.DeleteAllItems();

	m_RichEdit.SetWindowText(_T(""));
	ATLASSERT(m_HistoryTreeView.GetCount() == 0);

	m_strLastMonthYearItem = "";
	m_strLastDateItem = "";
	
	m_hLastMonthYearItem = NULL;
	m_hLastDateItem = NULL;
}


void CHistoryWindow::ClearHistoryEdit()
{
	m_RichEdit.SetWindowText(_T(""));
}


void CHistoryWindow::OnSize(UINT nType, const CSize& size)
{
	UpdateLayout();
}


void CHistoryWindow::OnItemSelected(UINT nId, int nCode, HWND hWnd)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
    ATLASSERT(pStringLoader);
	
	HTREEITEM hChild;
	HTREEITEM hSelectedItem = m_HistoryTreeView.GetSelectedItem();
	if (hSelectedItem) 
	{
		ClearHistoryEdit();
		DWORD dwTime = m_HistoryTreeView.GetItemData(hSelectedItem);
		if (dwTime > TREEVIEW_MONTH_NODE)
		{
			CTime TimeEvent(dwTime);
			CString strHistoryText = m_pHistoryRecordList->GetHistoryText(TimeEvent);
			if (strHistoryText.IsEmpty())
			{
				strHistoryText = pStringLoader->GetValue("uol.resource.error.IDS_HISTORY_UNABLE_TO_SHOW");
				m_RichEdit.SetWindowText(strHistoryText);
				m_bIsShowingHistoryText = FALSE;
			}
			else
			{
				m_RichEdit.SetWindowText(strHistoryText);
				m_bIsShowingHistoryText = TRUE;
			}
		} 
		else 
		{
			hChild = m_HistoryTreeView.GetChildItem(hSelectedItem);
			dwTime = m_HistoryTreeView.GetItemData(hChild);
			CTime TimeEvent(dwTime);
			if (dwTime > TREEVIEW_MONTH_NODE)
			{
				CString strHistoryText = m_pHistoryRecordList->GetHistoryText(TimeEvent);
				if (strHistoryText.IsEmpty())
				{
					strHistoryText = pStringLoader->GetValue("uol.resource.error.IDS_HISTORY_UNABLE_TO_SHOW");
					m_RichEdit.SetWindowText(strHistoryText);
					m_bIsShowingHistoryText = FALSE;
				}
				else
				{
					m_RichEdit.SetWindowText(strHistoryText);
					m_bIsShowingHistoryText = TRUE;
				}
			}
		}
		
		HighLightFindText();
	}
}


LRESULT CHistoryWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleKeyDownEvent((int)wParam);
	
    bHandled = FALSE;
	
	return 0L;
}


LRESULT CHistoryWindow::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleKeyUpEvent((int)wParam);
	
    bHandled = FALSE;
	
	return 0L;
}


LRESULT CHistoryWindow::OnChar(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleCharEvent((int)wParam);
	
	return 0L;
}


LRESULT CHistoryWindow::OnKeyDownEventNotifier(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleKeyDownEvent((int)wParam);
	
	return 0L;
}


LRESULT CHistoryWindow::OnKeyUpEventNotifier(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleKeyUpEvent((int)wParam);
	
	return 0L;
}


LRESULT CHistoryWindow::OnCharEventNotifier(
		UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleCharEvent((int)wParam);
	
	return 0L;
}


void CHistoryWindow::GetPreviousEvent(HTREEITEM hSelectedItem)
{
	HTREEITEM hPrevious;
	
	if (!hSelectedItem)
	{
		m_HistoryTreeView.SelectItem(m_HistoryTreeView.GetChildItem(m_HistoryTreeView.GetChildItem(m_HistoryTreeView.GetRootItem())));
	}
	else
	{
		hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
		if (!hPrevious)//Selecionado nao possui filhos: folha
		{
			hPrevious = m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_PREVIOUS);
			if (hPrevious) //Há anterior direto
			{
				m_HistoryTreeView.SelectItem(hPrevious);
			} 
			else
			{
				hSelectedItem = m_HistoryTreeView.GetParentItem(hSelectedItem);
				if (m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_PREVIOUS)) //Há anterior para o pai
				{
					hPrevious = m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_PREVIOUS);
					hPrevious = m_HistoryTreeView.GetChildItem(hPrevious);
					hSelectedItem = GetLast(hPrevious);
					m_HistoryTreeView.SelectItem(hSelectedItem);
				}
				else 
				{
					if (!m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem), TVGN_PREVIOUS))
					{
						hSelectedItem = m_HistoryTreeView.GetRootItem();
						hPrevious = m_HistoryTreeView.GetRootItem();
					}
					else
					{
						hPrevious = m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem), TVGN_PREVIOUS);
						hPrevious = m_HistoryTreeView.GetChildItem(hPrevious);
					}
					hSelectedItem = GetLast(hPrevious); 
					hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
					hSelectedItem = GetLast(hPrevious); 
					if (m_HistoryTreeView.GetChildItem(hSelectedItem))
					{
						hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
						hSelectedItem = GetLast(hPrevious); 
					}
					m_HistoryTreeView.SelectItem(hSelectedItem);
				}
			}
		}
		else
		{
			hPrevious = m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_PREVIOUS);
			if (hPrevious) //Há anterior direto
			{
				hPrevious = m_HistoryTreeView.GetChildItem(hPrevious);
				hSelectedItem = GetLast(hPrevious);
				if (m_HistoryTreeView.GetChildItem(hSelectedItem))
				{
					hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
					hSelectedItem = GetLast(hPrevious);
				}
				m_HistoryTreeView.SelectItem(hSelectedItem);
			} 
			else if ((!m_HistoryTreeView.GetParentItem(hSelectedItem))||(!m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem),TVGN_PREVIOUS)))
			{
				hSelectedItem = m_HistoryTreeView.GetRootItem();
				hPrevious = m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_NEXT);
				if (hPrevious)
				{
					hSelectedItem = GetLast(hPrevious); //vai para o último mês da árvore
					hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
					hSelectedItem = GetLast(hPrevious); //vai para o último dia do mês
					if (m_HistoryTreeView.GetChildItem(hSelectedItem))
					{
						hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
						hSelectedItem = GetLast(hPrevious); //vai para a última conversa do dia
					}
					m_HistoryTreeView.SelectItem(hSelectedItem);
				}
				else
				{				
					hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
					if (m_HistoryTreeView.GetChildItem(hPrevious))
					{
						hPrevious = m_HistoryTreeView.GetChildItem(hPrevious);
					}
					m_HistoryTreeView.SelectItem(hPrevious);
				}
			}
			else if (m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem), TVGN_PREVIOUS))
			{
				hPrevious = m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem), TVGN_PREVIOUS);
				hPrevious = m_HistoryTreeView.GetChildItem(hPrevious);
				hSelectedItem = GetLast(hPrevious); //vai para o último dia do mês
				hPrevious = m_HistoryTreeView.GetChildItem(hSelectedItem);
				hSelectedItem = GetLast(hPrevious); //vai para a última conversa do dia
				m_HistoryTreeView.SelectItem(hSelectedItem);
			}
		}
	}
}


void CHistoryWindow::GetNextEvent(HTREEITEM hSelectedItem)
{
	HTREEITEM hNext;
	
	if (!hSelectedItem)
	{
		m_HistoryTreeView.SelectItem(m_HistoryTreeView.GetChildItem(m_HistoryTreeView.GetChildItem(m_HistoryTreeView.GetRootItem())));
	}
	else
	{
		hNext = m_HistoryTreeView.GetChildItem(hSelectedItem);
		if (!hNext)
		{
			hNext = m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_NEXT);
			if (hNext) 
			{
				m_HistoryTreeView.SelectItem(hNext);
			} 
			else 
			{
				hSelectedItem = m_HistoryTreeView.GetParentItem(hSelectedItem);
				if (m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_NEXT)) 
				{
					hNext = m_HistoryTreeView.GetNextItem(hSelectedItem, TVGN_NEXT);
					hNext = m_HistoryTreeView.GetChildItem(hNext);
					m_HistoryTreeView.SelectItem(hNext);
				}
				else 
				{
					if (!m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem), TVGN_NEXT))
					{
						hSelectedItem = m_HistoryTreeView.GetRootItem();
						hNext = m_HistoryTreeView.GetChildItem(hSelectedItem);
					}
					else
					{
						hNext = m_HistoryTreeView.GetNextItem(m_HistoryTreeView.GetParentItem(hSelectedItem), TVGN_NEXT);
						hNext = m_HistoryTreeView.GetChildItem(hNext);
					}
					hNext = m_HistoryTreeView.GetChildItem(hNext);
					if (m_HistoryTreeView.GetChildItem(hNext))
					{
						hNext = m_HistoryTreeView.GetChildItem(hNext);
					}
					m_HistoryTreeView.SelectItem(hNext);
				}
			}
		}
		else
		{
			if (m_HistoryTreeView.GetChildItem(hNext))
			{
				hNext = m_HistoryTreeView.GetChildItem(hNext);
			}
			m_HistoryTreeView.SelectItem(hNext);
		}
	}
}


HTREEITEM CHistoryWindow::GetLast(HTREEITEM hItem)
{
	HTREEITEM hSelectedItem = NULL;

	while (hItem)
	{
		hSelectedItem = hItem;
		hItem = m_HistoryTreeView.GetNextItem(hItem, TVGN_NEXT);
	}
	return hSelectedItem;
}


void CHistoryWindow::InsertTreeItens(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CString strMonths[12];
	strMonths[0] = "Janeiro";
	strMonths[1] = "Fevereiro";
	strMonths[2] = "Março";
	strMonths[3] = "Abril";
	strMonths[4] = "Maio";
	strMonths[5] = "Junho";
	strMonths[6] = "Julho";
	strMonths[7] = "Agosto";
	strMonths[8] = "Setembro";
	strMonths[9] = "Outubro";
	strMonths[10] = "Novembro";
	strMonths[11] = "Dezembro";

	// Verify if window exists, maybe user closed window and the thread is still running
	if (!IsWindow())
	{
		return;
	}

	HTREEITEM hMonthYear;
	HTREEITEM hDate;

	BOOL bShowTime = m_pHistoryRecordList->GetShowTime();
	CTime EventTime = pHistoryRecord->GetTime();
	CString strMonthName = strMonths[EventTime.GetMonth() - 1];
	CString strMonthYear;
	strMonthYear.Format("%s %d", strMonthName, EventTime.GetYear());
	CString strDate = EventTime.Format(_T("%d/%m"));
	CString strTime = EventTime.Format(_T("%H:%M"));

	DWORD dwTime = EventTime.GetTime();

	if (0 != m_strLastMonthYearItem.Compare(strMonthYear))
	{
		hMonthYear = m_HistoryTreeView.InsertItem(strMonthYear, 0, TVI_FIRST);
		m_HistoryTreeView.SetItemData(hMonthYear, TREEVIEW_MONTH_NODE);
		m_strLastMonthYearItem = strMonthYear;
		m_hLastMonthYearItem = hMonthYear;
	}

	if (0 != m_strLastDateItem.Compare(strDate))
	{
		hDate = m_HistoryTreeView.InsertItem(strDate, m_hLastMonthYearItem, TVI_FIRST);
		m_HistoryTreeView.SetItemData(hDate, TREEVIEW_DATE_NODE);
		m_strLastDateItem = strDate;
		m_hLastDateItem = hDate;
	}

	if (bShowTime)
	{
		hDate = m_HistoryTreeView.InsertItem(strTime, m_hLastDateItem, TVI_FIRST);
		m_HistoryTreeView.SetItemData(hDate, dwTime);
	}
	else
	{
		m_HistoryTreeView.SetItemData(m_hLastDateItem, dwTime);
	}
}


void CHistoryWindow::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
}


CString CHistoryWindow::GetProtocolId()
{
	return m_pHistoryRecordList->GetProtocolId();
}


void CHistoryWindow::OnFindPanelTextChanged(CString strFindText)
{
	if (0 != m_strFindText.CompareNoCase(strFindText))
	{
		m_strFindText = strFindText;
		FilterTreeItens();
		HighLightFindText();

		if (!m_strFindText.IsEmpty())
		{
			FocusFindHistoryPanel();
		}
	}
}


void CHistoryWindow::OnFindPanelArrowKeyPressed()
{
	m_RichEdit.SetFocus();
}


void CHistoryWindow::OnFindPanelEscPressed()
{
	m_RichEdit.SetFocus();
}


void CHistoryWindow::OnUpdateHistoryRecordListStart(IUOLMessengerHistoryRecordPtr pHistoryRecord)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_lockLog);

	InsertTreeItens(pHistoryRecord);
	
	// Call this method while populating treeview to avoid slow search
	pHistoryRecord->GetHistoryText();
}


void CHistoryWindow::OnUpdateHistoryRecordListFinish()
{
	if (m_HistoryTreeView.GetCount() == 0)
	{
		ShowWarningEmptyHistory();
	}
	else
	{
		if (!m_bIsShowingHistoryText)
		{
			HTREEITEM hMonthYear = m_HistoryTreeView.GetRootItem();
			if (hMonthYear)
			{
				GetPreviousEvent(m_HistoryTreeView.GetRootItem());
			}
		}

		// Show Find Panel and navigation buttons
		CFindPanelButtonPtr pFindPanelButton = 
			ap_dynamic_cast<CFindPanelButtonPtr>(
			m_pFinderToolbar->FindImageButton("SearchHistory"));

		if (pFindPanelButton)
		{
			pFindPanelButton->SetVisible(TRUE);
		}

		IUOLMessengerImageButtonPtr pImageNextButton = 
			m_pFinderToolbar->FindImageButton("FindNextHistory");
		if (pImageNextButton)
		{
			pImageNextButton->SetVisible(TRUE);
		}

		IUOLMessengerImageButtonPtr pImagePriorButton = 
			m_pFinderToolbar->FindImageButton("FindPriorHistory");
		if (pImagePriorButton)
		{
			pImagePriorButton->SetVisible(TRUE);
		}
	}
}


IUOLMessengerImageButtonContainerPtr CHistoryWindow::GetToolbar()
{
	return m_pToolbar;
}


IUOLMessengerHistoryContactPtr CHistoryWindow::getContact()
{
	// Deprecated method.
	ATLASSERT(FALSE);
	
	return NULL;
}


void CHistoryWindow::SetContact(IUOLMessengerHistoryContactPtr pHistContact)
{
	// Deprecated method.
	ATLASSERT(FALSE);
}


void CHistoryWindow::RegisterObserver(CUOLMessengerHistoryWindowObserver* pObserver)
{
	m_ObserverNotifier.RegisterObserver(pObserver);
}


void CHistoryWindow::UnregisterObserver(CUOLMessengerHistoryWindowObserver* pObserver)
{
	m_ObserverNotifier.UnregisterObserver(pObserver);
}


void CHistoryWindow::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	HTREEITEM hSelectedItem = m_HistoryTreeView.GetSelectedItem();

	CString strMessage;
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	IUOLMessengerHistoryManager4Ptr pHistoryManager5;
	pHistoryManager5 = ap_dynamic_cast<IUOLMessengerHistoryManager4Ptr>(CUIMApplication::GetApplication()->GetHistoryManager());

	CString strCommand = pButton->GetCommandName();
	if (strCommand.CompareNoCase(_T("PreviousHistory")) == 0)
	{
		GetPreviousEvent(hSelectedItem);
	}
	else if (strCommand.CompareNoCase(_T("NextHistory")) == 0)
	{
		GetNextEvent(hSelectedItem);
	} 
	else if (strCommand.CompareNoCase(_T("DeleteHistory")) == 0)
	{
		strMessage = pStringLoader->GetValue("uol.resource.IDS_DELETE_HISTORY_COMMAND_WARNING_DEL_HISTORY");

		if ((CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES)) 
		{
			m_pHistoryRecordList->DeleteHistory(GetSelectedEvent());
			UpdateEventsList();
		}
	} 
	else if (strCommand.CompareNoCase(_T("DeleteAllHistory")) == 0)
	{
		strMessage = pStringLoader->GetValue("uol.resource.IDS_DELETE_ALL_HISTORY_COMMAND_WARNING_DELETE_HISTORY");

		if ((CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES)) 
		{
			m_pHistoryRecordList->DeleteAllHistory();
			UpdateEventsList();
		}
	}
	else if (strCommand.CompareNoCase(_T("FindNextHistory")) == 0)
	{
		GotoFindText(ftdNext);
	}
	else if (strCommand.CompareNoCase(_T("FindPriorHistory")) == 0)
	{
		GotoFindText(ftdPrior);
	}
	else
	{
		ATLASSERT(FALSE);
	}
}


void CHistoryWindow::SetHistoryRecordList(IUOLMessengerHistoryRecordListPtr pHistoryRecordList)
{
	m_pHistoryRecordList = pHistoryRecordList;
	m_pHistoryRecordList->RegisterObserver(this);
}


void CHistoryWindow::UpdateEventsList()
{
	ClearTreeItens();
	m_pHistoryRecordList->StartLoad();
}


void CHistoryWindow::FocusFindHistoryPanel()
{
	CFindPanelButtonPtr pFindPanelButton = 
			ap_dynamic_cast<CFindPanelButtonPtr>(
					m_pFinderToolbar->FindImageButton("SearchHistory"));
	
	if (pFindPanelButton)
	{
		pFindPanelButton->SetFocus();
	}
}


int CHistoryWindow::GetSelectedEvent()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_lockLog);

	int nIndex;
	HTREEITEM hSelectedItem = m_HistoryTreeView.GetSelectedItem();
	while (hSelectedItem)
	{
		nIndex = m_HistoryTreeView.GetItemData(hSelectedItem);
		if (nIndex > TREEVIEW_MONTH_NODE)
		{
			return nIndex;
		}
		else
		{
			hSelectedItem = m_HistoryTreeView.GetChildItem(hSelectedItem);
		}
	}

	return -1;
}


void CHistoryWindow::HighLightFindText()
{
	m_listFoundTextPos.RemoveAll();
	m_nFoundTextIndex = 0;

	if ((!m_strFindText.IsEmpty()) && m_bIsShowingHistoryText)
	{
		// FINDTEXTEX structure
		FINDTEXTEX ft;
		ft.chrg.cpMin = 0;
		ft.chrg.cpMax = -1;
		ft.lpstrText = (LPSTR) m_strFindText.GetBuffer();

		// Move cursor to first position in text
		m_RichEdit.SetSel(0, 0);
		long n = 0;

		// Find ALL
		while (n != -1)
		{
			m_RichEdit.HideSelection(TRUE, FALSE);

			n = m_RichEdit.FindText(FR_NOMATCHCASE | FR_NOWHOLEWORD | FR_DOWN, ft);

			if (n != -1)
			{
				m_RichEdit.SetSel(ft.chrgText);
				ft.chrg.cpMin = ft.chrgText.cpMax;

				m_listFoundTextPos.AddTail(ft.chrgText);

				// HighLight
				CHARFORMAT2 cfm;
				cfm.cbSize = sizeof(cfm);
				m_RichEdit.GetSelectionCharFormat(cfm);

				cfm.dwEffects = CFE_BOLD;
				cfm.crBackColor = m_clrHighlightText;
				cfm.dwMask = CFE_AUTOBACKCOLOR;
				m_RichEdit.SetSelectionCharFormat(cfm);
			}
		}
		m_strFindText.ReleaseBuffer();

		GotoFindText(ftdFirst);
	}
}


void CHistoryWindow::HandleKeyDownEvent(int vKey)
{
	if (vKey == VK_CONTROL)
	{
		m_bIsCtrlKeyPressed = TRUE;
	}
}


void CHistoryWindow::HandleKeyUpEvent(int vKey)
{
	if (vKey == VK_ESCAPE)
	{
		CFindPanelButtonPtr pFindPanelButton = 
				ap_dynamic_cast<CFindPanelButtonPtr>(
						m_pFinderToolbar->FindImageButton("SearchHistory"));
		
		if (pFindPanelButton)
		{
			pFindPanelButton->SendMessage(CFindContactPanel::WM_FIND_PANEL_ESC_PRESSED, 0, 0);
		}
	}
	else if (vKey == VK_CONTROL)
	{
		m_bIsCtrlKeyPressed = FALSE;
	}
	else if ( (vKey == (int)'F') && (m_bIsCtrlKeyPressed) )
	{
		FocusFindHistoryPanel();
	}
}


void CHistoryWindow::HandleCharEvent(int vKey)
{
	TCHAR ch = vKey;
	char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*()[{}]/?\\|";
	
	if (strchr(validChars, ch))
	{
		CString strText(ch);
		
		CFindPanelButtonPtr pFindPanelButton = 
				ap_dynamic_cast<CFindPanelButtonPtr>(
						m_pFinderToolbar->FindImageButton("SearchHistory"));
		
		if (pFindPanelButton)
		{
            pFindPanelButton->SetFocus();
			pFindPanelButton->SetText(strText);
        }
	}
}


void CHistoryWindow::FilterTreeItens()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_lockLog);

	ClearTreeItens();
	
	for (int nItem = 0; nItem < m_pHistoryRecordList->GetCount(); nItem++)
	{
		// Filter Tree by find text
		IUOLMessengerHistoryRecordPtr pHistoryRecord;
		pHistoryRecord = m_pHistoryRecordList->GetHistoryRecordByIndex(nItem);
		CString strHistoryText = pHistoryRecord->GetHistoryText();
		CString strFindText = m_strFindText;
		strHistoryText.MakeLower();
		strFindText.MakeLower();
		if ((strFindText.IsEmpty()) || (-1 != strHistoryText.Find(strFindText)))
		{			
			InsertTreeItens(pHistoryRecord);
		}
	}

	if (m_HistoryTreeView.GetCount() == 0)
	{
		ShowWarningEmptyHistory();
	}
	else
	{
		HTREEITEM hMonthYear = m_HistoryTreeView.GetRootItem();
		if (hMonthYear)
		{
			GetPreviousEvent(m_HistoryTreeView.GetRootItem());
		}
	}
}


void CHistoryWindow::ShowWarningEmptyHistory()
{
	UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strMessage;
	strMessage = pStringLoader->GetValue("uol.resource.IDS_EMPTY_HISTORY_WARNING");
	m_RichEdit.SetWindowText(strMessage);
	m_bIsShowingHistoryText = FALSE;
}


void CHistoryWindow::GotoFindText(FoundTextDirection direction)
{
	int nFoundTextCount = m_listFoundTextPos.GetCount() - 1;
	
	// Nothing found, return
	if (nFoundTextCount < 0)
	{
		return;
	}

	HTREEITEM hSelectedItem = m_HistoryTreeView.GetSelectedItem();

	switch (direction)
	{
	case ftdFirst:
		m_nFoundTextIndex = 0;
		break;
	case ftdNext:
		{
			m_nFoundTextIndex++;
			if (m_nFoundTextIndex > nFoundTextCount) 
			{
				m_nFoundTextIndex = 0;
				GetNextEvent(hSelectedItem);
				return;
			}
			break;
		}
	case ftdPrior:
		{
			m_nFoundTextIndex--;
			if (m_nFoundTextIndex < 0) 
			{
				m_nFoundTextIndex = 0;
				GetPreviousEvent(hSelectedItem);
				return;
			}
			break;
		}
	default: 
		m_nFoundTextIndex = 0;
	}

	POSITION pos = m_listFoundTextPos.FindIndex(m_nFoundTextIndex);
	if (pos)
	{
		CHARRANGE crFindSel = m_listFoundTextPos.GetAt(pos);
		m_RichEdit.SetSel(crFindSel);
		m_RichEdit.HideSelection(FALSE, FALSE);
		m_RichEdit.SetFocus();
		m_RichEdit.SendMessage(EM_SCROLLCARET);		
	}
}
