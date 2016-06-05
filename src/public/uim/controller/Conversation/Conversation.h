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

#include <interfaces/IUOLMessengerConversation.h>
#include <interfaces/IUOLMessengerContactList.h>

#include "../../model/Contact.h"

class CConversationObserver;


class CConversation : 
	public IUOLMessengerConversation2,
	public CUOLMessengerContactListObserver
{
public:
	CConversation(IUOLMessengerContactPtr pContact);
	CConversation(const int& nChatId, IUOLMessengerAccountPtr pAccount);
	virtual ~CConversation();
	
protected:
	virtual IUOLMessengerContactPtr GetContact();
	virtual IUOLMessengerAccountPtr GetAccount();
	virtual CString GetContactName();

	virtual void AddReceivedMessage(const CString& strText, const CTime& time);
	virtual void AddSentMessage(const CString& strText, const CTime& time);
	virtual void AddSystemMessage(const CString& strText, const CTime& time);

	virtual void GetMessages(CAtlList<IUOLMessengerConversation::MessageEntryPtr>& listMessages);

	virtual void EnableLogging(BOOL bEnable);
	virtual BOOL IsLoggingEnabled() const;

	virtual void SetTypingState(TypingState typingState);
	virtual TypingState GetTypingState() const;

	virtual void RegisterObserver(CUOLMessengerConversationObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerConversationObserver* pObserver);
	
	virtual void AddReceivedMessage(const CString& strText, const CTime& time, const CString& strSmiley);
	
	virtual IUOLMessengerConversation::ConversationType GetConversationType();
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
	
	virtual void AddRequestFileTransferAsync(CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);

	// CUOLMessengerContactListObserver interface
	virtual void OnAddContact(IUOLMessengerContactPtr pAddedContact);
    virtual void OnRemoveContact(IUOLMessengerContactPtr pRemovedContact) {};
    virtual void OnRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts) {};
    virtual void OnMoveContact(IUOLMessengerContactPtr pContact, const CString& strOldGroup) {};
    virtual void OnAddGroup(IUOLMessengerGroupPtr pGroup) {};
    virtual void OnRemoveGroup(IUOLMessengerGroupPtr pGroup) {};
    virtual void OnRemoveAllGroups() {};
	
private:
	void NotifyAll(void (CUOLMessengerConversationObserver::*pfnUpdate)(IUOLMessengerConversation*, IUOLMessengerConversation::MessageEntryPtr), IUOLMessengerConversation::MessageEntryPtr pMessageEntry);
    void NotifyAllChatEvents(void (CUOLMessengerConversationObserver2::*pfnUpdate)(IUOLMessengerConversation2*, IUOLMessengerContact3Ptr), IUOLMessengerContact3Ptr pChatContact);
	void NotifyAllSwitchContact(void (CUOLMessengerConversationObserver3::*pfnUpdate)(IUOLMessengerConversation*, IUOLMessengerContactPtr, IUOLMessengerContactPtr), IUOLMessengerContactPtr pRemovedContact, IUOLMessengerContactPtr pAddedContact);
	void AddMessageImpl(int	nMessageSender, const CString& strText, const CString& strAuthor, const CTime& time, const CString& strSmiley);

private:
	IUOLMessengerContactPtr		m_pContact;
	BOOL						m_bEnableLog;
	TypingState					m_typingState;
	
	/*
	typedef CAtlMap<CString, IUOLMessengerContactPtr>	CChatParticipantMap;
	CChatParticipantMap								m_chatParticipantMap;
	*/
	IUOLMessengerConversation::ConversationType		m_nConversationType;
	int                                             m_nChatId;
    CAtlList<CContactPtr>						    m_ChatContactList;
	IUOLMessengerAccountPtr							m_pAccount;

	CAtlList<IUOLMessengerConversation::MessageEntryPtr> m_listMessages;
	CAtlList<CUOLMessengerConversationObserver*> m_listObservers;

	CTime	m_timeStart;
};

MAKEAUTOPTR(CConversation);