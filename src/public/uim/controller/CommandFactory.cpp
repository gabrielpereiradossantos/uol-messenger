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
#include ".\CommandFactory.h"
#include ".\UIMApplication.h"


#include "../resource.h"

#include <commands/ShowPreferencesCommand.h>
#include <commands/CloseAppCommand.h>
#include <commands/ShowGroupDialogCommand.h>
#include <commands/ShowPreferencesCommand.h>
#include <commands/ShowHelpCommand.h>
#include <commands/OpenBugReportCommand.h>
#include <commands/ShowAboutDialogCommand.h>
#include <commands/ConnectAllAccountsCommand.h>
#include <commands/DisconnectAllAccountsCommand.h>
#include <commands/ShowContactDialogCommand.h>
#include <commands/SetAwayStatusAllCommand.h>
#include <commands/SetOfflineStatusAllCommand.h>
#include <commands/SetOnlineStatusAllCommand.h>
#include <commands/SetBusyStatusAllCommand.h>
#include <commands/OpenBrowserCommand.h>
#include <commands/OpenBrowserAuthenticatedCommand.h>
#include <commands/RestartAppCommand.h>
#include <commands/ShowDiagnoseToolCommand.h>
#include <commands/ShowFileTransfersDialogCommand.h>
#include <commands/ShowHotKeysDialogCommand.h>


CUOLCommandFactory::CUOLCommandFactory(void)
{
}

CUOLCommandFactory::~CUOLCommandFactory(void)
{
}


IUOLMessengerCommandPtr CUOLCommandFactory::GetCommand(const CString& strCommand)
{
	IUOLMessengerCommandPtr pCommand;

	if (strCommand.CompareNoCase("ShowAddGroupDialog") == 0)
	{
		pCommand = new CShowGroupDialogCommand();
	}
	else if (strCommand.CompareNoCase("OpenPreferences") == 0)
	{
		pCommand = new CShowPreferencesCommand();
	}
	else if (strCommand.CompareNoCase("ShowHelp") == 0)
	{
		pCommand = new CShowHelpCommand();
	}
	else if (strCommand.CompareNoCase("OpenBugReport") == 0)
	{
		CWindow mainWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();
		pCommand = new COpenBugReportCommand(mainWindow);
	}	
	else if (strCommand.CompareNoCase("CloseApp") == 0)
	{
		pCommand = new CCloseAppCommand();
	}
	else if (strCommand.CompareNoCase("ShowAboutDialog") == 0)
	{
		pCommand = new CShowAboutDialogCommand();
	}
	else if (strCommand.CompareNoCase("ConnectAllAccounts") == 0)
	{
		pCommand = new CConnectAllAccountsCommand();
	}
	else if (strCommand.CompareNoCase("DisconnectAllAccounts") == 0)
	{
		pCommand = new CDisconnectAllAccountsCommand();
	}
	else if (strCommand.CompareNoCase("ShowAddContactDialog") == 0)
	{
		pCommand = new CShowContactDialogCommand();
	}
	else if (strCommand.CompareNoCase("ShowFileTransfersDialog") == 0)
	{
		pCommand = new CShowFileTransfersDialogCommand();
	}
	else if (strCommand.CompareNoCase("SetAwayStatusAll") == 0)
	{
		pCommand = new CSetAwayStatusAllCommand();
	}
	else if (strCommand.CompareNoCase("SetBusyStatusAll") == 0)
	{
		pCommand = new CSetBusyStatusAllCommand();
	}
	else if (strCommand.CompareNoCase("SetOfflineStatusAll") == 0)
	{
		pCommand = new CSetOfflineStatusAllCommand();
	}
	else if (strCommand.CompareNoCase("SetOnlineStatusAll") == 0)
	{
		pCommand = new CSetOnlineStatusAllCommand();
	}
	else if (strCommand.CompareNoCase("OpenBrowser") == 0)
	{
		pCommand = new COpenBrowserCommand();
	}
	else if (strCommand.CompareNoCase("OpenBrowserAuthenticated") == 0)
	{
		pCommand = new COpenBrowserAuthenticatedCommand();
	}
	else if (strCommand.CompareNoCase("OpenUOLHome") == 0)
	{
		CString strURL;
		strURL.LoadString(IDS_UOL_BUTTON_URL);

		return new COpenBrowserAuthenticatedCommand(strURL);
	}
	else if (strCommand.CompareNoCase("OpenUOLKut") == 0)
	{
		CString strURL;
		strURL.LoadString(IDS_UOLKUT_BUTTON_URL);

		return new COpenBrowserAuthenticatedCommand(strURL);
	}
	else if (strCommand.CompareNoCase("RestartApp") == 0)
	{
		pCommand = new CRestartAppCommand();
	}
	else if (strCommand.CompareNoCase("ShowDiagnoseTool") == 0)
	{
		pCommand = new CShowDiagnoseToolCommand();
	}
	else if (strCommand.CompareNoCase("ShowHotKeysDialog") == 0)
	{
		pCommand = new CShowHotKeysDialogCommand();
	}

	return pCommand;
}
