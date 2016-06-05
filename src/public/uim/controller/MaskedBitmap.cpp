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
 


// MaskedBitmap.cpp: implementation of the CMaskedBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MaskedBitmap.h"


void CMaskedBitmap::Draw (WTL::CDC *pDC, int x, int y)
{
	BITMAP bm;
	GetBitmap (&bm);

	CPoint size (bm.bmWidth, bm.bmHeight);
	pDC->DPtoLP (&size);

	CPoint org (0, 0);
	pDC->DPtoLP (&org);

	WTL::CDC dcMem;
	dcMem.CreateCompatibleDC (pDC->m_hDC);
	HBITMAP pOldBitmap = dcMem.SelectBitmap (this->m_hBitmap);

	dcMem.SetMapMode (pDC->GetMapMode ());

	pDC->BitBlt (x, y, size.x, size.y, dcMem.m_hDC, org.x, org.y, SRCCOPY);

	dcMem.SelectBitmap (pOldBitmap);
		
}

void CMaskedBitmap::DrawTransparent (WTL::CDC *pDC, int x, int y, COLORREF clrTransparency)
{
	BITMAP bm;
	GetBitmap (&bm);
	CPoint size (bm.bmWidth, bm.bmHeight);
	pDC->DPtoLP (&size);

	CPoint org (0, 0);
	pDC->DPtoLP (&org);

	//
	// Create a memory DC (dcImage) and select the bitmap into it.
	//
	WTL::CDC dcImage;
	dcImage.CreateCompatibleDC (pDC->m_hDC);
	HBITMAP pOldBitmapImage = dcImage.SelectBitmap (this->m_hBitmap);

	dcImage.SetMapMode (pDC->GetMapMode ());

	//
	// Create a second memory DC (dcAnd) and in it create an AND mask.
	//
	WTL::CDC dcAnd;
	dcAnd.CreateCompatibleDC (pDC->m_hDC);
	dcAnd.SetMapMode (pDC->GetMapMode ());

	WTL::CBitmap bitmapAnd;
	bitmapAnd.CreateBitmap (bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	HBITMAP pOldBitmapAnd = dcAnd.SelectBitmap (bitmapAnd.m_hBitmap);

	dcImage.SetBkColor (clrTransparency);

	dcAnd.BitBlt (org.x, org.y, size.x, size.y, dcImage.m_hDC , org.x, org.y, SRCCOPY);

	//
	// Create a third memory DC (dcXor) and in it create an XOR mask.
	//
	WTL::CDC dcXor;
	dcXor.CreateCompatibleDC (pDC->m_hDC);
	dcXor.SetMapMode (pDC->GetMapMode ());

	WTL::CBitmap bitmapXor;
	bitmapXor.CreateCompatibleBitmap (dcImage.m_hDC, bm.bmWidth, bm.bmHeight);
	HBITMAP pOldBitmapXor = dcXor.SelectBitmap (bitmapXor.m_hBitmap);

	dcXor.BitBlt (org.x, org.y, size.x, size.y, dcImage.m_hDC, org.x, org.y, SRCCOPY);

	dcXor.BitBlt (org.x, org.y, size.x, size.y, dcAnd.m_hDC, org.x, org.y, 0x220326);

	//
	// Copy the pixels in the destination rectangle to a temporary
	// memory DC (dcTemp).
	//
	WTL::CDC dcTemp;
	dcTemp.CreateCompatibleDC (pDC->m_hDC);
	dcTemp.SetMapMode (pDC->GetMapMode ());

	WTL::CBitmap bitmapTemp;
	bitmapTemp.CreateCompatibleBitmap (dcImage.m_hDC, bm.bmWidth, bm.bmHeight);
	HBITMAP pOldBitmapTemp = dcTemp.SelectBitmap (bitmapTemp.m_hBitmap);

	dcTemp.BitBlt (org.x, org.y, size.x, size.y, pDC->m_hDC, x, y, SRCCOPY);

	//
	// Generate the final image by applying the AND and XOR masks to
	// the image in the temporary memory DC.
	//
	dcTemp.BitBlt (org.x, org.y, size.x, size.y, dcAnd.m_hDC, org.x, org.y, SRCAND);

	dcTemp.BitBlt (org.x, org.y, size.x, size.y, dcXor.m_hDC, org.x, org.y, SRCINVERT);

	//
	// Blit the resulting image to the screen.
	//
	pDC->BitBlt (x, y, size.x, size.y, dcTemp.m_hDC, org.x, org.y, SRCCOPY);

	//
	// Restore the default bitmaps.
	//
	dcTemp.SelectBitmap (pOldBitmapTemp);
	dcXor.SelectBitmap (pOldBitmapXor);
	dcAnd.SelectBitmap (pOldBitmapAnd);
	dcImage.SelectBitmap (pOldBitmapImage);
}
