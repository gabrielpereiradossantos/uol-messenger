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
#include "ChatInviteNode.h"

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../../controller/uimapplication.h"
#include "../../controller/UIManager.h"
#include "../../controller/Conversation/ConversationManager.h"
#include "../image/Image.h"
#include "../../resource.h"
#include "../tooltip/ContactTooltipBuilder.h"

#define	LEFT_SHIFT_SPACE		2
#define	RIGHT_SHIFT_SPACE		10
#define	INNER_ELEMENTS_SPACE	7
#define	STATUS_WIDTH_SPACE		16
#define	STATUS_HEIGHT_SPACE		16
#define	TOP_BOTTOM_SHIFT_SPACE	2
#define CHECKBOX_WIDTH			13
#define CHECKBOX_HEIGHT			13


using namespace GroupListCtrl;


CChatInviteNode::CChatInviteNode(IUOLMessengerContactPtr pContact, 
								 IUOLMessengerImagePtr pCheckOnImage,
								 IUOLMessengerImagePtr pCheckOffImage) :
m_pContact(pContact),
m_strName(pContact->GetName()),
m_pDefaultPainter(NULL),
m_pCurrentPainter(NULL),
m_pSelectionPainter(NULL),
m_clrTextNormal(RGB(0, 0, 0)),
m_clrTextSelected(RGB(0, 0, 0)),
m_bIsChecked(FALSE)
{
	// Create CheckOn button
	m_pCheckOnButton = new CImageButton();
	m_pCheckOnButton->SetImage(pCheckOnImage);
	m_pCheckOnButton->SetWidth(CHECKBOX_WIDTH);
	m_pCheckOnButton->SetDrawTransparent(FALSE);
	m_pCheckOnButton->SetCommandName("CheckOn");
	m_pCheckOnButton->AddEventListener(this);
	m_pCheckOnButton->SetVisible(m_bIsChecked);

	// Create CheckOff button
	m_pCheckOffButton = new CImageButton();
	m_pCheckOffButton->SetImage(pCheckOffImage);
	m_pCheckOffButton->SetWidth(CHECKBOX_WIDTH);
	m_pCheckOffButton->SetDrawTransparent(FALSE);
	m_pCheckOffButton->SetCommandName("CheckOff");
	m_pCheckOffButton->AddEventListener(this);
	m_pCheckOffButton->SetVisible(!m_bIsChecked);

	m_pContact->RegisterObserver(this);

	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	IUOLMessengerUIManager* pUIManagerInterface = dynamic_cast<IUOLMessengerUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	ATLASSERT(pUIManager);

	IElementPtr pElement = const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__ContactTooltipBuilderInstance");
	CElementBuilderPtr pContactTooltipBuilderPtr = ap_dynamic_cast<CElementBuilderPtr>(pElement);

	m_pContactTooltip = ap_dynamic_cast<CContactTooltipPtr>(pContactTooltipBuilderPtr->CreateElement());
}


CChatInviteNode::~CChatInviteNode()
{
	if (m_pCheckOnButton)
	{
		m_pCheckOnButton->RemoveEventListener(this);
	}

	if (m_pCheckOffButton)
	{
		m_pCheckOffButton->RemoveEventListener(this);
	}

	m_pContact->UnregisterObserver(this);
}


IUOLMessengerContactPtr CChatInviteNode::GetContact() const
{
	return m_pContact;
}


void CChatInviteNode::SetComparatorAdapter(CComparatorAdapter* pComparator)
{
	m_pComparatorAdapter = pComparator;
}


CComparatorAdapter* CChatInviteNode::GetComparatorAdapter() const
{
	return m_pComparatorAdapter;
}


void CChatInviteNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CChatInviteNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CChatInviteNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;

		RestorePainter(pPainter->GetTimeout());
	}
}


IPainter* CChatInviteNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CChatInviteNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CChatInviteNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CChatInviteNode::SetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextNormal = pFont;
}


void CChatInviteNode::SetSelectedTextFont(CFontElementPtr  pFont)
{
	m_pFontTextSelected = pFont;
}

void CChatInviteNode::SetTextFontBlocked(CFontElementPtr  pFont)
{
	m_pFontContactBlocked = pFont;
}

void CChatInviteNode::SetDropTargetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextDropTarget = pFont;
}


void CChatInviteNode::SetTextColor(COLORREF clrText)
{
	m_clrTextNormal = clrText;
}


void CChatInviteNode::SetTextColorBlocked(COLORREF clrText)
{
	m_clrTextBlocked = clrText;
}


void CChatInviteNode::SetSelectedTextColor(COLORREF clrText)
{
	m_clrTextSelected = clrText;
}


void CChatInviteNode::SetDropTargetTextColor(COLORREF clrText)
{
	m_clrTextDropTarget = clrText;
}


void CChatInviteNode::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}


void CChatInviteNode::SetAllContactsGroup(const CString& strAllContactsGroup)
{
	m_strAllContactsGroup = strAllContactsGroup;
}


CString CChatInviteNode::GetAllContactsGroup() const
{
	return m_strAllContactsGroup;
}


void CChatInviteNode::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CChatInviteNode::GetName() const
{
	return m_strName;
}


BOOL CChatInviteNode::IsVisible() const
{
    BOOL bVisible;

    if (m_pFindContactPanel->IsFinding())
    {
        bVisible = ((m_pContact->GetStatus() != IUOLMessengerContact::CS_OFFLINE) && IsFindContactMatch());
    }
    else
    {
        bVisible = (m_pContact->GetStatus() != IUOLMessengerContact::CS_OFFLINE);
    }

	return bVisible;
}


int CChatInviteNode::Compare(const CEntry* pEntry) const
{
	const CChatInviteNode* pChatInviteNode = dynamic_cast<const CChatInviteNode*>(pEntry);

	if (pChatInviteNode && m_pComparatorAdapter)
	{
		return m_pComparatorAdapter->Compare(this->GetContact().get(), pChatInviteNode->GetContact().get());
	}

	return CEntry::Compare(pEntry);
}


void CChatInviteNode::Paint(CDCHandle& dc)
{
	DrawCheckbox(dc);
    DrawText(dc);	
}


void CChatInviteNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CChatInviteNode::DrawBackground(CDCHandle& dc)
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


void CChatInviteNode::OnLButtonDown(const CPoint& pt)
{
    UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseClick);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseClick);

	if ((m_pCheckOnButton != NULL) && (m_pCheckOnButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOnButton))
		{
			m_pCheckOnButton->OnLButtonDown(pt);
		}
	}
	else if ((m_pCheckOffButton != NULL) && (m_pCheckOffButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOffButton))
		{
			m_pCheckOffButton->OnLButtonDown(pt);
		}
	}
}


void CChatInviteNode::OnLButtonUp(const CPoint& pt)
{
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseOut);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseOut);

	if ((m_pCheckOnButton != NULL) && (m_pCheckOnButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOnButton))
		{
			m_pCheckOnButton->OnLButtonUp(pt);
		}
	}
	else if ((m_pCheckOffButton != NULL) && (m_pCheckOffButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOffButton))
		{
			m_pCheckOffButton->OnLButtonUp(pt);
		}
	}
}


void CChatInviteNode::OnLBtnDblClick(const CPoint& pt)
{
}


void CChatInviteNode::OnRButtonDown(const CPoint& pt)
{
}


void CChatInviteNode::OnRBtnDblClick(const CPoint& pt)
{
}


void CChatInviteNode::OnMouseMove(const CPoint& pt)
{
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseOver);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseOver);

	if ((m_pCheckOnButton != NULL) && (m_pCheckOnButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOnButton))
		{
			m_pCheckOnButton->OnMouseMove(pt);
		}
	}

	if ((m_pCheckOffButton != NULL) && (m_pCheckOffButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOffButton))
		{
			m_pCheckOffButton->OnMouseMove(pt);
		}
	}
}


void CChatInviteNode::OnMouseHover(const CPoint& pt)
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

void CChatInviteNode::OnMouseLeave()
{
	CPoint pt;
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseOut);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseOut);
	
	if(m_pContactTooltip->IsWindow())
	{
		m_pContactTooltip->DestroyWindow();
	}
}

void CChatInviteNode::OnReturnKeyPressed()
{
}


void CChatInviteNode::OnTimer(UINT nId)
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


void CChatInviteNode::Update(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CChatInviteNode::UpdateAlias(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CChatInviteNode::UpdateStatus(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CChatInviteNode::UpdateIcon(IUOLMessengerContact* pContact)
{
	ATLASSERT(m_pGroupListCtrl);

	Invalidate();
	Redraw();
}


void CChatInviteNode::RestorePainter(int nTimeout)
{
	time_t curtime;
	time(&curtime);
	m_timeRestore = curtime + nTimeout;
}


void CChatInviteNode::DrawText(CDCHandle& dc)
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


void CChatInviteNode::UpdateImpl()
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

void CChatInviteNode::RegisterObserver(CUOLMessengerChatInviteNodeObserver* pObserver)
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


void CChatInviteNode::UnregisterObserver(CUOLMessengerChatInviteNodeObserver* pObserver)
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

void CChatInviteNode::NotifyObservers(void (CUOLMessengerChatInviteNodeObserver::*pfnCallback)(IUOLMessengerChatInviteNode*, IUOLMessengerImageButton2Ptr), IUOLMessengerImageButton2Ptr pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerChatInviteNodeObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(this, pButton);
	}

}

void CChatInviteNode::AddCustomButton(IUOLMessengerImageButton2Ptr pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	m_listButtons.AddHead(pButton);

	Invalidate();
}

void CChatInviteNode::RemoveCustomButton(IUOLMessengerImageButton2Ptr pButton)
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


void CChatInviteNode::AddCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	m_listMenus.AddHead(pCustomMenuItem);
}


void CChatInviteNode::RemoveCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem)
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


void CChatInviteNode::GetRectForElement(IUOLMessengerContactListSettings2::ContactNodeElements nContactElement,
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

void CChatInviteNode::GetMenuItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens)
{
	listItens.RemoveAll();
	listItens.AddTailList(&m_listMenus);
}


void CChatInviteNode::DrawCheckbox(CDCHandle& dc)
{
	CSize size = GetSize();
	int nTop = ((size.cy - CHECKBOX_HEIGHT) / 2);
	m_rectCheckbox = CRect(CPoint(LEFT_SHIFT_SPACE, nTop), 
		CPoint(LEFT_SHIFT_SPACE + CHECKBOX_WIDTH, nTop + CHECKBOX_HEIGHT));

	int x = m_rectCheckbox.left;
	int y = m_rectCheckbox.top;
	int cx = m_rectCheckbox.Width();
	int cy = m_rectCheckbox.Height();
	
	ATLASSERT(m_pCheckOnButton != NULL);
	ATLASSERT(m_pCheckOffButton != NULL);
	
	// Draw CheckOn button
	if ((m_pCheckOnButton.get()) && (m_pCheckOnButton->GetImage().get()) && 
		(m_pCheckOnButton->IsVisible()) && (m_bIsChecked))
	{
		IUOLMessengerImagePtr pImage = m_pCheckOnButton->GetImage();
		CImageButton::MouseState state = m_pCheckOnButton->GetState();
		m_pCheckOnButton->SetButtonRect(CRect(x, y, x+ cx, y + cy));
		m_pCheckOnButton->Draw(dc, x, y, cx, cy, state);
	}

	// Draw CheckOff button
	if ((m_pCheckOffButton.get()) && (m_pCheckOffButton->GetImage().get()) &&
		(m_pCheckOffButton->IsVisible())  && (!m_bIsChecked))
	{
		IUOLMessengerImagePtr pImage = m_pCheckOffButton->GetImage();
		CImageButton::MouseState state = m_pCheckOffButton->GetState();
		m_pCheckOffButton->SetButtonRect(CRect(x, y, x + cx, y + cy));
		m_pCheckOffButton->Draw(dc, x, y, cx, cy, state);
	}
}


void CChatInviteNode::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	CString cmdName = pButton->GetCommandName();

	if (cmdName.CompareNoCase(_T("CheckOn")) == 0)
    {
		m_bIsChecked = FALSE;
		m_pGroupListCtrl->GetParent().SendMessage(WM_USER_PREFS_CHANGED);
    }
	else if (cmdName.CompareNoCase(_T("CheckOff")) == 0)
    {
		m_bIsChecked = TRUE;
		m_pGroupListCtrl->GetParent().SendMessage(WM_USER_PREFS_CHANGED);
    }

	m_pCheckOnButton->SetVisible(m_bIsChecked);
	m_pCheckOffButton->SetVisible(!m_bIsChecked);
}


void CChatInviteNode::UpdateButtonState(const CPoint& pt, CImageButtonPtr pButton, CImageButton::MouseState mouseState)
{
	BOOL bPtInImage = IsPointInButtonRect(pt, pButton);

	CImageButton::MouseState stateCurrent = pButton->GetState();

	if (bPtInImage)
	{
		pButton->SetState(mouseState);
	}
	else
	{
		pButton->SetState(CImageButton::MouseOut);
	}

	if (stateCurrent != pButton->GetState())
	{
		Invalidate();
	}
}


BOOL CChatInviteNode::IsPointInButtonRect(const CPoint& pt, CImageButtonPtr pButton)
{
	CPoint scrollOffset;
	m_pGroupListCtrl->GetScrollOffset(scrollOffset);
	
	CPoint ptWithScroll = pt;
	ptWithScroll.Offset(scrollOffset.x, scrollOffset.y);

	CRect rectNode = CGroupListCtrlItem::GetRect();
	
	CRect rectButton = pButton->GetButtonRect();
	rectButton.top += rectNode.top;
	rectButton.bottom += rectNode.top;
	rectButton.left += rectNode.left;
	rectButton.right += rectNode.left;

	return rectButton.PtInRect(ptWithScroll);

}


BOOL CChatInviteNode::IsFindContactMatch() const
{
    if (!m_pFindContactPanel->IsFinding())
    {
        return TRUE;
    }

    CString strText;
    CString strName;
    CString strAlias;

    strText = m_pFindContactPanel->GetText();
    if (strText.IsEmpty())
    {
        return TRUE;
    }

    strName = m_pContact->GetName();
    strAlias = m_pContact->GetAlias();

    //make strings lower for no case find
    strName.MakeLower();
    strAlias.MakeLower();
    strText.MakeLower();

    ATLTRACE(_T("%s - %s\n"), __FUNCTION__, strText);

    return (strName.Find(strText) != -1) || (strAlias.Find(strText) != -1);
}


BOOL CChatInviteNode::IsChecked()
{
	return m_bIsChecked;
}