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
#include <commands/changeavatarcommand.h>
#include <interfaces/IUOLMessengerAccount.h>

#include "../../controller/uimapplication.h"
#include "../../resource.h"

#include "MinDependencySpecialFolder.h"
#include "../../view/UIMFileDialog.h"

CChangeAvatarCommand::CChangeAvatarCommand(CWindow wnd, IUOLMessengerAccountPtr pAccount) : 
	m_wnd(wnd),
	m_pAccount(pAccount)
{
}

CChangeAvatarCommand::~CChangeAvatarCommand(void)
{
}

void CChangeAvatarCommand::Execute()
{

	// Init file dialog...
	CMinDependencySpecialFolder mdsf;
	CString strUserDir = mdsf.GetPersonal();

	TCHAR szFilter[MAX_PATH + 1] = "Arquivos de Imagem (*.bmp;*.gif;*.jpeg;*.jpg)|*.bmp;*.gif;*.jpeg;*.jpg|";

	CWindow tmpWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();
    CUIMFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, tmpWindow.m_hWnd);
	fileDialog.m_ofn.lpstrInitialDir = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetLastAvatarFolder();
	
	// Request image path
	if ((IDOK == fileDialog.DoModal()) && (fileDialog.m_szFileName != NULL))
	{
		// Try load image
		CString strFilePath = fileDialog.m_szFileName;

		CString strFileFolder = strFilePath.Left(strFilePath.ReverseFind('\\'));
		CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetLastAvatarFolder(strFileFolder);

		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

		if (m_pAccount.get() == NULL)
		{
			CAtlList<IUOLMessengerAccountPtr> listAccounts;
			CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(listAccounts);

			for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
			{
				IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

				IUOLMessengerUserIconPtr pUserIcon = CUIMApplication::GetApplication()->GetUserIconCacheManager()->CacheIcon(pAccount.get(), strFilePath);

				if (pUserIcon != NULL)
				{
					if ((pAccount != NULL))
					{
						pAccount->SetUserIcon(pUserIcon);

						if (pAccountConnectionManager->IsConnected(pAccount))
						{
							pAccountConnectionManager->UploadDisplayIcon(pAccount);
						}
					}
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogWarning(__FUNCTION__, 
							"O arquivo [%s] é inválido. Somente os formatos (*.bmp;*.gif;*.jpeg;*.jpg) são aceitos.", 
							strFilePath);
					
					// Invalid file!
					UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
					CString strMessage;
					strMessage = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_INVALID_FILE_MSG");

					CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_wnd, strMessage, MB_OK | MB_ICONERROR);
				}
			}
		}
		else
		{
			IUOLMessengerUserIconPtr pUserIcon = CUIMApplication::GetApplication()->GetUserIconCacheManager()->CacheIcon(m_pAccount.get(), strFilePath);

			if (pUserIcon != NULL)
			{
				m_pAccount->SetUserIcon(pUserIcon);

				if (pAccountConnectionManager->IsConnected(m_pAccount))
				{
					pAccountConnectionManager->UploadDisplayIcon(m_pAccount);
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogWarning(__FUNCTION__, 
						"O arquivo [%s] é inválido. Somente os formatos (*.bmp;*.gif;*.jpeg;*.jpg) são aceitos.", 
						strFilePath);
				
				// Invalid file!
				UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
				CString strMessage;
				strMessage = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_INVALID_FILE_MSG");

				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_wnd, strMessage, MB_OK | MB_ICONERROR);
			}
		}
	}
}