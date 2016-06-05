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
#include ".\UOLFoneDegradeWindow.h"


CUOLFoneDegradeWindow::CUOLFoneDegradeWindow(void) : 
		m_initialColor(RGB(0xFF, 0xFF, 0xFF)), 
		m_finalColor(RGB(0xFF, 0xFF, 0xFF)), 
		m_STEPS(0), 
		m_OFFSET(0), 
		m_pBrushArray(NULL), 
		m_bkGrndBrushHandle(0)
{
	CClientDC dc(m_hWnd);
	
	if (dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		m_palette.Attach(::CreateHalftonePalette(NULL));
	}
	
	m_STEPS = 192;
	m_OFFSET = static_cast<int>(m_STEPS * 0.2);
	
	m_pBrushArray = new CBrush[m_STEPS];
}


CUOLFoneDegradeWindow::~CUOLFoneDegradeWindow(void)
{
	delete [] m_pBrushArray;
}


void CUOLFoneDegradeWindow::SetDegradeInitialColor(COLORREF initialColor)
{
	m_initialColor = initialColor;
	
	for (int i = 0; i < m_STEPS; i++)
	{
		double r = 0;
		double g = 0;
		double b = 0;
		
		if (i < m_OFFSET)
		{
			r = GetRValue(m_initialColor);
			g = GetGValue(m_initialColor);
			b = GetBValue(m_initialColor);
		}
		else if (i > (m_STEPS - m_OFFSET))
		{
			r = GetRValue(m_finalColor);
			g = GetGValue(m_finalColor);
			b = GetBValue(m_finalColor);
		}
		else
		{
			double d =  1.6 * (static_cast<double>(i - m_OFFSET) / static_cast<double>(m_STEPS));
			
			r = GetRValue(m_initialColor) + (((GetRValue(m_finalColor) - GetRValue(m_initialColor)) * d));
			g = GetGValue(m_initialColor) + (((GetGValue(m_finalColor) - GetGValue(m_initialColor)) * d));
			b = GetBValue(m_initialColor) + (((GetBValue(m_finalColor) - GetBValue(m_initialColor)) * d));
		}
		
		m_pBrushArray[i].CreateSolidBrush(RGB(r, g, b));
	}
}


LRESULT CUOLFoneDegradeWindow::OnEraseBkgnd(
		UINT uMsg, WPARAM hDC, LPARAM lParam, BOOL& bHandled)
{
	CDC eraseDC((HDC)hDC);
	
	RECT rect;
	GetClientRect(&rect);
	
	HPALETTE hOldPalette = NULL;
	
	if (m_palette.m_hPalette != NULL)
	{
		hOldPalette = eraseDC.SelectPalette(m_palette.m_hPalette, FALSE);
		eraseDC.RealizePalette();
	}
	
	DoGradientFill (&eraseDC, &rect);
	
	if (m_palette.m_hPalette != NULL)
	{
		hOldPalette = eraseDC.SelectPalette(hOldPalette, FALSE);
	}
	
	return 1L;
}


void CUOLFoneDegradeWindow::DoGradientFill(CDC* pDC, LPRECT pRect)
{
	CBitmap		bitmap;
	
	int nWidth = pRect->right - pRect->left;
	int nHeight = pRect->bottom - pRect->top;
	
	if (m_dcBackGnd.m_hDC != NULL)
	{
		m_dcBackGnd.DeleteDC();
	}
	
	bitmap.CreateCompatibleBitmap(pDC->m_hDC, nWidth, nHeight);
	
	m_dcBackGnd.CreateCompatibleDC(pDC->m_hDC);
	m_dcBackGnd.SelectBitmap(bitmap);
	
	FillUpCDCBitmap(&m_dcBackGnd, nWidth, nHeight, 0);
	
	if (!m_bkGrndBrush.IsNull())
	{
		m_bkGrndBrush.DeleteObject();
	}
	m_bkGrndBrushHandle = m_bkGrndBrush.CreatePatternBrush(bitmap);
	
	pDC->BitBlt(0, 0, nWidth, nHeight, m_dcBackGnd.m_hDC, 0, 0, SRCCOPY);
	
	bitmap.DeleteObject();
}


void CUOLFoneDegradeWindow::FillUpCDCBitmap(
		CDC *fillUpCDC, int width, int height, int offset)
{
	for (int i = 0; i < height; i++)
	{
		RECT rect={0, i, width, i + 1};
		fillUpCDC->FillRect(&rect, m_pBrushArray[(i * (m_STEPS - 1)) / height].m_hBrush);
	}
}


LRESULT CUOLFoneDegradeWindow::OnCtlColor(
		UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetBkMode((HDC)wParam, TRANSPARENT);
	
	return (LRESULT) m_bkGrndBrushHandle;
}

