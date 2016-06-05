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

#include <interfaces/IUOLMessengerFacade.h>
#include <interfaces/IUOLMessengerConversationManager.h>

#include "Conversation.h"
#include "ConversationManagerObserverNotifier.h"
#include "../UOLMessengerContactTraits.h"
#include "../CustomEmoticon/CustomEmoticonManager.h"
#include "../../model/FileTransfer.h"


class CConversationManager : 
	public IUOLMessengerConversationManager2
{
public:
	CConversationManager();
	virtual ~CConversationManager();

	virtual BOOL Init(IUOLMessengerFacade* pFacade);
	virtual void Finalize();

	void NotifyIMReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText);
	void NotifyIMSent(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText);
	void NotifyIMSendError(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError error);
	void NotifyContactTyping(IUOLMessengerAccountPtr pAccount, const CString& strContact, TypingState state);
	void NotifyConversationWindowClosed(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyConversationTimeout(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void NotifyNudgeReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText);
	
	// Custom Smiley (Emoticons)
	CustomEmoticonAddResult ConfirmCustomSmileyAdd(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strSmiley, const BOOL bRemote);
	void NotifyCustomSmileyWrite(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const CString& strFileExtension);
	void NotifyCustomSmileyClose(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strSmiley);
	void NotifyIMReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText, const CString& strSmileyList);
	
	IUOLMessengerCustomEmoticonManagerPtr GetCustomEmoticonManager();
	
	// Chat or Conference
	void NotifyChatAddUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact);
	void NotifyChatRenameUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactOldName, const CString& strChatContactNewName, const CString& strChatContactNewAlias);
	void NotifyChatRemoveUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName);
	void NotifyChatUpdateUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact);
    void NotifyChatReceived(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strText, const CString& strSmileyList);
	void NotifyChatSent(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strText);
	void NotifyChatSendError(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, IMProtocolError error);
	
	// Assyncronous File Transfer.
	RequestResult NotifyRequestFileTransferAsync(IUOLMessengerAccountPtr pAccount, const CString& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	void NotifyFileTransferStatus(CFileTransferPtr pFileTransfer);
	void NotifyFileTransferError(IUOLMessengerAccountPtr pAccount, const CString& strContact, int nError);	
    
	virtual BOOL IsChatParticipant(IUOLMessengerContactPtr pContact, const CString& strChatKey);
    virtual void SetChatInvitedContact(const CString& strChatContactName);
    virtual CString GetChatInvitedContact();
	
protected:
	virtual BOOL SendIM(IUOLMessengerConversationPtr pConversation, const CString& strMessage);
	virtual BOOL SendTyping(IUOLMessengerConversationPtr pConversation);
	virtual BOOL SendConversationWindowClosed(IUOLMessengerConversationPtr pConversation);

	virtual IUOLMessengerConversationPtr CreateConversation(IUOLMessengerContactPtr pContact);
	virtual IUOLMessengerConversation2Ptr CreateConversation(const int nChatId, IUOLMessengerAccountPtr pAccount);
    virtual void DestroyConversation(IUOLMessengerContactPtr pContact);
    virtual void DestroyConversation(const CString& strChatKey);
	virtual IUOLMessengerConversationPtr FindConversation(IUOLMessengerContactPtr pContact);
    virtual IUOLMessengerConversation2Ptr FindConversation(const CString& strChatKey);
	virtual void GetConversationList(CAtlList<IUOLMessengerConversationPtr>& listConversations);	

    virtual BOOL ChatSend(IUOLMessengerConversation2Ptr pConversation, const CString& strMessage);
    
	virtual BOOL AcceptFileAsync(IUOLMessengerConversation2Ptr pConversation, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	virtual BOOL RejectFileAsync(IUOLMessengerConversation2Ptr pConversation, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);
	
	virtual void RegisterObserver(CUOLMessengerConversationManagerObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerConversationManagerObserver* pObserver);

private:
	CConversationPtr FindConversation(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	CString	GetCustomEmoticonRootPath();
    CString GetChatKey(const int nChatId, IUOLMessengerAccountPtr pAccount);

private:
	IUOLMessengerFacade*	m_pFacade;

	typedef CAtlMap<IUOLMessengerContactPtr, CConversationPtr, CUOLMessengerContactTraits> CConversationMap;
    typedef CAtlMap<CString, CConversationPtr> CChatConversationMap;
	CConversationMap	    m_mapConversations;
    CChatConversationMap	m_mapChatConversations;

	CConversationManagerObserverNotifier	m_observerNotifier;
	
	CCustomEmoticonManagerPtr				m_pCustomEmoticonManager;
    CString                                 m_strChatInvitedContact;
};

MAKEAUTOPTR(CConversationManager);