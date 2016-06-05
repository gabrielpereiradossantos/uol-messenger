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
#include ".\menuelement.h"

//////////////////////////////////////////////////////////////////////
// CMenuElement
//////////////////////////////////////////////////////////////////////

CMenuElement::CMenuElement(void)
{
}

CMenuElement::~CMenuElement(void)
{
}

CMenuHandle CMenuElement::GetMenu()
{
	return CMenuHandle(m_menu);
}

CMenuItemElement* CMenuElement::GetMenuItemElement(int nId)
{
	const CAtlMap<int, CMenuItemElement*>::CPair* pPair = m_mapItemElement.Lookup(nId);

	if (pPair)
	{
		return pPair->m_value; 
	}

	return NULL;
}

CMenuItemElement* CMenuElement::FindMenuItemElement(CString strCommand)
{
	for (POSITION pos = m_mapItemElement.GetStartPosition(); pos;)
	{
		const CAtlMap<int, CMenuItemElement*>::CPair* pPair = m_mapItemElement.GetNext(pos);
		CMenuItemElement* pMenuItem = pPair->m_value;
		if (pMenuItem)
		{
			if (0 == strCommand.Compare(pMenuItem->GetCommand()))
			{
				return pMenuItem;
			}
		}
	}

	return NULL;
}

void CMenuElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);	
}

void CMenuElement::OnElementBuilt()
{	
	BOOL bOK = m_menu.CreateMenu();

	if (FALSE == bOK)
	{
		return;
	}
	
	int nCommand = 0;
	CMenuHandle menu(m_menu);

	int nPosition = 0;

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; nPosition++)
	{
		CMenuCommonBasePtr pSubMenu = ap_dynamic_cast<CSubMenuElementPtr>(m_listChilds.GetNext(pos));
		ATLASSERT(pSubMenu.get());

		pSubMenu->ProccessMenu(menu, nCommand, nPosition, &m_mapItemElement);
	}
}

//////////////////////////////////////////////////////////////////////
// CMenuCommonBase
//////////////////////////////////////////////////////////////////////

CMenuCommonBase::CMenuCommonBase()
{
}

CMenuCommonBase::~CMenuCommonBase()
{
}

void CMenuCommonBase::ProccessMenu(CMenuHandle& menuParent, int& nCommand, int nPosition, CAtlMap<int, CMenuItemElement*>* pMapItemElement)
{
}

void CMenuCommonBase::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		GetParameterValue(m_strDesc, pMapParams, "desc");	
	}
	catch(const CSkinException&)
	{
	
	}
}


CString CMenuCommonBase::GetName() const
{
	if (m_strName.GetLength() > 0)
		return m_strName;
	else
		return m_strGUIDName;
}

BOOL CMenuCommonBase::EnableItem(BOOL bEnable)
{
	UINT nEnable = MF_BYPOSITION;

	nEnable |= bEnable ? MF_ENABLED : MF_GRAYED;

	return m_menuHandle.EnableMenuItem(m_nMenuPosition, nEnable);
}


//////////////////////////////////////////////////////////////////////
// CSubMenuElement
//////////////////////////////////////////////////////////////////////

CSubMenuElement::CSubMenuElement(void)
{
}

CSubMenuElement::~CSubMenuElement(void)
{
}

void CSubMenuElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	__super::SetParameters(pMapParams, pElementTable);	
}

void CSubMenuElement::ProccessMenu(CMenuHandle& menuParent, 
								   int& nCommand, 
								   int nPosition,
								   CAtlMap<int, CMenuItemElement*>* pMapItemElement)
{
	__super::ProccessMenu(menuParent, nCommand, nPosition, pMapItemElement);

	CMenuHandle menu;
	BOOL bOk = menu.CreatePopupMenu();
	ATLASSERT(bOk);

	int nLocalPosition = 0;

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; nLocalPosition++)
	{
		CMenuCommonBasePtr pMenu = ap_dynamic_cast<CMenuCommonBasePtr>(m_listChilds.GetNext(pos));
		ATLASSERT(pMenu.get());

		if (pMenu.get())
		{
			pMenu->ProccessMenu(menu, nCommand, nLocalPosition, pMapItemElement);
		}
	}	

	int nItem = nCommand++;
	
	bOk = menuParent.InsertMenu(nItem, MF_STRING | MF_BYCOMMAND, menu, m_strDesc);
	ATLASSERT(bOk);

	m_nMenuPosition = nPosition;
	m_menuHandle = menuParent;				
}


//////////////////////////////////////////////////////////////////////
// CMenuItemElement
//////////////////////////////////////////////////////////////////////


CMenuItemElement::CMenuItemElement(void)
{
}

CMenuItemElement::~CMenuItemElement(void)
{
}

CString CMenuItemElement::GetCommand() const
{
	return m_strCommand;
}

void CMenuItemElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
	
	GetParameterValue(m_strCommand, pMapParams, "command");	
}

void CMenuItemElement::ProccessMenu(CMenuHandle& menuParent, int& nCommand, int nPosition, CAtlMap<int, CMenuItemElement*>* pMapItemElement)
{
	__super::ProccessMenu(menuParent, nCommand, nPosition, pMapItemElement);

	(*pMapItemElement)[nCommand] = this;

	int nItem = nCommand++;
	BOOL bOk = menuParent.InsertMenu(nPosition, MF_STRING | MF_BYPOSITION, nItem, m_strDesc);	
	ATLASSERT(bOk);	

	m_nMenuPosition = nPosition;
	m_menuHandle = menuParent;		
}

//////////////////////////////////////////////////////////////////////
// CMenuSeparator
//////////////////////////////////////////////////////////////////////


CMenuSeparator::CMenuSeparator(void)
{
}

CMenuSeparator::~CMenuSeparator(void)
{
}


void CMenuSeparator::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	__super::SetParameters(pMapParams, pElementTable);
}

void CMenuSeparator::ProccessMenu(CMenuHandle& menuParent, int& nCommand, int nPosition, CAtlMap<int, CMenuItemElement*>* pMapItemElement)
{
	__super::ProccessMenu(menuParent, nCommand, nPosition, pMapItemElement);

	int nItem = nCommand++;
	BOOL bOk = menuParent.InsertMenu(nPosition, MF_SEPARATOR | MF_BYPOSITION);
	ATLASSERT(bOk);

	m_nMenuPosition = nPosition;
	m_menuHandle = menuParent;	
}
