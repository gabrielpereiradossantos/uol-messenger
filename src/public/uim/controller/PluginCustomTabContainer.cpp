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

#pragma once

#include "stdafx.h"
#include "PluginCustomTabContainer.h"


CPluginCustomTabContainer::CPluginCustomTabContainer()
{
}


CPluginCustomTabContainer::~CPluginCustomTabContainer()
{
}


void CPluginCustomTabContainer::RegisterObserver(CPluginCustomTabContainerObserver* pObserver)
{
	POSITION posFind = m_listObservers.Find(pObserver);

	if (!posFind)
	{
		m_listObservers.AddTail(pObserver);
	}
}


void CPluginCustomTabContainer::UnregisterObserver(CPluginCustomTabContainerObserver* pObserver)
{
	POSITION posFind = m_listObservers.Find(pObserver);

	if (posFind != NULL)
	{
		m_listObservers.RemoveAt(posFind);
	}
}


void CPluginCustomTabContainer::GetTabs(CAtlList<IUOLMessengerPluginCustomTabPtr>& listTabs)
{
	listTabs.RemoveAll();

	CComCritSecLock<CComAutoCriticalSection> lock(m_csTabs);

	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomTabPtr pTab = m_listTabs.GetNext(pos);
		listTabs.AddTail(pTab);
	}
}


BOOL CPluginCustomTabContainer::AddTab(IUOLMessengerPluginCustomTabPtr pTab)
{
	BOOL bAdded = FALSE;

	if (pTab != NULL)
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csTabs);

			IUOLMessengerPluginCustomTabPtr pFindTabBuilder = FindTabImpl(pTab->GetTabName(), pTab->GetPluginInternalName());

			if (!pFindTabBuilder)
			{
				m_listTabs.AddTail(pTab);
				bAdded = TRUE;
			}
		}

		if (bAdded)
		{
			NotifyObservers(&CPluginCustomTabContainerObserver::OnAddTab, pTab);
		}
	}

	return bAdded;
}


IUOLMessengerPluginCustomTabPtr CPluginCustomTabContainer::RemoveTab(const CString& strTabName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomTabPtr pRemovedTab;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csTabs);

		for (POSITION pos = m_listTabs.GetHeadPosition(); pos; m_listTabs.GetNext(pos))
		{
			IUOLMessengerPluginCustomTabPtr pTab = m_listTabs.GetAt(pos);

			if (MatchTab(strTabName, strPluginInternalName, pTab))
			{
				pRemovedTab = pTab;			
				m_listTabs.RemoveAt(pos);
				break;
			}
		}
	}

	if (pRemovedTab != NULL)
	{
		NotifyObservers(&CPluginCustomTabContainerObserver::OnRemoveTab, pRemovedTab);
	}

	return pRemovedTab;
}


void CPluginCustomTabContainer::ShowTab(const CString& strTabName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomTabPtr pShowingTab;
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csTabs);

		for (POSITION pos = m_listTabs.GetHeadPosition(); pos; m_listTabs.GetNext(pos))
		{
			IUOLMessengerPluginCustomTabPtr pTab = m_listTabs.GetAt(pos);

			if (MatchTab(strTabName, strPluginInternalName, pTab))
			{
				pShowingTab = pTab;			
				break;
			}
		}
	}

	if (pShowingTab != NULL)
	{
		NotifyObservers(&CPluginCustomTabContainerObserver::OnShowTab, pShowingTab);
	}
}


void CPluginCustomTabContainer::ShowTab(int nTabIndex)
{
	IUOLMessengerPluginCustomTabPtr pShowingTab;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csTabs);
		
		for (POSITION pos = m_listTabs.GetHeadPosition(); pos; m_listTabs.GetNext(pos))
		{
			IUOLMessengerPluginCustomTab2Ptr pTab2 = ap_dynamic_cast<IUOLMessengerPluginCustomTab2Ptr>(m_listTabs.GetAt(pos));

			if (pTab2)
			{
				if (pTab2->GetTabButtonIndex() == nTabIndex)
				{
					pShowingTab = pTab2;
					break;
				}
			}
		}
	}
	
	if (pShowingTab != NULL)
	{
		NotifyObservers(&CPluginCustomTabContainerObserver::OnShowTab, pShowingTab);
	}
}


IUOLMessengerPluginCustomTabPtr CPluginCustomTabContainer::FindTab(const CString& strTabName, const CString& strPluginInternalName)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csTabs);

	return FindTabImpl(strTabName, strPluginInternalName);
}


void CPluginCustomTabContainer::NotifyObservers(void (CPluginCustomTabContainerObserver::*pfnHandler)(IUOLMessengerPluginCustomTabPtr), IUOLMessengerPluginCustomTabPtr pTab)
{
	CPluginCustomTabContainerObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CPluginCustomTabContainerObserver*)))
		{
			(pObserver->*pfnHandler)(pTab);
		}
	}
}


BOOL CPluginCustomTabContainer::MatchTab(const CString& strTabName, const CString& strPluginInternalName, IUOLMessengerPluginCustomTabPtr pTab)
{
	return ((0 == strTabName.Compare(pTab->GetTabName())) &&
			(0 == strPluginInternalName.Compare(pTab->GetPluginInternalName())));
}


IUOLMessengerPluginCustomTabPtr CPluginCustomTabContainer::FindTabImpl(const CString& strTabName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomTabPtr pFindTab;

	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomTabPtr pTab = m_listTabs.GetNext(pos);

		if (MatchTab(strTabName, strPluginInternalName, pTab))
		{
			pFindTab = pTab;
			break;
		}
	}

	return pFindTab;
}