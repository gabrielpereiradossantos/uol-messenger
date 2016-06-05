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

#include "Element.h"
#include "IconElement.h"
#include <interfaces/IUOLMessengerAccountConnectionManager.h>


class CAbstractStatusTrayIconElement : public CElement
{
public:
	CAbstractStatusTrayIconElement();
	virtual ~CAbstractStatusTrayIconElement();

	CIconElementPtr GetIcon();

	virtual CString GetName() const = 0;
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const = 0;

private:
	CIconElementPtr		m_pIcon;
};

MAKEAUTOPTR(CAbstractStatusTrayIconElement);



class COfflineStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~COfflineStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("OfflineStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__OfflineStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_OFFLINE;
	}
};


class COnlineStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~COnlineStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("OnlineStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__OnlineStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_ONLINE;
	}
};


class CAwayStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~CAwayStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("AwayStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__AwayStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_AWAY;
	}
};


class CBusyStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~CBusyStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("BusyStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__BusyStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_BUSY;
	}
};


class CIdleStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~CIdleStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("IdleStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__IdleStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY;
	}
};


class CInvisibleStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~CInvisibleStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("InvisibleStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__InvisibleStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_INVISIBLE;
	}
};


class CCustomStatusTrayIconElement : public CAbstractStatusTrayIconElement
{
public:
	virtual ~CCustomStatusTrayIconElement() {}

	DECLARE_ELEMENT_NAME("CustomStatusTrayIcon");

	virtual CString GetName() const
	{
		return _T("__CustomStatusTrayIconInstance");
	}

	virtual IUOLMessengerAccountConnectionManager::AccountStatus GetStatus() const
	{
		return IUOLMessengerAccountConnectionManager::AS_PERSONALIZED;
	}
};