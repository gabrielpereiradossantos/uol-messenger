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
#include "Conversation.h"
#include "../UIMApplication.h"


CConversation::CConversation(IUOLMessengerContactPtr pContact) :
	m_pContact(pContact),
	m_bEnableLog(TRUE),
	m_typingState(NOT_TYPING),
	m_nConversationType(IUOLMessengerConversation::CT_IM_CONVERSATION),
	m_nChatId(-1)
{
	CUIMApplication::GetApplication()->GetContactList()->RegisterObserver(this);
}


CConversation::CConversation(const int& nChatId, IUOLMessengerAccountPtr pAccount) :
	m_pAccount(pAccount),
	m_bEnableLog(TRUE),
	m_typingState(NOT_TYPING),
	m_nConversationType(IUOLMessengerConversation::CT_CHAT_CONVERSATION),
	m_nChatId(nChatId)
{
	CUIMApplication::GetApplication()->GetContactList()->RegisterObserver(this);
}


CConversation::~CConversation()
{
	CUIMApplication::GetApplication()->GetContactList()->UnregisterObserver(this);
}


IUOLMessengerContactPtr CConversation::GetContact()
{
	return m_pContact;
}


IUOLMessengerAccountPtr CConversation::GetAccount()
{
	if (m_nConversationType == IUOLMessengerConversation::CT_IM_CONVERSATION)
	{
		return m_pContact->GetAccount();
	}
	else if (m_nConversationType == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
	{
		if (m_pAccount.get())
		{
			return m_pAccount;
		}
		
		return NULL;
	}
    
    return NULL;
}


// This method should not be used when handling a chat.
// Anyway, let's return something.
//
CString CConversation::GetContactName()
{
	if (m_nConversationType == IUOLMessengerConversation::CT_IM_CONVERSATION)
	{
		return m_pContact->GetName();
	}
	else if (m_nConversationType == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
	{
		return CString();
	}

    return CString();
}


void CConversation::AddReceivedMessage(const CString& strText, const CTime& time)
{
	AddMessageImpl(IUOLMessengerConversation::MT_CONTACT, strText, m_pContact->GetAlias(), time, CString());
}


void CConversation::AddSentMessage(const CString& strText, const CTime& time)
{
	CString strAuthor;

	if (m_nConversationType == IUOLMessengerConversation::CT_IM_CONVERSATION)
	{
		strAuthor = m_pContact->GetAccount()->GetNickName();

		if (strAuthor.IsEmpty())
		{
			strAuthor = m_pContact->GetAccount()->GetUserId();
		}
	}
	else if ( (m_nConversationType == IUOLMessengerConversation::CT_CHAT_CONVERSATION) && 
			(GetAccount().get()) )
	{
		strAuthor = GetAccount()->GetUserId();
	}

	AddMessageImpl(IUOLMessengerConversation::MT_ACCOUNT_USER, strText, strAuthor, time, CString());
}


void CConversation::AddSystemMessage(const CString& strText, const CTime& time)
{
	AddMessageImpl(IUOLMessengerConversation::MT_SYSTEM, strText, _T(""), time, CString());
}


void CConversation::GetMessages(CAtlList<IUOLMessengerConversation::MessageEntryPtr>& listMessages)
{
	listMessages.RemoveAll();

	for (POSITION pos = m_listMessages.GetHeadPosition(); pos; )
	{
		IUOLMessengerConversation::MessageEntryPtr pMessageEntry = m_listMessages.GetNext(pos);

		listMessages.AddTail(pMessageEntry);
	}
}


void CConversation::EnableLogging(BOOL bEnable)
{
	m_bEnableLog = bEnable;
}


BOOL CConversation::IsLoggingEnabled() const
{
	return m_bEnableLog;
}


void CConversation::SetTypingState(TypingState typingState)
{
	m_typingState = typingState;
}


TypingState CConversation::GetTypingState() const
{
	return m_typingState;
}


void CConversation::RegisterObserver(CUOLMessengerConversationObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);

		if (!pos)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CConversation::UnregisterObserver(CUOLMessengerConversationObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);

		if (pos != NULL)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}


void CConversation::AddReceivedMessage(const CString& strText, const CTime& time, const CString& strSmiley)
{
	AddMessageImpl(IUOLMessengerConversation::MT_CONTACT, strText, m_pContact->GetAlias(), time, strSmiley);
}


IUOLMessengerConversation::ConversationType CConversation::GetConversationType()
{
	return m_nConversationType;
}


int CConversation::GetChatId()
{
	return m_nChatId;
}


CString CConversation::GetChatKey()
{
    CString strChatKey;

    strChatKey = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetChatKey(m_nChatId, m_pAccount);
    
    return strChatKey;
}


CString CConversation::GetChatTitle()
{
    CString strTitle;

	if (m_ChatContactList.GetCount() > 1)
	{
		for (POSITION pos = m_ChatContactList.GetHeadPosition(); pos; )
		{
			IUOLMessengerContact3Ptr pChatContact = ap_dynamic_cast<IUOLMessengerContact3Ptr>(m_ChatContactList.GetNext(pos));

			// Get chat participant alias
			CString strName = pChatContact->GetName();
			CString strAlias = pChatContact->GetAlias();
			if (strAlias.IsEmpty())
			{
				strAlias = pChatContact->GetName();
			}

			if (strTitle.IsEmpty())
			{
				strTitle.Append(strAlias);
			}
			else
			{
				strTitle.Append(", ");
				strTitle.Append(strAlias);
			}
		}
	}

    return strTitle;
}


void CConversation::GetChatParticipantList(CAtlList<IUOLMessengerContact3Ptr>& chatParticipantList)
{
	chatParticipantList.RemoveAll();
	
	for (POSITION pos = m_ChatContactList.GetHeadPosition(); pos; )
	{
		IUOLMessengerContact3Ptr pChatContact = ap_dynamic_cast<IUOLMessengerContact3Ptr>(m_ChatContactList.GetNext(pos));
		
		chatParticipantList.AddTail(pChatContact);
	}
}


void CConversation::AddReceivedMessage(const CString& strFromUser, const CString& strText, const CTime& time, const CString& strSmiley)
{
	// Verify if sender user is the account user
    if (m_nConversationType == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        CString strAccountUserId = m_pAccount->GetUserId();
        
        if (0 == strFromUser.Compare(strAccountUserId))
        {
            AddMessageImpl(IUOLMessengerConversation::MT_ACCOUNT_USER, strText, strFromUser, time, strSmiley);

            return;
        }
    }
     
    
    AddMessageImpl(IUOLMessengerConversation::MT_CONTACT, strText, strFromUser, time, strSmiley);
}


BOOL CConversation::AddChatParticipant(const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact)
{
	CContactPtr pChatContact = ap_dynamic_cast<CContactPtr>(FindChatParticipant(strChatContactName));
	BOOL bAddOk = FALSE;
	
	if (!pChatContact.get())
	{
		if (bIsContact)
        {
            pChatContact = ap_dynamic_cast<CContactPtr>(CUIMApplication::GetApplication()->GetContactList()->FindContact(m_pAccount, strChatContactName));
            
            // This should't happen, but if do create new contact
            if (!pChatContact.get())
            {
                pChatContact = new CContact(GetAccount(), strChatContactName, strChatContactAlias, bIsContact);
            }
        }
        else
        {
            pChatContact = new CContact(GetAccount(), strChatContactName, strChatContactAlias, bIsContact);
        }
        m_ChatContactList.AddTail(pChatContact);
		
		bAddOk = TRUE;

        NotifyAllChatEvents(&CUOLMessengerConversationObserver2::OnAddChatParticipant, pChatContact);
	}    

	return bAddOk;
}


BOOL CConversation::RenameChatParticipant(const CString& strChatContactOldName, const CString& strChatContactNewName, const CString& strChatContactNewAlias)
{
	CContactPtr pChatContact = ap_dynamic_cast<CContactPtr>(FindChatParticipant(strChatContactOldName));
	BOOL bRenameOk = FALSE;
	
	if (pChatContact.get())
	{
		pChatContact->SetName(strChatContactNewName);
		
		bRenameOk = TRUE;
        
        NotifyAllChatEvents(&CUOLMessengerConversationObserver2::OnRemoveChatParticipant, pChatContact);
	}    
	
	return bRenameOk;
}


BOOL CConversation::UpdateChatParticipant(const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact)
{
	CContactPtr pChatContact = ap_dynamic_cast<CContactPtr>(FindChatParticipant(strChatContactName));
	BOOL bUpdateOk = FALSE;
	
	if (pChatContact.get())
	{
		// We need update m_ChatContactList with updated chat participant, so let's remove it from list
        m_ChatContactList.RemoveAt(m_ChatContactList.Find(pChatContact));

        if (bIsContact)
        {
            pChatContact = ap_dynamic_cast<CContactPtr>(CUIMApplication::GetApplication()->GetContactList()->FindContact(m_pAccount, strChatContactName));
            
            // This should't happen, but if do create new contact
            if (!pChatContact.get())
            {
                pChatContact = new CContact(GetAccount(), strChatContactName, strChatContactAlias, bIsContact);
            }
        }
        else
        {
            pChatContact->UpdateChatContactInfo(strChatContactName, strChatContactAlias, bIsContact);
        }
        
        // Now add updated chat participant to list again
        m_ChatContactList.AddTail(pChatContact);
		
		bUpdateOk = TRUE;
	}
	
	return bUpdateOk;
}


BOOL CConversation::RemoveChatParticipant(const CString& strChatContactName)
{
	CContactPtr pChatContact = ap_dynamic_cast<CContactPtr>(FindChatParticipant(strChatContactName));
	BOOL bRemoveOk = FALSE;
	
	if (pChatContact.get())
	{
		m_ChatContactList.RemoveAt(m_ChatContactList.Find(pChatContact));
		
		bRemoveOk = TRUE;

        NotifyAllChatEvents(&CUOLMessengerConversationObserver2::OnRemoveChatParticipant, pChatContact);
	}
	
	return bRemoveOk;
}


void CConversation::NotifyAll(void (CUOLMessengerConversationObserver::*pfnUpdate)(IUOLMessengerConversation*, IUOLMessengerConversation::MessageEntryPtr), IUOLMessengerConversation::MessageEntryPtr pMessageEntry)
{
	CUOLMessengerConversationObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(this, pMessageEntry);
	}
}


void CConversation::NotifyAllChatEvents(void (CUOLMessengerConversationObserver2::*pfnUpdate)(IUOLMessengerConversation2*, IUOLMessengerContact3Ptr), IUOLMessengerContact3Ptr pChatContact)
{
	CUOLMessengerConversationObserver* pObserver;
	CUOLMessengerConversationObserver2* pObserver2;
	
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		
		pObserver2 = dynamic_cast<CUOLMessengerConversationObserver2*>(pObserver);
		
		if (pObserver2)
		{
			(pObserver2->*pfnUpdate)(this, pChatContact);
		}
	}
}


void CConversation::NotifyAllSwitchContact(void (CUOLMessengerConversationObserver3::*pfnUpdate)(IUOLMessengerConversation*, IUOLMessengerContactPtr, IUOLMessengerContactPtr), IUOLMessengerContactPtr pRemovedContact, IUOLMessengerContactPtr pAddedContact)
{
	CUOLMessengerConversationObserver* pObserver;
	CUOLMessengerConversationObserver3* pObserver3;
	
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		
		pObserver3 = dynamic_cast<CUOLMessengerConversationObserver3*>(pObserver);
		
		if (pObserver3)
		{
			(pObserver3->*pfnUpdate)(this, pRemovedContact, pAddedContact);
		}
	}
}


void CConversation::AddMessageImpl(int nMessageSender, const CString& strText, const CString& strAuthor, const CTime& time, const CString& strSmiley)
{
	IUOLMessengerConversation::MessageEntryPtr pMessageEntry = new IUOLMessengerConversation::MessageEntry();

	pMessageEntry->time = time;
	pMessageEntry->nMessageSender = nMessageSender;
	pMessageEntry->strText = strText;
	pMessageEntry->strAuthor = strAuthor;
	pMessageEntry->strSmiley = strSmiley;

	m_listMessages.AddTail(pMessageEntry);

	NotifyAll(&CUOLMessengerConversationObserver::OnAddMessage, pMessageEntry);
}


IUOLMessengerContact3Ptr CConversation::FindChatParticipant(const CString& strChatContactName)
{
	for (POSITION pos = m_ChatContactList.GetHeadPosition(); pos; )
	{
		IUOLMessengerContact3Ptr pChatContact = ap_dynamic_cast<IUOLMessengerContact3Ptr>(m_ChatContactList.GetNext(pos));

        if (pChatContact)
        {		
            if (pChatContact->GetName().CompareNoCase(strChatContactName) == 0)
            {
                return pChatContact;
            }
        }
	}
	
	return IUOLMessengerContact3Ptr();
}

int CConversation::GetChatParticipantCount()
{
    return m_ChatContactList.GetCount() + 1; // Mais 1 do usuario que iniciou a conferencia...
}


void CConversation::AddRequestFileTransferAsync(CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	CUOLMessengerConversationObserver* pObserver;
	CUOLMessengerConversationObserver2* pObserver2;
	
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		
		pObserver2 = dynamic_cast<CUOLMessengerConversationObserver2*>(pObserver);
		
		if (pObserver2)
		{
			pObserver2->OnAddRequestFileTransferAsync(this, pFileTransferAsyncInfo);
		}
	}
}


void CConversation::OnAddContact(IUOLMessengerContactPtr pAddedContact)
{
	// If this conversation was started with a contact that isn't in contact list
	// and the user adds this contact, we need to switch new added contact on this conversation

	if (m_pContact)
	{
		CString strContactName = m_pContact->GetName();
		if ((m_pContact->GetAccount() == pAddedContact->GetAccount()) && 
			(0 == strContactName.CompareNoCase(pAddedContact->GetName())) &&
			(m_pContact != pAddedContact))
		{
    		IUOLMessengerContactPtr pRemovedContact = m_pContact;
			m_pContact = pAddedContact;
			NotifyAllSwitchContact(&CUOLMessengerConversationObserver3::OnSwitchContact, pRemovedContact, pAddedContact);
		}
	}
}
