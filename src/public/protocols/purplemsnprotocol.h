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
 
#ifndef _PURPLE_MSN_PROTOCOL_H_
#define _PURPLE_MSN_PROTOCOL_H_

#include "purpleprotocol.h"


class CPurpleMSNProtocol : public CPurpleProtocol
{
public:
	CPurpleMSNProtocol();
	virtual ~CPurpleMSNProtocol();

	bool HasContactServerAliasSupport();

	int GetPrivacyFlags();

	bool IsValidUserName(const std::string& strUser);

	int ProcessSystemMessage(PurpleAccount* pAccount, MessageParam* pParam);
	int ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam);

	int ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam);
	int ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam);
	int ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam);

	virtual bool HasCommandSupport(const std::string& strCmdId);
	virtual bool FreeToSendCommand(const std::string& strCmdId, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError);
	virtual std::string GetProtocolCommandName(const std::string& strCmdId);
	
	virtual bool HasChatSupport();
    virtual std::string GetChatInviteMenuName();
	virtual bool HasCustomEmoticonSupport();

	bool HasUserAliasSupport();

protected:
	virtual bool GetStatusText(int status, std::string& strStatusText);
	virtual bool GetStatusCode(const std::string& statusText, int& nStatus);

	virtual std::string FormatSendingMessage(const std::string& strMessage);
	virtual std::string FormatSentMessage(const std::string& strMessage);
	//virtual std::string FormatReceivedMessage(const std::string& strMessage);

	virtual bool GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason);

public:
	static const char* PURPLE_MSN_PLUGIN_ID;
	static const char* PURPLE_MSN_PROTOCOL_NAME;

private:
	struct MSNStatus
	{
		int status;
		char* statusText;
	};

	static MSNStatus ms_status[];

	struct ConnectionErrorEntry
	{
		ConnectionError errorCode;
		char* pszText;
	};

	static ConnectionErrorEntry ms_pConnectionErrorEntries[];
	
	typedef std::map<std::string, time_t> CLastNudgeSendingTimeMap;
	typedef std::map<std::string, time_t> CLastNudgeReceivingTimeMap;
	
	CLastNudgeSendingTimeMap	m_lastNudgeSendingTimeMap;
	CLastNudgeReceivingTimeMap	m_lastNudgeReceivingTimeMap;
};

#endif
