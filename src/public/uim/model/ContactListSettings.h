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

#include <interfaces/IUOLMessengerContactListSettings.h>

#include "ISettings.h"
#include "../controller/SettingsEventSource.h"



class CContactListSettings : public IUOLMessengerContactListSettings2,
							 public ISettings//,
							 //public CSettingsEventSource
{
public:
	CContactListSettings();
	virtual ~CContactListSettings(void);

protected:
	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	virtual UINT GetSortType();
	virtual void SetSortType(UINT nSortType);

	virtual BOOL IsShowOfflineEnabled();
	virtual void EnableShowOffline(BOOL bShowOffline);

	virtual BOOL IsShowGroupsEnabled();
	virtual void EnableShowGroups(BOOL bShowGroups);

	virtual BOOL GetNotifyOnline();
	virtual void SetNotifyOnline(BOOL bNotifyOnline);

	virtual BOOL GetNotifyOffline();
	virtual void SetNotifyOffline(BOOL bNotifyOffline);

	virtual BOOL IsTopMost() const;
	virtual void SetTopMost(BOOL bTopMost);

	virtual void SetSizeImage(SizeImage nSizeImage);
	virtual SizeImage GetSizeImage() const;

	virtual UINT GetSecurityLevel();
	virtual void SetSecurityLevel(UINT nSecurityLevel);
	
	virtual CString GetContactNodeLayout();
	virtual void SetContactNodeLayout(CString sContactNodeLayout);
	
	virtual BOOL GetGroupByAccounts();
	virtual void SetGroupByAccounts(BOOL bGroupByAccounts);
	
	void AddEventListener(IUOLMessengerSettingsEventListener* pListener);
	void RemoveEventListener(IUOLMessengerSettingsEventListener* pListener);
	void NotifyEventListeners(UINT nSettingId);

protected:
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, SortType);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, ShowOffline);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, ShowGroups);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, TopMost);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, NotifyOnline);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, NotifyOffline);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, SizeImage);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, SecurityLevel);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, ContactNodeLayout);
	DECLARE_SETTINGS_CALLBACK(CContactListSettings, GroupByAccounts);

private:
	CProcessMap				ms_tagProcess;
	CSettingsEventSource	m_settingEventSource;

	UINT					m_nSortType;
	BOOL					m_bShowOffline;
	BOOL					m_bShowGroups;
	BOOL					m_bTopMost;
	BOOL					m_bNotifyOnline;
	BOOL					m_bNotifyOffline;
	SizeImage				m_enSizeImage;
	UINT					m_nSecurityLevel;
	CString					m_sContactNodeLayout;
	BOOL					m_bGroupByAccounts;

};

MAKEAUTOPTR(CContactListSettings);
