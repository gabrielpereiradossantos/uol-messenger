#include "StdAfx.h"
#include "membm.h"
#include "crtdbg.h"	//include for debug macros from CRT

#if _WIN32_WINDOWS >= 0x0410	//msimg32 library is provided only for winver > 95
#pragma comment( lib, "msimg32" )
#endif

/////////////////////////// CMemBm implementation //////////////////////////////////////
// create memory bitmap from source bitmap
bool CMemBm::
	Create(HDC hDC, HBITMAP hbmSrc, int iLeft, int iTop,
		int iWidth, int iHeight, DWORD dwRop )
{
	//if already created - do nothing
	if( !m_bCreated )
	{
		//we cannot use NULL hDC and NULL hbmSrc at the same time-
		//result will be nonsense (copy empty MemDC contents to our bitmap)
		//use appropriate DC as source DC
		HDC hdcSrc, hdcTmpSrc = NULL;
		if( hDC )
			hdcSrc = hDC;
		else
		{
			//use this only if input hDC is NULL - get screen DC
			hdcTmpSrc = GetDC( NULL );
			_ASSERT(hdcTmpSrc);
			hdcSrc = hdcTmpSrc;
		}

		m_hdcImg = CreateCompatibleDC( hdcSrc );
		_ASSERT(m_hdcImg);

		_ASSERT(hdcSrc);

		if( m_hdcImg )
		{
			m_hbmImg = CreateCompatibleBitmap( hdcSrc, iWidth, iHeight );
			_ASSERT( m_hbmImg );

			if(m_hbmImg)
			{
				//select our memory bitmap
				m_hbmOld = SelectObject( m_hdcImg, m_hbmImg );

				//select source bitmap if it's not NULL
				HANDLE hbmOldSrc = NULL;
				//check if we actually need selection of src image in src DC
				//(it may be already selected in it in case of copying contents of
				//another CMemBm object )
				bool bNeedSrcSelect = 
					hbmSrc && (GetCurrentObject( hdcSrc, OBJ_BITMAP) != hbmSrc );
				if( bNeedSrcSelect )
					hbmOldSrc = SelectObject( hdcSrc, hbmSrc );

				//copy bitmap bits
				BitBlt( m_hdcImg, 0, 0, iWidth, iHeight,
					hdcSrc, iLeft, iTop, dwRop );

				//setup sizes of our image
				m_iWidth = iWidth;
				m_iHeight = iHeight;
				//setup color information
				BITMAP bmInfo;
				memset( &bmInfo, 0, sizeof BITMAP );
				GetObject( m_hbmImg, sizeof BITMAP, &bmInfo );
				m_iPlanes = bmInfo.bmPlanes;
				m_iBpp = bmInfo.bmBitsPixel;

				//set "created" flag
				m_bCreated = true;

				//deselect source bitmap
				if( bNeedSrcSelect )
					SelectObject( hdcSrc, hbmOldSrc );
			}

			//if temporary dc was accessed - release it
			if( hdcTmpSrc )
				ReleaseDC( NULL, hdcTmpSrc );
		}
	}

	return m_bCreated;
};

bool CMemBm::
	Create( HDC hDC, int iWidth, int iHeight,
		UINT cPlanes, UINT cBitsPerPel, const void *lpvBits )
{
	//if already created - do nothing
	if( !m_bCreated )
	{

		m_hdcImg = CreateCompatibleDC( hDC );
		_ASSERT(m_hdcImg);
		if( m_hdcImg )
		{
			m_hbmImg = CreateBitmap( iWidth, iHeight, cPlanes, cBitsPerPel, lpvBits );
			_ASSERT( m_hbmImg );

			if(m_hbmImg)
			{
				//select our memory bitmap
				m_hbmOld = SelectObject( m_hdcImg, m_hbmImg );

				//setup sizes of our image
				m_iWidth = iWidth;
				m_iHeight = iHeight;
				m_iPlanes = cPlanes;
				m_iBpp = cBitsPerPel;

				//set "created" flag
				m_bCreated = true;
			}
		}
	}

	return m_bCreated;
};

bool CMemBm::
	Create( const CMemBm& bmSrc, DWORD dwRop )
{
	return Create( bmSrc, bmSrc.m_hbmImg, 0, 0, bmSrc.GetWidth(), bmSrc.GetHeight(), dwRop );
};

bool CMemBm::
	Create( HINSTANCE hInst, LPCTSTR pszName, BOOL bFile /*= FALSE*/ )
{
	if( !m_bCreated )
	{
		UINT nLoadFlags = LR_DEFAULTCOLOR | LR_DEFAULTSIZE;
		if( bFile )
			nLoadFlags |= LR_LOADFROMFILE;

		HBITMAP hbmImg = (HBITMAP)LoadImage(hInst, pszName,
			IMAGE_BITMAP, 0, 0, nLoadFlags );
		_ASSERT( hbmImg );
		if( hbmImg )
		{
			BITMAP bmInfo;
			//prepare structure for bitmap parameters query
			memset( &bmInfo, 0, sizeof BITMAP );

			m_hbmImg = hbmImg;

			m_hdcImg = CreateCompatibleDC( NULL );
			_ASSERT( m_hdcImg );
			if( m_hdcImg && GetObject( m_hbmImg, sizeof BITMAP, &bmInfo ) )
			{
				m_hbmOld = SelectObject( m_hdcImg, m_hbmImg );
				
				m_iWidth = bmInfo.bmWidth;
				m_iHeight = bmInfo.bmHeight;
				m_iPlanes = bmInfo.bmPlanes;
				m_iBpp = bmInfo.bmBitsPixel;

				m_bCreated = true;
			}
		}
	}

	return m_bCreated;
}

//cleanup object data
void CMemBm::
	Cleanup()
{
	if( m_hdcImg )
	{
		SelectObject( m_hdcImg, m_hbmOld );
		DeleteDC(m_hdcImg);
	}

	//this "if" is not necessary,
	//as we can call DeleteObject on non valid handles, but just in case
	if( m_hbmImg )
	{
		DeleteObject(m_hbmImg);
	}

	//reset data members to 0
	Initialize();
};

//perform pattern fill, adjusting brush origin if neccessary
void CMemBm::
	Fill( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
		bool bAdjustBrushOrg, DWORD dwRop )
{
	//if image is absent - do nothing
	if( !m_bCreated )
		return;

	//create brush from image
	HBRUSH hbrPat = CreatePatternBrush( m_hbmImg );
	_ASSERT( hbrPat );
	
	//do brush origin adjustment as neccessary
	POINT pntOldOrg;
	if( bAdjustBrushOrg )
	{
#if _WIN32_WINDOWS >= 0x0410	//automatic brush tracking not supported in Win9x
		UnrealizeObject( hbrPat );
#endif
		SetBrushOrgEx( hDC, iLeft, iTop, &pntOldOrg );
	}
	
	//select our brush to hDC
	HANDLE hbrOldPat = SelectObject( hDC, hbrPat );
	//do patterned fill
	PatBlt( hDC, iLeft, iTop, iWidth, iHeight, dwRop );
	//deselect brush
	SelectObject( hDC, hbrOldPat );

	//reset brush origin
	if( bAdjustBrushOrg )
	{
#if _WIN32_WINDOWS >= 0x0410	//automatic brush tracking not supported in Win9x
		UnrealizeObject( hbrPat );
#endif
		SetBrushOrgEx( hDC, pntOldOrg.x, pntOldOrg.y, NULL );
	}

	//delete brush
    DeleteObject( hbrPat );
};

//do 1-to-1 draw
void CMemBm::
	Draw( HDC hDC, int iLeft, int iTop, DWORD dwRop )
{
	//if not created - do nothing
	if( !m_bCreated )
		return;

	BitBlt( hDC, iLeft, iTop, m_iWidth, m_iHeight, m_hdcImg, 0, 0, dwRop );
};

//perform draw (simple or stretched)
void CMemBm::
	Draw( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
		DWORD dwRop, int iStretchMode )
{
	//if not created - do nothing
	if( !m_bCreated )
		return;

	//check if we need stretching
	if( RequireStretch(iWidth, iHeight) )
	{
		//set new stretch mode
		int iOldStretchBltMode = SetStretchBltMode( hDC, iStretchMode );
		
		//do stretch blt
		StretchBlt( hDC, iLeft, iTop, iWidth, iHeight,
			m_hdcImg, 0, 0, m_iWidth, m_iHeight, dwRop );

		//restore old stretch mode
		SetStretchBltMode( hDC, iOldStretchBltMode );
	}
	else
	{
		//do 1-to-1 drawing
		Draw( hDC, iLeft, iTop, dwRop ); 
	}
};

//get bitmap width
int CMemBm::
	GetWidth(void) const
{
	return m_iWidth;
}

//get bitmap height
int CMemBm::
	GetHeight(void) const
{
	return m_iHeight;
}

//get bitmap's bpp
int CMemBm::GetBpp(void)
{
	return m_iBpp;
}

//get bitmap's color planes
int CMemBm::GetPlanes(void)
{
	return m_iPlanes;
}

//draws bitmap transparently (stretching it when necessary)
void CMemBm::
	DrawTrans( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
				COLORREF crTransparent )
{
	//if our object is uninitialized - do nothing
	if( !m_bCreated )
		return;

#if _WIN32_WINDOWS >= 0x0410	//msimg library is provided only for winver > 95
	//call WinAPI realization of stretching - capable transparent drawing
	TransparentBlt( hDC, iLeft, iTop, iWidth, iHeight,
		m_hdcImg, 0, 0, m_iWidth, m_iHeight, crTransparent );
#else
	//non-API realization of stretching - capable transparent drawing
	CMemBm bmAnd;
	CMemBm bmXor;	
	CMemBm bmTemp;	//"collation" bitmap for flicker-free drawing

	COLORREF crOldBkColor = SetBkColor( m_hdcImg, crTransparent );
	if( bmAnd.Create( hDC, m_iWidth, m_iHeight, 1, 1, NULL ) &&
		bmXor.Create( *this ) )
	{
		// create AND mask 
		Draw( bmAnd, 0, 0 ); 
		// create XOR mask
		bmAnd.Draw( bmXor, 0, 0, 0x220326 ); 

		bmTemp.Create( hDC, NULL, iLeft, iTop, iWidth, iHeight ); //copy hDC contents to temporary
		
		// blend the AND and XOR masks into the temporary bitmap
		bmAnd.Draw( bmTemp, 0, 0, iWidth, iHeight, SRCAND ); 
		bmXor.Draw( bmTemp, 0, 0, iWidth, iHeight, SRCINVERT); 
		
		// draw the resulting image to the hDC
		bmTemp.Draw( hDC, iLeft, iTop, iWidth, iHeight );
	}
	SetBkColor( m_hdcImg, crOldBkColor );

#endif
}

void CMemBm::
	DrawAlpha( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight, const BLENDFUNCTION& blendFunc )
{
#if _WIN32_WINDOWS >= 0x0410	//msimg library is provided only for winver > 95
	AlphaBlend( hDC, iLeft, iTop, iWidth, iHeight, m_hdcImg, 0, 0, m_iWidth, m_iHeight, blendFunc );
#endif
}
/////////////////////////// CMemBm implementation end///////////////////////////////////