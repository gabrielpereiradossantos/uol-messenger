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
#include "MessageTemplateRecord.h"

#include "../HTMLFilter.h"
#include "../controller/UIMApplication.h"
#include "../controller/EmoticonParserManager.h"
#include "../HTMLFilter.h"


CMessageTemplateRecord::CMessageTemplateRecord(IUOLMessengerConversation2Ptr pConversation2, 
			IUOLMessengerConversation::MessageEntryPtr pMessageEntry) : 
		m_pConversation2(pConversation2), 
		m_pMessageEntry(pMessageEntry), 
		m_pFileTransferAsyncInfo(NULL),
		m_pHistoryEvent(NULL)
{
}

CMessageTemplateRecord::CMessageTemplateRecord(IUOLMessengerConversation2Ptr pConversation2, 
			IUOLMessengerConversation::MessageEntryPtr pMessageEntry, 
			CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo) : 
		m_pConversation2(pConversation2), 
		m_pMessageEntry(pMessageEntry), 
		m_pFileTransferAsyncInfo(pFileTransferAsyncInfo),
		m_pHistoryEvent(NULL)
{
}


CMessageTemplateRecord::CMessageTemplateRecord(IUOLMessengerConversation2Ptr pConversation2,
			IUOLMessengerHistoryEventPtr pHistoryEvent) :
		m_pConversation2(pConversation2), 
		m_pMessageEntry(NULL), 
		m_pFileTransferAsyncInfo(NULL),
		m_pHistoryEvent(pHistoryEvent)
{
}


CMessageTemplateRecord::~CMessageTemplateRecord()
{
	if (m_pFileTransferAsyncInfo)
	{
		delete m_pFileTransferAsyncInfo;
	}
}


IUOLMessengerConversation::MessageEntryPtr CMessageTemplateRecord::GetMessageEntry()
{
	return m_pMessageEntry;
}


CRequestFileTransferAsyncInfo* CMessageTemplateRecord::GetRequestFileTransferAsyncInfo()
{
	return m_pFileTransferAsyncInfo;
}


int CMessageTemplateRecord::GetRecordType() const
{
	if (m_pMessageEntry)
	{
		return m_pMessageEntry->nMessageSender;
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		CString strEventFrom = m_pHistoryEvent->GetEventFrom();
		if (0 == strEventFrom.CompareNoCase(m_pConversation2->GetAccount()->GetUserId()))
		{
			return IUOLMessengerConversation::MT_ACCOUNT_USER;
		}
		else
		{
			return IUOLMessengerConversation::MT_CONTACT;
		}
	}
	
	return INVALID_RECORD_TYPE;
}


IUOLMessengerConversationPtr CMessageTemplateRecord::GetConversation() const
{
	return m_pConversation2;
}


CString CMessageTemplateRecord::GetAuthor() const
{
	CString strAuthor;
	
	if (m_pMessageEntry)
	{
		if ( (m_pMessageEntry->nMessageSender != IUOLMessengerConversation::MT_CONTACT) && 
				(m_pConversation2) && (m_pConversation2->GetAccount()) )
		{
			strAuthor = GetAccountNickName();
		}
		else if (m_pConversation2)
		{
			strAuthor = GetContactAlias();
		}
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		strAuthor = FindAlias(m_pHistoryEvent->GetEventFrom());
	}
	
	return CHTMLFilter::TextToHTML(strAuthor);
}


CString CMessageTemplateRecord::GetAuthorId() const
{
	CString strAuthorId;
	
	if (m_pMessageEntry)
	{
		if ( (m_pMessageEntry->nMessageSender != IUOLMessengerConversation::MT_CONTACT) && 
				(m_pConversation2) && (m_pConversation2->GetAccount()) )
		{
			strAuthorId = GetAccountUserId();
		}
		else if (m_pConversation2)
		{
			strAuthorId = GetContactName();
		}
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		strAuthorId = m_pHistoryEvent->GetEventFrom();
	}
	
	return strAuthorId;
}


CString CMessageTemplateRecord::GetAuthorIconPath() const
{
	CString strIconPath;
	
	// Get author icon...
	//
	if (GetRecordType() == IUOLMessengerConversation::MT_ACCOUNT_USER)
	{
		if ((m_pConversation2) && (m_pConversation2->GetAccount()))
		{
			strIconPath = GetAccountIconPath();
		}
	}
	else
	{
		strIconPath = GetContactIconPath();
	}

	return strIconPath;
}


CString CMessageTemplateRecord::GetAccountUserId() const
{
	return m_pConversation2->GetAccount()->GetUserId();
}


CString CMessageTemplateRecord::GetAccountNickName() const
{
	return m_pConversation2->GetAccount()->GetNickName();
}


CString CMessageTemplateRecord::GetAccountIconPath() const
{
	IUOLMessengerAccountPtr pAccount = m_pConversation2->GetAccount();
	IUOLMessengerUserIconPtr pUserIcon;
	CString strIconPath;
	
	if (pAccount != NULL)
	{
		pUserIcon = pAccount->GetUserIcon();
		
		if (pUserIcon != NULL)
		{
			strIconPath = pUserIcon->GetImagePath();
		}
		
		if (strIconPath.IsEmpty())
		{
			strIconPath = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol()->GetUserImagePath(pAccount->GetProtocolId(),1);
		}
	}
	
	return strIconPath;
}


CString CMessageTemplateRecord::GetContactName() const
{
	CString strContactName;
	
	if (m_pMessageEntry)
	{
		IUOLMessengerContactPtr pContact;
		if (m_pConversation2->GetConversationType() == 
			IUOLMessengerConversation2::CT_CHAT_CONVERSATION)
		{
			pContact = ap_dynamic_cast<IUOLMessengerContactPtr>(
				m_pConversation2->FindChatParticipant(
				m_pMessageEntry->strAuthor));
		}
		else
		{
			pContact = m_pConversation2->GetContact();
		}

		if (pContact)
		{
			strContactName = pContact->GetName();
		}
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		CString strAuthorId = m_pHistoryEvent->GetEventFrom();
		if (0 == strAuthorId.CompareNoCase(m_pConversation2->GetAccount()->GetUserId()))
		{
			strContactName = m_pHistoryEvent->GetEventTo();
		}
		else
		{
			strContactName = strAuthorId;
		}
	}
	
	return strContactName;
}


CString CMessageTemplateRecord::GetContactAlias() const
{
	CString strContactAlias;
	
	if (m_pMessageEntry)
	{
		IUOLMessengerContactPtr pContact;
		if (m_pConversation2->GetConversationType() == 
			IUOLMessengerConversation2::CT_CHAT_CONVERSATION)
		{
			pContact = ap_dynamic_cast<IUOLMessengerContactPtr>(
				m_pConversation2->FindChatParticipant(
				m_pMessageEntry->strAuthor));
		}
		else
		{
			pContact = m_pConversation2->GetContact();
		}

		ATLASSERT(pContact);

		if ( (pContact) && (!pContact->GetAlias().IsEmpty()) )
		{
			strContactAlias = pContact->GetAlias();
		}
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		strContactAlias = FindAlias(GetContactName());
	}
	
	return strContactAlias;
}


CString CMessageTemplateRecord::GetContactIconPath() const
{
	CString strIconPath;
	IUOLMessengerContactPtr pContact;
	IUOLMessengerUserIconPtr pUserIcon;
	
	if (m_pMessageEntry)
	{
		if (m_pConversation2->GetConversationType() != 
			IUOLMessengerConversation::CT_CHAT_CONVERSATION)
		{
			pContact = m_pConversation2->GetContact();
		}
		else
		{
			// Find chat participant.
			//
			pContact = ap_dynamic_cast<IUOLMessengerContactPtr>(
				m_pConversation2->FindChatParticipant(m_pMessageEntry->strAuthor));
		}
	}
	else if (m_pHistoryEvent)
	{
		// Recent History
		pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(
			m_pConversation2->GetAccount(), GetContactName());
	}

	if (pContact)
	{
		pUserIcon = pContact->GetUserIcon();

		if (pUserIcon != NULL)
		{
			strIconPath = pUserIcon->GetImagePath();
		}
	}
	
	if (strIconPath.IsEmpty())
	{
		strIconPath = CUIMApplication::GetApplication()->
			GetUIManager()->GetImagesProtocol()->GetContactImagePath(
			m_pConversation2->GetAccount()->GetProtocolId(), 1);
	}


	return strIconPath;
}


CString CMessageTemplateRecord::GetService() const
{
	CString strService = m_pConversation2->GetAccount()->GetProtocolId();
	
	if (!strService.IsEmpty())
	{
		strService.Replace("prpl-", "");
		(strService.GetBuffer())[0] = toupper(strService[0]);
	}
	
	return strService;
}


CString CMessageTemplateRecord::GetMessage() const
{
	CString strMessage;
	
	if (m_pMessageEntry)
	{
		strMessage = m_pMessageEntry->strText;
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		strMessage = CHTMLFilter::TextToHTML(m_pHistoryEvent->GetEventText());
	}
	
	return strMessage;
}


CString CMessageTemplateRecord::GetMessageParsed()
{
	CString strProcessedMessage;
	
	if (m_pMessageEntry)
	{
		CString strMessage = m_pMessageEntry->strText;
		
		if (!m_pMessageEntry->strSmiley.IsEmpty())
		{
			CString newMessageInitialTags;
			CString newMessageText;
			CString newMessageFinalTags;
			
			if (SplitMessage(strMessage, newMessageInitialTags, newMessageText, newMessageFinalTags))
			{
				HRESULT hr = ProcessCustomEmoticons(newMessageText, m_pMessageEntry->strSmiley);
				
				if (FAILED(hr))
				{
					ATLTRACE("%s Failed to parse custom emoticons to message |%s| \n", 
							__FUNCTION__, newMessageText);
				}
				else
				{
					strMessage = newMessageInitialTags + newMessageText + newMessageFinalTags;
				}
			}
		}
		
		ATLTRACE("%s htmlMessage |%s| \n",__FUNCTION__, strMessage);
		
		// Delete "<span style='font-size: medium;'/>"
		strMessage.Replace("<span style='font-size: medium;'/>", "");
		strProcessedMessage = CEmoticonParserManager::GetInstance()->ParseHtmlText(
				m_pConversation2->GetAccount()->GetProtocol()->GetId().c_str(), strMessage);
		
		ATLTRACE("%s Message parsed |%s| \n",__FUNCTION__, strProcessedMessage);
	}
	else
	{	
		strProcessedMessage = CEmoticonParserManager::GetInstance()->ParseHtmlText(
				m_pConversation2->GetAccount()->GetProtocol()->GetId().c_str(), GetMessage());
	}
	
	return strProcessedMessage;
}


CTime CMessageTemplateRecord::GetTime() const
{
	if (m_pMessageEntry)
	{
		return m_pMessageEntry->time;
	}
	else if (m_pHistoryEvent)
	{
		// Recent history
		return m_pHistoryEvent->GetFullTime();
	}
	
	return CTime::GetCurrentTime();
}


int CMessageTemplateRecord::GetFileTransferId()
{
	if (m_pFileTransferAsyncInfo)
	{
		return m_pFileTransferAsyncInfo->GetId();
	}
	
	return INVALID_FILE_TRANSFER_ID;
}


CString CMessageTemplateRecord::GetFileTransferName()
{
	if (m_pFileTransferAsyncInfo)
	{
		return m_pFileTransferAsyncInfo->GetFileName().c_str();
	}
	
	return CString("");
}


HRESULT CMessageTemplateRecord::ProcessCustomEmoticons(CString& strMessage, CString strSmileys)
{
	HRESULT hr = E_FAIL;
	
	IUOLMessengerCustomEmoticonManagerPtr pCustomEmoticonManager = GetCustomEmoticonManager();
	
	if (pCustomEmoticonManager)
	{
		hr = S_OK;
		
		CAtlList<CString> emoticonList;
		CString customEmoticon;
		int curPos= 0;
		
		// Add custom emoticons shortcuts received to a list.
		// They must be added on a descending order so the parser 
		// works fine.
		// That way a custom emoticon with shortcut "_burn" has no 
		// chance to be parsed wrongly with a shortcut "_bu", for 
		// example.
		//
		customEmoticon = strSmileys.Tokenize(" ", curPos);
		while (!customEmoticon.IsEmpty())
		{
			BOOL bAdded = FALSE;
			
			if (emoticonList.IsEmpty())
			{
				emoticonList.AddHead(customEmoticon);
			}
			else
			{
				for (POSITION listPos = emoticonList.GetHeadPosition(); listPos; )
				{
					CString emoticonShortcut = emoticonList.GetNext(listPos);
					
					if (customEmoticon.CompareNoCase(emoticonShortcut) > 0)
					{
						emoticonList.InsertBefore(listPos, customEmoticon);
						
						bAdded = TRUE;
						
						break;
					}
				}
				
				if (!bAdded)
				{
					emoticonList.AddTail(customEmoticon);
				}
			}
			
			customEmoticon = strSmileys.Tokenize(" ",curPos);
		}
		
		BOOL bChangedEmoticon = FALSE;
		CString customEmoticonImageTag;
		CString escapedCustomEmoticon;
		
		for (int strMessageIndex = 0; strMessageIndex < strMessage.GetLength(); )
		{
			for (POSITION listPos = emoticonList.GetHeadPosition(); listPos; )
			{
				customEmoticon = emoticonList.GetNext(listPos);
				escapedCustomEmoticon = CHTMLFilter::TextToHTML(customEmoticon);
				
				if (strMessage.Find(escapedCustomEmoticon, strMessageIndex) == strMessageIndex)
				{
					hr = BuildCustomEmoticonImageTag(pCustomEmoticonManager, 
							customEmoticonImageTag, customEmoticon);
					
					if (SUCCEEDED(hr))
					{
						strMessage.Delete(strMessageIndex, escapedCustomEmoticon.GetLength());
						strMessage.Insert(strMessageIndex, customEmoticonImageTag);
						
						bChangedEmoticon = TRUE;
					}
					
					break;
				}
			}
			
			if (FAILED(hr))
			{
				break;
			}
			else if (bChangedEmoticon)
			{
				bChangedEmoticon = FALSE;
				
				strMessageIndex += (customEmoticonImageTag.GetLength() - customEmoticon.GetLength() + 1);
			}
			else
			{
				strMessageIndex++;
			}
		}
		
		emoticonList.RemoveAll();
	}
	
	return hr;
}


HRESULT CMessageTemplateRecord::BuildCustomEmoticonImageTag(
		IUOLMessengerCustomEmoticonManagerPtr pCustomEmoticonManager, 
		CString& customEmoticonImageTag, CString customEmoticon)
{
	HRESULT hr = E_FAIL;
	
	if (m_pConversation2)
	{
		IUOLMessengerCustomEmoticonPtr pCustomEmoticon = NULL;
		
		if ((pCustomEmoticon = pCustomEmoticonManager->FindCustomEmoticon(
				m_pConversation2, customEmoticon)) != NULL)
		{
			if (pCustomEmoticon->IsEmotionReadyForUse())
			{
				// Append the final arquive path.
				//
				LONG lImageFinalHeight = pCustomEmoticon->GetEmoticonHeight();
				LONG lImageFinalWidth = pCustomEmoticon->GetEmoticonWidth();
				
				CCustomEmoticonManager::CalculateImagePresentationSize(
						lImageFinalHeight, lImageFinalWidth);
				
				customEmoticonImageTag.Format("<img src=\"%s\" alt=\"%s\" height=\"%d\" width=\"%d\" class=\"%s\">", 
						pCustomEmoticon->GetEmoticonPath(), 
						CHTMLFilter::TextToHTML(pCustomEmoticon->GetEmoticonShortcut()), 
						lImageFinalHeight, lImageFinalWidth, CUSTOM_EMOTICON_IMAGE_TAG_CLASS_NAME);
			}
			else
			{
				// Create a tag of imagem type but with the default or a non valid image path.
				//
				CString downloadingFileName = pCustomEmoticonManager->GetDownloadingEmoticonFile();
				
				if (downloadingFileName.IsEmpty())
				{
					downloadingFileName.Format("%s.gif", pCustomEmoticon->GetEmoticonShortcut());
				}
				
				customEmoticonImageTag.Format("<img id=\"%s\" name=\"%s\" src=\"%s\" class=\"%s\">", 
						CHTMLFilter::TextToHTML(pCustomEmoticon->GetEmoticonShortcut()), 
						CHTMLFilter::TextToHTML(pCustomEmoticon->GetEmoticonShortcut()), 
						downloadingFileName, CUSTOM_EMOTICON_IMAGE_TAG_CLASS_NAME);
			}
			
			hr = S_OK;
		}
	}
	
	return hr;
}


IUOLMessengerCustomEmoticonManagerPtr CMessageTemplateRecord::GetCustomEmoticonManager()
{
	IUOLMessengerConversationManagerPtr pConversationManager = 
			CUIMApplication::GetApplication()->GetConversationManager();
	
	IUOLMessengerConversationManager2Ptr pConversationManager2 = NULL;
	IUOLMessengerCustomEmoticonManagerPtr pCustomEmoticonManager = NULL;
	
	if (pConversationManager)
	{
		pConversationManager2 = 
				ap_dynamic_cast<IUOLMessengerConversationManager2Ptr>(pConversationManager);
	}
	
	if (pConversationManager2)
	{
		pCustomEmoticonManager = pConversationManager2->GetCustomEmoticonManager();
	}
	
	return pCustomEmoticonManager;
}


BOOL CMessageTemplateRecord::SplitMessage(CString strOriginalMessage, 
		CString& strInitialTags, CString& strMessageText, CString& strFinalTags)
{
	BOOL bSplited = FALSE;
	int nSearchingInitialTag = 0;
	int nLastInitialTagFound = -1;
	int nSearchingFinalTag = 0;
	int nFirstLastTagFound = -1;
	CString tempSearchingFinalTag;
	int nFinalTagsLength = 0;
	
	if (!strOriginalMessage.IsEmpty())
	{
		while (nSearchingInitialTag != -1)
		{
			if ((nSearchingInitialTag = strOriginalMessage.Find(">", nSearchingInitialTag)) != -1)
			{
				nLastInitialTagFound = nSearchingInitialTag;
				nSearchingInitialTag++;
				
				if (strOriginalMessage[nSearchingInitialTag] != '<')
				{
					break;
				}
			}
		}
		
		if (nLastInitialTagFound > 0)
		{
			strInitialTags = strOriginalMessage.Left(nSearchingInitialTag);
		}
		
		tempSearchingFinalTag = strOriginalMessage.Right(
				strOriginalMessage.GetLength() - strInitialTags.GetLength());
		
		while (nSearchingFinalTag != -1)
		{
			if ((nSearchingFinalTag = tempSearchingFinalTag.ReverseFind('<')) != -1)
			{
				nFirstLastTagFound = nSearchingFinalTag;
				tempSearchingFinalTag.Delete(nSearchingFinalTag, 
						tempSearchingFinalTag.GetLength() - nSearchingFinalTag);
			}
		}
		
		if (nFirstLastTagFound)
		{
			strMessageText = tempSearchingFinalTag;
			
			nFinalTagsLength = strOriginalMessage.GetLength() - 
					(strInitialTags.GetLength() + strMessageText.GetLength());
			
			strFinalTags = strOriginalMessage.Right(nFinalTagsLength);
		}
		
		bSplited = !strMessageText.IsEmpty();
	}
	
	return bSplited;
}


CString CMessageTemplateRecord::FindAlias(const CString& strName) const
{
	CString strAlias = strName;

	if (m_pConversation2)
	{
		CString strAcountUserId = m_pConversation2->GetAccount()->GetUserId();
		
		// is sender (EventFrom) account user?
		if (0 == strAcountUserId.CompareNoCase(strName))
		{
			IUOLMessengerAccountPtr pAccount;
			pAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
				strAcountUserId, m_pConversation2->GetAccount()->GetProtocolId());
			if (pAccount)
			{
				strAlias = pAccount->GetNickName();
			}
		}
		else
		{
			IUOLMessengerContactPtr pContact;
			pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(
				strAcountUserId, m_pConversation2->GetAccount()->GetProtocolId(), strName);
			if (pContact)
			{
				strAlias = pContact->GetAlias();
			}
		}
	}

	return strAlias;
}
