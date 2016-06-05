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

#include "ContactListSettings.h"
#include "SettingsTraits.h"
#include "MinDependencySpecialFolder.h"
#include "../resource.h"


CContactListSettings::CContactListSettings() :
m_nSortType(IUOLMessengerContactListSettings::STATUS),
m_bShowOffline(FALSE),
m_bShowGroups(TRUE),
m_bTopMost(FALSE),
m_enSizeImage(MEDIUM_IMAGE),
m_bNotifyOnline(TRUE),
m_bNotifyOffline(TRUE),
m_nSecurityLevel(IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE),
m_sContactNodeLayout(IUOLMessengerContactListSettings2::AVATAR_ALIAS_STATUS_LAYOUT),
m_bGroupByAccounts(FALSE)
{
	m_settingEventSource.SetEventSource(this);

	InitTagProcessMap();
}

CContactListSettings::~CContactListSettings(void)
{
}

void CContactListSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"SortType", NEW_SETTINGS_CALLBACK(SortType, this)},
		{"ShowOffline", NEW_SETTINGS_CALLBACK(ShowOffline, this)},
		{"ShowGroups", NEW_SETTINGS_CALLBACK(ShowGroups, this)},
		{"NotifyOnline", NEW_SETTINGS_CALLBACK(NotifyOnline, this)},
		{"NotifyOffline", NEW_SETTINGS_CALLBACK(NotifyOffline, this)},
		{"TopMost", NEW_SETTINGS_CALLBACK(TopMost, this)},
		{"SizeImage", NEW_SETTINGS_CALLBACK(SizeImage, this)},
		{"SecurityLevel", NEW_SETTINGS_CALLBACK(SecurityLevel, this)},
		{"ContactNodeLayout", NEW_SETTINGS_CALLBACK(ContactNodeLayout, this)},
		{"GroupByAccounts", NEW_SETTINGS_CALLBACK(GroupByAccounts, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CContactListSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CContactListSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}

UINT CContactListSettings::GetSortType()
{
	return m_nSortType;
}

void CContactListSettings::SetSortType(UINT nSortType)
{
	m_nSortType = nSortType;

	//NotifyEventListeners(IUOLMessengerContactListSettings::CL_SORT_TYPE);
}

BOOL CContactListSettings::IsShowOfflineEnabled()
{
	return m_bShowOffline;
}

void CContactListSettings::EnableShowOffline(BOOL bShowOffline)
{
	m_bShowOffline = bShowOffline;

	//NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_OFFLINE);
}

BOOL CContactListSettings::IsShowGroupsEnabled()
{
	return m_bShowGroups;
}

void CContactListSettings::EnableShowGroups(BOOL bShowGroups)
{
	m_bShowGroups = bShowGroups;

	//NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_GROUPS);
}

BOOL CContactListSettings::GetNotifyOnline()
{
	return m_bNotifyOnline;
}

BOOL CContactListSettings::GetNotifyOffline()
{
	return m_bNotifyOffline;
}

void CContactListSettings::SetNotifyOnline(BOOL bNotifyOnline)
{
	m_bNotifyOnline = bNotifyOnline;
}

void CContactListSettings::SetNotifyOffline(BOOL bNotifyOffline)
{
	m_bNotifyOffline = bNotifyOffline;
}

BOOL CContactListSettings::IsTopMost() const
{
	return m_bTopMost;
}

void CContactListSettings::SetTopMost(BOOL bTopMost)
{
	m_bTopMost = bTopMost;
}

void CContactListSettings::SetSizeImage(SizeImage nSizeImage)
{
	m_enSizeImage = nSizeImage;
}

SizeImage CContactListSettings::GetSizeImage() const
{
	return (m_enSizeImage);
}

UINT CContactListSettings::GetSecurityLevel()
{
	return m_nSecurityLevel;
}

void CContactListSettings::SetSecurityLevel(UINT nSecurityLevel)
{
	m_nSecurityLevel = nSecurityLevel;
}

CString CContactListSettings::GetContactNodeLayout()
{
	return m_sContactNodeLayout;
}

void CContactListSettings::SetContactNodeLayout(CString sContactNodeLayout)
{
	m_sContactNodeLayout = sContactNodeLayout;
}

BOOL CContactListSettings::GetGroupByAccounts()
{
	return m_bGroupByAccounts;
}

void CContactListSettings::SetGroupByAccounts(BOOL bGroupByAccounts)
{
	m_bGroupByAccounts = bGroupByAccounts;
}


void CContactListSettings::AddEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.AddEventListener(pListener);
}

void CContactListSettings::RemoveEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.RemoveEventListener(pListener);
}

void CContactListSettings::NotifyEventListeners(UINT nSettingId)
{
	m_settingEventSource.NotifyEventListeners(nSettingId);
}


IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, SortType)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, (DWORD&)m_pThis->m_nSortType);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, (DWORD)m_pThis->m_nSortType);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, ShowOffline)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bShowOffline);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bShowOffline);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, ShowGroups)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bShowGroups);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bShowGroups);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, NotifyOnline)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bNotifyOnline);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bNotifyOnline);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, NotifyOffline)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bNotifyOffline);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bNotifyOffline);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, TopMost)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bTopMost);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bTopMost);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, SizeImage)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, (DWORD&) m_pThis->m_enSizeImage);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, (DWORD&) m_pThis->m_enSizeImage);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, SecurityLevel)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, (DWORD&) m_pThis->m_nSecurityLevel);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, (DWORD&) m_pThis->m_nSecurityLevel);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, ContactNodeLayout)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&) m_pThis->m_sContactNodeLayout);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString&) m_pThis->m_sContactNodeLayout);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CContactListSettings, GroupByAccounts)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bGroupByAccounts);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bGroupByAccounts);
	}
}

