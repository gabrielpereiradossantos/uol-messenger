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
#include "ConversationManager.h"

#include "../../resource.h"
#include "../UIMApplication.h"
#include "../Message/MessageQueueController.h"
#include "../UIManager.h"
#include "../../View/Skin/DefaultEmoticonImageElement.h"


// Test code
//#define _CONVERSATIONDEBUG


CConversationManager::CConversationManager()
{
}


CConversationManager::~CConversationManager()
{
}


BOOL CConversationManager::Init(IUOLMessengerFacade* pFacade)
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (pFacade)
	{
		m_pFacade = pFacade;

		m_pCustomEmoticonManager = new CCustomEmoticonManager();
		
		CString strCustomEmoticonPath = GetCustomEmoticonRootPath();
		
		if (m_pCustomEmoticonManager.get())
		{
			if ( (strCustomEmoticonPath.IsEmpty()) || 
					(!m_pCustomEmoticonManager->SetCustomEmoticonStoreRootPath(strCustomEmoticonPath)) )
			{
				m_pCustomEmoticonManager.reset();
				
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"SetCustomEmoticonStoreRootPath() failed. strCustomEmoticonPath = [%s]", 
						strCustomEmoticonPath);
			}
			else
			{
				CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
				
				CDefaultEmoticonImageElementPtr pDefaultEmoticonImage = 
						ap_dynamic_cast<CDefaultEmoticonImageElementPtr>(
								pUIManagerPtr->GetWindowManager()->GetElement("__DefaultEmoticonImageInstance"));
				
				if (pDefaultEmoticonImage)
				{
					m_pCustomEmoticonManager->SetDownloadingEmoticonFile(pDefaultEmoticonImage->GetImageFilePath());
					m_pCustomEmoticonManager->SetDownloadingEmoticonSize(pDefaultEmoticonImage->GetImageFileSize());
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogError(__FUNCTION__, "pDefaultEmoticonImage == NULL");
				}
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "m_pCustomEmoticonManager == NULL");
		}
		
		bRet = TRUE;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d]", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CConversationManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pFacade = [%x]", m_pFacade);
	
	if (m_pFacade)
	{
		m_mapConversations.RemoveAll();
		m_observerNotifier.UnregisterAll();
		
		m_pFacade = NULL;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CConversationManager::NotifyIMReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText)
{
	NotifyIMReceived(pAccount, strContact, strText, CString());
}


void CConversationManager::NotifyIMSent(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText)
{
	IUOLMessengerConversationPtr pConversation = FindConversation(pAccount, strContact);

	if (pConversation)
	{
		pConversation->AddSentMessage(strText, CTime::GetCurrentTime());

		m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnIMSent, pConversation, strText);
	}
}


void CConversationManager::NotifyIMSendError(IUOLMessengerAccountPtr pAccount, const CString& strContact, IMProtocolError error)
{
	IUOLMessengerConversationPtr pConversation = FindConversation(pAccount, strContact);

	if (!pConversation)
	{
		ATLASSERT(m_pFacade);

		IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);

		if (!pContact)
		{
			pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
		}

		pConversation = CreateConversation(pContact);

		// ... Launch message window
	}

	CString strMessage;

	if (error == IM_PROTOCOL_ERROR_RECEIPT_NOT_ONLINE)
	{
		strMessage.LoadString(IDS_MESSAGE_WINDOW_RECEIPT_NOT_ONLINE_IM_SEND_ERROR);
	}
	else if (error == IM_PROTOCOL_ERROR_SENDER_NOT_ONLINE)
	{
		strMessage.LoadString(IDS_MESSAGE_WINDOW_SENDER_NOT_ONLINE_IM_SEND_ERROR);
	}
	else
	{
		strMessage.LoadString(IDS_MESSAGE_WINDOW_GENERIC_IM_SEND_ERROR);
	}

	pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());

	m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnIMSendError, pConversation, error);
}


void CConversationManager::NotifyContactTyping(IUOLMessengerAccountPtr pAccount, const CString& strContact, TypingState state)
{
	IUOLMessengerConversationPtr pConversation = FindConversation(pAccount, strContact);

	if (pConversation)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnContactTyping, pConversation, state);
	}
}


void CConversationManager::NotifyConversationWindowClosed(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IUOLMessengerConversationPtr pConversation = FindConversation(pAccount, strContact);

	if (pConversation)
	{
		CString strMessage;
		/*strMessage.Format(IDS_MESSAGE_WINDOW_CONV_WINDOW_CLOSED_FORMAT, pConversation->GetContact()->GetAlias());

		pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());*/

		m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnConversationWindowClosed, pConversation);
	}
}


void CConversationManager::NotifyConversationTimeout(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	IUOLMessengerConversationPtr pConversation = FindConversation(pAccount, strContact);

	if (pConversation)
	{
		m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnConversationTimeout, pConversation);
	}
}


void CConversationManager::NotifyNudgeReceived(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strText)
{
	CString formatedStrText;
	CString nudgeFormatingText;
	
	IUOLMessengerMessageSettings3Ptr pMessageSettings3 = 
			ap_static_cast<IUOLMessengerMessageSettings3Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
	
	if ( (pMessageSettings3) && (!pMessageSettings3->GetAllowNudges()) )
	{
		ATLTRACE(_T("%s received nudge from contact %s. But it's not allowed now.\n"), pAccount->GetUserId(), strContact);
		return;
	}
	
	if (m_pFacade->GetPrivacyManager()->IsDenied(pAccount, strContact))
	{
		ATLTRACE(_T("%s received nudge from denied contact %s\n"), pAccount->GetUserId(), strContact);
		return;
	}
	
	BOOL bNeedConfirmation = FALSE;
	
	ATLASSERT(m_pFacade);
	
	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);
	
	/// <TODAOFERTA>
	// Authorize without checking
	if (!pContact)
	{
		CString strProtocolId = pAccount->GetProtocolId();
		
		if (0 == strProtocolId.Compare(_T("prpl-uolto")))
		{
			pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
		}
	}
	/// </TODAOFERTA>
	
	if (!pContact)
	{
		// Contact not on list, 
		// check nudge receiving security level...
		int nSecurityLevel = m_pFacade->GetSettingsManager()->GetContactListSettings()->GetSecurityLevel();
		
		if (nSecurityLevel != IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST)
		{
			// Create contact
			pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
			
			if (nSecurityLevel == IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE)
			{
				// Check if contact was authorized
				IUOLMessengerMessageReceivingFilterPtr pFilter = m_pFacade->GetMessageReceivingFilter();
				
				BOOL bAuthorized = FALSE;
				
				if (pFilter->CheckAuthorization(pContact, bAuthorized))
				{
					if (!bAuthorized)
					{
						// User has not authorized nudges from this contact
						ATLTRACE(_T("%s received nudge from unauthorized contact %s\n"), pAccount->GetUserId(), strContact);
						return;
					}
				}
				
				bNeedConfirmation = !bAuthorized;
			}
		}
	}
	
	if (pContact)
	{
		formatedStrText = strText;
		
		if (pContact->GetAlias())
		{
			UOLLib::ISettings* pStringLoader = 
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			
			nudgeFormatingText = pStringLoader->GetValue("uol.resource.IDS_NUDGE_RECEIVING_MESSAGE");
			
			formatedStrText.Format(nudgeFormatingText, pContact->GetAlias());
		}
		
		// User authorized contact nudges or 
		// need to authorize or deny contact nudges
		
		IUOLMessengerConversationPtr pConversation = CreateConversation(pContact);
		
		if (FALSE == bNeedConfirmation)
		{
			ATLASSERT(pConversation);
			
			// No confirmation needed, launch message window
			m_pFacade->GetUIManager()->LaunchMessageWindow(pContact, TRUE);
			
			pConversation->AddSystemMessage(formatedStrText, CTime::GetCurrentTime());
			
			// Send away auto-response...
			DWORD dwAccountStatus = m_pFacade->GetAccountConnectionManager()->GetStatus(pAccount);
			
			if ((dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY) ||
				(dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AWAY))
			{
				CString strUserMessage = m_pFacade->GetSettingsManager()->GetStatusSettings()->GetAwayMessage();
				
				strUserMessage.Trim();
				
				CString strContactName = pContact->GetName();
				if ((!strUserMessage.IsEmpty()) && (!m_pFacade->GetAccountConnectionManager()->IsReceivedAway(pAccount, strContactName)))
				{
					CString strAwayMessage;
					strAwayMessage.LoadString(IDS_AWAY_MESSAGE_PREFIX);
					
					strAwayMessage += strUserMessage;
					
					if (SendIM(pConversation, strAwayMessage))
					{
						m_pFacade->GetAccountConnectionManager()->AddReceivedAway(pAccount, strContactName);
					}
				}
			}
			
			m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnNudgeReceived, pConversation, formatedStrText, IUOLMessengerConversationManager::MRS_AUTHORIZED);
		}
		else
		{
			// Queue message...
			CMessageQueueController* pMessageQueueController = static_cast<CUIMApplication*>(m_pFacade)->GetMessageQueueController();
			
			CInstantMessagePtr pInstantMessage = new CInstantMessage(pContact, formatedStrText, CTime::GetCurrentTime());
			
			pMessageQueueController->AddMessage(pInstantMessage);					
			
			// Ask if user wants to receive messages from this contact
			m_pFacade->GetUIManager()->LaunchMessageReceivingConfirmationWindow(pContact);
			
			m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnNudgeReceived, pConversation, formatedStrText, IUOLMessengerConversationManager::MRS_CONFIRMATION_REQUIRED);
		}

		// Shake MessageWindowFrame
		CMessageWindowFramePtr pMessageWindow;
		IUOLMessengerConversationPtr pTempConversation;

		IUOLMessengerMessageWindowManagerPtr pConversationManager =	CUIMApplication::GetApplication()->GetUIManager()->GetMessageWindowManager();
		pMessageWindow = ap_dynamic_cast<CMessageWindowFramePtr>(pConversationManager->FindConversationMessageWindow(pAccount, strContact, pTempConversation));
		
		if (pMessageWindow)
		{
			CUIMApplication::GetApplication()->ShakeWindow(pMessageWindow->m_hWnd);
		}
	}
}


CustomEmoticonAddResult CConversationManager::ConfirmCustomSmileyAdd(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strSmiley, const BOOL bRemote)
{
	if (m_pCustomEmoticonManager.get())
	{
		BOOL bNeedConfirmation = FALSE;
		
		ATLASSERT(m_pFacade);
		
		IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);
		
		/// <TODAOFERTA>
		// Authorize without checking
		if (!pContact)
		{
			CString strProtocolId = pAccount->GetProtocolId();
			
			if (0 == strProtocolId.Compare(_T("prpl-uolto")))
			{
				pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
			}
		}
		/// </TODAOFERTA>
		
		if (!pContact)
		{
			// Contact not on list, 
			// check custom emoticon receiving security level...
			int nSecurityLevel = m_pFacade->GetSettingsManager()->GetContactListSettings()->GetSecurityLevel();
			
			if (nSecurityLevel != IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST)
			{
				// Create contact
				pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
				
				if (nSecurityLevel == IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE)
				{
					// Check if contact was authorized
					IUOLMessengerMessageReceivingFilterPtr pFilter = m_pFacade->GetMessageReceivingFilter();
					
					BOOL bAuthorized = FALSE;
					
					if (pFilter->CheckAuthorization(pContact, bAuthorized))
					{
						if (!bAuthorized)
						{
							// User has not authorized custom emoticons from this contact
							ATLTRACE(_T("%s received nudge from unauthorized contact %s\n"), pAccount->GetUserId(), strContact);
							
							return CUSTOM_EMOTICON_ADD_RESULT_ERROR;
						}
					}
					
					bNeedConfirmation = !bAuthorized;
				}
			}
		}
		
		if ( (pContact) && (FALSE == bNeedConfirmation) )
		{
			IUOLMessengerConversationPtr pConversation = CreateConversation(pContact);
			
			if (pConversation)
			{
				IUOLMessengerMessageSettings3Ptr pMessageSettings3 = 
						ap_static_cast<IUOLMessengerMessageSettings3Ptr>(
								CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
				
				if ( (pMessageSettings3) && (!pMessageSettings3->GetAcceptCustomEmoticons()) )
				{
					ATLTRACE(_T("%s received a custom emoticon from contact %s. But it's not allowed now.\n"), 
							pAccount->GetUserId(), strContact);
					
					return CUSTOM_EMOTICON_ADD_RESULT_NOT_ALLOWED;
				}
				if (m_pCustomEmoticonManager->FindCustomEmoticon(pConversation, strSmiley))
				{
					return CUSTOM_EMOTICON_ADD_RESULT_ALREADY_EXISTS;
				}
				else if (m_pCustomEmoticonManager->CreateCustomEmoticon(pConversation, strSmiley))
				{
					return CUSTOM_EMOTICON_ADD_RESULT_NEW;
				}
			}
		}
	}
	
	return CUSTOM_EMOTICON_ADD_RESULT_ERROR;
}


void CConversationManager::NotifyCustomSmileyWrite(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strSmiley, const unsigned char* pSmileyData, unsigned int nSmileyDataSize, const CString& strFileExtension)
{
	if (m_pCustomEmoticonManager.get())
	{
		ATLASSERT(m_pFacade);
		
		IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);
		
		if (pContact)
		{
			IUOLMessengerConversationPtr pConversation = CreateConversation(pContact);
			
			m_pCustomEmoticonManager->UpdateCustomEmoticonData(pConversation, 
					strSmiley, pSmileyData, nSmileyDataSize, strFileExtension);
		}
	}
}


void CConversationManager::NotifyCustomSmileyClose(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strSmiley)
{
	if (m_pCustomEmoticonManager.get())
	{
		ATLASSERT(m_pFacade);
		
		IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);
		
		if (pContact)
		{
			IUOLMessengerConversationPtr pConversation = CreateConversation(pContact);
			
			m_pCustomEmoticonManager->StoreCustomEmoticon(pConversation, strSmiley);
		}
	}
}


void CConversationManager::NotifyIMReceived(IUOLMessengerAccountPtr pAccount, 
		const CString& strContact, const CString& strText, const CString& strSmileyList)
{
	if (m_pFacade->GetPrivacyManager()->IsDenied(pAccount, strContact))
	{
		ATLTRACE(_T("%s received message from denied contact %s\n"), pAccount->GetUserId(), strContact);
		return;
	}

	BOOL bNeedConfirmation = FALSE;

	ATLASSERT(m_pFacade);

	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);

	/// <TODAOFERTA>
	// Authorize without checking
	if (!pContact)
	{
		CString strProtocolId = pAccount->GetProtocolId();

		if (0 == strProtocolId.Compare(_T("prpl-uolto")))
		{
			pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
		}
	}
	/// </TODAOFERTA>

	if (!pContact)
	{
		// Contact not on list, 
		// check message receiving security level...
		int nSecurityLevel = m_pFacade->GetSettingsManager()->GetContactListSettings()->GetSecurityLevel();

		if (nSecurityLevel != IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST)
		{
			// Create contact
			pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);

			if (nSecurityLevel == IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE)
			{
				// Check if contact was authorized
				IUOLMessengerMessageReceivingFilterPtr pFilter = m_pFacade->GetMessageReceivingFilter();

				BOOL bAuthorized = FALSE;

				if (pFilter->CheckAuthorization(pContact, bAuthorized))
				{
					if (!bAuthorized)
					{
						// User has not authorized messages from this contact
						ATLTRACE(_T("%s received message from unauthorized contact %s\n"), pAccount->GetUserId(), strContact);
						return;
					}
				}

				bNeedConfirmation = !bAuthorized;
			}
		}
	}

	if (pContact)
	{
		// User authorized contact messages or 
		// need to authorize or deny contact messages

		IUOLMessengerConversationPtr pConversation = CreateConversation(pContact);

		if (FALSE == bNeedConfirmation)
		{
			ATLASSERT(pConversation);

			// No confirmation needed, launch message window
			m_pFacade->GetUIManager()->LaunchMessageWindow(pContact, TRUE);

			if (strSmileyList.IsEmpty())
			{
				pConversation->AddReceivedMessage(strText, CTime::GetCurrentTime());
			}
			else
			{
				IUOLMessengerConversation2Ptr pConversation2 = 
						ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
				
				ATLASSERT(pConversation2);
				
				pConversation2->AddReceivedMessage(strText, CTime::GetCurrentTime(), strSmileyList);
			}

			// Send away auto-response...
			DWORD dwAccountStatus = m_pFacade->GetAccountConnectionManager()->GetStatus(pAccount);

			if ((dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY) ||
				(dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AWAY))
			{
				CString strUserMessage = m_pFacade->GetSettingsManager()->GetStatusSettings()->GetAwayMessage();

				strUserMessage.Trim();

				CString strContactName = pContact->GetName();
				if ((!strUserMessage.IsEmpty()) && (!m_pFacade->GetAccountConnectionManager()->IsReceivedAway(pAccount, strContactName)))
				{
					CString strAwayMessage;
					strAwayMessage.LoadString(IDS_AWAY_MESSAGE_PREFIX);

					strAwayMessage += strUserMessage;

					m_pFacade->GetAccountConnectionManager()->AddReceivedAway(pAccount, strContactName);

					SendIM(pConversation, strAwayMessage);
				}
			}

			m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnIMReceived, pConversation, strText, IUOLMessengerConversationManager::MRS_AUTHORIZED);
		}
		else
		{
			// Queue message...
			CMessageQueueController* pMessageQueueController = static_cast<CUIMApplication*>(m_pFacade)->GetMessageQueueController();

			CInstantMessagePtr pInstantMessage = new CInstantMessage(pContact, strText, CTime::GetCurrentTime());

			pMessageQueueController->AddMessage(pInstantMessage);

			// Ask if user wants to receive messages from this contact
			m_pFacade->GetUIManager()->LaunchMessageReceivingConfirmationWindow(pContact);

			m_observerNotifier.NotifyAll(&CUOLMessengerConversationManagerObserver::OnIMReceived, pConversation, strText, IUOLMessengerConversationManager::MRS_CONFIRMATION_REQUIRED);
		}
	}
}


IUOLMessengerCustomEmoticonManagerPtr CConversationManager::GetCustomEmoticonManager()
{
	return m_pCustomEmoticonManager;
}


void CConversationManager::NotifyChatAddUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact)
{
    CString strChatKey = GetChatKey(nChatId, pAccount);
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

    if (!pConversation2)
    {
        pConversation2 = CreateConversation(nChatId, pAccount);

        ATLASSERT(pConversation2);

        // Launch message window
        CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
        if (pUIManager)
        {
            // Was I invited this contact for chat?
            if (0 == strChatContactName.Compare(GetChatInvitedContact()))
            {
                pUIManager->LaunchMessageWindow(pAccount, nChatId, FALSE);
                SetChatInvitedContact("");

				// Invite others selected contacts
				pUIManager->InviteContactsForChat(pConversation2, pAccount, strChatContactName);
            }
            else
            {
                pUIManager->LaunchMessageWindow(pAccount, nChatId, TRUE);
            }
        }
    }

    pConversation2->AddChatParticipant(strChatContactName, strChatContactAlias, bIsContact);
}


void CConversationManager::NotifyChatRenameUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactOldName, const CString& strChatContactNewName, const CString& strChatContactNewAlias)
{
    CString strChatKey = GetChatKey(nChatId, pAccount);
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

    if (pConversation2)
    {
        pConversation2->RenameChatParticipant(strChatContactOldName, strChatContactNewName, strChatContactNewAlias);
    }    
}


void CConversationManager::NotifyChatRemoveUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName)
{
    CString strChatKey = GetChatKey(nChatId, pAccount);
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

    if (pConversation2)
    {
        pConversation2->RemoveChatParticipant(strChatContactName);
    }
}


void CConversationManager::NotifyChatUpdateUser(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strChatContactAlias, const int bIsContact)
{
    CString strChatKey = GetChatKey(nChatId, pAccount);

    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

    if (pConversation2)
    {
        pConversation2->UpdateChatParticipant(strChatContactName, strChatContactAlias, bIsContact);
    }
}


void CConversationManager::NotifyChatReceived(const int nChatId, IUOLMessengerAccountPtr pAccount, 
                                              const CString& strChatContactName, const CString& strText,
                                              const CString& strSmileyList)
{
	CString strChatKey = GetChatKey(nChatId, pAccount);
    
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);
    
    if (m_pFacade->GetPrivacyManager()->IsDenied(pAccount, strChatContactName))
	{
		ATLTRACE(_T("%s received message from denied contact %s\n"), pAccount->GetUserId(), strChatContactName);
		return;
	}
    
    // Launch message window
    IUOLMessengerUIManager4Ptr pUIManager;
    pUIManager = ap_dynamic_cast<IUOLMessengerUIManager4Ptr>(m_pFacade->GetUIManager());
    if (pUIManager)
    {
        pUIManager->LaunchMessageWindow(pAccount, nChatId, TRUE);
    }

	if (pConversation2)
	{
        pConversation2->AddReceivedMessage(strChatContactName, strText, CTime::GetCurrentTime(), strSmileyList);
    }
}


void CConversationManager::NotifyChatSent(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, const CString& strText)
{
	CString strChatKey = GetChatKey(nChatId, pAccount);
    
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

	if (pConversation2)
	{
		pConversation2->AddSentMessage(strText, CTime::GetCurrentTime());
	}
}


void CConversationManager::NotifyChatSendError(const int nChatId, IUOLMessengerAccountPtr pAccount, const CString& strChatContactName, IMProtocolError error)
{
	CString strChatKey = GetChatKey(nChatId, pAccount);
    
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

	if (pConversation2)
	{
        CString strMessage;

		if (error == IM_PROTOCOL_ERROR_RECEIPT_NOT_ONLINE)
        {
            strMessage.LoadString(IDS_MESSAGE_WINDOW_RECEIPT_NOT_ONLINE_IM_SEND_ERROR);
        }
        else if (error == IM_PROTOCOL_ERROR_SENDER_NOT_ONLINE)
        {
            strMessage.LoadString(IDS_MESSAGE_WINDOW_SENDER_NOT_ONLINE_IM_SEND_ERROR);
        }
        else
        {
            strMessage.LoadString(IDS_MESSAGE_WINDOW_GENERIC_IM_SEND_ERROR);
        }

        pConversation2->AddSystemMessage(strMessage, CTime::GetCurrentTime());
        
		if (error == IM_PROTOCOL_ERROR_UNKNOWN_SEND_ERROR)
		{
			// Message was posted, and now we need to disable the conference 
			// because it was already been terminated.
			//
			CAtlList<IUOLMessengerContact3Ptr> chatParticipantList;
			pConversation2->GetChatParticipantList(chatParticipantList);
			
			for (POSITION listPos = chatParticipantList.GetHeadPosition(); listPos; )
			{
				pConversation2->RemoveChatParticipant(
						chatParticipantList.GetNext(listPos)->GetName());
			}
		}
    }
}


RequestResult CConversationManager::NotifyRequestFileTransferAsync(IUOLMessengerAccountPtr pAccount, const CString& strContact, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
    IUOLMessengerConversationPtr pConversation = FindConversation(pAccount, strContact);
	IUOLMessengerConversation2Ptr pConversation2;
	
	ATLASSERT(m_pFacade);
	
	IUOLMessengerContactPtr pContact = m_pFacade->GetContactList()->FindContact(pAccount, strContact);
	
	if (!pContact)
	{
		pContact = m_pFacade->GetContactList()->CreateContact(pAccount, strContact);
	}
	
	if (!pConversation)
	{
		pConversation = CreateConversation(pContact);
	}
	
    if (pConversation)
	{
		pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
	}
	
	if (pConversation2)
	{
		// Launch message window.
		m_pFacade->GetUIManager()->LaunchMessageWindow(pContact, TRUE);
		
        pConversation2->AddRequestFileTransferAsync(pFileTransferAsyncInfo);
		
		return REQUEST_RESULT_OK;
    }
	
	return REQUEST_RESULT_ERROR;
}


BOOL CConversationManager::SendIM(IUOLMessengerConversationPtr pConversation, const CString& strMessage)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	if (pConversation)
	{
		CIMProtocol* pProtocol = pConversation->GetAccount()->GetProtocol();

		if (pProtocol)
		{
#ifdef _CONVERSATIONDEBUG
			CString strUserId = pConversation->GetAccount()->GetUserId();
			CString strContact = pConversation->GetContactName();
			CString strDebugMessage;

			strDebugMessage.Format(_T("  SendIM() - [Conv: 0x%x, From: %s To: %s\n  Message:%s]\n"), pConversation.get(), strUserId, strContact, strMessage);

			::OutputDebugString(strDebugMessage);
#endif

			error = pProtocol->SendIM((LPCTSTR) pConversation->GetAccount()->GetUserId(), (LPCTSTR) pConversation->GetContactName(), (LPCTSTR) strMessage);
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CConversationManager::SendTyping(IUOLMessengerConversationPtr pConversation)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	if (pConversation)
	{
		CIMProtocol* pProtocol = pConversation->GetAccount()->GetProtocol();

		if (pProtocol)
		{
			error = pProtocol->SendTyping((LPCTSTR) pConversation->GetAccount()->GetUserId(), (LPCTSTR) pConversation->GetContactName(), pConversation->GetTypingState());
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CConversationManager::SendConversationWindowClosed(IUOLMessengerConversationPtr pConversation)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	if (pConversation)
	{
		CIMProtocol* pProtocol = pConversation->GetAccount()->GetProtocol();

		if (pProtocol)
		{
			error = pProtocol->SendConversationWindowClosed((LPCTSTR) pConversation->GetAccount()->GetUserId(), (LPCTSTR) pConversation->GetContactName());
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


IUOLMessengerConversationPtr CConversationManager::CreateConversation(IUOLMessengerContactPtr pContact)
{
	CConversationMap::CPair* pPair = m_mapConversations.Lookup(pContact);

	if (pPair)
	{
		// Conversation already exists
		return pPair->m_value;
	}
	
	CConversationPtr pConversation = new CConversation(pContact);

	m_mapConversations.SetAt(pContact, pConversation);
	
	return pConversation;
}


IUOLMessengerConversation2Ptr CConversationManager::CreateConversation(const int nChatId, IUOLMessengerAccountPtr pAccount)
{
    CString strChatKey = GetChatKey(nChatId, pAccount);

    CChatConversationMap::CPair* pPair = m_mapChatConversations.Lookup(strChatKey);

	if (pPair)
	{
		// Conversation already exists
		return pPair->m_value;
	}
	
	CConversationPtr pConversation = new CConversation(nChatId, pAccount);

	m_mapChatConversations.SetAt(strChatKey, pConversation);
	
	return pConversation;
}


void CConversationManager::DestroyConversation(IUOLMessengerContactPtr pContact)
{
    CConversationMap::CPair* pPair = m_mapConversations.Lookup(pContact);

    ATLASSERT(pPair);
    if (pPair)
    {
        m_mapConversations.RemoveKey(pContact);
    }
}


void CConversationManager::DestroyConversation(const CString& strChatKey)
{
    CChatConversationMap::CPair* pPair = m_mapChatConversations.Lookup(strChatKey);

    ATLASSERT(pPair);
    if (pPair)
    {
        m_mapChatConversations.RemoveKey(strChatKey);
    }
}


IUOLMessengerConversationPtr CConversationManager::FindConversation(IUOLMessengerContactPtr pContact)
{
	CConversationMap::CPair* pPair = m_mapConversations.Lookup(pContact);

	if (pPair)
	{
		return pPair->m_value;
	}

	return IUOLMessengerConversationPtr();
}


IUOLMessengerConversation2Ptr CConversationManager::FindConversation(const CString& strChatKey)
{
	CChatConversationMap::CPair* pPair = m_mapChatConversations.Lookup(strChatKey);

	if (pPair)
	{
		return pPair->m_value;
	}

	return IUOLMessengerConversation2Ptr();
}


void CConversationManager::GetConversationList(CAtlList<IUOLMessengerConversationPtr>& listConversations)
{
	for (POSITION pos = m_mapConversations.GetStartPosition(); pos; )
	{
		listConversations.AddTail(m_mapConversations.GetNextValue(pos));
	}
}


void CConversationManager::RegisterObserver(CUOLMessengerConversationManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CConversationManager::UnregisterObserver(CUOLMessengerConversationManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


CConversationPtr CConversationManager::FindConversation(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	for (POSITION pos = m_mapConversations.GetStartPosition(); pos; )
	{
		CConversationPtr pNextConversation = m_mapConversations.GetNextValue(pos);

		IUOLMessengerConversation* pConv = dynamic_cast<IUOLMessengerConversation*>(pNextConversation.get());

		if ((0 == pConv->GetContactName().Compare(strContact)) &&
			(0 == pConv->GetAccount()->GetUserId().Compare(pAccount->GetUserId())) &&
			(0 == pConv->GetAccount()->GetProtocolId().Compare(pAccount->GetProtocolId())))
		{
			return pNextConversation;
		}
	}

	return CConversationPtr();
}


CString	CConversationManager::GetCustomEmoticonRootPath()
{
	CString strCustomEmoticonRootPath;
	
	IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();
	
	if (pProfileManager != NULL)
	{
		IUOLMessengerProfileElementPtr pProfileElement;
		
		pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);
		
		CString strProfilePathWithBackSlash = pProfileElement->GetProfilePath();
		
		if (strProfilePathWithBackSlash.Right(1) != '\\')
		{
			strProfilePathWithBackSlash += _T("\\");
		}
		
		strCustomEmoticonRootPath = strProfilePathWithBackSlash + "CustomEmoticons";
	}
	
	return strCustomEmoticonRootPath;
}

CString CConversationManager::GetChatKey(const int nChatId, IUOLMessengerAccountPtr pAccount)
{
    CString strChatKey;

    strChatKey = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetChatKey(nChatId, pAccount);
    
    return strChatKey;
}

BOOL CConversationManager::ChatSend(IUOLMessengerConversation2Ptr pConversation, const CString& strMessage)
{
    IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;

	if (pConversation)
	{
        CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pConversation->GetAccount()->GetProtocol());
        
		if (pProtocol5)
		{
            error = pProtocol5->ChatSend((LPCTSTR) pConversation->GetAccount()->GetUserId(), (LPCTSTR) strMessage, pConversation->GetChatId());
		}
	}

	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CConversationManager::IsChatParticipant(IUOLMessengerContactPtr pContact, const CString& strChatKey)
{
    if (strChatKey.IsEmpty())
    {
        return FALSE;
    }
    
    IUOLMessengerConversation2Ptr pConversation2;
    pConversation2 = FindConversation(strChatKey);

    if (pConversation2)
    {
        IUOLMessengerContactPtr pChatContact = pConversation2->FindChatParticipant(pContact->GetName());
        return (pChatContact != NULL);
    }

    return FALSE;
}


void CConversationManager::SetChatInvitedContact(const CString& strChatContactName)
{
    m_strChatInvitedContact = strChatContactName;
}


CString CConversationManager::GetChatInvitedContact()
{
    return m_strChatInvitedContact;
}


BOOL CConversationManager::AcceptFileAsync(
		IUOLMessengerConversation2Ptr pConversation, 
		CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;
	
	if (pConversation)
	{
		CIMProtocol5* pProtocol5 = 
				dynamic_cast<CIMProtocol5*>(pConversation->GetAccount()->GetProtocol());
		
		if (pProtocol5)
		{
			error = pProtocol5->AcceptFileAsync(
					(LPCTSTR) pConversation->GetAccount()->GetUserId(), 
					pFileTransferAsyncInfo);
		}
	}
	
	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


BOOL CConversationManager::RejectFileAsync(
		IUOLMessengerConversation2Ptr pConversation, 
		CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	IMProtocolError error = IM_PROTOCOL_ERROR_FAILED;
	
	if (pConversation)
	{
		CIMProtocol5* pProtocol5 = 
				dynamic_cast<CIMProtocol5*>(pConversation->GetAccount()->GetProtocol());
		
		if (pProtocol5)
		{
			error = pProtocol5->RejectFileAsync(
					(LPCTSTR) pConversation->GetAccount()->GetUserId(), 
					pFileTransferAsyncInfo);
		}
	}
	
	return (error == IM_PROTOCOL_ERROR_SUCCESS);
}


void CConversationManager::NotifyFileTransferStatus(CFileTransferPtr pFileTransfer)
{
	UOLLib::ISettings* pStringLoader = 
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			
	CString strMessage;
	CString strFormat;
	
	DWORD dwStatus = pFileTransfer->GetStatus();
	switch (dwStatus)
	{
	case FILE_TRANSFER_STATUS_ACCEPTED:
	case FILE_TRANSFER_STATUS_STARTED:
		strFormat = pStringLoader->GetValue("uol.resource.IDS_HISTORY_FILE_TRANSFER_ACCEPTED_MESSAGE_FORMAT");
		break;
	case FILE_TRANSFER_STATUS_CANCEL_REMOTE:
	case FILE_TRANSFER_STATUS_CANCEL_LOCAL:
		strFormat = pStringLoader->GetValue("uol.resource.IDS_HISTORY_FILE_TRANSFER_CANCELED_MESSAGE_FORMAT");
		break;
	case FILE_TRANSFER_STATUS_DONE:
		strFormat = pStringLoader->GetValue("uol.resource.IDS_HISTORY_FILE_TRANSFER_FINISHED_MESSAGE_FORMAT");
		break;
	}
	
	if (!strFormat.IsEmpty())
	{
		strMessage.Format(strFormat, pFileTransfer->GetFileName());
		IUOLMessengerConversationPtr pConversation;
		pConversation = FindConversation(pFileTransfer->GetAccount(), pFileTransfer->GetContactName());
		if (pConversation)
		{
			pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
		}
	}
}


void CConversationManager::NotifyFileTransferError(IUOLMessengerAccountPtr pAccount, const CString& strContact, int nError)
{
	UOLLib::ISettings* pStringLoader = 
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			
	CString strMessage;
	
	if (nError == FILE_TRANSFER_ERROR_UNSUPORTED)
	{
		strMessage = pStringLoader->GetValue("uol.resource.error.IDS_FILE_TRANSFER_ERROR_UNSUPORTED");
	}

	if (!strMessage.IsEmpty())
	{
		IUOLMessengerConversationPtr pConversation;
		pConversation = FindConversation(pAccount, strContact);
		if (pConversation)
		{
			pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());
		}
	}
}