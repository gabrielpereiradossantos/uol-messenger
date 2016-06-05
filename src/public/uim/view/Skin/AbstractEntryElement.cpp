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
#include "AbstractEntryElement.h"



CAbstractEntryElement::CAbstractEntryElement() : 
m_nHeight(0)
{
	// use default system font
	m_pTextFont = new CFontElement();
	m_pSelectedTextFont = m_pTextFont;
	m_pDropTargetTextFont = m_pTextFont;
}


CAbstractEntryElement::~CAbstractEntryElement()
{
}


int CAbstractEntryElement::GetHeight() const
{
	return m_nHeight;
}


CFontElementPtr CAbstractEntryElement::GetTextFont() const
{
	return m_pTextFont;	
}


COLORREF CAbstractEntryElement::GetTextColor() const
{
	return m_clrText;
}


CFontElementPtr CAbstractEntryElement::GetSelectedTextFont() const
{
	return m_pSelectedTextFont;
}


COLORREF CAbstractEntryElement::GetSelectedTextColor() const
{
	return m_clrSelectedText;
}


CFontElementPtr CAbstractEntryElement::GetDropTargetTextFont() const
{
	return m_pDropTargetTextFont;
}


COLORREF CAbstractEntryElement::GetDropTargetTextColor() const
{
	return m_clrDropTargetText;
}


void CAbstractEntryElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	try
	{
		GetParameterValue(m_nHeight, pMapParams, _T("Height"));
		m_nHeight = max(m_nHeight, 0);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Height");
		m_nHeight = 0;
	}

	try
	{
		CString strFontName;
		GetParameterValue(strFontName, pMapParams, "Font");

		m_pTextFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontName));
		ATLASSERT(m_pTextFont.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Font");
		m_pTextFont = new CFontElement();
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "TextColor");		

		m_clrText.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "TextColor");
		// Use font color
		m_clrText = m_pTextFont->GetColor();
	}

	try
	{
		CString strSelectedFontName;
		GetParameterValue(strSelectedFontName, pMapParams, "SelectedFont");

		m_pSelectedTextFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strSelectedFontName));
		ATLASSERT(m_pSelectedTextFont.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SelectedFont");
		// use default system font
		m_pSelectedTextFont = new CFontElement();
	}

	try
	{
		CString strSelectedTextColor;
		GetParameterValue(strSelectedTextColor, pMapParams, "SelectedTextColor");		

		m_clrSelectedText.SetColor(strSelectedTextColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SelectedTextColor");
		// Use font color
		m_clrSelectedText = m_pSelectedTextFont->GetColor();
	}

	try
	{
		CString strFontName;
		GetParameterValue(strFontName, pMapParams, "DropTargetFont");

		m_pDropTargetTextFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontName));
		ATLASSERT(m_pDropTargetTextFont.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "DropTargetFont");
		// use default system font
		m_pDropTargetTextFont = m_pTextFont;
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "DropTargetTextColor");		

		m_clrDropTargetText.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "DropTargetTextColor");
		// Use font color
		m_clrDropTargetText = m_pDropTargetTextFont->GetColor();
	}
}