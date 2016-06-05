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
 
#include "stdafx.h"
#include "MainDialog.h"
#include "../controller/UIMApplication.h"
#include "WindowCaptionBuilder.h"

#include <commands/MinimizeWindowCommand.h>
#include <commands/MaximizeWindowCommand.h>
#include <commands/CloseWindowCommand.h>
#include <commands/HideWindowCommand.h>
#include <commands/ShowWindowCommand.h>

static const int ROUND_FACTOR = 15;

CMainDialog::CMainDialog(DWORD dwIDD, const CString& strTitle) : 
	IDD(dwIDD), 
    m_strTitle(strTitle), 
    m_pCurrentBorder(NULL),
    m_pCurrentBackground(NULL),
	m_bResizable(TRUE),
    m_bCustomRgnEnabled(TRUE),
    m_nOpacity(100)
{
}

CMainDialog::~CMainDialog()
{
}


void CMainDialog::SetTitle(const CString& strTitle)
{
	m_strTitle = strTitle;

	if (IsWindow() && m_pWindowCaption)
	{
		SetWindowText(m_strTitle);
        m_pWindowCaption->SetText(m_strTitle);
	}
}


CString CMainDialog::GetTitle() const
{
	return m_strTitle;
}


void CMainDialog::SetIcon(IUOLMessengerImagePtr pIcon)
{
	m_pIcon = pIcon;
}


IUOLMessengerImagePtr CMainDialog::GetIcon() const
{
	return m_pIcon;
}


LRESULT CMainDialog::OnInitDialog(HWND, LPARAM)
{
	SetWindowText(GetTitle());

    if (m_pWindowCaption.get())
	{
		m_pWindowCaption->Create(m_hWnd);
		m_pWindowCaption->SetRoundCaptionFactor(ROUND_FACTOR);
		m_pWindowCaption->SetWindowWithRegion(this);
        m_pWindowCaption->AddButtonsEventListener(this);
		m_pWindowCaption->SetNotifyingWindow(m_hWnd);
		m_pWindowCaption->SetSendNewMessage(TRUE);
	}

    EnableRgn(TRUE);

    CenterWindow(::GetDesktopWindow());

	return FALSE;
}

LRESULT CMainDialog::OnEraseBackground(HDC hDc)
{
	return 0L;
}

void CMainDialog::OnPaint(HDC hDc)
{
	CRect rect;
	GetClientRect(&rect);
	CPaintDC dc(m_hWnd);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);

	if (m_nOpacity == 100)
	{
		CMemDC memDC2(dc, &rect);
		CDCHandle dcHandle(memDC2);

		dc.SaveDC();
		dc.SelectClipRgn(rgn);	

		DrawBorder(dcHandle);
        DrawBackground(dcHandle, m_rectView);	
		DoPaint(dcHandle);

		memDC2.Update();

		dc.RestoreDC(-1);
	}
	else
	{
		CMemDC memDC(dc, &rect, m_nOpacity);
		CMemDC memDC2(memDC, &rect);

		CDCHandle dcHandle(memDC2);

		dc.SaveDC();
		dc.SelectClipRgn(rgn);	

		DrawBorder(dcHandle);
        DrawBackground(dcHandle, m_rectView);	

		memDC2.Update();

		DoPaint(dcHandle);

		dc.RestoreDC(-1);
	}	
}

void CMainDialog::OnClose()
{
	//DestroyWindow();
	EndDialog(IDCANCEL);
}

void CMainDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

    CBackgroundElementPtr pInactiveBackground;
	CBackgroundElementPtr pActiveBackground;

	try
	{
		CString strBackground;
		GetParameterValue(strBackground, pMapParams, "background");

		pActiveBackground = ap_dynamic_cast<CBackgroundElementPtr>(pElementTable->GetElement(strBackground));
		ATLASSERT(pActiveBackground);		

		try
		{
			CString strBackground;
			GetParameterValue(strBackground, pMapParams, "inactive_background");

			pInactiveBackground= ap_dynamic_cast<CBackgroundElementPtr>(pElementTable->GetElement(strBackground));
			ATLASSERT(pInactiveBackground);
		}
		catch(const CSkinException&)
		{
			SetBackground(pActiveBackground, pActiveBackground);
		}

		SetBackground(pActiveBackground, pInactiveBackground);
	}
	catch(const CSkinException&)
	{
	}


	try
	{	
		GetParameterValue(m_nOpacity, pMapParams, "opacity");	

		// Valid range: 0 - 100
		// 0	- transparent
		// 100  - opaque
		m_nOpacity = max(0, min(100, m_nOpacity)); 
	}
	catch(const CSkinException&)
	{
	}

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

void CMainDialog::DrawBackground(CDCHandle& dc, const CRect& rect)
{
	if (m_pCurrentBackground)
	{	
		m_pCurrentBackground->Draw(dc, rect);
	}
}

void CMainDialog::DrawBorder(CDCHandle& dc)
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

void CMainDialog::SetBackground(CBackgroundElementPtr pActiveBackground, CBackgroundElementPtr pInactiveBackground)
{	
	m_pInactiveBackground = pInactiveBackground;
	m_pActiveBackground = pActiveBackground;	

	m_pCurrentBackground = m_pActiveBackground.get();	

	if (IsWindow())
	{
		InvalidateRect(NULL, TRUE);
		UpdateWindow();
		UpdateLayout();
	}
}

void CMainDialog::UpdateLayout()
{
	CRect rect;
	GetClientRect(&rect);

	// Calculate client rect without menu bar and status bar
	int x = rect.left;
	int y = rect.top;
	int cx = rect.Width();	
	int cy = rect.Height();	

	CSize sizeBorder = CSize(0, 0);
	CSize sizeWindowCaption = CSize(0, 0);	
	
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

    CRect rectView(CPoint(x, y), CSize(cx, cy));
    m_rectView = rectView;    

    SetWindowRgn(GetClippingRgn(rect), TRUE);	
}

void CMainDialog::GetMinSize(CSize& size)
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

	size.cx = cx;
	size.cy = cy;
}

CRgnHandle CMainDialog::GetClippingRgn(const CRect& rectWindow) const
{	
	CRgnHandle hClippingRgn;

	if (m_bCustomRgnEnabled)
	{
		int cySystemCaption = ::GetSystemMetrics(SM_CYCAPTION);
		int cxSystemBorder;
		int cySystemBorder;

        if (m_bResizable)
	    {
		    cxSystemBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);
		    cySystemBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
	    }
	    else
	    {
		    cxSystemBorder = ::GetSystemMetrics(SM_CYFIXEDFRAME);
		    cySystemBorder = ::GetSystemMetrics(SM_CXFIXEDFRAME);
	    }
		
		int l = cxSystemBorder;
		int t = cySystemCaption + cySystemBorder;

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

BOOL CMainDialog::IsRgnEnabled() const
{
	return m_bCustomRgnEnabled;
}

BOOL CMainDialog::EnableRgn(BOOL bEnable)
{
	BOOL bRet = m_bCustomRgnEnabled;

	m_bCustomRgnEnabled = bEnable;	

	UpdateLayout();
		
	return bRet;
}

BOOL CMainDialog::OnActivate(UINT nActivate, BOOL, HWND)
{
	BOOL bState = (nActivate == WA_ACTIVE) | (nActivate == WA_CLICKACTIVE);
    
    if (m_pWindowCaption.get())
	{
		m_pWindowCaption->OnActivate(bState);
	}

	m_pCurrentBorder = bState ? m_pActiveBorder.get() : m_pInactiveBorder.get();

	InvalidateRect(NULL, FALSE);
	UpdateLayout();

	return FALSE;
}

void CMainDialog::DoPaint(CDCHandle& dc)
{
}

LRESULT CMainDialog::OnNcHitTest(const CPoint& pt)
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

	SetMsgHandled(FALSE);

	return nTest;
}

BOOL CMainDialog::HitTestBorder(const CPoint& ptWindow, int& nTest)
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

void CMainDialog::OnWindowPosChanging(LPWINDOWPOS lpWindowPos)
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
		int cxSystemBorder;
		int cySystemBorder;

        if (m_bResizable)
	    {
		    cxSystemBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);
		    cySystemBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
	    }
	    else
	    {
		    cxSystemBorder = ::GetSystemMetrics(SM_CYFIXEDFRAME);
		    cySystemBorder = ::GetSystemMetrics(SM_CXFIXEDFRAME);
	    }

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

void CMainDialog::OnMove(const CPoint& pt)
{
	SetMsgHandled(FALSE);
}

void CMainDialog::OnDestroy()
{
	if (m_pWindowCaption.get() && m_pWindowCaption->IsWindow())
	{
		m_pWindowCaption->DestroyWindow();
	}

    SetMsgHandled(FALSE);
}

void CMainDialog::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	ProcessCommand(pButton->GetCommandName());
}

void CMainDialog::ProcessCommand(const CString& strCommand)
{
    if (strCommand.CompareNoCase("MinimizeWindow") == 0)
	{
		//Do nothing
	}
	else if (strCommand.CompareNoCase("MaximizeWindow") == 0)
	{
		//Do nothing
	}
	else if (strCommand.CompareNoCase("CloseWindow") == 0)
	{
        EndDialog(IDCANCEL);
	}
	else if (strCommand.CompareNoCase("HideWindow") == 0)
	{
		//Do nothing
	}
	else if (strCommand.CompareNoCase("ShowWindow") == 0)
	{
		//Do nothing
	}
}