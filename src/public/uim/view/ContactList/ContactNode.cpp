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
#include "ContactNode.h"

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../../controller/uimapplication.h"
#include "../../controller/UIManager.h"
#include "../image/Image.h"
#include "../../resource.h"

#include <DataObject.h>
#include <DropSource.h>

#include <commands/launchmessagewindowcommand.h>

#include "../tooltip/ContactTooltipBuilder.h"


#define			LEFT_SHIFT_SPACE					18
#define			RIGHT_SHIFT_SPACE					10
#define			INNER_ELEMENTS_SPACE				7
#define			STATUS_WIDTH_SPACE					16
#define			STATUS_HEIGHT_SPACE					16
#define			TOP_BOTTOM_SHIFT_SPACE				2


using namespace GroupListCtrl;



#ifdef _LEAKDEBUG
LONG CContactNode::ms_nRefs = 0;
#endif

CContactNode::CContactNode(IUOLMessengerContactPtr pContact) :	
m_pContact(pContact),
m_strName(pContact->GetName()),
m_pDefaultPainter(NULL),
m_pCurrentPainter(NULL),
m_pSelectionPainter(NULL),
m_clrTextNormal(RGB(0, 0, 0)),
m_clrTextSelected(RGB(0, 0, 0))
{
	m_pContact->RegisterObserver(this);

	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	IUOLMessengerUIManager* pUIManagerInterface = dynamic_cast<IUOLMessengerUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());

	ATLASSERT(pUIManager);

	IElementPtr pElement = const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__ContactTooltipBuilderInstance");
	CElementBuilderPtr pContactTooltipBuilderPtr = ap_dynamic_cast<CElementBuilderPtr>(pElement);

	m_pContactTooltip = ap_dynamic_cast<CContactTooltipPtr>(pContactTooltipBuilderPtr->CreateElement());

	m_pUOLMessengerContactListSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings();	

#ifdef _LEAKDEBUG
	::InterlockedIncrement(&ms_nRefs);
#endif
}


CContactNode::~CContactNode()
{
#ifdef _LEAKDEBUG
	::InterlockedDecrement(&ms_nRefs);

	ATLTRACE(_T("%s: %d - %s"), __FUNCTION__, ms_nRefs, GetName());
#endif

	m_pContact->UnregisterObserver(this);
}


IUOLMessengerContactPtr CContactNode::GetContact() const
{
	return m_pContact;
}


void CContactNode::SetComparatorAdapter(CComparatorAdapter* pComparator)
{
	m_pComparatorAdapter = pComparator;
}


CComparatorAdapter* CContactNode::GetComparatorAdapter() const
{
	return m_pComparatorAdapter;
}


void CContactNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CContactNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CContactNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;

		RestorePainter(pPainter->GetTimeout());
	}
}


IPainter* CContactNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CContactNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CContactNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CContactNode::SetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextNormal = pFont;
}


void CContactNode::SetSelectedTextFont(CFontElementPtr  pFont)
{
	m_pFontTextSelected = pFont;
}

void CContactNode::SetTextFontBlocked(CFontElementPtr  pFont)
{
	m_pFontContactBlocked = pFont;
}

void CContactNode::SetDropTargetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextDropTarget = pFont;
}


void CContactNode::SetTextColor(COLORREF clrText)
{
	m_clrTextNormal = clrText;
}


void CContactNode::SetTextColorBlocked(COLORREF clrText)
{
	m_clrTextBlocked = clrText;
}


void CContactNode::SetSelectedTextColor(COLORREF clrText)
{
	m_clrTextSelected = clrText;
}


void CContactNode::SetDropTargetTextColor(COLORREF clrText)
{
	m_clrTextDropTarget = clrText;
}


void CContactNode::SetAllContactsGroup(const CString& strAllContactsGroup)
{
	m_strAllContactsGroup = strAllContactsGroup;
}


void CContactNode::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}


CString CContactNode::GetAllContactsGroup() const
{
	return m_strAllContactsGroup;
}


void CContactNode::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CContactNode::GetName() const
{
	return m_strName;
}


BOOL CContactNode::IsVisible() const
{
	BOOL bShowOffline = m_pUOLMessengerContactListSettings->IsShowOfflineEnabled();

    BOOL bVisible;

    if ((NULL != m_pFindContactPanel) && (m_pFindContactPanel->IsFinding()))
    {
        bVisible = IsFindContactMatch();
    }
    else
    {
        bVisible = (bShowOffline || (m_pContact->GetStatus() != IUOLMessengerContact::CS_OFFLINE));
    }

	return bVisible;
}


int CContactNode::Compare(const CEntry* pEntry) const
{
	const CContactNode* pContactNode = dynamic_cast<const CContactNode*>(pEntry);

	if (pContactNode && m_pComparatorAdapter)
	{
		return m_pComparatorAdapter->Compare(this->GetContact().get(), pContactNode->GetContact().get());
	}

	return CEntry::Compare(pEntry);
}


void CContactNode::Paint(CDCHandle& dc)
{
	if (IsAvatarVisible())
	{
		DrawAvatar(dc);
	}
	if (IsStatusVisible())
	{
		DrawStatus(dc);
	}
	DrawButtons(dc);
	DrawText(dc);
}


void CContactNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CContactNode::DrawBackground(CDCHandle& dc)
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


void CContactNode::OnLButtonDown(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseClick);	

	if (pButton.get())
	{
		pButton->OnLButtonDown(pt);
	} 
	else
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

		IUOLMessengerGroupPtr pGroup = pContact->GetGroup();

		if (!pGroup.get())
		{
			return;
		}

		CEntryGroupPtr pEntryGroup = m_pGroupListCtrl->GetGroup(pGroup->GetName());

		if (!pEntryGroup.get())
		{
			return;
		}	

		pEntryGroup->SetValidDropTarget(FALSE);

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

	NotifyObservers(&CUOLMessengerContactNodeObserver::OnLButtonDown, pButton);
}

void CContactNode::OnLButtonUp(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseOver);

	if (pButton.get())
	{
		pButton->OnLButtonUp(pt);
	}

	NotifyObservers(&CUOLMessengerContactNodeObserver::OnLButtonUp, pButton);
}

void CContactNode::OnLBtnDblClick(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseClick);

	if (pButton.get())
	{
		pButton->OnLButtonDblClick(pt);
	}
	else
	{
		LaunchMessageWindow();
	}

	NotifyObservers(&CUOLMessengerContactNodeObserver::OnLButtonDblClick, pButton);
}

void CContactNode::OnRButtonDown(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseClick);

	if (pButton.get())
	{
		pButton->OnRButtonDown(pt);
	}

	NotifyObservers(&CUOLMessengerContactNodeObserver::OnRButtonDown, pButton);
}

void CContactNode::OnRBtnDblClick(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseClick);

	if (pButton.get())
	{
		pButton->OnRButtonDblClick(pt);
	}

	NotifyObservers(&CUOLMessengerContactNodeObserver::OnRButtonDblClick, pButton);
}

void CContactNode::OnMouseMove(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseOver);

	if (pButton.get())
	{
		pButton->OnMouseMove(pt);
	}
}

void CContactNode::OnMouseHover(const CPoint& pt)
{
	CImageButtonPtr pButton = GetImageButton(pt, CImageButton::MouseOver);

	if (!pButton.get())
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
}

void CContactNode::OnMouseLeave()
{
	CPoint ptPos;
	CImageButtonPtr pButton = GetImageButton(ptPos, CImageButton::MouseOver);

	if(m_pContactTooltip->IsWindow())
	{
		m_pContactTooltip->DestroyWindow();
	}
}

void CContactNode::OnReturnKeyPressed()
{
	LaunchMessageWindow();
}


void CContactNode::OnTimer(UINT nId)
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


void CContactNode::Update(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CContactNode::UpdateAlias(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CContactNode::UpdateStatus(IUOLMessengerContact* pContact)
{
	UpdateImpl();
}


void CContactNode::UpdateIcon(IUOLMessengerContact* pContact)
{
	ATLASSERT(m_pGroupListCtrl);

	Invalidate();
	Redraw();
}


void CContactNode::RestorePainter(int nTimeout)
{
	time_t curtime;
	time(&curtime);
	m_timeRestore = curtime + nTimeout;
}


void CContactNode::DrawAvatar(CDCHandle& dc)
{
	GetRectForElement(IUOLMessengerContactListSettings2::AVATAR_ELEMENT, m_rectAvatar);

	IUOLMessengerUserIconPtr pUserIcon = m_pContact->GetUserIcon();

	if (pUserIcon != NULL)
	{
		IUOLMessengerImagePtr pImage = pUserIcon->GetImage();

		if (pImage != NULL)
		{
			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawPhotoStrategy();

			CDCHandle dcHandle(dc);

			pImage->Draw(dcHandle, 
				m_rectAvatar.left, m_rectAvatar.top, 
				m_rectAvatar.Width(), m_rectAvatar.Height(), 
				pStrategy);
		}
	}
}


void CContactNode::DrawText(CDCHandle& dc)
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

	CString strText;
	CString strContact = m_pContact->GetAlias();

	if (strContact.IsEmpty())
	{
		strContact = m_pContact->GetName();
	}

	// Status message
	IUOLMessengerContact4Ptr pContact4 = ap_dynamic_cast<IUOLMessengerContact4Ptr>(m_pContact);
	if (pContact4)
	{
		CString strStatusMessage = pContact4->GetStatusMessage();
		if (!strStatusMessage.IsEmpty())
		{
			strText.Format("%s - %s", strContact, strStatusMessage);
		}
		else
		{
			strText = strContact;
		}
	}
	else
	{
		strText = strContact;
	}

	CRect rectText;
	GetRectForElement(IUOLMessengerContactListSettings2::ALIAS_ELEMENT, rectText);

	rectText.right = m_rectButtons.left;

	pFont->DrawColoredText(dc, rectText, strText, clrText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
}	


void CContactNode::DrawStatus(CDCHandle& dc)
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

void CContactNode::DrawButtons(CDCHandle& dc)
{
	CRect rectButtons;
	GetRectForElement(IUOLMessengerContactListSettings2::BUTTONS_ELEMENT, rectButtons);

	int x = rectButtons.right;
	int y = 0;
	int w = 0;
	int h = 0;

	m_rectButtons.right = x;

	for (POSITION pos = m_listButtons.GetHeadPosition(); pos;)
	{
		CImageButtonPtr	pButton = ap_static_cast<CImageButtonPtr>(m_listButtons.GetNext(pos));

		ATLASSERT(pButton != NULL);		

		if ((pButton.get()) && (pButton->GetImage().get()))
		{
			if (pButton->IsVisible() == FALSE) 
			{
				continue;
			}

			IUOLMessengerImagePtr pImage = pButton->GetImage();

			w = pButton->GetWidth();
			h = pImage->GetHeight();

			CImageButton::MouseState state = pButton->GetState();

			y = (rectButtons.Height() - h) / 2;

			pButton->Draw(dc, x - w, y, w, h, state);

			// TODO: Check a real spacement
			x -= (w + 5);
		}
	}

	m_rectButtons.left = x;
	m_rectButtons.top = y;
	m_rectButtons.bottom = h;
}

void CContactNode::LaunchMessageWindow()
{
	CUIMApplication::GetApplication()->ExecuteCommand(new CLaunchMessageWindowCommand(m_pContact));
}


void CContactNode::UpdateImpl()
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

void CContactNode::RegisterObserver(CUOLMessengerContactNodeObserver* pObserver)
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


void CContactNode::UnregisterObserver(CUOLMessengerContactNodeObserver* pObserver)
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

void CContactNode::NotifyObservers(void (CUOLMessengerContactNodeObserver::*pfnCallback)(IUOLMessengerContactNode*, IUOLMessengerImageButton2Ptr), IUOLMessengerImageButton2Ptr pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerContactNodeObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(this, pButton);
	}

}

void CContactNode::AddCustomButton(IUOLMessengerImageButton2Ptr pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	m_listButtons.AddHead(pButton);

	Invalidate();
}

void CContactNode::RemoveCustomButton(IUOLMessengerImageButton2Ptr pButton)
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

void CContactNode::AddCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	m_listMenus.AddHead(pCustomMenuItem);
}

void CContactNode::RemoveCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem)
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

CImageButtonPtr CContactNode::GetImageButton(const CPoint& pt, CImageButton::MouseState mouseState)
{
	BOOL bPtInImage;
	CRect rectImage = GetRect();

	CRect rectButtons;
	GetRectForElement(IUOLMessengerContactListSettings2::BUTTONS_ELEMENT, rectButtons);

	int x = rectButtons.right;
	int y = 0;
	int w = 0;
	int h = 0;

	CPoint ptClickAbsolute = pt;

	ptClickAbsolute += m_pGroupListCtrl->m_ptOffset;

	if (mouseState == CImageButton::MouseClick)
	{
		ATLTRACE(_T("%s - pt = (%d,%d); rectImage = [%d,%d]x[%d,%d]; item = %s\n"), 
			__FUNCTION__, pt.x, pt.y, rectImage.left, rectImage.right, rectImage.top, rectImage.bottom, GetName());
		ATLTRACE(_T("%s - ptOffset = (%d,%d); item = %s\n"), 
			__FUNCTION__, ptClickAbsolute.x, ptClickAbsolute.y, GetName());
	}

	CImageButtonPtr pButton;

	for (POSITION pos = m_listButtons.GetHeadPosition(); pos;)
	{
		CImageButtonPtr pBtn = ap_static_cast<CImageButtonPtr>(m_listButtons.GetNext(pos));
		ATLASSERT(pBtn != NULL);				

		if (pBtn->IsVisible()==FALSE) 
		{
			continue;
		}

		IUOLMessengerImagePtr pImage = pBtn->GetImage();

		if (pImage)
		{
			w = pBtn->GetWidth();
			h = pImage->GetHeight();

			rectImage.left = x - w;
			rectImage.top += (rectImage.Height() - h) / 2;
			rectImage.right = rectImage.left + w;
			rectImage.bottom = rectImage.top + h;

			if (mouseState == CImageButton::MouseClick)
			{
				ATLTRACE(_T("%s - pt = (%d,%d); rectImage = [%d,%d]x[%d,%d]; item = %s\n"), 
					__FUNCTION__, pt.x, pt.y, rectImage.left, rectImage.right, rectImage.top, rectImage.bottom, GetName());
			}

			bPtInImage = rectImage.PtInRect(ptClickAbsolute);

			CImageButton::MouseState stateCurrent = pBtn->GetState();

			if (bPtInImage)
			{
				pBtn->SetState(mouseState);

				pButton = pBtn;			
			}
			else
			{
				pBtn->SetState(CImageButton::MouseOut);
			}

			if (stateCurrent != pBtn->GetState())
			{
				Invalidate();
			}
		}

		x -= (w + 5);
	}

	return pButton;
}

BOOL CContactNode::IsAvatarVisible()
{
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pUOLMessengerContactListSettings);
	
	if (pContactListSettings2)
	{
		CString sContactNodeLayout = pContactListSettings2->GetContactNodeLayout();
		
		if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_LAYOUT) != 0) && 
			 (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_STATUS_LAYOUT) != 0) && 
			 (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::STATUS_ALIAS_LAYOUT) != 0) )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CContactNode::IsStatusVisible()
{
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pUOLMessengerContactListSettings);
	
	if (pContactListSettings2)
	{
		CString sContactNodeLayout = pContactListSettings2->GetContactNodeLayout();
		
		if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_LAYOUT) != 0) && 
			 (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_AVATAR_LAYOUT) != 0) && 
			 (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::AVATAR_ALIAS_LAYOUT) != 0) )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

int CContactNode::GetElementPosition(
		IUOLMessengerContactListSettings2::ContactNodeElements nContactElement)
{
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pUOLMessengerContactListSettings);
	
	if (!pContactListSettings2)
	{
		return 0;
	}
	
	CString sContactNodeLayout = pContactListSettings2->GetContactNodeLayout();
	int nElementPosition = 1;
	
	if ( (!IsAvatarVisible()) && 
			(nContactElement == IUOLMessengerContactListSettings2::AVATAR_ELEMENT) )
	{
		return 0;
	}
	
	if ( (!IsStatusVisible()) && 
			(nContactElement == IUOLMessengerContactListSettings2::STATUS_ELEMENT) )
	{
		return 0;
	}
	
	if ( (nContactElement == IUOLMessengerContactListSettings2::ALIAS_ELEMENT) || 
			(nContactElement == IUOLMessengerContactListSettings2::BUTTONS_ELEMENT) )
	{
		if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::AVATAR_ALIAS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::AVATAR_ALIAS_STATUS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::STATUS_ALIAS_AVATAR_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::STATUS_ALIAS_LAYOUT) == 0) )
		{
			nElementPosition = 2;
		}
		else if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::AVATAR_STATUS_ALIAS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::STATUS_AVATAR_ALIAS_LAYOUT) == 0) )
		{
			nElementPosition = 3;
		}
	}
	else if (nContactElement == IUOLMessengerContactListSettings2::AVATAR_ELEMENT)
	{
		if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_AVATAR_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_AVATAR_STATUS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::STATUS_AVATAR_ALIAS_LAYOUT) == 0) )
		{
			nElementPosition = 2;
		}
		else if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_STATUS_AVATAR_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::STATUS_ALIAS_AVATAR_LAYOUT) == 0) )
		{
			nElementPosition = 3;
		}
	}
	else if (nContactElement == IUOLMessengerContactListSettings2::STATUS_ELEMENT)
	{
		if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_STATUS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::AVATAR_STATUS_ALIAS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_STATUS_AVATAR_LAYOUT) == 0) )
		{
			nElementPosition = 2;
		}
		else if ( (sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::AVATAR_ALIAS_STATUS_LAYOUT) == 0) || 
				(sContactNodeLayout.Compare(IUOLMessengerContactListSettings2::ALIAS_AVATAR_STATUS_LAYOUT) == 0) )
		{
			nElementPosition = 3;
		}
	}
	
	return nElementPosition;
}

void CContactNode::GetRectForElement(
		IUOLMessengerContactListSettings2::ContactNodeElements nContactElement, CRect& rElementRect)
{
	CRect contactNodeRect(CPoint(0, 0), GetSize());
	
	// Initialize returning rect.
	rElementRect.SetRect(0, 0, 0, 0);
	
	// There are our variable components, that will share the remainder space left 
	// by the avatar and status elements.
	//
	if ( (nContactElement == IUOLMessengerContactListSettings2::ALIAS_ELEMENT) || 
			(nContactElement == IUOLMessengerContactListSettings2::BUTTONS_ELEMENT) )
	{
		rElementRect = contactNodeRect;
		rElementRect.left += LEFT_SHIFT_SPACE;
		rElementRect.right -= RIGHT_SHIFT_SPACE;
		
		int avatarPosition = GetElementPosition(IUOLMessengerContactListSettings2::AVATAR_ELEMENT);
		int statusPosition = GetElementPosition(IUOLMessengerContactListSettings2::STATUS_ELEMENT);
		int aliasPosition = GetElementPosition(IUOLMessengerContactListSettings2::ALIAS_ELEMENT);
		
		if (avatarPosition != 0)
		{
			if (avatarPosition < aliasPosition)
			{
				rElementRect.left += (GetAvatarSideSize() + INNER_ELEMENTS_SPACE);
			}
			else if (avatarPosition > aliasPosition)
			{
				rElementRect.right -= (INNER_ELEMENTS_SPACE + GetAvatarSideSize());
			}
		}
		
		if (statusPosition != 0)
		{
			if (statusPosition < aliasPosition)
			{
				rElementRect.left += (STATUS_WIDTH_SPACE + INNER_ELEMENTS_SPACE);
			}
			else if (statusPosition > aliasPosition)
			{
				rElementRect.right -= (INNER_ELEMENTS_SPACE + STATUS_WIDTH_SPACE);
			}
		}
	}
	// Fixed element.
	//
	else if (nContactElement == IUOLMessengerContactListSettings2::AVATAR_ELEMENT)
	{
		int avatarPosition = GetElementPosition(IUOLMessengerContactListSettings2::AVATAR_ELEMENT);
		int statusPosition = GetElementPosition(IUOLMessengerContactListSettings2::STATUS_ELEMENT);
		
		if (avatarPosition == 1)
		{
			rElementRect = contactNodeRect;
			
			rElementRect.bottom -= 2;
			rElementRect.top += 2;
			rElementRect.left = LEFT_SHIFT_SPACE;
			rElementRect.right = rElementRect.left + rElementRect.Height();
		}
		else if (  ( (avatarPosition == 2) && (statusPosition == 0) ) || (avatarPosition == 3) )
		{
			rElementRect = contactNodeRect;
			
			rElementRect.bottom -= 2;
			rElementRect.top += 2;
			rElementRect.right -= RIGHT_SHIFT_SPACE;
			rElementRect.left = rElementRect.right - rElementRect.Height();
		}
		else if ( (avatarPosition == 2) && (statusPosition != 0) )
		{
			if (statusPosition == 1)
			{
				rElementRect = contactNodeRect;
				
				rElementRect.bottom -= 2;
				rElementRect.top += 2;
				rElementRect.left = ( LEFT_SHIFT_SPACE + (STATUS_WIDTH_SPACE + INNER_ELEMENTS_SPACE) );
				rElementRect.right = rElementRect.left + rElementRect.Height();
			}
			else if (statusPosition == 3)
			{
				rElementRect = contactNodeRect;
				
				rElementRect.bottom -= 2;
				rElementRect.top += 2;
				rElementRect.right -= ( (INNER_ELEMENTS_SPACE + STATUS_WIDTH_SPACE) + RIGHT_SHIFT_SPACE );
				rElementRect.left = rElementRect.right - rElementRect.Height();
			}
		}
	}
	// Fixed element.
	//
	else if (nContactElement == IUOLMessengerContactListSettings2::STATUS_ELEMENT)
	{
		int avatarPosition = GetElementPosition(IUOLMessengerContactListSettings2::AVATAR_ELEMENT);
		int statusPosition = GetElementPosition(IUOLMessengerContactListSettings2::STATUS_ELEMENT);
		
		if (statusPosition == 1)
		{
			rElementRect = contactNodeRect;
			
			int nOffset = (rElementRect.Height() - STATUS_HEIGHT_SPACE) / 2;
			
			rElementRect.top += nOffset;
			rElementRect.bottom -= nOffset;
			rElementRect.left = LEFT_SHIFT_SPACE;
			rElementRect.right = rElementRect.left + rElementRect.Height();
		}
		else if (  ( (statusPosition == 2) && (avatarPosition == 0) ) || (statusPosition == 3) )
		{
			rElementRect = contactNodeRect;
			
			int nOffset = (rElementRect.Height() - STATUS_HEIGHT_SPACE) / 2;
			
			rElementRect.top += nOffset;
			rElementRect.bottom -= nOffset;
			rElementRect.right -= RIGHT_SHIFT_SPACE;
			rElementRect.left = rElementRect.right - rElementRect.Height();
		}
		else if ( (statusPosition == 2) && (avatarPosition != 0) )
		{
			if (avatarPosition == 1)
			{
				rElementRect = contactNodeRect;
				
				int nOffset = (rElementRect.Height() - STATUS_HEIGHT_SPACE) / 2;
				
				rElementRect.top += nOffset;
				rElementRect.bottom -= nOffset;
				rElementRect.left = ( LEFT_SHIFT_SPACE + (GetAvatarSideSize() + INNER_ELEMENTS_SPACE) );
				rElementRect.right = rElementRect.left + rElementRect.Height();
			}
			else if (avatarPosition == 3)
			{
				rElementRect = contactNodeRect;
				
				int nOffset = (rElementRect.Height() - STATUS_HEIGHT_SPACE) / 2;
				
				rElementRect.top += nOffset;
				rElementRect.bottom -= nOffset;
				rElementRect.right -= ( (INNER_ELEMENTS_SPACE + GetAvatarSideSize()) + RIGHT_SHIFT_SPACE );
				rElementRect.left = rElementRect.right - rElementRect.Height();
			}
		}
	}
}

int CContactNode::GetAvatarSideSize() const
{
	return GetSize().cy - (2 * TOP_BOTTOM_SHIFT_SPACE);
}

void CContactNode::GetMenuItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens)
{
	listItens.RemoveAll();
	listItens.AddTailList(&m_listMenus);
}

BOOL CContactNode::IsFindContactMatch() const
{
    if (NULL == m_pFindContactPanel)
	{
	    return TRUE;
	}

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
