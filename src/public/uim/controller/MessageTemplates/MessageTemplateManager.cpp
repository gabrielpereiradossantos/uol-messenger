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
#include "MessageTemplateManager.h"

#include "../../FileUtils.h"
#include "../UIMApplication.h"


CMessageTemplateManager::CMessageTemplateManager() : 
		m_bInitialized(FALSE)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (CUIMApplication::GetApplication()->GetUIManager())
	{
		IUOLMessengerUIManager3Ptr pUIManager3 = 
				ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(
						CUIMApplication::GetApplication()->GetUIManager());
		
		if (pUIManager3)
		{
			CString strThemePath = pUIManager3->
					GetCurrentThemeProperties()->GetDirPath();
			
			m_strTemplatesRootPath.Format("%s\\Skin\\MessageWindow\\Templates", 
					strThemePath);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pUIManager3 == NULL. Casting failed.");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "GetUIManager() failed.");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CMessageTemplateManager::~CMessageTemplateManager()
{
}


BOOL CMessageTemplateManager::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if ( (!m_strTemplatesRootPath.IsEmpty()) && 
			(ATLPath::IsDirectory(m_strTemplatesRootPath)) )
	{
		CAtlList<CString> templateFoldersList;
		CFileUtils::GetDirectoryFiles(m_strTemplatesRootPath, "*.*", TRUE, templateFoldersList);
		
		for (POSITION templateListPos = templateFoldersList.GetHeadPosition(); templateListPos; )
		{
			CString strTemplateFolder = templateFoldersList.GetNext(templateListPos);
			
			if (ATLPath::IsDirectory(strTemplateFolder))
			{
				BOOL bTemplateLoaded = FALSE;
				CMessageTemplatePtr pMessageTemplate = new CMessageTemplate(strTemplateFolder);
				
				if ((bTemplateLoaded = pMessageTemplate->LoadTemplate()) == TRUE)
				{
					if (!m_bInitialized)
					{
						// For now we will consider that the initialization is ok whether 
						// at least one template is loaded.
						//
						m_bInitialized = TRUE;
					}
					
					CString strTemplateName = ap_dynamic_cast<IUOLMessengerMessageTemplatePtr>(
							pMessageTemplate)->GetTemplateName();
					
					m_mapMessageTemplates.SetAt(strTemplateName, pMessageTemplate);
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogError(__FUNCTION__, 
							"Failure loading template from [%s]", strTemplateFolder);
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"strTemplateFolder = [%s] is not a template folder.", 
						strTemplateFolder);
			}
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Template root path problem: m_strTemplatesRootPath = [%s]", 
				m_strTemplatesRootPath);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bInitialized = [%d]", m_bInitialized);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return m_bInitialized;
}


void CMessageTemplateManager::Finalize()
{
}


CString CMessageTemplateManager::GetTemplatesRootPath()
{
	return m_strTemplatesRootPath;
}


CString CMessageTemplateManager::GetDefaultTemplateDocument()
{
	CString strDefaultTemplatePath = m_strTemplatesRootPath + "\\Template.html";
	CString strDefaultTemplate;
	
	unsigned char* pFileData = NULL;
	long iFileDataLenght = 0;
	
	if (CFileUtils::LoadFile(strDefaultTemplatePath, pFileData, iFileDataLenght) == TRUE)
	{
		strDefaultTemplate.Format("%s", pFileData);
		free(pFileData);
	}
	
	return strDefaultTemplate;
}


IUOLMessengerMessageTemplatePtr CMessageTemplateManager::GetTemplate(CString strTemplateName)
{
	CMessageTemplateMap::CPair* pTemplatePair = 
			m_mapMessageTemplates.Lookup(strTemplateName);
	
	if (pTemplatePair)
	{
		return pTemplatePair->m_value;
	}
	
	return IUOLMessengerMessageTemplatePtr();
}


IUOLMessengerMessageTemplatePtr CMessageTemplateManager::GetDefaultTemplate()
{
	if (m_mapMessageTemplates.GetCount())
	{
		//CMessageTemplateMap::CPair* pTemplatePair = 
				//m_mapMessageTemplates.Lookup("Candybars");			// Sem graca.
				//m_mapMessageTemplates.Lookup("Ethereal");				// Inhaca.
				//m_mapMessageTemplates.Lookup("Fiat");					// Bom.
				//m_mapMessageTemplates.Lookup("nanoformat");			// Seco.
				//m_mapMessageTemplates.Lookup("PurePlastics");			// Legal.
				//m_mapMessageTemplates.Lookup("Renkoo");				// Legal.
				//m_mapMessageTemplates.Lookup("Simply Smooth");		// Bom.
				//m_mapMessageTemplates.Lookup("Fixed minimal_mod");	// Sem graca.
				//m_mapMessageTemplates.Lookup("UOLMessenger");
		
		/*
		 * Templates do GMail:
		 *
		 * Bubble					(Failed)
		 * BubblePicture			(Failed)
		 * Classic					(Ok)
		 * ClassicPicture			(Failed)
		 * Orkut					(Ok)
		 * orkutPicture				(Failed)
		 * PingPongPicture			(Failed)
		 * Serene					(Failed)
		 * SerenePicture			(Failed)
		 * Swiss					(Failed)
		 * SwissPicture				(Failed)
		 *
		 */
		
		IUOLMessengerMessageSettings3Ptr pMessageSettings3 = 
				ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(
						CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
		
		if (pMessageSettings3)
		{
			CMessageTemplateMap::CPair* pTemplatePair = 
					m_mapMessageTemplates.Lookup(pMessageSettings3->GetDefaultMessageStyle());
			
			if (pTemplatePair)
			{
				return pTemplatePair->m_value;
			}
			else
			{
				// No default configured, get the first one.
				//
				return m_mapMessageTemplates.GetValueAt(
						m_mapMessageTemplates.GetStartPosition());
			}
		}
	}
	
	return IUOLMessengerMessageTemplatePtr();
}


void CMessageTemplateManager::GetTemplateNames(CAtlList<CString>& templateNamesList)
{
	templateNamesList.RemoveAll();
	
	for(POSITION templatePos = m_mapMessageTemplates.GetStartPosition(); templatePos; )
	{
		CMessageTemplateMap::CPair* pMessageTemplate = 
				m_mapMessageTemplates.GetNext(templatePos);
		
		templateNamesList.AddTail(pMessageTemplate->m_key);
	}
}


void CMessageTemplateManager::GetTemplates(
		CAtlList<IUOLMessengerMessageTemplatePtr>& templatesList)
{
	templatesList.RemoveAll();
	
	for(POSITION templatePos = m_mapMessageTemplates.GetStartPosition(); templatePos; )
	{
		CMessageTemplateMap::CPair* pMessageTemplate = 
				m_mapMessageTemplates.GetNext(templatePos);
		
		templatesList.AddTail(pMessageTemplate->m_value);
	}
}

