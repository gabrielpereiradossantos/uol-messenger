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
#include "MethodAuthorizationManager.h"

#include "UIMApplication.h"
#include "../Utility.h"
#include "UOLMessengerPluginManager.h"


#define			VERSION_ATTRIBUTE_TAG					"version"
#define			NAME_ATTRIBUTE_TAG						"name"
#define			AUTHORIZED_MODULE_TAG					"AuthorizedModule"
#define			AUTHORIZED_METHODS_TAG					"AuthorizedMethods"


CMethodAuthorizationManager::CMethodAuthorizationManager() : 
		m_bIsInitialized(FALSE)
{
	m_strAuthorizationFileURL.LoadString(IDS_METHOD_AUTHORIZING_FILE);
	
	m_strLocalAuthorizationFilePath = CUtility::GetTempFile();
}


CMethodAuthorizationManager::~CMethodAuthorizationManager()
{
	::DeleteFile(m_strLocalAuthorizationFilePath);
}


BOOL CMethodAuthorizationManager::Initialize()
{
	BOOL bInitializedOk = TRUE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (DownloadAuthorizationFile())
	{
		bInitializedOk = LoadAuthorizationFile();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bInitializedOk = %d", bInitializedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bInitializedOk;
}


void CMethodAuthorizationManager::Finalize()
{
	for (POSITION mapPos = m_moduleAuthorizationsMap.GetStartPosition(); mapPos; )
	{
		CModuleAuthorizationsMap::CPair* pPair = m_moduleAuthorizationsMap.GetNext(mapPos);
		
		if (pPair)
		{
			pPair->m_value->RemoveAll();
			delete pPair->m_value;
		}
	}
	
	m_moduleAuthorizationsMap.RemoveAll();
}


BOOL CMethodAuthorizationManager::IsAuthorized(
		void* pUOLMessengerPlugin, CString strMethodName)
{
	BOOL bIsAuthorized = FALSE;
	CModuleAuthorizationsMap::CPair* pPair = NULL;
	
	// Verify parameters.
	//
	if ( (pUOLMessengerPlugin != NULL) && (!strMethodName.IsEmpty()) )
	{
		CUOLMessengerPluginManager* pPluginManager = 
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();
		
		CUOLMessengerPlugin* pInvokingPlugin = (CUOLMessengerPlugin*)pUOLMessengerPlugin;
		CUOLMessengerPlugin* pLoadedPlugin = NULL;
		
		// Validate the plugin pointer provided matches with internal plugin loaded.
		//
		if ( (pPluginManager != NULL) && 
				((pLoadedPlugin = pPluginManager->GetPlugin(
						pInvokingPlugin->GetInternalName())) != NULL) && 
				(pLoadedPlugin == pInvokingPlugin) )
		{
			pPair = m_moduleAuthorizationsMap.Lookup(pInvokingPlugin->GetInternalName());
			
			// Validate the method authorization.
			//
			if ( (pPair) && (pPair->m_value->Find(strMethodName) != NULL) )
			{
				bIsAuthorized = TRUE;
			}
		}
	}
	
	return bIsAuthorized;
}


void CMethodAuthorizationManager::OnDownloadBegin()
{
}


void CMethodAuthorizationManager::OnDownloadComplete(DWORD dwError)
{
}


BOOL CMethodAuthorizationManager::DownloadAuthorizationFile()
{
	BOOL bDownloadedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CHttpDownloader httpDownloader;

	httpDownloader.SetCallback(this);
	httpDownloader.SetUrl(m_strAuthorizationFileURL);
	httpDownloader.SetLocalFilePath(m_strLocalAuthorizationFilePath);
	
	if (httpDownloader.DownloadFile() == CHttpDownloader::DE_ERROR_SUCCESS)
	{
		bDownloadedOk = TRUE;
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"CHttpDownloader failed: GetAdditionalErrorCode() = [%d], GetHTTPStatusErrorCode() = [%d], "
				"GetHTTPStatusErrorMessage() = [%s], strServerURL = [%s], strTempFilePath = [%s].", 
				httpDownloader.GetAdditionalErrorCode(), httpDownloader.GetHTTPStatusErrorCode(), 
				httpDownloader.GetHTTPStatusErrorMessage(), m_strAuthorizationFileURL, 
				m_strLocalAuthorizationFilePath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bDownloadedOk = %d", bDownloadedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bDownloadedOk;
}


BOOL CMethodAuthorizationManager::LoadAuthorizationFile()
{
	BOOL bLoadedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	try
	{
		MSXML::IXMLDOMDocumentPtr pDOMDocument;
		
		if (FAILED(pDOMDocument.CreateInstance(__uuidof(MSXML::DOMDocument))))
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pDOMDocument.CreateInstance() failed.");
			
			return FALSE;
		}
		
		CString strLocalFilePath;
		strLocalFilePath.Format("%s", m_strLocalAuthorizationFilePath);
		
		_bstr_t bstrXmlPath = (LPCTSTR)strLocalFilePath;
		
		VARIANT_BOOL bDocLoadedOk = pDOMDocument->load(bstrXmlPath);
		MSXML::IXMLDOMNodePtr pRootNode;
		
		if ( (bDocLoadedOk == VARIANT_TRUE) && 
				((pRootNode = pDOMDocument->documentElement) != NULL) && 
				(pRootNode->attributes != NULL) && 
				(pRootNode->attributes->length > 0) )
		{
			MSXML::IXMLDOMNodePtr pVersionNode = 
					pRootNode->attributes->getNamedItem(VERSION_ATTRIBUTE_TAG);
			
			if (pVersionNode)
			{
				_variant_t varDocVersion = pVersionNode->nodeValue;
				int iVersion = varDocVersion;
				
				// For now we have only version 1 loader.
				//
				if (iVersion == 1)
				{
					bLoadedOk = LoadAuthorizedModulesVersion1(pRootNode);
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogError(__FUNCTION__, 
							"File version [%d] is not supported.", iVersion);
				}
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
			GetLogger()->LogDebug(__FUNCTION__, "bLoadedOk = %d", bLoadedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bLoadedOk;
}


BOOL CMethodAuthorizationManager::LoadAuthorizedModulesVersion1(
		MSXML::IXMLDOMNodePtr pRootNode)
{
	BOOL bLoadedOk = FALSE;
	
	if (pRootNode != NULL)
	{
		MSXML::IXMLDOMNodeListPtr pAuthorizedModulesList = pRootNode->childNodes;
		long lLength = pAuthorizedModulesList->length;
		
		for (long listIndex = 0; listIndex < lLength; listIndex++)
		{
			MSXML::IXMLDOMNodePtr pAuthorizedModule = 
					pAuthorizedModulesList->item[listIndex];
			
			_bstr_t nodeName = pAuthorizedModule->nodeName;
			
			if ( (pAuthorizedModule != NULL) && 
					(pAuthorizedModule->attributes != NULL) && 
					(pAuthorizedModule->attributes->length > 0) )
			{
				MSXML::IXMLDOMNodePtr pNameNode = 
						pAuthorizedModule->attributes->getNamedItem(NAME_ATTRIBUTE_TAG);
				
				if (pNameNode != NULL)
				{
					_variant_t varAuthorizedModule = pNameNode->nodeValue;
					CString strAuthorizedModuleName(varAuthorizedModule);
					
					bLoadedOk = LoadAuthorizedMethodsVersion1(
							strAuthorizedModuleName, pAuthorizedModule);
					
					if (!bLoadedOk)
					{
						break;
					}
				}
				else
				{
					CString strNodeName = (LPCTSTR)nodeName;
					
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogWarning(__FUNCTION__, 
							"\"name\" attribute was not found on [%s] tag. "
							"Maybe a comment on file.", strNodeName);
				}
			}
			else
			{
				CString strNodeName = (LPCTSTR)nodeName;
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogWarning(__FUNCTION__, 
						"pAuthorizedModule = [%x], pAuthorizedModule->attributes = [%x], "
						"pAuthorizedModule->attributes->length = [%d], for [%s] tag.", 
						pAuthorizedModule, pAuthorizedModule->attributes, 
						(pAuthorizedModule->attributes != NULL) ? pAuthorizedModule->attributes->length : 0, 
						strNodeName);
			}
		}
	}
	
	return bLoadedOk;
}


BOOL CMethodAuthorizationManager::LoadAuthorizedMethodsVersion1(
		CString strAuthorizedModuleName, MSXML::IXMLDOMNodePtr pAuthorizedModule)
{
	BOOL bLoadedOk = FALSE;
	CAtlList<CString>* authorizedMethodsList = NULL;
	
	if ( (!strAuthorizedModuleName.IsEmpty()) && (pAuthorizedModule != NULL) )
	{
		if ( (pAuthorizedModule->childNodes != NULL) && 
				(pAuthorizedModule->childNodes->length == 1) )
		{
			MSXML::IXMLDOMNodePtr pAuthorizedMethods = 
					pAuthorizedModule->childNodes->item[0];
			
			_bstr_t nodeName = pAuthorizedModule->nodeName;
			
			if ( (pAuthorizedMethods != NULL) && 
					(pAuthorizedMethods->childNodes != NULL) && 
					(pAuthorizedMethods->childNodes->length > 0) )
			{
				MSXML::IXMLDOMNodeListPtr pAuthorizedMethodsList = pAuthorizedMethods->childNodes;
				long lLength = pAuthorizedMethodsList->length;
				
				for (long listIndex = 0; listIndex < lLength; listIndex++)
				{
					MSXML::IXMLDOMNodePtr pAuthorizedMethod = 
							pAuthorizedMethodsList->item[listIndex];
					
					CComBSTR bstrNodeText;
					HRESULT hr = pAuthorizedMethod->get_text(&bstrNodeText);
					
					if (SUCCEEDED(hr))
					{
						CString strAuthorizedMethod = bstrNodeText;
						
						if (authorizedMethodsList == NULL)
						{
							authorizedMethodsList = new CAtlList<CString>();
						}
						
						authorizedMethodsList->AddTail(strAuthorizedMethod);
					}
					else
					{
						static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
								GetLogger()->LogWarning(__FUNCTION__, 
								"pAuthorizedMethod->get_text() returned [%d], ", hr);
					}
				}
				
				if ( (authorizedMethodsList != NULL) && 
						(authorizedMethodsList->GetCount() > 0) )
				{
					bLoadedOk = TRUE;
					
					m_moduleAuthorizationsMap.SetAt(strAuthorizedModuleName, authorizedMethodsList);
				}
			}
			else
			{
				CString strNodeName = (LPCTSTR)nodeName;
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogWarning(__FUNCTION__, 
						"pAuthorizedMethods = [%x], pAuthorizedMethods->childNodes = [%x], "
						"pAuthorizedMethods->childNodes->length = [%d], for [%s] tag.", 
						pAuthorizedMethods, pAuthorizedMethods->childNodes, 
						(pAuthorizedMethods->childNodes != NULL) ? pAuthorizedMethods->childNodes->length : 0, 
						strNodeName);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pAuthorizedModule->childNodes = [%x], "
					"pAuthorizedModule->childNodes->length = [%d].", 
					pAuthorizedModule->childNodes, pAuthorizedModule->childNodes->length);
		}
	}
	
	return bLoadedOk;
}

