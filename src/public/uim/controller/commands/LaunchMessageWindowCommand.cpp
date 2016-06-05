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
#include <commands/launchmessagewindowcommand.h>

#include "../UIMApplication.h"
#include "../../resource.h"


CLaunchMessageWindowCommand::CLaunchMessageWindowCommand(IUOLMessengerContactPtr pContact, BOOL bFlashWindow) :
    m_nChatId(-1),	
    m_pContact(pContact),
	m_bFlashWindow(bFlashWindow)
{
}


CLaunchMessageWindowCommand::CLaunchMessageWindowCommand(IUOLMessengerAccountPtr pAccount, const int& nChatId, BOOL bFlashWindow) :
    m_nChatId(nChatId),	
    m_pAccount(pAccount),
    m_bFlashWindow(bFlashWindow)
{
}


CLaunchMessageWindowCommand::~CLaunchMessageWindowCommand(void)
{
}


void CLaunchMessageWindowCommand::Execute()
{
	if (m_pContact)
	{
		BOOL bNewWindow = FALSE;
		BOOL bSomeWindowHasFocus = FALSE;

		IUOLMessengerConversationManagerPtr pConversationManager = CUIMApplication::GetApplication()->GetConversationManager();
		IUOLMessengerMessageWindowManagerPtr pMessageWindowManager = CUIMApplication::GetApplication()->GetUIManager()->GetMessageWindowManager();

		IUOLMessengerConversationPtr pConversation;
		IUOLMessengerMessageWindowPtr pMessageWindow = pMessageWindowManager->FindConversationMessageWindow(m_pContact, pConversation);

		CAtlList<IUOLMessengerMessageWindowPtr> listMessageWindows;
		pMessageWindowManager->GetMessageWindowList(listMessageWindows);
		
		for (POSITION listPos = listMessageWindows.GetHeadPosition(); listPos; )
		{
			IUOLMessengerMessageWindowPtr pMessageWindowItem = listMessageWindows.GetNext(listPos);
			
			if ( (pMessageWindowItem) && (pMessageWindowItem->HasFocus()) )
			{
				bSomeWindowHasFocus = TRUE;
				
				break;
			}
		}
		
		if (!pMessageWindow)
		{
			pMessageWindow = pMessageWindowManager->GetLastMessageWindow();

			if ((!pMessageWindow) || 
				(FALSE == pMessageWindow->CanAddMoreConversation()))
			{
				pMessageWindow = pMessageWindowManager->CreateMessageWindow();
				bNewWindow = TRUE;
			}

			pConversation = pConversationManager->CreateConversation(m_pContact);
			if (pMessageWindow != NULL)
			{
				pMessageWindow->AddConversation(pConversation);
			}
		}

		if ((pMessageWindow != NULL) &&
			(pConversation != NULL))
		{
			if (m_bFlashWindow)
			{
				if (bNewWindow)
				{
					if (bSomeWindowHasFocus)
					{
						IUOLMessengerMessageWindow3Ptr pMessageWindow3 = 
								ap_dynamic_cast<IUOLMessengerMessageWindow3Ptr>(
										pMessageWindow);
						
						pMessageWindow3->SetActiveConversation(pConversation, FALSE);
					}
					else
					{
						pMessageWindow->SetActiveConversation(pConversation);
					}
					pMessageWindow->Show(SW_SHOWMINNOACTIVE);
					pMessageWindow->Flash();
					pMessageWindow->FlashConversation(pConversation);
				}
				else
				{
					if (!pMessageWindow->IsVisible())
					{
						pMessageWindow->Show(SW_SHOWMINNOACTIVE);
					}

					if (!pMessageWindow->HasFocus())
					{
						pMessageWindow->Flash();
					}

					IUOLMessengerConversationPtr pActiveConversation = pMessageWindow->GetActiveConversation();

					if (pActiveConversation.get() != pConversation.get())
					{
						pMessageWindow->FlashConversation(pConversation);
					}
				}
			}
			else
			{
				if (bSomeWindowHasFocus)
				{
					IUOLMessengerMessageWindow3Ptr pMessageWindow3 = 
							ap_dynamic_cast<IUOLMessengerMessageWindow3Ptr>(
									pMessageWindow);
					
					pMessageWindow3->SetActiveConversation(pConversation, FALSE);
				}
				else
				{
					pMessageWindow->SetActiveConversation(pConversation);
				}
                pMessageWindow->Show();
                pMessageWindow->ShowInputFocus();
			}
		}
	}
    else if (m_pAccount)
    {
        BOOL bNewWindow = FALSE;

		IUOLMessengerConversationManager2Ptr pConversationManager = ap_dynamic_cast<IUOLMessengerConversationManager2Ptr>(CUIMApplication::GetApplication()->GetConversationManager());
		IUOLMessengerMessageWindowManager2Ptr pMessageWindowManager = ap_dynamic_cast<IUOLMessengerMessageWindowManager2Ptr>(CUIMApplication::GetApplication()->GetUIManager()->GetMessageWindowManager());

		CString strChatKey;
        strChatKey = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetChatKey(m_nChatId, m_pAccount);
        
        IUOLMessengerConversationPtr pConversation;
		IUOLMessengerMessageWindowPtr pMessageWindow = pMessageWindowManager->FindConversationMessageWindow(strChatKey, pConversation);

		if (!pMessageWindow)
		{
			pMessageWindow = pMessageWindowManager->GetLastMessageWindow();

			if ((!pMessageWindow) || 
				(FALSE == pMessageWindow->CanAddMoreConversation()))
			{
				pMessageWindow = pMessageWindowManager->CreateMessageWindow();
				bNewWindow = TRUE;
			}

			pConversation = pConversationManager->CreateConversation(m_nChatId, m_pAccount);
			if (pMessageWindow != NULL)
			{
				pMessageWindow->AddConversation(pConversation);
			}
		}

		if ((pMessageWindow != NULL) &&
			(pConversation != NULL))
		{
			if (m_bFlashWindow)
			{
				if (bNewWindow)
				{
					pMessageWindow->SetActiveConversation(pConversation);
					pMessageWindow->Show(SW_SHOWMINNOACTIVE);
					pMessageWindow->Flash();
					pMessageWindow->FlashConversation(pConversation);
				}
				else
				{
					if (!pMessageWindow->IsVisible())
					{
						pMessageWindow->Show(SW_SHOWMINNOACTIVE);
					}

					if (!pMessageWindow->HasFocus())
					{
						pMessageWindow->Flash();
					}

					IUOLMessengerConversationPtr pActiveConversation = pMessageWindow->GetActiveConversation();

					if (pActiveConversation.get() != pConversation.get())
					{
						pMessageWindow->FlashConversation(pConversation);
					}
				}
			}
			else
			{
				pMessageWindow->SetActiveConversation(pConversation);
                pMessageWindow->Show();
                pMessageWindow->ShowInputFocus();
			}
		}
    }
}
