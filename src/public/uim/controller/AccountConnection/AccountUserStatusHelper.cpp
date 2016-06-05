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
#include "AccountUserStatusHelper.h"

#include <interfaces/IUOLMessengerProtocolManager.h>
#include <interfaces/IUOLMessengerAccountConnectionManager.h>

#include <msnconstants.h>
#include <oscarconstants.h>
#include <jabberconstants.h>
#include <yahooconstants.h>


int CAccountUserStatusHelper::GetProtocolUserStatus(const CString& strProtocolId, DWORD dwStatus)
{
	if (0 == strProtocolId.CompareNoCase("prpl-msn"))
	{
		return GetMSNUserStatus(dwStatus);
	}

	if ( (0 == strProtocolId.CompareNoCase("prpl-icq")) || 
			(0 == strProtocolId.CompareNoCase("prpl-aim")) )
	{
		return GetOscarUserStatus(dwStatus);
	}

	if (0 == strProtocolId.CompareNoCase("prpl-jabber"))
	{
		return GetJabberUserStatus(dwStatus);
	}

	if (0 == strProtocolId.CompareNoCase("prpl-uol"))
	{
		return GetJabberUserStatus(dwStatus);
	}

	/// <TODAOFERTA>
	if (0 == strProtocolId.CompareNoCase("prpl-uolto"))
	{
		return GetJabberUserStatus(dwStatus);
	}
	/// </TODAOFERTA>

	if (0 == strProtocolId.CompareNoCase("prpl-yahoo"))
	{
		return GetYahooUserStatus(dwStatus);
	}

	ATLASSERT(FALSE);
	return 0;
}


void CAccountUserStatusHelper::GetProtocolValidStatus(
		const CString& strProtocolId, CAtlList<int>& listValidStatus)
{
	listValidStatus.RemoveAll();
	
	// First fill up the common statuses.
	//
	listValidStatus.AddTail(IUOLMessengerAccountConnectionManager::AS_ONLINE);

	// Away and Busy status doesn't make any difference for UOL Toda Oferta, because both are offline actually
	if (0 != strProtocolId.CompareNoCase("prpl-uolto"))
	{
		listValidStatus.AddTail(IUOLMessengerAccountConnectionManager::AS_AWAY);
		listValidStatus.AddTail(IUOLMessengerAccountConnectionManager::AS_BUSY);
	}
	
	// Now the per protocol specific ones.
	//
	if ( (0 != strProtocolId.CompareNoCase("prpl-jabber")) && 
			(0 != strProtocolId.CompareNoCase("prpl-uol")) && 
			(0 != strProtocolId.CompareNoCase("prpl-uolto")) )
	{
		listValidStatus.AddTail(IUOLMessengerAccountConnectionManager::AS_INVISIBLE);
	}
	
	// Disconnect status must be the last.
	//
	listValidStatus.AddTail(IUOLMessengerAccountConnectionManager::AS_OFFLINE);
}


void CAccountUserStatusHelper::GetProtocolValidStatusMapping(
		const CString& strProtocolId, DWORD dwSettingStatus, DWORD& dwValidStatus)
{
	dwValidStatus = dwSettingStatus;
	
	// For now, only these protocols demands status mapping.
	//
	if ( (0 == strProtocolId.CompareNoCase("prpl-jabber")) || 
			(0 == strProtocolId.CompareNoCase("prpl-uol")) || 
			(0 == strProtocolId.CompareNoCase("prpl-uolto")) )
	{
		if (IUOLMessengerAccountConnectionManager::AS_INVISIBLE == dwSettingStatus)
		{
			dwValidStatus = IUOLMessengerAccountConnectionManager::AS_OFFLINE;
		}
	}
}


int CAccountUserStatusHelper::GetYahooUserStatus(DWORD dwStatus)
{
	int nProtocolStatus = YAHOO_STATUS_AVAILABLE;

	switch(dwStatus)
	{
	case IUOLMessengerAccountConnectionManager::AS_BUSY:
		nProtocolStatus = YAHOO_STATUS_BUSY;
		break;

	case IUOLMessengerAccountConnectionManager::AS_AWAY:
	case IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY:
		nProtocolStatus = YAHOO_STATUS_BRB;
		break;
		
	case IUOLMessengerAccountConnectionManager::AS_INVISIBLE:
		nProtocolStatus = YAHOO_STATUS_INVISIBLE;
		break;

	default:	// IUOLMessengerAccountConnectionManager::AS_ONLINE:
		nProtocolStatus = YAHOO_STATUS_AVAILABLE;
		break;
	}

	return nProtocolStatus;
}

int CAccountUserStatusHelper::GetJabberUserStatus(DWORD dwStatus)
{
	int nProtocolStatus = JABBER_STATUS_ONLINE;

	switch(dwStatus)
	{
	case IUOLMessengerAccountConnectionManager::AS_BUSY:
		nProtocolStatus = JABBER_STATUS_DO_NOT_DISTURB;
		break;

	case IUOLMessengerAccountConnectionManager::AS_AWAY:
	case IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY:
		nProtocolStatus = JABBER_STATUS_AWAY;
		break;
		
	case IUOLMessengerAccountConnectionManager::AS_OFFLINE:
		nProtocolStatus = JABBER_STATUS_UNAVAILABLE;
		break;

	default:	// IUOLMessengerAccount::AS_ONLINE:
		nProtocolStatus = JABBER_STATUS_ONLINE;
		break;
	}

	return nProtocolStatus;
}


int CAccountUserStatusHelper::GetMSNUserStatus(DWORD dwStatus)
{
	int nProtocolStatus = MSN_STATUS_AVAILABLE;

	switch(dwStatus)
	{
	case IUOLMessengerAccountConnectionManager::AS_BUSY:
		nProtocolStatus = MSN_STATUS_BUSY;
		break;

	case IUOLMessengerAccountConnectionManager::AS_AWAY:
	case IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY:
		nProtocolStatus = MSN_STATUS_AWAY;
		break;
		
	case IUOLMessengerAccountConnectionManager::AS_INVISIBLE:
		nProtocolStatus = MSN_STATUS_HIDDEN;
		break;

	default:	// IUOLMessengerAccount::AS_ONLINE:
		nProtocolStatus = MSN_STATUS_AVAILABLE;
		break;
	}

	return nProtocolStatus;
}


int CAccountUserStatusHelper::GetOscarUserStatus(DWORD dwStatus)
{
	int nProtocolStatus = OSCAR_STATUS_ICQ_AVAILABLE;

	switch(dwStatus)
	{
	case IUOLMessengerAccountConnectionManager::AS_BUSY:
		nProtocolStatus = OSCAR_STATUS_ICQ_OCCUPIED;
		break;

	case IUOLMessengerAccountConnectionManager::AS_AWAY:
	case IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY:
		nProtocolStatus = OSCAR_STATUS_ICQ_AWAY;
		break;
		
	case IUOLMessengerAccountConnectionManager::AS_INVISIBLE:
		nProtocolStatus = OSCAR_STATUS_ICQ_INVISIBLE;
		break;

	default:	// IUOLMessengerAccount::AS_ONLINE:
		nProtocolStatus = OSCAR_STATUS_ICQ_AVAILABLE;
		break;
	}

	return nProtocolStatus;
}


