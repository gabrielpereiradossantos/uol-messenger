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
#include ".\transparentstatic.h"
#include "../Utility.h"

CTransparentStatic::CTransparentStatic(void) : 
	m_clr()
{
}

CTransparentStatic::~CTransparentStatic(void)
{
}

COLORREF CTransparentStatic::SetColor(COLORREF clr)
{
	COLORREF _clr = m_clr ;

	m_clr = clr;

	return _clr;
}


void CTransparentStatic::OnPaint(HDC hDC)
{
	CPaintDC dc(m_hWnd);

	// Where to draw text
	CRect client_rect;
	GetClientRect(client_rect);

	// Get the caption
	CString szText;
	GetWindowText(szText);

	CFontHandle font = GetFont();

	// Map "Static Styles" to "Text Styles"
#define MAP_STYLE(src, dest) if(dwStyle & (src)) dwText |= (dest)
#define NMAP_STYLE(src, dest) if(!(dwStyle & (src))) dwText |= (dest)

	DWORD dwStyle = GetStyle(), dwText = 0;

	MAP_STYLE(	SS_RIGHT,			DT_RIGHT					);
	MAP_STYLE(	SS_CENTER,			DT_CENTER					);
	MAP_STYLE(	SS_CENTERIMAGE,		DT_VCENTER | DT_SINGLELINE	);
	MAP_STYLE(	SS_NOPREFIX,		DT_NOPREFIX					);
	MAP_STYLE(	SS_WORDELLIPSIS,	DT_WORD_ELLIPSIS			);
	MAP_STYLE(	SS_ENDELLIPSIS,		DT_END_ELLIPSIS				);
	MAP_STYLE(	SS_PATHELLIPSIS,	DT_PATH_ELLIPSIS			);

	NMAP_STYLE(	SS_LEFTNOWORDWRAP |
				SS_CENTERIMAGE |
				SS_WORDELLIPSIS |
				SS_ENDELLIPSIS |
				SS_PATHELLIPSIS,	DT_WORDBREAK				);

	// Set transparent background
	int nOldMode = dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_clr);

	CFont oldFont = dc.SelectFont(font);		

	// Draw the text
	dc.DrawText(szText, szText.GetLength(), client_rect, dwText);		

	dc.SelectFont(oldFont);
	dc.SetBkMode(OPAQUE);
}


LRESULT CTransparentStatic::OnEraseBackground(HDC hDC)
{
	return 0L;
}














CTransparentStatic2::CTransparentStatic2() :
	m_clr(0)
{
}


CTransparentStatic2::~CTransparentStatic2()
{
}


COLORREF CTransparentStatic2::SetColor(COLORREF clr)
{
	COLORREF _clr = m_clr;
	m_clr = clr;
	return _clr;
}


void CTransparentStatic2::DoPaint(CDCHandle& dc)
{
	// Where to draw text
	CRect client_rect;
	GetClientRect(&client_rect);

	// Get the caption
	CString szText;
	GetWindowText(szText);

	CFontHandle font = GetFont();

	// Map "Static Styles" to "Text Styles"
#define MAP_STYLE(src, dest) if(dwStyle & (src)) dwText |= (dest)
#define NMAP_STYLE(src, dest) if(!(dwStyle & (src))) dwText |= (dest)

	DWORD dwStyle = GetStyle(), dwText = 0;

	MAP_STYLE(	SS_RIGHT,			DT_RIGHT					);
	MAP_STYLE(	SS_CENTER,			DT_CENTER					);
	MAP_STYLE(	SS_CENTERIMAGE,		DT_VCENTER | DT_SINGLELINE	);
	MAP_STYLE(	SS_NOPREFIX,		DT_NOPREFIX					);
	MAP_STYLE(	SS_WORDELLIPSIS,	DT_WORD_ELLIPSIS			);
	MAP_STYLE(	SS_ENDELLIPSIS,		DT_END_ELLIPSIS				);
	MAP_STYLE(	SS_PATHELLIPSIS,	DT_PATH_ELLIPSIS			);

	NMAP_STYLE(	SS_LEFTNOWORDWRAP |
				SS_CENTERIMAGE |
				SS_WORDELLIPSIS |
				SS_ENDELLIPSIS |
				SS_PATHELLIPSIS,	DT_WORDBREAK				);

	// Set transparent background
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_clr);

	CFont oldFont = dc.SelectFont(font);		

	// Draw the text
	dc.DrawText(szText, szText.GetLength(), client_rect, dwText);		

	dc.SelectFont(oldFont);
	dc.SetBkMode(OPAQUE);
}