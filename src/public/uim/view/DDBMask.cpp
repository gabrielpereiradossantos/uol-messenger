#include "StdAfx.h"
#include ".\ddbmask.h"

CDDBMask::CDDBMask(void)
{
	m_hMemDC = NULL;
	m_hMask  = NULL;
	m_hOld   = NULL;
}

CDDBMask::~CDDBMask(void)
{
	Release();
}

// Create a monochrome mask bitmap from a source DC
BOOL CDDBMask::Create(HDC hDC, int nX, int nY, int nWidth, int nHeight, UINT crTransparent)
{
	Release();

	RECT rect = { nX, nY, nX + nWidth, nY + nHeight };
	::LPtoDP(hDC, (POINT *) & rect, 2);								  

	m_nMaskWidth  = ::abs(rect.right - rect.left);
	m_nMaskHeight = ::abs(rect.bottom - rect.top);					  // get real size

	m_hMemDC = ::CreateCompatibleDC(hDC);
	m_hMask  = ::CreateBitmap(m_nMaskWidth, m_nMaskHeight, 1, 1, NULL); // monochrome bitmap
	m_hOld   = (HBITMAP) ::SelectObject(m_hMemDC, m_hMask);

	COLORREF oldBk = ::SetBkColor(hDC, crTransparent);	// map crTransparent to 1, white
	BOOL rslt = ::StretchBlt(m_hMemDC, 0, 0, m_nMaskWidth, m_nMaskHeight, hDC, nX, nY, nWidth, nHeight, SRCCOPY);
	::SetBkColor(hDC, oldBk);

	return rslt;
}


BOOL CDDBMask::ApplyMask(HDC hDC, int nX, int nY, int nWidth, int nHeight, DWORD rop)
{
	COLORREF oldFore = ::SetTextColor(hDC, RGB(0, 0, 0));			// Foreground Black
	COLORREF oldBack = ::SetBkColor(hDC,   RGB(255, 255, 255));	// Background White

	BOOL rslt = ::StretchBlt(hDC, nX, nY, nWidth, nHeight, m_hMemDC, 0, 0, m_nMaskWidth, m_nMaskHeight, rop);

	::SetTextColor(hDC, oldFore);
	::SetBkColor(hDC,   oldBack);

	return rslt;
}


// D=D^S, D=D & Mask, D=D^S	--> if (Mask==1) D else S
BOOL CDDBMask::TransBlt(HDC hdcDest, int nDx0, int nDy0, int nDw, int nDh,
						HDC hdcSrc,  int nSx0, int nSy0, int nSw, int nSh)
{
	::StretchBlt(hdcDest, nDx0, nDy0, nDw, nDh, hdcSrc, nSx0, nSy0, nSw, nSh, SRCINVERT);		// D^S

	ApplyMask(hdcDest, nDx0, nDy0, nDw, nDh, SRCAND);	// if trans D^S else 0

	return ::StretchBlt(hdcDest, nDx0, nDy0, nDw, nDh, hdcSrc, nSx0, nSy0, nSw, nSh, SRCINVERT);	// if trans D else S
}


BOOL CDDBMask::TransBlt_FlickFree(HDC hdcDest, int nDx0, int nDy0, int nDw, int nDh,
								  HDC hdcSrc,  int nSx0, int nSy0, int nSw, int nSh)
{
	::StretchBlt(hdcSrc, nSx0, nSy0, nSw, nSh, 
		hdcDest, nDx0, nDy0, nDw, nDh, SRCINVERT); // D^S

	ApplyMask(hdcSrc, nSx0, nSy0, nSw, nSh,	0x220000); // if trans 0 else D^S

	return ::StretchBlt(hdcDest, nDx0, nDy0, nDw, nDh, 		// if trans D else S
		hdcSrc, nSx0, nSy0, nSw, nSh, SRCINVERT);
}


void CDDBMask::Release(void)
{
	if ( m_hMemDC )
	{
		::SelectObject(m_hMemDC, m_hOld);
		::DeleteObject(m_hMemDC);	
		m_hMemDC = NULL;
		m_hOld = NULL;
	}

	if ( m_hMask )
	{
		::DeleteObject(m_hMask);	
		m_hMask  = NULL;
	}
}