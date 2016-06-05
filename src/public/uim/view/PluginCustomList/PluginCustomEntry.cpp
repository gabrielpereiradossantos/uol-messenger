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
#include "PluginCustomEntry.h"

#include "../image/Image.h"


using namespace GroupListCtrl;


CPluginCustomEntry::CPluginCustomEntry(IUOLMessengerPluginCustomListEntryPtr pEntry, CPluginCustomEntryElementPtr pEntryElement)
{
	m_pEntry = pEntry;
	m_pEntryElement = pEntryElement;
}


CPluginCustomEntry::~CPluginCustomEntry()
{
}


IUOLMessengerPluginCustomListEntryPtr CPluginCustomEntry::GetEntry() const
{
	return m_pEntry;
}


void CPluginCustomEntry::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CPluginCustomEntry::GetName() const
{
	return m_strName;
}


void CPluginCustomEntry::SetSelected(BOOL bSelected)
{
	if (bSelected != GetSelected())
	{
		CEntry::SetSelected(bSelected);

		m_pEntry->OnSelectionChanged(bSelected);
	}
}


BOOL CPluginCustomEntry::IsVisible() const
{
	return TRUE;
}


void CPluginCustomEntry::Paint(CDCHandle& dc)
{
	ATLASSERT(m_pEntry != NULL);

	CRect rect(CPoint(0, 0), GetSize());

	IUOLMessengerPainterPtr pPainter = m_pEntry->GetPainter();

	if (pPainter != NULL)
	{
		pPainter->DoPaint(dc, rect);
	}
	else
	{
		DoDefaultPaint(dc, rect);
	}
}


void CPluginCustomEntry::DrawBackground(CDCHandle& dc)
{
}


void CPluginCustomEntry::OnLBtnDblClick(const CPoint& pt)
{
	m_pEntry->OnLButtonDoubleClick(pt);
}


void CPluginCustomEntry::OnReturnKeyPressed()
{
	m_pEntry->OnReturnKeyPressed();
}


void CPluginCustomEntry::DoDefaultPaint(CDCHandle& dc, const CRect& rect)
{
	DefaultDrawBackground(dc, rect);
	DefaultDrawIcon(dc, rect);
	DefaultDrawDisplayName(dc, rect);
}


void CPluginCustomEntry::DefaultDrawBackground(CDCHandle& dc, const CRect& rect)
{
	if (m_pEntryElement != NULL)
	{
		CRect rectDCMem = rect;
		CMemDC dcMem(dc, &rectDCMem);

		CDCHandle dcHandle = dcMem;

		CBackgroundElementPtr pBackgoundElement = m_pEntryElement->GetBackground();

		if (pBackgoundElement)
		{
			pBackgoundElement->Draw(dcHandle, rectDCMem);
		}
	}
}


void CPluginCustomEntry::DefaultDrawIcon(CDCHandle& dc, const CRect& rect)
{
	m_rectIcon = rect;

	m_rectIcon.bottom -= 2;
	m_rectIcon.top += 2;
	m_rectIcon.left = 18;

	BOOL bDrawTransparent = TRUE;
	IUOLMessengerImagePtr pImage = m_pEntry->GetIcon(bDrawTransparent);

	if (pImage != NULL)
	{
		m_rectIcon.right = m_rectIcon.left + m_rectIcon.Height();

		IUOLMessengerImageDrawStrategyPtr pDrawStrategy;
		
		if (bDrawTransparent)
		{
			pDrawStrategy = new UOLMessengerImage::CDrawStretchTransparentStrategy();
		}
		else
		{
			pDrawStrategy = new UOLMessengerImage::CDrawStretchOpaqueStrategy();
		}

		pImage->Draw(dc, 
			m_rectIcon.left, m_rectIcon.top, 
			m_rectIcon.Width(), m_rectIcon.Height(), 
			pDrawStrategy);
	}
	else
	{
		m_rectIcon.right = m_rectIcon.left;
	}
}


void CPluginCustomEntry::DefaultDrawDisplayName(CDCHandle& dc, const CRect& rect)
{
	if (m_pEntryElement != NULL)
	{
		CFontElementPtr pFont = GetSelected() ? m_pEntryElement->GetSelectedTextFont() : m_pEntryElement->GetTextFont();

		CRect rectText(CPoint(0, 0), GetSize());

		rectText.left = m_rectIcon.right + 7;
		rectText.right = rect.right;

		COLORREF clr = GetSelected() ? m_pEntryElement->GetSelectedTextColor() : m_pEntryElement->GetTextColor();
		
		CString strText = m_pEntry->GetDisplayName();

		pFont->DrawColoredText(dc, rectText, strText, clr, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
		
	}
}