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
#include "ButtonTabCtrl.h"

#include "../../controller/uimapplication.h"
#include "../image/image.h"

CButtonTabCtrl::CButtonTabCtrl() : 
	m_clrSelectionBackground(RGB(255, 255, 255)),
	m_nHorizontalSpacement(0),
	m_nVerticalSpacement(0),
	m_nImageAreaHeight(0),
	m_nSelectedItem(-1)
{
}


CButtonTabCtrl::~CButtonTabCtrl()
{
}


int CButtonTabCtrl::GetControlHeight() const
{
	return m_nImageAreaHeight + GetTextAreaHeight() + 2 * m_nVerticalSpacement;
}


void CButtonTabCtrl::InsertItem(int nIndex,  const CString& strText, IUOLMessengerImagePtr pImage, BOOL bDrawTransparent)
{
	if (nIndex >= 0)
	{
		if (pImage != NULL)
		{
			m_nImageAreaHeight = max(m_nImageAreaHeight, pImage->GetHeight());
		}

		m_arTabItems.InsertAt(nIndex, new CTabItem(strText, pImage, bDrawTransparent));

		if (IsWindow())
		{
			UpdateWindow();
		}
	}
}


void CButtonTabCtrl::RemoveItem(int nIndex)
{
	if ((nIndex >= 0) && (nIndex < (int) m_arTabItems.GetCount()))
	{
		m_arTabItems.RemoveAt(nIndex);

		UpdateImageAreaHeight();

		if (IsWindow())
		{
			UpdateWindow();
		}
	}
}


void CButtonTabCtrl::RemoveAll()
{
	m_arTabItems.RemoveAll();

	m_nImageAreaHeight = 0;

	if (IsWindow())
	{
		UpdateWindow();
	}
}


int CButtonTabCtrl::GetItemCount()
{
	return (int) m_arTabItems.GetCount();
}


int CButtonTabCtrl::GetCurSel()
{
	return m_nSelectedItem;
}


void CButtonTabCtrl::SetCurSel(int nIndex)
{
	m_nSelectedItem = nIndex;

	if (IsWindow())
	{
		InvalidateRect(NULL);
	}
}


void CButtonTabCtrl::OnLButtonDown(UINT nFlags, CPoint& ptClick)
{
	int nInd = GetTabItem(ptClick);

	if (-1 != nInd)
	{
		m_nSelectedItem = nInd;

		// Notify parent window
		CWindow wndParent = GetParent();
		if (wndParent.IsWindow())
		{
			NMHDR nmhdr = {0};
			nmhdr.code = TCN_SELCHANGE;
			nmhdr.hwndFrom = m_hWnd;
			nmhdr.idFrom = GetDlgCtrlID();

			wndParent.SendMessage(WM_NOTIFY, (WPARAM) GetDlgCtrlID(), (LPARAM) &nmhdr);
		}

		InvalidateRect(NULL);
	}

	//ATLTRACE(_T("CButtonTabCtrl::OnLButtonDown() - nInd = %d\n"), nInd);
}


void CButtonTabCtrl::UpdateImageAreaHeight()
{
	int nHeight = 0;

	int nCount = (int) m_arTabItems.GetCount();
	for (int nInd = 0; nInd < nCount; nInd++)
	{
		CTabItemPtr pTabItem = m_arTabItems[nInd];

		ATLASSERT(pTabItem != NULL);
		if (pTabItem != NULL)
		{
			IUOLMessengerImagePtr pDib = pTabItem->GetImage();
			nHeight = max(nHeight, pDib->GetHeight());
		}
	}

	m_nImageAreaHeight = nHeight;
}


void CButtonTabCtrl::DrawTabItems(CDCHandle&dc)
{
	// Draw each button...
	int x = 0;
	int y = m_nVerticalSpacement;
	int nCount = (int) m_arTabItems.GetCount();

	for (int nInd = 0; nInd < nCount; nInd++)
	{
		CTabItemPtr pTabItem = m_arTabItems[nInd];
		
		ATLASSERT(pTabItem!= NULL);
		if (pTabItem != NULL)
		{
			IUOLMessengerImagePtr pImage = pTabItem->GetImage();

			ATLASSERT(pImage != NULL);

			int nImageWidth = pImage->GetWidth();
			int nImageHeight = pImage->GetHeight();

			// Calculate text rect
			CRect rectText(CPoint(x + m_nHorizontalSpacement, y + m_nImageAreaHeight), 
							CSize(0, GetTextAreaHeight()));

			CString strText = pTabItem->GetText();
			CSize sizeText;
			if (!strText.IsEmpty())
			{
				
				m_pFont->GetTextWidth(dc, strText, sizeText);
				//dc.DrawText(strText, strText.GetLength(), &rectText, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);
			}

			// Calculate and save tab item width
			int nTabWidth = max(nImageWidth, sizeText.cx) + 2 * m_nHorizontalSpacement;
			pTabItem->SetWidth(nTabWidth);

			// Draw item background
			if (nInd == m_nSelectedItem)
			{
				CBrush brush;
				brush.CreateSolidBrush(m_clrSelectionBackground);

				dc.FillRect(CRect(CPoint(x, 0), 
					              CSize(nTabWidth, GetControlHeight())), brush);
			}


			// Draw item image
			int xImage = x + ((nTabWidth - nImageWidth) / 2);

			IUOLMessengerImageDrawStrategyPtr pStrategy;

			if (pTabItem->IsBackgroundTransparent())
			{
				pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();				
			}
			else
			{
				pStrategy = new UOLMessengerImage::CDrawOpaqueStrategy();
			}
			
			pImage->Draw(CDCHandle(dc), xImage, y, nImageWidth, nImageHeight, pStrategy);				

			// Draw item text if not empty
			if (!strText.IsEmpty())
			{
				m_pFont->DrawText(dc,rectText,strText,(UINT) DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				//dc.DrawText(strText, strText.GetLength(), &rectText, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}

			x += nTabWidth;
		}
	}
}


int CButtonTabCtrl::GetTabItem(CPoint& pt)
{
	BOOL bPtInItem;
	CRect rectTabItem;
	int x = 0;
	int y = 0;
	int w;

	int nCount = (int) m_arTabItems.GetCount();
	for (int nInd = 0; nInd < nCount; nInd++)
	{
		CTabItemPtr pTabItem = m_arTabItems[nInd];

		ATLASSERT(pTabItem != NULL);
		if (pTabItem != NULL)
		{
			IUOLMessengerImagePtr pImage = pTabItem->GetImage();

			w = pTabItem->GetWidth();

			rectTabItem.left = x;
			rectTabItem.top = y;
			rectTabItem.right = rectTabItem.left + w;
			rectTabItem.bottom = rectTabItem.top + pImage->GetHeight() + GetTextAreaHeight() + 2 * m_nVerticalSpacement;

			bPtInItem = rectTabItem.PtInRect(pt);
			if (bPtInItem)
			{
				return nInd;
			}

			x += w;
		}
	}

	return -1;
}


int CButtonTabCtrl::GetTextAreaHeight() const
{
	return 20;
}

void CButtonTabCtrl::UpdateLayout(BOOL bResizeBars)
{
	__super::UpdateLayout(bResizeBars);
}

void CButtonTabCtrl::DoPaint(CDCHandle& dc)
{
	//RecalcTabs(dc);
	//dc.FillSolidRect(m_rectClose,RGB(30,240,180));
	//DrawTabs(dc);
	DrawTabItems(dc);
}

void CButtonTabCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	// Fonts
	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "Font");

		m_pFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFont.get());

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Font");
		// use default system font
		m_pFont = new CFontElement();
	}



	try
	{
		GetParameterValue(m_nVerticalSpacement, pMapParams, "VerticalSpacement");
		GetParameterValue(m_nHorizontalSpacement, pMapParams, "HorizontalSpacement");
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Spacements");
	}

	//try
	//{
	//	CString strColor;
	//	GetParameterValue(strColor, pMapParams, "TextColor");
	//	m_clrText.SetColor(strColor);
	//}
	//catch(const CSkinException&)
	//{
	//	ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "TextColor");
	//	m_clrText.SetColor(_T("#e6e6e6"));
	//}

	//try
	//{
	//	CString strColor;
	//	GetParameterValue(strColor, pMapParams, "DlgBackgroundColor");
	//	m_clrBackground.SetColor(strColor);
	//}
	//catch(const CSkinException&)
	//{
	//	ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "DlgBackgroundColor");
	//	m_clrBackground.SetColor(_T("#e6e6e6"));
	//}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "SelectionBackColor");
		m_clrSelectionBackground.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SelectionBackColor");
		m_clrSelectionBackground.SetColor(_T("#e6e6e6"));
	}

	/*VerticalSpacement="4"
		HorizontalSpacement="4"
		SelectionBackColor="#000000"
		TextColor="#000000"	>*/
}

CString CButtonTabCtrl::GetName() const
{
	// Todo: Verificar esta função.
	return "__UOLTabViewInstance_" ;
}