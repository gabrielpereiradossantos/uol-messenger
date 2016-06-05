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
#include "ChatInviteListCtrl.h"

#include "ChatInviteNode.h"
#include "GroupChatInviteNode.h"
#include "../Image/ImageBuilder.h"
#include "../ContactList/GroupFindNode.h"
#include "../../controller/UIMApplication.h"

#include <commands/InviteForChatCommand.h>


const int CChatInviteListCtrl::TIMER_SCROLL_VIEW = 150;
const int CChatInviteListCtrl::NODE_HEIGHT = 15;


CChatInviteListCtrl::CChatInviteListCtrl() :
	m_pGroupElement(NULL),
	m_pContactElement(NULL),
	m_scrollCommand(SC_SCROLL_NONE),
	m_nTimerScroll(0),
	m_nScrollLineHeight(0),
	m_pTransferDataObject(NULL)
{

	//m_nWheelLines = 5;
}


CChatInviteListCtrl::~CChatInviteListCtrl()
{
	if (m_pChatInviteMenu != NULL)
	{
		m_pChatInviteMenu->DestroyMenu();
	}
}


void CChatInviteListCtrl::Init(IUOLMessengerContactListPtr pContactList)
{
	ATLASSERT(m_pConversation != NULL);
    ATLASSERT(pContactList != NULL);
	if (pContactList != NULL)
	{
		m_pContactList = pContactList;
		m_pContactList->RegisterObserver(this);

		m_pComparatorAdapter = new CComparatorAdapter();

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
		{
			IElement* pElement = m_listChilds.GetNext(pos).get();

            if (-1 != pElement->GetName().Find(_T("__GroupElementInstance_")))
			{
				m_pGroupElement = dynamic_cast<CGroupElement*>(pElement);
			}
			else if (-1 != pElement->GetName().Find(_T("__ContactElementInstance_")))
			{
				m_pContactElement = dynamic_cast<CContactElement*>(pElement);
			}
		}

		ATLASSERT(m_pGroupElement != NULL);
		ATLASSERT(m_pContactElement != NULL);

		m_nScrollLineHeight = max(m_pGroupElement->GetHeight(),	m_pContactElement->GetHeight());
	}
}


void CChatInviteListCtrl::Finalize()
{
	if (m_pContactList != NULL)
	{
		m_pContactList->UnregisterObserver(this);
	}
}


IUOLMessengerContactPtr CChatInviteListCtrl::GetSelectedContact()
{
	CChatInviteNodePtr pChatInviteNode = ap_static_cast<CChatInviteNodePtr>(GetSelectedItem());

	if (pChatInviteNode != NULL)
	{
		return pChatInviteNode->GetContact();
	}

	return IUOLMessengerContactPtr();
}


void CChatInviteListCtrl::SetOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits)
{
	if (m_pComparatorAdapter)
	{
		m_pComparatorAdapter->SetOrderTraits(pOrderTraits);

	}
}


IUOLMessengerContactListOrderTraitsPtr CChatInviteListCtrl::GetOrderTraits()
{
	IUOLMessengerContactListOrderTraitsPtr pOrderTraits;

	if (m_pComparatorAdapter)
	{
		pOrderTraits = m_pComparatorAdapter->GetOrderTraits();
	}

	return pOrderTraits;
}


void CChatInviteListCtrl::ChangeContactPainter(IUOLMessengerContact* pContact, UINT nPainterId)
{
	if (pContact != NULL)
	{
		IUOLMessengerAccount* pAccount = pContact->GetAccount().get();
		IUOLMessengerGroupPtr pGroup = pContact->GetGroup();

		if ((pAccount != NULL) && (pGroup != NULL))
		{
			CString strGroupName = pGroup->GetName();
			CString strEntryName = FormatChatInviteNodeName(pContact);

			CEntryPtr pEntry;
			
			pEntry = GetEntryByName(strEntryName, strGroupName);
			
            if (pEntry != NULL)
			{
				CChatInviteNode* pChatInviteNode = dynamic_cast<CChatInviteNode*>(pEntry.get());
				IPainter* pPainter = GetPainter(nPainterId);

				if ((pChatInviteNode != NULL) && (pPainter != NULL))
				{
					pChatInviteNode->SetPainter(pPainter);
					pChatInviteNode->Invalidate();
				}
			}
		}
	}
}


CString CChatInviteListCtrl::GetName() const
{
	return "__ChatInviteListInstance_" + m_strGUID;
}


void CChatInviteListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_check_on");
		m_pCheckOnImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_check_on");
	}

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_check_off");
		m_pCheckOffImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_check_off");
	}
}


void CChatInviteListCtrl::GetMinSize(CSize& size)
{
	return CGroupListCtrl::GetSize(size);
}


void CChatInviteListCtrl::AddChatContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}

    InternalAddContact(pContact);
}


void CChatInviteListCtrl::OnAddContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}

    // This ListCtrl handles only one account
    if (IsSameAccount(pContact->GetAccount()))
    {
        InternalAddContact(pContact);
    }
}


void CChatInviteListCtrl::OnRemoveContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}

	// This ListCtrl handles only one account
    if (IsSameAccount(pContact->GetAccount()))
    {
        InternalRemoveContact(pContact, TRUE);
    }
}


void CChatInviteListCtrl::OnRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if (FALSE == IsWindow())
	{
		return;
	}

	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);

        // This ListCtrl handles only one account
        if (IsSameAccount(pContact->GetAccount()))
        {
            InternalRemoveContact(pContact, FALSE);
        }
	}

	UpdateScrollSize();
}


IPainter* CChatInviteListCtrl::GetPainter(UINT nId)
{
	if ((nId == IPainterRepository::PR_GROUP_DEFAULT_PAINTER) ||
		(nId == IPainterRepository::PR_GROUP_SELECTION_PAINTER))
	{
		// Group painter
		if (m_pGroupElement != NULL)
		{
			return m_pGroupElement->GetPainter(nId);
		}
	}

	// Contact painter
	if (m_pContactElement != NULL)
	{
		return m_pContactElement->GetPainter(nId);
	}

	return NULL;
}


void CChatInviteListCtrl::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
{
	if (nTimerId == TIMER_SCROLL_VIEW)
	{
		if (m_scrollCommand == SC_SCROLL_UP)
		{
			CSize sizeLine(1, m_nScrollLineHeight);
			SetScrollLine(sizeLine);

			ATLTRACE(_T("SizeLine(%d,%d)n"), sizeLine.cx, sizeLine.cy);

			ScrollLineUp();
		}
		else if (m_scrollCommand == SC_SCROLL_DOWN)
		{
			CSize sizeLine(1, m_nScrollLineHeight);
			SetScrollLine(sizeLine);

			ATLTRACE(_T("SizeLine(%d,%d)n"), sizeLine.cx, sizeLine.cy);

			ScrollLineDown();
		}
	}

	SetMsgHandled(FALSE);
}


LRESULT CChatInviteListCtrl::OnGetDlgCode(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTCHARS;
	
	/*
	if (lParam != NULL)
	{
		MSG* pMsg = (MSG*)lParam;
		
		if (pMsg)
		{
			if (pMsg->message == WM_CHAR)
			{
				TCHAR ch = pMsg->wParam;
				char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*()[{}]/?\\|";
				
				if (strchr(validChars, ch))
				{
					SendMessage(WM_CHAR, pMsg->wParam, pMsg->lParam);
					
					lRet |= DLGC_WANTMESSAGE;
				}
			}
			
			if (pMsg->message == WM_MOUSEWHEEL)
			{
				SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
				
				lRet |= DLGC_WANTMESSAGE;
			}
			
			if (pMsg->message == WM_KEYDOWN)
			{
				if ((pMsg->wParam == VK_DOWN) || (pMsg->wParam == VK_UP))
				{
					SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
					
					lRet |= DLGC_WANTMESSAGE;
				}
			}
			
			if (pMsg->message == WM_KEYUP)
			{
				if ((pMsg->wParam == VK_HOME) || (pMsg->wParam == VK_END) ||
					(pMsg->wParam == VK_PRIOR) || (pMsg->wParam == VK_NEXT) ||
					(pMsg->wParam == VK_ESCAPE))
				{
					SendMessage(WM_KEYUP, pMsg->wParam, pMsg->lParam);
					
					lRet |= DLGC_WANTMESSAGE;
				}
			}
		}
	}
	*/
	
	return lRet;
}


void CChatInviteListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CChatInviteListCtrl::InternalAddContact(IUOLMessengerContactPtr pContact)
{
    CChatInviteNodePtr pChatInviteNode = new CChatInviteNode(pContact, m_pCheckOnImage,
		m_pCheckOffImage);

	CString strEntryName = FormatChatInviteNodeName(pContact.get());

	static_cast<CEntry*>(pChatInviteNode.get())->SetName(strEntryName);
	pChatInviteNode->SetComparatorAdapter(m_pComparatorAdapter.get());
	pChatInviteNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_CONTACT_DEFAULT_PAINTER));
	pChatInviteNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_CONTACT_SELECTION_PAINTER));

	pChatInviteNode->SetTextFont(m_pContactElement->GetTextFont());
	pChatInviteNode->SetTextColor(m_pContactElement->GetTextColor());
	pChatInviteNode->SetSelectedTextFont(m_pContactElement->GetSelectedTextFont());
	pChatInviteNode->SetSelectedTextColor(m_pContactElement->GetSelectedTextColor());
	pChatInviteNode->SetDropTargetTextFont(m_pContactElement->GetDropTargetTextFont());
	pChatInviteNode->SetDropTargetTextColor(m_pContactElement->GetDropTargetTextColor());
	pChatInviteNode->SetTextFontBlocked(m_pContactElement->GetBlockedTextFont());
	pChatInviteNode->SetTextColorBlocked(m_pContactElement->GetBlockedTextColor());
	pChatInviteNode->SetFindContactPanel(m_pFindContactPanel);

	static_cast<CEntry*>(pChatInviteNode.get())->SetHeight(NODE_HEIGHT);

	if (!m_pChatInviteGroup)
	{
        CreateChatInviteGroup();
	}

	// Add the find group
	if (!m_pGroupFindContacts)
	{
		m_pGroupFindContacts = CreateGroupFind(pContact->GetAccount());
	}

	CEntryPtr pFindEntry = GetEntryByName(strEntryName, m_pGroupFindContacts->GetName());
	if (!pFindEntry)
	{
		AddEntry(m_pGroupFindContacts, pChatInviteNode);
	}

	pChatInviteNode->SetAllContactsGroup(m_pChatInviteGroup->GetName());

	AddEntry(m_pChatInviteGroup, pChatInviteNode);
}


void CChatInviteListCtrl::InternalRemoveContact(IUOLMessengerContactPtr pContact, BOOL bRedraw)
{
	if (m_pChatInviteGroup && pContact)
	{
		CString strContactEntry = FormatChatInviteNodeName(pContact.get());
		IUOLMessengerChatInviteNodePtr pChatInviteNode = ap_dynamic_cast<IUOLMessengerChatInviteNodePtr>(GetEntryByName(strContactEntry, m_pChatInviteGroup->GetName()));

		// Remove contact from Find Group
		RemoveEntry(strContactEntry, m_pGroupFindContacts->GetName());

		// Let's remove the account group reference first, so the redraw can be happen 
		// after the default group reference remove...
		//
		CString strName;
        strName.Format(_T("%s:%s"), pContact->GetAccount()->GetProtocolId(), 
            pContact->GetAccount()->GetUserId());
        RemoveEntry(strContactEntry, strName);
		RemoveEntry(strContactEntry, m_pChatInviteGroup->GetName(), bRedraw);
	}
}


CString CChatInviteListCtrl::FormatChatInviteNodeName(IUOLMessengerContact* pContact)
{
	return FormatChatInviteNodeName(pContact->GetName(), pContact->GetAccount()->GetUserId(), pContact->GetAccount()->GetProtocolId());
}


CString CChatInviteListCtrl::FormatChatInviteNodeName(const CString& strContact, const CString& strAccountUserId, const CString& strAccountProtocolId)
{
	CString strName;
	strName.Format(_T("%s/%s/%s"), strContact, strAccountUserId, strAccountProtocolId);

	return strName;
}


int CChatInviteListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CChatInviteListCtrl::GetContactHeight(SizeImage sizeImage) const
{
	int nHeight = 0;

	if (m_pContactElement != NULL)
	{
		switch (sizeImage)
		{
		case IUOLMessengerContactListSettings::SMALL_IMAGE:
			nHeight = m_pContactElement->GetSmallSize();
			break;

		case IUOLMessengerContactListSettings::MEDIUM_IMAGE:
			nHeight = m_pContactElement->GetMediumSize();
			break;

		case IUOLMessengerContactListSettings::GREAT_IMAGE:
			nHeight = m_pContactElement->GetLargeSize();
			break;
		}
	}

	return nHeight;
}


void CChatInviteListCtrl::GetContacts(CAtlList<IUOLMessengerChatInviteNodePtr>& listContacts)
{
	struct ContactsHelper : public CGroupListCtrl::Callback
	{
		CAtlList<IUOLMessengerChatInviteNodePtr>* m_pListContacts;
		BOOL m_bCurrentGroupVisible;

		ContactsHelper(CAtlList<IUOLMessengerChatInviteNodePtr>& listContacts) :
		m_pListContacts(&listContacts),
			m_bCurrentGroupVisible(FALSE){}

			BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
			{
				if (pItem)
				{
					IUOLMessengerChatInviteNodePtr pChatInviteNode = ap_dynamic_cast<IUOLMessengerChatInviteNodePtr>(pItem);

					if (pChatInviteNode)
					{
						if (m_bCurrentGroupVisible)
						{
							m_pListContacts->AddTail(pChatInviteNode);
						}
					}
					else
					{
						CEntryGroupPtr pGroup = ap_dynamic_cast<CEntryGroupPtr>(pItem);
						if (pGroup)
						{
							m_bCurrentGroupVisible = pGroup->IsVisible();
						}
					}
				}

				return FALSE;
			}
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);

		listContacts.RemoveAll();

		IterateThroughGroupList(ContactsHelper(listContacts), CGroupListCtrl::ioALL);
	}
}

IUOLMessengerImageButton2Ptr CChatInviteListCtrl::CreateContactButton()
{
	IUOLMessengerImageButton2Ptr pButton = new CImageButton();
	return pButton;
}

IUOLMessengerContextMenuItemPtr CChatInviteListCtrl::CreateContextMenuItem(const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback)
{
	IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(15, (LPCTSTR) strLabel, pCallback);
	return pMenuItem;
}

void CChatInviteListCtrl::RegisterObserver(CUOLMessengerChatInviteListCtrlObserver* pObserver)
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


void CChatInviteListCtrl::UnregisterObserver(CUOLMessengerChatInviteListCtrlObserver* pObserver)
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


void CChatInviteListCtrl::NotifyObservers(void (CUOLMessengerChatInviteListCtrlObserver::*pfnCallback)(IUOLMessengerChatInviteNodePtr), IUOLMessengerChatInviteNodePtr pContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerChatInviteListCtrlObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(pContact);
	}

}


void CChatInviteListCtrl::GetAdditionalContextMenuItems(IUOLMessengerChatInviteNodePtr pChatInviteNode)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);
	
	if ( (m_pChatInviteMenu) && (pChatInviteNode) )
	{
		for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
		{
			CUOLMessengerChatInviteListCtrlObserver* pObserver = m_listObservers.GetNext(pos);
			
			pObserver->OnShowContextMenu(
					ap_static_cast<IUOLMessengerContextMenuPtr>(m_pChatInviteMenu->GetContextMenu()), 
					pChatInviteNode);
		}
	}
}


void CChatInviteListCtrl::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
    m_pConversation = pConversation;
}


BOOL CChatInviteListCtrl::IsSameAccount(IUOLMessengerAccountPtr pAccount)
{
    CString strConvUserId = m_pConversation->GetAccount()->GetUserId();
    CString strAccountUserId = pAccount->GetUserId();

    return (0 == strAccountUserId.Compare(strConvUserId));
}


void CChatInviteListCtrl::CreateChatInviteGroup()
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
    CGroupChatInviteNodePtr pGroupChatNode = new CGroupChatInviteNode(m_pConversation->GetAccount());
	pGroupChatNode->SetFindContactPanel(m_pFindContactPanel);
	AddGroup(pGroupChatNode);
	m_pChatInviteGroup = pGroupChatNode;
}


CEntryGroupPtr CChatInviteListCtrl::CreateGroupFind(IUOLMessengerAccountPtr pAccount)
{
	if (FALSE == IsWindow())
	{
		return CEntryGroupPtr();
	}
	
	CGroupFindNodePtr pGroupFindNode = new CGroupFindNode(pAccount);
	
	pGroupFindNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_GROUP_DEFAULT_PAINTER));
	pGroupFindNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_GROUP_SELECTION_PAINTER));
	pGroupFindNode->SetCollapsedImage(m_pGroupElement->GetCollapsedImage());
	pGroupFindNode->SetExpandedImage(m_pGroupElement->GetExpandedImage());
	pGroupFindNode->SetTextFont(m_pGroupElement->GetFont());
	pGroupFindNode->SetTextColor(m_pGroupElement->GetTextColor());
	pGroupFindNode->SetFindContactPanel(m_pFindContactPanel);
	
	int nHeight = GetGroupHeight();
	static_cast<CEntryGroup*>(pGroupFindNode.get())->SetHeight(nHeight);
	
	AddGroup(pGroupFindNode);
	
	return pGroupFindNode;
}


void CChatInviteListCtrl::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
	if (FALSE == IsWindow())
	{
		return;
	}

	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	IUOLMessengerContactListSettingsPtr pContactListSettings = pApplication->GetSettingsManager()->GetContactListSettings();

	if (pSource == pContactListSettings.get())
	{
		BOOL bRedraw = FALSE;

		if ((IUOLMessengerContactListSettings::CL_SHOW_OFFLINE == nSettingId) ||
			(IUOLMessengerContactListSettings::CL_SHOW_GROUPS == nSettingId))
		{
			bRedraw = TRUE;

			UpdateItemSelection();
		}
		else if (IUOLMessengerContactListSettings::CL_SIZEIMAGE == nSettingId)
		{
			int nHeight = GetContactHeight(pContactListSettings->GetSizeImage());

			SetHeight(nHeight, ioEntries);

			bRedraw = TRUE;
		}
		
		if (bRedraw)
		{
			UpdateScrollSize();
		}

        Sort();
	}
}


void CChatInviteListCtrl::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}