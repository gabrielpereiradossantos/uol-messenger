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

using namespace WTL;


#include "ShowTooltipSubclass.h"

typedef  CWinTraits <WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> CTooltipLabelTraits;

class CUOLFoneTooltipLabel : 
		public CWindowImpl<CUOLFoneTooltipLabel, CButton, CTooltipLabelTraits>, 
		public CShowTooltipSubclass<CUOLFoneTooltipLabel>
{
public:
	
	DECLARE_WND_SUPERCLASS( _T("CUOLFoneTooltipLabel"), _T("Button") )
	
	CUOLFoneTooltipLabel(void);
	virtual ~CUOLFoneTooltipLabel(void);
	
	static LPCTSTR GetWndClassName();
	void SetTextColor(COLORREF textColor);
	void SetBkGrndColor(COLORREF bkColor);
	
	int  GetWidth();
	int  GetHeight();
	
	void SetBitmap(int bitmapResourceID);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneTooltipLabel)
		CHAIN_MSG_MAP(CShowTooltipSubclass<CUOLFoneTooltipLabel>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBackground)
	END_MSG_MAP()
	
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBackground(HDC hDc);
	
	void CalculateComponentSize();
	
	void DrawText(HDC hdc, RECT& rect, CString strText, DWORD textStyle);
	void DrawBitmap(HDC hdc, RECT& rect);
	
private:
	
	COLORREF		m_rgbText;
	COLORREF		m_rgbBkGrnd;
	WTL::CBrush		m_pBrush;
	HBRUSH			m_bkGrndBrushHandle;
	
	int				m_height;
	int				m_width;
	
	WTL::CBitmap	m_buttonBitmap;
};

