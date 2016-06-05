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
 
#include "StdAfx.h"
#include ".\windowcaption.h"

#include "image/Image.h"
#include "image/ImageBuilder.h"


CWindowCaption::CWindowCaption(void) : 
	m_xFirstCaptionButtonPosition(0), 
	m_nRoundCaptionFactor(0), 
	m_pWindowWithRegion(NULL), 
	m_pCurrentImageCaption(NULL),
	m_pCurrentFrameSwitcher(NULL),
	m_bDrawTransparentImage(FALSE),
	m_bShowText(TRUE),
	m_bActive(FALSE)
{
}

CWindowCaption::~CWindowCaption(void)
{
}


void CWindowCaption::SetText(const CString& strText)
{
	m_strText = strText;

	if (IsWindow())
	{
		Invalidate();
		UpdateWindow();
	}
}

CString CWindowCaption::GetText() const
{
	return m_strText;
}


void CWindowCaption::SetActiveImage(IUOLMessengerImagePtr pImage)
{
	m_pImageActiveCaption = pImage;

	if (m_bActive)
	{
		m_pCurrentImageCaption = m_pImageActiveCaption.get();
	}

	if (IsWindow())
	{
		Invalidate();
	}
}


IUOLMessengerImagePtr CWindowCaption::GetActiveImage()
{
	return m_pImageActiveCaption;
}


void CWindowCaption::SetInactiveImage(IUOLMessengerImagePtr pImage)
{
	m_pImageInactiveCaption = pImage;

	if (!m_bActive)
	{
		m_pCurrentImageCaption = m_pImageInactiveCaption.get();
	}

	if (IsWindow())
	{
		Invalidate();
	}
}


IUOLMessengerImagePtr CWindowCaption::GetInactiveImage()
{
	return m_pImageInactiveCaption;
}


void CWindowCaption::SetDrawTransparentImage(BOOL bDrawTransparent)
{
	m_bDrawTransparentImage = bDrawTransparent;
}


BOOL CWindowCaption::GetDrawTransparentImage() const
{
	return m_bDrawTransparentImage;
}


CToolTipElement CWindowCaption::GetSelectedControl(CPoint& pos)
{
	if (IsPointInSwitcher(pos))
	{
        return m_toolTipSwitcher;
	}

	return CToolTipElement(_T(""));
}

void CWindowCaption::SetRoundCaptionFactor(int nRoundCaptionFactor)
{
	m_nRoundCaptionFactor = nRoundCaptionFactor;
}

void CWindowCaption::SetWindowWithRegion(IWindowWithRegion* pWindowWithRegion)
{
	m_pWindowWithRegion = pWindowWithRegion;
}

//CIconElementPtr CWindowCaption::GetSmallIcon() const
//{
//	return m_pIconSmall;
//}
//
//CIconElementPtr CWindowCaption::GetLargeIcon() const
//{
//	return m_pIconLarge;
//}

LRESULT CWindowCaption::OnNcHitTest(const CPoint& pt)
{
	CPoint ptClient = pt;
	ScreenToClient(&ptClient);

	if (IsPointInSwitcher(ptClient))
	{
		return HTCLIENT;
	}

	if (GetImageButton(ptClient, CImageButton::MouseOver).get() == NULL)
	{
		return HTTRANSPARENT;
	}

	return HTCLIENT;
}

void CWindowCaption::OnLButtonDown(UINT, const CPoint& pt)
{
	if (IsPointInSwitcher(pt))
	{	
		CPoint ptScreen = pt;
		ClientToScreen(&ptScreen);

		if (m_pWindowWithRegion)
		{
			m_pWindowWithRegion->EnableRgn(!m_pWindowWithRegion->IsRgnEnabled());
			Invalidate(FALSE);
		}
	}

	SetMsgHandled(FALSE);
}

void CWindowCaption::OnLButtonDblClk(UINT, const CPoint& pt)
{
	SetMsgHandled(FALSE);
}

void CWindowCaption::OnRButtonUp(UINT, const CPoint& pt)
{
	SetMsgHandled(FALSE);
}

LRESULT CWindowCaption::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	return 0L;
}

void CWindowCaption::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "font");

		m_pFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFont.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "font");
		
		// use default system font
		m_pFont = new CFontElement();		
	}

	
	try
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "image");

		m_pImageActiveCaption = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pImageActiveCaption.get());
		
		m_pCurrentImageCaption = m_pImageActiveCaption.get();
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "image");
	}

	try
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "inactive_image");

		m_pImageInactiveCaption = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pImageInactiveCaption.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "inactive_image");
		m_pImageInactiveCaption = m_pImageActiveCaption;
	}	

	try
	{
		CString strResizable;
		GetParameterValue(strResizable, pMapParams, "transparentimage");

		m_bDrawTransparentImage = (0 == strResizable.CompareNoCase("true") || ('1' == strResizable));	
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "transparentimage");
		m_bDrawTransparentImage = FALSE;
	}

	try
	{		
		CString strFrameSwitcher;
		GetParameterValue(strFrameSwitcher, pMapParams, "frameSwitcher");

		m_pActiveFrameSwitcher = UOLMessengerImage::CImageBuilder::LoadImage(strFrameSwitcher);
		ATLASSERT(m_pActiveFrameSwitcher.get());	

		m_pCurrentFrameSwitcher = m_pActiveFrameSwitcher.get();
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "frameSwitcher");
	}

	try
	{
		CString strInactiveFrameSwitcher;
		GetParameterValue(strInactiveFrameSwitcher, pMapParams, "inactiveframeswitcher");

		m_pInactiveFrameSwitcher = UOLMessengerImage::CImageBuilder::LoadImage(strInactiveFrameSwitcher);
		ATLASSERT(m_pInactiveFrameSwitcher.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "inactiveframeswitcher");
		m_pInactiveFrameSwitcher = m_pActiveFrameSwitcher;
	}

	try
	{
		CString strSwitcherTip;

		GetParameterValue(strSwitcherTip, pMapParams, "SwitcherTip");

		m_toolTipSwitcher.SetToolTipText(strSwitcherTip);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SwitcherTip");
	}

	try
	{
		CString strResizable;
		GetParameterValue(strResizable, pMapParams, "showtext");

		m_bShowText = (0 == strResizable.CompareNoCase("true") || ('1' == strResizable));	
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "showtext");
		m_bShowText = TRUE;
	}

	//try
	//{
	//	CString strIconSmall;
	//	GetParameterValue(strIconSmall, pMapParams, "smallicon");

	//	m_pIconSmall = ap_dynamic_cast<CIconElementPtr>(pElementTable->GetElement(strIconSmall));		
	//}
	//catch(const CSkinException&)
	//{
	//	m_pIconSmall = CIconElementPtr();
	//}

	//try
	//{
	//	CString strIconLarge;
	//	GetParameterValue(strIconLarge, pMapParams, "largeicon");

	//	m_pIconLarge = ap_dynamic_cast<CIconElementPtr>(pElementTable->GetElement(strIconLarge));
	//}
	//catch(const CSkinException&)
	//{
	//	m_pIconLarge = CIconElementPtr();
	//}
}

void CWindowCaption::DrawBackground(CDCHandle& dc, CRect& rect)
{
	__super::DrawBackground(dc, rect);
}

void CWindowCaption::DoPaint(CDCHandle& dc)
{
	if (IsRgnEnabled())
	{
		__super::DoPaint(dc);
	}	

	CRect rect;
	GetClientRect(&rect);

	int x = rect.left + START_FRAME_X;
	int cx = GetFirstButtonPosition();
	
	if (m_pCurrentFrameSwitcher)
	{
		int _cx = m_pCurrentFrameSwitcher->GetWidth();
		int _cy = m_pCurrentFrameSwitcher->GetHeight();
		
		IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();

		m_pCurrentFrameSwitcher->Draw(dc, 
							   x, (rect.Height() - _cy) / 2, 
							   _cx, _cy, 
							   pStrategy);


		x += _cx + 5;
		cx -= _cx;
	}

	if (m_pCurrentImageCaption)
	{		
		int _cy = m_pCurrentImageCaption->GetHeight();

		IUOLMessengerImageDrawStrategyPtr pStrategy;

		if (m_bDrawTransparentImage)
		{
			pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();			
		}
		else
		{
			pStrategy = new UOLMessengerImage::CDrawOpaqueStrategy();
		}

		m_pCurrentImageCaption->Draw(dc, 
							  x, (rect.Height() - _cy) / 2, 
							  m_pCurrentImageCaption->GetWidth(), m_pCurrentImageCaption->GetHeight(), 
							  pStrategy);

		x += m_pCurrentImageCaption->GetWidth() + 3;
	}

	if (m_bShowText && (m_pFont.get()))
	{
		int xButton = GetFirstButtonPosition();

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
		{
			_CWindowCaptionBase::CAddElementHelperPtr pHelper = ap_static_cast<_CWindowCaptionBase::CAddElementHelperPtr>(m_listChilds.GetNext(pos));
			CImageButtonBasePtr	pButton = pHelper->m_pButton;
			ATLASSERT(pButton != NULL);		

			if (pButton.get())
			{
				xButton = GetNextButtonPosition(xButton, pButton);
			}
		}

		rect.left = x + 3;
		rect.right = xButton;

		m_pFont->DrawText(dc, rect, m_strText);
	}	
}

void CWindowCaption::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);

	__super::UpdateLayout(bResizeBars);	

	CRgnHandle rgn = GetClippingRgn(rect);

	if (rgn)
	{
		SetWindowRgn(rgn, TRUE);
	}
}

BOOL CWindowCaption::HitTest(const CPoint& pt, int& nTest) const
{
	nTest = HTCAPTION; 

	return TRUE;
}

CRgnHandle CWindowCaption::GetClippingRgn(const CRect& rectWindow) const
{
	CRgnHandle hClippingRgn;

	if (m_nRoundCaptionFactor)
	{
		CRgn hBigRgn;
		hBigRgn.CreateRoundRectRgn(rectWindow.left, 
								rectWindow.top, 
								rectWindow.left + rectWindow.Width() + 1,
								rectWindow.top + rectWindow.Height() + m_nRoundCaptionFactor, 
								m_nRoundCaptionFactor, m_nRoundCaptionFactor);	

		
		hClippingRgn.CreateRectRgn(rectWindow.left, 
								rectWindow.top, 
								rectWindow.left + rectWindow.Width() + 1, 
								rectWindow.top + rectWindow.Height());

		hClippingRgn.CombineRgn(hBigRgn, RGN_AND);
	}	

	return hClippingRgn;
}

int CWindowCaption::GetFirstButtonPosition()
{
	if (FALSE == IsRgnEnabled())
	{
		return INT_MAX;
	}

	CRect rect;
	GetClientRect(&rect);

	return rect.Width() - m_nSpacement;
}

int CWindowCaption::GetNextButtonPosition(int x, CImageButtonBasePtr& pButton)
{
	m_xFirstCaptionButtonPosition = x - (2 * m_nSpacement + pButton->GetWidth());

	return m_xFirstCaptionButtonPosition;
}

int CWindowCaption::GetButtonOffset(CImageButtonBasePtr& pButton)
{
	return -pButton->GetWidth();
}

BOOL CWindowCaption::OnActivate(BOOL bActivate)
{
	m_bActive = bActivate;

	m_pCurrentImageCaption = (bActivate) ? m_pImageActiveCaption.get() : m_pImageInactiveCaption.get();
	m_pCurrentFrameSwitcher = (bActivate) ? m_pActiveFrameSwitcher.get() : m_pInactiveFrameSwitcher.get();

	return __super::OnActivate(bActivate);
}

BOOL CWindowCaption::IsPointInSwitcher(const CPoint& pt) const
{
	if (m_pCurrentFrameSwitcher)
	{
		CRect rect;
		GetClientRect(&rect);

		CSize size(m_pCurrentFrameSwitcher->GetWidth(), m_pCurrentFrameSwitcher->GetHeight());

		int x = rect.left + START_FRAME_X;
		int y = (rect.Height() - size.cy) / 2;	
		
		return CRect(CPoint(x, y), size).PtInRect(pt);
	}

	return FALSE;
}

BOOL CWindowCaption::IsRgnEnabled() const
{
	return (m_pWindowWithRegion) && (m_pWindowWithRegion->IsRgnEnabled());
}