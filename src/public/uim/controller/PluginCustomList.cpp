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
#include "PluginCustomList.h"


CPluginCustomList::CPluginCustomList()
{
}


CPluginCustomList::~CPluginCustomList()
{
	m_listEntries.RemoveAll();
}


void CPluginCustomList::RegisterObserver(CPluginCustomListObserver* pObserver)
{
	POSITION posFind = m_listObservers.Find(pObserver);

	if (!posFind)
	{
		m_listObservers.AddTail(pObserver);
	}
}


void CPluginCustomList::UnregisterObserver(CPluginCustomListObserver* pObserver)
{
	POSITION posFind = m_listObservers.Find(pObserver);

	if (posFind != NULL)
	{
		m_listObservers.RemoveAt(posFind);
	}
}


BOOL CPluginCustomList::AddEntry(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	if (pEntry != NULL)
	{
		IUOLMessengerPluginCustomListEntryPtr pFindEntry = FindEntryImpl(pEntry->GetEntryName(), pEntry->GetPluginInternalName());

		if (!pFindEntry)
		{
			m_listEntries.AddTail(pEntry);

			NotifyObservers(&CPluginCustomListObserver::OnAddEntry, pEntry);

			return TRUE;
		}
	}

	return FALSE;
}


IUOLMessengerPluginCustomListEntryPtr CPluginCustomList::RemoveEntry(const CString& strEntryName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomListEntryPtr pRemovedEntry;

	for (POSITION pos = m_listEntries.GetHeadPosition(); pos; m_listEntries.GetNext(pos))
	{
		IUOLMessengerPluginCustomListEntryPtr pEntry = m_listEntries.GetAt(pos);

		if (MatchEntry(strEntryName, strPluginInternalName, pEntry))
		{
			pRemovedEntry = pEntry;			
			m_listEntries.RemoveAt(pos);
			break;
		}
	}

	if (pRemovedEntry != NULL)
	{
		NotifyObservers(&CPluginCustomListObserver::OnRemoveEntry, pRemovedEntry);
	}

	return pRemovedEntry;
}


IUOLMessengerPluginCustomListEntryPtr CPluginCustomList::FindEntry(const CString& strEntryName, const CString& strPluginInternalName)
{
	return FindEntryImpl(strEntryName, strPluginInternalName);
}


void CPluginCustomList::GetEntries(CAtlList<IUOLMessengerPluginCustomListEntryPtr>& listEntries)
{
	listEntries.RemoveAll();

	for (POSITION pos = m_listEntries.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomListEntryPtr pEntry = m_listEntries.GetNext(pos);
		listEntries.AddTail(pEntry);
	}
}


void CPluginCustomList::UpdateEntry(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	if (pEntry != NULL)
	{
		NotifyObservers(&CPluginCustomListObserver::OnUpdateEntry, pEntry);
	}
}


void CPluginCustomList::UpdateEntry(const CString& strEntryName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomListEntryPtr pEntry = FindEntryImpl(strEntryName, strPluginInternalName);

	UpdateEntry(pEntry);
}


IUOLMessengerPluginCustomListEntryPtr CPluginCustomList::FindEntryImpl(const CString& strEntryName, const CString& strPluginInternalName)
{
	IUOLMessengerPluginCustomListEntryPtr pFindEntry;

	for (POSITION pos = m_listEntries.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginCustomListEntryPtr pEntry = m_listEntries.GetNext(pos);

		if (MatchEntry(strEntryName, strPluginInternalName, pEntry))
		{
			pFindEntry = pEntry;
			break;
		}
	}

	return pFindEntry;
}


BOOL CPluginCustomList::MatchEntry(const CString& strEntryName, const CString& strPluginInternalName, IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	return ((0 == strEntryName.Compare(pEntry->GetEntryName())) &&
			(0 == strPluginInternalName.Compare(pEntry->GetPluginInternalName())));
}


void CPluginCustomList::NotifyObservers(void (CPluginCustomListObserver::*pfnHandler)(IUOLMessengerPluginCustomListEntryPtr), IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	CPluginCustomListObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CPluginCustomListObserver*)))
		{
			(pObserver->*pfnHandler)(pEntry);
		}
	}
}