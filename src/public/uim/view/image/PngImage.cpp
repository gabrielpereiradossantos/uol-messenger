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
#include "pngImage.h"

#include "atlfile.h"
#include "png/PngHelper.h"

namespace UOLMessengerImage
{	

CPngImage::CPngImage(BYTE* pBuffer, int nLen)
{
	ATLASSERT(pBuffer);
	ATLASSERT(nLen);

	if (!pBuffer || !nLen)
	{
		return;
	}

	png_byte *pbImageData = NULL;
	int nWidth;
	int nHeight;
	int nChannel;
	png_color bkgColor = {127, 127, 127};	
	

	BOOL bPngRet = PngLoadImage (pBuffer, nLen, 
								&pbImageData,
								&nWidth, &nHeight, 
								&nChannel, &bkgColor);

	m_pDib = new CDIB();

	BITMAPINFO* pBMI = NULL;
	BYTE*		pBits = NULL;
	BYTE*		pDiData = NULL;

	if (GetBitmapInfo((BYTE**)&pBMI, (BYTE**)&pDiData, nWidth, nHeight))
	{
		pBMI->bmiHeader.biSizeImage = nLen;

		if (FillBitmap(pDiData, nWidth, nHeight, 
					   pbImageData, nWidth, nHeight, nChannel))
		{		
			m_pDib->Attach(pBMI, pDiData, new CDIB::CDIBReleaseBitmapInfoStrategy());			
		}
	}

	if (bPngRet)
	{
		PngFreeImage();

		delete[] pbImageData;
	}
}



BOOL CPngImage::IsValidImage(BYTE* pBuffer, int nLen)
{
	png_byte *pbImageData = NULL;
	int nWidth;
	int nHeight;
	int nChannel;
	png_color bkgColor;

	BOOL bRet = PngLoadImage (pBuffer, nLen, 
							 &pbImageData,
							 &nWidth, &nHeight, 
							 &nChannel, &bkgColor);


	if (bRet)
	{
		PngFreeImage();

		delete[] pbImageData;
	}	

	return bRet;
}


BOOL CPngImage::FillBitmap (
        BYTE *pDiData, int cxWinSize, int cyWinSize,
        BYTE *pbImage, int cxImgSize, int cyImgSize, int cImgChannels)
{
    BYTE *src, *dst;
    BYTE r, g, b, a;
    const int cDIChannels = 3;
    WORD wImgRowBytes;
    WORD wDIRowBytes;
    
    int cxImgPos, cyImgPos;
    int xImg, yImg;
    int xWin, yWin;
    
    
    // calculate the central position

    cxImgPos = (cxWinSize - cxImgSize) / 2;
    cyImgPos = (cyWinSize - cyImgSize) / 2;

    // check for image larger than window
	
	cxImgPos = 0;
	cyImgPos = 0;
    
    // calculate both row-bytes

    wImgRowBytes = cImgChannels * cxImgSize;
    wDIRowBytes = (WORD) ((cDIChannels * cxWinSize + 3L) >> 2) << 2;

    // copy image to screen

    for (yImg = 0, yWin = cyImgPos; yImg < cyImgSize; yImg++, yWin++)
    {
        if (yWin >= cyWinSize)
            break;
        src = pbImage + yImg * wImgRowBytes;
        dst = pDiData + yWin * wDIRowBytes + cxImgPos * cDIChannels;

        for (xImg = 0, xWin = cxImgPos; xImg < cxImgSize; xImg++, xWin++)
        {
            if (xWin >= cxWinSize)
                break;
            r = *src++;
            g = *src++;
            b = *src++;
            *dst++ = b; /* note the reverse order */
            *dst++ = g;
            *dst++ = r;
            if (cImgChannels == 4)
            {
                a = *src++;
            }
        }
    }

    return TRUE;
}

BOOL CPngImage::GetBitmapInfo (BYTE** ppDib, BYTE** ppDiData,
							   int cxWinSize, int cyWinSize)
{
    BYTE*                       pDib = *ppDib;
    BITMAPINFOHEADER*           pbmih;
    WORD                        wDIRowBytes;
    png_color                   bkgBlack = {0, 0, 0};
    png_color                   bkgGray  = {127, 127, 127};
    png_color                   bkgWhite = {255, 255, 255};

    // allocate memory for the Device Independant bitmap

    wDIRowBytes = (WORD) ((3 * cxWinSize + 3L) >> 2) << 2;

    if (pDib)
    {
        delete[] pDib;
        pDib = NULL;
    }

    if (!(pDib = new BYTE[sizeof(BITMAPINFOHEADER) +wDIRowBytes * cyWinSize]))
    {        
        *ppDib = pDib = NULL;

        return FALSE;
    }

    *ppDib = pDib;
    memset (pDib, 0, sizeof(BITMAPINFOHEADER));

    // initialize the dib-structure

    pbmih = (BITMAPINFOHEADER *) pDib;
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth = cxWinSize;
    pbmih->biHeight = -((long) cyWinSize);
    pbmih->biPlanes = 1;
    pbmih->biBitCount = 24;
	pbmih->biCompression = 0;	

	*ppDiData = pDib + sizeof(BITMAPINFOHEADER);    
    
    return TRUE;
}


}
