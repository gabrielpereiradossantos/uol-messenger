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
#include "PluginCustomPanelContainer.h"


CPluginCustomPanelContainer::CPluginCustomPanelContainer()
{
}


CPluginCustomPanelContainer::~CPluginCustomPanelContainer()
{
}


void CPluginCustomPanelContainer::RegisterObserver(CPluginCustomPanelContainerObserver* pObserver)
{
	POSITION posFind = m_listObservers.Find(pObserver);

	if (!posFind)
	{
		m_listObservers.AddTail(pObserver);
	}
}


void CPluginCustomPanelContainer::UnregisterObserver(CPluginCustomPanelContainerObserver* pObserver)
{
	POSITION posFind = m_listObservers.Find(pObserver);

	if (posFind != NULL)
	{
		m_listObservers.RemoveAt(posFind);
	}
}


void CPluginCustomPanelContainer::GetPanels(CAtlList<IUOLMessengerPluginCustomPanelPtr>& listPanels)
{
	listPanels.RemoveAll();

	CComCritSecLock<CComAutoCriticalSection> lock(m_csPanels);

	for (POSITION pos = m_listPanels.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomPanelPtr pPanel = m_listPanels.GetNext(pos);
		listPanels.AddTail(pPanel);
	}
}


BOOL CPluginCustomPanelContainer::AddPanel(IUOLMessengerPluginCustomPanelPtr pPanel)
{
	BOOL bAdded = FALSE;

	if (pPanel != NULL)
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csPanels);

			IUOLMessengerPluginCustomPanelPtr pFindPanelBuilder = FindPanelImpl(pPanel->GetPanelName(), pPanel->GetPluginInternalName());

			if (!pFindPanelBuilder)
			{
				m_listPanels.AddTail(pPanel);
				bAdded = TRUE;
			}
		}

		if (bAdded)
		{
			NotifyObservers(&CPluginCustomPanelContainerObserver::OnAddPanel, pPanel);
		}
	}

	return bAdded;
}


IUOLMessengerPluginCustomPanelPtr CPluginCustomPanelContainer::RemovePanel(const CString& strPanelName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomPanelPtr pRemovedPanel;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csPanels);

		for (POSITION pos = m_listPanels.GetHeadPosition(); pos; m_listPanels.GetNext(pos))
		{
			IUOLMessengerPluginCustomPanelPtr pPanel = m_listPanels.GetAt(pos);

			if (MatchPanel(strPanelName, strPluginInternalName, pPanel))
			{
				pRemovedPanel = pPanel;			
				m_listPanels.RemoveAt(pos);
				break;
			}
		}
	}

	if (pRemovedPanel != NULL)
	{
		NotifyObservers(&CPluginCustomPanelContainerObserver::OnRemovePanel, pRemovedPanel);
	}

	return pRemovedPanel;
}


IUOLMessengerPluginCustomPanelPtr CPluginCustomPanelContainer::FindPanel(const CString& strPanelName, const CString& strPluginInternalName)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csPanels);

	return FindPanelImpl(strPanelName, strPluginInternalName);
}


void CPluginCustomPanelContainer::NotifyObservers(void (CPluginCustomPanelContainerObserver::*pfnHandler)(IUOLMessengerPluginCustomPanelPtr), IUOLMessengerPluginCustomPanelPtr pPanel)
{
	CPluginCustomPanelContainerObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CPluginCustomPanelContainerObserver*)))
		{
			(pObserver->*pfnHandler)(pPanel);
		}
	}
}


BOOL CPluginCustomPanelContainer::MatchPanel(const CString& strPanelName, const CString& strPluginInternalName, IUOLMessengerPluginCustomPanelPtr pPanel)
{
	return ((0 == strPanelName.Compare(pPanel->GetPanelName())) &&
			(0 == strPluginInternalName.Compare(pPanel->GetPluginInternalName())));
}


IUOLMessengerPluginCustomPanelPtr CPluginCustomPanelContainer::FindPanelImpl(const CString& strPanelName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomPanelPtr pFindPanel;

	for (POSITION pos = m_listPanels.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomPanelPtr pPanel = m_listPanels.GetNext(pos);

		if (MatchPanel(strPanelName, strPluginInternalName, pPanel))
		{
			pFindPanel = pPanel;
			break;
		}
	}

	return pFindPanel;
}