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
#include "EntryGroup.h"

#include "GroupListCtrl.h"

namespace GroupListCtrl {

struct EntryHelper
{
	CEntryPtr pEntry;
};


CEntryGroup::CEntryGroup() : CGroupListCtrlItem(),
		m_nState(expanded), 
		m_pListEntries(new CEntryList())
{
}

CEntryGroup::~CEntryGroup()
{
}

CEntryGroup::GroupState CEntryGroup::GetGroupState() const
{
	return m_nState;
}

void CEntryGroup::SetGroupState(CEntryGroup::GroupState nState)
{
	m_nState = nState;
}

CEntryGroup::GroupState  CEntryGroup::InvertGroupState()
{
	GroupState oldState = m_nState;

	if (m_nState == collapsed)
	{
		m_nState = expanded;
	}
	else
	{
		m_nState = collapsed;
	}

	if (m_pGroupListCtrl) 
	{
		m_pGroupListCtrl->UpdateScrollSize();
	}	

	return oldState;
}

int CEntryGroup::Compare(const CEntryGroup* pEntryGroup) const
{
	if (pEntryGroup != NULL)
	{
		return GetName().CompareNoCase(pEntryGroup->GetName());
	}

	return 0;
}

void CEntryGroup::Sort()
{
	int nEntryCount = (int) m_pListEntries->GetCount();

	if (nEntryCount > 0)
	{
		EntryHelper* pEntryHelper;
		int nInd = 0;
		EntryHelper** ppEntries = new EntryHelper* [nEntryCount];

		while (m_pListEntries->GetCount() > 0)
		{
			pEntryHelper = new EntryHelper;
			pEntryHelper->pEntry = m_pListEntries->RemoveHead();
			ppEntries[nInd] = pEntryHelper;
			nInd++;
		}

		ATLASSERT(nInd == nEntryCount);

		::qsort(ppEntries, nEntryCount, sizeof(EntryHelper*), CompareEntries);

		for (nInd = 0; nInd < nEntryCount; nInd++)
		{
			pEntryHelper = ppEntries[nInd];
			m_pListEntries->AddTail(pEntryHelper->pEntry);
			delete pEntryHelper;
		}

		ATLASSERT(nInd == nEntryCount);

		delete ppEntries;
	}
}

void CEntryGroup::Paint(CDCHandle& dc)
{
	CBrush brush;
	brush.CreateSolidBrush(RGB(128, 128, 255));

	CRect rect = GetRect();
	rect.bottom = rect.Height();
	rect.right = rect.Width();
	rect.left =  rect.top = 0;

	dc.FillRect(&rect, brush);
	dc.TextOut(rect.left, rect.top, GetName());	
}

void CEntryGroup::OnLButtonDown(const CPoint& pt)
{
	InvertGroupState();		
}

void CEntryGroup::OnLBtnDblClick(const CPoint& pt)
{
	
}

CEntryListPtr CEntryGroup::GetEntryList() const
{
	return m_pListEntries;
}

void CEntryGroup::AddEntry(CEntryPtr pEntry)
{
	ATLASSERT(m_pListEntries);

	if (m_pListEntries)
	{
		m_pListEntries->AddTail(pEntry);
	}	
}

void CEntryGroup::RemoveEntry(CEntryPtr pEntry)
{
	ATLASSERT(m_pListEntries);

	if (m_pListEntries)
	{
		POSITION pos = m_pListEntries->Find(pEntry);

		if (pos)
		{
			m_pListEntries->RemoveAt(pos);
		}
	}
}

void CEntryGroup::RemoveEntry(const CString& strName)
{
	ATLASSERT(m_pListEntries);

	if (m_pListEntries)
	{
		for (POSITION pos = m_pListEntries->GetHeadPosition(); pos; m_pListEntries->GetNext(pos))
		{
			CEntryPtr pEntry = m_pListEntries->GetAt(pos);

			if (0 == strName.Compare(pEntry->GetName()))
			{
				m_pListEntries->RemoveAt(pos);
				return;
			}
		}
	}
}

void CEntryGroup::RemoveAllEntries()
{
	ATLASSERT(m_pListEntries);

	if (m_pListEntries)
	{
		m_pListEntries->RemoveAll();
	}
}

void CEntryGroup::OnReturnKeyPressed()
{
	InvertGroupState();
}

void CEntryGroup::OnKeyDown(TCHAR ch)
{
	switch (ch)
	{	
	case VK_LEFT:
		SetGroupState(CEntryGroup::collapsed);
		Redraw();
		break;	
	
	case VK_RIGHT:
		SetGroupState(CEntryGroup::expanded);
		Redraw();
		break;

	default:
		__super::OnKeyDown(ch);
	}		
}

int CEntryGroup::CompareEntries(const void* pEntry1, const void* pEntry2)
{
	EntryHelper* pEntryHelper1 = * ((EntryHelper**) pEntry1);
	EntryHelper* pEntryHelper2 = * ((EntryHelper**) pEntry2);

	return (pEntryHelper1->pEntry)->Compare((pEntryHelper2->pEntry).get());
}

void CEntryGroup::SetValidDropTarget(BOOL bValid)
{
	m_bValidDropTarget = bValid;

	for (POSITION pos = m_pListEntries->GetHeadPosition(); pos;)
	{
		CEntryPtr pEntry = m_pListEntries->GetNext(pos);

		if (pEntry.get())
		{
			pEntry->SetValidDropTarget(bValid);
		}
	}
}

}