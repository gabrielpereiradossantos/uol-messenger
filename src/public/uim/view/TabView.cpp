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
 
// TabView.cpp : implementation of the CTabView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tabview.h"
#include "../controller/UIMApplication.h"

#include "skin/BitmapBackgroundElement.h"


#define TAB_CHECK_TIMER 1234

#define		BUTTON_IMAGE_TEXT_SPACE				5


//////////////////////////////////////////////////////////////////////////
// CWidget
//////////////////////////////////////////////////////////////////////////


CWidget::CWidget(CTabViewButtonElementPtr& pTabViewButton,
		IUOLMessengerImagePtr& pCloseTabImage,
		CFontElementPtr& pFont,
		CFontElementPtr& pFontSelected, 
		int nWidgetIndex,
		CTabView* pTabView) :
	m_pTabViewButton(pTabViewButton),
	m_pCloseTabImage(pCloseTabImage),
	m_pFont(pFont),
	m_pFontSelected(pFontSelected),
	m_nWidgetIndex(nWidgetIndex), 
	m_nTabWidth(0), 
	m_bTabChanged(FALSE), 
	m_widgetTextSize(0, 0)
{
	m_state=Normal;
	m_caption="";
	m_rectText=CRect(0,0,0,0);
	m_rectClose=CRect(0,0,0,0);
	m_rectWidget=CRect(0,0,0,0);
	m_rectBtnImage=CRect(0, 0, 0, 0);
	m_pStrategy = new UOLMessengerImage::CPatternStrategy();
	m_pButtonTabImage = NULL;
	
	if (pTabView)
	{
		m_showCloseButton = pTabView->GetShowCloseButton();
		
		pTabView->GetTabButtonMargins(m_buttonLeftMargin, 
			m_buttonTopMargin, m_buttonRightMargin, m_buttonBottomMargin);
	}
	else
	{
		// Considering here, the MessageWindow conditions.
		m_showCloseButton = TRUE;
		
		m_buttonLeftMargin = 5;		// Largura da imagem da esquerda.
		m_buttonTopMargin = -2;
		m_buttonRightMargin = 6;	// Largura da imagem da direita + 2.
		m_buttonBottomMargin = 0;
	}
}


void CWidget::DrawIt(	CDCHandle * dc,
					 CImage* pLeftImage,
					 IUOLMessengerImagePtr& pFillImage,
					 CImage* pRightImage)
{
	int nl, nr, nf;
	nl = pLeftImage->GetWidth();
	nr = pRightImage->GetWidth();
	nf = m_rectWidget.Width() - nl - nr;

	dc->BitBlt(
		m_rectWidget.left, 
		m_rectWidget.top, 
		nl, 
		m_rectWidget.Height(),  
		pLeftImage->GetDC(),0,0,SRCCOPY);

	pFillImage->Draw(
		*dc,
		m_rectWidget.left+nl, 
		m_rectWidget.top,
		nf,
		m_rectWidget.Height(),
		m_pStrategy);

	dc->BitBlt(
		m_rectWidget.left+nl+nf, 
		m_rectWidget.top, 
		nr, 
		m_rectWidget.Height(),
		pRightImage->GetDC(),0,0,SRCCOPY);

	pLeftImage->ReleaseDC();
	pRightImage->ReleaseDC();

}


//void CWidget::DrawMouseOver(CDCHandle * dc)
//{
////	m_pTabViewButton->GetLeftImage()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);
//	IUOLMessengerImagePtr leftImage;
//	IUOLMessengerImagePtr fillImage;
//	IUOLMessengerImagePtr rightImage;
//
//	leftImage = m_pTabViewButton->GetLeftImage()->GetOver();
//	fillImage = m_pTabViewButton->GetFillImage()->GetOver();
//	rightImage= m_pTabViewButton->GetRightImage()->GetOver();
//
//
//
//
//}

//void CWidget::DrawBlinking(CDCHandle * dc)
//{
//	//m_pTabViewButton->GetLeftImage()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);	
//	m_pTabViewButton->GetLeftImage()->GetBlinking()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);
//}
//
//void CWidget::DrawCliked(CDCHandle * dc) 
//{
//	//m_pTabViewButton->GetLeftImage()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);	
//	m_pTabViewButton->GetLeftImage()->GetClicked()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);
//}
//
//void CWidget::DrawNormal(CDCHandle * dc)
//{
//	//m_pTabViewButton->GetLeftImage()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);	
//	m_pTabViewButton->GetLeftImage()->GetNormal()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);
//}
//
//void CWidget::DrawSelected(CDCHandle * dc)
//{
//	//m_pTabViewButton->GetLeftImage()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);	
//	m_pTabViewButton->GetLeftImage()->GetSelected()->Draw(*dc, m_rectWidget.left, m_rectWidget.top, m_rectWidget.Width(), m_rectWidget.Height(), m_pStrategy);
//}

void CWidget::SetRect(const CRect &rect) 
{ 
	m_rectWidget = rect;
	
	if ( (m_pCloseTabImage) && (m_showCloseButton) )
	{
		m_rectClose.top = ( (m_rectWidget.Height() - m_pCloseTabImage->GetHeight()) / 2) 
				 + m_buttonTopMargin;
		m_rectClose.right = m_rectWidget.right - m_buttonRightMargin;
		m_rectClose.bottom = m_rectClose.top + m_pCloseTabImage->GetHeight();
		m_rectClose.left = m_rectClose.right - m_pCloseTabImage->GetWidth();
	}
	
	/*
	m_rectText.left = m_rectWidget.left + 2;
	m_rectText.top = m_rectWidget.top + 2;
	m_rectText.right = m_rectWidget.right - 10;
	m_rectText.bottom = m_rectWidget.bottom - 1;
	
	m_rectClose.top = m_rectWidget.top - 1;
	m_rectClose.right = m_rectWidget.right - 2;
	m_rectClose.bottom = m_rectClose.top + 18;
	m_rectClose.left = m_rectClose.right - 11;
	*/
}

void CWidget::DrawWidget(CDCHandle * dc, BOOL selected, BOOL blinking/*, BOOL close*/)
{
	ATLASSERT(dc!=NULL);

	ATL::CImage* pLeftImage = NULL;
	IUOLMessengerImagePtr pFillImage = NULL;
	ATL::CImage* pRightImage = NULL;

	if (selected)
	{
		
			pLeftImage = m_pTabViewButton->GetLeftImage()->GetSelected();
			pFillImage = m_pTabViewButton->GetFillImage()->GetSelected();
			pRightImage = m_pTabViewButton->GetRightImage()->GetSelected();
	} 
	else 
	{
		switch (m_state) 
		{
		case  Selected:
			
				pLeftImage = m_pTabViewButton->GetLeftImage()->GetSelected();
				pFillImage = m_pTabViewButton->GetFillImage()->GetSelected();
				pRightImage = m_pTabViewButton->GetRightImage()->GetSelected();

			break;
		case  Normal:
			
				pLeftImage = m_pTabViewButton->GetLeftImage()->GetNormal();					
				pFillImage = m_pTabViewButton->GetFillImage()->GetNormal();
				pRightImage = m_pTabViewButton->GetRightImage()->GetNormal();

			break;
		case MouseClicked:
			
				pLeftImage = m_pTabViewButton->GetLeftImage()->GetClicked();
				pFillImage = m_pTabViewButton->GetFillImage()->GetClicked();
				pRightImage = m_pTabViewButton->GetRightImage()->GetClicked();

			break;
		case MouseOver:
			
					pLeftImage = m_pTabViewButton->GetLeftImage()->GetOver();
					pFillImage = m_pTabViewButton->GetFillImage()->GetOver();
					pRightImage = m_pTabViewButton->GetRightImage()->GetOver();

			break;
		case Blinking:
			if (blinking)
			{
				
					pLeftImage = m_pTabViewButton->GetLeftImage()->GetBlinking();
					pFillImage = m_pTabViewButton->GetFillImage()->GetBlinking();
					pRightImage = m_pTabViewButton->GetRightImage()->GetBlinking();

			}
			else
			{
				
					pLeftImage = m_pTabViewButton->GetLeftImage()->GetNormal();
					pFillImage = m_pTabViewButton->GetFillImage()->GetNormal();
					pRightImage = m_pTabViewButton->GetRightImage()->GetNormal();

			}

			break;
		}
	}

	DrawIt(dc, pLeftImage, pFillImage, pRightImage);

	IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();
	
	CFontElementPtr pFont;
	BOOL selectedFont = FALSE;
	if ((selected) || (Selected == m_state))
	{
		pFont = m_pFontSelected;
		selectedFont = TRUE;
	}
	else
	{
		pFont = m_pFont;
	}
	
	int textLeftShift = 0;
	int textRightShift = 0;
	int textAvailableHeight = 0;
	
	if (m_pButtonTabImage.get())
	{
		m_rectBtnImage.left = m_rectWidget.left + m_buttonLeftMargin;
		m_rectBtnImage.top = (m_rectWidget.Height() - m_pButtonTabImage->GetHeight()) / 2;
		m_rectBtnImage.right = m_rectBtnImage.left + m_pButtonTabImage->GetWidth();
		m_rectBtnImage.bottom = m_rectBtnImage.top + m_pButtonTabImage->GetHeight();
		
		m_pButtonTabImage->Draw(*dc, m_rectBtnImage.left, 
								m_rectBtnImage.top, 
								m_rectBtnImage.Width(), 
								m_rectBtnImage.Height(), 
								pStrategy);
		
		textLeftShift = m_rectBtnImage.Width() + BUTTON_IMAGE_TEXT_SPACE;
	}
	
	if ( (m_pCloseTabImage) && (m_showCloseButton) )
	{
		textRightShift = m_rectClose.Width();
	}
	
	// The height available for text is the size of the widget minus the top 
	// and bottom margins.
	textAvailableHeight = m_rectWidget.Height() - (m_buttonTopMargin + m_buttonBottomMargin);
	
	CString tmpString(m_caption);
	
	CSize textSize(0, 0);
	GetWidgetTextSize(dc->m_hDC, textSize, selectedFont);
	
	m_rectText.left = m_rectWidget.left + m_buttonLeftMargin + textLeftShift;
	if ( textAvailableHeight < textSize.cy)
	{
		m_rectText.top = (m_rectWidget.Height() - textSize.cy) / 2;
	}
	else
	{
		m_rectText.top = m_buttonTopMargin + ( (textAvailableHeight - textSize.cy) / 2 );
	}
	m_rectText.right = m_rectWidget.right - m_buttonRightMargin - textRightShift;
	m_rectText.bottom = m_rectText.top + textSize.cy;
	
	// draw the contact name
	pFont->DrawText(*dc, m_rectText, tmpString, (UINT) DT_VCENTER | DT_END_ELLIPSIS);

	///if (close)
	// draw the close butom image
	if ( (m_pCloseTabImage) && (m_showCloseButton) )
	{
		m_pCloseTabImage->Draw(*dc, m_rectClose.left, 
								m_rectClose.top + 1, 
								m_rectClose.Width(),
								m_rectClose.Height(), 
								pStrategy);
	}
}


int CWidget::PtInWidget(const CPoint &point )
{
	if ( (m_showCloseButton) && (m_rectClose.PtInRect(point)) )
	{
		return CWidget::clickClose;
	}
	else if (m_rectWidget.PtInRect(point))
	{
		return CWidget::clickWidget;
	}
	
	return CWidget::clickNone;
}


int CWidget::GetWidgetIndex()
{
	return m_nWidgetIndex;
}


void CWidget::SetTabButtonImage(IUOLMessengerImagePtr&  pButtonTabImage)
{
	m_pButtonTabImage = pButtonTabImage;
	
	m_bTabChanged = TRUE;
}


void CWidget::GetWidgetTextSize(HDC hDC, CSize& size, BOOL selected)
{
	HDC compatibleDC = ::CreateCompatibleDC(hDC);
	CFont localFont;
	
	CFontElementPtr pFont;
	if (selected)
	{
		pFont = m_pFontSelected;
	}
	else
	{
		pFont = m_pFont;
	}
	
	localFont.CreateFontIndirect(pFont->GetFont().get());
	
	::SelectObject(compatibleDC, localFont);
	
	::GetTextExtentPoint32(compatibleDC, m_caption, m_caption.GetLength(), &size);
	
	localFont.DeleteObject();
	::DeleteDC(compatibleDC);
}


int CWidget::CalculateTabWidth(HDC windowDC)
{
	CSize textSize(0, 0);
	CFontElementPtr pFont;
	
	if ( (m_bTabChanged) || (m_nTabWidth == 0) )
	{
		m_nTabWidth = m_buttonLeftMargin + m_buttonRightMargin;
		
		//ATLTRACE(_T("CWidget::CalculateTabWidth() - m_nTabWidth with margins = %d\n"), m_nTabWidth);
		
		if (m_pButtonTabImage.get())
		{
			m_nTabWidth += m_pButtonTabImage->GetWidth() + BUTTON_IMAGE_TEXT_SPACE;
			
			//ATLTRACE(_T("CWidget::CalculateTabWidth() - m_nTabWidth + buttonImage = %d\n"), m_nTabWidth);
		}
		
		if ( (m_pCloseTabImage) && (m_showCloseButton) )
		{
			m_nTabWidth += m_pCloseTabImage->GetWidth();
			
			//ATLTRACE(_T("CWidget::CalculateTabWidth() - m_nTabWidth + closeImage = %d\n"), m_nTabWidth);
		}
		
		if (m_state == Selected)
		{
			pFont = m_pFontSelected;
		}
		else
		{
			pFont = m_pFont;
		}
		
		CString tmpString(m_caption);
		GetWidgetTextSize(windowDC, textSize, (m_state == Selected));
		
		m_nTabWidth += textSize.cx;
		
		//ATLTRACE(_T("CWidget::CalculateTabWidth() - textSize.cx = %d and textSize.cy = %d\n"), 
		//		textSize.cx, textSize.cy);
		
		//ATLTRACE(_T("CWidget::CalculateTabWidth() - m_nTabWidth + textWidth of [%s] = %d\n"), 
		//		tmpString, m_nTabWidth);
		
		m_bTabChanged = FALSE;
	}
	
	return m_nTabWidth;
}


void CWidget::SetTabChanged(BOOL bTabChanged)
{
    m_bTabChanged = bTabChanged;
}



//////////////////////////////////////////////////////////////////////////
// CTabView
//////////////////////////////////////////////////////////////////////////

CTabView::CTabView() 
{
	m_current = -1; 
	SetMaxTabsLines(3);
	SetMaxTabs(5);
	m_minWidth = 50;
	m_lastOver = NULL;
	m_lastClick = NULL;
	m_blinkingItems = 0;
	m_blinking = TRUE;
	m_timer = NULL;
//	m_rectClose = CRect(0,0,0,0);
	
	m_tabViewType = IUOLMessengerTabViewCtrl::TAB_VIEW_MESSAGE_WINDOW;
	m_showCloseButton = TRUE;
	m_buttonLeftMargin = 0;
	m_buttonTopMargin = 0;
	m_buttonRightMargin = 0;
	m_buttonBottomMargin = 0;
	m_buttonSpace = 0;
	m_nTotalTabsWidth = 0;
	m_bTabsChanged = FALSE;
	m_bAutoCalculateWidths = FALSE;
	m_bHover = FALSE;
}


CTabView::~CTabView() 
{
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
	{
		CWidget * ptr = vWidgets.GetAt(pos);
		ATLASSERT(ptr!=NULL);
		delete ptr;
	}
	vWidgets.RemoveAll();
}

void CTabView::OnDestroy(void)
{
	if (m_timer!=NULL) {
		KillTimer(TAB_CHECK_TIMER);
		m_timer = NULL;
	}
}


void CTabView::SetActiveConversation(IUOLMessengerConversationPtr pConversation)
{
	ATLASSERT(pConversation!=NULL);
	int i=0;
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos),i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		if (tmp->m_pConversation==pConversation) {
			m_current = i;
			tmp->m_state = CWidget::Selected;
		}
		else if (tmp->m_state == CWidget::Selected)
		{
			tmp->m_state = CWidget::Normal;
		}
	}
}

void CTabView::SetActiveTab(int nIndex)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if ((nIndex >= 0) && (nIndex < ((int)vWidgets.GetCount())))
	{
		m_current = nIndex;

		if (IsWindow())
		{
			InvalidateRect(NULL);
		}
	}
}


int CTabView::GetTabCount(CWidget::tabState state)
{
	int nCount = 0;
	
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; )
	{
		CWidget * tmp = vWidgets.GetNext(pos);

		if (tmp->m_state == state)
		{
			nCount++;
		}
	}

	return nCount;
}



BOOL CTabView::AddTab(IUOLMessengerConversationPtr pConversation,
					  const CString& strCaption,
					  CWidget::tabState state, 
					  BOOL bInformTabView)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if (m_tabViewType == IUOLMessengerTabViewCtrl::TAB_VIEW_MESSAGE_WINDOW)
	{
		ATLASSERT(pConversation!=NULL);
	}
	CString caption;
	caption = strCaption;
	int len = vWidgets.GetCount();
	if (len>=m_maxTabs)
		return FALSE;
	
	CWidget * tmp = NULL;
	
	if (bInformTabView)
	{
		tmp = new CWidget(m_pTabButton,
					  m_pCloseTabImage,
					  m_pFont,
					  m_pFontSelected,
					  len,
					  this);
	}
	else
	{
		tmp = new CWidget(m_pTabButton,
					  m_pCloseTabImage,
					  m_pFont,
					  m_pFontSelected,
					  len,
					  NULL);
	}
	
	if ((state == CWidget::Blinking) && (len>0)) {
		tmp->m_state = state;
		m_blinkingItems++;
		if (m_timer==NULL)
			m_timer = SetTimer(TAB_CHECK_TIMER, 500);	
	} else {
		tmp->m_state = CWidget::Normal;
	}
	tmp->m_pConversation=pConversation;
	tmp->m_caption=caption;
	vWidgets.AddTail(tmp);
	return TRUE;
}

BOOL CTabView::RemoveTab(IUOLMessengerConversationPtr pConversation)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	ATLASSERT(pConversation);
	int i=0;
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos),i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		if (tmp->m_pConversation==pConversation) {
			if (m_current==i) {
				m_current=0;
			}
			if (m_lastClick==tmp) {
				m_lastClick = NULL;
			}
			if (m_lastOver==tmp) {
				m_lastOver = NULL;
			}
			delete tmp;
			vWidgets.RemoveAt(pos);
			break;
		}
	}
	return FALSE;
}

void CTabView::RecalcTabs(CDCHandle & dc) 
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	RECT rect;
	CRect tmpRect(0, 0, 0, 0);
	CSize size;
	BOOL tabWidthsAutoCalculated = FALSE;
	
	GetClientRect(&rect);
		
	if (m_bAutoCalculateWidths)
	{
		m_bTabsChanged = TRUE;
        int tabsTotalWidth = CalculateTabsTotalWidth();
		int tabWidth = 0;
		CRect widgetRect;
		int xCoord = 2;
        int tabCount = vWidgets.GetCount();
        float overSize = 0;
        if (tabsTotalWidth > (rect.right - rect.left))
		{
            overSize = 1 - (((float)rect.right - (float)rect.left) / (float)tabsTotalWidth);
        }
		
        HDC hWndDC = GetDC();

        for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
        {
            CWidget* pWidget = vWidgets.GetAt(pos);

            tabWidth = (pWidget->CalculateTabWidth(hWndDC));

            float percent = ((float)tabWidth * overSize);
            tabWidth = static_cast<int>(tabWidth - percent);

            //ATLTRACE(_T("CTabView::RecalcTabs() - tabWidth = %d\n"), tabWidth);

            widgetRect.top = 0;
            widgetRect.bottom = m_pNormalImage->GetHeight();
            widgetRect.left = xCoord;
            widgetRect.right = widgetRect.left + tabWidth;

            pWidget->SetRect(widgetRect);

            xCoord += tabWidth + m_buttonSpace;
        }

        ReleaseDC(hWndDC);

        tabWidthsAutoCalculated = TRUE;
	}
	
	if (!tabWidthsAutoCalculated)
	{
		int lx = rect.right - 16;
		//int ly=rect.bottom;
		int lastx = 2;
		int len;
		//int acum=0;
		int fixedwidth = 0;
		len = vWidgets.GetCount();

		//m_rectClose.right  = rect.right - 1;
		//m_rectClose.top	   = rect.top+1;
		//m_rectClose.left   = m_rectClose.right - 8;
		//m_rectClose.bottom = m_rectClose.top+8;

		if (len > 0)
		{
			int needWidthPixel;

			//// Calc the min pixel to all tabs width
			//for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
			//{
			//	CWidget * tmp = vWidgets.GetAt(pos);
			//	dc.GetTextExtent(tmp->m_caption,tmp->m_caption.GetLength(),&size);
			//	size.cx += 12;
			//	acum+= (size.cx+BORDER);
			//}

			fixedwidth = lx / len; 
			
			if (fixedwidth > MAX_WIDHT)
			{
				fixedwidth = MAX_WIDHT;
			}

			for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
			{
				CWidget * tmp = vWidgets.GetAt(pos);
				
				needWidthPixel = fixedwidth;
				
				tmpRect.top = 0;
				tmpRect.bottom = m_pNormalImage->GetHeight();
				
				tmpRect.left = lastx;
				
				tmpRect.right = lastx + needWidthPixel;
				lastx = tmpRect.right + m_buttonSpace;
				
				tmp->SetRect(tmpRect);
			}
		}
	}
}

BOOL CTabView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CTabView::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if ((m_lastOver!=NULL) && (m_lastOver->m_state != CWidget::Blinking)) {
		m_lastOver->SetState(CWidget::Normal);
		m_lastOver=NULL;
		Invalidate(FALSE);
	}
	bHandled = FALSE;
	NotifyAll(&IUOLMessengerTabViewCtrlObserver2::OnConversationTabMouseLeave);
	m_bHover = FALSE;
	return 0;
}

LRESULT CTabView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	CWidget * tmp=NULL;
	CPoint m_StartPoint;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 

	// Find the widget with click
	POSITION pos;

	for (pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		// Click Item
		CWidget::clickItem itemclicked = (CWidget::clickItem) tmp->PtInWidget(m_StartPoint);
		if (itemclicked != CWidget::clickNone) 
		{
			TRACKMOUSEEVENT mouseevent;
			mouseevent.cbSize=sizeof(TRACKMOUSEEVENT);
			mouseevent.dwFlags=TME_LEAVE|TME_HOVER;
			mouseevent.dwHoverTime=400;
			mouseevent.hwndTrack=m_hWnd;
			TrackMouseEvent(&mouseevent);
			
			if (tmp!=m_lastOver) 
			{
				if (tmp->m_state != CWidget::Blinking)
				{
					if (m_lastOver) 
					{
						if (m_lastOver->m_state != CWidget::Blinking)
						{				
							m_lastOver->SetState(CWidget::Normal);
						}
					}
					tmp->SetState(CWidget::MouseOver);
				}
				m_lastOver = tmp;
				
				//Call MouseLeave to destroy tooltip window
				NotifyAll(&IUOLMessengerTabViewCtrlObserver2::OnConversationTabMouseLeave);
				m_bHover = FALSE;
			} 			

			if (tmp->m_state==CWidget::Blinking) 
				break;

			if (tmp==m_lastClick) {
				if (tmp!=m_lastOver)  { // Redraw
					if (m_lastOver) {
						m_lastOver->SetState(CWidget::Normal);
					}
					tmp->SetState(CWidget::MouseClicked);
					m_lastOver = tmp;
				}
			}
			break;
		}
	}

	if (pos==NULL){
		if (m_lastOver) {
			m_lastOver->SetState(CWidget::Normal);
			m_lastOver=NULL;
		}
	}
	Invalidate(FALSE);
	bHandled = FALSE;
	return 0;
}

LRESULT CTabView::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CWidget * tmp=NULL;
	CPoint m_StartPoint;
	int i;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 

	// Find the widget with click
	i =0;
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos), i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		CWidget::clickItem itemclicked = (CWidget::clickItem) tmp->PtInWidget(m_StartPoint);
		
		if (itemclicked==CWidget::clickWidget)
		{
			tmp->SetState(CWidget::MouseClicked);
			Invalidate(FALSE);
			m_lastClick = tmp;
			break;
		}
		else if (itemclicked==CWidget::clickClose)
		{
			CWidget * tmpClose=NULL;
			tmpClose = vWidgets.GetAt(vWidgets.FindIndex(i));

			BOOL bCanClose = TRUE;
			
			if (IUOLMessengerTabViewCtrl::TAB_VIEW_MESSAGE_WINDOW == m_tabViewType)
			{
				IUOLMessengerConversationPtr pConversation;
				IUOLMessengerMessageWindowManagerPtr pConversationManager =
						CUIMApplication::GetApplication()->GetUIManager()->GetMessageWindowManager();
				IUOLMessengerMessageWindowManager2Ptr pConversationManager2 = 
					ap_dynamic_cast<IUOLMessengerMessageWindowManager2Ptr>(pConversationManager);

				IUOLMessengerMessageWindowPtr pMessageWindow;

				IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(tmpClose->m_pConversation);
				if ((pConversation2 != NULL) && 
					(pConversation2->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION))
				{
					pMessageWindow = pConversationManager2->FindConversationMessageWindow(
						pConversation2->GetChatKey(), pConversation);

					// If conversation is chat alert closing window
					IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

					if ((pConversation2->GetChatParticipantCount() > 2) &&
						(TRUE == pAccountConnectionManager->IsConnected(pConversation->GetAccount())))
					{
						UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
						ATLASSERT(pStringLoader);

						CString strMessage = pStringLoader->GetValue("uol.resource.IDS_CHAT_CONFIRM_CLOSE_WND");
						bCanClose = (CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(GetParent(), strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES);
					}
				}
				else
				{

					pMessageWindow = pConversationManager->FindConversationMessageWindow(
						tmpClose->m_pConversation->GetAccount(),
						tmpClose->m_pConversation->GetContactName(), pConversation);
				}
				ATLASSERT(pMessageWindow != NULL);
				if (pMessageWindow != NULL)
				{
					if (bCanClose)
                    {
                        pMessageWindow->RemoveConversation(i);
                    }
                    else
                    {
                        break;
                    }
                }
			}

			if (m_current > i)
			{
				m_current--;
			}
			
			break;
		}
	}
	//Call MouseLeave to destroy tooltip window
	NotifyAll(&IUOLMessengerTabViewCtrlObserver2::OnConversationTabMouseLeave);
	m_bHover = FALSE;
	
	bHandled = FALSE;	
	
	return 0;
}

LRESULT CTabView::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bHover)
	{
		Invalidate(FALSE);
		return 0;
	}

	m_bHover=TRUE;
	
	CWidget * tmp=NULL;
	CPoint m_StartPoint;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 

	// Find the widget with click
	POSITION pos;

	for (pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		// Click Item
		CWidget::clickItem itemclicked = (CWidget::clickItem) tmp->PtInWidget(m_StartPoint);
		if (itemclicked != CWidget::clickNone)
		{
			NotifyAll(&IUOLMessengerTabViewCtrlObserver2::OnConversationTabMouseHover, tmp->m_pConversation, m_StartPoint, m_hWnd);
		}
	}

	Invalidate(FALSE);
	bHandled = FALSE;
	return 0;
}



LRESULT CTabView::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int i;
	CWidget * tmp=NULL;
	CPoint m_StartPoint;
	m_StartPoint.x = LOWORD(lParam); 
	m_StartPoint.y = HIWORD(lParam); 
	
	
	// Find the widget with click
	i = 0;
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos),i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		CWidget::clickItem itemclicked = (CWidget::clickItem) tmp->PtInWidget(m_StartPoint);
		if (itemclicked==CWidget::clickWidget) {
			if (tmp==m_lastClick) {
				if (tmp->m_state==CWidget::Blinking) {
					m_blinkingItems--;
					if (m_blinkingItems<1){
						if (m_timer!=NULL) {
							KillTimer(TAB_CHECK_TIMER);
							m_timer = NULL;
						}
					}
				}

				// Click Event;
				if (m_current>-1)
					Invalidate(FALSE);

				m_current=i;

				NotifyAll(&IUOLMessengerTabViewCtrlObserver2::OnConversationTabSelected, tmp->m_pConversation, m_current);
			}
			Invalidate(FALSE);
			m_lastClick=tmp;
			break;
		} else if (itemclicked==CWidget::clickClose) {
			// Click Widget Close
			//	MessageBox("CloseWidget");
		}
	}
	bHandled = FALSE;
	return 0;
}


void CTabView::RegisterObserver(IUOLMessengerTabViewCtrlObserver2* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);
		if (pos == NULL)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CTabView::UnregisterObserver(IUOLMessengerTabViewCtrlObserver2* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);
		if (pos != NULL)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}


void CTabView::SetTabViewType(int tabViewType)
{
	if ( (tabViewType == IUOLMessengerTabViewCtrl::TAB_VIEW_MESSAGE_WINDOW) || 
			(tabViewType == IUOLMessengerTabViewCtrl::TAB_VIEW_OTHER) )
	{
		m_tabViewType = tabViewType;
	}
}


void CTabView::ShowCloseButton(BOOL bShow)
{
	m_showCloseButton = bShow;
}


void CTabView::SetInterButtonSpace(int buttonSpace)
{
	m_buttonSpace = buttonSpace;
}


void CTabView::SetTabButtonMargins(
		int leftMarging, int topMarging, int rightMargin, int bottomMarging)
{
	m_buttonLeftMargin = leftMarging;
	m_buttonTopMargin = topMarging;
	m_buttonRightMargin = rightMargin;
	m_buttonBottomMargin = bottomMarging;
	
	m_bTabsChanged = TRUE;
}


void CTabView::AutoCalculateTabsWidths(BOOL bCalculate)
{
	m_bAutoCalculateWidths = bCalculate;
}


int CTabView::CreateTabButton(CString tabButtonName)
{
	int tabIndex = -1;
	
	if (m_tabViewType == IUOLMessengerTabViewCtrl::TAB_VIEW_OTHER)
	{
		tabIndex = GetTabCount(CWidget::Normal);
		
		AddTab(NULL, tabButtonName, CWidget::Normal, TRUE);
		
		m_bTabsChanged = TRUE;
		
		Invalidate();
		UpdateWindow();
	}
	
	return tabIndex;
}


BOOL CTabView::SetTabButtonImage(int nTabIndex, HMODULE hModule, DWORD dwResourceID)
{
	BOOL resultOk = FALSE;
	
	if (nTabIndex < GetTabCount(CWidget::Normal))
	{
		int i = 0;
		
		for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos), i++)
		{
			CWidget* pWidget = vWidgets.GetAt(pos);
			
			if (pWidget->GetWidgetIndex() == nTabIndex)
			{
				IUOLMessengerImagePtr pButtonTabImage = 
						UOLMessengerImage::CImageBuilder::LoadImageFromResource(
								hModule, dwResourceID);
				
				ATLASSERT(pButtonTabImage.get());
				
				pWidget->SetTabButtonImage(pButtonTabImage);
				
				m_bTabsChanged = TRUE;
				
				resultOk = TRUE;

				break;
			}
		}
	}

	if (resultOk)
	{
		Invalidate();
		UpdateWindow();
	}
	
	return resultOk;
}


BOOL CTabView::RemoveTabButton(int nTabIndex)
{
	int i = 0;
	
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos), i++)
	{
		CWidget* pWidget = vWidgets.GetAt(pos);
		
		if (pWidget->GetWidgetIndex() == nTabIndex)
		{
			if (m_current == i)
			{
				m_current = 0;
			}
			
			if (m_lastClick == pWidget)
			{
				m_lastClick = NULL;
			}
			
			if (m_lastOver == pWidget)
			{
				m_lastOver = NULL;
			}
			
			delete pWidget;
			vWidgets.RemoveAt(pos);
			
			// When ending app, this should not be done.
			if (IsWindow())
			{
				Invalidate();
				UpdateWindow();
			}
			
			m_bTabsChanged = TRUE;
			
			return TRUE;
		}
	}
	
	return FALSE;
}


void CTabView::NotifyAll(void (IUOLMessengerTabViewCtrlObserver2::*pfnHandler)(
		IUOLMessengerTabViewCtrl*, IUOLMessengerConversationPtr, int), IUOLMessengerConversationPtr pConversation, int nTabIndex)
{
	IUOLMessengerTabViewCtrlObserver2* pObserver;
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);
		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(IUOLMessengerTabViewCtrlObserver2*)))
		{
			(pObserver->*pfnHandler)(this, pConversation, nTabIndex);
		}
		else
		{
			ATLASSERT(FALSE);
			break;
		}
	}
}

void CTabView::NotifyAll(void (IUOLMessengerTabViewCtrlObserver2::*pfnHandler)(
		IUOLMessengerTabViewCtrl*, IUOLMessengerConversationPtr, CPoint&, HWND), IUOLMessengerConversationPtr pConversation, CPoint& pt, HWND wHandle)
{
	IUOLMessengerTabViewCtrlObserver2* pObserver;
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);
		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(IUOLMessengerTabViewCtrlObserver2*)))
		{
			(pObserver->*pfnHandler)(this, pConversation, pt, wHandle);
		}
		else
		{
			ATLASSERT(FALSE);
			break;
		}
	}
}

void CTabView::NotifyAll(void (IUOLMessengerTabViewCtrlObserver2::*pfnHandler)(void))
{
	IUOLMessengerTabViewCtrlObserver2* pObserver;
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);
		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(IUOLMessengerTabViewCtrlObserver2*)))
		{
			(pObserver->*pfnHandler)();
		}
		else
		{
			ATLASSERT(FALSE);
			break;
		}
	}
}

void CTabView::DrawTabs(CDCHandle& dc)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	//CRect rect;
	//GetClientRect(&rect);
	//CRect k (rect);
	//CRect m_w, m_g;
	//GetClientRect(m_g);
	//GetWindowRect(m_w);

	int i = 0;
	int len = vWidgets.GetCount();
	
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos),i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		tmp->DrawWidget(&dc,m_current==i,m_blinking/*,(len>1)*/);
	}
	
}
void  CTabView::SetBlinking(IUOLMessengerConversationPtr pConversation)
{
	ATLASSERT(pConversation!=NULL);
	if (vWidgets.GetCount()<=0) return ;
	int i=0;
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos),i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		if (tmp->m_pConversation==pConversation) {
			if (tmp->m_state!=CWidget::Blinking) {
				if (m_timer==NULL)
					m_timer = SetTimer(TAB_CHECK_TIMER, 500);	
				m_blinking++;
			}
			tmp->m_state = CWidget::Blinking;	
		}
	}
}

void CTabView::UpdateLayout(BOOL bResizeBars)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	__super::UpdateLayout(bResizeBars);
}

void CTabView::DoPaint(CDCHandle& dc)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	RecalcTabs(dc);
	//dc.FillSolidRect(m_rectClose,RGB(30,240,180));
	DrawTabs(dc);
}

void CTabView::GetMinSize(CSize& size)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	if ( (m_pTabButton) && (m_pTabButton->GetFillImage()) && 
			(m_pTabButton->GetFillImage()->GetNormal()) )
	{
		size.cx = 50;		// Aleatory value;
		size.cy = m_pTabButton->GetFillImage()->GetNormal()->GetHeight();
	}
	else
	{
		// No conditions to calculate it.
		size.cx = 0;
		size.cy = 0;
	}
}

BOOL CTabView::GetShowCloseButton()
{
	return m_showCloseButton;
}

void CTabView::GetTabButtonMargins(
		int& leftMarging, int& topMarging, int& rightMargin, int& bottomMarging)
{
	leftMarging = m_buttonLeftMargin;
	topMarging = m_buttonTopMargin;
	rightMargin = m_buttonRightMargin;
	bottomMarging = m_buttonBottomMargin;
}

UINT CTabView::GetTabsCount() const
{
	return (UINT) vWidgets.GetCount();
}

void CTabView::OnTimer(UINT nTimerId, TIMERPROC tp)
{
	if (nTimerId == TAB_CHECK_TIMER)
	{
		m_blinking = !m_blinking;
		Invalidate();
		UpdateWindow();
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}

CString CTabView::GetName() const
{
	// Todo: Verificar esta função.
	return "__UOLTabViewInstance_" + m_strGUID;
}

void CTabView::OnElementBuilt()
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
	{
		CElementPtr pElement = ap_dynamic_cast<CElementPtr>(m_listChilds.GetNext(pos));
		//CProtocolImageListElementPtr pProtocolImageList = ap_dynamic_cast<CProtocolImageListElementPtr>(pElement);
		//if (pProtocolImageList) {
		//	m_ProtocolImageListElement=pProtocolImageList;
		//}
	}
}

void CTabView::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	
	CString strDefaultCaption;

	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "Normal");
		m_pNormalImage = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pNormalImage.get());
		strDefaultCaption = strImageCaption;
	}


	try
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "Over");

		m_pOverImage = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pOverImage.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Over");
		m_pOverImage = m_pNormalImage;
		ATLASSERT(m_pOverImage.get());
	}	

	try
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "Clicked");

		m_pClikedImage  = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pClikedImage .get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Clicked");
		m_pClikedImage  = m_pNormalImage;
	}	

	try
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "Selected");

		m_pSelectedImage  = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pSelectedImage .get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Selected");
		m_pSelectedImage  = m_pNormalImage;
	}	


	try
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "Blinking");

		m_pBlinkingImage = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pBlinkingImage.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Blinking");
		m_pBlinkingImage  = m_pNormalImage;
	}	
	
	// Close Tab Image
	{
		CString strImageCaption;
		GetParameterValue(strImageCaption, pMapParams, "CloseTab");

		m_pCloseTabImage = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);	
		ATLASSERT(m_pCloseTabImage.get());
	}

	// Fonts
	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "SelectedFont");

		m_pFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFont.get());

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SelectedFont");
		// use default system font
		m_pFont = new CFontElement();
	}

	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "font");

		m_pFontSelected = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFontSelected.get());

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "font");
		// use default system font
		m_pFontSelected = new CFontElement();
	}

}

LRESULT CTabView::OnCreate(LPCREATESTRUCT lpCs)
{
	SetMsgHandled(FALSE);

	CTabViewButtonElementPtr pTabButton;

	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pTabButton.get() == NULL)); )
	{
		pTabButton = ap_dynamic_cast<CTabViewButtonElementPtr>(m_listChilds.GetNext(pos));
	}	

	if (pTabButton)
	{
		m_pTabButton = pTabButton;

		//CTabViewLeftImageElementPtr pLeftImage;
		//CTabViewFillImageElementPtr pFillImage;
		//CTabViewRightImageElementPtr pRightImage;

		//pLeftImage = pTabButton->GetLeftImage();
		//ATLASSERT(pLeftImage);

		//pFillImage = pTabButton->GetFillImage();
		//ATLASSERT(pFillImage);

		//pRightImage = pTabButton->GetRightImage();
		//ATLASSERT(pRightImage);

		//m_pOverImage = pLeftImage->GetOver();
		//m_pClikedImage = pLeftImage->GetOver();
		//m_pNormalImage = pLeftImage->GetOver();
		//m_pSelectedImage = pLeftImage->GetOver();
		//m_pBlinkingImage =pLeftImage->GetOver();
		
	}
	//m_pOverImage = NULL;
	//m_pOverImage = NULL;
	//m_pOverImage = NULL;
	//m_pOverImage = NULL;
	//m_pOverImage = NULL;
	return 0;
}

int CTabView::CalculateTabsTotalWidth()
{
	//ATLTRACE(_T("CTabView::CalculateTabsTotalWidth() - number of widgets = %d, and m_bTabsChanged = [%d]\n"), vWidgets.GetCount(), m_bTabsChanged);
	
	if (m_bTabsChanged)
	{
		m_nTotalTabsWidth = 0;
		HDC hWndDC = GetDC();
		
		for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos))
		{
			CWidget* pWidget = vWidgets.GetAt(pos);
			
			m_nTotalTabsWidth += pWidget->CalculateTabWidth(hWndDC);
			
			//ATLTRACE(_T("CTabView::CalculateTabsTotalWidth() - m_nTotalTabsWidth + widget [%s] width = %d\n"), pWidget->m_caption, m_nTotalTabsWidth);
		}
		
		ReleaseDC(hWndDC);
		
		m_bTabsChanged = FALSE;
	}
	
	//ATLTRACE(_T("CTabView::CalculateTabsTotalWidth() - m_nTotalTabsWidth = %d\n"), m_nTotalTabsWidth);
	
	return m_nTotalTabsWidth;
}

void  CTabView::SetCaption(IUOLMessengerConversationPtr pConversation, const CString& strCaption)
{
	ATLASSERT(pConversation!=NULL);
	if (vWidgets.GetCount()<=0)
    {
        return;
    }

	int i=0;
	for (POSITION pos = vWidgets.GetHeadPosition(); pos; vWidgets.GetNext(pos),i++)
	{
		CWidget * tmp = vWidgets.GetAt(pos);
		if (tmp->m_pConversation==pConversation) 
        {
			tmp->m_caption = strCaption;
            tmp->SetTabChanged(TRUE);
		}
	}
}
