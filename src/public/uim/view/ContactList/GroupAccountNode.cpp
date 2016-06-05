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
#include "GroupAccountNode.h"

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../Image/Image.h"
#include "../InputDialog.h"
#include "../../controller/UIMApplication.h"
#include "../../resource.h"
#include "../FindContactPanel.h"



#define			COLLAPSER_LEFT_SHIFT_SPACE				7
#define			INNER_ELEMENTS_SHIFT_SPACE				5


#ifdef _LEAKDEBUG
	LONG CGroupAccountNode::ms_nRefs = 0;
#endif

CGroupAccountNode::CGroupAccountNode(IUOLMessengerAccountPtr pAccount) :
	CGroupNode(NULL),
	m_pAccount(pAccount)
{
	m_pAccount->RegisterAccountObserver(this);
	
	if ( (CUIMApplication::GetApplication()) && 
			(CUIMApplication::GetApplication()->GetAccountConnectionManager()) )
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(
				m_pAccount, this);
	}
	
#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CGroupAccountNode::~CGroupAccountNode()
{
#ifdef _LEAKDEBUG
	::InterlockedDecrement(&ms_nRefs);
	
	ATLTRACE(_T("%s: %d - %s"), __FUNCTION__, ms_nRefs, GetName());
#endif
	
	m_pAccount->UnregisterAccountObserver(this);
	
	if ( (CUIMApplication::GetApplication()) && 
			(CUIMApplication::GetApplication()->GetAccountConnectionManager()) )
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(
				m_pAccount, this);
	}
}


IUOLMessengerAccountPtr CGroupAccountNode::GetAccount() const
{
	return m_pAccount;
}


CString CGroupAccountNode::GetName() const
{
	CString strName;
    strName.Format(_T("%s:%s"), m_pAccount->GetProtocolId(), m_pAccount->GetUserId());

    return strName;
}

void CGroupAccountNode::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}


BOOL CGroupAccountNode::IsVisible() const
{
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
	
	BOOL bAccountExists = 
			CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
					m_pAccount->GetUserId(), m_pAccount->GetProtocolId());
	
	return ( (bAccountExists) && (pContactListSettings2->IsShowGroupsEnabled()) && 
			(pContactListSettings2->GetGroupByAccounts()) && (!m_pFindContactPanel->IsFinding()));
}


int CGroupAccountNode::Compare(const CEntryGroup* pEntryGroup) const
{
	const CGroupAccountNode* pGroupAccountNode = dynamic_cast<const CGroupAccountNode*>(pEntryGroup);
	
	if (pGroupAccountNode && m_pComparatorAdapter)
	{
		return m_pComparatorAdapter->Compare(this->GetAccount().get(), pGroupAccountNode->GetAccount().get());
	}
	
	// it should be allways first group
    return -1; 
    //return CEntryGroup::Compare(pEntryGroup);
}


void CGroupAccountNode::Paint(CDCHandle& dc)
{
	DrawCollapser(dc);
	DrawStatus(dc);
	DrawText(dc);
}


void CGroupAccountNode::OnNickNameChanged(IUOLMessengerAccount* pAccount)
{
	Invalidate();
}


void CGroupAccountNode::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupAccountNode::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupAccountNode::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupAccountNode::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	Invalidate();
}


void CGroupAccountNode::OnStatusChanged(IUOLMessengerAccountPtr pAccount)
{
	Invalidate();
}


void CGroupAccountNode::DrawStatus(CDCHandle& dc)
{
	IUOLMessengerImagesProtocolPtr pImagesProtocol = 
			CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
	
	DWORD dwStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(m_pAccount);
	
	IUOLMessengerImagePtr pStatusImage = pImagesProtocol->GetUserStatusImage(
			m_pAccount->GetProtocolId(), dwStatus);
	
	ATLASSERT(pStatusImage);
	
	if (pStatusImage)
	{
		IUOLMessengerImagePtr pCollapserImage = 
				(GetGroupState() == CEntryGroup::collapsed) ? m_pImageGroupCollapserCollapsed : m_pImageGroupCollapserExpanded;
		
		ATLASSERT(pCollapserImage);
		
		m_rectStatus.left = COLLAPSER_LEFT_SHIFT_SPACE + pCollapserImage->GetWidth() + 
				INNER_ELEMENTS_SHIFT_SPACE;
		m_rectStatus.right = m_rectStatus.left + pStatusImage->GetWidth();
		m_rectStatus.top = 0;
		m_rectStatus.bottom = pStatusImage->GetHeight();
		
		CDCHandle dcHandle(dc);
		
		int yCoord = (GetSize().cy - m_rectStatus.Height()) / 2;
		
		IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();
		
		pStatusImage->Draw(dcHandle, m_rectStatus.left, yCoord, 
				m_rectStatus.Width(), m_rectStatus.Height(), pStrategy);
	}
}


void CGroupAccountNode::DrawText(CDCHandle& dc)
{
	BOOL bDropTarget = IsActiveDropTarget();
	
	CFontElementPtr pFont = bDropTarget ? m_pFontTextDropTarget : m_pFontText;
	
	ATLASSERT(pFont);
	
	CString strName = m_pAccount->GetUserId();
	CRect rectText(CPoint(0, 0), GetSize());
	
	rectText.left = m_rectStatus.right + INNER_ELEMENTS_SHIFT_SPACE;
	
	CString strText;
	//strText.Format(_T("%s (%d/%d)"), strName, m_pGroup->GetConnectedContactCount(), m_pGroup->GetContactCount());
	strText.Format(_T("%s"), strName);
	
	pFont->DrawColoredText(dc, rectText, strText, 
			bDropTarget ? m_clrTextDropTarget : m_clrText, 
			DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
}

