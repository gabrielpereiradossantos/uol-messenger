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
#include "NetworkSettings.h"

#include "SettingsTraits.h"


CNetworkSettings::CNetworkSettings() : 
m_pNetworkProxy(new CNetworkProxy())
{
	m_settingEventSource.SetEventSource(this);

	InitTagProcessMap();
}


CNetworkSettings::~CNetworkSettings()
{
}


void CNetworkSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Proxy", NEW_SETTINGS_CALLBACK(Proxy, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		m_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CNetworkSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, m_tagProcess, this);
}


void CNetworkSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, m_tagProcess, this);
}


int CNetworkSettings::GetProxyType()
{
	return m_pNetworkProxy->GetProxyType();
}


void CNetworkSettings::SetProxyType(int nType)
{
	m_pNetworkProxy->SetProxyType(nType);
}


CString CNetworkSettings::GetProxyHost()
{
	return m_pNetworkProxy->GetProxyHost();
}


void CNetworkSettings::SetProxyHost(const CString& strHost)
{
	m_pNetworkProxy->SetProxyHost(strHost);
}


int CNetworkSettings::GetProxyPort()
{
	return m_pNetworkProxy->GetProxyPort();
}


void CNetworkSettings::SetProxyPort(int nPort)
{
	m_pNetworkProxy->SetProxyPort(nPort);
}


CString CNetworkSettings::GetProxyUsername()
{
	return m_pNetworkProxy->GetProxyUsername();
}


void CNetworkSettings::SetProxyUsername(const CString& strUsername)
{
	m_pNetworkProxy->SetProxyUsername(strUsername);
}


CString CNetworkSettings::GetProxyPassword()
{
	return m_pNetworkProxy->GetProxyPassword();
}


void CNetworkSettings::SetProxyPassword(const CString& strPassword)
{
	m_pNetworkProxy->SetProxyPassword(strPassword);
}


void CNetworkSettings::AddEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.AddEventListener(pListener);
}


void CNetworkSettings::RemoveEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.RemoveEventListener(pListener);
}


void CNetworkSettings::NotifyEventListeners(UINT nSettingId)
{
	m_settingEventSource.NotifyEventListeners(nSettingId);
}


IMPLEMENT_SETTINGS_CALLBACK(CNetworkSettings, Proxy)
{
	CNetworkProxyPtr pNetworkProxy;

	if (bSave)
	{
		MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
		m_pThis->m_pNetworkProxy->Save(pChild, strKey);
	}
	else
	{
		pNetworkProxy = new CNetworkProxy();

		if (pNetworkProxy)
		{
			pNetworkProxy->Load(pNode, strKey);
			m_pThis->m_pNetworkProxy = pNetworkProxy;
		}
	}
}
