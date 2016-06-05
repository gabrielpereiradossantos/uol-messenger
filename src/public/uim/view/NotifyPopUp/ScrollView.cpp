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
#include "ScrollView.h"

#include "NotifyPopUpHyperLink.h"
#include "../skin/SolidBackgroundElement.h"

#include "../image/image.h"


CScrollView::CScrollView() : 
	m_clrLink(RGB(0, 0, 0)),
	m_clrVisitedLink(RGB(128, 0, 128))
{
}


CScrollView::~CScrollView()
{
	CNotifyPopUpStaticTextPtr pStat;
	CNotifyPopUpHyperLinkPtr pLink;

	//ATLTRACE("[CScrollView::~CScrollView] Inicio\n");

	for(size_t x=0; x<m_statics.GetCount(); x++)
	{
		pStat = m_statics[x];
		if(pStat->IsWindow())
		{
			pStat->DestroyWindow();
		}
		pStat->m_hWnd = NULL;
	}

	for(size_t x=0; x<m_links.GetCount(); x++)
	{
		pLink = m_links[x];
		if(pLink->IsWindow())
		{
			pLink->DestroyWindow();
		}
		pLink->m_hWnd = NULL;
	}

	//ATLTRACE("[CScrollView::~CScrollView] Fim\n");
}



/*
 * AddLink
 *
 *	Adiciona um link à janela
 *
 *  sTitle		título
 *	sUrl		url	
 *	
 * 
 *  
 *  
 *
 */
void CScrollView::AddLink(const CString& strTitle, const CString& strUrl)
{
	CNotifyPopUpStaticTextPtr pStat;
	CNotifyPopUpHyperLinkPtr pLink;

	pStat = new CNotifyPopUpStaticText();
	pLink = new CNotifyPopUpHyperLink();
	pLink->SetHyperLink((LPCTSTR) strUrl);

	m_statics.Add(pStat);
	m_links.Add(pLink);
	m_titles.Add(strTitle);
}


void CScrollView::SetMessageIcon(IUOLMessengerImagePtr pMessageIcon)
{
	m_pMessageIcon = pMessageIcon;
}


void CScrollView::SetWindowIcon(IUOLMessengerImagePtr pWindowIcon)
{
	m_pWindowIcon = pWindowIcon;
}


void CScrollView::SetMessageIconDrawStrategy(IUOLMessengerImageDrawStrategyPtr pMessageIconDrawStrategy)
{
	m_pMessageIconDrawStrategy = pMessageIconDrawStrategy;
}


void CScrollView::SetWindowIconDrawStrategy(IUOLMessengerImageDrawStrategyPtr pWindowIconDrawStrategy)
{
	m_pWindowIconDrawStrategy = pWindowIconDrawStrategy;
}


/*
 * OnCreate
 *
 *	Trata o evento de criação da janela
 *
 *  
 *	
 *	
 *  
 *  Cria a janelas para cada link, configura a área de rolagem
 *  
 *
 */
LRESULT CScrollView::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	RECT rect;
	GetClientRect(&rect);

	CNotifyPopUpStaticTextPtr pStat;
	CNotifyPopUpHyperLinkPtr pLink;
	CString sTitle;
	CString sNewTitle;

	// Create font
	if (m_pFontLink != NULL)
	{
		LOGFONT* pLogFont = m_pFontLink->GetFont().get();
		m_staticFont.CreateFontIndirect(pLogFont);
	}
	else
	{
		m_staticFont.CreateFont(14, 0, 1,
			1, FW_NORMAL, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			"Arial");
	}

	// Create links...
	if (m_statics.GetCount() > 0)
	{
		rect.left = 5;
		if (m_pBorderInternal.get())
		{
			CSize sizeBorder;
			m_pBorderInternal->GetSize(sizeBorder);

			rect.left += sizeBorder.cx;
			rect.top += sizeBorder.cy;
		}

		if (m_pMessageIcon.get())
		{
			rect.left += (rect.bottom - rect.top);
		}

		rect.bottom = 0;
		rect.right = rect.right - 35;
		for(size_t x=0; x<m_statics.GetCount(); x++)
		{
			pStat = m_statics[x];
			pLink = m_links[x];
			sTitle= m_titles[x];

			rect.top = rect.bottom+2;
			rect.bottom = rect.top+13;
			pStat->Create(m_hWnd, rect, NULL, WS_VISIBLE | WS_CHILD);

			pStat->FitString(sTitle, m_staticFont);

			pStat->SetWindowText(sTitle);

			pLink->SetLinkFont(m_staticFont);
			pLink->SubclassWindow(pStat->m_hWnd);
			pLink->m_clrLink = m_clrLink;
			pLink->m_clrVisited = m_clrVisitedLink;
		}

		SetScrollSize(rect.right-20, rect.bottom);
		SetScrollLine(20, 23);
		SetScrollPage(40, 46);
	}


	rect.left = rect.right-20;
	rect.right = rect.left+15;

	return 1;
}

void CScrollView::DoPaint(CDCHandle dcHandle)
{
	dcHandle.SetBkMode(TRANSPARENT);

	CRect rectClient;
	GetClientRect(&rectClient);

	if (m_pBorderInternal.get())
	{
		CSize sizeBorder;
		m_pBorderInternal->GetSize(sizeBorder);

		CBrush brush;
		brush.CreateSolidBrush(m_pBorderInternal->GetCOLORREF());

		dcHandle.FillRect(&rectClient, brush);

		rectClient.left += sizeBorder.cx;
		rectClient.right -= sizeBorder.cx;
		rectClient.top += sizeBorder.cy;
		rectClient.bottom -= sizeBorder.cy;
	}

	m_pActiveBackground->Draw(dcHandle, rectClient);

	if (m_pWindowIcon)
	{
		int nRectHeight = rectClient.Height();

		int nIconWidth = m_pWindowIcon->GetWidth();
		int nIconHeight = m_pWindowIcon->GetHeight();

		IUOLMessengerImageDrawStrategyPtr pStrategy;
		
		if (m_pWindowIconDrawStrategy)
		{
			pStrategy = m_pWindowIconDrawStrategy;
		}
		else 
		{
			pStrategy = new UOLMessengerImage::CDrawStretchTransparentStrategy();

			if (nIconHeight > nRectHeight)
			{
				nIconWidth = (int) floor(nIconWidth * (nRectHeight / (double) nIconHeight));
				nIconHeight = nRectHeight;
			}
		}

		m_pWindowIcon->Draw(dcHandle, 
					rectClient.right - nIconWidth, 
					rectClient.bottom - nIconHeight, 
					nIconWidth, nIconHeight, 
					pStrategy);
	}

	if (m_pMessageIcon)
	{
		int nWidth = rectClient.Height();

		IUOLMessengerImageDrawStrategyPtr pStrategy;
		
		if (m_pMessageIconDrawStrategy)
		{
			pStrategy = m_pMessageIconDrawStrategy;
		}
		else
		{
			pStrategy = new UOLMessengerImage::CDrawStretchOpaqueStrategy();
		}

		m_pMessageIcon->Draw(dcHandle, 
					rectClient.left, rectClient.top, 
					nWidth, nWidth, 
					pStrategy);
	}
}

/*
 * OnDestroy
 *
 *	Trata o evento de destruição da janela
 *
 *  
 *	
 *	
 *  
 *  Libra recursos alocados
 *  
 *
 */
void CScrollView::OnDestroy()
{
	CNotifyPopUpStaticTextPtr pStat;
	CNotifyPopUpHyperLinkPtr pLink;


	//ATLTRACE("[CScrollView::OnDestroy] Inicio\n");

	for(size_t x=0; x<m_statics.GetCount(); x++)
	{
		pStat = m_statics[x];
		if(pStat->IsWindow())
		{
			pStat->DestroyWindow();
		//else
			pStat->m_hWnd = NULL;
		}
	}

	for(size_t x=0; x<m_links.GetCount(); x++)
	{
		pLink = m_links[x];
		if(pLink->IsWindow())
		{
			pLink->DestroyWindow();
		//else
			pLink->m_hWnd = NULL;
		}
	}

	//ATLTRACE("[CScrollView::OnDestroy] Fim\n");
}


/*
 * OnCtlColor
 *
 *	Trata o evento de solicitação de cor para controles estáticos
 *
 *  
 *	
 *	Retorna o brush de desenho
 *  
 *  Configura o modo de background como transparente
 *  
 *
 */
HBRUSH CScrollView::OnCtlColor(HDC hdc, HWND /*hWnd*/)
{
	::SetBkMode(hdc, TRANSPARENT );
	::SelectObject(hdc, (HFONT)m_staticFont);

	SetMsgHandled(TRUE);

	return NULL;
}

/*
 * OnMouseDown
 *
 *	Trata o evento de click na janela
 *
 *  
 *	
 *	Apenas encaminha a notificação para a janela pai.
 *  
 *  Retorna 0
 *  
 *
 */
LRESULT CScrollView::OnMouseDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE("[CScrollView::OnMouseDown] Inicio\n");
	::SendMessage(GetParent(), uMsg, wParam, lParam);
	bHandled = FALSE;
	//ATLTRACE("[CScrollView::OnMouseDown] End\n");
	return 0;
}


LRESULT CScrollView::OnHyperlinkButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SendMessage(GetParent(), uMsg, (WPARAM) m_hWnd, lParam);

	return 0;
}


//void CScrollView::DoScroll(int nType, int nScrollCode, int& cxyOffset, int cxySizeAll, int cxySizePage, int cxySizeLine)
//{
//	__super::DoScroll(nType, nScrollCode, cxyOffset, cxySizeAll, cxySizePage, cxySizeLine);
//	InvalidateRect(NULL);
//}


CString CScrollView::GetName() const
{
	return _T("__ScrollViewInstance");
}


void CScrollView::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	if (!m_pActiveBackground.get())
	{
		m_pActiveBackground = new CSolidBackgroundElement(RGB(255, 255, 0));
		m_pCurrentBackground = m_pActiveBackground.get();
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

	try
	{
		CString strBorder;
		GetParameterValue(strBorder, pMapParams, "internalborder");
		
		m_pBorderInternal = ap_dynamic_cast<CBorderElementPtr>(pElementTable->GetElement(strBorder));
		ATLASSERT(m_pBorderInternal.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "internalborder");
	}
}