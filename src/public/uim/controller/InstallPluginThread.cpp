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
#include "InstallPluginThread.h"

#include "UIMApplication.h"
#include "UOLMessengerPluginManager.h"


CInstallPluginThread::CInstallPluginThread(CPluginInfoPtr pPluginInfo) : 
	m_pPluginInfo(pPluginInfo),
	m_pStopFlag(NULL),
	m_nStopped(FALSE),
	m_dwErrorCode(CUOLMessengerPluginManager::PIE_ERROR_FAILED),
	m_pCallback(NULL)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	ATLASSERT(m_pPluginInfo != NULL);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Thread for update plugin [%s] from [%s].", 
			m_pPluginInfo->GetInternalName(), m_pPluginInfo->GetDownloadUrl());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CInstallPluginThread::~CInstallPluginThread()
{
}


void CInstallPluginThread::SetCallback(CHttpDownloaderCallback* pCallback)
{
	m_pCallback = pCallback;
}


void CInstallPluginThread::SetStopFlag(CHttpDownloaderStopFlag* pStopFlag)
{
	m_pStopFlag = pStopFlag;
}


DWORD CInstallPluginThread::GetInstallationError()
{
	return m_dwErrorCode;
}


void CInstallPluginThread::CancelInstallation()
{
	::InterlockedExchange(&m_nStopped, TRUE);
}


void CInstallPluginThread::Run()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	::InterlockedExchange(&m_nStopped, FALSE);

	CUOLMessengerPluginManager* pPluginManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();

	if (pPluginManager)
	{
		// cast to force string clone
		CString strUrl = (LPCTSTR)m_pPluginInfo->GetDownloadUrl();
		CString strDirName = (LPCTSTR)m_pPluginInfo->GetDirName();
		CString strInternalName = (LPCTSTR)m_pPluginInfo->GetInternalName();
		CString strFileName = (LPCTSTR)m_pPluginInfo->GetFileName();

		if (m_pPluginInfo->IsInstallAvailable())
		{	
			m_dwErrorCode = pPluginManager->InstallPlugin(strInternalName, this, this);
		}	
		else if (m_pPluginInfo->IsUpdateAvailable())
		{	
			m_dwErrorCode = pPluginManager->UpdatePlugin(strInternalName, this, this);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogError(__FUNCTION__, 
			"Thread for update plugin [%s] from [%s] returned with [%d].", 
			m_pPluginInfo->GetName(), m_pPluginInfo->GetDownloadUrl(), m_dwErrorCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CInstallPluginThread::OnDownloadBegin()
{
	if (m_pCallback)
	{
		m_pCallback->OnDownloadBegin();
	}
}


void CInstallPluginThread::OnProgressChange(ULONG nProgress, ULONG nProgressMax)
{
	if (m_pCallback)
	{
		m_pCallback->OnProgressChange(nProgress, nProgressMax);
	}
}


void CInstallPluginThread::OnDownloadComplete(DWORD dwError)
{
	if (m_pCallback)
	{
		m_pCallback->OnDownloadComplete(dwError);
	}
}


BOOL CInstallPluginThread::GetStopFlag()
{
	BOOL bStopped = m_nStopped;

	if (m_pStopFlag)
	{
		bStopped |= m_pStopFlag->GetStopFlag();
	}

	return bStopped;
}