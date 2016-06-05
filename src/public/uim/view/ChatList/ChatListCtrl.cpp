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
#include "ChatListCtrl.h"

#include "ChatNode.h"
#include "GroupChatNode.h"

#include "../../controller/UIMApplication.h"
#include "../../model/Contact.h"


#include <commands/InviteForChatCommand.h>


const int CChatListCtrl::TIMER_SCROLL_VIEW = 150;
const int CChatListCtrl::NODE_HEIGHT = 15;


CChatListCtrl::CChatListCtrl() :
	CWindowElement<CChatListCtrl, CChatListCtrlTarget>(),
	m_pGroupElement(NULL),
	m_pContactElement(NULL),
	m_scrollCommand(SC_SCROLL_NONE),
	m_nTimerScroll(0),
	m_nScrollLineHeight(0),
	m_pTransferDataObject(NULL)
{

	//m_nWheelLines = 5;
}


CChatListCtrl::~CChatListCtrl()
{
	if (m_pChatMenu != NULL)
	{
		m_pChatMenu->DestroyMenu();
	}
}


void CChatListCtrl::Init(IUOLMessengerContactListPtr pContactList)
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


void CChatListCtrl::Finalize()
{
	if (m_pContactList != NULL)
	{
		m_pContactList->UnregisterObserver(this);
	}
}


IUOLMessengerContact3Ptr CChatListCtrl::GetSelectedContact()
{
	CChatNodePtr pChatNode = ap_static_cast<CChatNodePtr>(GetSelectedItem());

	if (pChatNode != NULL)
	{
		return pChatNode->GetContact();
	}

	return IUOLMessengerContact3Ptr();
}


void CChatListCtrl::SetOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits)
{
	if (m_pComparatorAdapter)
	{
		m_pComparatorAdapter->SetOrderTraits(pOrderTraits);

	}
}


IUOLMessengerContactListOrderTraitsPtr CChatListCtrl::GetOrderTraits()
{
	IUOLMessengerContactListOrderTraitsPtr pOrderTraits;

	if (m_pComparatorAdapter)
	{
		pOrderTraits = m_pComparatorAdapter->GetOrderTraits();
	}

	return pOrderTraits;
}


void CChatListCtrl::ChangeContactPainter(IUOLMessengerContact3Ptr pContact, UINT nPainterId)
{
	if (pContact != NULL)
	{
		IUOLMessengerAccount* pAccount = pContact->GetAccount().get();
		
        if ((pAccount != NULL) && (m_pChatGroup != NULL))
		{
			CString strGroupName = m_pChatGroup->GetName();
			CString strEntryName = FormatChatNodeName(pContact);

			CEntryPtr pEntry;
			
			pEntry = GetEntryByName(strEntryName, strGroupName);
			
            if (pEntry != NULL)
			{
				CChatNode* pChatNode = dynamic_cast<CChatNode*>(pEntry.get());
				IPainter* pPainter = GetPainter(nPainterId);

				if ((pChatNode != NULL) && (pPainter != NULL))
				{
					pChatNode->SetPainter(pPainter);
					pChatNode->Invalidate();
				}
			}
		}
	}
}


void CChatListCtrl::AddChatContact(IUOLMessengerContact3Ptr pContact)
{
    if (FALSE == IsWindow())
	{
		return;
	}

	InternalAddContact(pContact);
}


void CChatListCtrl::RemoveChatContact(IUOLMessengerContact3Ptr pContact)
{
    if (FALSE == IsWindow())
	{
		return;
	}

	InternalRemoveContact(pContact, TRUE);
}


void CChatListCtrl::RemoveChatContacts()
{
	if (FALSE == IsWindow())
	{
		return;
	}

    if (m_pConversation)
    {
        CAtlList<IUOLMessengerContact3Ptr> listContacts;
        m_pConversation->GetChatParticipantList(listContacts);

        for (POSITION pos = listContacts.GetHeadPosition(); pos;)
        {
            IUOLMessengerContact3Ptr pContact = listContacts.GetNext(pos);
            if (pContact)
            {
                InternalRemoveContact(pContact, FALSE);
            }
        }
    }

	UpdateScrollSize();
}


CString CChatListCtrl::GetName() const
{
	return "__ChatListInstance_" + m_strGUID;
}


void CChatListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}


void CChatListCtrl::GetMinSize(CSize& size)
{
	return CGroupListCtrl::GetSize(size);
}

void CChatListCtrl::OnAddContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}

    // This ListCtrl handles only one account
    if (IsSameAccount(pContact->GetAccount()))
    {
        // Look if this contact is a chat participant
        IUOLMessengerContact3Ptr pChatParticipant = m_pConversation->FindChatParticipant(pContact->GetName());
        if (pChatParticipant)
        {
            // Chat participant was found, if it isn't contact remove it.
            if (!pChatParticipant->IsContact())
            {
                InternalRemoveContact(pChatParticipant, FALSE);
                
                m_pConversation->UpdateChatParticipant(pContact->GetName(), pContact->GetAlias(), TRUE);
            }
        }
        
        // Add contact
        IUOLMessengerContact3Ptr pContact3 = ap_dynamic_cast<IUOLMessengerContact3Ptr>(pContact);
        if (pContact3)
        {
            InternalAddContact(pContact3);            
        }
    }
}


void CChatListCtrl::OnRemoveContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}

	// This ListCtrl handles only one account
    if (IsSameAccount(pContact->GetAccount()))
    {
        // Look if this contact is a chat participant
        IUOLMessengerContact3Ptr pChatParticipant = m_pConversation->FindChatParticipant(pContact->GetName());
        if (pChatParticipant)
        {
            // Remove contact
            IUOLMessengerContact3Ptr pContact3 = ap_dynamic_cast<IUOLMessengerContact3Ptr>(pContact);
            if (pContact3)
            {
                InternalRemoveContact(pContact3, TRUE);
            }
            
            // Chat participant was found, if it is contact add a ChatContact.
            if (pChatParticipant->IsContact())
            {
                CContactPtr pChatContact;
                pChatContact = new CContact(pContact->GetAccount(), pContact->GetName(), pContact->GetAlias(), FALSE);
                
                InternalAddContact(pChatContact);

                m_pConversation->UpdateChatParticipant(pContact->GetName(), pContact->GetAlias(), FALSE);
            }
        }
        else
        {
            // Remove contact
            IUOLMessengerContact3Ptr pContact3 = ap_dynamic_cast<IUOLMessengerContact3Ptr>(pContact);
            if (pContact3)
            {
                InternalRemoveContact(pContact3, TRUE);
            }
        }
    }
}


void CChatListCtrl::OnRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts)
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
            IUOLMessengerContact3Ptr pContact3 = ap_dynamic_cast<IUOLMessengerContact3Ptr>(pContact);
            if (pContact3)
            {
                InternalRemoveContact(pContact3, FALSE);
            }
        }
	}

	UpdateScrollSize();
}


IPainter* CChatListCtrl::GetPainter(UINT nId)
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


void CChatListCtrl::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
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


void CChatListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CChatListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
	CPoint ptMenu = pt;
	::ClientToScreen(m_hWnd, &ptMenu);

	CChatNodePtr pChatNode = ap_dynamic_cast<CChatNodePtr>(pItem);

	if (pChatNode != NULL)
	{
		// Handle contact click...
		if (m_pChatMenu != NULL)
		{
			if (m_pChatMenu->IsBusy())
			{
				return;
			}

			m_pChatMenu->DestroyMenu();
		}
		
		m_pChatMenu = new CChatMenu();
        m_pChatMenu->CreateMenu(pChatNode->GetContact());
		m_pChatMenu->ShowMenu(ptMenu);
        
		return;
	}

    ATLASSERT(FALSE);
}


BOOL CChatListCtrl::IsValidObject(IDataObject *pDataObject)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;

	if (m_pTransferDataObject != pDataObject)
	{
		m_pTransferDataObject = pDataObject;
	}

    if (pDataObject->QueryGetData(&fmtetc) == S_OK)
    {
        if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
        {
            IUOLMessengerContact** ppTransferedContact = (IUOLMessengerContact**) ::GlobalLock(stgmed.hGlobal);

            if (FALSE == ::IsBadReadPtr(ppTransferedContact, sizeof(IUOLMessengerContact**)))
            {
                IUOLMessengerContact* pDroppedContact = *ppTransferedContact;

                if (pDroppedContact)
                {
                    IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(m_pConversation->GetAccount(), pDroppedContact->GetName());
                    if (pContact)
                    {
                        if (pContact->GetStatus() == IUOLMessengerContact::CS_OFFLINE)
                        {
                            return FALSE;
                        }
                        
                        IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

                        // Is the same account?
                        if (m_pChatGroup.get() && pAccount.get() && m_pConversation)
                        {
                            if (IsSameAccount(pAccount))
                            {
                                // Look if this contact is a chat participant
                                IUOLMessengerContact3Ptr pChatParticipant = m_pConversation->FindChatParticipant(pContact->GetName());
                                if (pChatParticipant)
                                {
                                    return FALSE;
                                }
                                else
                                {
                                    return TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}


DWORD CChatListCtrl::DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	DWORD dwEffect = DROPEFFECT_NONE;

	CPoint ptClient(pt.x, pt.y);
	::ScreenToClient(m_hWnd, &ptClient);

	if (dwAllowed & DROPEFFECT_MOVE)
	{
		dwEffect = DROPEFFECT_MOVE;
	}

	return dwEffect;
}


DWORD CChatListCtrl::DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	DWORD dwEffect = DROPEFFECT_NONE;

	CPoint ptClient(pt.x, pt.y);
	::ScreenToClient(m_hWnd, &ptClient);

	if (dwAllowed & DROPEFFECT_MOVE)
	{
		dwEffect = DROPEFFECT_MOVE;
	}

	//ATLTRACE(_T("%s, (%d,%d)\n"), __FUNCTION__, ptClient.x, ptClient.y);

	const int EPSILON = 30;

	if (ptClient.y < EPSILON)
	{
		DoScrolling(SC_SCROLL_UP);
	}
	else
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		if ((rectClient.Height() - ptClient.y) < EPSILON)
		{
			DoScrolling(SC_SCROLL_DOWN);
		}
		else
		{
			DoScrolling(SC_SCROLL_NONE);
		}
	}

	return dwEffect;
}


void CChatListCtrl::DoDragLeave()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	DoScrolling(SC_SCROLL_NONE);

	ClearActiveDropTarget();

	if (m_pTransferDataObject)
	{
		FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stgmed;

		if (m_pTransferDataObject->QueryGetData(&fmtetc) == S_OK)
		{
			if (m_pTransferDataObject->GetData(&fmtetc, &stgmed) == S_OK)
			{
				IUOLMessengerContact** ppTransferedContact = (IUOLMessengerContact**) ::GlobalLock(stgmed.hGlobal);

				if (FALSE == ::IsBadReadPtr(ppTransferedContact, sizeof(IUOLMessengerContact**)))
				{
					ATLTRACE(_T("DoDragLeave pTransferedContact = 0x%x\n"), ppTransferedContact);
				}

				::GlobalUnlock(stgmed.hGlobal);
				::ReleaseStgMedium(&stgmed);

				m_pTransferDataObject = NULL;
			}
		}
	}
}


DWORD CChatListCtrl::DoDrop(IDataObject *pDataObject, POINTL pt)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	DWORD dwEffect = DROPEFFECT_NONE;

    CPoint point(pt.x, pt.y);
    ::ScreenToClient(m_hWnd, &point);

    DoScrolling(SC_SCROLL_NONE);
    ClearActiveDropTarget();

    FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;

    if(pDataObject->QueryGetData(&fmtetc) == S_OK)
    {
        if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
        {
            IUOLMessengerContact** ppTransferedContact = (IUOLMessengerContact**) ::GlobalLock(stgmed.hGlobal);

            if (FALSE == ::IsBadReadPtr(ppTransferedContact, sizeof(IUOLMessengerContact**)))
            {
                IUOLMessengerContact* pDroppedContact = *ppTransferedContact;

                if (pDroppedContact)
                {
                    IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(m_pConversation->GetAccount(), pDroppedContact->GetName());
                    if (pContact)
                    {
                        IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

                        // Is the same account?
                        if (m_pChatGroup.get() && pAccount.get() && m_pConversation)
                        {
                            if (IsSameAccount(pAccount))
                            {
                                dwEffect = DROPEFFECT_MOVE;
                                // Invite this contact to chat.
                                IUOLMessengerCommandPtr pCommand = new CInviteForChatCommand(m_pConversation, pContact, FALSE);
                                IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();

                                if (pCommand)
                                {
                                    pApplication->ExecuteCommand(pCommand);
                                }
                            }
                        }
                    }
                }
            }

            ::GlobalUnlock(stgmed.hGlobal);
            ::ReleaseStgMedium(&stgmed);

            m_pTransferDataObject = NULL;
        }
    }

	return dwEffect;
}



void CChatListCtrl::InternalAddContact(IUOLMessengerContact3Ptr pContact)
{
	CChatNodePtr pChatNode = new CChatNode(pContact, m_strChatKey);

	CString strEntryName = FormatChatNodeName(pContact);

	static_cast<CEntry*>(pChatNode.get())->SetName(strEntryName);
	pChatNode->SetComparatorAdapter(m_pComparatorAdapter.get());
	pChatNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_CONTACT_DEFAULT_PAINTER));
	pChatNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_CONTACT_SELECTION_PAINTER));

	pChatNode->SetTextFont(m_pContactElement->GetTextFont());
	pChatNode->SetTextColor(m_pContactElement->GetTextColor());
	pChatNode->SetSelectedTextFont(m_pContactElement->GetSelectedTextFont());
	pChatNode->SetSelectedTextColor(m_pContactElement->GetSelectedTextColor());
	pChatNode->SetTextFontBlocked(m_pContactElement->GetBlockedTextFont());
	pChatNode->SetTextColorBlocked(m_pContactElement->GetBlockedTextColor());

	static_cast<CEntry*>(pChatNode.get())->SetHeight(NODE_HEIGHT);

	if (!m_pChatGroup)
	{
		CreateChatGroup();
	}

	pChatNode->SetAllContactsGroup(m_pChatGroup->GetName());

	AddEntry(m_pChatGroup, pChatNode);
}


void CChatListCtrl::InternalRemoveContact(IUOLMessengerContact3Ptr pContact, BOOL bRedraw)
{
	if (m_pChatGroup)
	{
		CString strContactEntry = FormatChatNodeName(pContact);
		IUOLMessengerChatNodePtr pChatNode = ap_dynamic_cast<IUOLMessengerChatNodePtr>(GetEntryByName(strContactEntry, m_pChatGroup->GetName()));

		// Let's remove the account group reference first, so the redraw can be happen 
		// after the default group reference remove...
		//
		CString strName;
        strName.Format(_T("%s:%s"), pContact->GetAccount()->GetProtocolId(), 
            pContact->GetAccount()->GetUserId());
        RemoveEntry(strContactEntry, strName);
		RemoveEntry(strContactEntry, m_pChatGroup->GetName(), bRedraw);
	}
}


CString CChatListCtrl::FormatChatNodeName(IUOLMessengerContact3Ptr pContact)
{
	return FormatChatNodeName(pContact->GetName(), pContact->GetAccount()->GetUserId(), pContact->GetAccount()->GetProtocolId());
}


CString CChatListCtrl::FormatChatNodeName(const CString& strContact, const CString& strAccountUserId, const CString& strAccountProtocolId)
{
	CString strName;
	strName.Format(_T("%s/%s/%s"), strContact, strAccountUserId, strAccountProtocolId);

	return strName;
}


int CChatListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CChatListCtrl::GetContactHeight(SizeImage sizeImage) const
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


DWORD CChatListCtrl::QueryDragEffect(DWORD /*grfKeyState*/, const CPoint& ptClient)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DWORD dwEffect = DROPEFFECT_NONE;

    CGroupListCtrlItemPtr pItem = Pt2Item(ptClient);
    CEntryGroupPtr pEntryGroup;

    if (pItem.get())
    {
        dwEffect = DROPEFFECT_MOVE;

        CChatNodePtr pChatNode = ap_dynamic_cast<CChatNodePtr>(pItem);

        if (pChatNode.get())
        {
            IUOLMessengerContact3Ptr pContact = pChatNode->GetContact();

            if (pContact.get())
            {
                if (m_pChatGroup.get())
                {
                    pEntryGroup = GetGroup(m_pChatGroup->GetName());
                }
            }
        }
        
        if (pEntryGroup.get())
        {
            SetActiveDropTarget(pEntryGroup);
        }
    }
    else
    {
        ClearActiveDropTarget();
    }

	return dwEffect;
}


void CChatListCtrl::DoScrolling(ScrollCommand scCommand)
{
	m_scrollCommand = scCommand;

	if (scCommand == SC_SCROLL_NONE)
	{
		if (0 != m_nTimerScroll)
		{
			//ATLTRACE(_T("Stop scrolling\n"));
			KillTimer(TIMER_SCROLL_VIEW);
			m_nTimerScroll = 0;
		}
	}
	else 
	{
		if (0 == m_nTimerScroll)
		{
			if (scCommand == SC_SCROLL_UP)
			{
				//ATLTRACE(_T("ScrollUp\n"));
				ScrollLineUp();
			}
			else
			{
				//ATLTRACE(_T("ScrollDown\n"));
				ScrollLineDown();
			}

			m_nTimerScroll = SetTimer(TIMER_SCROLL_VIEW, 100, NULL);
		}
	}
}

void CChatListCtrl::GetContacts(CAtlList<IUOLMessengerChatNodePtr>& listContacts)
{
	struct ContactsHelper : public CGroupListCtrl::Callback
	{
		CAtlList<IUOLMessengerChatNodePtr>* m_pListContacts;
		BOOL m_bCurrentGroupVisible;

		ContactsHelper(CAtlList<IUOLMessengerChatNodePtr>& listContacts) :
		m_pListContacts(&listContacts),
			m_bCurrentGroupVisible(FALSE){}

			BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
			{
				if (pItem)
				{
					IUOLMessengerChatNodePtr pContact = ap_dynamic_cast<IUOLMessengerChatNodePtr>(pItem);

					if (pContact)
					{
						if (m_bCurrentGroupVisible)
						{
							m_pListContacts->AddTail(pContact);
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

IUOLMessengerImageButton2Ptr CChatListCtrl::CreateContactButton()
{
	IUOLMessengerImageButton2Ptr pButton = new CImageButton();
	return pButton;
}

IUOLMessengerContextMenuItemPtr CChatListCtrl::CreateContextMenuItem(const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback)
{
	IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(15, (LPCTSTR) strLabel, pCallback);
	return pMenuItem;
}


void CChatListCtrl::SetChatKey(const CString& strChatKey)
{
    m_strChatKey = strChatKey;
}


void CChatListCtrl::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
    m_pConversation = pConversation;
}


BOOL CChatListCtrl::IsSameAccount(IUOLMessengerAccountPtr pAccount)
{
    CString strConvUserId = m_pConversation->GetAccount()->GetUserId();
    CString strAccountUserId = pAccount->GetUserId();

    return (0 == strAccountUserId.Compare(strConvUserId));
}


void CChatListCtrl::CreateChatGroup()
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
    CGroupChatNodePtr pGroupChatNode = new CGroupChatNode();
	AddGroup(pGroupChatNode);
	m_pChatGroup = pGroupChatNode;
}


void CChatListCtrl::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
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
