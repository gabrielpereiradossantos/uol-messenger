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
#include "PluginCustomListCtrl.h"

#include "PluginCustomEntry.h"
#include "../../resource.h"


using namespace GroupListCtrl;


CPluginCustomListCtrl::CPluginCustomListCtrl() : 
	m_pGroupElement(NULL),
	m_pEntryElement(NULL)
{
}


CPluginCustomListCtrl::~CPluginCustomListCtrl()
{
	if (m_pEntryMenu != NULL)
	{
		m_pEntryMenu->DestroyMenu();
	}

	if (m_pGroupElement)
	{
		m_pGroupElement.reset();
	}

	if (m_pEntryElement)
	{
		m_pEntryElement.reset();
	}
}


void CPluginCustomListCtrl::Init(IUOLMessengerPluginCustomListPtr pPluginCustomList)
{
	if (pPluginCustomList != NULL)
	{
		m_pPluginCustomList = pPluginCustomList;
		
		m_pPluginCustomList->RegisterObserver(this);

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
		{
			IElementPtr pElement = m_listChilds.GetNext(pos);

			if (0 == pElement->GetName().CompareNoCase(_T("__PluginCustomGroupElementInstance")))
			{
				m_pGroupElement = ap_dynamic_cast<CPluginCustomGroupElementPtr>(pElement);
			}
			else if (0 == pElement->GetName().CompareNoCase(_T("__PluginCustomEntryElementInstance")))
			{
				m_pEntryElement = ap_dynamic_cast<CPluginCustomEntryElementPtr>(pElement);
			}
		}

		ATLASSERT(m_pGroupElement);
		ATLASSERT(m_pEntryElement);
	}
}


void CPluginCustomListCtrl::Finalize()
{
	if (m_pPluginCustomList != NULL)
	{
		m_pPluginCustomList->UnregisterObserver(this);
	}
}


CString CPluginCustomListCtrl::GetName() const
{
	return "__PluginCustomListInstance";
}


void CPluginCustomListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}


void CPluginCustomListCtrl::GetMinSize(CSize& size)
{
	return CGroupListCtrl::GetSize(size);
}


void CPluginCustomListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CPluginCustomListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
	CPoint ptMenu = pt;
	::ClientToScreen(m_hWnd, &ptMenu);

	CPluginCustomEntryPtr pEntryNode = ap_dynamic_cast<CPluginCustomEntryPtr>(pItem);

	if (pEntryNode != NULL)
	{
		// Handle entry click...
		if (m_pEntryMenu != NULL)
		{
			if (m_pEntryMenu->IsBusy())
			{
				return;
			}

			m_pEntryMenu->DestroyMenu();
		}

		m_pEntryMenu = new CPluginCustomEntryMenu();

		m_pEntryMenu->CreateMenu(pEntryNode->GetEntry());
		m_pEntryMenu->ShowMenu(ptMenu);
		return;
	}
}


void CPluginCustomListCtrl::OnAddEntry(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	AddCustomListEntryImpl(pEntry);
}


void CPluginCustomListCtrl::OnRemoveEntry(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	CString strEntryName = FormatCustomEntryName(pEntry);

	if (m_pAllEntriesGroup != NULL)
	{
		RemoveEntry(strEntryName, static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->GetName());
	}
}


void CPluginCustomListCtrl::OnUpdateEntry(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	InvalidateRect(NULL);
	UpdateWindow();
}


LRESULT CPluginCustomListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	LRESULT lRet = CGroupListCtrl::OnCreate(lpCreateStruct);

	CString strAllEntriesGroupName;
	strAllEntriesGroupName.LoadString(IDS_PLUGIN_CUSTOM_LIST_MAIN_GROUP_NAME);

	m_pAllEntriesGroup = new CPluginCustomGroup();
	static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->SetName(strAllEntriesGroupName);
	static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->SetHeight(GetGroupHeight());
	
	if (m_pGroupElement != NULL)
	{
		m_pAllEntriesGroup->SetBackground(m_pGroupElement->GetBackground());
		m_pAllEntriesGroup->SetCollapsedImage(m_pGroupElement->GetCollapsedImage());
		m_pAllEntriesGroup->SetExpandedImage(m_pGroupElement->GetExpandedImage());
		m_pAllEntriesGroup->SetTextFont(m_pGroupElement->GetFont());
		m_pAllEntriesGroup->SetTextColor(m_pGroupElement->GetTextColor());
	}

	AddGroup(m_pAllEntriesGroup, FALSE);

	if (m_pPluginCustomList)
	{
		CAtlList<IUOLMessengerPluginCustomListEntryPtr> listEntries;
		m_pPluginCustomList->GetEntries(listEntries);

		for (POSITION pos = listEntries.GetHeadPosition(); pos; )
		{
			IUOLMessengerPluginCustomListEntryPtr pEntry = listEntries.GetNext(pos);
			AddCustomListEntryImpl(pEntry);
		}
	}

	return lRet;
}


void CPluginCustomListCtrl::OnDestroy()
{
	RemoveAll(FALSE);

	SetMsgHandled(FALSE);
}


int CPluginCustomListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CPluginCustomListCtrl::GetEntryHeight() const
{
	int nHeight = 0;

	if (m_pEntryElement != NULL)
	{
		nHeight = m_pEntryElement->GetHeight();
	}

	return nHeight;
}


CString CPluginCustomListCtrl::FormatCustomEntryName(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	CString strEntryName;
	strEntryName.Format(_T("%s/%s"), pEntry->GetEntryName(), pEntry->GetPluginInternalName());

	return strEntryName;
}


void CPluginCustomListCtrl::AddCustomListEntryImpl(IUOLMessengerPluginCustomListEntryPtr pEntry)
{
	CString strEntryName = FormatCustomEntryName(pEntry);

	if (m_pAllEntriesGroup != NULL)
	{
		CPluginCustomEntryPtr pCustomEntry = new CPluginCustomEntry(pEntry, m_pEntryElement);
		static_cast<CEntry*>(pCustomEntry.get())->SetName(strEntryName);
		static_cast<CEntry*>(pCustomEntry.get())->SetHeight(GetEntryHeight());

		AddEntry(m_pAllEntriesGroup, pCustomEntry);
	}
}