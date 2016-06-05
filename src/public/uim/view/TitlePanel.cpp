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
#include "TitlePanel.h"
#include "../controller/UIMApplication.h"

#define LEFT_MARGIN 2
#define TOP_MARGIN  0

CTitlePanel::CTitlePanel()
{    
}


CTitlePanel::~CTitlePanel()
{
}


CString CTitlePanel::GetName() const
{
	return "__TitlePanelInstance_" + m_strGUID;
}


void CTitlePanel::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strValue;
        GetParameterValue(strValue, pMapParams, "textcolor");

        m_rgbTextColor.SetColor(strValue);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "textcolor");
		m_rgbTextColor.SetColor(GetSysColor(COLOR_WINDOWTEXT));
	}
    
    try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "textfont");

		m_pTextFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "textfont");
		// use default system font
	    m_pTextFont = new CFontElement();
	}

    try
	{
		GetParameterValue(m_nHeight, pMapParams, "height");
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "height");
		// use default height
	    m_nHeight = 19;
	}
}


void CTitlePanel::DoPaint(CDCHandle& dc)
{
    DrawText(dc);
}


void CTitlePanel::UpdateLayout(CSize size)
{
    SetWindowPos(NULL, 0, 0, size.cx, m_nHeight, SWP_NOZORDER);
}


void CTitlePanel::UpdateLayout()
{
    CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());
    
    UpdateLayout(size);
}


void CTitlePanel::GetMinSize(CSize& size)
{
	size = CSize(0, m_nHeight);
}


void CTitlePanel::SetText(CString strText)
{
    m_strTitleText = strText;
}


CString CTitlePanel::GetText()
{
    return m_strTitleText;
}


void CTitlePanel::DrawText(CDCHandle& dc)
{
	ATLASSERT(m_pTextFont);

    CRect rect;
	GetClientRect(&rect);

	CRect rectText = CRect(CPoint(LEFT_MARGIN, TOP_MARGIN), CSize(rect.Width(), rect.Height()));
		
	m_pTextFont->DrawColoredText(dc, rectText, m_strTitleText, m_rgbTextColor,
			DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
}
