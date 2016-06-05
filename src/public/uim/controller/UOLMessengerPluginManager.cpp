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
 
#include "StdAfx.h"
#include "./uolmessengerpluginmanager.h"

#include "uimapplication.h"
#include "../resource.h"

#include <MinDependencySpecialFolder.h>
#include <directoryenumerator.h>

#include "../FileUtils.h"
#include "HttpDownloader.h"
#include "../view/Image/ImageBuilder.h"

//static const CString PLUGIN_SUBDIR = "UOL\\UIM\\plugins\\";
//static const CString PLUGIN_LIB_SUBDIR = "UOL\\UIM\\lib\\";
//static const CString PLUGIN_WILDCARD = "*.dll";


CUOLMessengerPluginManager::CUOLMessengerPluginManager(void):
	m_bPluginUpdate(FALSE)
{	
	m_llAppVersion = CAppVersion().GetVersion();
	m_pUOLMessengerFacade = CUIMApplication::GetApplication();
}


CUOLMessengerPluginManager::~CUOLMessengerPluginManager(void)
{		
}


BOOL CUOLMessengerPluginManager::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	ATLASSERT(FALSE == IsWindow());

	HWND hWnd = Create(NULL, NULL, "UOLMessengerPluginManager", WS_POPUP);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hWnd = [%x]", hWnd);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return ::IsWindow(hWnd);
}


void CUOLMessengerPluginManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"IsWindow() = [%d]", IsWindow());
	
	ATLASSERT(IsWindow());
	if (IsWindow())
	{
		DestroyWindow();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUOLMessengerPluginManager::LoadPlugins()
{	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_strPluginBaseDir = GetPluginsDir();

	CString strRemoteXML = GetPluginListFromServer();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strRemoteXML = [%s].", strRemoteXML);
	
	ProccessPlugingListXML(m_strPluginBaseDir + "localuolmessengerplugins.xml", 
			&CUOLMessengerPluginManager::ProccessInstalledPlugin);
	ProccessPlugingListXML(strRemoteXML, &CUOLMessengerPluginManager::ProccessAvailablePlugin);

	NotifyAll(&CPluginInfoListObserver::OnPluginInfoListChanged);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUOLMessengerPluginManager::UnloadPlugins()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CUOLMessengerPlugin* pUOLMessengerPlugin = NULL;
	
	int iPluginsCount = 0;
	do
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
			
			pUOLMessengerPlugin = m_listPlugins.GetHead();
		}
		
		if (pUOLMessengerPlugin != NULL)
		{
			UnloadPlugin(pUOLMessengerPlugin);
			
			{
				CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
				
				m_listPlugins.RemoveHeadNoReturn();
				
				iPluginsCount = m_listPlugins.GetCount();
			}
		}
	}
	while ( (pUOLMessengerPlugin != NULL) && (iPluginsCount > 0) );
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
		
		m_listPlugins.RemoveAll();
	}
	
	NotifyAll(&CPluginInfoListObserver::OnPluginInfoListChanged);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUOLMessengerPluginManager::GetPluginsList(CAtlList<CUOLMessengerPlugin*>& listplugins)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
	
	listplugins.RemoveAll();
	
	for (POSITION pos = m_listPlugins.GetHeadPosition(); pos;)
	{
		listplugins.AddTail(m_listPlugins.GetNext(pos));
	}
}


CUOLMessengerPlugin* CUOLMessengerPluginManager::GetPlugin(const CString& strInternalName)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
	
	CUOLMessengerPlugin* pPlugin;
	
	for (POSITION pos = m_listPlugins.GetHeadPosition(); pos;)
	{
		pPlugin = m_listPlugins.GetNext(pos);
		if (0 == strInternalName.CompareNoCase(pPlugin->GetInternalName()))
		{
			return pPlugin;
		}
	}
	
	return NULL;
}


DWORD CUOLMessengerPluginManager::InstallPlugin(const CString& strInternalName,
											   CHttpDownloaderCallback* pCallback,
											   CHttpDownloaderStopFlag* pStopFlag)
{
	DWORD dwError = PIE_ERROR_FAILED;

	CPluginInfoPtr pPluginInfo = GetPluginInfo(strInternalName);

	if (!pPluginInfo)
	{
		return dwError;
	}

	CString strDownloadUrl = pPluginInfo->GetDownloadUrl();
	CString strDirName = pPluginInfo->GetDirName();
	CString strFileName = pPluginInfo->GetFileName();

	if ((strDownloadUrl.IsEmpty()) ||
		(strDirName.IsEmpty()) ||
		(strFileName.IsEmpty()))
	{
		return dwError;
	}

	RemovePluginInfo(strInternalName);

	RemovePlugin(strInternalName);

	dwError = InstallPluginImpl(strInternalName, strDownloadUrl, strDirName, strFileName, pCallback, pStopFlag);

	NotifyAll(&CPluginInfoListObserver::OnPluginInfoListChanged);

	return dwError;
}


DWORD CUOLMessengerPluginManager::UpdatePlugin(const CString& strInternalName,
											  CHttpDownloaderCallback* pCallback,
											  CHttpDownloaderStopFlag* pStopFlag)
{
	DWORD dwError = PIE_ERROR_FAILED;

	CPluginInfoPtr pPluginInfo = GetPluginInfo(strInternalName);

	if (!pPluginInfo)
	{
		return dwError;
	}

	CString strDownloadUrl = pPluginInfo->GetDownloadUrl();
	CString strDirName = pPluginInfo->GetDirName();
	CString strFileName = pPluginInfo->GetFileName();

	if ((strDownloadUrl.IsEmpty()) ||
		(strDirName.IsEmpty()) ||
		(strFileName.IsEmpty()))
	{
		return dwError;
	}

	RemovePluginInfo(strInternalName);

	BOOL bUnloadOK = TRUE;

	CUOLMessengerPlugin* pPlugin = RemovePlugin(strInternalName);

	if (pPlugin != NULL)
	{
		bUnloadOK = UnloadPlugin(pPlugin);
	}

	ATLASSERT(bUnloadOK);
	if (bUnloadOK)
	{
		dwError = InstallPluginImpl(strInternalName, strDownloadUrl, strDirName, strFileName, pCallback, pStopFlag);
	}

	NotifyAll(&CPluginInfoListObserver::OnPluginInfoListChanged);

	return dwError;
}


BOOL CUOLMessengerPluginManager::InitPlugin(CUOLMessengerPlugin* pMessengerPlugin)
{
	BOOL bInit = FALSE;

	if (IsWindow())
	{
		bInit = SendMessage(WM_USER_INIT_PLUGIN, (WPARAM) pMessengerPlugin, NULL);
	}

	return bInit;
}


void CUOLMessengerPluginManager::FinalizePlugin(CUOLMessengerPlugin* pMessengerPlugin)
{
	if (IsWindow())
	{
		SendMessage(WM_USER_FINALIZE_PLUGIN, (WPARAM) pMessengerPlugin, NULL);
	}
}


BOOL CUOLMessengerPluginManager::IsPluginEnabled(const CString& strInternalName) const
{
	CString strEnabled = "true";
	CString strKeyFormat = "uolmessenger.plugins.%s.enabled";

	CString strKey;
	strKey.Format(strKeyFormat, strInternalName);

	m_pUOLMessengerFacade->GetSettingsManager()->GetValue(strKey, strEnabled);

	return (strEnabled.CompareNoCase("true") == 0  || strEnabled == '1');
}

void CUOLMessengerPluginManager::EnablePlugin(const CString& strInternalName, BOOL bEnable)
{
	CString strEnabled;
	strEnabled.Format("%i", bEnable);

	CString strKeyFormat = "uolmessenger.plugins.%s.enabled";

	CString strKey;
	strKey.Format(strKeyFormat, strInternalName);

	m_pUOLMessengerFacade->GetSettingsManager()->SetValue(strKey, strEnabled);
}



void CUOLMessengerPluginManager::SetVersionPlugin(const CString& strInternalName, const CString& strVersion)
{
	CString strKeyFormat = "uolmessenger.plugins.%s.version";

	CString strKey;
	strKey.Format(strKeyFormat, strInternalName);

	m_pUOLMessengerFacade->GetSettingsManager()->SetValue(strKey, strVersion);
}

CString CUOLMessengerPluginManager::GetVersionPlugin(const CString& strInternalName) const
{
	CString strVersion = "";
	CString strKeyFormat = "uolmessenger.plugins.%s.version";

	CString strKey;
	strKey.Format(strKeyFormat, strInternalName);

	m_pUOLMessengerFacade->GetSettingsManager()->GetValue(strKey, strVersion);

	return strVersion;
}

void CUOLMessengerPluginManager::GetPluginInfoList(CAtlList<CPluginInfoPtr>& listPluginInfo)
{
	listPluginInfo.RemoveAll();

	for (POSITION pos = m_listPluginInfo.GetHeadPosition(); pos;)
	{
		listPluginInfo.AddTail(m_listPluginInfo.GetNext(pos));
	}
}


void CUOLMessengerPluginManager::AddPluginInfoListObserver(CPluginInfoListObserver* pObserver)
{
	POSITION posFind = m_listPluginInfoListObserver.Find(pObserver);

	if (!posFind)
	{
		m_listPluginInfoListObserver.AddTail(pObserver);
	}
}


void CUOLMessengerPluginManager::RemovePluginInfoListObserver(CPluginInfoListObserver* pObserver)
{
	POSITION posFind = m_listPluginInfoListObserver.Find(pObserver);

	if (posFind)
	{
		m_listPluginInfoListObserver.RemoveAt(posFind);
	}
}


LRESULT CUOLMessengerPluginManager::OnInitPlugin(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CUOLMessengerPlugin* pMessengerPlugin = (CUOLMessengerPlugin*) wParam;

	if (FALSE == ::IsBadReadPtr(pMessengerPlugin, sizeof(CUOLMessengerPlugin*)))
	{
		return InitPluginImpl(pMessengerPlugin);
	}

	return FALSE;
}


LRESULT CUOLMessengerPluginManager::OnFinalizePlugin(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CUOLMessengerPlugin* pMessengerPlugin = (CUOLMessengerPlugin*) wParam;

	if (FALSE == ::IsBadReadPtr(pMessengerPlugin, sizeof(CUOLMessengerPlugin*)))
	{
		FinalizePluginImpl(pMessengerPlugin);
	}

	return 0L;
}


BOOL CUOLMessengerPluginManager::InitPluginImpl(CUOLMessengerPlugin* pMessengerPlugin)
{
	if (pMessengerPlugin->Init(m_llAppVersion))
	{
		return TRUE;
	}

	// Plugin initialization failed
	CString strInternalName = (LPCTSTR) pMessengerPlugin->GetInternalName();

	CPluginInfoPtr pPluginInfo = GetPluginInfo(strInternalName);
	if (pPluginInfo)
	{
		pPluginInfo->SetPlugin(NULL);
	}

	RemovePlugin(strInternalName);

	UnloadPlugin(pMessengerPlugin);

	return FALSE;
}

void CUOLMessengerPluginManager::FinalizePluginImpl(CUOLMessengerPlugin* pMessengerPlugin)
{
	BOOL bFinalize = IsPluginEnabled(pMessengerPlugin->GetInternalName());

	if (bFinalize)
	{
		pMessengerPlugin->Finalize();		
	}
}


CUOLMessengerPlugin* CUOLMessengerPluginManager::RemovePlugin(const CString& strInternalName)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
	
	CUOLMessengerPlugin* pPlugin = NULL;
	
	for (POSITION pos = m_listPlugins.GetHeadPosition(); pos;)
	{
		POSITION posCurrent = pos;
		CUOLMessengerPlugin* pTestPlugin = m_listPlugins.GetNext(pos);
		
		if (0 == strInternalName.CompareNoCase(pTestPlugin->GetInternalName()))
		{
			pPlugin = pTestPlugin;
			m_listPlugins.RemoveAt(posCurrent);
			break;
		}
	}
	
	return pPlugin;
}


CString CUOLMessengerPluginManager::GetPluginsDir() const
{
	CString strDir;
	CString strKey = "uolmessenger.general.pluginsdir";

	if (FALSE == m_pUOLMessengerFacade->GetSettingsManager()->GetValue(strKey, strDir))
	{
		ATLASSERT(FALSE);
	}

	if (strDir.Right(1) != '\\')
	{
		strDir += '\\';
	}

	return strDir;
}


CUOLMessengerPlugin* CUOLMessengerPluginManager::LoadPlugin(const CString& strPluginPath)
{
	CUOLMessengerPlugin* pMessengerPlugin = NULL;
	CDllModule	dllModule;	

	if (dllModule.LoadLibrary(strPluginPath))
	{
		func_AllocPlugin pfAllocPlugin = reinterpret_cast<func_AllocPlugin>(dllModule.GetProcAddr("AllocUOLMessengerPlugin"));

		if (pfAllocPlugin)
		{
			pMessengerPlugin = pfAllocPlugin(m_pUOLMessengerFacade);
			ATLASSERT(pMessengerPlugin);

			if (pMessengerPlugin)
			{
				pMessengerPlugin->SetID(reinterpret_cast<LONG_PTR>(dllModule.Detach()));

				// Init plugin if enabled
				CString strInternalName = (LPCTSTR) pMessengerPlugin->GetInternalName();

				BOOL bInit = IsPluginEnabled(strInternalName);

				if (bInit)
				{
					if (FALSE == InitPlugin(pMessengerPlugin))
					{
						pMessengerPlugin = NULL;
						EnablePlugin(strInternalName, FALSE);
					}
				}
			}
		}
	}

	return pMessengerPlugin;
}


BOOL CUOLMessengerPluginManager::UnloadPlugin(CUOLMessengerPlugin* pMessengerPlugin)
{
	CString strInternalName = (LPCTSTR) pMessengerPlugin->GetInternalName();
	CPluginInfoPtr pPluginInfo = GetPluginInfo(strInternalName);
	if (pPluginInfo)
	{
		pPluginInfo->SetPlugin(NULL);
	}


	HMODULE hModule = (HMODULE)pMessengerPlugin->GetID();

	if (hModule)
	{
		CDllModule	dllModule;
		dllModule.Attach(hModule);

		func_FreePlugin pfFreePlugin = reinterpret_cast<func_FreePlugin>(dllModule.GetProcAddr("FreeUOLMessengerPlugin"));

		FinalizePlugin(pMessengerPlugin);

		if (pfFreePlugin)
		{
			pfFreePlugin(pMessengerPlugin);

			return TRUE;
		}
	}

	return FALSE;
}


CString CUOLMessengerPluginManager::GetPluginListFromServer()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strRet;

	CString strServerURL;
	strServerURL.LoadString(IDS_URL_PLUGINS_LIST);

	CString strTempFilePath = GetTempFilePath();

	CHttpDownloader httpDownloader;

	httpDownloader.SetCallback(this);
	httpDownloader.SetUrl(strServerURL);
	httpDownloader.SetLocalFilePath(strTempFilePath);
	
	if (CHttpDownloader::DE_ERROR_SUCCESS == httpDownloader.DownloadFile())
	{
		strRet = strTempFilePath;
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"CHttpDownloader failed: GetAdditionalErrorCode() = [%d], GetHTTPStatusErrorCode() = [%d], "
				"GetHTTPStatusErrorMessage() = [%s], strServerURL = [%s], strTempFilePath = [%s].", 
				httpDownloader.GetAdditionalErrorCode(), httpDownloader.GetHTTPStatusErrorCode(), 
				httpDownloader.GetHTTPStatusErrorMessage(), strServerURL, strTempFilePath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return strRet;
}


void CUOLMessengerPluginManager::ProccessPlugingListXML(const CString& strXML, 														
														void (CUOLMessengerPluginManager::*pFnc)(MSXML::IXMLDOMNodePtr pItem))
{	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "strXML = [%s].", strXML);
	
	if (strXML.IsEmpty())
	{
		return;
	}

	try
	{
		MSXML::IXMLDOMDocumentPtr pDOMDocument;

		if (FAILED(pDOMDocument.CreateInstance(__uuidof(MSXML::DOMDocument))))
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pDOMDocument.CreateInstance() failed.");
			
			return;
		}

		_bstr_t bstrXmlPath = (LPCTSTR)strXML;

		VARIANT_BOOL b = pDOMDocument->load(bstrXmlPath);

		if (b == VARIANT_TRUE)
		{
			MSXML::IXMLDOMNodeListPtr pDOMNodeList = pDOMDocument->getElementsByTagName("plugin");
			long lLength = pDOMNodeList->length;

			for (long lInd = 0; lInd < lLength; lInd++)
			{
				MSXML::IXMLDOMNodePtr pItem = pDOMNodeList->item[lInd];
				
				(this->*pFnc)(pItem);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pDOMDocument->load() failed.");
		}
	}
	catch(const _com_error& comError)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Error catched with code [%d].", comError.Error());
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUOLMessengerPluginManager::ProccessInstalledPlugin(MSXML::IXMLDOMNodePtr pItem)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	MSXML::IXMLDOMNamedNodeMapPtr pAttrib = pItem->attributes;

	MSXML::IXMLDOMNodePtr pInternalNameAttrib = pAttrib->getNamedItem("internalname");			
	_variant_t varInternalname =  pInternalNameAttrib->nodeValue;
	
	CString strInternalName(varInternalname);

	MSXML::IXMLDOMNodePtr pPluginFileNameAttrib = pAttrib->getNamedItem("pluginFileName");			
	_variant_t varFileName =  pPluginFileNameAttrib->nodeValue;

	MSXML::IXMLDOMNodePtr pPluginDirNameAttrib = pAttrib->getNamedItem("pluginDirname");			
	_variant_t varDirName =  pPluginDirNameAttrib->nodeValue;

	CString strPluginRelativePath = CString(varDirName) + CString('\\') + CString(varFileName);

	if (NULL == GetPlugin(varInternalname))
	{
		CUOLMessengerPlugin* pMessengerPlugin = LoadPlugin(m_strPluginBaseDir + strPluginRelativePath);

		if (pMessengerPlugin)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"Adding plugin [%s].", strInternalName);
			
			{
				CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
				
				m_listPlugins.AddTail(pMessengerPlugin);
			}

			CPluginInfoPtr pNewPluginInfo = new CPluginInfo(pMessengerPlugin->GetInternalName());

			pNewPluginInfo->SetPlugin(pMessengerPlugin);
			pNewPluginInfo->SetName(pMessengerPlugin->GetName());
			pNewPluginInfo->SetDesc(pMessengerPlugin->GetDesc());
			pNewPluginInfo->SetVersion(pMessengerPlugin->GetVersion());
			//pNewPluginInfo->SetIcon(pMessengerPlugin->GetIcon());

			m_listPluginInfo.AddTail(pNewPluginInfo);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Error loading plugin [%s].", strInternalName);
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Plugin [%s] already loaded.", strInternalName);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUOLMessengerPluginManager::ProccessAvailablePlugin(MSXML::IXMLDOMNodePtr pItem)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static BOOL notifiedUpdate = FALSE;

	MSXML::IXMLDOMNamedNodeMapPtr pAttrib = pItem->attributes;

	MSXML::IXMLDOMNodePtr pInternalNameAttrib = pAttrib->getNamedItem("internalname");
	_variant_t varInternalname =  pInternalNameAttrib->nodeValue;
	
	CString strInternalName(varInternalname);

	MSXML::IXMLDOMNodePtr pNameAttrib = pAttrib->getNamedItem("name");
	_variant_t varName =  pNameAttrib->nodeValue;

	MSXML::IXMLDOMNodePtr pVersionAttrib = pAttrib->getNamedItem("version");
	_variant_t varVersion =  pVersionAttrib->nodeValue;

	MSXML::IXMLDOMNodePtr pDownloadUrlAttrib = pAttrib->getNamedItem("downloadurl");
	_variant_t varDownloadUrl =  pDownloadUrlAttrib->nodeValue;

	MSXML::IXMLDOMNodePtr pIconUrl = pAttrib->getNamedItem("iconurl");
	_variant_t varIconUrl =  pIconUrl->nodeValue;

	MSXML::IXMLDOMNodePtr pPluginDirNameAttrib = pAttrib->getNamedItem("pluginDirname");
	_variant_t varDirName =  pPluginDirNameAttrib->nodeValue;

	MSXML::IXMLDOMNodePtr pPluginFielNameAttrib = pAttrib->getNamedItem("pluginFileName");
	_variant_t varFileName =  pPluginFielNameAttrib->nodeValue;

	_variant_t varMinAppVersion;

	MSXML::IXMLDOMNodePtr pMinAppVersionAttrib = pAttrib->getNamedItem("minappversion");

	if (pMinAppVersionAttrib)
	{
		varMinAppVersion =  pMinAppVersionAttrib->nodeValue;
	}
	else
	{
		varMinAppVersion = "";
	}

	MSXML::IXMLDOMNodePtr pChild = NULL;
	_variant_t varDesc = "";

	// Read description...
	if (pItem->hasChildNodes())
	{
		pChild = pItem->GetfirstChild();

		HRESULT hr;
		while (pChild != NULL)
		{
			CComBSTR bstrName;
			hr = pChild->get_nodeName(&bstrName);

			if (SUCCEEDED(hr))
			{
				CString strName = bstrName;
				if (0 == strName.CompareNoCase("desc"))
				{
					MSXML::IXMLDOMNodePtr pDescChild = pChild->GetfirstChild();
					
					if ((NULL != pDescChild) && 
						((NODE_TEXT == pDescChild->GetnodeType()) || 
						 (NODE_CDATA_SECTION == pDescChild->GetnodeType())))
					{
						varDesc =  pDescChild->GetnodeValue();
						break;
					}
				}
			}

			pChild = pChild->GetnextSibling();
		}
	}

	CString strMinAppVersion = varMinAppVersion;

	if (FALSE == strMinAppVersion.IsEmpty())
	{
		if (FALSE == CheckMinAppVersion(strMinAppVersion))
		{
			// Do not add to list if plugin requires later version of application
			return;
		}
	}

	ProccessInstalledPlugin(pItem);

	CPluginInfoPtr pNewPluginInfo = new CPluginInfo(varInternalname);

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pNewPluginInfo = [%x].", pNewPluginInfo);
	
	pNewPluginInfo->SetName(varName);
	pNewPluginInfo->SetDesc(varDesc);
	pNewPluginInfo->SetVersion(VersionString2ULONGLONG(varVersion));
	pNewPluginInfo->SetDownloadUrl(varDownloadUrl);
	pNewPluginInfo->SetDirName(varDirName);
	pNewPluginInfo->SetFileName(varFileName);
	pNewPluginInfo->SetIconUrl(varIconUrl);

	if (POSITION pos = m_listPluginInfo.Find(pNewPluginInfo))
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Processing already installed plugin [%s].", strInternalName);
		
		// Check current version...
		CPluginInfoPtr pPluginInfo = m_listPluginInfo.GetAt(pos);

		ULONGLONG oldv = pPluginInfo->GetVersion();
		ULONGLONG newv = pNewPluginInfo->GetVersion();

		if (pNewPluginInfo->GetVersion() > pPluginInfo->GetVersion())
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"Downloaded plugin [%s] has bigger version. Old = [%u] and New = [%u].", 
					strInternalName, pPluginInfo->GetVersion(), pNewPluginInfo->GetVersion());
			
			pPluginInfo->SetDownloadUrl(pNewPluginInfo->GetDownloadUrl());
			pPluginInfo->SetDirName(pNewPluginInfo->GetDirName());
			pPluginInfo->SetDesc(pNewPluginInfo->GetDesc());
			pPluginInfo->SetName(pNewPluginInfo->GetName());
			pPluginInfo->SetFileName(pNewPluginInfo->GetFileName());
			
			pPluginInfo->SetUpdateInfo(CPluginInfo::updateAvailable);

			CString xmlVersion = GetVersionPlugin(pPluginInfo->GetInternalName());
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"Plugin [%s] xmlVersion = [%s].", strInternalName, xmlVersion);
			
			if (xmlVersion.IsEmpty())
			{
				SetVersionPlugin(pPluginInfo->GetInternalName(),varVersion);
				m_bPluginUpdate=TRUE;
				//{
				//	notifiedUpdate = TRUE;
				//}
			}
			else
			{
				ULONGLONG nXmlVersion = VersionString2ULONGLONG(xmlVersion);
				ULONGLONG nPluginVersion = pNewPluginInfo->GetVersion();
				if (nPluginVersion > nXmlVersion)
				{
					m_bPluginUpdate=TRUE;
					//if (notifiedUpdate==FALSE)
					//{
					//	notifiedUpdate = TRUE;
					//}
					SetVersionPlugin(pPluginInfo->GetInternalName(),varVersion);
				}
			}
			
			IUOLMessengerImagePtr pIcon = GetPluginIcon(pNewPluginInfo->GetIconUrl());
			if (pIcon)
			{
				pPluginInfo->SetIcon(pIcon);
			}
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Adding new plugin [%s].", strInternalName);
		
		// New plugin...
		IUOLMessengerImagePtr pIcon = GetPluginIcon(pNewPluginInfo->GetIconUrl());
		if (pIcon)
		{
			pNewPluginInfo->SetIcon(pIcon);
		}

		pNewPluginInfo->SetUpdateInfo(CPluginInfo::installAvailable);

		m_listPluginInfo.AddTail(pNewPluginInfo);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


ULONGLONG CUOLMessengerPluginManager::VersionString2ULONGLONG(const CString& strVersion)
{
	ULONGLONG ll = 0;

	UINT s1, s2, s3, s4;
	s1 = s2 = s3 = s4 = 0;

	int nDots = 0;
	int nInd = 0;

	while ((nInd = strVersion.Find('.', nInd)) != -1)
	{
		nDots++;
		nInd++;
	}

	if (::sscanf(strVersion, 
				 "%i.%i.%i.%i", 
				 &s1, &s2, &s3, &s4))
	{
		ATLASSERT(s1 <= USHRT_MAX);
		ATLASSERT(s2 <= USHRT_MAX);
		ATLASSERT(s3 <= USHRT_MAX);
		ATLASSERT(s4 <= USHRT_MAX);

		USHORT* pShort = reinterpret_cast<USHORT*>(&ll);

#ifdef _M_IX86
		pShort[0] = static_cast<USHORT>(s4);
		pShort[1] = static_cast<USHORT>(s3);
		pShort[2] = static_cast<USHORT>(s2);
		pShort[3] = static_cast<USHORT>(s1);
#else
		#error "implement this!"
#endif
	}


	ll >>= ((3 - nDots) * sizeof(USHORT) * 8);

	return ll;
}


BOOL CUOLMessengerPluginManager::CheckMinAppVersion(const CString& strVersion)
{
	BOOL bRet = TRUE;

	ULONGLONG llMinAppVersion = VersionString2ULONGLONG(strVersion);

	TCHAR szAppPath[MAX_PATH];
	CString strAppPath;	

	HINSTANCE hInst = ATL::_AtlBaseModule.GetModuleInstance();
	if (::GetModuleFileName(hInst, szAppPath, MAX_PATH))
	{
		strAppPath = szAppPath;

		CFileVersion fv(strAppPath);
		ULONGLONG llVersion = fv.GetVersion();
		
		USHORT* pVersion = reinterpret_cast<USHORT*>(&llVersion);
		USHORT* pMinAppVersion = reinterpret_cast<USHORT*>(&llMinAppVersion);

		int nInd = 3;

		while (nInd >= 0)
		{
			if (pVersion[nInd] == pMinAppVersion[nInd])
			{
				nInd--;
			}
			else
			{
				bRet = (pVersion[nInd] > pMinAppVersion[nInd]);
				break;
			}
		}
	}

	return bRet;
}



CString CUOLMessengerPluginManager::GetTempFilePath()
{
	CString strFileNamePath;

	TCHAR pszTempDir[MAX_PATH + 1] = {0};
	
	if (::GetTempPath(MAX_PATH, pszTempDir))
	{
		TCHAR pszTempFilePath[MAX_PATH + 1] = {0};

		if (::GetTempFileName(pszTempDir, _T("uim"), 0, pszTempFilePath))
		{
			strFileNamePath = pszTempFilePath;
		}
	}

	return strFileNamePath;
}


IUOLMessengerImagePtr CUOLMessengerPluginManager::GetPluginIcon(const CString& strUrl)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerImagePtr pImage;

	if (FALSE == strUrl.IsEmpty())
	{
		CString strTempFilePath = GetTempFilePath();

		CHttpDownloader httpDownloader;

		httpDownloader.SetCallback(this);
		httpDownloader.SetUrl(strUrl);
		httpDownloader.SetLocalFilePath(strTempFilePath);
		
		if (CHttpDownloader::DE_ERROR_SUCCESS == httpDownloader.DownloadFile())
		{
			pImage = UOLMessengerImage::CImageBuilder::LoadImage(strTempFilePath, FALSE);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pImage;
}


CPluginInfoPtr CUOLMessengerPluginManager::GetPluginInfo(const CString& strInternalName) const
{
	CPluginInfoPtr pPluginInfo;
	
	for (POSITION pos = m_listPluginInfo.GetHeadPosition(); pos;)
	{
		pPluginInfo = m_listPluginInfo.GetNext(pos);

		if (0 == strInternalName.CompareNoCase(pPluginInfo->GetInternalName()))
		{
			return pPluginInfo;
		}
	}

	return CPluginInfoPtr();
}


void CUOLMessengerPluginManager::RemovePluginInfo(const CString& strInternalName)
{
	for (POSITION pos = m_listPluginInfo.GetHeadPosition(); pos;)
	{
		POSITION posCurrent = pos;
		CPluginInfoPtr pPluginInfo = m_listPluginInfo.GetNext(pos);

		if (0 == strInternalName.CompareNoCase(pPluginInfo->GetInternalName()))
		{
			CPluginInfoPtr pPluginInfo = m_listPluginInfo.GetAt(posCurrent);

			pPluginInfo->SetPlugin(NULL);

			m_listPluginInfo.RemoveAt(posCurrent);
			break;
		}
	}
}


DWORD CUOLMessengerPluginManager::InstallPluginImpl(const CString& strInternalName,
												   const CString& strDownloadUrl, 
												   const CString& strDirName,
												   const CString& strFileName,
												   CHttpDownloaderCallback* pCallback,
												   CHttpDownloaderStopFlag* pStopFlag)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	DWORD dwError = PIE_ERROR_FAILED;

	CString strDirNameWithBackSlash = strDirName;
	
	if (strDirNameWithBackSlash.Right(1) != '\\')
	{
		strDirNameWithBackSlash += '\\';
	}

	CUOLMessengerPlugin* pPlugin = NULL;

	CString strPluginDir = m_strPluginBaseDir + strDirNameWithBackSlash;

	if (CFileUtils::CreateDirectory(strPluginDir))
	{
		CString strPluginPath = strPluginDir + strFileName;

		CHttpDownloader httpDownloader;

		httpDownloader.SetUrl(strDownloadUrl);
		httpDownloader.SetLocalFilePath(strPluginPath);
		httpDownloader.SetCallback(pCallback);
		httpDownloader.SetStopFlag(pStopFlag);
	
		DWORD dwDownloadError = httpDownloader.DownloadFile();

		if (CHttpDownloader::DE_ERROR_SUCCESS == dwDownloadError)
		{
			EnablePlugin(strInternalName, TRUE);

			pPlugin = LoadPlugin(strPluginPath);
		}
		else
		{
			dwError = GetInstallationErrorCode(dwDownloadError);
		}
	}
	else
	{
		if (pCallback)
		{
			pCallback->OnDownloadComplete(CHttpDownloader::DE_ERROR_FAILED);
		}
	}

	if (pPlugin)
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csPluginsList);
			
			m_listPlugins.AddTail(pPlugin);
		}
		
		CPluginInfoPtr pNewPluginInfo = new CPluginInfo(pPlugin->GetInternalName());

		pNewPluginInfo->SetPlugin(pPlugin);
		pNewPluginInfo->SetName(pPlugin->GetName());
		pNewPluginInfo->SetDesc(pPlugin->GetDesc());
		pNewPluginInfo->SetVersion(pPlugin->GetVersion());
		//pNewPluginInfo->SetIcon(pMessengerPlugin->GetIcon());

		m_listPluginInfo.AddTail(pNewPluginInfo);

		dwError = PIE_ERROR_SUCCESS;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return dwError;
}


DWORD CUOLMessengerPluginManager::GetInstallationErrorCode(DWORD dwDownloadError) const
{
	DWORD dwRetError = PIE_DOWNLOAD_FAILED;
	
	switch (dwDownloadError)
	{
		case CHttpDownloader::DE_STOPPED:
			
			dwRetError = PIE_USER_CANCELED;
			
		break;
		
		case CHttpDownloader::DE_FILE_CREATION_ERROR:
		case CHttpDownloader::DE_FILE_WRITING_ERROR:
			
			dwRetError = PIE_DLL_WRITE_ERROR;
			
		break;
	}
	
	// (dwDownloadError == CHttpDownloader::DE_URL_NOT_FOUND)
	// (dwDownloadError == CHttpDownloader::DE_ERROR_FAILED)
	// (dwDownloadError == CHttpDownloader::DE_INVALID_PARAM)
	
	return dwRetError;
}


void CUOLMessengerPluginManager::NotifyAll(void (CPluginInfoListObserver::*pfnHandler)())
{
	CPluginInfoListObserver* pObserver;
	for (POSITION pos = m_listPluginInfoListObserver.GetHeadPosition(); pos; )
	{
		pObserver = m_listPluginInfoListObserver.GetNext(pos);

		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CPluginInfoListObserver*)))
		{
			(pObserver->*pfnHandler)();
		}
	}
}


BOOL CUOLMessengerPluginManager::HasPluginUpdate()
{
	return m_bPluginUpdate;
}


void CUOLMessengerPluginManager::OnDownloadBegin()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUOLMessengerPluginManager::OnDownloadComplete(DWORD dwError)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

