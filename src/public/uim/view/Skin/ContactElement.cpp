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
#include "ContactElement.h"

#include "../IPainterRepository.h"


CContactElement::CContactElement() : 
	m_nSmallSize(0),
	m_nMediumSize(0),
	m_nLargeSize(0)
{
	m_pFontContactBlocked = GetTextFont();
}


CContactElement::~CContactElement()
{
}


int CContactElement::GetHeight() const
{
	return GetSmallSize();
}


int CContactElement::GetSmallSize() const
{
	return m_nSmallSize;
}


int CContactElement::GetMediumSize() const
{
	return m_nMediumSize;
}


int CContactElement::GetLargeSize() const
{
	return m_nLargeSize;
}


IPainter* CContactElement::GetPainter(UINT nPainterId) const
{
	CString strPainterName;

	switch (nPainterId)
	{
	case IPainterRepository::PR_CONTACT_DEFAULT_PAINTER:
		strPainterName = "__ContactDefaultPainterInstance_";
		break;

	case IPainterRepository::PR_CONTACT_SELECTION_PAINTER:
		strPainterName = "__ContactSelectionPainterInstance_";
		break;

	case IPainterRepository::PR_CONTACT_ONLINE_PAINTER:
		strPainterName = "__OnlinePainterInstance_";
		break;

	case IPainterRepository::PR_CONTACT_OFFLINE_PAINTER:
		strPainterName = "__OfflinePainterInstance_";
		break;

	case IPainterRepository::PR_CONTACT_TYPING_PAINTER:
		strPainterName = "__TypingPainterInstance_";
		break;

	case IPainterRepository::PR_CONTACT_NEW_MESSAGE_PAINTER:
		strPainterName = "__NewMessagePainterInstance_";
		break;
	}

	return FindPainter(strPainterName);
}


CFontElementPtr CContactElement::GetBlockedTextFont() const
{
	return m_pFontContactBlocked;
}


COLORREF CContactElement::GetBlockedTextColor() const
{
	return m_clrContactBlocked;
}


CString CContactElement::GetName() const
{
	return "__ContactElementInstance_" + m_strGUID;
}


void CContactElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	__super::SetParameters(pMapParams, pElementTable);

	m_nSmallSize = m_nMediumSize = m_nLargeSize = 0;

	GetParameterValue(m_nSmallSize, pMapParams, _T("small"));
	m_nSmallSize = max(m_nSmallSize, 0);

	GetParameterValue(m_nMediumSize, pMapParams, _T("medium"));
	m_nMediumSize = max(m_nMediumSize, m_nSmallSize);

	GetParameterValue(m_nLargeSize, pMapParams, _T("large"));
	m_nLargeSize = max(m_nLargeSize, m_nMediumSize);

	try
	{
		CString strBlockedFontName;
		GetParameterValue(strBlockedFontName, pMapParams, "BlockedFont");

		m_pFontContactBlocked = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strBlockedFontName));
		ATLASSERT(m_pFontContactBlocked.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "BlockedFont");
		// use default system font
		m_pFontContactBlocked = GetTextFont();
	}

	try
	{
		CString strBlockedTextColor;
		GetParameterValue(strBlockedTextColor, pMapParams, "BlockedTextColor");		

		m_clrContactBlocked.SetColor(strBlockedTextColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "BlockedTextColor");
		// Use font color
		m_clrContactBlocked = m_pFontContactBlocked->GetColor();
	}
}


IPainter* CContactElement::FindPainter(const CString& strElementName) const
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		const IElement* pElement = m_listChilds.GetNext(pos).get();

		CString strElement = pElement->GetName();
        if (-1 != strElement.Find(strElementName))
		{
			IPainter* pPainter = dynamic_cast<IPainter*>(const_cast<IElement*>(pElement));

			return pPainter;
		}
	}

	return NULL;
}