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
#include "NotifyPopUpCaption.h"

#include "../skin/SolidBackgroundElement.h"
#include "../../resource.h"


CNotifyPopUpCaption::CNotifyPopUpCaption() :
m_nHeight(-1),
m_bShowOptionLink(TRUE)
{
}


CNotifyPopUpCaption::~CNotifyPopUpCaption()
{
	OnDestroy();
}


void CNotifyPopUpCaption::SetTitle(const CString& strTitle)
{
	m_strTitle = strTitle;
}


CString CNotifyPopUpCaption::GetTitle() const
{
	return m_strTitle;
}


void CNotifyPopUpCaption::ShowOptionLink(BOOL bShow)
{
	m_bShowOptionLink = bShow;
}


int CNotifyPopUpCaption::GetHeight() const
{
	return m_nHeight;
}


CString CNotifyPopUpCaption::GetName() const
{
	return "__NotifyPopUpCaptionInstance";
}


void CNotifyPopUpCaption::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	if (!m_pActiveBackground.get())
	{
		m_pActiveBackground = new CSolidBackgroundElement(RGB(0, 255, 255));
		m_pCurrentBackground = m_pActiveBackground.get();
	}

	GetParameterValue(m_nHeight, pMapParams, "height");

	try
	{
		CString strFontName;
		GetParameterValue(strFontName, pMapParams, "titlefont");		

		m_pFontTitle = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontName));
		ATLASSERT(m_pFontTitle.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "titlefont");
		// use default system font
		m_pFontTitle = new CFontElement();
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "titlecolor");

		m_clrTitle.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "titlecolor");
		m_clrTitle.SetColor(RGB(0, 0, 0));
	}

	try
	{
		CString strFontName;
		GetParameterValue(strFontName, pMapParams, "linkfont");		

		m_pFontLink = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontName));
		ATLASSERT(m_pFontLink.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "linkfont");
		// use default system font
		m_pFontLink = new CFontElement();
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "linkcolor");

		m_clrLink.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "linkcolor");
		m_clrLink.SetColor(RGB(0, 0, 0));
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "visitedlinkcolor");

		m_clrVisitedLink.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "visitedlinkcolor");
		m_clrVisitedLink.SetColor(RGB(128, 0, 128));
	}
}


void CNotifyPopUpCaption::DoPaint(CDCHandle dc)
{
	dc.SetBkMode(TRANSPARENT);

	CRect rectClient;
	GetClientRect(&rectClient);

	m_pActiveBackground->Draw(dc, rectClient);
}


void CNotifyPopUpCaption::GetMinSize(CSize& size)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	size = CSize(rectClient.Width(), m_nHeight);
}


LRESULT CNotifyPopUpCaption::OnCreate(LPCREATESTRUCT lpcs)
{
	m_brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	LOGFONT* pLogFont;
	pLogFont = m_pFontTitle->GetFont().get();
	m_fontTitle.CreateFontIndirect(pLogFont);

	pLogFont = m_pFontLink->GetFont().get();
	m_fontLink.CreateFontIndirect(pLogFont);

	CRect rectWnd;
	GetClientRect(&rectWnd);

	CRect rect;
	CString strTitle;

	//FECHAR
	rect.right = rectWnd.right - 4;
	rect.left = rect.right - 40;
	rect.top = 1;
	rect.bottom = min(20, rectWnd.Height());

	CNotifyPopUpStaticText staticClose;
	staticClose.Create(m_hWnd, rect, NULL, WS_VISIBLE | WS_CHILD);
	strTitle.LoadString(IDS_NOTIFY_POPUP_CLOSE);
	staticClose.SetWindowText((LPCTSTR) strTitle);	
	m_linkClose.SetLinkFont(m_fontLink);
	m_linkClose.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
	m_linkClose.SubclassWindow(staticClose);
	m_linkClose.SetDlgCtrlID(1);
	m_linkClose.m_clrLink = m_clrLink;
	m_linkClose.m_clrVisited = m_clrVisitedLink;

	staticClose.Detach();

	//OPÇÕES
	if (m_bShowOptionLink)
	{
		rect.top = 1;
		rect.bottom = min(20, rectWnd.Height());
		rect.right = rect.left - 2;
		rect.left = rect.right - 40;

		CNotifyPopUpStaticText staticOption;
		staticOption.Create(m_hWnd, rect, NULL, WS_VISIBLE | WS_CHILD);
		strTitle.LoadString(IDS_NOTIFY_POPUP_OPTIONS);
		staticOption.SetWindowText((LPCTSTR) strTitle);
		
		m_linkOption.SetLinkFont(m_fontLink);
		m_linkOption.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
		m_linkOption.SubclassWindow(staticOption);
		m_linkOption.SetDlgCtrlID(2);
		m_linkOption.m_clrLink = m_clrLink;
		m_linkOption.m_clrVisited = m_clrVisitedLink;
		
		staticOption.Detach();
	}

	//TITULO
	rect.top = 4;
	rect.bottom = min(20, rectWnd.Height());
	rect.right = rect.left - 2;
	rect.left = 4;

	m_staticTitle.Create(m_hWnd, rect, NULL, WS_VISIBLE | WS_CHILD);

	strTitle = m_strTitle;
	m_staticTitle.FitString(strTitle, m_fontTitle);
	m_staticTitle.SetWindowText(strTitle);

	return 0L;
}


void CNotifyPopUpCaption::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	if (m_linkOption.IsWindow())
	{
		m_linkOption.DestroyWindow();
	}

	if (m_linkClose.IsWindow())
	{
		m_linkClose.DestroyWindow();
	}

	if (m_staticTitle.IsWindow())
	{
		m_staticTitle.DestroyWindow();
	}
	
	m_linkOption.m_hWnd = NULL;
	m_linkClose.m_hWnd = NULL;
	m_staticTitle.m_hWnd = NULL;
}


HBRUSH CNotifyPopUpCaption::OnCtlColor(HDC hdc, HWND hWnd)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	::SetBkMode(hdc, TRANSPARENT );
	::SelectObject(hdc, (HFONT) m_fontTitle);
	::SetTextColor(hdc, m_clrTitle);

	SetMsgHandled(TRUE);

	return m_brush;
}


LRESULT CNotifyPopUpCaption::OnMouseDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SendMessage(GetParent(), uMsg, wParam, lParam);
	bHandled = FALSE;
	return 0;
}


void CNotifyPopUpCaption::OnCommand(UINT code, int id, HWND hWnd)
{
	UINT uNotifyMessage;

	if (hWnd == m_linkClose)
	{
		uNotifyMessage = WM_USER_NOTIFYPOPUPCLOSEBUTTONDOWN;
	}
	else
	{
		//  (hWnd == m_linkOptions)
		uNotifyMessage = WM_USER_NOTIFYPOPUPOPTIONSBUTTONDOWN;
	}

	::SendMessage(GetParent(), uNotifyMessage, (WPARAM) m_hWnd, 0);
}