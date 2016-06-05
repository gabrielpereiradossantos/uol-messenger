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
#include "MessageTemplate.h"
#include "../controller/UIMApplication.h"

#include "SettingsTraits.h"
#include "../FileUtils.h"


/*
 * Template folder hierarchy.
 *
 *	Template Folder
 *		Contents [directory]					(Required)
 *			Info.plist							(Optional) (used by Adium)
 *			Resources [directory]				(Required)
 *				main.css						(Required)
 *				Footer.html						(Required by pattern, but could be empty or not exist.)
 *				Header.html						(Required by pattern, but could be empty or not exist.)
 *				Status.html						(Required)
 *				NextStatus.html					(Optional)
 *				Template.html					(Optional)
 *				Incoming [directory]			(Required)
 *					Content.html				(Required)
 *					NextContent.html			(Required)
 *					Context.html				(Optional)
 *					NextContext.html			(Optional)
 *					FileTransferRequest.html	(Optional) (used by Kopete)
 *					Action.html					(Optional) (used by Kopete)
 *					buddy_icon.png				(Optional)
 *				Outgoing [directory]			(Required)
 *					Content.html				(Required)
 *					NextContent.html			(Required)
 *					Context.html				(Optional)
 *					NextContext.html			(Optional)
 *					Action.html					(Optional) (used by Kopete)
 *					buddy_icon.png				(Optional)
 *				Variants [directory]			(Optional)
 *					slightlydifferentX.css
 *					slightlydifferentY.css
 *					slightlydifferentZ.css
 * 
 * When exists the file "Info.plist" provides two valuable information:
 * 
 * - CFBundleName: unique identifier and name of the package (message 
 *                 view template);
 * 
 * - DefaultVariant: refers to the name of the variant that must be 
 *                   considered as default (implies the existence 
 *                   of "Variants" folder).
 * 
 */


#define				CONTENTS_FOLDER					"Contents"
#define				RESOURCES_FOLDER				"Resources"
#define				INCOMING_FOLDER					"Incoming"
#define				OUTGOING_FOLDER					"Outgoing"
#define				VARIANTS_FOLDER					"Variants"

#define				MANIFEST_FILE					"Info.plist"
#define				MAIN_CSS_FILE					"main.css"
#define				FOOTER_FILE						"Footer.html"
#define				HEADER_FILE						"Header.html"
#define				STATUS_FILE						"Status.html"
#define				NEXT_STATUS_FILE				"NextStatus.html"
#define				TEMPLATE_FILE					"Template.html"
#define				CONTENT_FILE					"Content.html"
#define				NEXT_CONTENT_FILE				"NextContent.html"
#define				CONTEXT_FILE					"Context.html"
#define				NEXT_CONTEXT_FILE				"NextContext.html"
#define				FILE_TRANSFER_REQUEST_FILE		"FileTransferRequest.html"
#define				ACTION_FILE						"Action.html"

#define				MANIFEST_BUNDLE_NAME_TAG		"CFBundleName"
#define				MANIFEST_DEFAULT_VARIANT_TAG	"DefaultVariant"

#define				CSS_TAG_FORMAT					"<link href=\"%s\" rel=\"stylesheet\" type=\"text/css\"/>"


CMessageTemplate::CMessageTemplate(CString strTemplatePath) : 
		m_userSettingsHandler(""),
		m_strTemplatePath(strTemplatePath), 
		m_bLoaded(FALSE)
{
	CPath pathBreaker(strTemplatePath);
	pathBreaker.StripPath();
	
	// Set the name initially to the template's folder name.
	// If the manifest file "Info.plist" doesn't exists, the 
	//
	m_strTemplateName = pathBreaker.m_strPath;
	
	// Initialize settings processing map.
	//
	InitTagProcessMap();
	
	m_userSettingsHandler.SetRootTag("plist");
	m_userSettingsHandler.AddSetting("dict", this);
}


CMessageTemplate::~CMessageTemplate()
{
}


BOOL CMessageTemplate::LoadTemplate()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if ( (!m_strTemplatePath.IsEmpty()) && 
			(ATLPath::IsDirectory(m_strTemplatePath)) )
	{
		// Let's dig the hierarchy folder.
		//
		CString currentPath = m_strTemplatePath + "\\" + CONTENTS_FOLDER;
		
		if (ATLPath::IsDirectory(currentPath))
		{
			CString manifestFile = currentPath + "\\" + MANIFEST_FILE;
			BOOL bTemplateProcessOk = FALSE;
			
			if (ATLPath::FileExists(manifestFile))
			{
				bTemplateProcessOk = LoadManifest(manifestFile);
			}
			else
			{
				bTemplateProcessOk = TRUE;
			}
			
			if (bTemplateProcessOk)
			{
				currentPath += CString("\\") + CString(RESOURCES_FOLDER);
				
				bTemplateProcessOk = ProcessResourcesFiles(currentPath);
			}
			
			if (bTemplateProcessOk)
			{
				bTemplateProcessOk = ProcessIncomingFolderFiles(
						currentPath + "\\" + INCOMING_FOLDER);
			}
			
			if (bTemplateProcessOk)
			{
				bTemplateProcessOk = ProcessOutgoingFolderFiles(
						currentPath + "\\" + OUTGOING_FOLDER);
			}
			
			if (ATLPath::IsDirectory(currentPath + "\\" + VARIANTS_FOLDER))
			{
				bTemplateProcessOk = ProcessVariantsFolderFiles(
						currentPath + "\\" + VARIANTS_FOLDER);
			}
			
			if ( (bTemplateProcessOk) && (!m_strDefaultVariant.IsEmpty()) && 
					(!m_templateVariantFiles.Find(m_strDefaultVariant + ".css")) )
			{
				bTemplateProcessOk = FALSE;
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"Default variant [%s.css] not found among the %d variants list.", 
						m_strDefaultVariant, m_templateVariantFiles.GetCount());
			}
			
			m_bLoaded = bTemplateProcessOk;
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Template path problem: m_strTemplatePath = [%s]", 
				m_strTemplatePath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bLoaded = [%d]", m_bLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return m_bLoaded;
}


CString CMessageTemplate::GetTemplateName()
{
	return m_strTemplateName;
}


int CMessageTemplate::GetVariantsNumber()
{
	return m_templateVariantFiles.GetCount();
}


CString CMessageTemplate::GetDefaultVariantName()
{
	return m_strDefaultVariant;
}


void CMessageTemplate::GetVariantNames(CAtlList<CString>& variantNamesList)
{
	variantNamesList.RemoveAll();
	
	for(POSITION variantNamePos = m_templateVariantFiles.GetHeadPosition(); variantNamePos; )
	{
		variantNamesList.AddTail(m_templateVariantFiles.GetNext(variantNamePos));
	}
}


CString CMessageTemplate::GetTemplateElement(MessageTemplateElementId nElementId)
{
	CTemplateElementMap::CPair* pTemplateElementPair = 
			m_templateElementMap.Lookup(nElementId);
	
	if (pTemplateElementPair)
	{
		return pTemplateElementPair->m_value;
	}
	else if ( (nElementId == TEMPLATE_CSS_IMPORT) && 
			(!m_strDefaultVariant.IsEmpty()) )
	{
		return GetTemplateElement(m_strDefaultVariant, nElementId);
	}
	
	return CString("");
}


CString CMessageTemplate::GetTemplateElement(CString variantName, 
		MessageTemplateElementId nElementId)
{
	if (nElementId != TEMPLATE_CSS_IMPORT)
	{
		return GetTemplateElement(nElementId);
	}
	else if (m_templateVariantFiles.Find(variantName + ".css"))
	{
		//return BuildCSSTag(variantName, TRUE);
		return BuildImportFilePath(variantName, TRUE);
	}
	
	return CString("");
}


void CMessageTemplate::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"key", NEW_SETTINGS_CALLBACK(Key, this)},
		{"string", NEW_SETTINGS_CALLBACK(String, this)}
	};
	
	int nSize = sizeof(ih) / sizeof(ih[0]);
	
	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CMessageTemplate::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}


void CMessageTemplate::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


BOOL CMessageTemplate::LoadManifest(CString strManifestFile)
{
	BOOL bLoaded = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_userSettingsHandler.Init(strManifestFile, FALSE, FALSE);
	
	// Try loading manifest file.
	//
	if (ATLPath::FileExists(strManifestFile))
	{
		bLoaded = m_userSettingsHandler.LoadSettings(FALSE);
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogWarning(__FUNCTION__, 
				"Manifest file doesn't exist: strManifestFile = [%s]", 
				strManifestFile);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bLoaded = [%d]", bLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bLoaded;
}


BOOL CMessageTemplate::ProcessResourcesFiles(CString strResourcesPath)
{
	BOOL bProcessedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (ATLPath::IsDirectory(strResourcesPath))
	{
		CString mainCSSFile = strResourcesPath + "\\" + MAIN_CSS_FILE;
		CString footerFile = strResourcesPath + "\\" + FOOTER_FILE;
		CString headerFile = strResourcesPath + "\\" + HEADER_FILE;
		CString statusFile = strResourcesPath + "\\" + STATUS_FILE;
		CString nextStatusFile = strResourcesPath + "\\" + NEXT_STATUS_FILE;
		CString templateFile = strResourcesPath + "\\" + TEMPLATE_FILE;
		
		// Test mandatory files first, so we don't loose time.
		//
		if ( (ATLPath::FileExists(statusFile)) && 
				(ImportTemplateElement(TEMPLATE_STATUS, statusFile)) )
		{
			// Mandatory ok...
			// Now this gets FALSE only on error below.
			//
			if (ATLPath::FileExists(mainCSSFile))
			{
				//m_templateElementMap.SetAt(TEMPLATE_CSS_IMPORT, 
						//BuildCSSTag(MAIN_CSS_FILE, FALSE));
				
				m_templateElementMap.SetAt(TEMPLATE_CSS_IMPORT, 
						BuildImportFilePath(mainCSSFile, FALSE));
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Main CSS file not found: mainCSSFile = [%s]", 
						mainCSSFile);
			}
			
			if ( (ATLPath::FileExists(mainCSSFile)) || 
					(!m_strDefaultVariant.IsEmpty()) )
			{
				bProcessedOk = TRUE;
				
				if (ATLPath::FileExists(footerFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_FOOTER, footerFile);
				}
				
				if (ATLPath::FileExists(headerFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_HEADER, headerFile);
				}
				
				if (ATLPath::FileExists(nextStatusFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_NEXT_STATUS, nextStatusFile);
				}
				
				if (ATLPath::FileExists(templateFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_TEMPLATE, templateFile);
				}
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Status file loading failure: statusFile = [%s]", 
					statusFile);
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Resources path invalid: strResourcesPath = [%s]", 
				strResourcesPath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bProcessedOk = [%d]", bProcessedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bProcessedOk;
}


BOOL CMessageTemplate::ProcessIncomingFolderFiles(CString strIncomingPath)
{
	BOOL bProcessedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (ATLPath::IsDirectory(strIncomingPath))
	{
		CString contentFile = strIncomingPath + "\\" + CONTENT_FILE;
		CString nextContentFile = strIncomingPath + "\\" + NEXT_CONTENT_FILE;
		CString contextFile = strIncomingPath + "\\" + CONTEXT_FILE;
		CString nextContextFile = strIncomingPath + "\\" + NEXT_CONTEXT_FILE;
		CString fileTransferFile = strIncomingPath + "\\" + FILE_TRANSFER_REQUEST_FILE;
		CString actionFile = strIncomingPath + "\\" + ACTION_FILE;
		
		// Test mandatory files first, so we don't loose time.
		//
		if ( (ATLPath::FileExists(contentFile)) && 
				(ImportTemplateElement(TEMPLATE_INCOMING_CONTENT, contentFile)) )
		{
			if ( (ATLPath::FileExists(nextContentFile)) && 
					(ImportTemplateElement(TEMPLATE_INCOMING_NEXT_CONTENT, nextContentFile)) )
			{
				// Mandatory ok...
				// Now this gets FALSE only on error below.
				//
				bProcessedOk = TRUE;
				
				if (ATLPath::FileExists(contextFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_INCOMING_CONTEXT, contextFile);
				}
				
				if (ATLPath::FileExists(nextContextFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_INCOMING_NEXT_CONTEXT, nextContextFile);
				}
				
				if (ATLPath::FileExists(fileTransferFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_INCOMING_FILE_TRANSFER, fileTransferFile);
				}
				
				if (ATLPath::FileExists(actionFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_INCOMING_ACTION, actionFile);
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"Content file loading failure: contentFile = [%s]", 
						contentFile);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"NextContent file loading failure: nextContentFile = [%s]", 
					nextContentFile);
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Incomming path invalid: strIncomingPath = [%s]", 
				strIncomingPath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bProcessedOk = [%d]", bProcessedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bProcessedOk;
}


BOOL CMessageTemplate::ProcessOutgoingFolderFiles(CString strOutgoingPath)
{
	BOOL bProcessedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (ATLPath::IsDirectory(strOutgoingPath))
	{
		CString contentFile = strOutgoingPath + "\\" + CONTENT_FILE;
		CString nextContentFile = strOutgoingPath + "\\" + NEXT_CONTENT_FILE;
		CString contextFile = strOutgoingPath + "\\" + CONTEXT_FILE;
		CString nextContextFile = strOutgoingPath + "\\" + NEXT_CONTEXT_FILE;
		CString fileTransferFile = strOutgoingPath + "\\" + FILE_TRANSFER_REQUEST_FILE;
		CString actionFile = strOutgoingPath + "\\" + ACTION_FILE;
		
		// Test mandatory files first, so we don't loose time.
		//
		if ( (ATLPath::FileExists(contentFile)) && 
				(ImportTemplateElement(TEMPLATE_OUTGOING_CONTENT, contentFile)) )
		{
			if ( (ATLPath::FileExists(nextContentFile)) && 
					(ImportTemplateElement(TEMPLATE_OUTGOING_NEXT_CONTENT, nextContentFile)) )
			{
				// Mandatory ok...
				// Now this gets FALSE only on error below.
				//
				bProcessedOk = TRUE;
				
				if (ATLPath::FileExists(contextFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_OUTGOING_CONTEXT, contextFile);
				}
				
				if (ATLPath::FileExists(nextContextFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_OUTGOING_NEXT_CONTEXT, nextContextFile);
				}
				
				if (ATLPath::FileExists(actionFile))
				{
					bProcessedOk = ImportTemplateElement(
							TEMPLATE_OUTGOING_ACTION, actionFile);
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"Content file loading failure: contentFile = [%s]", 
						contentFile);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"NextContent file loading failure: nextContentFile = [%s]", 
					nextContentFile);
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Outgoing path invalid: strOutgoingPath = [%s]", 
				strOutgoingPath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bProcessedOk = [%d]", bProcessedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bProcessedOk;
}


BOOL CMessageTemplate::ProcessVariantsFolderFiles(CString strVariantsPath)
{
	BOOL bProcessedOk = FALSE;
	CAtlList<CString> variantFilesList;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Listing variants from path [%s].", strVariantsPath);
	
	CFileUtils::GetDirectoryFiles(strVariantsPath, "*.*", TRUE, variantFilesList);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Processing the %d variants found.", variantFilesList.GetCount());
	
	for(POSITION variantListPos = variantFilesList.GetHeadPosition(); variantListPos; )
	{
		CString strVariantFile = variantFilesList.GetNext(variantListPos);
		
		if ( (ATLPath::FileExists(strVariantFile)) && 
				(lstrcmpi(ATLPath::FindExtension(strVariantFile), ".css") == 0) )
		{
			CPath pathBreaker(strVariantFile);
			pathBreaker.StripPath();
			
			m_templateVariantFiles.AddTail(pathBreaker.m_strPath);
			
			// Here we are considering that the folder must have at least 
			// one variant template file.
			//
			bProcessedOk = TRUE;
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"Not a valid variant. strVariantFile = [%s]", strVariantFile);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bProcessedOk = [%d]", bProcessedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bProcessedOk;
}


CString CMessageTemplate::BuildCSSTag(CString strFileName, BOOL bIsVariant)
{
	CString cssFilePath;
	
	if (bIsVariant)
	{
		cssFilePath.Format("%s\\%s\\%s\\%s\\%s.css", m_strTemplatePath, 
				CONTENTS_FOLDER, RESOURCES_FOLDER, VARIANTS_FOLDER, strFileName);
	}
	else
	{
		cssFilePath.Format("%s\\%s\\%s\\%s", m_strTemplatePath, 
				CONTENTS_FOLDER, RESOURCES_FOLDER, strFileName);
	}
	
	CString strCSSTag;
	strCSSTag.Format(CSS_TAG_FORMAT, cssFilePath);
	
	return strCSSTag;
}


CString CMessageTemplate::BuildImportFilePath(CString strFile, BOOL bIsVariant)
{
	CString strFileFullPath;
	char pszHTMLFilePath[MAX_PATH] = {0};
	DWORD dwHTMLFilePathSize = MAX_PATH;
	
	// Generate full path if needed.
	//
	if (bIsVariant)
	{
		strFileFullPath.Format("%s\\%s\\%s\\%s\\%s.css", m_strTemplatePath, 
				CONTENTS_FOLDER, RESOURCES_FOLDER, VARIANTS_FOLDER, strFile);
	}
	else
	{
		strFileFullPath = strFile;
	}
	
	// Now converts to HTML sintax.
	//
	HRESULT hr = UrlCreateFromPath(
			strFileFullPath, pszHTMLFilePath, &dwHTMLFilePathSize, NULL);
	
	if (SUCCEEDED(hr))
	{
		strFileFullPath = pszHTMLFilePath;
	}
	
	return strFileFullPath;
}


BOOL CMessageTemplate::ImportTemplateElement(MessageTemplateElementId nElementId, CString strFileName)
{
	BOOL bImportOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Importing element [%d] from file [%s].", nElementId, strFileName);
	
	unsigned char* pFileData = NULL;
	long iFileDataLenght = 0;
	
	if ((bImportOk = CFileUtils::LoadFile(strFileName, pFileData, iFileDataLenght)) == TRUE)
	{
		CString strFileData(pFileData);
		free(pFileData);
		
		m_templateElementMap.SetAt(nElementId, strFileData);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bImportOk = [%d]", bImportOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bImportOk;
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageTemplate, Key)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strLastKeyRead);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageTemplate, String)
{
	if (!bSave)
	{
		CString stringValue;
		CSettingsTraits::GetStringValue(pNode, strKey, stringValue);
		
		if (m_pThis->m_strLastKeyRead.CompareNoCase(MANIFEST_BUNDLE_NAME_TAG) == 0)
		{
			m_pThis->m_strTemplateName = stringValue;
		}
		else if (m_pThis->m_strLastKeyRead.CompareNoCase(MANIFEST_DEFAULT_VARIANT_TAG) == 0)
		{
			m_pThis->m_strDefaultVariant = stringValue;
		}
	}
}

