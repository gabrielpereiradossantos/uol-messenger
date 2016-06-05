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
 
// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../resource.h"

#include "MainFrm.h"

#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"

#include "windowcaptionbuilder.h"
#include "uimstatusbarbuilder.h"

#include "skin/AppDefaultIconElement.h"

#include <commands/MinimizeWindowCommand.h>
#include <commands/MaximizeWindowCommand.h>
#include <commands/CloseWindowCommand.h>
#include <commands/hidewindowcommand.h>
#include <commands/showwindowcommand.h>


static const int ROUND_FACTOR = 15;

CMainFrame::CMainFrame() :
	m_pCurrentBorder(NULL),
	m_bResizable(TRUE), 
	m_bShowStatusBar(TRUE),
	m_bCustomRgnEnabled(TRUE)
{
}


CMainFrame::~CMainFrame()
{
}


void CMainFrame::SetCaptionText(const CString& strText)
{
	if (m_pWindowCaption.get())
	{
		m_pWindowCaption->SetText(strText);		
		SetWindowText(strText);
	}
}


CString CMainFrame::GetCaptionText() const
{
	if (m_pWindowCaption.get())
	{
		return m_pWindowCaption->GetText();
	}

	return _T("");
}


void CMainFrame::SetStatusBarText(const CString& strText)
{
	if (m_pStatusbar.get())
	{
		m_pStatusbar->SetText(strText);
	}
}


CString CMainFrame::GetStatusBarText() const
{
	if (m_pStatusbar.get())
	{
		return m_pStatusbar->GetText();
	}

	return _T("");
}


CWindowCaptionPtr CMainFrame::GetWindowCaption() const
{
	return m_pWindowCaption;
}

UINT CMainFrame::RegisterCustomWindowMessage()
{	
	return m_mapUserMessage.GetNextID();
}

void CMainFrame::UnregisterCustomWindowMessage(UINT nId)
{
	m_mapUserMessage.ReleaseID(nId);
}

void CMainFrame::CallCustomWindowMessage(UINT nId, IAppThreadCallback* pCallBack)
{
	SendMessage(CUSTOM_USER_MESSAGE, 
				nId, 
				reinterpret_cast<LPARAM>(pCallBack));
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (_CMainFrameBase::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	//if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST))
	//{
	//	return FALSE;
	//}

	//if ((pMsg->message == WM_KEYDOWN) &&
	//	(pMsg->wParam == VK_ESCAPE) &&
	//	(IsChild(pMsg->hwnd)))
	//{
	//	ATLTRACE(_T("Closing window with ESC key\n"));
	//	PostMessage(WM_CLOSE);
	//	return TRUE;
	//}

	return m_pView->PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	//UIUpdateToolBar();
	return FALSE;
}

void CMainFrame::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);

	// Calculate client rect without menu bar and status bar
	int x = rect.left;
	int y = rect.top;
	int cx = rect.Width();	
	int cy = rect.Height();	

	CSize sizeBorder = CSize(0, 0);
	CSize sizeStatusbar = CSize(0, 0);
	CSize sizeWindowCaption = CSize(0, 0);	
	CSize sizeRebar = CSize(0, 0);	

	if (m_pCurrentBorder)
	{		
		m_pCurrentBorder->GetSize(sizeBorder);

		x += sizeBorder.cx;
		y += sizeBorder.cy;

		cx -= 2 * sizeBorder.cx;
		cy -= 2 * sizeBorder.cy;
	}
	

	if (m_pWindowCaption.get() && (m_pWindowCaption->GetWindow()))
	{		
		m_pWindowCaption->GetMinSize(sizeWindowCaption);

		m_pWindowCaption->SetWindowPos(NULL, 
									   x, y, 
									   cx, sizeWindowCaption.cy, 
									   SWP_NOZORDER); 								

		y += sizeWindowCaption.cy;
		cy -= sizeWindowCaption.cy;
	}	

	if (::IsWindow(m_hWndToolBar))
	{
		sizeRebar = CSize(cx, 21);

		if (bResizeBars)
		{		
			::SetWindowPos(m_hWndToolBar, 
						NULL, 
						x, y, 
						cx, sizeRebar.cy, 
						SWP_NOZORDER | SWP_NOSENDCHANGING);
		}
		

		y += sizeRebar.cy;
		cy -= sizeRebar.cy;
	}

	if (m_pStatusbar.get() && (m_pStatusbar->GetWindow()))
	{		
		m_pStatusbar->GetMinSize(sizeStatusbar);	

		m_pStatusbar->SetWindowPos(NULL, 
								   x, rect.Height() - sizeStatusbar.cy - sizeBorder.cy, 
								   cx, sizeStatusbar.cy, 
								   SWP_NOZORDER); 	

		cy -= sizeStatusbar.cy;
	}	
	
	// resize client window
	if (m_hWndClient != NULL)
	{
		::SetWindowPos(m_hWndClient, 
			NULL, 
			x, y,
			cx, cy,
			SWP_NOZORDER | SWP_NOACTIVATE);

	}		

	SetWindowRgn(GetClippingRgn(rect), TRUE);	
}

void CMainFrame::GetMinSize(CSize& size)
{
	int cx = 0;
	int cy = 0;

	if (m_bResizable)
	{
		cy += 2 * ::GetSystemMetrics(SM_CYSIZEFRAME);
		cx += 2 * ::GetSystemMetrics(SM_CXSIZEFRAME);
	}
	else
	{
		cy += 2 * ::GetSystemMetrics(SM_CYFIXEDFRAME);
		cx += 2 * ::GetSystemMetrics(SM_CXFIXEDFRAME);
	}

	cy += ::GetSystemMetrics(SM_CYCAPTION);

	if (m_pMenu.get())
	{
		cy += ::GetSystemMetrics(SM_CYMENU);
	}

	if (m_pCurrentBorder)
	{
		CSize sizeBorder;
		m_pCurrentBorder->GetSize(sizeBorder);

		cy += 2 * sizeBorder.cy;
		cx += 2 * sizeBorder.cx;
	}

	if (m_pWindowCaption.get() && (m_pWindowCaption->GetWindow()))
	{		
		CSize sizeWindowCaption;
		m_pWindowCaption->GetMinSize(sizeWindowCaption);

		cy += sizeWindowCaption.cy;
	}

	if (m_pStatusbar.get() && (m_pStatusbar->GetWindow()))
	{		
		CSize sizeStatusbar;
		m_pStatusbar->GetMinSize(sizeStatusbar);	

		cy += sizeStatusbar.cy;
	}

	if (m_pView.get())
	{
		CSize sizeView;
		m_pView->GetMinSize(sizeView);	

		cy += sizeView.cy;
		cx += sizeView.cx;
	}

	size.cx = cx;
	size.cy = cy;
}

BOOL CMainFrame::IsRgnEnabled() const
{
	return m_bCustomRgnEnabled;
}

BOOL CMainFrame::EnableRgn(BOOL bEnable)
{
	BOOL bRet = m_bCustomRgnEnabled;

	m_bCustomRgnEnabled = bEnable;	

	UpdateLayout();
		
	return bRet;
}


LRESULT CMainFrame::OnCreate(LPCREATESTRUCT lpCs)
{
	/*static CCommandBarCtrl		m_CmdBar;
	m_CmdBar.Create(m_hWnd, 
						rcDefault, 
						NULL, 
						ATL_SIMPLE_CMDBAR_PANE_STYLE);

	m_CmdBar.AttachMenu(GetMenu());*/

	// load command bar images
	//m_CmdBar.LoadImages(IDR_MAINFRAME);

	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	CAppDefaultIconElementPtr pAppDefaultIcon = ap_dynamic_cast<CAppDefaultIconElementPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__AppDefaultIconInstance"));

	if (pAppDefaultIcon)
	{
		CIconElementPtr pIconSmall = pAppDefaultIcon->GetSmallIcon();

		if (pIconSmall)
		{
			SetIcon(pIconSmall->GetIcon(), FALSE);
		}

		CIconElementPtr pIconLarge = pAppDefaultIcon->GetLargeIcon();

		if (pIconLarge)
		{
			SetIcon(pIconLarge->GetIcon(), TRUE);
		}
	}

	SetMenu(NULL);

	if (m_pMenu.get())
	{
		SetMenu(m_pMenu->GetMenu());

		//HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		////CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);

		//
		//if (m_rebar.SubclassWindow(m_hWndToolBar))
		//{		
		//	//AddSimpleReBarBand(m_rebar.SetMenu(m_pMenu, m_hWnd));

		//	//AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		//	//UIAddToolBar(hWndToolBar);

		//	//CreateSimpleStatusBar(_T(""));
		//}
		//else
		//{
		//	ATLASSERT(FALSE);
		//}

		// Disable Diagnose Tool menu item if Diagnose Manager isn't present on ctrlcenter.dll
		IUOLMessengerFacade4* pFacade4 = dynamic_cast<IUOLMessengerFacade4*>(
		CUIMApplication::GetApplication());

		CDiagnoseManagerPtr pDiagnoseManager = ap_dynamic_cast<CDiagnoseManagerPtr>(
			pFacade4->GetDiagnoseManager());

		if (!pDiagnoseManager->IsDiagnoseManagerPresent())
		{
			CMenuItemElement* pMenuItem = m_pMenu->FindMenuItemElement(pDiagnoseManager->GetCommandName());
			if (pMenuItem)
			{
				pMenuItem->EnableItem(FALSE);
			}
		}
	}

	ATLASSERT(m_pView.get());

	m_hWndClient = m_pView->CreateWnd(m_hWnd);

	if (m_pWindowCaption.get())
	{
		m_pWindowCaption->Create(m_hWnd);
		m_pWindowCaption->SetRoundCaptionFactor(ROUND_FACTOR);
		m_pWindowCaption->SetWindowWithRegion(this);

		//m_pWindowCaption->SetMenu(m_menu);

		m_pWindowCaption->AddButtonsEventListener(this);

		//CIconElementPtr pIconSmall = m_pWindowCaption->GetSmallIcon();

		//if (pIconSmall)
		//{
		//	SetIcon(pIconSmall->GetIcon(), FALSE);
		//}

		//CIconElementPtr pIconLarge = m_pWindowCaption->GetLargeIcon();

		//if (pIconLarge)
		//{
		//	SetIcon(pIconLarge->GetIcon(), TRUE);
		//}
	}

	if ((m_pStatusbar.get()) && m_bShowStatusBar)
	{
		m_pStatusbar->Create(m_hWnd);		
	}	

	//UISetCheck(ID_VIEW_TOOLBAR, 1);
	//UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);    	

	return 0;
}

void CMainFrame::OnDestroy()
{
	if (m_pWindowCaption.get())
	{
		m_pWindowCaption->RemoveButtonsEventListener(this);
		m_pWindowCaption->DestroyWindow();
	}

	if (m_pStatusbar.get() && m_pStatusbar->IsWindow())
	{		
		m_pStatusbar->DestroyWindow();
	}

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	SetMsgHandled(FALSE);
}

void CMainFrame::OnSize(UINT, const CSize& size)
{
	SetMsgHandled(FALSE);
}

LRESULT CMainFrame::OnNcHitTest(const CPoint& pt)
{
	CRect rectMainFrame;
	GetWindowRect(&rectMainFrame);

	int nTest = HTCLIENT;

	if ((m_bResizable) && (m_bCustomRgnEnabled))
	{
		if (HitTestBorder(pt, nTest))
		{
			return nTest;
		}	
	}
    
	// Hit test caption
	if (m_pWindowCaption.get())
	{
		CRect rectCaption;
		m_pWindowCaption->GetWindowRect(&rectCaption);

		CRect rectIntersect = rectMainFrame & rectCaption;

		if (rectIntersect.PtInRect(pt))
		{
			return HTCAPTION;
		}
	}	

	// Hit test Statusbar
	if (m_pStatusbar.get() && m_pStatusbar->IsWindow())
	{
		CRect rectStatusBar;
		m_pStatusbar->GetWindowRect(&rectStatusBar);

		CRect rectIntersect = rectMainFrame & rectStatusBar;

		if (rectIntersect.PtInRect(pt))
		{
			int nTest;
			m_pStatusbar->HitTest(pt, nTest);
			
			return nTest;
		}
	}

	SetMsgHandled(FALSE);

	return nTest;
}


//LRESULT CMainFrame::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	return 0L;
//}

void CMainFrame::OnClose()
{
	DestroyWindow();	
}

void CMainFrame::OnMove(const CPoint& pt)
{
	SetMsgHandled(FALSE);
}

void CMainFrame::OnWindowPosChanging(LPWINDOWPOS lpWindowPos)
{
	if (~lpWindowPos->flags & SWP_NOMOVE)
	{
		CRect rectDesk;
		::GetWindowRect(::GetDesktopWindow(), &rectDesk);

		CRect rectWindow;
		GetWindowRect(&rectWindow);

		const int nDelta = static_cast<int>(0.01875 * rectDesk.Width());
		const int cx = rectWindow.Width();
		const int cy = rectWindow.Height();
		//const int y = GetRelativeWindowYPos();

		const int cySystemCaption = ::GetSystemMetrics(SM_CYCAPTION);
		const int cxSystemBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
		const int cySystemBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);

		if (((lpWindowPos->x + cx + cxSystemBorder) >= rectDesk.right) && 
			((lpWindowPos->x + cx + cxSystemBorder) <= (rectDesk.right + nDelta)))
		{		
			lpWindowPos->x = rectDesk.right - cx + cxSystemBorder;						
		}

		/*
		if (lpWindowPos->y < -cySystemCaption)
		{		
			lpWindowPos->y = -42;						
		}
		*/

		if ((lpWindowPos->x <= -cxSystemBorder) && 
			((-lpWindowPos->x) <= nDelta))
		{
			lpWindowPos->x = -cxSystemBorder;
		}
	}
	
	SetMsgHandled(FALSE);
}

void CMainFrame::OnCommand(UINT nId, int nCode, HWND /*hWnd*/)
{
	if (m_pMenu.get())
	{
		CMenuItemElement* pMenuItemElement = m_pMenu->GetMenuItemElement(nCode);

		if (pMenuItemElement)
		{
			IUOLMessengerCommandPtr pCommand = GetCommand(pMenuItemElement->GetCommand());

			if (pCommand)
			{
				CUIMApplication::GetApplication()->ExecuteCommand(pCommand);
			}
		}
	}
}

void CMainFrame::OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo)
{
	CSize size;
	GetMinSize(size);
	
	lpMinMaxInfo->ptMinTrackSize.x = size.cx;
	lpMinMaxInfo->ptMinTrackSize.y = size.cy;
}

void CMainFrame::OnCustomUserMessage(WPARAM wParam, LPARAM lParam)
{	
	m_mapUserMessage.Execute(static_cast<UINT>(wParam), 
							 reinterpret_cast<IAppThreadCallback*>(lParam));
}

void CMainFrame::OnInitMenu(HMENU hMenu)
{
	SetMsgHandled(FALSE);
}


void CMainFrame::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	try
	{
		CString strCaption;
		GetParameterValue(strCaption, pMapParams, "caption");	

		CWindowCaptionBuilderPtr pWindowCaptionBuilder = ap_dynamic_cast<CWindowCaptionBuilderPtr>(pElementTable->GetElement(strCaption));
		
		m_pWindowCaption = ap_dynamic_cast<CWindowCaptionPtr>(pWindowCaptionBuilder->CreateElement());
		ATLASSERT(m_pWindowCaption.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "caption");
	}

	try
	{
		CString strStatusbar;
		GetParameterValue(strStatusbar, pMapParams, "statusbar");

		CUIMStatusBarBuilderPtr pUIMStatusBarBuilder = ap_dynamic_cast<CUIMStatusBarBuilderPtr>(pElementTable->GetElement(strStatusbar));
		
		m_pStatusbar = ap_dynamic_cast<CUIMStatusBarCtrlPtr>(pUIMStatusBarBuilder->CreateElement());
		ATLASSERT(m_pStatusbar.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "statusbar");
	}

	m_bShowStatusBar = (m_pStatusbar != NULL);

	try
	{
		CString strMenu;
		GetParameterValue(strMenu, pMapParams, "menu");

		m_pMenu = ap_dynamic_cast<CMenuElementPtr>(pElementTable->GetElement(strMenu));
		ATLASSERT(m_pMenu.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "menu");
	}
	

	try
	{
		CString strBorder;
		GetParameterValue(strBorder, pMapParams, "border");
		
		m_pActiveBorder = ap_dynamic_cast<CBorderElementPtr>(pElementTable->GetElement(strBorder));
		ATLASSERT(m_pActiveBorder.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "border");
	}	


	try
	{
		CString strBorder;
		GetParameterValue(strBorder, pMapParams, "inactive_border");
		
		m_pInactiveBorder = ap_dynamic_cast<CBorderElementPtr>(pElementTable->GetElement(strBorder));
		ATLASSERT(m_pInactiveBorder.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "inactive_border");
		m_pInactiveBorder = m_pActiveBorder;
	}

	if (m_pActiveBorder.get())
	{
		m_pCurrentBorder = m_pActiveBorder.get();
	}



	try
	{
		CString strResizable;
		GetParameterValue(strResizable, pMapParams, "resizable");

		m_bResizable = (0 == strResizable.CompareNoCase("true") || ('1' == strResizable));	
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "resizable");
		m_bResizable = TRUE;
	}
}

void CMainFrame::DoPaint(CDCHandle& dc)
{
	if (m_pCurrentBorder)
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		CBrush brush;
		brush.CreateSolidBrush(m_pCurrentBorder->GetCOLORREF());

		dc.FillRect(&rectClient, brush);		
	}
}

void CMainFrame::AddElement(IElementPtr pElement)
{	
	ATLASSERT(m_pView.get());

	CElement* pViewElement = dynamic_cast<CElement*>(m_pView.get());

	if (pViewElement)
	{
		pViewElement->AddElement(pElement);
	}
}


void CMainFrame::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	IUOLMessengerCommandPtr pCommand = GetCommand(pButton->GetCommandName());

	if (pCommand)
	{
		CUIMApplication::GetApplication()->ExecuteCommand(pCommand);
	}
}

BOOL CMainFrame::OnActivate(BOOL bState)
{
	if (m_pWindowCaption.get())
	{
		m_pWindowCaption->OnActivate(bState);
	}

	m_pCurrentBorder = bState ? m_pActiveBorder.get() : m_pInactiveBorder.get();

	CWindow wndView = m_pView->GetWindow();

	ATLASSERT(wndView.IsWindow());
	if (wndView.IsWindow())
	{
		//wndView.SetFocus();
	}

	InvalidateRect(NULL, FALSE);

	return FALSE;
}

IUOLMessengerCommandPtr CMainFrame::GetCommand(const CString& strCommand)
{
	IUOLMessengerCommandPtr pCommand = m_pView->GetCommand(strCommand);

	if (!pCommand.get())
	{
		pCommand = CMainFrameCommandFactory::GetCommand(strCommand, this);
	}

	return pCommand;
}


CRgnHandle CMainFrame::GetClippingRgn(const CRect& rectWindow) const
{	
	CRgnHandle hClippingRgn;

	if (m_bCustomRgnEnabled)
	{
		int cySystemCaption = ::GetSystemMetrics(SM_CYCAPTION);
		int cxSystemBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
		int cySystemBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);
		
		int l = cxSystemBorder;
		int t = cySystemCaption + cySystemBorder;

		if (m_pMenu.get())
		{
			t += ::GetSystemMetrics(SM_CYMENU);
		}
		

		CRect rectNoFramesWindow = rectWindow;		

		//rectNoFramesWindow += CPoint(1, 1);
		rectNoFramesWindow += CPoint(l, t);	
									   

		CRgn hBigRgn;
		hBigRgn.CreateRoundRectRgn(rectNoFramesWindow.left, 
								rectNoFramesWindow.top, 
								rectNoFramesWindow.left + rectNoFramesWindow.Width() + 1,
								rectNoFramesWindow.top + rectNoFramesWindow.Height() + ROUND_FACTOR, 
								ROUND_FACTOR, ROUND_FACTOR);	

		hClippingRgn.CreateRectRgn(rectNoFramesWindow.left, 
								rectNoFramesWindow.top, 
								rectNoFramesWindow.left + rectNoFramesWindow.Width() + 1, 
								rectNoFramesWindow.top + rectNoFramesWindow.Height());

		hClippingRgn.CombineRgn(hBigRgn, RGN_AND);		
	}
	else
	{	
		hClippingRgn = NULL;
	}

	return (HRGN) hClippingRgn;
}


BOOL CMainFrame::HitTestBorder(const CPoint& ptWindow, int& nTest)
{
	struct CCompare
	{
		BOOL operator()(int x1, int x2, int nEpsilon) const
		{
			return ::abs(x1 - x2) <= nEpsilon;
		}		
	};

	CCompare Compare;

	CRect rect;
	GetClientRect(&rect);

	CSize size(rect.Width(), rect.Height());

	CPoint pt = ptWindow;
	ScreenToClient(&pt);

	const INT EPSILON = 5;	

	if (Compare(pt.x, 0, EPSILON) && Compare(pt.y, 0, EPSILON))
	{
		nTest = HTTOPLEFT;

		return TRUE;
	}

	if (Compare(pt.x, 0, EPSILON) && Compare(pt.y, size.cy, EPSILON))
	{
		nTest = HTBOTTOMLEFT;

		return TRUE;
	}

	if (Compare(pt.x, size.cx, EPSILON) && Compare(pt.y, size.cy, EPSILON))
	{
		nTest = HTBOTTOMRIGHT;

		return TRUE;
	}

	if (Compare(pt.x, size.cx, EPSILON) && Compare(pt.y, 0, EPSILON))
	{
		nTest = HTTOPRIGHT;

		return TRUE;
	}

	if (Compare(pt.y, 0, EPSILON))
	{
		nTest = HTTOP;

		return TRUE;
	}

	if (Compare(pt.x, 0, EPSILON))
	{
		nTest = HTLEFT;

		return TRUE;
	}	

	if (Compare(pt.y, size.cy, EPSILON))
	{
		nTest = HTBOTTOM;

		return TRUE;
	}

	if (Compare(pt.x, size.cx, EPSILON))
	{
		nTest = HTRIGHT;

		return TRUE;
	}

	return FALSE;
}

int CMainFrame::GetRelativeWindowYPos() const
{
	CRect rectWindow;
	GetWindowRect(&rectWindow);

	CRect rectCaption(0, 0, 0, 0);		
	CSize sizeBorder(0);	

	if (m_pWindowCaption.get())	
	{			
		m_pWindowCaption->GetWindow().GetWindowRect(&rectCaption);		
	}

	return 	rectWindow.top - rectCaption.top;
}


IUOLMessengerCommandPtr CMainFrameCommandFactory::GetCommand(const CString& strCommand, CWindow* pWindow)
{
	ATLASSERT(pWindow);

	if (strCommand.CompareNoCase("MinimizeWindow") == 0)
	{
		return new CMinimizeWindowCommand(*pWindow);
	}
	else if (strCommand.CompareNoCase("MaximizeWindow") == 0)
	{
		return new CMaximizeWindowCommand(*pWindow);
	}
	else if (strCommand.CompareNoCase("CloseWindow") == 0)
	{
		return new CCloseWindowCommand(*pWindow);
	}
	else if (strCommand.CompareNoCase("HideWindow") == 0)
	{
		return new CHideWindowCommand(*pWindow);
	}
	else if (strCommand.CompareNoCase("ShowWindow") == 0)
	{
		return new CShowWindowCommand(*pWindow);
	}

	return CUIMApplication::GetApplication()->GetCommandFactory()->GetCommand(strCommand);
}

