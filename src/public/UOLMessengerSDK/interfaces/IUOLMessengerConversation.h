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

#include "IUOLMessengerAccount.h"
#include "IUOLMessengerContact.h"
#include "IUOLMessengerUserIcon.h"


class CUOLMessengerConversationObserver;


class  __declspec(novtable) IUOLMessengerConversation
{
public:
	enum MessageSender
	{
		MT_ACCOUNT_USER = 0,
		MT_CONTACT,
		MT_SYSTEM,
		MT_FILE_TRANSFER
	};

	struct MessageEntry
	{
		CTime	time;
		int		nMessageSender;
		CString strText;
		CString strAuthor;
		CString strSmiley;
	};

	MAKEAUTOPTR(MessageEntry);

	enum ConversationType
	{
		CT_IM_CONVERSATION = 0,
		CT_CHAT_CONVERSATION
	};

public:
	virtual ~IUOLMessengerConversation() {}

	virtual IUOLMessengerContactPtr GetContact() = 0;
	virtual IUOLMessengerAccountPtr GetAccount() = 0;
	virtual CString GetContactName() = 0;

	virtual void AddReceivedMessage(const CString& strText, const CTime& time) = 0;
	virtual void AddSentMessage(const CString& strText, const CTime& time) = 0;
	virtual void AddSystemMessage(const CString& strText, const CTime& time) = 0;

	virtual void GetMessages(CAtlList<IUOLMessengerConversation::MessageEntryPtr>& listMessages) = 0;

	virtual void EnableLogging(BOOL bEnable) = 0;
	virtual BOOL IsLoggingEnabled() const = 0;

	virtual void SetTypingState(TypingState typingState) = 0;
	virtual TypingState GetTypingState() const = 0;

	virtual void RegisterObserver(CUOLMessengerConversationObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerConversationObserver* pObserver) = 0;
}; 

MAKEAUTOPTR(IUOLMessengerConversation);


class  __declspec(novtable) IUOLMessengerConversation2 : public IUOLMessengerConversation
{
public:
	virtual ~IUOLMessengerConversation2() {}
	
	// Custom smileys (emoticons)
	virtual void AddReceivedMessage(const CString& strText, const CTime& time, const CString& strSmiley) = 0;
	
	// Chat or Conference
	virtual ConversationType GetConversationType();
    virtual int GetChatId();
	virtual CString GetChatKey();
    virtual CString GetChatTitle();
	virtual void GetChatParticipantList(CAtlList<IUOLMessengerContact3Ptr>& chatParticipantList);
	virtual void AddReceivedMessage(const CString& strFromUser, const CString& strText, const CTime& time, const CString& strSmiley);
	virtual BOOL AddChatParticipant(const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact);
	virtual BOOL RenameChatParticipant(const CString& strChatContactOldName, const CString& strChatContactNewName, const CString& strChatContactNewAlias);
	virtual BOOL UpdateChatParticipant(const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact);
	virtual BOOL RemoveChatParticipant(const CString& strChatContactName);
    virtual IUOLMessengerContact3Ptr FindChatParticipant(const CString& strChatContactName);
    virtual int GetChatParticipantCount();
	
	// Assyncronous File Transfer.
	virtual void AddRequestFileTransferAsync(CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
};

MAKEAUTOPTR(IUOLMessengerConversation2);



class CUOLMessengerConversationObserver
{
public:
	virtual ~CUOLMessengerConversationObserver() {}

	virtual void OnLoggingEnabledChanged(IUOLMessengerConversation* pConversation) {}
	virtual void OnTypingStateChanged(IUOLMessengerConversation* pConversation) {}
	virtual void OnAddMessage(IUOLMessengerConversation* pConversation, IUOLMessengerConversation::MessageEntryPtr pMessageEntry) {}
};


class CUOLMessengerConversationObserver2 : public CUOLMessengerConversationObserver
{
public:
	virtual ~CUOLMessengerConversationObserver2() {}
	
	virtual void OnAddChatParticipant(IUOLMessengerConversation2* pConversation, IUOLMessengerContact3Ptr pChatContact) {}
	virtual void OnRemoveChatParticipant(IUOLMessengerConversation2* pConversation, IUOLMessengerContact3Ptr pChatContact) {}
	virtual void OnAddRequestFileTransferAsync(IUOLMessengerConversation2* pConversation, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo) {}
};


class CUOLMessengerConversationObserver3 : public CUOLMessengerConversationObserver2
{
public:
	virtual ~CUOLMessengerConversationObserver3() {}
	
	virtual void OnSwitchContact(IUOLMessengerConversation* pConversation, IUOLMessengerContactPtr pRemovedContact, IUOLMessengerContactPtr pAddedContact) {}
};

