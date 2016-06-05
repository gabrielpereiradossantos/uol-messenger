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
#include "AccountProtocolSettings.h"

#include "SettingsTraits.h"


using namespace std;



CAccountProtocolSettingsElement::CAccountProtocolSettingsElement()
{
	InitTagProcessMap();
}


CAccountProtocolSettingsElement::~CAccountProtocolSettingsElement()
{
}


void CAccountProtocolSettingsElement::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Name", NEW_SETTINGS_CALLBACK(Name, this)},
		{"Type", NEW_SETTINGS_CALLBACK(Type, this)},
		{"Value", NEW_SETTINGS_CALLBACK(Value, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		m_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CAccountProtocolSettingsElement::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, m_tagProcess, this);
}


void CAccountProtocolSettingsElement::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, m_tagProcess, this);
}


IMPLEMENT_SETTINGS_CALLBACK(CAccountProtocolSettingsElement, Name)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strName);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strName);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccountProtocolSettingsElement, Type)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strType);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strType);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccountProtocolSettingsElement, Value)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strValue);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strValue);
	}
}











CAccountProtocolSettings::CAccountProtocolSettings()
{
	InitTagProcessMap();
}


CAccountProtocolSettings::~CAccountProtocolSettings()
{
}


void CAccountProtocolSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Setting", NEW_SETTINGS_CALLBACK(Setting, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		m_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CAccountProtocolSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, m_tagProcess, this);
}


void CAccountProtocolSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, m_tagProcess, this);
}


IMPLEMENT_SETTINGS_CALLBACK(CAccountProtocolSettings, Setting)
{
	if (bSave)
	{
		m_pThis->SaveIntegerMap(pNode, strKey);
		m_pThis->SaveStringMap(pNode, strKey);
		m_pThis->SaveBooleanMap(pNode, strKey);
	}
	else
	{
		m_pThis->LoadMaps(pNode, strKey);
	}
}


void CAccountProtocolSettings::SaveIntegerMap(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CAccountProtocolSettingsElement apse;
	apse.m_strType = "integer";

	CIntegerMap::const_iterator it;

	for (it = m_mapInteger.begin(); it != m_mapInteger.end(); it++)
	{
		MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);

		if (pChild)
		{
			apse.m_strName = it->first.c_str();
			apse.m_strValue.Format("%d", it->second);
			apse.Save(pChild, strKey);
		}
	}
}


void CAccountProtocolSettings::SaveStringMap(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CAccountProtocolSettingsElement apse;
	apse.m_strType = "string";

	CStringMap::const_iterator it;

	for (it = m_mapString.begin(); it != m_mapString.end(); it++)
	{
		MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);

		if (pChild)
		{
			apse.m_strName = it->first.c_str();
			apse.m_strValue = it->second.c_str();
			apse.Save(pChild, strKey);
		}
	}
}


void CAccountProtocolSettings::SaveBooleanMap(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CAccountProtocolSettingsElement apse;
	apse.m_strType = "boolean";

	CBooleanMap::const_iterator it;

	for (it = m_mapBoolean.begin(); it != m_mapBoolean.end(); it++)
	{
		MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);

		if (pChild)
		{
			apse.m_strName = it->first.c_str();
			apse.m_strValue.Format("%d", it->second);
			apse.Save(pChild, strKey);
		}
	}
}


void CAccountProtocolSettings::LoadMaps(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CAccountProtocolSettingsElement apse;

	apse.Load(pNode, strKey);

	if (0 == apse.m_strType.CompareNoCase("integer"))
	{
		int nValue = ::_ttol(apse.m_strValue);
		
		SetInteger((LPCTSTR) apse.m_strName, nValue);
	}
	else if (0 == apse.m_strType.CompareNoCase("boolean"))
	{
		BOOL bValue = ::_ttol(apse.m_strValue);

		SetBoolean((LPCTSTR) apse.m_strName, (bValue != FALSE));
	}
	else if (0 == apse.m_strType.CompareNoCase("string"))
	{
		SetString((LPCTSTR) apse.m_strName, (LPCTSTR) apse.m_strValue);
	}
}