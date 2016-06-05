#include "StdAfx.h"

#include ".\dib.h"

CDIB::CDIB(void)
{
	::ZeroMemory(static_cast<CDIB*>(this), sizeof(CDIB));
}

CDIB::~CDIB(void)
{
	Dettach();
}

BOOL CDIB::Attach(BITMAPINFO * pDIB, BYTE * pBits, CDIB::IDIBReleaseMemoryStrategyPtr pDIBReleaseMemoryStrategy)
{
	if (::IsBadReadPtr(pDIB, sizeof(BITMAPCOREHEADER)))
	{
		return FALSE;
	}

	Dettach();

	m_pBMI      = pDIB;
	m_pDIBReleaseMemoryStrategy = pDIBReleaseMemoryStrategy;

	DWORD size = * (DWORD *) pDIB; // always DWORD size, key to information header

	int compression;
	// gather information from bitmap information header structures
	switch ( size )
	{
	case sizeof(BITMAPCOREHEADER):
		{
			BITMAPCOREHEADER * pHeader = (BITMAPCOREHEADER *) pDIB;

			m_nWidth    = pHeader->bcWidth;
			m_nHeight   = pHeader->bcHeight;
			m_nPlanes   = pHeader->bcPlanes;
			m_nBitCount = pHeader->bcBitCount;
			m_nImageSize= 0;
			compression = BI_RGB;

			if ( m_nBitCount <= 8 )
			{
				m_nClrUsed   = 1 << m_nBitCount;				
				m_pRGBTRIPLE = (RGBTRIPLE *) ((BYTE *) m_pBMI + size);

				m_pBits      = (BYTE *) & m_pRGBTRIPLE[m_nClrUsed];
			}
			else
				m_pBits      = (BYTE *) m_pBMI + size;
			break;
		}

	case sizeof(BITMAPINFOHEADER):
	case sizeof(BITMAPV4HEADER):
#if (WINVER >= 0x0500)
	case sizeof(BITMAPV5HEADER):
#endif 
		{
			BITMAPINFOHEADER * pHeader = & m_pBMI->bmiHeader;

			m_nWidth    = pHeader->biWidth;
			m_nHeight   = pHeader->biHeight;
			m_nPlanes   = pHeader->biPlanes;
			m_nBitCount = pHeader->biBitCount;
			m_nImageSize= pHeader->biSizeImage;
			compression = pHeader->biCompression;

			m_nClrUsed  = pHeader->biClrUsed;			

			if ( m_nBitCount<=8 )
				if ( m_nClrUsed==0 )	// 0 means full color table
					m_nClrUsed = 1 << m_nBitCount;

			if ( m_nClrUsed )	// has a color table
			{
				if (compression == BI_BITFIELDS)
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size + 3*sizeof(DWORD));
				}
				else
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size);

				m_pBits = (BYTE *) & m_pRGBQUAD[m_nClrUsed];
			}
			else
			{
				if (compression == BI_BITFIELDS)
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pBits      = (BYTE *) m_pBMI + size + 3 * sizeof(DWORD);
				}
				else
					m_pBits      = (BYTE *) m_pBMI + size;
			}
			break;
		}

	default:
		return FALSE;
	}

	if ( pBits )
		m_pBits = pBits;

	int nBPS	  = (m_nWidth * m_nBitCount + 31) / 32 * 4;

	if (m_nHeight < 0 ) // top-down bitmap
	{
		m_nHeight = - m_nHeight;				
	}	

	if ( m_nImageSize==0 )
		m_nImageSize = nBPS * m_nPlanes * m_nHeight;

	// convert compression mode to image format
	switch ( m_nBitCount )
	{
	case 0:
	case 1:
	case 2:
	case 4:
	case 8:	
	case 16:
	case 24:
	case 32:
		break;
	default:
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDIB::LoadFile(const TCHAR * pFileName)
{
	if (NULL == pFileName)
	{
		return FALSE;
	}

	HANDLE handle = ::CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == handle)
	{
		return FALSE;
	}

	BITMAPFILEHEADER bmFH;

	DWORD dwRead = 0;
	::ReadFile(handle, &bmFH, sizeof(bmFH), &dwRead, NULL);

	if ( (bmFH.bfType == 0x4D42) && (bmFH.bfSize<=GetFileSize(handle, NULL)) )
	{
		BITMAPINFO * pDIB = (BITMAPINFO *) new BYTE[bmFH.bfSize];

		if ( pDIB )
		{
			::ReadFile(handle, pDIB, bmFH.bfSize, & dwRead, NULL);
			::CloseHandle(handle);

			return Attach(pDIB, NULL, new CDIB::CDIBReleaseBitmapInfoStrategy());
		}
	}
	
	::CloseHandle(handle);

	return FALSE;
}


BOOL CDIB::LoadBitmap(HMODULE hModule, LPCTSTR lpszBitmapName)
{
	HRSRC hResFind = ::FindResource(hModule, lpszBitmapName, RT_BITMAP);

	if (hResFind != NULL)
	{
		HGLOBAL hResData = ::LoadResource(hModule, hResFind);

		if (hResData != NULL)
		{
			BITMAPINFO * pDIB = (BITMAPINFO *) ::LockResource(hResData);

			if (pDIB != NULL)
			{
				return Attach(pDIB, NULL, IDIBReleaseMemoryStrategyPtr());
			}
		}
	}

	return FALSE;
}



void CDIB::Dettach()
{
	if (m_pDIBReleaseMemoryStrategy)
	{
		m_pDIBReleaseMemoryStrategy->Dispose(this);
	}
}

int CDIB::DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, DWORD rop) const
{
	if ( m_pBMI )
		return ::StretchDIBits(hDC, dx, dy, dw, dh, sx, sy, sw, sh, 
		m_pBits, m_pBMI, DIB_RGB_COLORS, rop);
	else
		return GDI_ERROR;
}

int CDIB::GetWidth(void) const	
{
	return m_nWidth;  
}

int CDIB::GetHeight(void) const	
{ 
	return m_nHeight; 
}

const void* CDIB::GetBitmapInfo() const
{
	return m_pBMI;
}

const BYTE* CDIB::GetBitmapBits() const
{
	return m_pBits;

}