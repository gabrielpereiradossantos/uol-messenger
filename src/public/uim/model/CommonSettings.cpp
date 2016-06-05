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
#include "CommonSettings.h"

#include "../resource.h"
#include <MinDependencySpecialFolder.h>
#include "SettingsTraits.h"
#include "../controller/UIMApplication.h"



CCommonSettings::CCommonSettings() :
	m_userSettingsHandler("")
{
	m_userSettingsHandler.AddSetting("Common", this);
}


CCommonSettings::~CCommonSettings()
{
}


BOOL CCommonSettings::LoadSettings()
{
	BOOL bRet = FALSE;

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	LoadDefaultValues();

	CString strCommonSettingsFilePath;

	CMinDependencySpecialFolder		minDSF;
	CString	strCompany;
	CString	strProduct;
	CString strCommonConfigDir;
	CString strCommonFileName;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strCommonConfigDir.LoadString(IDS_DIR_COMMON_CONFIG);
	strCommonFileName.LoadString(IDS_FILE_COMMON_SETTINGS);

	strCommonSettingsFilePath.Format(_T("%s%s\\%s\\%s\\%s"), 
			minDSF.GetCommonAppData(), 
			strCompany, 
			strProduct, 
			strCommonConfigDir,
			strCommonFileName);

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"strCommonSettingsFilePath = [%s].", strCommonSettingsFilePath);
	
	m_userSettingsHandler.Init(strCommonSettingsFilePath, FALSE, FALSE);

	bRet = m_userSettingsHandler.LoadSettings(TRUE);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bRet;
}


BOOL CCommonSettings::SaveSettings()
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	bRet = m_userSettingsHandler.SaveSettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CCommonSettings::SetValue(const CString& strKey, const CString& strValue)
{
	SetString((LPCTSTR) strKey, (LPCTSTR) strValue);
}


BOOL CCommonSettings::GetValue(const CString& strKey, CString& strValue) const
{
	std::string strRetValue;

	if (GetString((LPCTSTR) strKey, strRetValue))
	{
		strValue = strRetValue.c_str();

		return TRUE;
	}

	return FALSE;
}


void CCommonSettings::SetValue(const CString& strKey, int nValue)
{
	SetInteger((LPCTSTR) strKey, nValue);
}


BOOL CCommonSettings::GetValue(const CString& strKey, int& nValue) const
{
	if (GetInteger((LPCTSTR) strKey, nValue))
	{
		return TRUE;
	}

	return FALSE;
}


CString CCommonSettings::GetAppMode() const
{
	CString strAppMode;

	GetValue(IUOLMessengerCommonSettings::APP_MODE_SETTING, strAppMode);

	return strAppMode;
}


void CCommonSettings::SetAppMode(const CString& strMode)
{
	SetValue(IUOLMessengerCommonSettings::APP_MODE_SETTING, strMode);
}


CString CCommonSettings::GetDefaultTheme() const
{
	CString strDefaultTheme;

	GetValue(IUOLMessengerCommonSettings::APP_THEME_SETTING, strDefaultTheme);

	return strDefaultTheme;
}


void CCommonSettings::SetDefaultTheme(const CString& strDefaultTheme)
{
	SetValue(IUOLMessengerCommonSettings::APP_THEME_SETTING, strDefaultTheme);
}


void CCommonSettings::LoadDefaultValues()
{
	SetValue(IUOLMessengerCommonSettings::APP_MODE_SETTING, IUOLMessengerCommonSettings::APP_MODE_DEFAULT_VALUE);
	SetValue(IUOLMessengerCommonSettings::APP_THEME_SETTING, IUOLMessengerCommonSettings::APP_THEME_DEFAULT_VALUE);
	SetValue(IUOLMessengerCommonSettings::APP_STRING_RESOURCE_SETTING, IUOLMessengerCommonSettings::APP_STRING_RESOURCE_DEFAULT_VALUE);
}