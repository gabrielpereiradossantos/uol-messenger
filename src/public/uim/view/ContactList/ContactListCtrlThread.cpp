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

#include "Stdafx.h"
#include "ContactListCtrlThread.h"
#include "ContactListCtrl.h"
#include "../../controller/UIMApplication.h"


#define			TERMINATE_ASYNC_OPERATIONS_SLEEP			500


CContactListCtrlThread::CContactListCtrlThread(HWND hContactListCtrlWnd) :
		m_hContactListCtrlWnd(hContactListCtrlWnd), 
		m_bStopFlag(FALSE),
		m_thread(this)
{
}


CContactListCtrlThread::~CContactListCtrlThread()
{
	if (m_terminateEvent)
	{
		m_terminateEvent.Close();
	}
}


BOOL CContactListCtrlThread::StartListCtrlThread()
{
	if (!m_thread.IsAlive())
	{
		if (m_terminateEvent.Create(NULL, TRUE, FALSE, NULL))
		{
			IUOLMessengerUIManager2Ptr pUIManager2 = 
					ap_dynamic_cast<IUOLMessengerUIManager2Ptr>(
						CUIMApplication::GetApplication()->GetUIManager());
			
			if (pUIManager2)
			{
				m_pContactListCtrl = pUIManager2->GetContactListCtrl();
			}
			
			m_bStopFlag = FALSE;
			return m_thread.Start();
		}
	}
	
	return FALSE;
}


BOOL CContactListCtrlThread::StopListCtrlThread()
{
	if (m_terminateEvent)
	{
		m_terminateEvent.Set();
	}
	
	if (m_thread.IsAlive())
	{
		m_bStopFlag = TRUE;
		m_thread.MessageLoopWait();
	}
	
	return TRUE;
}


void CContactListCtrlThread::AsyncAddContact(IUOLMessengerContactPtr pContact)
{
	ListContactOperationInfo* pOperationInfo = new ListContactOperationInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_ADD_CONTACT;
		pOperationInfo->pContact = pContact;
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


void CContactListCtrlThread::AsyncRemoveContact(IUOLMessengerContactPtr pContact)
{
	ListContactOperationInfo* pOperationInfo = new ListContactOperationInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_REMOVE_CONTACT;
		pOperationInfo->pContact = pContact;
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


void CContactListCtrlThread::AsyncRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts)
{
	ListRemoveContactsInfo* pOperationInfo = new ListRemoveContactsInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_REMOVE_CONTACTS;
		pOperationInfo->listContacts.AddHeadList(&listContacts);
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


void CContactListCtrlThread::AsyncMoveContact(IUOLMessengerContactPtr pContact, const CString& strOldGroup)
{
	ListMoveContactInfo* pOperationInfo = new ListMoveContactInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_MOVE_CONTACT;
		pOperationInfo->pContact = pContact;
		pOperationInfo->strOldGroup = strOldGroup;
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


void CContactListCtrlThread::AsyncAddGroup(IUOLMessengerGroupPtr pGroup)
{
	ListGroupOperationInfo* pOperationInfo = new ListGroupOperationInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_ADD_GROUP;
		pOperationInfo->pGroup = pGroup;
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


void CContactListCtrlThread::AsyncRemoveGroup(IUOLMessengerGroupPtr pGroup)
{
	ListGroupOperationInfo* pOperationInfo = new ListGroupOperationInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_REMOVE_GROUP;
		pOperationInfo->pGroup = pGroup;
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


void CContactListCtrlThread::AsyncRemoveAllGroups()
{
	ListRemoveAllGroupsInfo* pOperationInfo = new ListRemoveAllGroupsInfo;
	
	if (pOperationInfo != NULL)
	{
		pOperationInfo->uiOperationId = LIST_REMOVE_ALL_GROUPS;
		
		AddNewOperation((ListOperationInfo*)pOperationInfo);
	}
}


CContactListCtrlThread::ListOperationInfo* CContactListCtrlThread::GetNextOperation()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csOperationsList);
	
	if (m_operationsList.GetCount() > 0)
	{
		ListOperationInfo* pOperationInfo = m_operationsList.GetHead();
		m_operationsList.RemoveHead();
		
		return pOperationInfo;
	}
	
	return NULL;
}


void CContactListCtrlThread::AddNewOperation(ListOperationInfo* pOperationInfo)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csOperationsList);
	
	m_operationsList.AddTail(pOperationInfo);
}


void CContactListCtrlThread::ClearAllOperations()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csOperationsList);
	
	while (m_operationsList.GetCount() > 0)
	{
		ListOperationInfo* pOperationInfo = m_operationsList.GetHead();
		m_operationsList.RemoveHead();
		
		delete pOperationInfo;
	}
}


void CContactListCtrlThread::ExecuteOperation(ListOperationInfo* pOperationInfo)
{
	ListContactOperationInfo* pContactOperationInfo = NULL;
	ListRemoveContactsInfo* pRemoveContactsInfo = NULL;
	ListMoveContactInfo* pMoveContactInfo = NULL;
	ListGroupOperationInfo* pGroupOperationInfo = NULL;
	ListRemoveAllGroupsInfo* pRemoveAllGroupsInfo = NULL;
	
	if (pOperationInfo == NULL)
	{
		return;
	}
	
	if (m_pContactListCtrl == NULL)
	{
		return;
	}
	
	CContactListCtrl* pContactListCtrl = 
			dynamic_cast<CContactListCtrl*>(m_pContactListCtrl.get());
	
	if (pContactListCtrl == NULL)
	{
		return;
	}
	
	switch (pOperationInfo->uiOperationId)
	{
		case LIST_ADD_CONTACT:
			
			pContactOperationInfo = (ListContactOperationInfo*)pOperationInfo;
			
			if (pContactOperationInfo)
			{
				pContactListCtrl->AsyncAddContact(pContactOperationInfo->pContact);
			}
			
		break;
		
		case LIST_REMOVE_CONTACT:
			
			pContactOperationInfo = (ListContactOperationInfo*)pOperationInfo;
			
			if (pContactOperationInfo)
			{
				pContactListCtrl->AsyncRemoveContact(pContactOperationInfo->pContact);
			}
			
		break;
		
		case LIST_REMOVE_CONTACTS:
			
			pRemoveContactsInfo = (ListRemoveContactsInfo*)pOperationInfo;
			
			if (pRemoveContactsInfo)
			{
				pContactListCtrl->AsyncRemoveContacts(pRemoveContactsInfo->listContacts);
				pRemoveContactsInfo->listContacts.RemoveAll();
			}
			
		break;
		
		case LIST_MOVE_CONTACT:
			
			pMoveContactInfo = (ListMoveContactInfo*)pOperationInfo;
			
			if (pMoveContactInfo)
			{
				pContactListCtrl->AsyncMoveContact(
						pMoveContactInfo->pContact, pMoveContactInfo->strOldGroup);
			}
			
		break;
		
		case LIST_ADD_GROUP:
			
			pGroupOperationInfo = (ListGroupOperationInfo*)pOperationInfo;
			
			if (pGroupOperationInfo)
			{
				pContactListCtrl->AsyncAddGroup(pGroupOperationInfo->pGroup);
			}
			
		break;
		
		case LIST_REMOVE_GROUP:
			
			pGroupOperationInfo = (ListGroupOperationInfo*)pOperationInfo;
			
			if (pGroupOperationInfo)
			{
				pContactListCtrl->AsyncRemoveGroup(pGroupOperationInfo->pGroup);
			}
			
		break;
		
		case LIST_REMOVE_ALL_GROUPS:
			
			pRemoveAllGroupsInfo = (ListRemoveAllGroupsInfo*)pOperationInfo;
			
			if (pRemoveAllGroupsInfo)
			{
				pContactListCtrl->AsyncRemoveAllGroups();
			}
			
		break;
	}
	
	delete pOperationInfo;
}


void CContactListCtrlThread::Run()
{
	ListOperationInfo* pOperationInfo = NULL;
	
	while (m_bStopFlag == FALSE)
	{
		pOperationInfo = GetNextOperation();
		
		if (pOperationInfo != NULL)
		{
			ExecuteOperation(pOperationInfo);
		}
		else
		{
			::WaitForSingleObject(m_terminateEvent, TERMINATE_ASYNC_OPERATIONS_SLEEP);
		}
	}
	
	ClearAllOperations();
}


