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
#include "DownloadScheduler.h"

#include "IDiskCacheManager.h"
#include "../UIMApplication.h"


void CDownloadSchedulerObserverNotifier::NotifyAll(void (CDownloadSchedulerObserver::*pfnHandler)(IDownloadInfo*), IDownloadInfo* pDownloadInfo)
{
	CAtlList<CDownloadSchedulerObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	CDownloadSchedulerObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnHandler)(pDownloadInfo);
	}
}




CDownloadScheduler::CDownloadScheduler() : 
	m_nStop(FALSE)
{
}


CDownloadScheduler::~CDownloadScheduler()
{
}


BOOL CDownloadScheduler::Init()
{
	BOOL bRet = FALSE;

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	::InterlockedExchange(&m_nStop, FALSE);

	bRet = m_eventQueueNotEmpty.Create(NULL, TRUE, FALSE, NULL);

	if (bRet)
	{
		bRet = Start();
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"m_eventQueueNotEmpty.Create() failed.");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CDownloadScheduler::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	StopAll();
	
	Stop();
	MessageLoopWait();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IDownloadInfoPtr CDownloadScheduler::DownloadNow(const CString& strUrl, const CString& strLocalPath)
{
	ATLASSERT(FALSE == strUrl.IsEmpty());
	ATLASSERT(FALSE == strLocalPath.IsEmpty());

	CDownloadInfoPtr pDownloadInfo = new CDownloadInfo();

	pDownloadInfo->SetUrl(strUrl);
	pDownloadInfo->SetLocalPath(strLocalPath);

	ExecuteDownload(pDownloadInfo);

	m_observerNotifier.NotifyAll(&CDownloadSchedulerObserver::OnDownloadComplete, pDownloadInfo.get());

	return pDownloadInfo;
}


DWORD CDownloadScheduler::ScheduleDownload(const CString& strUrl, const CString& strLocalPath)
{
	ATLASSERT(FALSE == strUrl.IsEmpty());
	ATLASSERT(FALSE == strLocalPath.IsEmpty());

	if (strUrl.IsEmpty() || strLocalPath.IsEmpty())
	{
		return CHttpDownloader::DE_INVALID_PARAM;
	}

	CDownloadInfoPtr pDownloadInfo = new CDownloadInfo();

	pDownloadInfo->SetUrl(strUrl);
	pDownloadInfo->SetLocalPath(strLocalPath);

	EnqueueDownload(pDownloadInfo);

	return CHttpDownloader::DE_ERROR_SUCCESS;
}


void CDownloadScheduler::StopAll()
{
	::InterlockedExchange(&m_nStop, TRUE);

	CComCritSecLock<CComAutoCriticalSection> lock(m_csDownloadQueue);

	if (m_listDownloads.IsEmpty())
	{
		m_eventQueueNotEmpty.Set();
	}
}


void CDownloadScheduler::OnDownloadBegin()
{
}


void CDownloadScheduler::OnDownloadComplete(DWORD dwError)
{
}


void CDownloadScheduler::RegisterObserver(CDownloadSchedulerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CDownloadScheduler::UnregisterObserver(CDownloadSchedulerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CDownloadScheduler::Run()
{
	DWORD dwWaitReason;

	while (TRUE)
	{
		dwWaitReason = ::WaitForSingleObject(m_eventQueueNotEmpty, INFINITE);

		if (GetStopFlag())
		{
			break;
		}

		if (dwWaitReason == WAIT_OBJECT_0)
		{
			CDownloadInfoPtr pDownloadInfo = DequeueDownload();

			ExecuteDownload(pDownloadInfo);

			m_observerNotifier.NotifyAll(&CDownloadSchedulerObserver::OnDownloadComplete, pDownloadInfo.get());
		}
	}
}


BOOL CDownloadScheduler::GetStopFlag()
{
	return m_nStop;
}


void CDownloadScheduler::OnDownloadComplete(IDownloadInfo* pDownloadInfo)
{
	m_observerNotifier.NotifyAll(&CDownloadSchedulerObserver::OnDownloadComplete, pDownloadInfo);
}


CDownloadInfoPtr CDownloadScheduler::DequeueDownload()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csDownloadQueue);

	CDownloadInfoPtr pDownloadInfo = m_listDownloads.RemoveHead();

	if (m_listDownloads.IsEmpty())
	{
		m_eventQueueNotEmpty.Reset();
	}

	return pDownloadInfo;
}


void CDownloadScheduler::EnqueueDownload(CDownloadInfoPtr pDownload)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csDownloadQueue);

	m_listDownloads.AddTail(pDownload);

	if (m_listDownloads.GetCount() == 1)
	{
		m_eventQueueNotEmpty.Set();
	}
}


void CDownloadScheduler::ExecuteDownload(CDownloadInfoPtr pDownloadInfo)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CHttpDownloader httpDownloader;
	httpDownloader.SetUrl(pDownloadInfo->GetUrl());
	httpDownloader.SetLocalFilePath(pDownloadInfo->GetLocalPath());
	httpDownloader.SetCallback(this);
	httpDownloader.SetStopFlag(this);
	httpDownloader.DisableCache(FALSE);

	DWORD dwError = httpDownloader.DownloadFile();

	pDownloadInfo->SetStatus(dwError);
	
	if (CHttpDownloader::DE_ERROR_SUCCESS != dwError)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"CHttpDownloader failed: GetAdditionalErrorCode() = [%d], GetHTTPStatusErrorCode() = [%d], "
				"GetHTTPStatusErrorMessage() = [%s], pDownloadInfo->GetUrl() = [%s], "
				"pDownloadInfo->GetLocalPath() = [%s].", 
				httpDownloader.GetAdditionalErrorCode(), httpDownloader.GetHTTPStatusErrorCode(), 
				httpDownloader.GetHTTPStatusErrorMessage(), pDownloadInfo->GetUrl(), 
				pDownloadInfo->GetLocalPath());
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}