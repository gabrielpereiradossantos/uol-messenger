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


#include "protocolsettings.h"


CProtocolSettings::~CProtocolSettings()
{
}


void CProtocolSettings::SetInteger(const std::string& strSetting, int nValue)
{
	m_mapInteger[strSetting] = nValue;
}


bool CProtocolSettings::GetInteger(const std::string& strSetting, int& nValue) const
{
	CIntegerMap::const_iterator it = m_mapInteger.find(strSetting);

	if (it != m_mapInteger.end())
	{
		nValue = it->second;
		return true;
	}

	return false;
}


void CProtocolSettings::SetString(const std::string& strSetting, const std::string& strValue)
{
	m_mapString[strSetting] = strValue;
}


bool CProtocolSettings::GetString(const std::string& strSetting, std::string& strValue) const
{
	CStringMap::const_iterator it = m_mapString.find(strSetting);

	if (it != m_mapString.end())
	{
		strValue = it->second;
		return true;
	}

	return false;
}


void CProtocolSettings::SetBoolean(const std::string& strSetting, bool bValue)
{
	m_mapBoolean[strSetting] = bValue;
}


bool CProtocolSettings::GetBoolean(const std::string& strSetting, bool& bValue) const
{
	CBooleanMap::const_iterator it = m_mapBoolean.find(strSetting);

	if (it != m_mapBoolean.end())
	{
		bValue = it->second;
		return true;
	}

	return false;
}






////////////////////////////////////////////
// CProtocolSettings2


CProtocolSettings2::~CProtocolSettings2()
{
}


CProtocolSettings2* CProtocolSettings2::Clone() const
{
	CProtocolSettings2* pProtocolSettings = new CProtocolSettings2();

	if (pProtocolSettings)
	{
		for (CIntegerMap::const_iterator itInt = m_mapInteger.begin(); itInt != m_mapInteger.end(); itInt++)
		{
			pProtocolSettings->SetInteger(itInt->first, itInt->second);
		}

		for (CStringMap::const_iterator itStr = m_mapString.begin(); itStr != m_mapString.end(); itStr++)
		{
			pProtocolSettings->SetString(itStr->first, itStr->second);
		}

		for (CBooleanMap::const_iterator itBool = m_mapBoolean.begin(); itBool != m_mapBoolean.end(); itBool++)
		{
			pProtocolSettings->SetBoolean(itBool->first, itBool->second);
		}
	}

	return pProtocolSettings;
}






////////////////////////////////////////////
// CProtocolSettings3


CProtocolSettings3::~CProtocolSettings3()
{
}


void CProtocolSettings3::GetIntegerSettingsList(
		CStringList& settingList, CIntegerList& valueList)
{
	settingList.clear();
	valueList.clear();
	
	CIntegerMap::const_iterator itIntegerMap;
	
	for (itIntegerMap = m_mapInteger.begin(); 
			itIntegerMap != m_mapInteger.end(); itIntegerMap++)
	{
		settingList.push_front(itIntegerMap->first);
		valueList.push_front(itIntegerMap->second);
	}
}


void CProtocolSettings3::GetStringSettingsList(
		CStringList& settingList, CStringList& valueList)
{
	settingList.clear();
	valueList.clear();
	
	CStringMap::const_iterator itStringMap;
	
	for (itStringMap = m_mapString.begin(); 
			itStringMap != m_mapString.end(); itStringMap++)
	{
		settingList.push_front(itStringMap->first);
		valueList.push_front(itStringMap->second);
	}
}


void CProtocolSettings3::GetBooleanSettingsList(
		CStringList& settingList, CBooleanList& valueList)
{
	settingList.clear();
	valueList.clear();
	
	CBooleanMap::const_iterator itBooleanMap;
	
	for (itBooleanMap = m_mapBoolean.begin(); 
			itBooleanMap != m_mapBoolean.end(); itBooleanMap++)
	{
		settingList.push_front(itBooleanMap->first);
		valueList.push_front(itBooleanMap->second);
	}
}

