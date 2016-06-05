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
 
#ifndef _PURPLE_SERVICE_CALLBACK_H_
#define _PURPLE_SERVICE_CALLBACK_H_

#include <glib.h>
#include <glib-object.h>

#include <string>
#include <vector>

// Purple headers
#include <account.h>
#include <connection.h>
#include <notify.h>
#include <ft.h>


struct MessageParam
{
	std::string strFrom;
	std::string strMessage;
	PurpleConversationType conversationType;
	int iChatId;
};


struct NotifyMessageParam
{
	PurpleConnection* pPurpleConn;
	PurpleNotifyMsgType type;
	std::string strTitle;
	std::string strPrimary;
	std::string strSecondary;
};


struct NotifyUserInfoParam
{
	PurpleConnection* pPurpleConn;
	std::string strWho;
	std::string strUserInfo;
};


struct NotifyFormattedParam
{
	//void* pHandle;
	PurpleConnection* pPurpleConn;
	std::string strTitle;
	std::string strPrimary;
	std::string strSecondary;
	std::string strText;
	//GCallback callback;
	//void* pUserData;
};


struct RequestInputParam
{
	PurpleAccount* pPurpleAccount;
	std::string strTitle;
	std::string strPrimary;
	std::string strSecondary;
	std::string strDefaultValue;
	std::string strWho;
	PurpleConversation* pConversation;
	//int bMultiline;
	//int bMasked;
	//std::string strHint;
	//std::string strOkText;
	GCallback okCallback;
	//std::string strCancelText;
	GCallback cancelCallback;
	void* pUserData;
};

struct RequestActionParam
{
	PurpleAccount* pPurpleAccount;
	PurpleConnection *pPurpleConnection;
	std::string strTitle;
	std::string strPrimary;
	std::string strSecondary;
	unsigned int nDefaultAction;
	std::string strWho;
	PurpleConversation* pConversation;
	void* pUserData;
	std::vector<std::string> vActionLabel;
	std::vector<GCallback> vActionCallback;
};

struct RequestFileParam
{
	PurpleAccount* pPurpleAccount;
	PurpleConnection *pPurpleConnection;
	std::string strTitle;
	std::string strFileName;
	int bSaveDialog;
	GCallback okCallback;
	GCallback cancelCallback;
	std::string strWho;
	PurpleConversation* pConversation;
	void* pUserData;
};

struct AccountRequestAuthorizeParam
{
	PurpleAccount*							pPurpleAccount;
	std::string								strRemoteUser;
	std::string								strId;
	std::string								strAlias;
	std::string								strMessage;
	int										bOnList;
	PurpleAccountRequestAuthorizationCb		pAuthorizeCallback;
	PurpleAccountRequestAuthorizationCb		pDenyCallback;
	void*									pUserData;
	int										bRequestIsCanceled;
};


class CPurpleServiceCallback
{
public:
	virtual ~CPurpleServiceCallback()
	{
	}

	virtual int ProcessSystemMessage(PurpleAccount* pAccount, MessageParam* pParam) = 0;
	virtual int ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam) = 0;
	
	virtual int ProcessAccountRequestAuthorize(AccountRequestAuthorizeParam* pParam) = 0;
	virtual void ProcessAccountRequestAuthorizeCancel(AccountRequestAuthorizeParam* pParam) = 0;
	
	virtual int ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam) = 0;
	virtual int ProcessNotifyUserInfo(PurpleAccount* pAccount, NotifyUserInfoParam* pParam) = 0;

	virtual int ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam) = 0;
	virtual int ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam) = 0;
	virtual int ProcessRequestFileTransferAction(PurpleXfer* pXfer, RequestActionParam* pParam) = 0;
	virtual int ProcessRequestFile(PurpleXfer* pXfer, RequestFileParam* pParam) = 0;
};

#endif