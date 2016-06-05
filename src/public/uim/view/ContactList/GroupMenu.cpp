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
#include "GroupMenu.h"

#include "../../resource.h"
#include "../InputDialog.h"
#include "../../controller/UIMApplication.h"


CGroupMenu::CGroupMenu() : 
m_bBusy(FALSE)
{
}


CGroupMenu::~CGroupMenu()
{
}


IUOLMessengerGroupPtr CGroupMenu::GetGroup()
{
	return m_pGroup;
}


BOOL CGroupMenu::IsBusy() const
{
	return m_bBusy;
}


void CGroupMenu::CreateMenu(IUOLMessengerGroupPtr pGroup)
{
	if ((pGroup != NULL) && (!m_bBusy))
	{
		m_pGroup = pGroup;

		CreateContextMenu();
	}
}


void CGroupMenu::ShowMenu(const CPoint& ptPoint)
{
	if ((m_pMainMenu != NULL) && (!m_bBusy))
	{
		m_pMainMenu->ShowMenu(ptPoint);
	}
}


void CGroupMenu::DestroyMenu()
{
	if (m_pMainMenu != NULL)
	{
		m_pMainMenu->DestroyMenu();
		m_pMainMenu.reset();
	}
}


void CGroupMenu::OnCommand(IUOLMessengerContextMenuItem* pItem)
{
	if (!m_pGroup)
	{
		return;
	}

	m_bBusy = TRUE;

	IUOLMessengerFacade* pApp = CUIMApplication::GetApplication();
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strNewName;

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	IUOLMessengerAccountPtr pAccount;

	CString strName = m_pGroup->GetName();

	CString strCaption, strMessage;

	if (GC_REMOVE_GROUP == pItem->GetCommand())
	{
		// Remove group
		if (m_pGroup->GetContactCount() != 0)
		{
			CString strFormat = pStringLoader->GetValue("uol.resource.error.IDS_GROUP_NODE_ERROR_NO_EMPTY_GROUP");
			strMessage.Format(strFormat, strName);
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
		}
		else
		{
			strMessage.Format(IDS_GROUP_NODE_QUESTION_REMOVE_GROUP, strName);

			if (CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				IUOLMessengerRemoteContactList2Ptr pIUOLMessengerRemoteContactList2 = 
					ap_dynamic_cast<IUOLMessengerRemoteContactList2Ptr>(
						CUIMApplication::GetApplication()->GetRemoteContactList()
																		);

				ATLASSERT(pIUOLMessengerRemoteContactList2.get());

				if (pIUOLMessengerRemoteContactList2.get())
				{
					IMProtocolError imProtocolError;
					
					BOOL bRemoteRemoved = pIUOLMessengerRemoteContactList2->RemoveGroup(strName, imProtocolError);

					if (FALSE == bRemoteRemoved)
					{
						if (imProtocolError == IM_PROTOCOL_ERROR_GROUP_NOT_FOUND)
						{
							pApp->GetContactList()->RemoveGroup(strName);
						}
						else
						{					
							ATLASSERT(FALSE);
						}
					}							
				}
			}
		}
	}
	else if (GC_RENAME_GROUP == pItem->GetCommand())
	{
		// Rename group
		strCaption.LoadString(IDS_GROUP_NODE_RENAME_GROUP_TITLE);
		strMessage.LoadString(IDS_GROUP_NODE_RENAME_GROUP_LABEL);
		strNewName = strName;

		do
		{
			if (CUIMApplication::GetApplication()->GetUIManager()->ShowInputDlg(strCaption, strMessage, strNewName, IUOLMessengerGroup::MAX_GROUP_NAME_LENGTH))
			{
				strNewName.Trim();

				if (strNewName.GetLength() > 15)
				{
					strMessage = pStringLoader->GetValue("uol.resource.error.IDS_GROUP_NODE_ERROR_LIMIT_NAME");

					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
				}
				else if (!strNewName.IsEmpty())
				{
					IUOLMessengerGroupPtr pNewGroup = pApp->GetContactList()->FindGroup(strNewName);

					if (!pNewGroup)
					{
						BOOL bRemoteRenamed = CUIMApplication::GetApplication()->GetRemoteContactList()->RenameGroup(strName, strNewName);

						ATLASSERT(bRemoteRenamed);
						//if (!bRemoteRenamed)
						//{
						//	// Local list rename
						//	pApp->GetContactList()->RenameGroup(strName, strNewName);
						//}
					}
					else
					{
						CString strFormat = pStringLoader->GetValue("uol.resource.error.IDS_GROUP_NODE_ERROR_GROUP_EXISTS");
						strMessage.Format(strFormat, strNewName);
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);

						strNewName.Empty();
					}
				}
				else
				{
					strMessage = pStringLoader->GetValue("uol.resource.error.IDS_GROUP_MENU_EMPTY_NEW_NAME_MESSAGE");
					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
				}
			}
			else
			{
				break;
			}
		}
		while (strNewName.IsEmpty());
	}

	m_bBusy = FALSE;
}


void CGroupMenu::CreateContextMenu()
{
	CString strMenuText;

	if (m_pMainMenu != NULL)
	{
		m_pMainMenu->DestroyMenu();
	}

	m_pMainMenu = new CContextMenu();

	if (m_pMainMenu->CreateMenu())
	{
		IUOLMessengerContextMenuItemPtr pMenuItem;

		strMenuText.LoadString(IDS_GROUP_NODE_RENAMEGROUP_CAPTION);
		pMenuItem = new CContextMenuItem(GC_RENAME_GROUP, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);

		strMenuText.LoadString(IDS_GROUP_NODE_REMOVEGROUP_CAPTION);
		pMenuItem = new CContextMenuItem(GC_REMOVE_GROUP, strMenuText, this);
		m_pMainMenu->AddItem(pMenuItem);
	}
}