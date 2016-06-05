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
#include "GroupFindNode.h"

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../Image/Image.h"
#include "../InputDialog.h"
#include "../../controller/UIMApplication.h"
#include "../../resource.h"
#include "../FindContactPanel.h"


#define			COLLAPSER_LEFT_SHIFT_SPACE				7
#define			INNER_ELEMENTS_SHIFT_SPACE				5


#ifdef _LEAKDEBUG
	LONG CGroupFindNode::ms_nRefs = 0;
#endif

CGroupFindNode::CGroupFindNode(IUOLMessengerAccountPtr pAccount) :
	m_pAccount(pAccount),
	m_pDefaultPainter(NULL),
	m_pCurrentPainter(NULL),
	m_pSelectionPainter(NULL)
{
	m_pAccount->RegisterAccountObserver(this);
	
	if ( (CUIMApplication::GetApplication()) && 
			(CUIMApplication::GetApplication()->GetAccountConnectionManager()) )
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(
				m_pAccount, this);
	}
}


CGroupFindNode::~CGroupFindNode()
{
	m_pAccount->UnregisterAccountObserver(this);
	
	if ( (CUIMApplication::GetApplication()) && 
			(CUIMApplication::GetApplication()->GetAccountConnectionManager()) )
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(
				m_pAccount, this);
	}
}


IUOLMessengerAccountPtr CGroupFindNode::GetAccount() const
{
	return m_pAccount;
}


void CGroupFindNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CGroupFindNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CGroupFindNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CGroupFindNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CGroupFindNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CGroupFindNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CGroupFindNode::SetCollapsedImage(IUOLMessengerImagePtr pImageGroupCollapserCollapsed)
{
	m_pImageGroupCollapserCollapsed = pImageGroupCollapserCollapsed;
}


void CGroupFindNode::SetExpandedImage(IUOLMessengerImagePtr pImageGroupCollapserExpanded)
{
	m_pImageGroupCollapserExpanded = pImageGroupCollapserExpanded;
}


void CGroupFindNode::SetTextFont(CFontElementPtr pFont)
{
	m_pFontText = pFont;
}


void CGroupFindNode::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}


void CGroupFindNode::SetName(const CString& strName)
{
	// Ignore
}


CString CGroupFindNode::GetName() const
{
	return _T("__GroupFindNode__");
}

void CGroupFindNode::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}

BOOL CGroupFindNode::IsVisible() const
{
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
	
	return ((pContactListSettings2->IsShowGroupsEnabled()) && 
			(m_pFindContactPanel->IsFinding()));
}


int CGroupFindNode::Compare(const CEntryGroup* pEntryGroup) const
{
	return -1;
}


void CGroupFindNode::Paint(CDCHandle& dc)
{
	DrawCollapser(dc);
	DrawText(dc);
}


void CGroupFindNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CGroupFindNode::DrawBackground(CDCHandle& dc)
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


void CGroupFindNode::DrawCollapser(CDCHandle& dc)
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

		pImage->Draw(dcHandle, 7, y, w, h, pStrategy);
	}
}


void CGroupFindNode::OnNickNameChanged(IUOLMessengerAccount* pAccount)
{
	Invalidate();
}


void CGroupFindNode::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupFindNode::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupFindNode::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	Invalidate();
}


void CGroupFindNode::OnStatusChanged(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupFindNode::DrawText(CDCHandle& dc)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
    CString strName;
    strName = pStringLoader->GetValue("uol.resource.IDS_FIND_CONTACT_GROUP_NAME");

	CRect rectText(CPoint(0, 0), GetSize());
	
    IUOLMessengerImagePtr pCollapserImage = 
				(GetGroupState() == CEntryGroup::collapsed) ? m_pImageGroupCollapserCollapsed : m_pImageGroupCollapserExpanded;
		
	ATLASSERT(pCollapserImage);

	rectText.left = COLLAPSER_LEFT_SHIFT_SPACE + pCollapserImage->GetWidth() + 
				INNER_ELEMENTS_SHIFT_SPACE;
	
	CString strText;
	strText.Format(_T("%s"), strName);
	
	m_pFontText->DrawColoredText(dc, rectText, strText, m_clrText, 
			DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
}
