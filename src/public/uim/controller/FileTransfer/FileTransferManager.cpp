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
#include "FileTransferManager.h"

#include "../UIMApplication.h"
#include "../UIManager.h"
#include "../Conversation/ConversationManager.h"


CFileTransferManager::CFileTransferManager()
{
}


CFileTransferManager::~CFileTransferManager()
{
}


BOOL CFileTransferManager::Init()
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pFileTransferFilter = new CFileTransferFilter();

	if (m_pFileTransferFilter)
	{
		return m_pFileTransferFilter->Init();
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "m_pFileTransferFilter == NULL");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CFileTransferManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pFileTransferFilter = [%x]", m_pFileTransferFilter);
	
	if (m_pFileTransferFilter)
	{
		m_pFileTransferFilter->Finalize();
		m_pFileTransferFilter.reset();
	}

	m_observerNotifier.UnregisterAll();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IUOLMessengerFileTransferPtr CFileTransferManager::GetSmartFromThisPointer(
		IUOLMessengerFileTransfer* pFileTransfer)
{
	for (POSITION listPos = m_listFileTransfers.GetHeadPosition(); listPos; )
	{
		IUOLMessengerFileTransferPtr pSmrtFileTransfer = 
				m_listFileTransfers.GetNext(listPos);
		
		if (pSmrtFileTransfer.get() == pFileTransfer)
		{
			return pSmrtFileTransfer;
		}
	}
	
	return IUOLMessengerFileTransferPtr();
}


void CFileTransferManager::NotifyFileTransferInit(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	// Create new file transfer window...
	CFileTransferPtr pFileTransfer = InternalCreateFileTransfer(pAccount, strContact, pFileTransferInfo->GetId(), 
		pFileTransferInfo->IsSending() ? IUOLMessengerFileTransfer::FTO_SEND : IUOLMessengerFileTransfer::FTO_RECEIVE);

	if (pFileTransfer != NULL)
	{
		m_listFileTransfers.AddTail(pFileTransfer);

		InternalSetFileTransferAttributes(pFileTransfer, pFileTransferInfo->GetFileName().c_str(), pFileTransferInfo->GetFileLocalPath().c_str(), pFileTransferInfo->GetTotalBytes());

		CUIMApplication::GetApplication()->GetUIManager()->ShowFileTransferDialog(pFileTransfer);

		m_observerNotifier.NotifyAll(&CUOLMessengerFileTransferManagerObserver::OnFileTransferInit, pFileTransfer);
	}
}


void CFileTransferManager::NotifyFileTransferUpdateProgress(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	UpdateFileTransferProgress(pAccount, pFileTransferInfo->GetId(), pFileTransferInfo->GetTransferedBytes(), pFileTransferInfo->GetTransferStatus());
}


void CFileTransferManager::NotifyFileTransferCanceled(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CFileTransferInfo* pFileTransferInfo)
{
	CFileTransferPtr pFileTransfer = InternalGetFileTransfer(pAccount->GetUserId(), pAccount->GetProtocolId(), pFileTransferInfo->GetId());

	if (pFileTransfer != NULL)
	{
		InternalUpdateFileTransferProgress(pFileTransfer, pFileTransferInfo->GetTransferedBytes(), pFileTransferInfo->GetTransferStatus());

		if (!pFileTransferInfo->IsSending())
		{
			// Delete incomplete file
			if (::PathFileExists(pFileTransferInfo->GetFileLocalPath().c_str()))
			{
				BOOL bDeleteOk = ::DeleteFile(pFileTransferInfo->GetFileLocalPath().c_str());
				ATLASSERT(bDeleteOk);
			}
		}

		pFileTransfer->NotifyAll();

		m_observerNotifier.NotifyAll(&CUOLMessengerFileTransferManagerObserver::OnFileTransferUpdateProgress, pFileTransfer);
	}
	else
	{
		CFileTransferPtr pTempFileTransfer = InternalCreateFileTransfer(pAccount, strContact, pFileTransferInfo->GetId(), 
			pFileTransferInfo->IsSending() ? IUOLMessengerFileTransfer::FTO_SEND : IUOLMessengerFileTransfer::FTO_RECEIVE);

		InternalSetFileTransferAttributes(pTempFileTransfer, pFileTransferInfo->GetFileName().c_str(), pFileTransferInfo->GetFileLocalPath().c_str(), pFileTransferInfo->GetTotalBytes());

		pFileTransfer = pTempFileTransfer;
	}

	pFileTransfer->SetStatus(pFileTransferInfo->GetTransferStatus());

	m_observerNotifier.NotifyAll(&CUOLMessengerFileTransferManagerObserver::OnFileTransferCanceled, pFileTransfer);

	// Flash File Transfer Window
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	pUIManager->FlashFileTransferDialog();

	// Warn user
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	pConversationManager->NotifyFileTransferStatus(pFileTransfer);
}


void CFileTransferManager::NotifyFileTransferError(IUOLMessengerAccountPtr pAccount, const CString& strContact, int nError)
{
	// Warn user
	CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
	pConversationManager->NotifyFileTransferError(pAccount, strContact, nError);
}


BOOL CFileTransferManager::SendFile(IUOLMessengerAccountPtr pAccount, const CString& strContactName, const CString& strFilePath)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	CIMProtocol* pProtocol = pAccount->GetProtocol();

	if (pProtocol)
	{
		error = pProtocol->SendFile((LPCTSTR) pAccount->GetUserId(), (LPCTSTR) strContactName, (LPCTSTR) strFilePath);
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


IUOLMessengerFileTransferPtr CFileTransferManager::CreateFileTransfer(IUOLMessengerAccountPtr pAccount, const CString& strContactName, long lTransferId, DWORD dwTransferOperation)
{
	if ((pAccount != NULL) && (!strContactName.IsEmpty()))
	{
		CFileTransferPtr pFileTransfer = InternalCreateFileTransfer(pAccount, strContactName, lTransferId, dwTransferOperation);

		m_listFileTransfers.AddTail(pFileTransfer);

		return pFileTransfer;
	}

	return IUOLMessengerFileTransferPtr();
}


IUOLMessengerFileTransferPtr CFileTransferManager::GetFileTransfer(IUOLMessengerAccountPtr pAccount, long lTransferId)
{
	if (pAccount != NULL)
	{
		return InternalGetFileTransfer(pAccount->GetUserId(), pAccount->GetProtocolId(), lTransferId);
	}

	return IUOLMessengerFileTransferPtr();
}


IUOLMessengerFileTransferPtr CFileTransferManager::GetFileTransfer(const CString& strAccountUserId, const CString& strAccountProtocolId, long lTransferId)
{
	return InternalGetFileTransfer(strAccountUserId, strAccountProtocolId, lTransferId);
}


void CFileTransferManager::DestroyFileTransfer(IUOLMessengerFileTransferPtr pFileTransfer)
{
	for (POSITION pos = m_listFileTransfers.GetHeadPosition(); pos; m_listFileTransfers.GetNext(pos))
	{
		CFileTransferPtr pTempFileTransfer = m_listFileTransfers.GetAt(pos);

		if (pTempFileTransfer.get() == pFileTransfer.get())
		{
			m_listFileTransfers.RemoveAt(pos);
			return;
		}
	}
}


void CFileTransferManager::GetFileTransferList(CAtlList<IUOLMessengerFileTransferPtr>& listTransfers)
{
	listTransfers.RemoveAll();

	for (POSITION pos = m_listFileTransfers.GetHeadPosition(); pos; )
	{
		CFileTransferPtr pTempFileTransfer = m_listFileTransfers.GetNext(pos);

		listTransfers.AddTail(pTempFileTransfer);
	}
}


void CFileTransferManager::SetFileTransferFileAttributes(IUOLMessengerAccountPtr pAccount, long lTransferId, const CString& strFileName, const CString& strLocalFilePath, ULONG lFileTotalBytes)
{
	if (pAccount != NULL)
	{
		CFileTransferPtr pFileTransfer = InternalGetFileTransfer(pAccount->GetUserId(), pAccount->GetProtocolId(), lTransferId);

		if (pFileTransfer != NULL)
		{
			InternalSetFileTransferAttributes(pFileTransfer, strFileName, strLocalFilePath, lFileTotalBytes);
		}
	}
}


void CFileTransferManager::UpdateFileTransferProgress(IUOLMessengerAccountPtr pAccount, long lTransferId, ULONG lTransferedBytes, FileTransferStatus transferStatus)
{
	if (pAccount != NULL)
	{
		CFileTransferPtr pFileTransfer = InternalGetFileTransfer(pAccount->GetUserId(), pAccount->GetProtocolId(), lTransferId);

		if (pFileTransfer != NULL)
		{
			// Identify when file transfer was accepted and warn user
			DWORD dwOldStatus = pFileTransfer->GetStatus();
			DWORD dwOperation = pFileTransfer->GetTransferOperation();

			if ((dwOldStatus == FILE_TRANSFER_STATUS_NOT_STARTED) &&
				(transferStatus == FILE_TRANSFER_STATUS_STARTED) &&
				(dwOperation == IUOLMessengerFileTransfer::FTO_SEND))
			{
				InternalUpdateFileTransferProgress(pFileTransfer, lTransferedBytes, transferStatus);

				// Warn user
				CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
				pConversationManager->NotifyFileTransferStatus(pFileTransfer);			
			}

			InternalUpdateFileTransferProgress(pFileTransfer, lTransferedBytes, transferStatus);

			pFileTransfer->NotifyAll();

			m_observerNotifier.NotifyAll(&CUOLMessengerFileTransferManagerObserver::OnFileTransferUpdateProgress, pFileTransfer);

			DWORD dwStatus = pFileTransfer->GetStatus();
			if (dwOldStatus != dwStatus) 
			{
				if ((dwStatus == FILE_TRANSFER_STATUS_CANCEL_REMOTE) || 
					(dwStatus == FILE_TRANSFER_STATUS_CANCEL_LOCAL) ||
					(dwStatus == FILE_TRANSFER_STATUS_DONE))
				{
					// Flash File Transfer Window
					CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
					pUIManager->FlashFileTransferDialog();

					// Warn user
					CConversationManagerPtr pConversationManager = ap_dynamic_cast<CConversationManagerPtr>(CUIMApplication::GetApplication()->GetConversationManager());
					pConversationManager->NotifyFileTransferStatus(pFileTransfer);
				}
			}
		}
	}
}


void CFileTransferManager::CancelFileTransfer(IUOLMessengerFileTransferPtr pFileTransfer)
{
	if (pFileTransfer != NULL)
	{
		FileTransferStatus status = pFileTransfer->GetStatus();

		if ((status != FILE_TRANSFER_STATUS_DONE) &&
			(status != FILE_TRANSFER_STATUS_CANCEL_LOCAL) &&
			(status != FILE_TRANSFER_STATUS_CANCEL_REMOTE))
		{
			IUOLMessengerAccountPtr pAccount = pFileTransfer->GetAccount();
			
			CIMProtocol* pProtocol = pAccount->GetProtocol();

			if (pProtocol)
			{
				pProtocol->CancelFileTransfer((LPCTSTR) pAccount->GetUserId(), pFileTransfer->GetId());
			}

			ap_static_cast<CFileTransferPtr>(pFileTransfer)->SetStatus(FILE_TRANSFER_STATUS_CANCEL_LOCAL);
		}
	}
}


IUOLMessengerFileTransferFilterPtr CFileTransferManager::GetFileTransferFilter()
{
	return m_pFileTransferFilter;
}


void CFileTransferManager::RegisterObserver(CUOLMessengerFileTransferManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CFileTransferManager::UnregisterObserver(CUOLMessengerFileTransferManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


CFileTransferPtr CFileTransferManager::InternalCreateFileTransfer(IUOLMessengerAccountPtr pAccount, const CString& strContactName, long lTransferId, DWORD dwTransferOperation)
{
	CFileTransferPtr pFileTransfer = new CFileTransfer(pAccount, strContactName, lTransferId, CTime::GetCurrentTime());

	pFileTransfer->SetTransferOperation(dwTransferOperation);

	return pFileTransfer;
}


CFileTransferPtr CFileTransferManager::InternalGetFileTransfer(const CString& strAccountUserId, const CString& strAccountProtocolId, long lTransferId)
{
	for (POSITION pos = m_listFileTransfers.GetHeadPosition(); pos; )
	{
		CFileTransferPtr pFileTransfer = m_listFileTransfers.GetNext(pos);

		if ((0 == strAccountUserId.Compare(pFileTransfer->GetAccount()->GetUserId())) &&
			(0 == strAccountProtocolId.Compare(pFileTransfer->GetAccount()->GetProtocolId())) &&
			(lTransferId == pFileTransfer->GetId()))
		{
			return pFileTransfer;
		}
	}

	return CFileTransferPtr();
}


void CFileTransferManager::InternalSetFileTransferAttributes(CFileTransferPtr pFileTransfer, const CString& strFileName, const CString& strLocalFilePath, ULONG lFileTotalBytes)
{
	pFileTransfer->SetFileName(strFileName);
	pFileTransfer->SetLocalFilePath(strLocalFilePath);
	pFileTransfer->SetTotalBytes(lFileTotalBytes);
}


void CFileTransferManager::InternalUpdateFileTransferProgress(CFileTransferPtr pFileTransfer, ULONG lTransferedBytes, FileTransferStatus transferStatus)
{
	pFileTransfer->SetTransferedBytes(lTransferedBytes);
	pFileTransfer->SetStatus(transferStatus);
}