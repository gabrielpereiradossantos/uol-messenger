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
#include "./PluginPropertiesRegister.h"
#include "../model/PluginProperties.h"


CPluginPropertiesRegister::CPluginPropertiesRegister()
{
}


CPluginPropertiesRegister::~CPluginPropertiesRegister()
{
	m_pluginPropertiesMap.RemoveAll();
}


BOOL CPluginPropertiesRegister::RegisterPluginProperty(
		const CString& strPluginName, const CString& strPropertyName, 
		const CString& strPropertyValue)
{
	CPluginPropertiesMap::CPair* pPair = m_pluginPropertiesMap.Lookup(strPluginName);
	BOOL bResult = FALSE;
	
	if (pPair == NULL)
	{
		IUOLMessengerPluginProperties* pPluginProperties = new CPluginProperties(strPluginName);
		
		if (pPluginProperties)
		{
			if (pPluginProperties->AddPluginProperty(strPropertyName, strPropertyValue))
			{
				m_pluginPropertiesMap.SetAt(strPluginName, pPluginProperties);
				
				bResult = TRUE;
			}
		}
	}
	else
	{
		bResult = pPair->m_value->AddPluginProperty(strPropertyName, strPropertyValue);
	}
	
	return bResult;
}


BOOL CPluginPropertiesRegister::UnregisterPluginProperty(
		const CString& strPluginName, const CString& strPropertyName)
{
	CPluginPropertiesMap::CPair* pPair = m_pluginPropertiesMap.Lookup(strPluginName);
	
	if ( (pPair != NULL) && 
			(pPair->m_value->RemovePluginProperty(strPropertyName)) )
	{
		if (pPair->m_value->GetPluginPropertiesCount() == 0)
		{
			m_pluginPropertiesMap.RemoveKey(strPluginName);
		}
		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CPluginPropertiesRegister::UnregisterAllPluginProperties(
		const CString& strPluginName)
{
	CPluginPropertiesMap::CPair* pPair = m_pluginPropertiesMap.Lookup(strPluginName);
	
	if ( (pPair != NULL) && 
			(pPair->m_value->RemoveAllPluginProperties()) )
	{
		m_pluginPropertiesMap.RemoveKey(strPluginName);
		
		return TRUE;
	}
	
	return FALSE;
}


IUOLMessengerPluginPropertiesPtr CPluginPropertiesRegister::GetPluginProperties(
		const CString& strPluginName)
{
	CPluginPropertiesMap::CPair* pPair = m_pluginPropertiesMap.Lookup(strPluginName);
	
	if (pPair != NULL)
	{
		return pPair->m_value;
	}
	
	return IUOLMessengerPluginPropertiesPtr();
}


void CPluginPropertiesRegister::GetPluginPropertiesList(
		CAtlList<IUOLMessengerPluginPropertiesPtr>& listPluginProperties)
{
	CPluginPropertiesMap::CPair* pPair = NULL;
	
	listPluginProperties.RemoveAll();
	
	for (POSITION mapPos = m_pluginPropertiesMap.GetStartPosition(); mapPos; )
	{
		pPair = m_pluginPropertiesMap.GetNext(mapPos);
		
		if (pPair != NULL)
		{
			listPluginProperties.AddTail(pPair->m_value);
		}
	}
}

