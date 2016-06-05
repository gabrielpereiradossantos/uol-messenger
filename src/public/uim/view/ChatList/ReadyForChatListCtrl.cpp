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
#include "ReadyForChatListCtrl.h"

#include "ReadyForChatNode.h"
#include "GroupReadyForChatNode.h"

#include "../../controller/UIMApplication.h"

#include <commands/InviteForChatCommand.h>


const int CReadyForChatListCtrl::TIMER_SCROLL_VIEW = 150;
const int CReadyForChatListCtrl::NODE_HEIGHT = 15;


CReadyForChatListCtrl::CReadyForChatListCtrl() :
	m_pGroupElement(NULL),
	m_pContactElement(NULL),
	m_scrollCommand(SC_SCROLL_NONE),
	m_nTimerScroll(0),
	m_nScrollLineHeight(0),
	m_pTransferDataObject(NULL)
{

	//m_nWheelLines = 5;
}


CReadyForChatListCtrl::~CReadyForChatListCtrl()
{
	if (m_pReadyForChatMenu != NULL)
	{
		m_pReadyForChatMenu->DestroyMenu();
	}
}


void CReadyForChatListCtrl::Init(IUOLMessengerContactListPtr pContactList)
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


void CReadyForChatListCtrl::Finalize()
{
	if (m_pContactList != NULL)
	{
		m_pContactList->UnregisterObserver(this);
	}
}


IUOLMessengerContactPtr CReadyForChatListCtrl::GetSelectedContact()
{
	CReadyForChatNodePtr pReadyForChatNode = ap_static_cast<CReadyForChatNodePtr>(GetSelectedItem());

	if (pReadyForChatNode != NULL)
	{
		return pReadyForChatNode->GetContact();
	}

	return IUOLMessengerContactPtr();
}


void CReadyForChatListCtrl::SetOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits)
{
	if (m_pComparatorAdapter)
	{
		m_pComparatorAdapter->SetOrderTraits(pOrderTraits);

	}
}


IUOLMessengerContactListOrderTraitsPtr CReadyForChatListCtrl::GetOrderTraits()
{
	IUOLMessengerContactListOrderTraitsPtr pOrderTraits;

	if (m_pComparatorAdapter)
	{
		pOrderTraits = m_pComparatorAdapter->GetOrderTraits();
	}

	return pOrderTraits;
}


void CReadyForChatListCtrl::ChangeContactPainter(IUOLMessengerContact* pContact, UINT nPainterId)
{
	if (pContact != NULL)
	{
		IUOLMessengerAccount* pAccount = pContact->GetAccount().get();
		IUOLMessengerGroupPtr pGroup = pContact->GetGroup();

		if ((pAccount != NULL) && (pGroup != NULL))
		{
			CString strGroupName = pGroup->GetName();
			CString strEntryName = FormatReadyForChatNodeName(pContact);

			CEntryPtr pEntry;
			
			pEntry = GetEntryByName(strEntryName, strGroupName);
			
            if (pEntry != NULL)
			{
				CReadyForChatNode* pReadyForChatNode = dynamic_cast<CReadyForChatNode*>(pEntry.get());
				IPainter* pPainter = GetPainter(nPainterId);

				if ((pReadyForChatNode != NULL) && (pPainter != NULL))
				{
					pReadyForChatNode->SetPainter(pPainter);
					pReadyForChatNode->Invalidate();
				}
			}
		}
	}
}


CString CReadyForChatListCtrl::GetName() const
{
	return "__ReadyForChatListInstance_" + m_strGUID;
}


void CReadyForChatListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}


void CReadyForChatListCtrl::GetMinSize(CSize& size)
{
	return CGroupListCtrl::GetSize(size);
}


void CReadyForChatListCtrl::AddChatContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}

    InternalAddContact(pContact);
}


void CReadyForChatListCtrl::OnAddContact(IUOLMessengerContactPtr pContact)
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


void CReadyForChatListCtrl::OnRemoveContact(IUOLMessengerContactPtr pContact)
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


void CReadyForChatListCtrl::OnRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts)
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


IPainter* CReadyForChatListCtrl::GetPainter(UINT nId)
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


void CReadyForChatListCtrl::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
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


void CReadyForChatListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CReadyForChatListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
	CPoint ptMenu = pt;
	::ClientToScreen(m_hWnd, &ptMenu);

	CReadyForChatNodePtr pReadyForChatNode = ap_dynamic_cast<CReadyForChatNodePtr>(pItem);

	if (pReadyForChatNode != NULL)
	{
		// Handle contact click...
		if (m_pReadyForChatMenu != NULL)
		{
			if (m_pReadyForChatMenu->IsBusy())
			{
				return;
			}

			m_pReadyForChatMenu->DestroyMenu();
		}
		
		m_pReadyForChatMenu = new CReadyForChatMenu();
		m_pReadyForChatMenu->CreateMenu(pReadyForChatNode->GetContact(), m_pConversation);
		
		GetAdditionalContextMenuItems(pReadyForChatNode);
		
		m_pReadyForChatMenu->ShowMenu(ptMenu);
		
		return;
	}

    ATLASSERT(FALSE);
}


void CReadyForChatListCtrl::OnLBtnDblClick(GroupListCtrl::CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
    CPoint ptMenu = pt;
	::ClientToScreen(m_hWnd, &ptMenu);

	CReadyForChatNodePtr pReadyForChatNode = ap_dynamic_cast<CReadyForChatNodePtr>(pItem);

	if (pReadyForChatNode != NULL)
	{
		// Handle contact double click
        IUOLMessengerContactPtr pContact = pReadyForChatNode->GetContact();

        if (pContact)
        {
            if (m_pConversation)
            {
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


BOOL CReadyForChatListCtrl::IsValidObject(IDataObject *pDataObject)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	if (m_pTransferDataObject != pDataObject)
	{
		m_pTransferDataObject = pDataObject;
	}

	return (pDataObject->QueryGetData(&fmtetc) == S_OK);
}


DWORD CReadyForChatListCtrl::DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	return DROPEFFECT_NONE;
}


DWORD CReadyForChatListCtrl::DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	return DROPEFFECT_NONE;
}


void CReadyForChatListCtrl::DoDragLeave()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

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


DWORD CReadyForChatListCtrl::DoDrop(IDataObject *pDataObject, POINTL pt)
{
	return DROPEFFECT_NONE;
}



void CReadyForChatListCtrl::InternalAddContact(IUOLMessengerContactPtr pContact)
{
    CReadyForChatNodePtr pReadyForChatNode = new CReadyForChatNode(pContact, m_strChatKey);

	CString strEntryName = FormatReadyForChatNodeName(pContact.get());

	static_cast<CEntry*>(pReadyForChatNode.get())->SetName(strEntryName);
	pReadyForChatNode->SetComparatorAdapter(m_pComparatorAdapter.get());
	pReadyForChatNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_CONTACT_DEFAULT_PAINTER));
	pReadyForChatNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_CONTACT_SELECTION_PAINTER));

	pReadyForChatNode->SetTextFont(m_pContactElement->GetTextFont());
	pReadyForChatNode->SetTextColor(m_pContactElement->GetTextColor());
	pReadyForChatNode->SetSelectedTextFont(m_pContactElement->GetSelectedTextFont());
	pReadyForChatNode->SetSelectedTextColor(m_pContactElement->GetSelectedTextColor());
	pReadyForChatNode->SetDropTargetTextFont(m_pContactElement->GetDropTargetTextFont());
	pReadyForChatNode->SetDropTargetTextColor(m_pContactElement->GetDropTargetTextColor());
	pReadyForChatNode->SetTextFontBlocked(m_pContactElement->GetBlockedTextFont());
	pReadyForChatNode->SetTextColorBlocked(m_pContactElement->GetBlockedTextColor());

	static_cast<CEntry*>(pReadyForChatNode.get())->SetHeight(NODE_HEIGHT);

	if (!m_pReadyForChatGroup)
	{
        CreateReadyForChatGroup();
	}

	pReadyForChatNode->SetAllContactsGroup(m_pReadyForChatGroup->GetName());

	AddEntry(m_pReadyForChatGroup, pReadyForChatNode);
}


void CReadyForChatListCtrl::InternalRemoveContact(IUOLMessengerContactPtr pContact, BOOL bRedraw)
{
	if (m_pReadyForChatGroup && pContact)
	{
		CString strContactEntry = FormatReadyForChatNodeName(pContact.get());
		IUOLMessengerReadyForChatNodePtr pReadyForChatNode = ap_dynamic_cast<IUOLMessengerReadyForChatNodePtr>(GetEntryByName(strContactEntry, m_pReadyForChatGroup->GetName()));

		// Let's remove the account group reference first, so the redraw can be happen 
		// after the default group reference remove...
		//
		CString strName;
        strName.Format(_T("%s:%s"), pContact->GetAccount()->GetProtocolId(), 
            pContact->GetAccount()->GetUserId());
        RemoveEntry(strContactEntry, strName);
		RemoveEntry(strContactEntry, m_pReadyForChatGroup->GetName(), bRedraw);
	}
}


CString CReadyForChatListCtrl::FormatReadyForChatNodeName(IUOLMessengerContact* pContact)
{
	return FormatReadyForChatNodeName(pContact->GetName(), pContact->GetAccount()->GetUserId(), pContact->GetAccount()->GetProtocolId());
}


CString CReadyForChatListCtrl::FormatReadyForChatNodeName(const CString& strContact, const CString& strAccountUserId, const CString& strAccountProtocolId)
{
	CString strName;
	strName.Format(_T("%s/%s/%s"), strContact, strAccountUserId, strAccountProtocolId);

	return strName;
}


int CReadyForChatListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CReadyForChatListCtrl::GetContactHeight(SizeImage sizeImage) const
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


void CReadyForChatListCtrl::GetContacts(CAtlList<IUOLMessengerReadyForChatNodePtr>& listContacts)
{
	struct ContactsHelper : public CGroupListCtrl::Callback
	{
		CAtlList<IUOLMessengerReadyForChatNodePtr>* m_pListContacts;
		BOOL m_bCurrentGroupVisible;

		ContactsHelper(CAtlList<IUOLMessengerReadyForChatNodePtr>& listContacts) :
		m_pListContacts(&listContacts),
			m_bCurrentGroupVisible(FALSE){}

			BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
			{
				if (pItem)
				{
					IUOLMessengerReadyForChatNodePtr pReadyForChatNode = ap_dynamic_cast<IUOLMessengerReadyForChatNodePtr>(pItem);

					if (pReadyForChatNode)
					{
						if (m_bCurrentGroupVisible)
						{
							m_pListContacts->AddTail(pReadyForChatNode);
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

IUOLMessengerImageButton2Ptr CReadyForChatListCtrl::CreateContactButton()
{
	IUOLMessengerImageButton2Ptr pButton = new CImageButton();
	return pButton;
}

IUOLMessengerContextMenuItemPtr CReadyForChatListCtrl::CreateContextMenuItem(const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback)
{
	IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(15, (LPCTSTR) strLabel, pCallback);
	return pMenuItem;
}

void CReadyForChatListCtrl::RegisterObserver(CUOLMessengerReadyForChatListCtrlObserver* pObserver)
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


void CReadyForChatListCtrl::UnregisterObserver(CUOLMessengerReadyForChatListCtrlObserver* pObserver)
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

void CReadyForChatListCtrl::NotifyObservers(void (CUOLMessengerReadyForChatListCtrlObserver::*pfnCallback)(IUOLMessengerReadyForChatNodePtr), IUOLMessengerReadyForChatNodePtr pContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerReadyForChatListCtrlObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(pContact);
	}

}


void CReadyForChatListCtrl::GetAdditionalContextMenuItems(IUOLMessengerReadyForChatNodePtr pReadyForChatNode)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);
	
	if ( (m_pReadyForChatMenu) && (pReadyForChatNode) )
	{
		for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
		{
			CUOLMessengerReadyForChatListCtrlObserver* pObserver = m_listObservers.GetNext(pos);
			
			pObserver->OnShowContextMenu(
					ap_static_cast<IUOLMessengerContextMenuPtr>(m_pReadyForChatMenu->GetContextMenu()), 
					pReadyForChatNode);
		}
	}
}


void CReadyForChatListCtrl::SetChatKey(const CString& strChatKey)
{
    m_strChatKey = strChatKey;
}


void CReadyForChatListCtrl::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
    m_pConversation = pConversation;
}


BOOL CReadyForChatListCtrl::IsSameAccount(IUOLMessengerAccountPtr pAccount)
{
    CString strConvUserId = m_pConversation->GetAccount()->GetUserId();
    CString strAccountUserId = pAccount->GetUserId();

    return (0 == strAccountUserId.Compare(strConvUserId));
}


void CReadyForChatListCtrl::CreateReadyForChatGroup()
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
    CGroupReadyForChatNodePtr pGroupChatNode = new CGroupReadyForChatNode(m_pConversation->GetAccount());
	AddGroup(pGroupChatNode);
	m_pReadyForChatGroup = pGroupChatNode;
}


void CReadyForChatListCtrl::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
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


void CReadyForChatListCtrl::OnKeydown(TCHAR ch, UINT, UINT)
{
	char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*()[{}]/?\\|";

    if (strchr(validChars, ch))
    {
		SelectItemStartingBy(ch);
	}
}


void CReadyForChatListCtrl::SelectItemStartingBy(TCHAR ch)
{
    struct SelectItemStartingByHelper : public CGroupListCtrl::Callback
	{
		SelectItemStartingByHelper(CGroupListCtrl* pGroupListCtrl, TCHAR ch, CGroupListCtrlItemPtr pItemSelected) :
			m_pGroupListCtrl(pGroupListCtrl), 
            m_strChar(ch),
			m_pItemSelected(pItemSelected) {}

		BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
		{
			if (bIsVisible)
			{
				if (pItem)
				{
					if (!pItem->GetSelected())
					{
						IUOLMessengerReadyForChatNodePtr pReadyForChatNode = ap_dynamic_cast<IUOLMessengerReadyForChatNodePtr>(pItem);
						if (pReadyForChatNode)
						{
							IUOLMessengerContactPtr pContact = pReadyForChatNode->GetContact();
							if (pContact)
							{
								CString strName = pContact->GetAlias();
								CString strFirstChar = strName.Mid(0, 1);
								strFirstChar.MakeLower();
								m_strChar.MakeLower();

								if (0 == strFirstChar.Compare(m_strChar))
								{					
									if (m_pItemSelected)
									{
										m_pItemSelected->SetSelected(FALSE);
										m_pItemSelected->Invalidate();
									}
									pItem->SetSelected(TRUE);
									pItem->Invalidate();

									return TRUE;
								}
							}
						}
					}
					m_pGroupListCtrl->Invalidate();
				}							
			}

			return FALSE;
		}		

	private:
		CGroupListCtrl* m_pGroupListCtrl;
		CGroupListCtrlItemPtr m_pItemSelected;
        CString m_strChar;
	};

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csList);
		CGroupListCtrlItemPtr pItemSelected;
		pItemSelected = GetSelectedItem(IterateOptions::ioEntries);
		IterateThroughGroupList(SelectItemStartingByHelper(this, ch, pItemSelected), IterateOptions::ioEntries);
	}

	UpdateWindow();

	CGroupListCtrlItemPtr pItem = GetSelectedItem(CGroupListCtrl::ioALL);

	if (pItem.get())
	{
		CRect pRect = pItem->GetRect();
		ScrollToView(pRect);
	}
}