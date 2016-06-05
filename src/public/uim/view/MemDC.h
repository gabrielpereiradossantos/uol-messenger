#pragma once

/////////////////////////////////////////////////////////////////////////////
// Additional GDI/USER wrappers
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001-2002 Bjarke Viksoe.
// Thanks to Daniel Bowen for COffscreenDrawRect.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//


template <bool t_bManaged>
class CMemDCT : public CDCT<t_bManaged>
{
public:
	CMemDCT(HDC hDC, LPRECT pRect = NULL, int nOpacity = 100) : 
	  m_nOpacity(nOpacity)
	{
		ATLASSERT(hDC != NULL);
		m_dc = hDC;
		CreateCompatibleDC(m_dc);

		if (pRect == NULL)
		{
			m_dc.GetClipBox(&m_rc);
			::LPtoDP(m_dc, (LPPOINT) &m_rc, sizeof(RECT) / sizeof(POINT));
		}    
		else 
		{
			m_rc = *pRect; 
		}
		
		m_bitmap.CreateCompatibleBitmap(m_dc, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);
		m_hOldBitmap = SelectBitmap(m_bitmap);		

		if (m_nOpacity < 100)
		{
			BitBlt(0, 0, 
					m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
					m_dc, 0, 0, SRCCOPY);
		}

		if( pRect == NULL ) 
		{
			::DPtoLP(m_dc, (LPPOINT) &m_rc, sizeof(RECT) / sizeof(POINT));
		}

		SetWindowOrg(m_rc.left, m_rc.top, &m_ptOldWindowOrg);	  
	}

	~CMemDCT()
	{
		Update();
		
		// Swap back the original bitmap.
		SelectBitmap(m_hOldBitmap);

		SetWindowOrg(m_ptOldWindowOrg);	  
	}

	void Update()
	{
		if (m_nOpacity < 100)
		{
			// 1/5, 1/4, 1/3, 1/2, 1/1
			BLENDFUNCTION blend = { AC_SRC_OVER, 0, BYTE(255 * (float(m_nOpacity) / 100)) , 0 };

			// Copy the offscreen bitmap onto the screen.
			m_dc.AlphaBlend(m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
							m_hDC, m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
							blend);
		}
		else 
		{
			// Copy the offscreen bitmap onto the screen.
			m_dc.BitBlt(m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
						m_hDC, m_rc.left, m_rc.top, SRCCOPY);
		}
	}

private:
	int				m_nOpacity;
	CDCHandle		m_dc;          
	CBitmap			m_bitmap;      
	CBitmapHandle	m_hOldBitmap;  
	RECT			m_rc;   
	CPoint			m_ptOldWindowOrg;
};

typedef CMemDCT<false>   CMemDCHandle;
typedef CMemDCT<true>    CMemDC;