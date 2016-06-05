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
#include "UIMSettings.h"

#include "../resource.h"
#include "SettingsHandler.h"
#include "../controller/UIMApplication.h"


static const CString PLUGIN_SUBDIR = "UOL\\UIM\\plugins\\";
static const CString PLUGIN_LIB_SUBDIR = "UOL\\lib\\";
static const CString CACHE_SUBDIR = "UOL\\UIM\\cache\\";
static const CString CONFIG_SUBDIR = "UOL\\UIM\\config\\";



CUIMSettings::CUIMSettings(CString strProfilePath) : 
m_userSettingsHandler(strProfilePath + "\\Settings.xml", FALSE, NULL)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pContactListSettings = new CContactListSettings();
	m_pGeneralSettings = new CGeneralSettings();
	m_pMessageSettings = new CMessageSettings();
	m_pStatusSettings = new CStatusSettings();
	m_pNetworkSettings = new CNetworkSettings();

	m_userSettingsHandler.AddSetting("GeneralSettings", this);
	m_userSettingsHandler.AddSetting("ContactList", m_pContactListSettings.get());
	m_userSettingsHandler.AddSetting("General", m_pGeneralSettings.get());
	m_userSettingsHandler.AddSetting("Message", m_pMessageSettings.get());
	m_userSettingsHandler.AddSetting("Status", m_pStatusSettings.get());
	m_userSettingsHandler.AddSetting("Network", m_pNetworkSettings.get());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pContactListSettings = [%x], m_pGeneralSettings = [%x], m_pMessageSettings = [%x], m_pStatusSettings = [%x], and m_pNetworkSettings = [%x].", 
			m_pContactListSettings.get(), m_pGeneralSettings.get(), m_pMessageSettings.get(), m_pStatusSettings.get(), m_pNetworkSettings.get());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

CUIMSettings::~CUIMSettings(void)
{
}

BOOL CUIMSettings::Load()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bRet = m_userSettingsHandler.LoadSettings(TRUE);

	LoadPredefinedsettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d].", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}

BOOL CUIMSettings::Save()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bRet = m_userSettingsHandler.SaveSettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d].", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}

IUOLMessengerContactListSettingsPtr CUIMSettings::GetContactListSettings() const
{
	return m_pContactListSettings;
}

IUOLMessengerGeneralSettingsPtr CUIMSettings::GetGeneralSettings() const
{
	return m_pGeneralSettings;
}

IUOLMessengerMessageSettingsPtr CUIMSettings::GetMessageSettings() const
{
	return m_pMessageSettings;
}

IUOLMessengerStatusSettingsPtr CUIMSettings::GetStatusSettings() const
{
	return m_pStatusSettings;
}

IUOLMessengerNetworkSettingsPtr CUIMSettings::GetNetworkSettings() const
{
	return m_pNetworkSettings;
}

BOOL CUIMSettings::SetValue(const CString& strKey, const CString& strValue, BOOL bCanReplace)
{
	BOOL bLookup = NULL != m_mapSettings.Lookup(strKey);

	if ((bCanReplace) && (bLookup))
	{
		m_mapSettings.RemoveKey(strKey);
	}
	if ((!bCanReplace) && (bLookup))
	{	
		return FALSE;
	}	

	m_mapSettings[strKey] = strValue;

	return TRUE;
}

BOOL CUIMSettings::GetValue(const CString& strKey, CString& strValue) const
{
	const CStringMap::CPair* pPair = m_mapSettings.Lookup(strKey);

	if (pPair)
	{		
		strValue = pPair->m_value;
	}

	return NULL != pPair;
}

void CUIMSettings::InitTagProcessMap()
{	
}

void CUIMSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	try
	{
		MSXML::IXMLDOMNamedNodeMapPtr pAttrib = pNode->attributes;

		long lLength = pAttrib->length;

		for (long lInd = 0; lInd < lLength; lInd++)
		{
			MSXML::IXMLDOMNodePtr pItem = pAttrib->item[lInd];		

			_bstr_t bstrKey = pItem->nodeName;
			_variant_t vValue = pItem->nodeValue;

			m_mapSettings[bstrKey] = (LPCTSTR)_bstr_t(vValue);
		}
	}
	catch(const _com_error&)
	{
	}
	
}

void CUIMSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	try
	{		
		for (POSITION pos = m_mapSettings.GetStartPosition(); pos; )
		{
			CStringMap::CPair* pPair = m_mapSettings.GetNext(pos);

       		MSXML::IXMLDOMDocumentPtr pDoc = pNode->GetownerDocument();

			if (pDoc)
			{
				MSXML::IXMLDOMNamedNodeMapPtr pAttributes = pNode->Getattributes();

				if (pAttributes)
				{				
					MSXML::IXMLDOMNodePtr pAttr = pDoc->createAttribute((LPCTSTR)pPair->m_key);

					if (pAttr)
					{
						pAttr->PutnodeValue(_variant_t(pPair->m_value));
						pAttributes->setNamedItem(pAttr);									
					}
				}
			}	
		}	
	}
	catch(const _com_error&)
	{
	}
}

void CUIMSettings::LoadPredefinedsettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	LoadDirValue("uolmessenger.general.pluginsdir", PLUGIN_SUBDIR);
	LoadDirValue("uolmessenger.general.libdir", PLUGIN_LIB_SUBDIR);
	LoadDirValue("uolmessenger.general.cachedir", CACHE_SUBDIR);
	LoadDirValue("uolmessenger.general.configdir", CONFIG_SUBDIR);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CUIMSettings::LoadDirValue(const CString& strKey, const CString& strDefaultDirValue)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Loading strKey = [%s] with strDefaultDirValue = [%s]", 
			strKey, strDefaultDirValue);
	
	CString strDir;

	if (FALSE == GetValue(strKey, strDir))
	{	
		try
		{
			CMinDependencySpecialFolder mdsf;

			CString strAppDataFolder = mdsf.GetCommonAppData();

			if (FALSE == strAppDataFolder.IsEmpty())
			{
				strAppDataFolder += strDefaultDirValue;

				SetValue(strKey, strAppDataFolder);

				strDir = strAppDataFolder;
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogDebug(__FUNCTION__, 
						"Path build ok: strAppDataFolder = [%s].", strAppDataFolder);
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"strAppDataFolder returned is empty.");
				
				ATLASSERT(FALSE);
			}
		}
		catch(CSpecialFolderException* pEx)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"mdsf.GetCommonAppData() returned error = [%d]", pEx->m_hr);
			
			delete pEx;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pContactListSettings = [%x], m_pGeneralSettings = [%x], m_pMessageSettings = [%x], m_pStatusSettings = [%x], and m_pNetworkSettings = [%x].", 
			m_pContactListSettings.get(), m_pGeneralSettings.get(), m_pMessageSettings.get(), m_pStatusSettings.get(), m_pNetworkSettings.get());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}
