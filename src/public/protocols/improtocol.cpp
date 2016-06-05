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
 
#include "improtocol.h"
#include "critseclock.h"

#include <stdlib.h>
#include <string.h>

using namespace std;


CIMProtocol::CIMProtocol() : 	
	m_pNext(NULL)
{
}


CIMProtocol::~CIMProtocol()
{
}


void CIMProtocol::SetNextProtocol(CIMProtocol* pProtocol)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(m_csProtocol);

	m_pNext = pProtocol;
}


CIMProtocol* CIMProtocol::GetNextProtocol()
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(m_csProtocol);

	return m_pNext;
}

unsigned CIMProtocol::GetProtocolPos()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetProtocolPos();
	}

	return 0;
}

void CIMProtocol::Init()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		pNext->Init();
	}
}

void CIMProtocol::Finalize()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		pNext->Finalize();
	}
}

std::string CIMProtocol::GetId()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetId();
	}

	return "prpl-none";
}

std::string CIMProtocol::GetName()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetName();
	}

	return "none";
}

void CIMProtocol::SetCallback(const std::string& strUser, CIMProtocolCallback* pCallback)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		pNext->SetCallback(strUser, pCallback);
	}
}

bool CIMProtocol::HasUserDisplayIconSupport()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->HasUserDisplayIconSupport();
	}

	return false;
}

bool CIMProtocol::HasContactBlockingSupport()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->HasContactBlockingSupport();
	}

	return false;
}

bool CIMProtocol::HasFileTransferSupport()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->HasFileTransferSupport();
	}

	return false;
}


bool CIMProtocol::HasContactServerAliasSupport()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->HasContactServerAliasSupport();
	}

	return false;
}


bool CIMProtocol::HasReRequestAuthorizationSupport()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->HasReRequestAuthorizationSupport();
	}

	return false;
}


int CIMProtocol::GetPrivacyFlags()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetPrivacyFlags();
	}

	return 0;
}


bool CIMProtocol::AutoAddContactOnAuthorization()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->AutoAddContactOnAuthorization();
	}

	return false;
}

std::string CIMProtocol::GetDefaultContactGroup()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetDefaultContactGroup();
	}

	return string("");
}

bool CIMProtocol::IsValidUserName(const std::string& strUser)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->IsValidUserName(strUser);
	}

	return false;
}

IMProtocolError CIMProtocol::Connect(const std::string& strUser, const std::string& strPass, CProtocolSettings* pExtraSettings, CProxyInfo* pProxyInfo)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->Connect(strUser, strPass, pExtraSettings, pProxyInfo);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::Disconnect(const std::string& strUser)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->Disconnect(strUser);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

int CIMProtocol::IsConnected(const std::string& strUser)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->IsConnected(strUser);
	}

	return FALSE;
}

IMProtocolError CIMProtocol::ChangePassword(const std::string& strUser, const std::string& strOldPassword, const std::string& strNewPassword)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->ChangePassword(strUser, strOldPassword, strNewPassword);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::AddContact(const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->AddContact(strUser, strContact, strGroup);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::RemoveContact(const std::string& strUser, const std::string& strContact, const std::string& strGroup)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->RemoveContact(strUser, strContact, strGroup);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::AliasContact(const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->AliasContact(strUser, strContact, strAlias);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::MoveContact(const std::string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->MoveContact(strUser, strContact, strOldGroup, strNewGroup);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::GetContactInfo(const std::string& strUser, const std::string& strContact)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetContactInfo(strUser, strContact);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::CanRequestContactAuthorization(const std::string& strUser, const std::string& strContact, bool& bCanRequest)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->CanRequestContactAuthorization(strUser, strContact, bCanRequest);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::RequestContactAuthorization(const std::string& strUser, const std::string& strContact)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->RequestContactAuthorization(strUser, strContact);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol::RenameGroup(const std::string& strUser, const std::string& strOldName, const std::string& strNewName)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->RenameGroup(strUser, strOldName, strNewName);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::RemoveGroup(const std::string& strUser, const std::string& strGroup)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->RemoveGroup(strUser, strGroup);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SetAlias(const std::string& strUser, const std::string& strAlias)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SetAlias(strUser, strAlias);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

void CIMProtocol::GetIconSpecification(CIconSpec& iconSpec)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		pNext->GetIconSpecification(iconSpec);
	}
}

IMProtocolError CIMProtocol::SetUserDisplayIcon(const std::string& strUser, const std::string& strFile)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SetUserDisplayIcon(strUser, strFile);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SetUserStatus(const std::string& strUser, int nStatus)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SetUserStatus(strUser, nStatus);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SendIM(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SendIM(strFromUser, strToUser, strMessage);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SendTyping(const std::string& strFromUser, const std::string& strToUser, TypingState state)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SendTyping(strFromUser, strToUser, state);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SendConversationWindowClosed(const std::string& strFromUser, const std::string& strToUser)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SendConversationWindowClosed(strFromUser, strToUser);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SetIdle(const std::string& strUser, int nSeconds)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SetIdle(strUser, nSeconds);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::GetDenyContactsList(const std::string& strUser, CPrivacyList& listContacts)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetDenyContactsList(strUser, listContacts);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::GetPermitContactsList(const std::string& strUser, CPrivacyList& listContacts)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetPermitContactsList(strUser, listContacts);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::AddDenyContact(const std::string& strUser, const std::string& strContact)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->AddDenyContact(strUser, strContact);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::AddPermitContact(const std::string& strUser, const std::string& strContact)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->AddPermitContact(strUser, strContact);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::RemoveDenyContact(const std::string& strUser, const std::string& strContact)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->RemoveDenyContact(strUser, strContact);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::RemovePermitContact(const std::string& strUser, const std::string& strContact)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->RemovePermitContact(strUser, strContact);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::SendFile(const std::string& strFromUser, const std::string& strToUser, const std::string& strFilePath)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->SendFile(strFromUser, strToUser, strFilePath);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol::CancelFileTransfer(const std::string& strUser, long lFileTransferId)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->CancelFileTransfer(strUser, lFileTransferId);
	}

	return IM_PROTOCOL_ERROR_FAILED;
}




//////////////////////////////////////////////////////
// CIMProtocol2
//

CIMProtocol2::CIMProtocol2()
{
}


CIMProtocol2::~CIMProtocol2()
{
}


IMProtocolError CIMProtocol2::Connect(const std::string& strUser, const std::string& strPass, CProtocolSettings* pExtraSettings, CProxyInfo* pProxyInfo, CProtocolSettings2* pExtendedCapabilities)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		CIMProtocol2* pNext2 = dynamic_cast<CIMProtocol2*>(pNext);

		if (pNext2)
		{
			return pNext2->Connect(strUser, strPass, pExtraSettings, pProxyInfo, pExtendedCapabilities);
		}
	}

	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol2::SetExtendedCapabilities(const std::string& strUser, CProtocolSettings2* pExtendedCapabilities)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		CIMProtocol2* pNext2 = dynamic_cast<CIMProtocol2*>(pNext);

		if (pNext2)
		{
			return pNext2->SetExtendedCapabilities(strUser, pExtendedCapabilities);
		}
	}

	return IM_PROTOCOL_ERROR_FAILED;
}




//////////////////////////////////////////////////////
// CIMProtocol3
//

CIMProtocol3::CIMProtocol3()
{
}


CIMProtocol3::~CIMProtocol3()
{
}


bool CIMProtocol3::HasContactListSupport()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		CIMProtocol3* pNext3 = dynamic_cast<CIMProtocol3*>(pNext);

		if (pNext3)
		{
			return pNext3->HasContactListSupport();
		}
	}

	return false;
}




//////////////////////////////////////////////////////
// CIMProtocol4
//

CIMProtocol4::CIMProtocol4()
{
}


CIMProtocol4::~CIMProtocol4()
{
}


bool CIMProtocol4::HasCommandSupport(const std::string& strCmdId)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->HasCommandSupport(strCmdId);
		}
	}
	
	return false;
}


IMProtocolError CIMProtocol4::SendCommand(const std::string& strCmdId, const std::string& strCmdParameters, const std::string& strFromUser, const std::string& strToUser)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->SendCommand(strCmdId, strCmdParameters, strFromUser, strToUser);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


bool CIMProtocol4::FreeToSendCommand(const std::string& strCmdId, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->FreeToSendCommand(strCmdId, strFromUser, strToUser, blockError);
		}
	}
	
	return false;
}


std::string CIMProtocol4::GetProtocolCommandName(const std::string& strCmdId)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->GetProtocolCommandName(strCmdId);
		}
	}
	
	return "";
}


/*
bool CIMProtocol4::HasSendAttentionSupport(int nAttentionType)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->HasSendAttentionSupport(nAttentionType);
		}
	}
	
	return false;
}


IMProtocolError CIMProtocol4::SendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->SendAttention(nAttentionType, strFromUser, strToUser);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


bool CIMProtocol4::FreeToSendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol4* pNext4 = dynamic_cast<CIMProtocol4*>(pNext);
		
		if (pNext4)
		{
			return pNext4->FreeToSendAttention(nAttentionType, strFromUser, strToUser, blockError);
		}
	}
	
	return false;
}
*/




//////////////////////////////////////////////////////
// CIMProtocol5
//

CIMProtocol5::CIMProtocol5()
{
}


CIMProtocol5::~CIMProtocol5()
{
}


bool CIMProtocol5::HasChatSupport()
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->HasChatSupport();
		}
	}
	
	return false;
}


std::string CIMProtocol5::GetChatInviteMenuName()
{
    CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->GetChatInviteMenuName();
		}
	}
	
    return "";
}


IMProtocolError CIMProtocol5::ChatInvite(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage, int& nChatId, bool& bNewChatInvite, const std::string& strChatInviteMenuName)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->ChatInvite(strFromUser, strToUser, strMessage, nChatId, bNewChatInvite, strChatInviteMenuName);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::ChatLeave(const std::string& strFromUser, const int& nChatId)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->ChatLeave(strFromUser, nChatId);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::ChatSend(const std::string& strFromUser, const std::string& strMessage, const int& nChatId)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->ChatSend(strFromUser, strMessage, nChatId);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


bool CIMProtocol5::HasCustomEmoticonSupport()
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->HasCustomEmoticonSupport();
		}
	}
	
	return false;
}


/*
IMProtocolError CIMProtocol5::AddCustomEmoticonToAccount(const std::string& strFromUser, const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->AddCustomEmoticonToAccount(strFromUser, strCustomEmoticonShortcut, strCustomEmoticonFileName);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::RemoveCustomEmoticonFromAccount(const std::string& strFromUser, const std::string& strCustomEmoticonShortcut)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->RemoveCustomEmoticonFromAccount(strFromUser, strCustomEmoticonShortcut);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}
*/

IMProtocolError CIMProtocol5::AddCustomEmoticon(const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->AddCustomEmoticon(strCustomEmoticonShortcut, strCustomEmoticonFileName);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::RemoveCustomEmoticon(const std::string& strCustomEmoticonShortcut)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->RemoveCustomEmoticon(strCustomEmoticonShortcut);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::ChangeCustomEmoticonShortcut(const std::string& strCustomEmoticonOldShortcut, const std::string& strCustomEmoticonNewShortcut)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->ChangeCustomEmoticonShortcut(strCustomEmoticonOldShortcut, strCustomEmoticonNewShortcut);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::AcceptFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->AcceptFileAsync(strUser, pFileTransferAsyncInfo);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::RejectFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->RejectFileAsync(strUser, pFileTransferAsyncInfo);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::DisconnectAsync(const std::string& strUser)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->DisconnectAsync(strUser);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol5::SetAliasAsync(const std::string& strUser, const std::string& strAlias)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->SetAliasAsync(strUser, strAlias);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}

IMProtocolError CIMProtocol5::SetUserDisplayIconAsync(const std::string& strUser, const std::string& strFile)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->SetUserDisplayIconAsync(strUser, strFile);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::SetUserStatusAsync(const std::string& strUser, int nStatus)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->SetUserStatusAsync(strUser, nStatus);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol5::AliasContactAsync(const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->AliasContactAsync(strUser, strContact, strAlias);
		}
	}

	return IM_PROTOCOL_ERROR_FAILED;
}


bool CIMProtocol5::HasUserAliasSupport()
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol5* pNext5 = dynamic_cast<CIMProtocol5*>(pNext);
		
		if (pNext5)
		{
			return pNext5->HasUserAliasSupport();
		}
	}
	
	return false;
}




//////////////////////////////////////////////////////
// CIMProtocol6
//

CIMProtocol6::CIMProtocol6()
{
}


CIMProtocol6::~CIMProtocol6()
{
}


IMProtocolError CIMProtocol6::SetUserStatus(const std::string& strUser, int nStatus, const std::string& strStatusMessage)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		CIMProtocol6* pNext6 = dynamic_cast<CIMProtocol6*>(pNext);

		if (pNext6)
		{
			return pNext6->SetUserStatus(strUser, nStatus, strStatusMessage);
		}
	}

	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CIMProtocol6::SetUserStatusAsync(const std::string& strUser, int nStatus, const std::string& strStatusMessage)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol6* pNext6 = dynamic_cast<CIMProtocol6*>(pNext);
		
		if (pNext6)
		{
			return pNext6->SetUserStatusAsync(strUser, nStatus, strStatusMessage);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}


bool CIMProtocol6::HasOfflineMessageSupport()
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol6* pNext6 = dynamic_cast<CIMProtocol6*>(pNext);
		
		if (pNext6)
		{
			return pNext6->HasOfflineMessageSupport();
		}
	}
	
	return false;
}




//////////////////////////////////////////////////////
// CIMProtocol7
//

CIMProtocol7::CIMProtocol7()
{
}


CIMProtocol7::~CIMProtocol7()
{
}


IMProtocolError CIMProtocol7::SetExtendedCapabilitiesAsync(const std::string& strUser, CProtocolSettings2* pExtendedCapabilities)
{
	CIMProtocol* pNext = GetNextProtocol();
	
	if (pNext)
	{
		CIMProtocol7* pNext7 = dynamic_cast<CIMProtocol7*>(pNext);
		
		if (pNext7)
		{
			return pNext7->SetExtendedCapabilitiesAsync(strUser, pExtendedCapabilities);
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}

