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
#include "FileTransferDialogManager.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../../controller/HistoryRecordFileTransfer.h"
#include "../../model/FileTransfer.h"

#include <interfaces/IUOLMessengerHistoryRecord.h>


CFileTransferDialogManager::CFileTransferDialogManager()
{
	m_bIsHistoryUpdated = FALSE;
}


CFileTransferDialogManager::~CFileTransferDialogManager()
{
}


BOOL CFileTransferDialogManager::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	ATLASSERT(FALSE == IsWindow());

	HWND hWnd = Create(NULL, NULL, "FileTransferManager", WS_POPUP);

	// Create File Transfer Frame
	if (!m_pFileTransferFrame)
	{
		CFileTransferFrameBuilderPtr pFileTransferFrameBuilder;

		CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		if (pUIManager)
		{
			pFileTransferFrameBuilder = ap_dynamic_cast<CFileTransferFrameBuilderPtr>(pUIManager->GetWindowManager()->GetElement("__FileTransferFrameBuilderInstance"));
			ATLASSERT(pFileTransferFrameBuilder.get());

			m_pFileTransferFrame = ap_dynamic_cast<CFileTransferFramePtr>(pFileTransferFrameBuilder->CreateElement());
			ATLASSERT(m_pFileTransferFrame.get());

			if (m_pFileTransferFrame != NULL)
			{
				m_pFileTransferFrame->Create(::GetDesktopWindow(), CRect(CPoint(200, 200), CSize(400, 400)));
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, "m_pFileTransferFrame == NULL");
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pUIManager == NULL");
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hWnd = [%x]", hWnd);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return (hWnd != NULL);
}


void CFileTransferDialogManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pFileTransferFrame = [%x]", m_pFileTransferFrame.get());
	
	if ((m_pFileTransferFrame != NULL) && (m_pFileTransferFrame->IsWindow()))
	{
		m_pFileTransferFrame->DestroyWindow();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"IsWindow() = [%d]", IsWindow());
	
	if (IsWindow())
	{
		DestroyWindow();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CFileTransferDialogManager::ShowFileTransferWindow(IUOLMessengerFileTransferPtr pFileTransfer)
{
	if (m_pFileTransferFrame)
	{
		CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		if (pUIManager)
		{
			CFileTransferListCtrlPtr pFileTransferListCtrl = ap_dynamic_cast<CFileTransferListCtrlPtr>(pUIManager->GetWindowManager()->GetElement("__FileTransferListInstance"));

			if (pFileTransferListCtrl && pFileTransfer)
			{
				pFileTransferListCtrl->AddFileTransfer(pFileTransfer);
			}

			m_pFileTransferFrame->ShowWindow(SW_NORMAL);

			if (!m_pFileTransferFrame->HasFocus())
			{
				m_pFileTransferFrame->Flash();
			}

			pFileTransferListCtrl->SetFocus();
			pFileTransferListCtrl->ScrollTop();
			pFileTransferListCtrl->SelectItem(1);
		}
	}
}


void CFileTransferDialogManager::UpdateRecentHistory()
{
	if (!m_bIsHistoryUpdated)
	{
		m_bIsHistoryUpdated = TRUE;

		CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		ATLASSERT(pUIManager);

		CFileTransferListCtrlPtr pFileTransferListCtrl = ap_dynamic_cast<CFileTransferListCtrlPtr>(pUIManager->GetWindowManager()->GetElement("__FileTransferListInstance"));
		ATLASSERT(pFileTransferListCtrl);
		
		// Get file transfer history
		CHistoryRecordFileTransferPtr pHistoryRecordFileTransfer;
		pHistoryRecordFileTransfer = new CHistoryRecordFileTransfer();
		ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
			CUIMApplication::GetApplication()->GetHistoryManager())->GetEventRecord(pHistoryRecordFileTransfer);

		CAtlList<CFileTransferPtr> listFileTransferEvents;
		pHistoryRecordFileTransfer->GetFileTransferEvents(listFileTransferEvents);
		for (POSITION pos = listFileTransferEvents.GetHeadPosition(); pos;)
		{
			CFileTransferPtr pFileTransfer = listFileTransferEvents.GetNext(pos);
			if (pFileTransfer)
			{
				// Add to list
				if (pFileTransferListCtrl)
				{
					pFileTransferListCtrl->AddFileTransfer(pFileTransfer);
				}
			}
		}
	}
}


void CFileTransferDialogManager::FlashFileTransferWindow()
{
	if (m_pFileTransferFrame)
	{
		m_pFileTransferFrame->ShowWindow(SW_NORMAL);

		if (!m_pFileTransferFrame->HasFocus())
		{
			m_pFileTransferFrame->Flash();
		}
	}
}


HWND CFileTransferDialogManager::GetFileTransferWindow()
{
	return m_pFileTransferFrame->m_hWnd;
}