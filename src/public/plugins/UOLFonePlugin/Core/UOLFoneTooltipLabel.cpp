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
#include ".\UOLFoneTooltipLabel.h"


#define			DEFAULT_SPACE				5


CUOLFoneTooltipLabel::CUOLFoneTooltipLabel(void) : 
		m_height(0), 
		m_width(0)
{
	m_rgbText = RGB(0, 0, 0);
	m_rgbBkGrnd = GetSysColor(COLOR_WINDOW);
	
	m_bkGrndBrushHandle = m_pBrush.CreateSolidBrush(m_rgbBkGrnd);
}


CUOLFoneTooltipLabel::~CUOLFoneTooltipLabel(void)
{
	if (!m_pBrush.IsNull())
	{
		m_pBrush.DeleteObject();
	}
	
	if (!m_buttonBitmap.IsNull())
	{
		m_buttonBitmap.DeleteObject();
	}
}


LPCTSTR CUOLFoneTooltipLabel::GetWndClassName()
{
	return _T("CUOLFoneTooltipLabel");
}


void CUOLFoneTooltipLabel::SetTextColor(COLORREF textColor)
{
	if (textColor != NULL)
	{
		m_rgbText = textColor;
	}
}


void CUOLFoneTooltipLabel::SetBkGrndColor(COLORREF bkColor)
{
	if (bkColor != NULL)
	{
		m_rgbBkGrnd = bkColor;
		
		m_pBrush.DeleteObject();
		m_bkGrndBrushHandle = m_pBrush.CreateSolidBrush(m_rgbBkGrnd);
	}
}


int CUOLFoneTooltipLabel::GetWidth()
{
	CalculateComponentSize();
	
	return m_width;
}


int CUOLFoneTooltipLabel::GetHeight()
{
	CalculateComponentSize();
	
	return m_height;
}


void CUOLFoneTooltipLabel::SetBitmap(int bitmapResourceID)
{
	if (!m_buttonBitmap.IsNull())
	{
		m_buttonBitmap.DeleteObject();
	}
	
	m_buttonBitmap.LoadBitmap(bitmapResourceID);
	
	Invalidate();
}


LRESULT CUOLFoneTooltipLabel::OnPaint(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC thisDC(m_hWnd); // device context for painting
	
	// Get static's text.
	CString strText = _T("");
	GetWindowText(strText);
	
	if ( (!strText.IsEmpty()) || (!m_buttonBitmap.IsNull()) )
	{
		thisDC.SaveDC();
		
		CRect rect;
		GetClientRect(rect); 
		
		HDC hdcMem = ::CreateCompatibleDC(thisDC.m_hDC);
		HBITMAP hbmMem = ::CreateCompatibleBitmap(thisDC.m_hDC,
				rect.Width(), rect.Height());
	    
		HGDIOBJ hbmOld = ::SelectObject(hdcMem, (HGDIOBJ)hbmMem);
		
		FillRect(hdcMem , &rect, m_bkGrndBrushHandle);
		
		DWORD dwStyle = GetStyle();
		
		if (m_buttonBitmap.IsNull())
		{
			DrawText(hdcMem, rect, strText, dwStyle);
		}
		else
		{
			CSize bmpSize;
			m_buttonBitmap.GetSize(bmpSize);
			
			// Set DrawText format from button style settings.
			if (dwStyle & BS_LEFT)
			{
				CRect textRect(
					rect.left, 
					rect.top, 
					rect.right - (bmpSize.cx + DEFAULT_SPACE), 
					rect.bottom);
				
				DrawText(hdcMem, textRect, strText, dwStyle);
				
				CRect bmpRect(
					rect.right - bmpSize.cx, 
					rect.top, 
					rect.right, 
					rect.bottom);
				
				DrawBitmap(hdcMem, bmpRect);
			}
			else
			{
				// Default option. If it's BS_RIGHT, or none.
				CRect textRect(
					rect.left + bmpSize.cx + DEFAULT_SPACE, 
					rect.top, 
					rect.right, 
					rect.bottom);
				
				DrawText(hdcMem, textRect, strText, dwStyle);
				
				CRect bmpRect(
					rect.left, 
					rect.top, 
					rect.left + bmpSize.cx, 
					rect.bottom);
				
				DrawBitmap(hdcMem, bmpRect);
			}
		}
		
		StretchBlt(thisDC.m_hDC, rect.left, rect.top, rect.Width(), rect.Height(), 
				hdcMem, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
		
		::SelectObject(hdcMem, hbmOld);
		::DeleteObject((HGDIOBJ)hbmMem);
		::DeleteDC(hdcMem);
		
		thisDC.RestoreDC(-1);
	}
	
	return 0L;
}


LRESULT CUOLFoneTooltipLabel::OnEraseBackground(HDC hDc)
{
	return 1L;
}


void CUOLFoneTooltipLabel::CalculateComponentSize()
{
	CString staticText;
	int textLength = GetWindowText(staticText);
	
	m_height = m_width = 0;
	
	if (textLength > 0)
	{
		CPaintDC paintDC(m_hWnd);
		paintDC.SelectFont(GetFont());
		
		CSize textSize;
		::GetTextExtentPoint32(paintDC.m_ps.hdc, staticText, textLength, &textSize);
		
		m_width = textSize.cx;
		m_height = textSize.cy;
	}
	
	if (!m_buttonBitmap.IsNull())
	{
		CSize bmpSize;
		m_buttonBitmap.GetSize(bmpSize);
		
		m_width += bmpSize.cx + DEFAULT_SPACE;
		
		m_height = __max(m_height, bmpSize.cy);
	}
	
	m_width += 2;
}


void CUOLFoneTooltipLabel::DrawText(HDC hdc, RECT& rect, CString strText, DWORD textStyle)
{
	::SetBkMode(hdc, OPAQUE);
	::SetTextColor(hdc, m_rgbText);
	SetBkColor(hdc, m_rgbBkGrnd);
	::SelectObject(hdc, m_pBrush);
	
	::SelectObject(hdc, GetFont());
	
	UINT nFormat = 0;
	
	// Set DrawText format from button style settings.
	if (textStyle & BS_LEFT)
	{
		nFormat |= DT_LEFT;
	}
	else if (textStyle & BS_RIGHT)
	{
		nFormat |= DT_RIGHT;
	}
	else if (textStyle & BS_CENTER)
	{
		nFormat |= DT_CENTER;
	}
	
	if (textStyle & BS_VCENTER) 		// vertical centering ==> single line only
	{
		nFormat |= DT_VCENTER | DT_SINGLELINE;
	}
	else if (textStyle & BS_MULTILINE)
	{
		nFormat |= DT_WORDBREAK;
	}
	
	DWORD lastError;
	if (::DrawText(hdc, strText, strlen(strText), &rect, nFormat) == 0)
	{
		lastError = GetLastError();
		
		ATLTRACE("CUOLFoneTooltipLabel::DrawBitmap() - DrawText() failed with error [%u]\n", lastError);
	}
}


void CUOLFoneTooltipLabel::DrawBitmap(HDC hdc, RECT& rect)
{
	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HGDIOBJ hbmOld = ::SelectObject(hdcMem, m_buttonBitmap);
	
	CSize bmpSize;
	m_buttonBitmap.GetSize(bmpSize);
	
	int cx = rect.right  - rect.left;
	int cy = rect.bottom - rect.top;
	
	DWORD lastError;
	if (::StretchBlt(hdc, rect.left, rect.top, cx, cy, hdcMem, 0, 0, bmpSize.cx, bmpSize.cy, SRCCOPY) == 0)
	{
		lastError = GetLastError();
		
		ATLTRACE("CUOLFoneTooltipLabel::DrawBitmap() - StretchBlt() failed with error [%u]\n", lastError);
	}
	
	::SelectObject(hdcMem, hbmOld);
	::DeleteDC(hdcMem);
}

