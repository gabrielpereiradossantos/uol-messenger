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
#include "NotifyPopUp.h"

#include "NotifyPopUpCaptionBuilder.h"
#include "../../resource.h"
#include "../skin/solidbackgroundelement.h"





CNotifyPopUpObserverNotifier::CNotifyPopUpObserverNotifier(CNotifyPopUp* pNotifyPopUp) :
	m_pNotifyPopUp(pNotifyPopUp)
{
}


void CNotifyPopUpObserverNotifier::NotifyAll(void (CNotifyPopUpObserver::*pEventHandler)(CNotifyPopUp* pNotifyPopUp))
{
	CAtlList<CNotifyPopUpObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}
		
	CNotifyPopUpObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pEventHandler)(m_pNotifyPopUp);
	}
}







CNotifyPopUp::CNotifyPopUp() : 
m_observerNotifier(this),
m_nIDTimer(1),
m_nTimerInterval(50),
m_bExpired(FALSE),
m_bProtected(FALSE),
m_bFirstTime(TRUE),
m_dwShowTime(0),
m_dwTimeout(0)
{
}


CNotifyPopUp::~CNotifyPopUp(void)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	if(m_scrollView.IsWindow())
	{
		m_scrollView.DestroyWindow();
	}

	m_scrollView.m_hWnd = NULL;
}


void CNotifyPopUp::SetNotifyPopUpHandler(IUOLMessengerNotifyPopUpHandlerPtr pNotifyPopUpHandler)
{
	m_pNotifyPopUpHandler = pNotifyPopUpHandler;
}


void CNotifyPopUp::Create(HWND hParent)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	DWORD dwStyle;
	CRect rectWnd(CPoint(0, 0), CSize(300, 68));
	dwStyle = WS_POPUP;

	__super::Create(hParent, &rectWnd, "UOL Messenger", dwStyle);

	// Get border size
	CSize sizeBorder = CSize(0, 0);
	if (m_pBorder)
	{
		m_pBorder->GetSize(sizeBorder);
	}

	CRect rect;

	// Get caption builder
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		IElement* pElement = m_listChilds.GetNext(pos).get();

		if (0 == pElement->GetName().CompareNoCase(_T("__NotifyPopUpCaptionBuilderInstance")))
		{
			CNotifyPopUpCaptionBuilder*	pNotifyPopUpCaptionBuilder = dynamic_cast<CNotifyPopUpCaptionBuilder*>(pElement);
			ATLASSERT(pNotifyPopUpCaptionBuilder != NULL);

			m_pNotifyPopUpCaption = ap_dynamic_cast<CNotifyPopUpCaptionPtr>(pNotifyPopUpCaptionBuilder->CreateElement());
			break;
		}
	}

	ATLASSERT(m_pNotifyPopUpCaption.get());

	CString strTitle;
	IUOLMessengerNotifyPopUpHyperLinkHandlerPtr pOptionsLinkHandler;

	ATLASSERT(m_pNotifyPopUpHandler.get());

	if (m_pNotifyPopUpHandler)
	{
		pOptionsLinkHandler = m_pNotifyPopUpHandler->GetTextHyperLinkHandler();
		strTitle = m_pNotifyPopUpHandler->GetTitle();

		m_pNotifyPopUpCaption->SetTitle(strTitle);
		m_pNotifyPopUpCaption->ShowOptionLink(pOptionsLinkHandler != NULL);

		CRect rectCaption = CRect(CPoint(sizeBorder.cx, sizeBorder.cy), 
								CSize(rectWnd.Width() - (2 * sizeBorder.cx), m_pNotifyPopUpCaption->GetHeight()));
		m_pNotifyPopUpCaption->Create(m_hWnd, rectCaption, NULL, WS_VISIBLE|WS_CHILD, WS_EX_TRANSPARENT);

		// Create scroll view...
		rect.top = rectCaption.bottom;
		rect.bottom = rectWnd.bottom - sizeBorder.cy;
		rect.left = sizeBorder.cx;
		rect.right = rectWnd.right - sizeBorder.cx;

		CAtlList<CString> listTextLines;
		m_pNotifyPopUpHandler->GetTextLines(listTextLines);

		CString strText;

		for (POSITION pos = listTextLines.GetHeadPosition(); pos; )
		{
			strText = listTextLines.GetNext(pos);
			m_scrollView.AddLink(strText, strText);
		}

		m_scrollView.SetMessageIcon(m_pNotifyPopUpHandler->GetMessageIcon());
		m_scrollView.SetMessageIconDrawStrategy(m_pNotifyPopUpHandler->GetMessageIconDrawStrategy());
		m_scrollView.SetWindowIcon(m_pNotifyPopUpHandler->GetWindowIcon());
		m_scrollView.SetWindowIconDrawStrategy(m_pNotifyPopUpHandler->GetWindowIconDrawStrategy());

		m_scrollView.Create(m_hWnd, rect, NULL, WS_VISIBLE|WS_CHILD, WS_EX_TRANSPARENT);
	}
}


DWORD CNotifyPopUp::GetShowTime()
{
	return m_dwShowTime;
}

void CNotifyPopUp::SetShowTime(DWORD dwShowTime)
{
	m_dwShowTime = dwShowTime;
}

DWORD CNotifyPopUp::GetTimeout()
{
	return m_dwTimeout;
}

void CNotifyPopUp::SetTimeout(DWORD dwTimeout)
{
	m_dwTimeout = dwTimeout;
}

BOOL CNotifyPopUp::IsExpired()
{
	return m_bExpired;
}

void CNotifyPopUp::SetExpired(BOOL bExpired)
{
	m_bExpired = bExpired;
}

BOOL CNotifyPopUp::IsProtected()
{
	return m_bProtected;
}

void CNotifyPopUp::SetProtected(BOOL bProtected)
{
	m_bProtected = bProtected;
}

BOOL CNotifyPopUp::IsFirstTime()
{
	return m_bFirstTime;
}

void CNotifyPopUp::SetFirstTime(BOOL bFirstTime)
{
	m_bFirstTime = bFirstTime;
}

LRESULT CNotifyPopUp::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	RECT rectClient;

	// Load default brush;
	m_brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	GetClientRect( &rectClient );

	m_size.cx = rectClient.right;    // zero based
	m_size.cy = rectClient.bottom;    // zero based
	m_pt.x = rectClient.left;
	m_pt.y = rectClient.top;

	return 1;
}

void CNotifyPopUp::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	if(m_scrollView.IsWindow())
	{
		m_scrollView.DestroyWindow();
	}

	if ((m_pNotifyPopUpCaption != NULL) &&
		(m_pNotifyPopUpCaption->IsWindow()))
	{
		m_pNotifyPopUpCaption->DestroyWindow();
	}

	m_scrollView.m_hWnd = NULL;
}


LRESULT CNotifyPopUp::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_observerNotifier.NotifyAll(&CNotifyPopUpObserver::OnClose);

	return 0L;
}


LRESULT CNotifyPopUp::OnOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	if (m_pNotifyPopUpHandler)
	{
		IUOLMessengerNotifyPopUpHyperLinkHandlerPtr pOptionsHyperLinkHandler = m_pNotifyPopUpHandler->GetOptionsHyperLinkHandler();
		
		if (pOptionsHyperLinkHandler)
		{
			pOptionsHyperLinkHandler->HandleClick();
		}
	}

	m_observerNotifier.NotifyAll(&CNotifyPopUpObserver::OnOptions);

	return 0L;
}


LRESULT CNotifyPopUp::OnMouseDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	return 0L;
}


LRESULT CNotifyPopUp::OnHyperlinkButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	HWND hWnd = (HWND) wParam;

	if (hWnd == m_scrollView)
	{
		if (m_pNotifyPopUpHandler)
		{
			IUOLMessengerNotifyPopUpHyperLinkHandlerPtr pTextHyperLinkHandler = m_pNotifyPopUpHandler->GetTextHyperLinkHandler();
		
			if (pTextHyperLinkHandler)
			{
				pTextHyperLinkHandler->HandleClick();
			}
		}

		m_observerNotifier.NotifyAll(&CNotifyPopUpObserver::OnClick);
	}

	//PostMessage(WM_CLOSE);

	return 0L;
}



// IUOLMessengerNotifyPopUp methods
void CNotifyPopUp::AddNotifyPopUpObserver(CNotifyPopUpObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CNotifyPopUp::RemoveNotifyPopUpObserver(CNotifyPopUpObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CNotifyPopUp::UpdateLayout(BOOL bResizeBars)
{
	if (!IsWindow())
	{
		return;
	}

	CRect rect;
	GetClientRect(&rect);
	
	CSize size(rect.Width(), rect.Height());

	CSize sizeBorder = CSize(0, 0);
	if (m_pBorder)
	{
		m_pBorder->GetSize(sizeBorder);
	}

	int x = sizeBorder.cx;
	int y = sizeBorder.cy;

	CSize sizeElement(0, 0);
	int cx = size.cx - (2 * sizeBorder.cx);

	if (m_pNotifyPopUpCaption.get())
	{
		m_pNotifyPopUpCaption->GetMinSize(sizeElement);

		m_pNotifyPopUpCaption->SetWindowPos(NULL, x, y, cx, sizeElement.cy, SWP_NOZORDER);
		y += sizeElement.cy;
	}

	if (m_scrollView.IsWindow())
	{
		int cy = size.cy - (2 * sizeBorder.cy) - sizeElement.cy;
		m_scrollView.SetWindowPos(NULL, x, y, cx, cy, SWP_NOZORDER);
	}
}


CString CNotifyPopUp::GetName() const
{
	return "__NotifyPopUpInstance";
}


void CNotifyPopUp::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strBorder;
		GetParameterValue(strBorder, pMapParams, "border");
		
		m_pBorder = ap_dynamic_cast<CBorderElementPtr>(pElementTable->GetElement(strBorder));
		ATLASSERT(m_pBorder.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "border");
	}

	m_scrollView.SetParameters(pMapParams, pElementTable);
}


void CNotifyPopUp::DoPaint(CDCHandle& dcHandle)
{
	if (m_pBorder)
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		CBrush brush;
		brush.CreateSolidBrush(m_pBorder->GetCOLORREF());

		dcHandle.FillRect(&rectClient, brush);
	}
}