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
#include "GroupNode.h"

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../Image/Image.h"
#include "../InputDialog.h"
#include "../../controller/UIMApplication.h"
#include "../../resource.h"
#include "../FindContactPanel.h"


#ifdef _LEAKDEBUG
	LONG CGroupNode::ms_nRefs = 0;
#endif

CGroupNode::CGroupNode(IUOLMessengerGroupPtr pGroup) :
	CEntryGroup(),
	m_pGroup(pGroup),
	m_pComparatorAdapter(NULL),
	m_pDefaultPainter(NULL),
	m_pCurrentPainter(NULL),
	m_pSelectionPainter(NULL)
{
	if (m_pGroup)
	{
		m_pGroup->RegisterObserver(this);
	}

#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CGroupNode::~CGroupNode()
{
#ifdef _LEAKDEBUG
	::InterlockedDecrement(&ms_nRefs);

	ATLTRACE(_T("%s: %d - %s"), __FUNCTION__, ms_nRefs, GetName());
#endif

	if (m_pGroup)
	{
		m_pGroup->UnregisterObserver(this);
	}
}


IUOLMessengerGroupPtr CGroupNode::GetGroup() const
{
	return m_pGroup;
}


void CGroupNode::SetComparatorAdapter(CComparatorAdapter* pComparator)
{
	m_pComparatorAdapter = pComparator;
}


CComparatorAdapter* CGroupNode::GetComparatorAdapter() const
{
	return m_pComparatorAdapter;
}


void CGroupNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CGroupNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CGroupNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CGroupNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CGroupNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CGroupNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CGroupNode::SetCollapsedImage(IUOLMessengerImagePtr pImageGroupCollapserCollapsed)
{
	m_pImageGroupCollapserCollapsed = pImageGroupCollapserCollapsed;
}


void CGroupNode::SetExpandedImage(IUOLMessengerImagePtr pImageGroupCollapserExpanded)
{
	m_pImageGroupCollapserExpanded = pImageGroupCollapserExpanded;
}


void CGroupNode::SetTextFont(CFontElementPtr pFont)
{
	m_pFontText = pFont;
}


void CGroupNode::SetDropTargetTextFont(CFontElementPtr pFont)
{
	m_pFontTextDropTarget = pFont;
}


void CGroupNode::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}


void CGroupNode::SetDropTargetTextColor(COLORREF clrText)
{
	m_clrTextDropTarget = clrText;
}


void CGroupNode::SetName(const CString& strName)
{
	m_pGroup->SetName(strName);
}


CString CGroupNode::GetName() const
{
	return m_pGroup->GetName();
}

void CGroupNode::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}


BOOL CGroupNode::IsVisible() const
{
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
	
	return ( (pContactListSettings2->IsShowGroupsEnabled()) && (!pContactListSettings2->GetGroupByAccounts()) 
          && (!m_pFindContactPanel->IsFinding()));
}


int CGroupNode::Compare(const CEntryGroup* pEntryGroup) const
{
	const CGroupNode* pGroupNode = dynamic_cast<const CGroupNode*>(pEntryGroup);
	
	if (pGroupNode && m_pComparatorAdapter)
	{
		return m_pComparatorAdapter->Compare(this->GetGroup().get(), pGroupNode->GetGroup().get());
	}

	return CEntryGroup::Compare(pEntryGroup);
}


void CGroupNode::Paint(CDCHandle& dc)
{
	DrawCollapser(dc);
	DrawText(dc);
}


void CGroupNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CGroupNode::DrawBackground(CDCHandle& dc)
{
	IPainter* pPainter = m_pCurrentPainter;

	if (GetSelected() && m_pSelectionPainter != NULL)
	{
		pPainter = m_pSelectionPainter;
	}

	if (pPainter != NULL)
	{
		CRect rect(CPoint(0, 0), GetSize());
		CMemDC dcMem(dc, &rect);

		pPainter->Draw(CDCHandle(dcMem), rect);
	}
}


void CGroupNode::Update(IUOLMessengerGroup* pGroup)
{
	m_pGroupListCtrl->SortGroups();
}


void CGroupNode::DrawCollapser(CDCHandle& dc)
{
	IUOLMessengerImagePtr pImage = (GetGroupState() == CEntryGroup::collapsed) ? m_pImageGroupCollapserCollapsed : m_pImageGroupCollapserExpanded;
	ATLASSERT(pImage);

	if (pImage)
	{
		CRect rect(CPoint(0, 0), GetSize());

		int w = pImage->GetWidth();
		int h = pImage->GetHeight();

		CDCHandle dcHandle(dc);

		int y = (rect.Height() - h) / 2;

		IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();

		pImage->Draw(dcHandle, 
				7, y, 
				w, h, 
				pStrategy);
	}
}


void CGroupNode::DrawText(CDCHandle& dc)
{
	BOOL bDropTarget = IsActiveDropTarget();

	CFontElementPtr pFont = bDropTarget ? m_pFontTextDropTarget : m_pFontText;

	ATLASSERT(pFont);

	CString strName = GetName();
	CRect rectText(CPoint(0, 0), GetSize());

	IUOLMessengerImagePtr pImage = (GetGroupState() == CEntryGroup::collapsed) ? m_pImageGroupCollapserCollapsed : m_pImageGroupCollapserExpanded;
	ATLASSERT(pImage);
	rectText.left = 7 + pImage->GetWidth() + 5;

	CString strText;
	strText.Format(_T("%s (%d/%d)"), strName, m_pGroup->GetConnectedContactCount(), m_pGroup->GetContactCount());

	pFont->DrawColoredText(dc, rectText, strText, 
		bDropTarget ? m_clrTextDropTarget : m_clrText, 
			DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
}