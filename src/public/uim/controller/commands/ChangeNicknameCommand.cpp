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
#include <commands/changenicknamecommand.h>
#include <interfaces/IUOLMessengerAccount.h>

#include "../../controller/UIManager.h"
#include "../../controller/uimapplication.h"

#include "../../view/InputDialog.h"


CChangeNicknameCommand::CChangeNicknameCommand(CWindow wnd, CString oldNickname, IUOLMessengerAccountPtr pAccount) : 
	m_wnd(wnd),
	m_pAccount(pAccount),
	m_OldNickname(oldNickname)
{
}

CChangeNicknameCommand::~CChangeNicknameCommand(void)
{
}

void CChangeNicknameCommand::Execute()
{
	CString strCaption;
	CString strMessage;
	CString strNickname;
	
	strCaption.LoadString(IDS_CHANGE_NICKNAME_TITLE);
	strMessage.LoadString(IDS_CHANGE_NICKNAME_LABEL);
		
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
    ATLASSERT(pUIManager);

    CInputDialogPtr pdlg = pUIManager->GetInputDialog();
	pdlg->Initialize(strCaption, strMessage, IUOLMessengerContact::MAX_CONTACT_ALIAS_LENGTH, 0, m_OldNickname);

	if (pdlg)
	{
		strNickname.Empty();

		if (IDOK == pdlg->DoModal(m_wnd))
		{
			strNickname = pdlg->GetText();
			strNickname.Trim();

			if (m_pAccount.get() == NULL)
			{
				CAtlList<IUOLMessengerAccountPtr> listAccounts;
				CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(listAccounts);

				IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

				for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
				{
					IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

					if ((pAccount != NULL))
					{
						pAccount->SetNickName(strNickname);

						if (pAccountConnectionManager->IsConnected(pAccount))
						{
							CUIMApplication::GetApplication()->GetAccountConnectionManager()->UpdateServerAlias(pAccount);
						}
					}
				}
			}
			else
			{
				if (m_pAccount.get() != NULL)
				{
					m_pAccount->SetNickName(strNickname);

					if (CUIMApplication::GetApplication()->GetAccountConnectionManager()->IsConnected(m_pAccount))
					{
						CUIMApplication::GetApplication()->GetAccountConnectionManager()->UpdateServerAlias(m_pAccount);
					}
				}
			}
		}
	}
}