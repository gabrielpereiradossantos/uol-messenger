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

#include <interfaces/IUOLMessengerFacade.h>
#include <interfaces/IUOLMessengerAccount.h>

#include "utils.h"
#include "Jobthread.h"

#include "UOLMessengerPluginManager.h"
#include "Message/MessageQueueController.h"
#include "diskcache/IDiskCacheManager.h"
#include "ProtocolCallback/ProtocolCallbackController.h"
#include "PropertyPageCommandFactory.h"
#include "./MessageTemplates/MessageTemplateManager.h"
#include "../view/UIMInitProgressWnd.h"
#include "../UIMGlobalLogger.h"
#include "../PurpleLogger.h"
#include "./PluginPropertiesRegister.h"
#include "./LoggingManager.h"
#include "DiagnoseManager.h"
#include "./MethodAuthorizationManager.h"



class CUIMApplication :
	public IUOLMessengerFacade4,
	public IShutdownFlag,
	public ILiveUpdateFlag
{
public:
	enum APPLICATION_INIT_ERROR
	{
		AIE_ERROR_SUCCESS = 0,
		AIE_ERROR_FAILED = 1,
		AIE_ERROR_RESTART = 2
	};

public:
	virtual ~CUIMApplication(void);	

	static IUOLMessengerFacade3* AllocApplication();
	static void ReleaseApplication();
	static IUOLMessengerFacade3* GetApplication();

	DWORD Init(CString strCmdLine);
    void PreFinalize();
	void Finalize();
	int Run();

	// ILiveUpdateFlag interface
	void SetLiveUpdateFlag(LONG bState)
	{
		::InterlockedExchange(&m_bLiveUpdateAvailable, bState);
	}

	LONG GetLiveUpdateFlag() const
	{
		return m_bLiveUpdateAvailable;
	}

	LONG KillReceived() const
	{
		return m_bKillReceived;
	}
	
	// A plugin can't get CUOLMessengerPluginManager
	virtual CUOLMessengerPluginManager* GetPluginManager();

	virtual UOLLib::ISettings* GetStringLoader();

	virtual CMessageQueueController* GetMessageQueueController();
	virtual IDiskCacheManagerPtr GetDiskCacheManager();
    virtual CString GetChatKey(const int nChatId, IUOLMessengerAccountPtr pAccount);
	
	CUIMGlobalLoggerPtr	GetLogger();
	CPurpleLoggerPtr	GetPurpleLogger();

	void SetProgressWindowVisible(BOOL bVisible);
	
	CMethodAuthorizationManagerPtr GetMethodAuthorizationManager();
	
protected:
	virtual void ExecuteCommand(IUOLMessengerCommandPtr pCommand);

	virtual void GetInstallCookie(GUID& cookie) const;

	virtual IUOLMessengerSettingsManagerPtr GetSettingsManager();
	virtual IUOLMessengerAccountManagerPtr	GetAccountManager();
	virtual IUOLMessengerProtocolManagerPtr	GetProtocolManager();
	virtual IUOLMessengerUserIconCacheManagerPtr GetUserIconCacheManager();
	virtual IUOLMessengerUIManagerPtr	GetUIManager();
	virtual IUOLMessengerThemeManagerPtr GetThemeManager();
	virtual IUOLMessengerProfileManagerPtr GetProfileManager();
	virtual IUOLMessengerHistoryManagerPtr GetHistoryManager();
	virtual IUOLMessengerCommandFactoryPtr GetCommandFactory();
	virtual IUOLMessengerContactListPtr GetContactList();
	virtual IUOLMessengerFileTransferManagerPtr GetFileTransferManager();
	virtual IUOLMessengerTrayWindowPtr GetTrayWindow();
	virtual IUOLMessengerNotifyPopUpManagerPtr GetNotifyPopUpManager();
	virtual IUOLMessengerAuthenticationManagerPtr GetAuthenticationManager();
	virtual IUOLMessengerMessageReceivingFilterPtr GetMessageReceivingFilter();
	virtual IUOLMessengerInternetConnectionMonitorPtr GetInternetConnectionMonitor();
	virtual IUOLMessengerAccountConnectionManagerPtr GetAccountConnectionManager();
	virtual IUOLMessengerRemoteContactListPtr GetRemoteContactList();
	virtual IUOLMessengerPrivacyManagerPtr GetPrivacyManager();
	virtual IUOLMessengerConversationManagerPtr GetConversationManager();

	virtual BOOL AddMessageFilter(CMessageFilter* pMessageFilter);
	virtual BOOL RemoveMessageFilter(CMessageFilter* pMessageFilter);
	virtual BOOL AddIdleHandler(CIdleHandler* pIdleHandler);
	virtual BOOL RemoveIdleHandler(CIdleHandler* pIdleHandler);	

	virtual IUOLMessengerCommonSettingsPtr GetCommonSettings();
	
	virtual IUOLMessengerPropertyPageCommandFactoryPtr GetPropertyPageCommandFactory();
	
	virtual DWORD GetApplicationThreadId();
	
	virtual IUOLMessengerMessageTemplateManagerPtr GetMessageTemplateManager();
	
	virtual void ShakeWindow(HWND hWnd, int nRepetitions = 30, DWORD dwDelay = 25);
	virtual void PlayNudgeSound(IUOLMessengerContactPtr pContact);
	
	virtual IUOLMessengerLoggerPtr GetApplicationLogger();
	
	virtual IUOLMessengerPluginPropertiesRegisterPtr GetPluginPropertiesRegister();
	virtual IUOLMessengerLoggingManagerPtr GetLoggingManager();
	virtual IUOLMessengerDiagnoseManagerPtr GetDiagnoseManager();
	
private:
	CUIMApplication();

	void InitAccounts();	

private:
	static IUOLMessengerFacade3*		ms_pUIMApplication;

	CMessageLoop						m_theLoop;

	IUOLMessengerSettingsManager2Ptr	m_pSettingsManager;
	IUOLMessengerAccountManagerPtr		m_pAccountManager;
	IUOLMessengerProtocolManagerPtr		m_pProtocolManager;
	IUOLMessengerUserIconCacheManagerPtr	m_pUserIconCacheManager;
	IUOLMessengerUIManagerPtr			m_pUIManager;
	IUOLMessengerThemeManagerPtr		m_pThemeManager;
	IUOLMessengerProfileManagerPtr		m_pProfileManager;	
	IUOLMessengerHistoryManager4Ptr		m_pHistory;
	IUOLMessengerCommandFactoryPtr		m_pCommandFactory;

	IUOLMessengerContactListPtr			m_pContactList;
	IUOLMessengerFileTransferManagerPtr m_pFileTransferManager;
	IUOLMessengerTrayWindowPtr			m_pTrayWindow;

	IUOLMessengerNotifyPopUpManagerPtr			m_pNotifyPopUpManager;
	IUOLMessengerAuthenticationManagerPtr		m_pAuthenticationManager;
	IUOLMessengerMessageReceivingFilterPtr		m_pMessageReceivingFilter;
	IUOLMessengerInternetConnectionMonitorPtr	m_pInternetConnectionMonitor;

	IUOLMessengerAccountConnectionManagerPtr	m_pAccountConnectionManager;
	IUOLMessengerRemoteContactListPtr			m_pRemoteContactList;
	IUOLMessengerPrivacyManagerPtr				m_pPrivacyManager;
	IUOLMessengerConversationManagerPtr			m_pConversationManager;

	IUOLMessengerCommonSettingsPtr		m_pCommonSettings;
	IUOLMessengerPropertyPageCommandFactoryPtr	m_pPropPageCommandFactory;

	CProtocolCallbackController			m_protocolCallbackController;
	CUOLMessengerPluginManagerPtr		m_pMessengerPluginManager;
	CMessageQueueController 			m_messageQueueController;
	IDiskCacheManagerPtr				m_pDiskCacheManager;

	UOLLib::ISettings*					m_pStringLoader;
	
	CJobThreadPtr					m_pThreadJobs;
	LONG							m_bLiveUpdateAvailable;
	LONG							m_bKillReceived;	
	
	DWORD							m_dwAppThreadId;
	
	IUOLMessengerMessageTemplateManagerPtr		m_pMessageTemplateManager;
	
	CUIMInitProgressWndPtr			m_pInitProgressWnd;
	
	CPluginPropertiesRegisterPtr	m_pPluginPropertiesRegister;
	CLoggingManagerPtr				m_pLoggingManager;

	CDiagnoseManagerPtr				m_pDiagnoseManager;
	
	CMethodAuthorizationManagerPtr	m_pMethodAuthorizationManager;
};
