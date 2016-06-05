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

#include "StatusSettings.h"
#include "SettingsTraits.h"

CStatusSettings::CStatusSettings() : 
m_bAutoAway(TRUE),
m_nTimeout(10),
m_strAwayMessage(_T(""))
{
	m_settingEventSource.SetEventSource(this);

	InitTagProcessMap();
}

CStatusSettings::~CStatusSettings(void)
{
}

void CStatusSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"AutoAway", NEW_SETTINGS_CALLBACK(AutoAway, this)},
		{"Timeout", NEW_SETTINGS_CALLBACK(Timeout, this)},
		{"AwayMessage", NEW_SETTINGS_CALLBACK(AwayMessage, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}
void CStatusSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CStatusSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}

BOOL CStatusSettings::IsAutoAwayEnabled()
{
	return m_bAutoAway;
}

void CStatusSettings::EnableAutoAway(BOOL bAutoAway)
{
	m_bAutoAway = bAutoAway;
}

UINT CStatusSettings::GetTimeout()
{
	return m_nTimeout;
}

void CStatusSettings::SetTimeout(UINT nTimeout)
{
	m_nTimeout = nTimeout;
}

CString CStatusSettings::GetAwayMessage() const
{
	return m_strAwayMessage;
}

void CStatusSettings::SetAwayMessage(const CString& strAwayMessage)
{
	m_strAwayMessage = strAwayMessage;
}

void CStatusSettings::AddEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.AddEventListener(pListener);
}

void CStatusSettings::RemoveEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.RemoveEventListener(pListener);
}

void CStatusSettings::NotifyEventListeners(UINT nSettingId)
{
	m_settingEventSource.NotifyEventListeners(nSettingId);
}

IMPLEMENT_SETTINGS_CALLBACK(CStatusSettings, AutoAway)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bAutoAway);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bAutoAway);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CStatusSettings, Timeout)
{
	if (bSave)
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, (DWORD)m_pThis->m_nTimeout);
	}
	else
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, (DWORD&)m_pThis->m_nTimeout);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CStatusSettings, AwayMessage)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strAwayMessage);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strAwayMessage);
	}
}