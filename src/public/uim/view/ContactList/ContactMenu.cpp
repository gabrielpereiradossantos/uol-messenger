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
#include "ContactMenu.h"

#include "../../resource.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../../controller/HistoryRecordMessage.h"
#include <interfaces/IUOLMessengerGroup.h>
#include <commands/sendfilecommand.h>
#include <commands/launchinfowindowcommand.h>
#include <commands/BlockContactCommand.h>
#include <commands/UnblockContactCommand.h>


using namespace std;


CContactMenu::CContactMenu() :
	m_bBusy(FALSE)
{
}


CContactMenu::~CContactMenu()
{
}


IUOLMessengerContactPtr CContactMenu::GetContact()
{
	return m_pContact;
}


BOOL CContactMenu::IsBusy() const
{
	return m_bBusy;
}


void CContactMenu::CreateMenu(IUOLMessengerContactPtr pContact)
{
	if ((pContact != NULL) && (!m_bBusy))
	{
		m_pContact = pContact;

		CreateContextMenu();
	}
}


void CContactMenu::ShowMenu(const CPoint& ptPoint)
{
	if ((m_pMainMenu != NULL) && (!m_bBusy))
	{
		m_pMainMenu->ShowMenu(ptPoint);
	}
}


void CContactMenu::DestroyMenu()
{
	if (m_pMainMenu != NULL)
	{
		m_pMainMenu->DestroyMenu();
		m_pMainMenu.reset();
	}
}


CContextMenuPtr CContactMenu::GetContextMenu()
{
	return m_pMainMenu;
}


void CContactMenu::OnCommand(IUOLMessengerContextMenuItem* pItem)
{
	if (!m_pContact)
	{
		return;
	}

	m_bBusy = TRUE;

	CString strAlias;
	CString name;
	string m_protocol;
	CString account;

	IUOLMessengerAccountPtr pAccount = m_pContact->GetAccount();
	IUOLMessengerGroupPtr pGroup;

	IUOLMessengerCommandPtr pCommand;

	CString strMessage;
	CString strInputCaption, strInputLabel;

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strAll, strContact;

	if (pAccount)
	{
		switch (pItem->GetCommand())
		{
		case CC_REMOVE_CONTACT:
			{
				// Remove contact
				strMessage.Format(IDS_CONTACT_NODE_REMOVECONTACT_CONFIRM_MESSAGE_FORMAT, m_pContact->GetAlias());

				UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_YESNO | MB_ICONQUESTION);
				if (ret == IDYES)
				{
					BOOL bShowGroups = CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->IsShowGroupsEnabled();
					if (bShowGroups)
					{
						pGroup = m_pContact->GetGroup();
						CUIMApplication::GetApplication()->GetRemoteContactList()->RemoveContact(pAccount, m_pContact->GetName(), pGroup->GetName());
					}
					else
					{
						CAtlList<IUOLMessengerContactPtr> listContacts;
						CUIMApplication::GetApplication()->GetContactList()->FindContacts(pAccount->GetUserId(), pAccount->GetProtocolId(), m_pContact->GetName(), listContacts);

						ATLASSERT(listContacts.GetCount() > 0);
						for (POSITION pos = listContacts.GetHeadPosition(); pos; )
						{
							IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);
							pGroup = pContact->GetGroup();
							CUIMApplication::GetApplication()->GetRemoteContactList()->RemoveContact(pAccount, pContact->GetName(), pGroup->GetName());
						}
					}
				}
			}
			break;

		case CC_RENAME_CONTACT:
			// Rename contact			
			strAlias = m_pContact->GetAlias();

			strInputCaption.LoadString(IDS_RENAME_CONTACT_DIALOG_CAPTION);
			strInputLabel.LoadString(IDS_RENAME_CONTACT_DIALOG_LABEL);

			if (CUIMApplication::GetApplication()->GetUIManager()->ShowInputDlg(strInputCaption, strInputLabel, strAlias, IUOLMessengerContact::MAX_CONTACT_ALIAS_LENGTH, 0))
			{
				BOOL bAskUser = TRUE;
				strAlias.Trim();

				while ((bAskUser) && (strAlias.IsEmpty()))
				{
					strMessage = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_NODE_ERROR_NAME_INVALID");
					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);

					if (!CUIMApplication::GetApplication()->GetUIManager()->ShowInputDlg(strInputCaption, strInputLabel, strAlias, IUOLMessengerContact::MAX_CONTACT_ALIAS_LENGTH, 0))
					{
						bAskUser = FALSE;
					}
					else
					{
						strAlias.Trim();
					}
				}

				if (bAskUser && (!strAlias.IsEmpty()))
				{
					CUIMApplication::GetApplication()->GetRemoteContactList()->AliasContact(pAccount, m_pContact->GetName(), strAlias);

					m_pContact->SetAlias(strAlias);
					m_pContact->NotifyAllAliasChange();
				}
			}
			break;

		case CC_TURN_VISIBLE:
		case CC_TURN_INVISIBLE:
			// Visibility
			if (CUIMApplication::GetApplication()->GetPrivacyManager()->IsPermited(pAccount, m_pContact->GetName()))
			{
				CUIMApplication::GetApplication()->GetPrivacyManager()->RemovePermitedContact(pAccount, m_pContact->GetName());
			}
			else
			{
				CUIMApplication::GetApplication()->GetPrivacyManager()->AddPermitedContact(pAccount, m_pContact->GetName());
			}
			break;

		case CC_BLOCK_CONTACT:
			pCommand = new CBlockContactCommand(pAccount, m_pContact->GetName(), CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow());
			pCommand->Execute();
			break;

		case CC_UNBLOCK_CONTACT:
			pCommand = new CUnblockContactCommand(pAccount, m_pContact->GetName(), CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow());
			pCommand->Execute();
			break;

		case CC_SEND_MESSAGE:
			CUIMApplication::GetApplication()->GetUIManager()->LaunchMessageWindow(m_pContact, FALSE);
			break;

		case CC_SEND_FILE:
			// Send file
			CUIMApplication::GetApplication()->ExecuteCommand(new CSendFileCommand(m_pContact, CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow()));
			break;

		case CC_GET_INFO:
			// User info
			CUIMApplication::GetApplication()->ExecuteCommand(new CLaunchInfoWindowCommand(m_pContact, CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow()));
			break;

		case CC_REQUEST_AUTHORIZATION:
			// Request contact authorization
			CUIMApplication::GetApplication()->GetRemoteContactList()->RequestContactAuthorization(pAccount, m_pContact->GetName());
			break;

		case CC_SHOW_HISTORY:
			{
				CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
				if (pUIManager)
				{
					CString strCaption;
					CString strFormat;
					strFormat = pStringLoader->GetValue("uol.resource.IDS_HISTORY_WINDOW_MESSAGES_TITLE");
					strCaption.Format(strFormat, m_pContact->GetAlias());

					CHistoryRecordMessagePtr pHistoryRecordMessage;
					pHistoryRecordMessage = new CHistoryRecordMessage(pAccount, m_pContact->GetName());
					
					CHistoryRecordMessageListPtr pHistoryRecordMessageList;
					pHistoryRecordMessageList = new CHistoryRecordMessageList(pHistoryRecordMessage);
					pUIManager->ShowHistoryWindow(pHistoryRecordMessage, pHistoryRecordMessageList, strCaption);
				}
			}
			break;

		default:
			// (CC_LAST+)
			// Move contact
			pGroup = m_pContact->GetGroup();
			CUIMApplication::GetApplication()->GetRemoteContactList()->MoveContact(pAccount, m_pContact->GetName(), pGroup->GetName(), pItem->GetLabel());
			break;
		}
	}

	m_bBusy = FALSE;
}


void CContactMenu::CreateContextMenu()
{
	IUOLMessengerAccountPtr pAccount = m_pContact->GetAccount();

	if (!pAccount)
	{
		return;
	}

	CIMProtocol* pProtocol = pAccount->GetProtocol();

	if (!pProtocol)
	{
		return;
	}

	CString strMenuText;

	if (m_pMainMenu != NULL)
	{
		m_pMainMenu->DestroyMenu();
	}

	m_pMainMenu = new CContextMenu();

	CAtlList<IUOLMessengerGroupPtr> listGroups;
	IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();

	pContactList->GetGroupList(listGroups);

	if (m_pMainMenu->CreateMenu())
	{
		IUOLMessengerContextMenuItemPtr pMenuItem;

		strMenuText.LoadString(IDS_CONTACT_NODE_SENDMESSAGE_CAPTION);
		pMenuItem = new CContextMenuItem(CC_SEND_MESSAGE, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);

		if (pProtocol->HasFileTransferSupport())
		{
			strMenuText.LoadString(IDS_CONTACT_NODE_SENDFILE_CAPTION);
			pMenuItem = new CContextMenuItem(CC_SEND_FILE, strMenuText, this);
			m_pMainMenu->AddItem(pMenuItem);
		}

		m_pMainMenu->AddSeparator();

        strMenuText.LoadString(IDS_CONTACT_NODE_INFO_CAPTION);
		pMenuItem = new CContextMenuItem(CC_GET_INFO, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);

        strMenuText.LoadString(IDS_CONTACT_NODE_HISTORY_CAPTION);
		pMenuItem = new CContextMenuItem(CC_SHOW_HISTORY, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);

		// Block/unblock
		if (pProtocol->HasContactBlockingSupport())
		{
			m_pMainMenu->AddSeparator();

			if (!CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(pAccount, m_pContact->GetName()))
			{
				strMenuText.LoadString(IDS_CONTACT_NODE_BLOCKCONTACT_CAPTION);

				pMenuItem = new CContextMenuItem(CC_BLOCK_CONTACT, strMenuText, this);
				m_pMainMenu->AddItem(pMenuItem);
			}
			else
			{
				strMenuText.LoadString(IDS_CONTACT_NODE_UNBLOCKCONTACT_CAPTION);

				pMenuItem = new CContextMenuItem(CC_UNBLOCK_CONTACT, strMenuText, this);
				m_pMainMenu->AddItem(pMenuItem);
			}

			if (!(pProtocol->GetPrivacyFlags() & PRIVACY_FLAGS_NO_LIST_INTERSECTION))
			{
				if (!CUIMApplication::GetApplication()->GetPrivacyManager()->IsPermited(pAccount, m_pContact->GetName()))
				{
					strMenuText.LoadString(IDS_CONTACT_NODE_INVISIBLE_CAPTION);

					pMenuItem = new CContextMenuItem(CC_TURN_INVISIBLE, strMenuText, this);
					m_pMainMenu->AddItem(pMenuItem);
				}
				else
				{
					strMenuText.LoadString(IDS_CONTACT_NODE_VISIBLE_CAPTION);

					pMenuItem = new CContextMenuItem(CC_TURN_VISIBLE, strMenuText, this);
					m_pMainMenu->AddItem(pMenuItem);
				}
			}
		}

		m_pMainMenu->AddSeparator();

		IUOLMessengerContact3Ptr pContact3 = 
				ap_dynamic_cast<IUOLMessengerContact3Ptr>(m_pContact);
		
		if ((pProtocol->HasReRequestAuthorizationSupport()) && 
				(pContact3) && (pContact3->CanRequestAuthorization()))
		{
			strMenuText.LoadString(IDS_CONTACT_NODE_REQUEST_AUTHORIZATION);

			pMenuItem = new CContextMenuItem(CC_REQUEST_AUTHORIZATION, strMenuText, this);
			m_pMainMenu->AddItem(pMenuItem);

			m_pMainMenu->AddSeparator();
		}

        strMenuText.LoadString(IDS_CONTACT_NODE_RENAMECONTACT_CAPTION);
		pMenuItem = new CContextMenuItem(CC_RENAME_CONTACT, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);

		// Move to group...
		int nItemIndex = CC_LAST + 100;

		BOOL bShowGroups = CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->IsShowGroupsEnabled();;

		IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
				ap_static_cast<IUOLMessengerContactListSettings2Ptr>(
						CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
		
		if ( (bShowGroups) && (listGroups.GetCount() > 1) && (!pContactListSettings2->GetGroupByAccounts()) )
		{
			strMenuText.LoadString(IDS_CONTACT_NODE_MOVECONTACT_CAPTION);
			CContextMenuPtr pMoveMenu = new CContextMenu(strMenuText);

			if (pMoveMenu->CreateMenu())
			{
				m_pMainMenu->AddItem(pMoveMenu);

				IUOLMessengerGroupPtr pCurrentGroup = m_pContact->GetGroup();

				if (pCurrentGroup != NULL)
				{
					CString strCurrentGroup = pCurrentGroup->GetName();

					IUOLMessengerGroupPtr pGroup;
					IUOLMessengerContactPtr pContact;
					CString strGroup;
					for (POSITION pos = listGroups.GetHeadPosition(); pos;)
					{
						pGroup = listGroups.GetNext(pos);

						pContact = pGroup->GetContact(m_pContact->GetAccount(), m_pContact->GetName());
						if (!pContact)
						{
							pMenuItem = new CContextMenuItem(++nItemIndex, pGroup->GetName(), this);
							pMoveMenu->AddItem(pMenuItem);
						}
					}
				}
			}
		}

        strMenuText.LoadString(IDS_CONTACT_NODE_REMOVECONTACT_CAPTION);
		pMenuItem = new CContextMenuItem(CC_REMOVE_CONTACT, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);
	}
}

void CContactMenu::AddItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens)
{
	for (POSITION pos = listItens.GetHeadPosition(); pos;)
	{
		IUOLMessengerContextMenuItemPtr pMenuItem = listItens.GetNext(pos);
		
		if (pMenuItem)
		{
			m_pMainMenu->AddItem(pMenuItem);
		}
	}
}

void CContactMenu::AddSeparator()
{
	m_pMainMenu->AddSeparator();
}