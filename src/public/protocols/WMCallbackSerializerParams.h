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
 

#ifndef _WM_CALLBACK_SERIALIZER_PARAMS_H
#define _WM_CALLBACK_SERIALIZER_PARAMS_H


#include "filetransferinfo.h"
#include "notificationmessageparam.h"
#include "protocolsettings.h"


#include <string>

using namespace std;


struct WMS_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	void (CIMProtocolCallback::*pfnHandler)(const string&);
};


struct WMS_STR_CONNECTIONERROR_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	ConnectionError connectionError;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			ConnectionError, const string&);
};


struct WMS_STR_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&);
};


struct WMS_STR_CONTACTINFO_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	CContactInfo* pContactInfo;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const CContactInfo*, const string&);
};


struct WMS_STR_STR_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	string strParam3;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, const string&);
};


struct WMS_STR_STR_STR_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	string strParam3;
	string strParam4;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, const string&, const string&);
};


struct WMS_STR_STR_PCHAR_INT_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	char* pData;
	int intParam1;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, const char*, int);
};


struct WMS_STR_STR_IMPROTOCOLERROR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	IMProtocolError imProtocolError;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, IMProtocolError);
};


struct WMS_STR_STR_INT_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	int intParam1;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, int);
};


struct WMS_STR_STR_TYPINGSTATE_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	TypingState typingState;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, TypingState);
};


struct WMS_STR_NOTIFYREASON_STR_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	NotifyReason notifyReason;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			NotifyReason, const string&);
};


struct WMS_STR_STR_PFILETRANSFERINFO_PARAM
{
	CIMProtocolCallback* pCallback;
	string strParam1;
	string strParam2;
	CFileTransferInfo* pFileTransferInfo;
	void (CIMProtocolCallback::*pfnHandler)(const string&, 
			const string&, const CFileTransferInfo*);
};


struct WMS_CBK2_STR_STR_PPROTOCOLSETTINGS2_PARAM
{
	CIMProtocolCallback2* pCallback2;
	string strParam1;
	string strParam2;
	CProtocolSettings2* pProtocolSettings2;
	void (CIMProtocolCallback2::*pfnHandler)(const string&, 
			const string&, const CProtocolSettings2*);
};


struct WMS_CBK3_STR_NOTIFICATIONMESSAGEPARAM_PARAM
{
	CIMProtocolCallback3* pCallback3;
	string strParam1;
	CNotificationMessageParam* pNotificationMessageParam;
	void (CIMProtocolCallback3::*pfnHandler)(const string&, 
			const CNotificationMessageParam*);
};


struct WMS_CBK4_STR_STR_STR_PARAM
{
	CIMProtocolCallback4* pCallback4;
	string strParam1;
	string strParam2;
	string strParam3;
	void (CIMProtocolCallback4::*pfnHandler)(const string&, 
			const string&, const string&);
};


struct WMS_CBK5_STR_STR_STR_PUCHAR_INT_STR_PARAM
{
	CIMProtocolCallback5* pCallback5;
	string strParam1;
	string strParam2;
	string strParam3;
	string strParam4;
	unsigned char* pData;
	int intParam1;
	void (CIMProtocolCallback5::*pfnHandler)(const string&, 
			const string&, const string&, const unsigned char*, 
			unsigned int, const string&);
};


struct WMS_CBK5_STR_STR_STR_PARAM
{
	CIMProtocolCallback5* pCallback5;
	string strParam1;
	string strParam2;
	string strParam3;
	void (CIMProtocolCallback5::*pfnHandler)(const string&, 
			const string&, const string&);
};


struct WMS_CBK5_STR_STR_STR_STR_PARAM
{
	CIMProtocolCallback5* pCallback5;
	string strParam1;
	string strParam2;
	string strParam3;
	string strParam4;
	void (CIMProtocolCallback5::*pfnHandler)(const string&, 
			const string&, const string&, const string&);
};


struct WMS_CBK6_STR_INT_STR_STR_INT_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	string strParam3;
	int intParam1;
	int intParam2;
	void (CIMProtocolCallback6::*pfnHandler)(const string&, 
			int, const string&, const string&, int);
};


struct WMS_CBK6_STR_INT_STR_STR_STR_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	string strParam3;
	string strParam4;
	int intParam1;
	void (CIMProtocolCallback6::*pfnHandler)(const string&, 
			int, const string&, const string&, const string&);
};


struct WMS_CBK6_STR_INT_STR_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	int intParam1;
	void (CIMProtocolCallback6::*pfnHandler)(const string&, 
			int, const string&);
};


struct WMS_CBK6_STR_INT_STR_STR_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	string strParam3;
	int intParam1;
	void (CIMProtocolCallback6::*pfnHandler)(const string&, 
			int, const string&, const string&);
};


struct WMS_CBK6_STR_INT_STR_IMPROTOCOLERROR_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	int intParam1;
	IMProtocolError imProtocolError;
	void (CIMProtocolCallback6::*pfnHandler)(const string&, 
			int, const string&, IMProtocolError);
};


struct WMS_CBK6_STR_STR_REQUESTFILETRANSFERASYNC_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo;
	RequestResult (CIMProtocolCallback6::*pfnHandler)(const string&, 
			const string&, CRequestFileTransferAsyncInfo*);
};


struct WMS_CBK6_STR_STR_INT_PARAM
{
	CIMProtocolCallback6* pCallback6;
	string strParam1;
	string strParam2;
	int intParam1;
	void (CIMProtocolCallback6::*pfnHandler)(const string&, 
			const string&, int);
};


struct WMS_CBK7_STR_STR_INT_STR_PARAM
{
	CIMProtocolCallback7* pCallback7;
	string strParam1;
	string strParam2;
	int intParam1;
	string strParam3;
	void (CIMProtocolCallback7::*pfnHandler)(const string&, 
			const string&, int, const string&);
};


#endif // _WM_CALLBACK_SERIALIZER_PARAMS_H

