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

class IUOLMessengerFacade;
          
#include "IUOLMessengerHistoryManager.h"
#include "IUOLMessengerSettingsManager.h"
#include "IUOLMessengerAccountManager.h"
#include "IUOLMessengerProtocolManager.h"
#include "IUOLMessengerUserIconCacheManager.h"
#include "IUOLMessengerUIManager.h"
#include "IUOLMessengerThemeManager.h"
#include "IUOLMessengerProfileManager.h"
#include "IUOLMessengerCommandFactory.h"
#include "IUOLMessengerCommand.h"
#include "IUOLMessengerContactList.h"
#include "IUOLMessengerFileTransferManager.h"
#include "IUOLMessengerTrayWindow.h"
#include "IUOLMessengerNotifyPopUpManager.h"
#include "IUOLMessengerAuthenticationManager.h"
#include "IUOLMessengerMessageReceivingFilter.h"
#include "IUOLMessengerInternetConnectionMonitor.h"
#include "IUOLMessengerAccountConnectionManager.h"
#include "IUOLMessengerRemoteContactList.h"
#include "IUOLMessengerPrivacyManager.h"
#include "IUOLMessengerConversationManager.h"
#include "IUOLMessengerCommonSettings.h"
#include "IUOLMessengerPropertyPageCommandFactory.h"
#include "IUOLMessengerMessageTemplateManager.h"
#include "IUOLMessengerLogger.h"
#include "IUOLMessengerPluginPropertiesRegister.h"
#include "IUOLMessengerLoggingManager.h"
#include "IUOLMessengerDiagnoseManager.h"


class __declspec(novtable) IUOLMessengerFacade
{
public:
	virtual ~IUOLMessengerFacade(void) { }

	virtual void ExecuteCommand(IUOLMessengerCommandPtr pCommand) = 0;

	virtual void GetInstallCookie(GUID& cookie) const = 0;
	
	virtual IUOLMessengerHistoryManagerPtr  GetHistoryManager() = 0;
	virtual IUOLMessengerSettingsManagerPtr GetSettingsManager() = 0;
	virtual IUOLMessengerAccountManagerPtr	GetAccountManager() = 0;
	virtual IUOLMessengerProtocolManagerPtr	GetProtocolManager() = 0;	
	virtual IUOLMessengerUserIconCacheManagerPtr GetUserIconCacheManager() = 0;
	virtual IUOLMessengerUIManagerPtr	GetUIManager() = 0;

	// Deprecated method.
	// - Use IUOLMessengerUIManager3::GetCurrentThemeProperties() to retrieve theme attributes.
	// - Use IUOLMessengerSettingsManager2::GetSoundSettings() to change sound settings.
	// - Use IUOLMessengerSettingsManager2::GetEmoticonSettings() to change emoticon settings.
	virtual IUOLMessengerThemeManagerPtr GetThemeManager() = 0;

	virtual IUOLMessengerProfileManagerPtr GetProfileManager() = 0;
	virtual IUOLMessengerCommandFactoryPtr GetCommandFactory() = 0;
	virtual IUOLMessengerContactListPtr GetContactList() = 0;
	virtual IUOLMessengerFileTransferManagerPtr GetFileTransferManager() = 0;
	virtual IUOLMessengerTrayWindowPtr GetTrayWindow() = 0;
	virtual IUOLMessengerNotifyPopUpManagerPtr GetNotifyPopUpManager() = 0;
	virtual IUOLMessengerAuthenticationManagerPtr GetAuthenticationManager() = 0;
	virtual IUOLMessengerMessageReceivingFilterPtr GetMessageReceivingFilter() = 0;
	virtual IUOLMessengerInternetConnectionMonitorPtr GetInternetConnectionMonitor() = 0;
	virtual IUOLMessengerAccountConnectionManagerPtr GetAccountConnectionManager() = 0;
	virtual IUOLMessengerRemoteContactListPtr GetRemoteContactList() = 0;
	virtual IUOLMessengerPrivacyManagerPtr GetPrivacyManager() = 0;
	virtual IUOLMessengerConversationManagerPtr GetConversationManager() = 0;

	// Message loop filters
	virtual BOOL AddMessageFilter(CMessageFilter* pMessageFilter) = 0;
	virtual BOOL RemoveMessageFilter(CMessageFilter* pMessageFilter) = 0;
	virtual BOOL AddIdleHandler(CIdleHandler* pIdleHandler) = 0;
	virtual BOOL RemoveIdleHandler(CIdleHandler* pIdleHandler) = 0;
};



class __declspec(novtable) IUOLMessengerFacade2 : public IUOLMessengerFacade
{
public:
	virtual ~IUOLMessengerFacade2() {}

	virtual IUOLMessengerCommonSettingsPtr GetCommonSettings() = 0;
	virtual IUOLMessengerPropertyPageCommandFactoryPtr GetPropertyPageCommandFactory() = 0;
};


class __declspec(novtable) IUOLMessengerFacade3 : public IUOLMessengerFacade2
{
public:
	virtual ~IUOLMessengerFacade3() {}
	
	virtual void PreFinalize() = 0;
	virtual DWORD GetApplicationThreadId() = 0;

	virtual void ShakeWindow(HWND hWnd, int nRepetitions = 30, DWORD dwDelay = 25) = 0;
	virtual IUOLMessengerMessageTemplateManagerPtr GetMessageTemplateManager() = 0;
	virtual void PlayNudgeSound(IUOLMessengerContactPtr pContact) = 0;
	
	virtual IUOLMessengerLoggerPtr GetApplicationLogger() = 0;
};


class __declspec(novtable) IUOLMessengerFacade4 : public IUOLMessengerFacade3
{
public:
	virtual ~IUOLMessengerFacade4() {}
	
	virtual IUOLMessengerPluginPropertiesRegisterPtr GetPluginPropertiesRegister() = 0;
	virtual IUOLMessengerLoggingManagerPtr GetLoggingManager() = 0;
	virtual IUOLMessengerDiagnoseManagerPtr GetDiagnoseManager() = 0;
};


