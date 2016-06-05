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

#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerNotifyPopUpResource.h>


enum UserMessages
{
	WM_USER_ADD_MESSAGE = WM_USER + 0x100,
	WM_USER_NOTIFY_ADDED,
	WM_USER_SHOW_ALERT,
	WM_USER_UPDATE_LAYOUT,
	WM_USER_INTERNET_CONNECTION_STATE_CHANGED,
	WM_USER_SHOW_CONNECTION_ERROR
};


typedef struct _ADDMESSAGEPARAM
{
	IUOLMessengerContactPtr pContact;
	CString strText;
	BOOL bSent;
	BOOL bNeedConfirmation;

	_ADDMESSAGEPARAM& operator= (const _ADDMESSAGEPARAM& amp)
	{
		pContact = amp.pContact;
		strText = amp.strText;
		bSent = amp.bSent;
		bNeedConfirmation = amp.bNeedConfirmation;

		return *this;
	}
} ADDMESSAGEPARAM, *LPADDMESSAGEPARAM;


typedef struct _NOTIFYADDEDPARAM
{
	IUOLMessengerContactPtr	pContact;
	BOOL		bHasAlreadyAdded;
} NOTIFYADDEDPARAM, *LPNOTIFYADDEDPARAM;


typedef struct _SHOWALERTPARAM
{
	IUOLMessengerContactPtr pContact;
	CString	strLine1;
	CString	strLine2;
	IUOLMessengerNotifyPopUpResource::NotifyPopUpResource resource;


	_SHOWALERTPARAM& operator= (const _SHOWALERTPARAM& sap)
	{
		pContact = sap.pContact;
		strLine1 = sap.strLine1;
		strLine2 = sap.strLine2;
		resource = sap.resource;

		return *this;
	}
} SHOWALERTPARAM, *LPSHOWALERTPARAM;


typedef struct _SHOWCONNECTIONERRORPARAM
{
	IUOLMessengerAccountPtr pAccount;
	ConnectionError error;
	CString message;

	_SHOWCONNECTIONERRORPARAM& operator= (const _SHOWCONNECTIONERRORPARAM& scep)
	{
		pAccount = scep.pAccount;
		error = scep.error;
		message = scep.message;
		return *this;
	}

} SHOWCONNECTIONERRORPARAM, *LPSHOWCONNECTIONERRORPARAM;