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
#include "FileTransferListCtrl.h"
#include "FileTransferNode.h"
#include "../../resource.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../../controller/HistoryRecordFileTransfer.h"

#include <interfaces/IUOLMessengerFileTransferManager.h>


using namespace GroupListCtrl;


CFileTransferListCtrl::CFileTransferListCtrl() : 
	m_pGroupElement(NULL),
	m_pFileTransferElement(NULL)
{
}


CFileTransferListCtrl::~CFileTransferListCtrl()
{
	if (m_pGroupElement)
	{
		m_pGroupElement.reset();
	}

	if (m_pFileTransferElement)
	{
		m_pFileTransferElement.reset();
	}
}


void CFileTransferListCtrl::Init(IUOLMessengerFileTransferListPtr pFileTransferList)
{
	if (pFileTransferList != NULL)
	{
		m_pFileTransferList = pFileTransferList;
		
		m_pFileTransferList->RegisterObserver(this);

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
		{
			IElementPtr pElement = m_listChilds.GetNext(pos);

			if (0 == pElement->GetName().CompareNoCase(_T("__FileTransferGroupElementInstance")))
			{
				m_pGroupElement = ap_dynamic_cast<CFileTransferGroupElementPtr>(pElement);
			}
			else if (0 == pElement->GetName().CompareNoCase(_T("__FileTransferEntryElementInstance")))
			{
				m_pFileTransferElement = ap_dynamic_cast<CFileTransferEntryElementPtr>(pElement);
			}
		}
		
		ATLASSERT(m_pGroupElement);
		ATLASSERT(m_pFileTransferElement);
	}
}


void CFileTransferListCtrl::Finalize()
{
	if (m_pFileTransferList != NULL)
	{
		m_pFileTransferList->UnregisterObserver(this);
	}
}


void CFileTransferListCtrl::AddFileTransfer(IUOLMessengerFileTransferPtr pFileTransfer)
{
	// Create FileTransfer Node
	CFileTransferNodePtr pFileTransferNode = new CFileTransferNode(
			pFileTransfer, m_pFileTransferElement->GetProgressBarElement());
	CString strEntryName = pFileTransferNode->GetFileTransferInternalName();

	static_cast<CEntry*>(pFileTransferNode.get())->SetName(strEntryName);
	pFileTransferNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_FILETRANSFER_DEFAULT_PAINTER));
	pFileTransferNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_FILETRANSFER_SELECTION_PAINTER));
	pFileTransferNode->SetTextFont(m_pFileTransferElement->GetTextFont());
	pFileTransferNode->SetTextColor(m_pFileTransferElement->GetTextColor());
	pFileTransferNode->SetSelectedTextFont(m_pFileTransferElement->GetSelectedTextFont());
	pFileTransferNode->SetSelectedTextColor(m_pFileTransferElement->GetSelectedTextColor());
	pFileTransferNode->SetTitleTextFont(m_pFileTransferElement->GetTitleTextFont());
	pFileTransferNode->SetTitleTextColor(m_pFileTransferElement->GetTitleTextColor());
	pFileTransferNode->SetItemSelectionBkgrndImage(
			m_pFileTransferElement->GetItemSelectionBkgrndImage());
	pFileTransferNode->SetActionsTextFont(m_pFileTransferElement->GetActionsTextFont());
	pFileTransferNode->SetActionsTextColor(m_pFileTransferElement->GetActionsTextColor());
	pFileTransferNode->SetItemSeparatorColor(m_pFileTransferElement->GetItemSeparatorColor());

	static_cast<CEntry*>(pFileTransferNode.get())->SetHeight(GetEntryHeight());

	AddEntry(m_pAllEntriesGroup, pFileTransferNode);
	m_pFileTransferList->AddFileTransfer(pFileTransferNode);	
}


void CFileTransferListCtrl::RemoveFileTransfer(IUOLMessengerFileTransferPtr pFileTransfer)
{
	CString strEntryName = FormatFileTransferNodeName(pFileTransfer);
	IUOLMessengerFileTransferNodePtr pFileTransferNode = m_pFileTransferList->FindFileTransfer(strEntryName);

	if (pFileTransferNode)
	{
		RemoveEntry(strEntryName, m_pAllEntriesGroup->GetName());
		m_pFileTransferList->RemoveFileTransfer(pFileTransferNode);
	}
}


CString CFileTransferListCtrl::GetName() const
{
	return "__FileTransferListInstance";
}


void CFileTransferListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}


void CFileTransferListCtrl::GetMinSize(CSize& size)
{
	CGroupListCtrl::GetSize(size);

	size.cy = GetEntryHeight();
}


void CFileTransferListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CFileTransferListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
}


LRESULT CFileTransferListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	LRESULT lRet = CGroupListCtrl::OnCreate(lpCreateStruct);

	CUIMApplication::GetApplication()->AddMessageFilter(this);

	CString strAllEntriesGroupName = _T("__FileTransferGroup__");
	
	m_pAllEntriesGroup = new CFileTransferGroup();
	static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->SetName(strAllEntriesGroupName);
	static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->SetHeight(0);
	
	ATLASSERT(m_pGroupElement != NULL);
	
	AddGroup(m_pAllEntriesGroup, FALSE);

	return lRet;
}


void CFileTransferListCtrl::OnDestroy()
{
	CUIMApplication::GetApplication()->RemoveMessageFilter(this);

	RemoveAll(FALSE);

	SetMsgHandled(FALSE);
}


IPainter* CFileTransferListCtrl::GetPainter(UINT nId)
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

	// Element painter
	if (m_pFileTransferElement != NULL)
	{
		return m_pFileTransferElement->GetPainter(nId);
	}

	return NULL;
}


int CFileTransferListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CFileTransferListCtrl::GetEntryHeight() const
{
	int nHeight = 0;

	if (m_pFileTransferElement != NULL)
	{
		nHeight = m_pFileTransferElement->GetHeight();
	}

	return nHeight;
}

void CFileTransferListCtrl::RemoveAllFinishedFileTransfers()
{
	CAtlList<IUOLMessengerFileTransferNodePtr> listEntries;
	m_pFileTransferList->GetEntries(listEntries);
	
	IUOLMessengerFileTransferPtr pFileTransfer;
	
	for (POSITION pos = listEntries.GetHeadPosition(); pos;)
	{
		IUOLMessengerFileTransferNodePtr pFileTransferNode = listEntries.GetNext(pos);

		if (pFileTransferNode)
		{
			pFileTransfer = pFileTransferNode->GetFileTransfer();
			if (pFileTransfer)
			{
				FileTransferStatus status = pFileTransfer->GetStatus();
				if ((status == FILE_TRANSFER_STATUS_DONE) || 
					(status == FILE_TRANSFER_STATUS_CANCEL_REMOTE) ||
					(status == FILE_TRANSFER_STATUS_CANCEL_LOCAL))
				{
					RemoveFileTransfer(pFileTransfer);
				}
			}
		}
	}
	
	IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
			ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
					CUIMApplication::GetApplication()->GetHistoryManager());
	
	if ( (pHistoryManager4) && (pFileTransfer) )
	{
		CHistoryRecordFileTransferPtr pHistoryRecordFileTransfer;
		pHistoryRecordFileTransfer = new CHistoryRecordFileTransfer(pFileTransfer);
		
		if (pHistoryRecordFileTransfer)
		{
			pHistoryManager4->DeleteHistory(pHistoryRecordFileTransfer);
		}
	}
	
	UpdateWindow();
	UpdateScrollSize();
}


void CFileTransferListCtrl::CancelAllFileTransfers()
{
	CAtlList<IUOLMessengerFileTransferNodePtr> listEntries;
	m_pFileTransferList->GetEntries(listEntries);

	for (POSITION pos = listEntries.GetHeadPosition(); pos;)
	{
		IUOLMessengerFileTransferNodePtr pFileTransferNode = listEntries.GetNext(pos);

		if (pFileTransferNode)
		{
			IUOLMessengerFileTransferPtr pFileTransfer = pFileTransferNode->GetFileTransfer();
			if (pFileTransfer)
			{
				CUIMApplication::GetApplication()->GetFileTransferManager()->CancelFileTransfer(pFileTransfer);
			}
		}
	}

	UpdateWindow();
}


CString CFileTransferListCtrl::FormatFileTransferNodeName(IUOLMessengerFileTransferPtr pFileTransfer) const
{
	CString strName;
	strName.Format("%s:%s:%d", pFileTransfer->GetContactName(), 
		pFileTransfer->GetFileName(), pFileTransfer->GetId());

	return strName.Trim();
}


BOOL CFileTransferListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// Get File Transfer window HWND
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
    if (pUIManager)
    {
		CWindow ftWindow = pUIManager->GetFileTransferWindow();

		if (((ftWindow.IsChild(pMsg->hwnd)) || (ftWindow == pMsg->hwnd)) && 
			  IsWindowVisible())
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
	}

	return FALSE;
}