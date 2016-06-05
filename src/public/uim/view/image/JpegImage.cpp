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
#include "jpegImage.h"

#include "..\jlib\jpeglib.h"

namespace UOLMessengerImage
{
struct CJepegStructDestructor
{
	CJepegStructDestructor(jpeg_decompress_struct& dinfo) : 
		m_dinfo(dinfo) { }

	~CJepegStructDestructor()
	{		
		try
		{
			m_dinfo.jpeg_finish_decompress();
		}
		catch(int)
		{
			m_dinfo.src->term_source(&m_dinfo);
			jpeg_abort(&m_dinfo);
		}
		
		m_dinfo.jpeg_destroy_decompress();	
	}

	jpeg_decompress_struct& m_dinfo;
};

CComAutoCriticalSection	CJpegImage::ms_critSec;

CJpegImage::CJpegImage()
{
	m_nBitmapSize = 0;	
	m_nJPEGSize = 0;

	m_pJPEG = NULL;
	m_pBMI  = NULL;
	m_pBits = NULL;	
}

CJpegImage::~CJpegImage()
{
	Release();
}

BOOL CJpegImage::Load(BYTE* pBuffer, int nLen)
{
	Release();

	if (DecodeJPEG(pBuffer, nLen, m_pBMI, m_pBits, m_nBitmapSize))
	{
		m_pDib = new CDIB();		

		BITMAPINFO* pDIB = new BITMAPINFO;
		::CopyMemory(pDIB, m_pBMI, sizeof(BITMAPINFO));			

		BYTE* pBits = new BYTE[m_nBitmapSize];
		::CopyMemory(pBits, m_pBits, m_nBitmapSize);

		BOOL bOK = m_pDib->Attach(pDIB, 
								  pBits, 
								  new CDIB::CDIBReleaseBitmapInfoAndBitsInfoStrategy());	

		Release();

		return bOK;
	}

	return FALSE;
}

void CJpegImage::Release(void)
{
	if (m_pBMI)
	{
		delete [] (BYTE *) m_pBMI;
		m_pBMI = NULL;
	}

	if (m_pBits)
	{
		delete [] (BYTE *) m_pBits;
		m_pBits = NULL;
	}
}

BOOL CJpegImage::DecodeJPEG(const void * jpegimage, int jpegsize, LPBITMAPINFO& pBMI, LPBYTE& pBits, int& nBitmapSize)
{
	struct jpeg_decompress_struct dinfo;

	try
	{		
		CComCritSecLock<CComAutoCriticalSection> theLock(ms_critSec);

		CJepegStructDestructor jepegStructDestructor(dinfo);

		jpeg_error_mgr jerr;
		dinfo.err  = &jerr;

		dinfo.jpeg_create_decompress();

		jpeg_const_src(&dinfo, (const BYTE *) jpegimage, jpegsize);

		dinfo.jpeg_read_header(TRUE);
		dinfo.jpeg_start_decompress();

		int	bps = 0;		

		if (Allocate(dinfo.image_width, dinfo.image_height, dinfo.out_color_components, true, pBMI, pBits, bps, nBitmapSize))
		{			
			for (int h=dinfo.image_height-1; h>=0; h--) // bottom-up
			{
				BYTE * addr = pBits + bps * h; 
				dinfo.jpeg_read_scanlines(& addr, 1);
			}
		}				
	}
	catch (int /*msg_code*/)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CJpegImage::Allocate(int width, int height, int channels, bool bBits, BITMAPINFO*& pBMI, LPBYTE& pBits, int& bps, int& nBitmapSize)
{
	switch (channels)
	{
	case 3:
		pBMI = new BITMAPINFO;
		::ZeroMemory(pBMI, sizeof(BITMAPINFO));
		break;

	case 1:
		pBMI = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
		{
			for (int i = 0; i < 256; i++)
			{
				pBMI->bmiColors[i].rgbRed		= i;
				pBMI->bmiColors[i].rgbGreen		= i;
				pBMI->bmiColors[i].rgbBlue		= i;
				pBMI->bmiColors[i].rgbReserved	= 0;
			}
		}
		break;

	default:
		return FALSE;
	}

	if (pBMI == NULL )
	{
		return FALSE;
	}

	memset(pBMI, 0, sizeof(BITMAPINFOHEADER));

	pBMI->bmiHeader.biSize     = sizeof(pBMI->bmiHeader);
	pBMI->bmiHeader.biWidth    = width;
	pBMI->bmiHeader.biHeight   = height;
	pBMI->bmiHeader.biBitCount = channels * 8;
	pBMI->bmiHeader.biPlanes   = 1;

	// allocate bitmap
	bps = (width * channels + 3) / 4 * 4;

	if ( bBits )
	{
		nBitmapSize = bps * height;
		pBits = (BYTE *) new BYTE[nBitmapSize];		
	}

	return TRUE;
}


}
