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

class CNotifyPopUpStaticText : public CStatic
{
public:
	void FitString(CString& sText, CFont& staFont)
	{
		CString sTempText;
		CString sTempText2;
		CRect rcBounds;
		CRect rect;

		CDC dc = GetDC();
		::SelectObject(dc, (HFONT)staFont);

		GetClientRect(rcBounds);
		GetClientRect(rect);
		sTempText = sText;
		sTempText2 = sText;
		dc.DrawText(sText, sText.GetLength(), rcBounds, DT_CALCRECT | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE);
		if((rcBounds.right-rcBounds.left) > (rect.right-rect.left))
		{

			while((rcBounds.right-rcBounds.left) > (rect.right-rect.left))
			{
				sTempText = sTempText.Left(sTempText.GetLength()-1);
				sTempText2 = sTempText;
				sTempText2 += _T("...");
				dc.DrawText(sTempText2, sTempText2.GetLength(), rcBounds, DT_CALCRECT | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE);
			}
		}
		sText = sTempText2;
	}
};
MAKEAUTOPTR(CNotifyPopUpStaticText);

class CNotifyPopUpHyperLink : public CHyperLinkImpl<CNotifyPopUpHyperLink>
{
public:

	CNotifyPopUpHyperLink() : CHyperLinkImpl<CNotifyPopUpHyperLink>(HLINK_COMMANDBUTTON)
	{
	};
	~CNotifyPopUpHyperLink(){};

	DECLARE_WND_CLASS(_T("NotifyPopUpHyperLink"))

	enum
	{
		WM_USER_HYPERLINKLBUTTONDOWN = WM_USER + 0x101,
		WM_USER_HYPERLINKRBUTTONDOWN
	};


// Message map and handlers
	BEGIN_MSG_MAP(CNotifyPopUpHyperLink)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		CHAIN_MSG_MAP(CHyperLinkImpl<CNotifyPopUpHyperLink>)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLTRACE("[CNotifyPopUpHyperLink::OnLButtonDown] Inicio\n");
		::SendMessage(GetParent(), WM_USER_HYPERLINKLBUTTONDOWN, (WPARAM) m_hWnd, lParam);
		bHandled = FALSE;
		ATLTRACE("[CNotifyPopUpHyperLink::OnLButtonDown] Fim\n");
		return 0;
	}

	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLTRACE("[CNotifyPopUpHyperLink::OnRButtonDown] Inicio\n");
		::SendMessage(GetParent(), WM_USER_HYPERLINKRBUTTONDOWN, (WPARAM) m_hWnd, lParam);
		bHandled = FALSE;
		ATLTRACE("[CNotifyPopUpHyperLink::OnLButtonDown] Fim\n");
		return 0;
	}

	// Override DoPaint because WTL::CHyperLinkImpl call DrawText without DT_NOPREFIX,
	// resulting in incorrect drawing of text containing "&"
	void DoPaint(CDCHandle dc)
	{
		if(IsUsingTags())
		{
			// find tags and label parts
			LPTSTR lpstrLeft = NULL;
			int cchLeft = 0;
			LPTSTR lpstrLink = NULL;
			int cchLink = 0;
			LPTSTR lpstrRight = NULL;
			int cchRight = 0;

			CalcLabelParts(lpstrLeft, cchLeft, lpstrLink, cchLink, lpstrRight, cchRight);

			// get label part rects
			RECT rcClient = { 0 };
			GetClientRect(&rcClient);

			dc.SetBkMode(TRANSPARENT);
			HFONT hFontOld = dc.SelectFont(m_hFontNormal);

			if(lpstrLeft != NULL)
				dc.DrawText(lpstrLeft, cchLeft, &rcClient, DT_LEFT | DT_WORDBREAK | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);

			COLORREF clrOld = dc.SetTextColor(IsWindowEnabled() ? (m_bVisited ? m_clrVisited : m_clrLink) : (::GetSysColor(COLOR_GRAYTEXT)));
			if(m_hFont != NULL && (!IsUnderlineHover() || (IsUnderlineHover() && m_bHover)))
				dc.SelectFont(m_hFont);
			else
				dc.SelectFont(m_hFontNormal);

			dc.DrawText(lpstrLink, cchLink, &m_rcLink, DT_LEFT | DT_WORDBREAK | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);

			dc.SetTextColor(clrOld);
			dc.SelectFont(m_hFontNormal);
			if(lpstrRight != NULL)
			{
				RECT rcRight = { m_rcLink.right, m_rcLink.top, rcClient.right, rcClient.bottom };
				dc.DrawText(lpstrRight, cchRight, &rcRight, DT_LEFT | DT_WORDBREAK | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);
			}

			if(GetFocus() == m_hWnd)
				dc.DrawFocusRect(&m_rcLink);

			dc.SelectFont(hFontOld);
		}
		else
		{
			dc.SetBkMode(TRANSPARENT);
			COLORREF clrOld = dc.SetTextColor(IsWindowEnabled() ? (m_bVisited ? m_clrVisited : m_clrLink) : (::GetSysColor(COLOR_GRAYTEXT)));

			HFONT hFontOld = NULL;
			if(m_hFont != NULL && (!IsUnderlineHover() || (IsUnderlineHover() && m_bHover)))
				hFontOld = dc.SelectFont(m_hFont);
			else
				hFontOld = dc.SelectFont(m_hFontNormal);

			LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;

			DWORD dwStyle = GetStyle();
			int nDrawStyle = DT_LEFT;
			if (dwStyle & SS_CENTER)
				nDrawStyle = DT_CENTER;
			else if (dwStyle & SS_RIGHT)
				nDrawStyle = DT_RIGHT;

			dc.DrawText(lpstrText, -1, &m_rcLink, nDrawStyle | DT_WORDBREAK | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);

			if(GetFocus() == m_hWnd)
				dc.DrawFocusRect(&m_rcLink);

			dc.SetTextColor(clrOld);
			dc.SelectFont(hFontOld);
		}
	}
};
MAKEAUTOPTR(CNotifyPopUpHyperLink);