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

#include <interfaces/IUOLMessengerSettingsEventSource.h>



class  __declspec(novtable) IUOLMessengerContactListSettings : public IUOLMessengerSettingsEventSource
{
public:
	static enum SettingId
	{
		CL_SORT_TYPE = 0,
		CL_SHOW_OFFLINE,
		CL_SHOW_GROUPS,
		CL_NOTIFY_TYPE,
		CL_TOPMOST,
		CL_SIZEIMAGE,
		CL_SECURITY_LEVEL,
		CL_CONTACT_NODE_LAYOUT,
		CL_GROUP_BY_ACCOUNTS
	};

	enum SORT_TYPE
	{ 
		ASCENDING = 0, 
		DESCENDING = 1, 
		NAME = 2, 
		STATUS = 4, 
		BOTH = 6 
	};
	
	enum SIZE_IMAGE
	{
		SMALL_IMAGE,
		MEDIUM_IMAGE,
		GREAT_IMAGE
	};

	enum SECURITY_LEVEL
	{
		SL_RECEIVE_FROM_ANY_USER = 0,
		SL_RECEIVE_ONLY_FROM_LIST,
		SL_CONFIRM_RECEIVE
	};


public:
	virtual ~IUOLMessengerContactListSettings() {}

	virtual UINT GetSortType() = 0;
	virtual void SetSortType(UINT nSortType) = 0;

	virtual BOOL IsShowOfflineEnabled() = 0;
	virtual void EnableShowOffline(BOOL bShowOffline) = 0;

	virtual BOOL IsShowGroupsEnabled() = 0;
	virtual void EnableShowGroups(BOOL bShowGroups) = 0;

	virtual BOOL GetNotifyOnline() = 0;
	virtual void SetNotifyOnline(BOOL bNotifyOnline) = 0;

	virtual BOOL GetNotifyOffline() = 0;
	virtual void SetNotifyOffline(BOOL bNotifyOffline) = 0;

	virtual void SetTopMost(BOOL bTopMost) = 0;
	virtual BOOL IsTopMost() const = 0;

	virtual void SetSizeImage(SIZE_IMAGE nSizeImage) = 0;
	virtual SIZE_IMAGE GetSizeImage() const = 0;

	virtual UINT GetSecurityLevel() = 0;
	virtual void SetSecurityLevel(UINT nMessageReceivingSecurity) = 0;
};

typedef IUOLMessengerContactListSettings::SIZE_IMAGE SizeImage;

MAKEAUTOPTR(IUOLMessengerContactListSettings);



class __declspec(novtable) IUOLMessengerContactListSettings2 : public IUOLMessengerContactListSettings
{
public:
	
	enum ContactNodeElements
	{
		ALIAS_ELEMENT = 1,
		AVATAR_ELEMENT,
		STATUS_ELEMENT,
		BUTTONS_ELEMENT
	};
	
	//ContactNodeLayouts
	static const CString ALIAS_LAYOUT;
	static const CString ALIAS_AVATAR_LAYOUT;
	static const CString AVATAR_ALIAS_LAYOUT;
	static const CString ALIAS_STATUS_LAYOUT;
	static const CString STATUS_ALIAS_LAYOUT;
	static const CString AVATAR_ALIAS_STATUS_LAYOUT;
	static const CString AVATAR_STATUS_ALIAS_LAYOUT;
	static const CString ALIAS_AVATAR_STATUS_LAYOUT;
	static const CString ALIAS_STATUS_AVATAR_LAYOUT;
	static const CString STATUS_ALIAS_AVATAR_LAYOUT;
	static const CString STATUS_AVATAR_ALIAS_LAYOUT;
	
public:
	
	virtual ~IUOLMessengerContactListSettings2() {}
	
    virtual CString GetContactNodeLayout() = 0;
	virtual void SetContactNodeLayout(CString sContactNodeLayout) = 0;
	
	virtual BOOL GetGroupByAccounts() = 0;
	virtual void SetGroupByAccounts(BOOL bGroupByAccounts) = 0;	
};

MAKEAUTOPTR(IUOLMessengerContactListSettings2);



__declspec(selectany)
const CString IUOLMessengerContactListSettings2::ALIAS_LAYOUT = _T("1");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::ALIAS_AVATAR_LAYOUT = _T("1,2");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::AVATAR_ALIAS_LAYOUT = _T("2,1");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::ALIAS_STATUS_LAYOUT = _T("1,3");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::STATUS_ALIAS_LAYOUT = _T("3,1");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::AVATAR_ALIAS_STATUS_LAYOUT = _T("2,1,3");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::AVATAR_STATUS_ALIAS_LAYOUT = _T("2,3,1");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::ALIAS_AVATAR_STATUS_LAYOUT = _T("1,2,3");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::ALIAS_STATUS_AVATAR_LAYOUT = _T("1,3,2");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::STATUS_ALIAS_AVATAR_LAYOUT = _T("3,1,2");

__declspec(selectany)
const CString IUOLMessengerContactListSettings2::STATUS_AVATAR_ALIAS_LAYOUT = _T("3,2,1");
