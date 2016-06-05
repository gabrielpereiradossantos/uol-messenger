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
#include <commands/NudgeContactCommand.h>

#include "../../resource.h"
#include "../UIMApplication.h"


CNudgeContactCommand::CNudgeContactCommand(IUOLMessengerConversationPtr pConversation) :
		m_pConversation(pConversation)
{
}


CNudgeContactCommand::~CNudgeContactCommand()
{
}


void CNudgeContactCommand::Execute()
{
	ATLASSERT(m_pConversation);
	ATLASSERT(m_pConversation->GetAccount());
	ATLASSERT(m_pConversation->GetContact());
	
	CIMProtocol* pProtocol = m_pConversation->GetAccount()->GetProtocol();
	CIMProtocol4* pProtocol4 = NULL;
	
	if (pProtocol)
	{
		pProtocol4 = dynamic_cast<CIMProtocol4*>(pProtocol);
	}
	
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	if (pProtocol4 && pProtocol4->HasCommandSupport(NUDGE_COMMAND_ID))
	{
		IMProtocolError imError = pProtocol4->SendCommand(
				NUDGE_COMMAND_ID, "", 
				(LPCTSTR) m_pConversation->GetAccount()->GetUserId(), 
				(LPCTSTR) m_pConversation->GetContact()->GetName());
		
		if (imError == IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED)
		{
			CString offLineAccountWarning;
			
			offLineAccountWarning = pStringLoader->GetValue("uol.resource.IDS_NUDGE_ERROR_ACCOUNT_OFF_LINE");
			
			m_pConversation->AddSystemMessage(offLineAccountWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_CONTACT_NOT_CONNECTED)
		{
			CString offLineContactWarning;
			
			offLineContactWarning = pStringLoader->GetValue("uol.resource.IDS_NUDGE_ERROR_CONTACT_OFF_LINE");
			
			m_pConversation->AddSystemMessage(offLineContactWarning, CTime::GetCurrentTime());
		}
		else if (imError == IM_PROTOCOL_ERROR_WAIT_COMMAND_COOLDOWN)
		{
			CString waitCommandCooldownWarning;
			
			waitCommandCooldownWarning = pStringLoader->GetValue("uol.resource.IDS_NUDGE_WAIT_COMMAND_COOLDOWN");
			
			m_pConversation->AddSystemMessage(waitCommandCooldownWarning, CTime::GetCurrentTime());
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
		else if (imError == IM_PROTOCOL_ERROR_SUCCESS)
		{
			CString nudgesSuccessfullySentWarning;
			
			nudgesSuccessfullySentWarning = pStringLoader->GetValue("uol.resource.IDS_NUDGE_SENDING_MESSAGE");
			
			m_pConversation->AddSystemMessage(nudgesSuccessfullySentWarning, CTime::GetCurrentTime());
		}
	}
	/*
	else
	{
		CString strFormat;
		strFormat = pStringLoader->GetValue("uol.resource.error.IDS_BLOCKING_NOT_SUPPORTED");
		strMessage.Format(strFormat, m_strContactName, m_pAccount->GetProtocolName());
		
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_wnd, strMessage, MB_OK | MB_ICONSTOP);
	}
	*/
}

