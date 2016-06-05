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
#include ".\fontelement.h"

CFontElement::CFontElement(void) : 
	m_pFont()
{
}

CFontElement::~CFontElement(void)
{
}

void CFontElement::SetFont(const CString& strFaceName, UINT nSize, BOOL bBold, BOOL bItalic)
{
	if (!m_pFont)
	{
		m_pFont = new LOGFONT();
		::ZeroMemory(m_pFont.get(), sizeof(LOGFONT));
	}

	::_tcsncpy(m_pFont->lfFaceName, (LPCSTR)strFaceName, 32);
	m_pFont->lfHeight = nSize;
	m_pFont->lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	m_pFont->lfItalic = bItalic;
}

void CFontElement::SetItalic(BOOL bItalic)
{
	m_pFont->lfItalic = bItalic;
}

void CFontElement::SetBold(BOOL bBold)
{
	m_pFont->lfWeight = bBold ? FW_BOLD : FW_NORMAL;
}

void CFontElement::SetColor(COLORREF clr)
{
	m_clr = clr;
}


LOGFONTPtr CFontElement::GetFont() const
{
	return m_pFont;
}

COLORREF CFontElement::GetColor() const
{
	return m_clr;
}

void CFontElement::DrawText(CDCHandle& dc, CRect& rectText, const CString& strText, UINT nFormat) const
{
	DrawColoredText(dc, rectText, strText, m_clr, nFormat);
}

void CFontElement::GetTextWidth(CDCHandle& dc, const CString& strText, CSize &size) 
{
	WTL::CFont font;

	// Create font
	if (m_pFont.get() != NULL)
	{	
		font.CreateFontIndirect(m_pFont.get());

		ATLASSERT(static_cast<HFONT>(font));
	}

	if (font.IsNull())
	{
		font.Attach(reinterpret_cast<HFONT>(::GetStockObject(SYSTEM_FONT)));
	}


	// Save current DC config
	HFONT oldFont = dc.SelectFont(font);
	int nOldBkMode = dc.SetBkMode(TRANSPARENT);	

	// Get text dimensions
	CRect tmpRect;
	dc.DrawText(strText, strText.GetLength(), &tmpRect,  DT_CALCRECT);
	size.cx = tmpRect.Width();
	size.cy = tmpRect.Height();

	// Draw text
	//dc.DrawText(strText, strText.GetLength(), &rectText, nFormat);

	// Restore old DC config
	dc.SetBkMode(nOldBkMode);
	//dc.SetTextColor(oldClr);
	dc.SelectFont(oldFont);
}

void CFontElement::DrawColoredText(CDCHandle& dc, CRect& rectText, const CString& strText, COLORREF clrText, UINT nFormat) const
{
	WTL::CFont font;

	// Create font
	if (m_pFont.get() != NULL)
	{	
		font.CreateFontIndirect(m_pFont.get());

		ATLASSERT(static_cast<HFONT>(font));
	}

	if (font.IsNull())
	{
		font.Attach(reinterpret_cast<HFONT>(::GetStockObject(SYSTEM_FONT)));
	}
	
	// Save current DC config
	HFONT oldFont = dc.SelectFont(font);
	COLORREF oldClr = dc.SetTextColor(clrText);
	int nOldBkMode = dc.SetBkMode(TRANSPARENT);	

	// Get text dimensions
	CRect rectCalc = rectText;
	dc.DrawText(strText, strText.GetLength(), &rectCalc, nFormat | DT_CALCRECT);
	rectCalc.right += 5;

	// Draw text
	rectText.right = max(rectText.right, rectCalc.right);
	rectText.bottom = max(rectText.bottom, rectCalc.bottom);
	dc.DrawText(strText, strText.GetLength(), &rectText, nFormat);

	// Restore old DC config
	dc.SetBkMode(nOldBkMode);
	dc.SetTextColor(oldClr);
	dc.SelectFont(oldFont);
}

void CFontElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	m_pFont = new LOGFONT();
	::ZeroMemory(m_pFont.get(), sizeof(LOGFONT));

	CString str;

	GetParameterValue(str, pMapParams, "facename");
	::_tcsncpy(m_pFont->lfFaceName, (LPCSTR)str, 32);

	GetParameterValue(m_pFont->lfHeight, pMapParams, "size");

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "color");

		m_clr.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "color");
	}

	// bold
	try
	{
		CString strBold;
		GetParameterValue(strBold, pMapParams, "bold");

		m_pFont->lfWeight = (strBold.CompareNoCase("true") == 0  || strBold == '1') ? FW_BOLD : FW_NORMAL;		
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "bold");
		m_pFont->lfWeight = FW_NORMAL;
	}


	// italic
	try
	{
		CString strItalic;
		GetParameterValue(strItalic, pMapParams, "italic");

		m_pFont->lfItalic = (strItalic.CompareNoCase("true") == 0  || strItalic == '1') ? TRUE : FALSE;		
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "italic");
		m_pFont->lfItalic = FALSE;
	}
}