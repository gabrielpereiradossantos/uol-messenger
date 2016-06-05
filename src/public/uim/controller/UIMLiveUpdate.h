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
 


// UIMLiveUpdate.h: interface for the CUIMLiveUpdate class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIMLiveUpdateParser.h"
#include "DataGUID.h"

#include "irunnableJob.h"
#include "utils.h"
#include <InternetHandle.h>

#include "UIMLiveUpdateCallback.h"
#include "NotifyDlgThread.h"

#include "UOLMessengerPluginManager.h"

#include <commands/showpreferencescommand.h>


template <class Callback>
class CUIMLiveUpdate : 	public IRunnableJob
{
public:
	CUIMLiveUpdate(Callback callback, IShutdownFlag& shutdownFlag) : 
	  m_callback(callback), m_shutdownFlag(shutdownFlag)
	{
	}

	virtual ~CUIMLiveUpdate()
	{
		ATLTRACE("CUIMLiveUpdate::~CUIMLiveUpdate()\n");
	}

	virtual void LookForUpdates()
	{
	}

protected:
	void RunJob()
	{
		LiveUpdate();
	}
	
private:
	void LiveUpdate()
	{	
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogStartMethod(__FUNCTION__);
		
		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Kill received.");
			
			return;
		}

		// Get GUID		
		GUID guidInfo;
		CUIMApplication::GetApplication()->GetInstallCookie(guidInfo);
		CString strRegisterCode = CDataGUID::GetString(guidInfo);

		CString strUserAgent = GetAppUserAgent();

		CString strURL;
		CString strAux;

		UOLLib::ISettings* pResourceSettings = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

		if (pResourceSettings)
		{
			strURL = pResourceSettings->GetValue(_T("uol.resource.IDS_LIVEUPDATE_URL"));
		}

		strURL += "?";
		strURL += (strAux.Format(IDS_VERSION_PARAM, strRegisterCode), strAux);

		const TCHAR* pszAccept[] = {"text/html", NULL};
		
		CUrl url;
		
		if (FALSE == url.CrackUrl(strURL))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - FALSE == url.CrackUrl(strURL)\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "CrackUrl failed.");
			
			return;
		}		

		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Kill received.");
			
			return;
		}

		DWORD_PTR pContext = NULL;

		CInternetHandle hInet = ::InternetOpen(strUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

		if (!hInet)
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - !hInet\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "InternetOpen failed.");
			
			return;
		}

		// Set timeout
		ULONG ulTimeOut = 30000;
		DWORD dwTimeOutSize = sizeof(ulTimeOut);

		BOOL bRet = ::InternetSetOption(hInet, INTERNET_OPTION_RECEIVE_TIMEOUT, &ulTimeOut, dwTimeOutSize);
		ATLASSERT(bRet);

		CInternetHandle hConnect = ::InternetConnect(hInet, 
													 url.GetHostName(), 
													 url.GetPortNumber(), 
													 NULL, 
													 NULL, 
													 INTERNET_SERVICE_HTTP, 
													 0, 
													 pContext);
		
		if (!hConnect)
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - !hConnect\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "InternetConnect failed.");
			
			return;
		}

		CString strObject;
	
		strObject = url.GetUrlPath();
		strObject += url.GetExtraInfo();

		CInternetHandle hRequest = ::HttpOpenRequest(hConnect, 
													 "GET", 
													 strObject, 
													 NULL, 
													 NULL, 
													 pszAccept, 
													 INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_AUTO_REDIRECT, 
													 pContext);

		if (!hRequest)
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - !hRequest\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "HttpOpenRequest failed.");
			
			return;
		}

		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Kill received.");
			
			return;
		}

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Calling HttpSendRequest...");
		
		if (FALSE == ::HttpSendRequest(hRequest, NULL, 0, NULL, 0))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - HttpSendRequest\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "HttpSendRequest failed.");
			
			return;
		}

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "HttpSendRequest called...");
		
		TCHAR szStatusCode[20] = {0};
		DWORD dwStatusCodeSize = sizeof(szStatusCode) / sizeof(TCHAR);		

		if (FALSE == ::HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwStatusCodeSize, NULL))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - HttpQueryInfo\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "HttpQueryInfo \"StatusCode\" failed.");
			
			return;
		}

		DWORD dwStatusCode = (DWORD) _ttol(szStatusCode);

		if (dwStatusCode != 200)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Wrong HTTP Status returned.");
			
			return;
		}

		TCHAR szContentLength[20] = {0};
		DWORD dwContentLengthSize = sizeof(szContentLength) / sizeof(TCHAR);	

		if (FALSE == ::HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwContentLengthSize, NULL))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - HttpQueryInfo\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "HttpQueryInfo \"ContentLength\" failed.");
			
			return;
		}

		DWORD dwSize = _ttol(szContentLength);
		
		TCHAR szTempFile[MAX_PATH + 1] = {0};
		TCHAR szCurrentDir[MAX_PATH + 1] = {0};
		
		if (FALSE == ::GetTempPath(MAX_PATH, szCurrentDir))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - GetTempPath\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "GetTempPath() failed.");
			
			return;
		}
		
		if (FALSE == ::GetTempFileName(szCurrentDir, "UFC", 0, szTempFile))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - GetTempFileName\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "GetTempFileName() failed.");
			
			return;
		}

		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Kill received.");
			
			return;
		}

		CHandle hFile(::CreateFile(szTempFile, 
									 GENERIC_WRITE | GENERIC_READ, 
									 0, 
									 NULL, 
									 CREATE_ALWAYS, 
									 FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 
									 NULL));

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - INVALID_HANDLE_VALUE == hFile\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "CreateFile() failed.");
			
			return;
		}

		
		DWORD dwBytesRead = 0;

		while(dwSize > dwBytesRead)
		{
			// Check shutdown flag
			if (m_shutdownFlag.KillReceived())
			{
				ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, "Kill received.");
				
				return;
			}

			DWORD dwRead;			
			DWORD dwWrite;
			DWORD dwBytesInBuffer = 0;
		
			::InternetQueryDataAvailable(hRequest, &dwBytesInBuffer, 0, 0);

			std::auto_ptr<TCHAR> pBuffer(new TCHAR[dwBytesInBuffer + 1]);										
			::ZeroMemory((PVOID) pBuffer.get(), (SIZE_T) dwBytesInBuffer + 1);

			UINT nRead = ::InternetReadFile(hRequest, pBuffer.get(), dwBytesInBuffer, &dwRead);	
			UNUSED_ALWAYS(nRead);

			::WriteFile(hFile, pBuffer.get(), dwBytesInBuffer, &dwWrite, NULL);

			dwBytesRead += dwRead;
		}

		if (INVALID_SET_FILE_POINTER == ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - SetFilePointer\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "SetFilePointer() failed.");
			
			return;
		}

		DWORD dwFileSize = ::GetFileSize(hFile, NULL);

		if (dwFileSize != dwSize)
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - dwFileSize != dwSize\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "GetFileSize() failed.");
			
			return;
		}		
		
		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Kill received.");
			
			return;
		}

		std::auto_ptr<BYTE> pBuffer(new BYTE[dwFileSize + 1]);
		::ZeroMemory((PVOID) pBuffer.get(), (SIZE_T) dwFileSize + 1);

		DWORD dwNumberOfBytesRead;
		BOOL bOk = ::ReadFile(hFile, pBuffer.get(), dwFileSize, &dwNumberOfBytesRead, NULL);

		if ((FALSE == bOk) || (dwFileSize != dwNumberOfBytesRead))
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - (FALSE == bOk) || (dwFileSize != dwNumberOfBytesRead)\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "ReadFile() failed.");
			
			return;
		}

		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			ATLTRACE(_T("CUIMLiveUpdate::LiveUpdate() - shutdown flag\n"));
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Kill received.");
			
			return;
		}

		CString strFile(pBuffer.get());

		CUIMLiveUpdateParser	UIMLiveUpdateParser(strFile);

		//DWORD xmlVersion = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetUIMVersion();

		if (UIMLiveUpdateParser.GetVersion() > INSTALL_SEQUENCE)
		//if (UIMLiveUpdateParser.GetVersion() > xmlVersion) /* removed the warning about new version show be showed always */
		{
			//CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetUIMVersion(UIMLiveUpdateParser.GetVersion());
			m_callback.LiveUpdateAvailable();
		}
		else
		{
			m_callback.NoLiveUpdateAvailable();
		}			

		//::DeleteFile(szTempFile);																																				
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogEndMethod(__FUNCTION__);
	}

private:
	Callback		m_callback;
	IShutdownFlag&	m_shutdownFlag;
};


class CUIMPluginUpdate : 	public IRunnableJob
{
public:
	CUIMPluginUpdate(IShutdownFlag& shutdownFlag) : 
	  m_shutdownFlag(shutdownFlag)
	  {
	  }

	  virtual ~CUIMPluginUpdate()
	  {
		  ATLTRACE("CUIMLiveUpdate::~CUIMLiveUpdate()\n");
	  }

protected:
	void RunJob()
	{
		CUOLMessengerPluginManager* pPluginManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();

		if (pPluginManager->HasPluginUpdate()==TRUE)
		{

			CString strLiveUpdatePlugin;
			strLiveUpdatePlugin.LoadString(IDS_LIVEUPDATE_PLUGIN);

			CNotifyDlgThread* pNotifyThread2 = new CNotifyDlgThread(m_shutdownFlag);
			pNotifyThread2->SetMessage(strLiveUpdatePlugin);		
			pNotifyThread2->SetCommand(new CShowPreferencesCommand(CShowPreferencesCommand::SHOW_PLUGINS_TAB));
			pNotifyThread2->Init();	
		}
	}
private:
	IShutdownFlag&	m_shutdownFlag;
};
