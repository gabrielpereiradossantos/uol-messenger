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
#include "ReadyForChatNode.h"

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../../controller/uimapplication.h"
#include "../../controller/UIManager.h"
#include "../../controller/Conversation/ConversationManager.h"
#include "../image/Image.h"
#include "../../resource.h"
#include "../tooltip/ContactTooltipBuilder.h"

#include <DataObject.h>
#include <DropSource.h>

#define			LEFT_SHIFT_SPACE					2
#define			RIGHT_SHIFT_SPACE					10
#define			INNER_ELEMENTS_SPACE				7
#define			STATUS_WIDTH_SPACE					16
#define			STATUS_HEIGHT_SPACE					16
#define			TOP_BOTTOM_SHIFT_SPACE				2


using namespace GroupListCtrl;



#ifdef _LEAKDEBUG
LONG CReadyForChatNode::ms_nRefs = 0;
#endif

CReadyForChatNode::CReadyForChatNode(IUOLMessengerContactPtr pContact, const CString& strChatKey) :	
m_pContact(pContact),
m_strName(pContact->GetName()),
m_pDefaultPainter(NULL),
m_pCurrentPainter(NULL),
m_pSelectionPainter(NULL),
m_clrTextNormal(RGB(0, 0, 0)),
m_clrTextSelected(RGB(0, 0, 0)),
m_strChatKey(strChatKey)
{
	m_pContact->RegisterObserver(this);

	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	IUOLMessengerUIManager* pUIManagerInterface = dynamic_cast<IUOLMessengerUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	ATLASSERT(pUIManager);

	IElementPtr pElement = const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__ContactTooltipBuilderInstance");
	CElementBuilderPtr pContactTooltipBuilderPtr = ap_dynamic_cast<CElementBuilderPtr>(pElement);

	m_pContactTooltip = ap_dynamic_cast<CContactTooltipPtr>(pContactTooltipBuilderPtr->CreateElement());

#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CReadyForChatNode::~CReadyForChatNode()
{
#ifdef _LEAKDEBUG
	::InterlockedDecrement(&ms_nRefs);

	ATLTRACE(_T("%s: %d - %s"), __FUNCTION__, ms_nRefs, GetName());
#endif

	m_pContact->UnregisterObserver(this);
}


IUOLMessengerContactPtr CReadyForChatNode::GetContact() const
{
	return m_pContact;
}


void CReadyForChatNode::SetComparatorAdapter(CComparatorAdapter* pComparator)
{
	m_pComparatorAdapter = pComparator;
}


CComparatorAdapter* CReadyForChatNode::GetComparatorAdapter() const
{
	return m_pComparatorAdapter;
}


void CReadyForChatNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CReadyForChatNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CReadyForChatNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;

		RestorePainter(pPainter->GetTimeout());
	}
}


IPainter* CReadyForChatNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CReadyForChatNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CReadyForChatNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CReadyForChatNode::SetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextNormal = pFont;
}


void CReadyForChatNode::SetSelectedTextFont(CFontElementPtr  pFont)
{
	m_pFontTextSelected = pFont;
}

void CReadyForChatNode::SetTextFontBlocked(CFontElementPtr  pFont)
{
	m_pFontContactBlocked = pFont;
}

void CReadyForChatNode::SetDropTargetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextDropTarget = pFont;
}


void CReadyForChatNode::SetTextColor(COLORREF clrText)
{
	m_clrTextNormal = clrText;
}


void CReadyForChatNode::SetTextColorBlocked(COLORREF clrText)
{
	m_clrTextBlocked = clrText;
}


void CReadyForChatNode::SetSelectedTextColor(COLORREF clrText)
{
	m_clrTextSelected = clrText;
}


void CReadyForChatNode::SetDropTargetTextColor(COLORREF clrText)
{
	m_clrTextDropTarget = clrText;
}


void CReadyForChatNode::SetAllContactsGroup(const CString& strAllContactsGroup)
{
	m_strAllContactsGroup = strAllContactsGroup;
}


CString CReadyForChatNode::GetAllContactsGroup() const
{
	return m_strAllContactsGroup;
}


void CReadyForChatNode::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CReadyForChatNode::GetName() const
{
	return m_strName;
}


BOOL CReadyForChatNode::IsVisible() const
{
    CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
    
    BOOL bVisible = FALSE;

    if (m_pContact)
    {
        if (pConversationManager)
        {
            bVisible = ((m_pContact->GetStatus() != IUOLMessengerContact::CS_OFFLINE) && (!pConversationManager->IsChatParticipant(m_pContact, m_strChatKey)));
        }
        else
        {
            bVisible = ((m_pContact->GetStatus() != IUOLMessengerContact::CS_OFFLINE));
        }
    }
    
	return bVisible;
}


int CReadyForChatNode::Compare(const CEntry* pEntry) const
{
	const CReadyForChatNode* pReadyForChatNode = dynamic_cast<const CReadyForChatNode*>(pEntry);

	if (pReadyForChatNode && m_pComparatorAdapter)
	{
		return m_pComparatorAdapter->Compare(this->GetContact().get(), pReadyForChatNode->GetContact().get());
	}

	return CEntry::Compare(pEntry);
}


void CReadyForChatNode::Paint(CDCHandle& dc)
{
	DrawStatus(dc);
    DrawText(dc);
}


void CReadyForChatNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CReadyForChatNode::DrawBackground(CDCHandle& dc)
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


void CReadyForChatNode::OnLButtonDown(const CPoint& pt)
{
    IDataObject *pDataObject;
    IDropSource *pDropSource;

    DWORD		 dwEffect;
    DWORD		 dwResult;

    IUOLMessengerContactPtr pContact = GetContact();

    if (!pContact.get())
    {
        return;
    }

    FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };

    stgmed.hGlobal = ::GlobalAlloc(GHND, sizeof(IUOLMessengerContact*));
    IUOLMessengerContact** ppTransferedContact = (IUOLMessengerContact**) ::GlobalLock(stgmed.hGlobal);
    *ppTransferedContact = pContact.get();

    ::GlobalUnlock(stgmed.hGlobal);

    pDropSource = new CDropSource();
    pDataObject = new CDataObject(&fmtetc, &stgmed, 1);

    if (pDropSource && pDataObject)
    {
        dwResult = ::DoDragDrop(pDataObject, pDropSource, DROPEFFECT_MOVE, &dwEffect);
    }

    if (dwResult == DRAGDROP_S_DROP)
    {
        if (dwEffect & DROPEFFECT_MOVE)
        {
            ATLTRACE("Dropped result = %d\n", dwResult);
        }
    }

    pDataObject->Release();
    pDropSource->Release();
}


void CReadyForChatNode::OnLButtonUp(const CPoint& pt)
{
}


void CReadyForChatNode::OnLBtnDblClick(const CPoint& pt)
{
}


void CReadyForChatNode::OnRButtonDown(const CPoint& pt)
{
}


void CReadyForChatNode::OnRBtnDblClick(const CPoint& pt)
{
}


void CReadyForChatNode::OnMouseMove(const CPoint& pt)
{
}


void CReadyForChatNode::OnMouseHover(const CPoint& pt)
{
    m_pContactTooltip->SetContact(m_pContact);
    m_pContactTooltip->Create(*m_pGroupListCtrl, NULL, "", WS_POPUP);

    if (m_pContactTooltip->IsWindow())
    {
        CPoint ptTooltipPos;
        ptTooltipPos.x = pt.x;
        ptTooltipPos.y = pt.y;

        ::ClientToScreen(static_cast<HWND>(*m_pGroupListCtrl), &ptTooltipPos);

        m_pContactTooltip->Show(static_cast<HWND>(*m_pGroupListCtrl), ptTooltipPos);
    }
}

void CReadyForChatNode::OnMouseLeave()
{
	if(m_pContactTooltip->IsWindow())
	{
		m_pContactTooltip->DestroyWindow();
	}
}

void CReadyForChatNode::OnReturnKeyPressed()
{
}


void CReadyForChatNode::OnTimer(UINT nId)
{
	if (CGroupListCtrl::SYSTEM_TIMER == nId)
	{
		time_t curtime;
		time(&curtime);

		if (m_timeRestore != 0 && curtime >= m_timeRestore)
		{
			m_timeRestore = 0;

			m_pCurrentPainter = m_pDefaultPainter;

			if (m_pCurrentPainter != NULL)
			{
				Invalidate();
			}
		}
	}
}


void CReadyForChatNode::Update(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CReadyForChatNode::UpdateAlias(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CReadyForChatNode::UpdateStatus(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CReadyForChatNode::UpdateIcon(IUOLMessengerContact* pContact)
{
	ATLASSERT(m_pGroupListCtrl);

	Invalidate();
	Redraw();
}


void CReadyForChatNode::RestorePainter(int nTimeout)
{
	time_t curtime;
	time(&curtime);
	m_timeRestore = curtime + nTimeout;
}


void CReadyForChatNode::DrawText(CDCHandle& dc)
{	
	IUOLMessengerAccountPtr pAccount = m_pContact->GetAccount();
	CFontElementPtr pFont;
	COLORREF clrText;

	if (IsActiveDropTarget())
	{
		pFont = m_pFontTextDropTarget;
		clrText = m_clrTextDropTarget;
	}
	else if (FALSE == GetSelected())
	{
		pFont = m_pFontTextNormal;
		clrText = m_clrTextNormal;
	}
	else
	{	
		pFont = m_pFontTextSelected;
		clrText = m_clrTextSelected;
	}

	if (pAccount != NULL) 
	{
		if (CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(pAccount, m_pContact->GetName()))
		{
			pFont = m_pFontContactBlocked;
			clrText =  m_clrTextBlocked;
		}
	}

	ATLASSERT(pFont);

	CString strName = m_pContact->GetAlias();

	if (strName.IsEmpty())
	{
		strName = m_pContact->GetName();
	}

	CRect rectText;
	GetRectForElement(IUOLMessengerContactListSettings2::ALIAS_ELEMENT, rectText);

	pFont->DrawColoredText(dc, rectText, strName, clrText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
}	

void CReadyForChatNode::DrawStatus(CDCHandle& dc)
{
	IUOLMessengerImagesProtocolPtr pImagesProtocol = NULL;
	IUOLMessengerImagePtr pImage = NULL;
	
	if (m_pContact.get())
	{
		pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
	}
	
	if (pImagesProtocol)
	{
		pImage = pImagesProtocol->GetContactStatusImage(m_pContact);
	}

	if (pImage)
	{
		GetRectForElement(IUOLMessengerContactListSettings2::STATUS_ELEMENT, m_rectStatus);

		IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();

		CDCHandle dcHandle(dc);

		pImage->Draw(dcHandle, 
			m_rectStatus.left, m_rectStatus.top,
			m_rectStatus.Width(), m_rectStatus.Height(), 
			pStrategy);

		// Draw extra status icon...
		IUOLMessengerImagePtr pExtraIcon;

		// Try blocked icon
		IUOLMessengerAccountPtr pAccount = m_pContact->GetAccount();
		IUOLMessengerPrivacyManagerPtr pPrivacyManager = CUIMApplication::GetApplication()->GetPrivacyManager();

		if ((pAccount != NULL) && 
				(pPrivacyManager != NULL) && 
				(pPrivacyManager->IsDenied(pAccount, m_pContact->GetName())))
		{	
			pExtraIcon = pImagesProtocol->GetContactBlockedImage(pAccount->GetProtocolId());
		}

		// Try not authorized icon
		if (!pExtraIcon)
		{
			CIMProtocol* pProtocol = pAccount->GetProtocol();
			IUOLMessengerRemoteContactListPtr pRemoteContactList = 
					CUIMApplication::GetApplication()->GetRemoteContactList();

			IUOLMessengerContact3Ptr pContact3 = 
					ap_dynamic_cast<IUOLMessengerContact3Ptr>(m_pContact);
			
			if ((pProtocol) && 
					(pProtocol->HasReRequestAuthorizationSupport()) && 
					(pRemoteContactList) && (pContact3) && 
					(pContact3->CanRequestAuthorization()))
			{
				pExtraIcon = pImagesProtocol->GetContactAuthorizationIcon(pAccount->GetProtocolId(), FALSE);
			}
		}

		if (pExtraIcon != NULL)
		{
			pExtraIcon->Draw(dcHandle, 
				m_rectStatus.left, m_rectStatus.top,
				m_rectStatus.Width(), m_rectStatus.Height(), 
				pStrategy);

		}
	}
}


void CReadyForChatNode::UpdateImpl()
{
	ATLASSERT(m_pGroupListCtrl);
	ATLASSERT(m_pContact);

	if (m_pGroupListCtrl && m_pContact)
	{
		IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
				ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
						CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
		
		if (!pContactListSettings2->GetGroupByAccounts())
		{
			m_pGroupListCtrl->SortGroupEntries(m_pContact->GetGroup()->GetName());
		}
		else
		{
			CString strName;
            strName.Format(_T("%s:%s"), m_pContact->GetAccount()->GetProtocolId(), 
                    m_pContact->GetAccount()->GetUserId());
            m_pGroupListCtrl->SortGroupEntries(strName);
		}
	}

	ATLASSERT(!m_strAllContactsGroup.IsEmpty());

	if (m_pGroupListCtrl && !m_strAllContactsGroup.IsEmpty())
	{
		m_pGroupListCtrl->SortGroupEntries(m_strAllContactsGroup);
	}
}

void CReadyForChatNode::RegisterObserver(CUOLMessengerReadyForChatNodeObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listObservers.Find(pObserver);

		if (!pos)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CReadyForChatNode::UnregisterObserver(CUOLMessengerReadyForChatNodeObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listObservers.Find(pObserver);

		if (pos)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}

void CReadyForChatNode::NotifyObservers(void (CUOLMessengerReadyForChatNodeObserver::*pfnCallback)(IUOLMessengerReadyForChatNode*, IUOLMessengerImageButton2Ptr), IUOLMessengerImageButton2Ptr pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerReadyForChatNodeObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(this, pButton);
	}

}

void CReadyForChatNode::AddCustomButton(IUOLMessengerImageButton2Ptr pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	m_listButtons.AddHead(pButton);

	Invalidate();
}

void CReadyForChatNode::RemoveCustomButton(IUOLMessengerImageButton2Ptr pButton)
{
	if (pButton!= NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listButtons.Find(pButton);

		if (pos)
		{
			m_listButtons.RemoveAt(pos);
			Invalidate();
		}
	}
}

void CReadyForChatNode::AddCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	m_listMenus.AddHead(pCustomMenuItem);
}

void CReadyForChatNode::RemoveCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem)
{
	if (pCustomMenuItem != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listMenus.Find(pCustomMenuItem);

		if (pos)
		{
			m_listMenus.RemoveAt(pos);
		}
	}
}


void CReadyForChatNode::GetRectForElement(IUOLMessengerContactListSettings2::ContactNodeElements nContactElement,
                                          CRect& rElementRect)
{
	CRect contactNodeRect(CPoint(0, 0), GetSize());
	
	// Initialize returning rect.
	rElementRect.SetRect(0, 0, 0, 0);
	
	if (nContactElement == IUOLMessengerContactListSettings2::ALIAS_ELEMENT)
	{
		rElementRect = contactNodeRect;
		rElementRect.left += LEFT_SHIFT_SPACE;
		rElementRect.right -= RIGHT_SHIFT_SPACE;
		
		rElementRect.left += (STATUS_WIDTH_SPACE + INNER_ELEMENTS_SPACE);
	}
	else if (nContactElement == IUOLMessengerContactListSettings2::STATUS_ELEMENT)
	{
        rElementRect = contactNodeRect;

        int nOffset = (rElementRect.Height() - STATUS_HEIGHT_SPACE) / 2;

        rElementRect.top += nOffset;
        rElementRect.bottom -= nOffset;
        rElementRect.left = LEFT_SHIFT_SPACE;
        rElementRect.right = rElementRect.left + rElementRect.Height();
	}
}

void CReadyForChatNode::GetMenuItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens)
{
	listItens.RemoveAll();
	listItens.AddTailList(&m_listMenus);
}
