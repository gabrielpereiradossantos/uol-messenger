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
#include "ContactListCtrl.h"

#include "ContactNode.h"
#include "GroupNode.h"
#include "DummyGroupNode.h"
#include "DummyContactNode.h"
#include "GroupAccountNode.h"
#include "DummyGroupFindNode.h"
#include "GroupFindNode.h"

#include "../../controller/UIMApplication.h"
#include "../FindContactPanel.h"

#include <commands/SendDroppedFileCommand.h>


const int CContactListCtrl::TIMER_SCROLL_VIEW = 150;


CContactListCtrl::CContactListCtrl() :
	CWindowElement<CContactListCtrl, CGroupListCtrlTarget>(),
	m_pGroupElement(NULL),
	m_pContactElement(NULL),
	m_scrollCommand(SC_SCROLL_NONE),
	m_nTimerScroll(0),
	m_nScrollLineHeight(0),
	m_pTransferDataObject(NULL),
	m_bGroupByAccounts(FALSE)
{
}


CContactListCtrl::~CContactListCtrl()
{
	if (m_pGroupMenu != NULL)
	{
		m_pGroupMenu->DestroyMenu();
	}

	if (m_pContactMenu != NULL)
	{
		m_pContactMenu->DestroyMenu();
	}
}


void CContactListCtrl::Init(IUOLMessengerContactListPtr pContactList)
{
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
	
	IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
			ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
	
	m_bGroupByAccounts = pContactListSettings2->GetGroupByAccounts();
}


void CContactListCtrl::Finalize()
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->StopListCtrlThread();
	}
	
	if (m_pContactList != NULL)
	{
		m_pContactList->UnregisterObserver(this);
	}
}


LRESULT CContactListCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CUIMApplication::GetApplication()->AddMessageFilter(this);
	
	m_pContactListCtrlThread = new CContactListCtrlThread(m_hWnd);
	
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->StartListCtrlThread();
	}

	bHandled = FALSE;

	return 0L;
}


LRESULT CContactListCtrl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CUIMApplication::GetApplication()->RemoveMessageFilter(this);

	bHandled = FALSE;

	return 0L;
}


IUOLMessengerContactPtr CContactListCtrl::GetSelectedContact()
{
	CContactNodePtr pContactNode = ap_static_cast<CContactNodePtr>(GetSelectedItem());

	if (pContactNode != NULL)
	{
		return pContactNode->GetContact();
	}

	return IUOLMessengerContactPtr();
}


void CContactListCtrl::SetOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits)
{
	if (m_pComparatorAdapter)
	{
		m_pComparatorAdapter->SetOrderTraits(pOrderTraits);

	}
}


IUOLMessengerContactListOrderTraitsPtr CContactListCtrl::GetOrderTraits()
{
	IUOLMessengerContactListOrderTraitsPtr pOrderTraits;

	if (m_pComparatorAdapter)
	{
		pOrderTraits = m_pComparatorAdapter->GetOrderTraits();
	}

	return pOrderTraits;
}


void CContactListCtrl::ChangeContactPainter(IUOLMessengerContact* pContact, UINT nPainterId)
{
	if (pContact != NULL)
	{
		IUOLMessengerAccount* pAccount = pContact->GetAccount().get();
		IUOLMessengerGroupPtr pGroup = pContact->GetGroup();

		if ((pAccount != NULL) && (pGroup != NULL))
		{
			CString strGroupName = pGroup->GetName();
			CString strEntryName = FormatContactNodeName(pContact);

			CEntryPtr pEntry;
			
			if (!m_bGroupByAccounts)
			{
                pEntry = GetEntryByName(strEntryName, strGroupName);
			}
			else
			{
				CString strName;
                strName.Format(_T("%s:%s"), pContact->GetAccount()->GetProtocolId(), 
                    pContact->GetAccount()->GetUserId());
                pEntry = GetEntryByName(strEntryName, strName);
			}

			if (pEntry != NULL)
			{
				CContactNode* pContactNode = dynamic_cast<CContactNode*>(pEntry.get());
				IPainter* pPainter = GetPainter(nPainterId);

				if ((pContactNode != NULL) && (pPainter != NULL))
				{
					pContactNode->SetPainter(pPainter);
					pContactNode->Invalidate();
				}
			}
		}
	}
}


void CContactListCtrl::ChangeGroupPainter(IUOLMessengerGroup* pGroup, UINT nPainterId)
{
	if (pGroup != NULL)
	{
		CEntryGroup* pEntryGroup = GetGroup(pGroup->GetName()).get();

		if (pEntryGroup != NULL)
		{
			CGroupNode* pGroupNode = dynamic_cast<CGroupNode*>(pEntryGroup);

			IPainter* pPainter = GetPainter(nPainterId);

			if ((pGroupNode != NULL) && (pPainter != NULL))
			{
				pGroupNode->SetPainter(pPainter);
				pGroupNode->Invalidate();
			}
		}
	}
}

CString CContactListCtrl::GetName() const
{
	return "__ContactListInstance";
}


void CContactListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}


void CContactListCtrl::GetMinSize(CSize& size)
{
	return CGroupListCtrl::GetSize(size);
}


void CContactListCtrl::OnAddContact(IUOLMessengerContactPtr pContact)
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncAddContact(pContact);
	}
}


void CContactListCtrl::OnRemoveContact(IUOLMessengerContactPtr pContact)
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncRemoveContact(pContact);
	}
}


void CContactListCtrl::OnRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncRemoveContacts(listContacts);
	}
}


void CContactListCtrl::OnMoveContact(IUOLMessengerContactPtr pContact, const CString& strOldGroup)
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncMoveContact(pContact, strOldGroup);
	}
}


void CContactListCtrl::OnAddGroup(IUOLMessengerGroupPtr pGroup)
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncAddGroup(pGroup);
	}
}


void CContactListCtrl::OnRemoveGroup(IUOLMessengerGroupPtr pGroup)
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncRemoveGroup(pGroup);
	}
}


void CContactListCtrl::OnRemoveAllGroups()
{
	if (m_pContactListCtrlThread)
	{
		m_pContactListCtrlThread->AsyncRemoveAllGroups();
	}
}


void CContactListCtrl::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
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
		else if (IUOLMessengerContactListSettings::CL_GROUP_BY_ACCOUNTS == nSettingId)
		{
			bRedraw = TRUE;
			BOOL bGroupByAccounts;
			
			IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
					ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
							CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
			
			bGroupByAccounts = pContactListSettings2->GetGroupByAccounts();
			
			if (bGroupByAccounts != m_bGroupByAccounts)
			{
				m_bGroupByAccounts = bGroupByAccounts;
				
				Sort();
				UpdateItemSelection();
			}
		}

		if (bRedraw)
		{
			UpdateScrollSize();
		}
	}
}


IPainter* CContactListCtrl::GetPainter(UINT nId)
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


void CContactListCtrl::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
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


void CContactListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CContactListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
	CPoint ptMenu = pt;
	::ClientToScreen(m_hWnd, &ptMenu);

	CGroupAccountNodePtr pGroupAccountNode = ap_dynamic_cast<CGroupAccountNodePtr>(pItem);
	
	if (pGroupAccountNode != NULL)
	{
		// GroupAccount groups don't show the general group menu...
		//
		return;
	}

	CGroupNodePtr pGroupNode = ap_dynamic_cast<CGroupNodePtr>(pItem);

	if (pGroupNode != NULL)
	{
		// Handle group click...
		if (m_pGroupMenu != NULL)
		{
			if (m_pGroupMenu->IsBusy())
			{
				return;
			}

			m_pGroupMenu->DestroyMenu();
		}

		m_pGroupMenu = new CGroupMenu();
		m_pGroupMenu->CreateMenu(pGroupNode->GetGroup());
		m_pGroupMenu->ShowMenu(ptMenu);
		return;
	}

	CContactNodePtr pContactNode = ap_dynamic_cast<CContactNodePtr>(pItem);

	if (pContactNode != NULL)
	{
		// Handle contact click...
		if (m_pContactMenu != NULL)
		{
			if (m_pContactMenu->IsBusy())
			{
				return;
			}

			m_pContactMenu->DestroyMenu();
		}
		
		m_pContactMenu = new CContactMenu();
		m_pContactMenu->CreateMenu(pContactNode->GetContact());
		
		GetAdditionalContextMenuItems(pContactNode);
		
		m_pContactMenu->ShowMenu(ptMenu);
		
		return;
	}

    CGroupFindNodePtr pGroupFindNode = ap_dynamic_cast<CGroupFindNodePtr>(pItem);
	
	if (pGroupFindNode != NULL)
	{
		// GroupFind groups don't show the general group menu...
		//
		return;
	}

	ATLASSERT(FALSE);
}


void CContactListCtrl::OnChar(TCHAR ch, UINT, UINT)
{
    CGroupListCtrl::OnChar(ch, 0, 0);

	NotifyObservers(&CUOLMessengerContactListCtrlObserver2::OnContactListChar, ch);

	SetMsgHandled(FALSE);
}


BOOL CContactListCtrl::IsValidObject(IDataObject *pDataObject)
{
	if (!pDataObject)
	{
		return FALSE;
	}
	
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	// Disable drag and drop if no groups are visible
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	IUOLMessengerContactListSettingsPtr pContactListSettings = pApplication->GetSettingsManager()->GetContactListSettings();

	if (!pContactListSettings->IsShowGroupsEnabled())
	{
		return FALSE;
	}

	// Dragging Contact
	FORMATETC fmtetcContact = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// Dragging Shell Files
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	if (m_pTransferDataObject != pDataObject)
	{
		m_pTransferDataObject = pDataObject;
	}

	// Accept "Contact" and "Shell Files"
	return (pDataObject->QueryGetData(&fmtetcContact) == S_OK) || 
		   (pDataObject->QueryGetData(&fmtetcFiles) == S_OK);
}


DWORD CContactListCtrl::DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	CPoint ptClient(pt.x, pt.y);
	::ScreenToClient(m_hWnd, &ptClient);

	DWORD dwEffect = DROPEFFECT_NONE;
	
	if (m_pTransferDataObject)
	{
		// Verify if drag object is Shell File
		FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if (dwAllowed && (m_pTransferDataObject->QueryGetData(&fmtetcFiles) == S_OK))
		{
			dwEffect = DROPEFFECT_COPY;
			return dwEffect;
		}

		// Verify if drag object is Contact
		FORMATETC fmtetcContact = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if (dwAllowed && (m_pTransferDataObject->QueryGetData(&fmtetcContact) == S_OK))
		{
			dwEffect = QueryDragEffect(grfKeyState, ptClient);
			return dwEffect;
		}
	}	

	return dwEffect;
}


DWORD CContactListCtrl::DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	CPoint ptClient(pt.x, pt.y);
	::ScreenToClient(m_hWnd, &ptClient);

	DWORD dwEffect = DROPEFFECT_NONE;

	
	if (m_pTransferDataObject)
	{
		// Verify if drag object is Shell File
		FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if (dwAllowed && (m_pTransferDataObject->QueryGetData(&fmtetcFiles) == S_OK))
		{
			dwEffect = DROPEFFECT_COPY;

			// Find node at point
			CGroupListCtrlItemPtr pItem = Pt2Item(ptClient);
			if (pItem.get())
			{
				if (!IsGroup(pItem))
				{
					CContactNodePtr pContactNode = ap_dynamic_cast<CContactNodePtr>(pItem);
					if (pContactNode.get())
					{
						SelectItem(pItem);
					}
				}
			}
		}

		// Verify if drag object is Contact
		FORMATETC fmtetcContact = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if (dwAllowed && (m_pTransferDataObject->QueryGetData(&fmtetcContact) == S_OK))
		{
			dwEffect = QueryDragEffect(grfKeyState, ptClient);
		}
			
		// Scroll list if needed
		if ((m_pTransferDataObject->QueryGetData(&fmtetcContact) == S_OK) || 
			(m_pTransferDataObject->QueryGetData(&fmtetcFiles) == S_OK))
		{		
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
		}
	}

	return dwEffect;
}


void CContactListCtrl::DoDragLeave()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (m_pTransferDataObject)
	{
		DoScrolling(SC_SCROLL_NONE);
		ClearActiveDropTarget();

		m_pTransferDataObject = NULL;
	}
}


DWORD CContactListCtrl::DoDrop(IDataObject *pDataObject, POINTL pt)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	DWORD dwEffect = DROPEFFECT_NONE;

	if (m_pTransferDataObject)
	{
		// Verify if drag object is Shell File
		FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if (pDataObject->QueryGetData(&fmtetcFiles) == S_OK)
		{
			ProcessDropFiles(pDataObject);

			return dwEffect;
		}

		// Verify if drag object is Contact
		FORMATETC fmtetcContact = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if (pDataObject->QueryGetData(&fmtetcContact) == S_OK)
		{
			if (!m_bGroupByAccounts)
			{
				CPoint point(pt.x, pt.y);
				::ScreenToClient(m_hWnd, &point);

				CGroupListCtrlItemPtr pItem = Pt2Item(point);
				CEntryGroupPtr pEntryGroup;

				if (pItem.get())
				{
					if (IsGroup(pItem))
					{
						pEntryGroup = ap_dynamic_cast<CEntryGroupPtr>(pItem);
					}
					else
					{
						CContactNodePtr pContactNode = ap_dynamic_cast<CContactNodePtr>(pItem);

						if (pContactNode.get())
						{
							IUOLMessengerContactPtr pContact = pContactNode->GetContact();

							if (pContact.get())
							{
								IUOLMessengerGroupPtr pGroup = pContact->GetGroup();

								if (pGroup.get())
								{
									pEntryGroup = GetGroup(pGroup->GetName());
								}
							}
						}
					}
				}

				if (pEntryGroup.get())
				{
					DoScrolling(SC_SCROLL_NONE);
					ClearActiveDropTarget();

					STGMEDIUM stgmed;
					if(pDataObject->GetData(&fmtetcContact, &stgmed) == S_OK)
					{
						IUOLMessengerContact** ppTransferedContact = (IUOLMessengerContact**) ::GlobalLock(stgmed.hGlobal);

						if (FALSE == ::IsBadReadPtr(ppTransferedContact, sizeof(IUOLMessengerContact**)))
						{
							IUOLMessengerContact* pContact = *ppTransferedContact;

							if (pContact)
							{
								IUOLMessengerGroupPtr pGroup = pContact->GetGroup();
								IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

								if (pGroup.get() && pAccount.get())
								{
									dwEffect = DROPEFFECT_MOVE;

									CUIMApplication::GetApplication()->GetRemoteContactList()->MoveContact(pAccount, pContact->GetName(), pGroup->GetName(), pEntryGroup->GetName());
								}
							}
						}

						::GlobalUnlock(stgmed.hGlobal);
						::ReleaseStgMedium(&stgmed);

						m_pTransferDataObject = NULL;
					}
				}
			}
			
			return dwEffect;
		}
	}

	return dwEffect;
}



void CContactListCtrl::AsyncAddContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	IUOLMessengerGroupPtr pGroup = pContact->GetGroup();
	
	if (pGroup != NULL)
	{
		InternalAddContact(pContact, pGroup);
	}
}


void CContactListCtrl::AsyncRemoveContact(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	InternalRemoveContact(pContact, TRUE);
}


void CContactListCtrl::AsyncRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	for (POSITION pos = listContacts.GetHeadPosition(); pos;)
	{
		IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);
		
		InternalRemoveContact(pContact, FALSE);
	}
	
	UpdateScrollSize();
}


void CContactListCtrl::AsyncMoveContact(IUOLMessengerContactPtr pContact, const CString& strOldGroup)
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	if (!m_bGroupByAccounts)
	{
		RemoveEntry(FormatContactNodeName(pContact.get()), strOldGroup);
		
		IUOLMessengerGroupPtr pGroup = pContact->GetGroup();
		
		if (pGroup != NULL)
		{
			InternalAddContact(pContact, pGroup);
		}
	}
}


void CContactListCtrl::AsyncAddGroup(IUOLMessengerGroupPtr pGroup)
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	InternalAddGroup(pGroup);
}


void CContactListCtrl::AsyncRemoveGroup(IUOLMessengerGroupPtr pGroup)
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	CEntryGroupPtr pEntryGroup = GetGroup(pGroup->GetName());
	
	RemoveGroup(pGroup->GetName());
}


void CContactListCtrl::AsyncRemoveAllGroups()
{
	if (FALSE == IsWindow())
	{
		return;
	}
	
	RemoveAll();
}


void CContactListCtrl::InternalAddContact(IUOLMessengerContactPtr pContact, IUOLMessengerGroupPtr pGroup)
{
	CEntryGroupPtr pEntryGroup = GetGroup(pGroup->GetName());

	CContactNodePtr pContactNode = new CContactNode(pContact);

	CString strEntryName = FormatContactNodeName(pContact.get());

	static_cast<CEntry*>(pContactNode.get())->SetName(strEntryName);
	pContactNode->SetComparatorAdapter(m_pComparatorAdapter.get());
	pContactNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_CONTACT_DEFAULT_PAINTER));
	pContactNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_CONTACT_SELECTION_PAINTER));

	pContactNode->SetTextFont(m_pContactElement->GetTextFont());
	pContactNode->SetTextColor(m_pContactElement->GetTextColor());
	pContactNode->SetSelectedTextFont(m_pContactElement->GetSelectedTextFont());
	pContactNode->SetSelectedTextColor(m_pContactElement->GetSelectedTextColor());
	pContactNode->SetDropTargetTextFont(m_pContactElement->GetDropTargetTextFont());
	pContactNode->SetDropTargetTextColor(m_pContactElement->GetDropTargetTextColor());
	pContactNode->SetTextFontBlocked(m_pContactElement->GetBlockedTextFont());
	pContactNode->SetTextColorBlocked(m_pContactElement->GetBlockedTextColor());
	pContactNode->SetFindContactPanel(m_pFindContactPanel);

	int nHeight = GetContactHeight(CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->GetSizeImage());
	static_cast<CEntry*>(pContactNode.get())->SetHeight(nHeight);

	if (!m_pGroupAllContacts)
	{
		CDummyGroupNodePtr pDummyGroupNode = new CDummyGroupNode();
		pDummyGroupNode->SetFindContactPanel(m_pFindContactPanel);
		m_pGroupAllContacts = pDummyGroupNode;
	}

	pContactNode->SetAllContactsGroup(m_pGroupAllContacts->GetName());

	AddEntry(pEntryGroup, pContactNode);
	
	// Add to all contacts group...
	CEntryPtr pEntry = GetEntryByName(strEntryName, m_pGroupAllContacts->GetName());

	if (!pEntry)
	{
		AddEntry(m_pGroupAllContacts, pContactNode);
	}
	
	// Now adds the account group...
	CEntryGroupPtr pEntryGroupAccount;
    CString strName;
    strName.Format(_T("%s:%s"), pContact->GetAccount()->GetProtocolId(), 
        pContact->GetAccount()->GetUserId());
	pEntryGroupAccount = GetGroup(strName);
	
	if (!pEntryGroupAccount)
	{
		pEntryGroupAccount = CreateGroupAccount(pContact->GetAccount());
	}
	
	CEntryPtr pAccountEntry = GetEntryByName(strEntryName, pEntryGroupAccount->GetName());
	
	if (!pAccountEntry)
	{
		AddEntry(pEntryGroupAccount, pContactNode);
	}
	
    // Add the dummy find group
	if (!m_pDummyGroupFindNode)
	{
		CDummyGroupFindNodePtr pDummyGroupFindNode = new CDummyGroupFindNode();
		pDummyGroupFindNode->SetFindContactPanel(m_pFindContactPanel);
		m_pDummyGroupFindNode = pDummyGroupFindNode;
	}
	
	CEntryPtr pDummyFindEntry = GetEntryByName(strEntryName, m_pDummyGroupFindNode->GetName());
	if (!pDummyFindEntry)
	{
		AddEntry(m_pDummyGroupFindNode, pContactNode);
	}
	
	// Add the find group
	if (!m_pGroupFindContacts)
	{
		m_pGroupFindContacts = CreateGroupFind(pContact->GetAccount());
	}
	
	CEntryPtr pFindEntry = GetEntryByName(strEntryName, m_pGroupFindContacts->GetName());
	if (!pFindEntry)
	{
		AddEntry(m_pGroupFindContacts, pContactNode);
	}
	

	NotifyObservers(&CUOLMessengerContactListCtrlObserver::OnAddContact, pContactNode);
}


void CContactListCtrl::InternalRemoveContact(IUOLMessengerContactPtr pContact, BOOL bRedraw)
{
	IUOLMessengerGroupPtr pGroup = pContact->GetGroup();
	if (pGroup != NULL)
	{
		CString strContactEntry = FormatContactNodeName(pContact.get());
		IUOLMessengerContactNodePtr pContactNode = ap_dynamic_cast<IUOLMessengerContactNodePtr>(GetEntryByName(strContactEntry, pGroup->GetName()));

		if (pContactNode)
		{
			NotifyObservers(&CUOLMessengerContactListCtrlObserver::OnRemoveContact, pContactNode);
		}
		
		// Let's remove the account group reference first, so the redraw can be happen 
		// after the default group reference remove...
		//
		CString strName;
        strName.Format(_T("%s:%s"), pContact->GetAccount()->GetProtocolId(), 
            pContact->GetAccount()->GetUserId());
        RemoveEntry(strContactEntry, strName);

		// Remove contact from Find Group
		RemoveEntry(strContactEntry, m_pGroupFindContacts->GetName());
		
		// Remove contact from group
		RemoveEntry(strContactEntry, pGroup->GetName(), bRedraw);

		IUOLMessengerContactPtr pFindContact = m_pContactList->FindContact(pContact->GetAccount(), pContact->GetName());
		if (!pFindContact)
		{
			// No more references to contact, remove from all contacts group
			ATLASSERT(m_pGroupAllContacts != NULL);
			RemoveEntry(strContactEntry, m_pGroupAllContacts->GetName(), bRedraw);
		}
	}
}


void CContactListCtrl::InternalAddGroup(IUOLMessengerGroupPtr pGroup)
{
	CGroupNodePtr pGroupNode = new CGroupNode(pGroup);
	
	pGroupNode->SetComparatorAdapter(m_pComparatorAdapter.get());
	pGroupNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_GROUP_DEFAULT_PAINTER));
	pGroupNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_GROUP_SELECTION_PAINTER));
	pGroupNode->SetCollapsedImage(m_pGroupElement->GetCollapsedImage());
	pGroupNode->SetExpandedImage(m_pGroupElement->GetExpandedImage());
	pGroupNode->SetTextFont(m_pGroupElement->GetFont());
	pGroupNode->SetTextColor(m_pGroupElement->GetTextColor());
	pGroupNode->SetDropTargetTextFont(m_pGroupElement->GetDropTargetFont());
	pGroupNode->SetDropTargetTextColor(m_pGroupElement->GetDropTargetTextColor());
	pGroupNode->SetFindContactPanel(m_pFindContactPanel);
	
	int nHeight = GetGroupHeight();
	static_cast<CEntryGroup*>(pGroupNode.get())->SetHeight(nHeight);
	
	AddGroup(pGroupNode);
}


CString CContactListCtrl::FormatContactNodeName(IUOLMessengerContact* pContact)
{
	return FormatContactNodeName(pContact->GetName(), pContact->GetAccount()->GetUserId(), pContact->GetAccount()->GetProtocolId());
}


CString CContactListCtrl::FormatContactNodeName(const CString& strContact, const CString& strAccountUserId, const CString& strAccountProtocolId)
{
	CString strName;
	strName.Format(_T("%s/%s/%s"), strContact, strAccountUserId, strAccountProtocolId);

	return strName;
}


int CContactListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CContactListCtrl::GetContactHeight(SizeImage sizeImage) const
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


DWORD CContactListCtrl::QueryDragEffect(DWORD /*grfKeyState*/, const CPoint& ptClient)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DWORD dwEffect = DROPEFFECT_NONE;

	if (!m_bGroupByAccounts)
	{
		CGroupListCtrlItemPtr pItem = Pt2Item(ptClient);
		CEntryGroupPtr pEntryGroup;

		if ((pItem.get()) &&
			(pItem->IsValidDropTarget()))
		{
			dwEffect = DROPEFFECT_MOVE;

			if (IsGroup(pItem))
			{
				pEntryGroup = ap_dynamic_cast<CEntryGroupPtr>(pItem);
			}
			else
			{
				CContactNodePtr pContactNode = ap_dynamic_cast<CContactNodePtr>(pItem);

				if (pContactNode.get())
				{
					IUOLMessengerContactPtr pContact = pContactNode->GetContact();

					if (pContact.get())
					{
						IUOLMessengerGroupPtr pGroup = pContact->GetGroup();

						if (pGroup.get())
						{
							pEntryGroup = GetGroup(pGroup->GetName());
						}
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
	}

	return dwEffect;
}


void CContactListCtrl::DoScrolling(ScrollCommand scCommand)
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

void CContactListCtrl::GetContacts(CAtlList<IUOLMessengerContactNodePtr>& listContacts)
{
	struct ContactsHelper : public CGroupListCtrl::Callback
	{
		CAtlList<IUOLMessengerContactNodePtr>* m_pListContacts;
		BOOL m_bCurrentGroupVisible;

		ContactsHelper(CAtlList<IUOLMessengerContactNodePtr>& listContacts) :
		m_pListContacts(&listContacts),
			m_bCurrentGroupVisible(FALSE){}

			BOOL operator() (CGroupListCtrlItemPtr pItem, BOOL bIsVisible)
			{
				if (pItem)
				{
					IUOLMessengerContactNodePtr pContact = ap_dynamic_cast<IUOLMessengerContactNodePtr>(pItem);

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


IUOLMessengerImageButton2Ptr CContactListCtrl::CreateContactButton()
{
	IUOLMessengerImageButton2Ptr pButton = new CImageButton();
	return pButton;
}


IUOLMessengerContextMenuItemPtr CContactListCtrl::CreateContextMenuItem(const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback)
{
	IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(15, (LPCTSTR) strLabel, pCallback);
	return pMenuItem;
}


void CContactListCtrl::RegisterObserver(CUOLMessengerContactListCtrlObserver* pObserver)
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


void CContactListCtrl::UnregisterObserver(CUOLMessengerContactListCtrlObserver* pObserver)
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


void CContactListCtrl::NotifyObservers(void (CUOLMessengerContactListCtrlObserver::*pfnCallback)(IUOLMessengerContactNodePtr), IUOLMessengerContactNodePtr pContact)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerContactListCtrlObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(pContact);
	}

}


void CContactListCtrl::NotifyObservers(void (CUOLMessengerContactListCtrlObserver2::*pfnCallback)(TCHAR), TCHAR vkey)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerContactListCtrlObserver* pObserver = m_listObservers.GetNext(pos);
        CUOLMessengerContactListCtrlObserver2* pObserver2 = dynamic_cast<CUOLMessengerContactListCtrlObserver2*>(pObserver);
        if (pObserver2)
        {
		    (pObserver2->*pfnCallback)(vkey);
        }
	}

}


void CContactListCtrl::GetAdditionalContextMenuItems(IUOLMessengerContactNodePtr pContactNode)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);
	
	if ( (m_pContactMenu) && (pContactNode) )
	{
		for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
		{
			CUOLMessengerContactListCtrlObserver* pObserver = m_listObservers.GetNext(pos);
			
			pObserver->OnShowContextMenu(
					ap_static_cast<IUOLMessengerContextMenuPtr>(m_pContactMenu->GetContextMenu()), 
					pContactNode);
		}
	}
}
		

CEntryGroupPtr CContactListCtrl::CreateGroupAccount(IUOLMessengerAccountPtr pAccount)
{
	if (FALSE == IsWindow())
	{
		return CEntryGroupPtr();
	}
	
	CGroupAccountNodePtr pGroupAccountNode = new CGroupAccountNode(pAccount);
	
	pGroupAccountNode->SetComparatorAdapter(m_pComparatorAdapter.get());
	pGroupAccountNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_GROUP_DEFAULT_PAINTER));
	pGroupAccountNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_GROUP_SELECTION_PAINTER));
	pGroupAccountNode->SetCollapsedImage(m_pGroupElement->GetCollapsedImage());
	pGroupAccountNode->SetExpandedImage(m_pGroupElement->GetExpandedImage());
	pGroupAccountNode->SetTextFont(m_pGroupElement->GetFont());
	pGroupAccountNode->SetTextColor(m_pGroupElement->GetTextColor());
	pGroupAccountNode->SetDropTargetTextFont(m_pGroupElement->GetDropTargetFont());
	pGroupAccountNode->SetDropTargetTextColor(m_pGroupElement->GetDropTargetTextColor());
	pGroupAccountNode->SetFindContactPanel(m_pFindContactPanel);
	
	int nHeight = GetGroupHeight();
	static_cast<CEntryGroup*>(pGroupAccountNode.get())->SetHeight(nHeight);
	
	AddGroup(pGroupAccountNode);
	
	return pGroupAccountNode;
}


CEntryGroupPtr CContactListCtrl::CreateGroupFind(IUOLMessengerAccountPtr pAccount)
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


void CContactListCtrl::OnKeyUp(TCHAR vkey, UINT repeats, UINT code)
{
	if (vkey == VK_ESCAPE)
	{
        ::SendMessage(GetParent(), CFindContactPanel::WM_FIND_PANEL_ESC_PRESSED, 0, 0);
	}

	SetMsgHandled(FALSE);
}


BOOL CContactListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// Get application window HWND
	CWindow appWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();

	if ((appWindow.IsChild(pMsg->hwnd)) && (IsWindowVisible()))
	{
		if (pMsg->message == WM_CHAR)
		{
			TCHAR ch = pMsg->wParam;
			char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*()[{}]/?\\|";

			if (strchr(validChars, ch))
			{

				SendMessage(WM_CHAR, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
		
		if (pMsg->message == WM_MOUSEWHEEL)
		{
			SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}

		if (pMsg->message == WM_KEYDOWN)
		{
			if ((pMsg->wParam == VK_DOWN) || (pMsg->wParam == VK_UP))
			{
				SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
				return TRUE; 
			}
		}

		if (pMsg->message == WM_KEYUP)
		{
			if ((pMsg->wParam == VK_HOME) || (pMsg->wParam == VK_END) ||
				(pMsg->wParam == VK_PRIOR) || (pMsg->wParam == VK_NEXT) ||
				(pMsg->wParam == VK_ESCAPE))
			{
				SendMessage(WM_KEYUP, pMsg->wParam, pMsg->lParam);
				return TRUE; 
			}
		}
	}

	return FALSE;
}


void CContactListCtrl::ProcessDropFiles(IDataObject* pDataObject)
{
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;
	if (pDataObject->GetData(&fmtetcFiles, &stgmed) == S_OK)
	{
		HDROP hdrop = (HDROP)GlobalLock(stgmed.hGlobal);

		if (hdrop != NULL)
		{
			CPoint ptClient;
			if (DragQueryPoint(hdrop, &ptClient))
			{
				IUOLMessengerContactPtr pContact = GetSelectedContact();
				if (pContact.get())
				{
					TCHAR szFileName[_MAX_PATH + 1];
					UINT nFiles = DragQueryFile(hdrop, (UINT)-1, NULL, 0);
					for(UINT nNames = 0; nNames < nFiles; nNames++)
					{
						ZeroMemory(szFileName, _MAX_PATH + 1);
						DragQueryFile(hdrop, nNames, (LPTSTR)szFileName, _MAX_PATH + 1);
						CString strFileName = szFileName;
						CUIMApplication::GetApplication()->ExecuteCommand(new CSendDroppedFileCommand(pContact, m_hWnd, strFileName)); 
					}
				}
			}
			GlobalUnlock(hdrop);
		}
		ReleaseStgMedium(&stgmed);

		m_pTransferDataObject = NULL;
	}
}


void CContactListCtrl::SetFindContactPanel(CFindContactPanelPtr pFindContactPanel)
{
	m_pFindContactPanel = pFindContactPanel;
}
