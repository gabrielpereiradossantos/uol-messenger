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
#include "ContactUserStatusHelper.h"

#include <interfaces/IUOLMessengerContact.h>

#include <msnconstants.h>
#include <oscarconstants.h>
#include <jabberconstants.h>
#include <yahooconstants.h>


int CContactUserStatusHelper::GetContactStatus(const CString& strProtocolId, DWORD dwStatus)
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

int CContactUserStatusHelper::GetYahooUserStatus(DWORD dwStatus)
{
	int nContactStatus = IUOLMessengerContact::CS_ONLINE;

	switch(dwStatus)
	{
	case YAHOO_STATUS_BUSY:
	case YAHOO_STATUS_ONPHONE:
		nContactStatus = IUOLMessengerContact::CS_BUSY;
		break;

	case YAHOO_STATUS_BRB:
	case YAHOO_STATUS_NOTATHOME:
	case YAHOO_STATUS_NOTATDESK:
	case YAHOO_STATUS_NOTINOFFICE:
	case YAHOO_STATUS_ONVACATION:
	case YAHOO_STATUS_OUTTOLUNCH:
	case YAHOO_STATUS_STEPPEDOUT:
	case YAHOO_STATUS_AWAY:
		nContactStatus = IUOLMessengerContact::CS_AWAY;
		break;
		
	case YAHOO_STATUS_INVISIBLE:
		nContactStatus = IUOLMessengerContact::CS_OFFLINE;
		break;

	default:	// YAHOO_STATUS_AVAILABLE
		nContactStatus = IUOLMessengerContact::CS_ONLINE;
		break;
	}

	return nContactStatus;
}

int CContactUserStatusHelper::GetJabberUserStatus(DWORD dwStatus)
{
	int nContactStatus = IUOLMessengerContact::CS_ONLINE;

	switch(dwStatus)
	{
	case JABBER_STATUS_DO_NOT_DISTURB:
		nContactStatus = IUOLMessengerContact::CS_BUSY;
		break;

	case JABBER_STATUS_AWAY:
	case JABBER_STATUS_EXTENDED_AWAY:
		nContactStatus = IUOLMessengerContact::CS_AWAY;
		break;
		
	case JABBER_STATUS_UNAVAILABLE:
	case JABBER_STATUS_ERROR:
		nContactStatus = IUOLMessengerContact::CS_OFFLINE;
		break;

	default:	// JABBER_STATUS_ONLINE
		nContactStatus = IUOLMessengerContact::CS_ONLINE;
		break;
	}

	return nContactStatus;
}


int CContactUserStatusHelper::GetMSNUserStatus(DWORD dwStatus)
{
	int nContactStatus = IUOLMessengerContact::CS_ONLINE;

	switch(dwStatus)
	{
	case MSN_STATUS_BUSY:
	case MSN_STATUS_ON_PHONE:
		nContactStatus = IUOLMessengerContact::CS_BUSY;
		break;

	case MSN_STATUS_AWAY:
	case MSN_STATUS_BE_RIGHT_BACK:
	case MSN_STATUS_OUT_TO_LUNCH:
		nContactStatus = IUOLMessengerContact::CS_AWAY;
		break;
		
	case MSN_STATUS_HIDDEN:
	case MSN_STATUS_MOBILE:
	case MSN_STATUS_OFFLINE:
		nContactStatus = IUOLMessengerContact::CS_OFFLINE;
		break;

	default:	// MSN_STATUS_AVAILABLE:
		nContactStatus = IUOLMessengerContact::CS_ONLINE;
		break;
	}

	return nContactStatus;
}


int CContactUserStatusHelper::GetOscarUserStatus(DWORD dwStatus)
{
	int nContactStatus = IUOLMessengerContact::CS_ONLINE;

	switch(dwStatus)
	{
	case OSCAR_STATUS_ICQ_OCCUPIED:
	case OSCAR_STATUS_ICQ_DO_NOT_DISTURB:
		nContactStatus = IUOLMessengerContact::CS_BUSY;
		break;

	case OSCAR_STATUS_ICQ_AWAY:
	case OSCAR_STATUS_ICQ_NOT_AVAILABLE:
		nContactStatus = IUOLMessengerContact::CS_AWAY;
		break;
		
	//case OSCAR_STATUS_GENERAL_INVISIBLE:
	//	nContactStatus = IUOLMessengerContact::CS_OFFLINE;
	//	break;

	default:	// OSCAR_STATUS_GENERAL_AVAILABLE:
		nContactStatus = IUOLMessengerContact::CS_ONLINE;
		break;
	}

	return nContactStatus;
}


