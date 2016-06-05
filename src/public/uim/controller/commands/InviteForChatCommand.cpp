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
 
#include "StdAfx.h"
#include <commands/InviteForChatCommand.h>

#include "../UIMApplication.h"
#include "../../resource.h"


CInviteForChatCommand::CInviteForChatCommand(IUOLMessengerConversationPtr pConversation, 
                                             IUOLMessengerContactPtr pContact,
                                             BOOL bNewChatInvite) :
    m_pConversation(pConversation),
    m_pContact(pContact),
    m_bNewChatInvite(bNewChatInvite)
{
}


CInviteForChatCommand::~CInviteForChatCommand(void)
{
}


void CInviteForChatCommand::Execute()
{
	ATLASSERT(m_pConversation);
	ATLASSERT(m_pConversation->GetAccount());
	ATLASSERT(m_pContact);
	
	CIMProtocol* pProtocol = m_pConversation->GetAccount()->GetProtocol();
	CIMProtocol5* pProtocol5 = NULL;
	
	if (pProtocol)
	{
		pProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocol);
	}
	
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	if (pProtocol5 && pProtocol5->HasChatSupport())
	{
		int nChatId = 0;

        // Get ChatId
        if (!m_bNewChatInvite)
        {
            IUOLMessengerConversation2Ptr pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(m_pConversation);
            if (pConversation2)
            {
                nChatId = pConversation2->GetChatId();
            }
        }
        
        bool bNewChatInvite = m_bNewChatInvite ? true : false;

        IMProtocolError imError = pProtocol5->ChatInvite(
				(LPCTSTR) m_pConversation->GetAccount()->GetUserId(), 
				(LPCTSTR) m_pContact->GetName(), "", nChatId, 
                bNewChatInvite, pProtocol5->GetChatInviteMenuName());
		
		if (imError == IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED)
		{
			CString offLineAccountWarning;
			
			offLineAccountWarning = pStringLoader->GetValue("uol.resource.IDS_CHAT_ERROR_ACCOUNT_OFF_LINE");
			
			m_pConversation->AddSystemMessage(offLineAccountWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_CONTACT_NOT_CONNECTED)
		{
			CString offLineContactWarning;
			
			offLineContactWarning = pStringLoader->GetValue("uol.resource.IDS_CHAT_ERROR_CONTACT_OFF_LINE");
			
			m_pConversation->AddSystemMessage(offLineContactWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_NOT_SUPPORTED)
		{
			// Considering the right practices of implementation (like check the support for the 
			// command first), this should not happen.
			
			CString commandNotSupportedWarning;
			
			commandNotSupportedWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_NOT_SUPPORTED");
			
			m_pConversation->AddSystemMessage(commandNotSupportedWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_NOT_FOUND)
		{
			CString commandNotFoundWarning;
			
			commandNotFoundWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_NOT_FOUND");
			
			m_pConversation->AddSystemMessage(commandNotFoundWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_WRONG_ARGS)
		{
			CString commandWrongArgsWarning;
			
			commandWrongArgsWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_WRONG_ARGS");
			
			m_pConversation->AddSystemMessage(commandWrongArgsWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_FAILED)
		{
			CString commandFailedWarning;
			
			commandFailedWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_FAILED");
			
			m_pConversation->AddSystemMessage(commandFailedWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_ONLY_CHAT_ALLOWED)
		{
			CString commandOnlyChatAllowedWarning;
			
			commandOnlyChatAllowedWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_ONLY_CHAT_ALLOWED");
			
			m_pConversation->AddSystemMessage(commandOnlyChatAllowedWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_ONLY_IM_ALLOWED)
		{
			CString commandOnlyIMAllowedWarning;
			
			commandOnlyIMAllowedWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_ONLY_IM_ALLOWED");
			
			m_pConversation->AddSystemMessage(commandOnlyIMAllowedWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_COMMAND_WRONG_PRPL)
		{
			CString commandWrongPrplWarning;
			
			commandWrongPrplWarning = pStringLoader->GetValue("uol.resource.IDS_ERROR_COMMAND_WRONG_PRPL");
			
			m_pConversation->AddSystemMessage(commandWrongPrplWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_FAILED)
		{
			CString strMessage;
			
			strMessage = pStringLoader->GetValue("uol.resource.IDS_CHAT_STARTING_ERROR_MSG");
			
			m_pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_SUCCESS)
		{
			CString strMessage;
			
			if (bNewChatInvite)
			{
				strMessage = pStringLoader->GetValue("uol.resource.IDS_CHAT_STARTING_MSG");
			}
			else
			{
				CString strFormat = pStringLoader->GetValue("uol.resource.IDS_CHAT_INVITING_MSG");
				strMessage.Format(strFormat, m_pContact->GetAlias(), m_pContact->GetName());
			}
			
			m_pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
		}
	}
}
