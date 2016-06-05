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


#include <interfaces/IUOLMessengerMessageTemplate.h>
#include "ISettings.h"
#include "SettingsHandler.h"


class CMessageTemplate : public IUOLMessengerMessageTemplate, 
						 public ISettings
{
public:
	
	CMessageTemplate(CString strTemplatePath);
	virtual ~CMessageTemplate();
	
	BOOL LoadTemplate();
	
protected:
	
	// IUOLMessengerMessageTemplate.
	//
	virtual CString GetTemplateName();
	
	virtual int GetVariantsNumber();
	virtual CString GetDefaultVariantName();
	virtual void GetVariantNames(CAtlList<CString>& variantNamesList);
	
	virtual CString GetTemplateElement(MessageTemplateElementId nElementId);
	virtual CString GetTemplateElement(CString variantName, MessageTemplateElementId nElementId);
	
	// ISettings.
	//
	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	
	BOOL LoadManifest(CString strManifestFile);
	BOOL ProcessResourcesFiles(CString strResourcesPath);
	BOOL ProcessIncomingFolderFiles(CString strIncomingPath);
	BOOL ProcessOutgoingFolderFiles(CString strOutgoingPath);
	BOOL ProcessVariantsFolderFiles(CString strVariantsPath);
	CString BuildCSSTag(CString strFileName, BOOL bIsVariant);
	CString BuildImportFilePath(CString strFile, BOOL bIsVariant);
	BOOL ImportTemplateElement(MessageTemplateElementId nElementId, CString strFileName);
	
protected:
	
	DECLARE_SETTINGS_CALLBACK(CMessageTemplate, Key);
	DECLARE_SETTINGS_CALLBACK(CMessageTemplate, String);
	
private:
	
	CString			m_strTemplatePath;
	CString			m_strTemplateName;
	BOOL			m_bLoaded;
	
	CString			m_strDefaultVariant;
	
	CAtlList<CString>		m_templateVariantFiles;
	
	typedef CAtlMap<int, CString> CTemplateElementMap;
	CTemplateElementMap		m_templateElementMap;
	
	// "Info.plist" XML file handling...
	//
	CSettingsHandler		m_userSettingsHandler;
	CProcessMap				ms_tagProcess;
	CString					m_strLastKeyRead;
};

MAKEAUTOPTR(CMessageTemplate);

