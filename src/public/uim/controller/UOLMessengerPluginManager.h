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
 
#pragma once

#include <interfaces/iuolmessengerfacade.h>
#include <interfaces/uolmessengerplugin.h>

#include "../model/PluginInfo.h"
#include "HttpDownloader.h"


class CPluginInfoListObserver
{
public:
	virtual ~CPluginInfoListObserver() {}

	virtual void OnPluginInfoListChanged() = 0;
};


class CUOLMessengerPluginInfoTraits : public CElementTraits<CPluginInfoPtr>
{
public:
	static bool CompareElements(const CPluginInfoPtr element1, const CPluginInfoPtr element2)
	{		
		return (0 == element1->GetInternalName().CompareNoCase(element2->GetInternalName()));
	}
};


class CUOLMessengerPluginTraits : public CElementTraits<CUOLMessengerPlugin*>
{
public:
	static bool CompareElements(const CUOLMessengerPlugin* element1, const CUOLMessengerPlugin* element2)
	{		
		return 0 == element1->GetInternalName().CompareNoCase(element2->GetInternalName());
	}
};



class CUOLMessengerPluginManager : 
		public CWindowImpl<CUOLMessengerPluginManager>, 
		public CHttpDownloaderCallback
{
public:
	enum PluginInstallationError
	{
		PIE_ERROR_SUCCESS = 0,
		PIE_ERROR_FAILED = 1, // Generic error
		PIE_DOWNLOAD_FAILED,
		PIE_DLL_WRITE_ERROR,
		PIE_USER_CANCELED
	};

	DECLARE_WND_CLASS("UOLMessengerPluginManager");

public:
	CUOLMessengerPluginManager(void);
	virtual ~CUOLMessengerPluginManager(void);

	BOOL Init();
	void Finalize();

	void LoadPlugins();
	void UnloadPlugins();

	void GetPluginsList(CAtlList<CUOLMessengerPlugin*>& listplugins);
	CUOLMessengerPlugin* GetPlugin(const CString& strInternalName);

	DWORD InstallPlugin(const CString& strInternalName,
					   CHttpDownloaderCallback* pCallback,
					   CHttpDownloaderStopFlag* pStopFlag);
	DWORD UpdatePlugin(const CString& strInternalName,
					  CHttpDownloaderCallback* pCallback,
					  CHttpDownloaderStopFlag* pStopFlag);	
	
	BOOL InitPlugin(CUOLMessengerPlugin* pMessengerPlugin);
	void FinalizePlugin(CUOLMessengerPlugin* pMessengerPlugin);

	BOOL IsPluginEnabled(const CString& strInternalName) const;
	void EnablePlugin(const CString& strInternalName, BOOL bEnable);

	void SetVersionPlugin(const CString& strInternalName, const CString& strVersion);
	CString GetVersionPlugin(const CString& strInternalName) const;

	void GetPluginInfoList(CAtlList<CPluginInfoPtr>& listPluginInfo);

	void AddPluginInfoListObserver(CPluginInfoListObserver* pObserver);
	void RemovePluginInfoListObserver(CPluginInfoListObserver* pObserver);

	BOOL HasPluginUpdate();

	// CHttpDownloaderCallback interface.
	//
	virtual void OnDownloadBegin();
	virtual void OnDownloadComplete(DWORD dwError);
	
protected:
	enum
	{
		WM_USER_INIT_PLUGIN = WM_USER + 0x10,
		WM_USER_FINALIZE_PLUGIN
	};

	BEGIN_MSG_MAP(CUOLMessengerPluginManager)
		MESSAGE_HANDLER(WM_USER_INIT_PLUGIN, OnInitPlugin)
		MESSAGE_HANDLER(WM_USER_FINALIZE_PLUGIN, OnFinalizePlugin)
	END_MSG_MAP()

	LRESULT OnInitPlugin(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFinalizePlugin(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	BOOL InitPluginImpl(CUOLMessengerPlugin* pMessengerPlugin);
	void FinalizePluginImpl(CUOLMessengerPlugin* pMessengerPlugin);

	CUOLMessengerPlugin* RemovePlugin(const CString& strInternalName);

	CString GetPluginsDir() const;
	CUOLMessengerPlugin* LoadPlugin(const CString& strPluginPath);
	BOOL UnloadPlugin(CUOLMessengerPlugin* pMessengerPlugin);
	
	CString GetPluginListFromServer();

	void ProccessPlugingListXML(const CString& strXML, 								
								void (CUOLMessengerPluginManager::*pFnc)(MSXML::IXMLDOMNodePtr pItem));

	void ProccessInstalledPlugin(MSXML::IXMLDOMNodePtr pItem);
	void ProccessAvailablePlugin(MSXML::IXMLDOMNodePtr pItem);

	ULONGLONG VersionString2ULONGLONG(const CString& strVersion);
	BOOL CheckMinAppVersion(const CString& strVersion);

	CString GetTempFilePath();
	IUOLMessengerImagePtr GetPluginIcon(const CString& strUrl);

	CPluginInfoPtr GetPluginInfo(const CString& strInternalName) const;
	void RemovePluginInfo(const CString& strInternalName);

	DWORD InstallPluginImpl(const CString& strInternalName,
						   const CString& strDownloadUrl, 
						   const CString& strDirName,
						   const CString& strFileName,
						   CHttpDownloaderCallback* pCallback,
						   CHttpDownloaderStopFlag* pStopFlag);

	DWORD GetInstallationErrorCode(DWORD dwDownloadError) const;

	void NotifyAll(void (CPluginInfoListObserver::*pfnHandler)());
    
private:
	CAtlList<CPluginInfoPtr, CUOLMessengerPluginInfoTraits>		m_listPluginInfo;
	CAtlList<CUOLMessengerPlugin*, CUOLMessengerPluginTraits>	m_listPlugins;
	
	ULONGLONG					m_llAppVersion;	
	CString						m_strPluginBaseDir;
	IUOLMessengerFacade*		m_pUOLMessengerFacade;
	BOOL						m_bPluginUpdate;
	
	CAtlList<CPluginInfoListObserver*>	m_listPluginInfoListObserver;
	
	CComAutoCriticalSection				m_csPluginsList;
};

MAKEAUTOPTR(CUOLMessengerPluginManager);
