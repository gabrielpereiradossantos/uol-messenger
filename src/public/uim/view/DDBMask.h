#pragma once

class CDDBMask
{		
public:
	CDDBMask();
	~CDDBMask();

	BOOL Create(HDC hDC, int nX, int nY, int nWidth, int nHeight, UINT crTransparent);
	BOOL ApplyMask(HDC HDC, int nX, int nY, int nWidth, int nHeight, DWORD Rop);

	BOOL TransBlt(HDC hdcDest, int nDx0, int nDy0, int nDw, int nDh,
		HDC hdcSrc,  int nSx0, int nSy0, int nSw, int nSh);

	BOOL TransBlt_FlickFree(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
		HDC hdcSrc,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc);

private:
	void Release(void);

private:
	HDC		m_hMemDC;
	HBITMAP	m_hMask;
	HBITMAP m_hOld;
	int		m_nMaskWidth;
	int		m_nMaskHeight;
};