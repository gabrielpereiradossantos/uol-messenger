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
#include "UOLProgressBar.h"
#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"


#define OUTTER_BORDER	1
#define	INNER_BORDER	2


CUOLProgressBar::CUOLProgressBar(CProgressBarElementPtr pProgressBarElement) :
	m_BorderRect(0, 0, 0, 0),
	m_nRoundX(0),
	m_nRoundY(0),
	m_nMin(0),
	m_nMax(100),
	m_nPos(0)
{
	if (pProgressBarElement)
	{
		// Get colors.
		//
		m_clrBackground = pProgressBarElement->GetBackgroundColor();
		m_clrBorder = pProgressBarElement->GetBorderColor();
		m_clrFill = pProgressBarElement->GetFillColor();
		
		// Get images.
		//
		m_hLeftImage = pProgressBarElement->GetProgressLeftImage();
		m_hMiddleImage = pProgressBarElement->GetProgressMiddleImage();
		m_hRightImage = pProgressBarElement->GetProgressRightImage();
		
		// Update image progressbar flag.
		//
		if ( (m_hLeftImage != NULL) && (m_hMiddleImage != NULL) && (m_hRightImage != NULL) )
		{
			m_bIsImageProgressBar = TRUE;
		}
		else
		{
			m_bIsImageProgressBar = FALSE;
		}
	}
}


CUOLProgressBar::~CUOLProgressBar()
{
}


void CUOLProgressBar::SetRect(CRect rect)
{
	m_BorderRect = rect;
}


void CUOLProgressBar::SetRoundedCorners(int rx, int ry)
{
	m_nRoundX = rx;
	m_nRoundY = ry;
}


void CUOLProgressBar::Draw(CDCHandle& dc)
{
	// Create background brush
	CBrush brushBackground;
	brushBackground.CreateSolidBrush(m_clrBackground);

	// Save old brush
	HBRUSH hOldBrush = dc.SelectBrush(brushBackground);

	// Create pen
	CPen penBorder;
	penBorder.CreatePen(PS_SOLID, 1, m_clrBorder);

	// Save old pen
	HPEN hOldPen = dc.SelectPen(penBorder);

	// Draw a rounded rectangle
	dc.RoundRect(m_BorderRect, CPoint(m_nRoundX, m_nRoundY));

	// Put back the old objects.
	dc.SelectBrush(hOldBrush);
	dc.SelectPen(hOldPen);

	if (!m_bIsImageProgressBar)
	{
		// Fill rectangle
		int x1 = m_BorderRect.left + OUTTER_BORDER;
		int x2 = GetPosPixel(m_BorderRect);
		int x3 = m_nRoundX - INNER_BORDER;
		int y1 = m_BorderRect.top + OUTTER_BORDER;
		int y2 = m_BorderRect.bottom;
		int y3 = m_nRoundY - INNER_BORDER;
		CRgn fillRgn;
		fillRgn.CreateRoundRectRgn(x1, y1, x2, y2, x3, y3);
		
		CBrush brushFill;
		brushFill.CreateSolidBrush(m_clrFill);
		
		dc.FillRgn((HRGN)fillRgn, brushFill);
		
		DeleteObject(fillRgn);
		DeleteObject(brushFill);
	}
	else
	{
		CSize sizeLeftImage;
		GetImageSize(m_hLeftImage, sizeLeftImage);
		CSize sizeRightImage;
		GetImageSize(m_hRightImage, sizeRightImage);
		
		int x1 = m_BorderRect.left + OUTTER_BORDER;
		int x2 = GetPosPixel(m_BorderRect) - OUTTER_BORDER;
		int y1 = m_BorderRect.top + OUTTER_BORDER;
		int y2 = y1 + sizeLeftImage.cy;
		
		if (DrawImageToCDC(dc, x1, y1, 0, 0, m_hLeftImage))
		{
			CRect middleRect(0, 0, 0, 0);
			
			if ( (x2 - x1) > (sizeLeftImage.cx + sizeRightImage.cx) )
			{
				middleRect.SetRect(x1 + sizeLeftImage.cx, y1, x2 - sizeRightImage.cx, y2);
				
				DrawImageToCDC(dc, middleRect.left, middleRect.top, 
						middleRect.Width(), middleRect.Height(), m_hMiddleImage);
			}
			
			if ((x2 - x1) > sizeLeftImage.cx)
			{
				DrawImageToCDC(dc, x1 + sizeLeftImage.cx + middleRect.Width(), y1, 0, 0, m_hRightImage);
			}
		}
	}
	
	DeleteObject(brushBackground);
	DeleteObject(penBorder);
}


void CUOLProgressBar::SetRange(int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;
}


void CUOLProgressBar::SetPos(int nPos)
{
	if ((nPos >= m_nMin) && (nPos <= m_nMax))
	{
		m_nPos = nPos;
	}
	else if(nPos < m_nMin)
	{
		m_nPos = m_nMin;
	}
	else if(nPos > m_nMax)
	{
		m_nPos = m_nMax;
	}
}


int CUOLProgressBar::GetPos()
{
	return m_nPos;
}


int CUOLProgressBar::DrawImageToCDC(CDCHandle& dc, 
		int x, int y, int width, int height, HBITMAP hImage)
{
	int iDrawnWidth = 0;
	int widthToDraw = width;
	int heightToDraw = height;
	
	CSize sizeImage;
	GetImageSize(hImage, sizeImage);
	
	if (!widthToDraw)
	{
		widthToDraw = sizeImage.cx;
	}
	
	if (!heightToDraw)
	{
		heightToDraw = sizeImage.cy;
	}
	
	HDC hImageDC = ::CreateCompatibleDC(dc.m_hDC);
	
	if (hImageDC)
	{
		HGDIOBJ hOldBitmap = ::SelectObject(hImageDC, hImage);
		
		if (dc.TransparentBlt(x, y, widthToDraw, heightToDraw, hImageDC, 
				0, 0, sizeImage.cx, sizeImage.cy, RGB(0x00, 0x00, 0xFF)))
		{
			iDrawnWidth = sizeImage.cx;
		}
		
		::SelectObject(hImageDC, hOldBitmap);
		::DeleteObject(hImageDC);
	}
	
	return iDrawnWidth;
}


void CUOLProgressBar::GetImageSize(HBITMAP hImage, CSize& sizeImage)
{
	if (hImage)
	{
		CBitmap bmpImage;
		bmpImage.Attach(hImage);
		
		bmpImage.GetSize(sizeImage);
		
		bmpImage.Detach();
	}
}


int CUOLProgressBar::GetPosPixel(CRect rect)
{
	// Calculate max pixels inside rectagle
	int nMaxWidth = rect.Width();
	int nRangeMax = abs(m_nMax - m_nMin);
	
	// avoid division by zero
	if (nRangeMax == 0)
	{
		nRangeMax = 1;
	}

	double dPos = m_nPos;
	double dMaxWidth = nMaxWidth;
	double dRangeMax = nRangeMax;

	int nPosPixel = rect.left + static_cast<int>(ceil((dPos * dMaxWidth) / dRangeMax));

	ATLASSERT(nPosPixel >= rect.left);

	return nPosPixel;
}

