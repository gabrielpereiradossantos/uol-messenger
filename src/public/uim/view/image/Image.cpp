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
#include "Image.h"
#include "../DrawTransparentDib.h"


namespace UOLMessengerImage
{

// CDIBDrawStrategy
void CDIBDrawStrategy::SetDIB(const CDIBPtr pDib)
{
	m_pDib = pDib;
}


// CDrawTransparentStrategy
BOOL CDrawTransparentStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
{
	DrawTransparentDib(dc, 
					   x, y, cx, cy, 
					   xSrc, ySrc, cx, cy, 
					   m_pDib.get());

	return TRUE;	
};


// CDrawStretchTransparentStrategy
BOOL CDrawStretchTransparentStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
{
	DrawTransparentDib(dc, 
					   x, y, cx, cy, 
					   xSrc, ySrc, m_pDib->GetWidth(), m_pDib->GetHeight(), 
					   m_pDib.get());

	return TRUE;	
};




// CDrawOpaqueStrategy
BOOL CDrawOpaqueStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
{
	int nOldBltMode =  dc.SetStretchBltMode(COLORONCOLOR);

	//int nRet = m_pDib->DrawDIB(dc, x, y, cx, cy, xSrc, ySrc, m_pDib->GetWidth(), m_pDib->GetHeight(), SRCCOPY);	

	int nRet = m_pDib->DrawDIB(dc, x, y, cx, cy, xSrc, ySrc, cx, cy, SRCCOPY);	

	dc.SetStretchBltMode(nOldBltMode);

	return (GDI_ERROR != nRet);
};


// CDrawPhotoStrategy
BOOL CDrawPhotoStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
{
	int nOldBltMode =  dc.SetStretchBltMode(COLORONCOLOR);
	int offsetx, offsety;
	int newcx, newcy;
	int bx = m_pDib->GetWidth();
	int by = m_pDib->GetHeight();

	float rx =  ( ((float) m_pDib->GetWidth()) / ( (float) cx ) );
	float ry =  ( ((float) m_pDib->GetHeight()) / ( (float) cx ) );

	BOOL ycut=TRUE;

	// descubro quem sofrera o corte
	if ( ry > rx )
	{
		ycut = TRUE;
	}

	// se for a altura q sofre o corte
	if (ycut)
	{
		// x eh maximo
		newcx = m_pDib->GetWidth();
		offsetx = 0;

		newcy = (newcx * cy ) / cx;
		offsety = (m_pDib->GetHeight() - newcy) / 2;
	}
	// se a largura sofre o corte
	else
	{
		newcy = m_pDib->GetHeight();
		offsety = 0;

		newcx = (newcy * cx ) / cy;
		offsety = (m_pDib->GetHeight() - newcy) / 2;
	}     

	int nRet = m_pDib->DrawDIB(dc, x, y, cx, cy, offsetx, offsety, newcx, newcx , SRCCOPY);

	dc.SetStretchBltMode(nOldBltMode);

	return (GDI_ERROR != nRet);
};



// CDrawStretchOpaqueStrategy
BOOL CDrawStretchOpaqueStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
{
	int nOldBltMode =  dc.SetStretchBltMode(COLORONCOLOR);

	int nRet = m_pDib->DrawDIB(dc, x, y, cx, cy, xSrc, ySrc, m_pDib->GetWidth(), m_pDib->GetHeight(), SRCCOPY);

	dc.SetStretchBltMode(nOldBltMode);

	return (GDI_ERROR != nRet);
};



class CPatternStrategy9x : public CDIBDrawStrategy
{
protected:
	virtual BOOL Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
	{
		const CDIB* pDib = m_pDib.get();

		if (NULL == pDib)
		{
			return FALSE;
		}		

		BOOL bDrawRet;
		
		const int cxImg = m_pDib->GetWidth();
		const int cyImg = m_pDib->GetHeight();
		const int nWidth = cx + x;
		const int nHeight = cy + y;

		for (int _x = x; _x < nWidth; _x += cxImg)
		{
			for (int _y = y; _y < nHeight; _y += cyImg)
			{
				bDrawRet = GDI_ERROR != pDib->DrawDIB(dc, _x, _y, 
													  cxImg, cyImg, 
													  0, 0, 
												 	  cxImg, cyImg,
													  SRCCOPY);	

				if (FALSE == bDrawRet)
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}

} ;

class CPatternStrategyNT : public CDIBDrawStrategy
{
protected:
	virtual BOOL Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
	{
		CBrush brush;
		brush.CreateDIBPatternBrush(m_pDib->GetBitmapInfo(), DIB_RGB_COLORS);

		HBRUSH hOldBrush = dc.SelectBrush(brush);

		CRect rect(x, y, x + cx, y + cy);		

		BOOL bOK = dc.FillRect(&rect, brush);

		dc.SelectBrush(hOldBrush);
		
		::DeleteObject(brush);
		
		return bOK;
	}
} ;


// CPatternStrategy

CPatternStrategy::CPatternStrategy()
{
	OSVERSIONINFO ovi = {0};	
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&ovi);
	
	if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		m_pStrategy = new CPatternStrategyNT();
	}
	else
	{
		m_pStrategy = new CPatternStrategy9x();
	}	
}

void CPatternStrategy::SetDIB(const CDIBPtr pDib)
{
	if (m_pStrategy)
	{
		m_pStrategy->SetDIB(pDib);
	}
}

BOOL CPatternStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
{
	if (m_pStrategy)
	{
		return m_pStrategy->Draw(dc, x, y, cx, cy, xSrc, ySrc);
	}

	return FALSE;
}


//// CPatternStrategy
//BOOL CPatternStrategy::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc) const
//{
//	CBrush brush;
//	brush.CreateDIBPatternBrush(m_pDib->m_pBMI, DIB_RGB_COLORS);
//
//	HBRUSH hOldBrush = dc.SelectBrush(brush);
//
//	CRect rect(x, y, x + cx, y + cy);		
//
//	BOOL bOK = dc.FillRect(&rect, brush);
//
//	dc.SelectBrush(hOldBrush);
//
//	return bOK;
//}











// CImage
//
CImage::~CImage()
{
}

BOOL CImage::Draw(CDCHandle& dc, int x, int y, int cx, int cy, IUOLMessengerImageDrawStrategyPtr pStrategy) const  
{ 
	return Draw(dc, x, y, cx, cy, 0, 0, pStrategy);
}

BOOL CImage::Draw(CDCHandle& dc, int x, int y, int cx, int cy, int xSrc, int ySrc, IUOLMessengerImageDrawStrategyPtr pStrategy) const  
{ 
	CDIBDrawStrategy* pDIBDrawStrategy = dynamic_cast<CDIBDrawStrategy*>(pStrategy.get());

	ATLASSERT(pDIBDrawStrategy);

	if (pDIBDrawStrategy)
	{
		pDIBDrawStrategy->SetDIB(GetDIB());

		return pDIBDrawStrategy->Draw(dc, x, y, cx, cy, xSrc, ySrc);	
	}

	return FALSE;
}	


int CImage::GetHeight() const
{
	return m_pDib->GetHeight();
}


int CImage::GetWidth() const 
{
	return m_pDib->GetWidth();
}


CDIBPtr CImage::GetDIB() const 
{ 
	return m_pDib; 
}


BOOL CImage::LoadDIB(const CString& strFilePath)
{
	m_pDib = new CDIB();
	return m_pDib->LoadFile(strFilePath);
}


BOOL CImage::LoadDIB(HMODULE hModule, LPCTSTR lpszBitmapName)
{
	m_pDib = new CDIB();
	return m_pDib->LoadBitmap(hModule, lpszBitmapName);
}

};