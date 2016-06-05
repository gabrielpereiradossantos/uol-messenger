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
#include "ProtocolResourceTranslator.h"
#include "UIMApplication.h"

#include "../resource.h"

CProtocolResourceTranslator::CProtocolResourceTranslator()
{
	InitEntries();
}


CProtocolResourceTranslator::~CProtocolResourceTranslator()
{
}

void CProtocolResourceTranslator::InitEntries()
{
	struct ConnectionErrorEntry
	{
		ConnectionError error;
		CString strResource;
	};

	ConnectionErrorEntry pConnectionErrorEntries[] =
	{
		{ CONNECTION_ERROR_UNKNOWN, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_GENERIC_CONNECTION_ERROR") },
		{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_USER_DATA_INVALID") },
		{ CONNECTION_ERROR_UNABLE_TO_CONNECT, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_UNABLE_TO_CONNECT") },
		{ CONNECTION_ERROR_LOGGED_ON_ANOTHER_LOCATION, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_USER_LOGGED") },
		{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_CONNECTION_FAILED") },
		{ CONNECTION_ERROR_SERVER_UNAVAILABLE, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_CONNECTION_FAILED") },
		{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_GENERIC_CONNECTION_ERROR") },
		{ CONNECTION_ERROR_UNAVAILABLE_BUDDY_LIST, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_UNAVAILABLE_BUDDY_LIST") },
		{ CONNECTION_ERROR_PROXY_CONNECTION_ERROR, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_PROXY_ERROR") },
		{ CONNECTION_ERROR_PROXY_ACCESS_DENIED, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_PROXY_ERROR") },
		{ CONNECTION_ERROR_SERVER_CONNECTION_TERMINATED, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_CONNECTION_TERMINATED") },
		{ CONNECTION_ERROR_PARSE_ERROR, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_ERROR_PARSING_HTTP") },
		{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_SSL_SUPPORT") },
		{ CONNECTION_ERROR_SECURITY_ERROR, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_INVALID_SECURID") },
		{ CONNECTION_ERROR_SUSPENDED_ACCOUNT, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_SUSPENDED_ACCOUNT") },
		{ CONNECTION_ERROR_TOO_MANY_ATTEMPTS, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_TOO_MANY_CONNECTION_ATTEMPTS") },
		{ CONNECTION_ERROR_INVALID_USERNAME, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_INVALID_USER_NAME") },
		{ CONNECTION_ERROR_UOL_SERVER_ERROR, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_UOL_SERVER_ERROR") },
		{ CONNECTION_ERROR_REGISTRATION_REQUIRED, _T("uol.resource.error.IDS_MESSAGE_PROTOCOL_REGISTRATION_REQUIRED") }
	};

	int nEntryCount = sizeof(pConnectionErrorEntries) / sizeof(pConnectionErrorEntries[0]);

	for (int nInd = 0; nInd < nEntryCount; nInd++)
	{
		m_mapConnectionErrorEntries.SetAt(pConnectionErrorEntries[nInd].error, pConnectionErrorEntries[nInd].strResource);
	}
}

BOOL CProtocolResourceTranslator::GetConnectionErrorTextFormat(ConnectionError error, CString& strTextFormat) const
{
	UOLLib::ISettings *pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	strTextFormat.Empty();

	const CAtlMap<ConnectionError, CString>::CPair* pPair	= m_mapConnectionErrorEntries.Lookup(error);

	if (NULL != pPair)
	{
		strTextFormat = pStringLoader->GetValue(pPair->m_value);
		ATLASSERT(!strTextFormat.IsEmpty() );
	}

	return !strTextFormat.IsEmpty();
}