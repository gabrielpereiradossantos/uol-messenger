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
#include "PluginCustomGroup.h"

#include "../Image/Image.h"


CPluginCustomGroup::CPluginCustomGroup()
{
}


CPluginCustomGroup::~CPluginCustomGroup()
{
}

void CPluginCustomGroup::SetBackground(CBackgroundElementPtr pBackground)
{
	m_pBackground = pBackground;
}


void CPluginCustomGroup::SetCollapsedImage(IUOLMessengerImagePtr pImageGroupCollapserCollapsed)
{
	m_pImageGroupCollapserCollapsed = pImageGroupCollapserCollapsed;
}


void CPluginCustomGroup::SetExpandedImage(IUOLMessengerImagePtr pImageGroupCollapserExpanded)
{
	m_pImageGroupCollapserExpanded = pImageGroupCollapserExpanded;
}


void CPluginCustomGroup::SetTextFont(CFontElementPtr pFont)
{
	m_pFontText = pFont;
}


void CPluginCustomGroup::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}


void CPluginCustomGroup::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CPluginCustomGroup::GetName() const
{
	return m_strName;
}


BOOL CPluginCustomGroup::IsVisible() const
{
	return TRUE;
}


void CPluginCustomGroup::Paint(CDCHandle& dc)
{		
	DrawCollapser(dc);	
	DrawText(dc);
}


void CPluginCustomGroup::DrawBackground(CDCHandle& dc)
{
	if (m_pBackground != NULL)
	{
		CRect rect(CPoint(0, 0), GetSize());
		CMemDC dcMem(dc, &rect);

		CDCHandle dcHandle = dcMem;
		
		m_pBackground->Draw(dcHandle, rect);
	}
}


void CPluginCustomGroup::DrawCollapser(CDCHandle& dc)
{
	IUOLMessengerImagePtr pImage = (GetGroupState() == CEntryGroup::collapsed) ? m_pImageGroupCollapserCollapsed : m_pImageGroupCollapserExpanded;
	ATLASSERT(pImage);

	if (pImage)
	{
		CRect rect(CPoint(0, 0), GetSize());

		int w = pImage->GetWidth();
		int h = pImage->GetHeight();

		CDCHandle dcHandle(dc);

		int y = (rect.Height() - h) / 2;

		IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();

		pImage->Draw(dcHandle, 
			7, y, 
			w, h, 
			pStrategy);
	}
}


void CPluginCustomGroup::DrawText(CDCHandle& dc)
{
	CString strName = GetName();
	CRect rectText(CPoint(0, 0), GetSize());

	IUOLMessengerImagePtr pImage = (GetGroupState() == CEntryGroup::collapsed) ? m_pImageGroupCollapserCollapsed : m_pImageGroupCollapserExpanded;
	ATLASSERT(pImage);
	rectText.left = 7 + pImage->GetWidth() + 5;

	m_pFontText->DrawColoredText(dc, rectText, m_strName, m_clrText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
}

