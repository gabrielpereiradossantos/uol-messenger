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
#include <commands/OpenBugReportCommand.h>
#include <interfaces/IUOLMessengerPluginPropertiesRegister.h>

#include "../../resource.h"
#include "../BugReportThread.h"
#include "../../FileUtils.h"
#include "../../Cryptography.h"
#include "../LoggingManager.h"
#include "../UIManager.h"


#define		GLOBAL_LOG_RECORDS_REPORT_TITLE		"\nWARNING and ERROR from glogal log records"
#define		LIBPURPLE_LOG_RECORDS_REPORT_TITLE	"\nINFO, WARNING and ERROR from libpurple log records"
#define		COMMON_SETTINGS_REPORT_TITLE		"\nCommon Settings info"
#define		CONTACT_LIST_SETTINGS_REPORT_TITLE	"\nContactList Settings info"
#define		EMOTICON_SETTINGS_REPORT_TITLE		"\nEmoticon Settings info"
#define		GENERAL_SETTINGS_REPORT_TITLE		"\nGeneral Settings info"
#define		MESSAGE_SETTINGS_REPORT_TITLE		"\nMessage Settings info"
#define		NETWORK_SETTINGS_REPORT_TITLE		"\nNetwork Settings info"
#define		PROFILE_SETTINGS_REPORT_TITLE		"\nProfile Settings info"
#define		SOUND_SETTINGS_REPORT_TITLE			"\nSound Settings info"
#define		STATUS_SETTINGS_REPORT_TITLE		"\nStatus Settings info"
#define		ACCOUNT_LIST_REPORT_TITLE			"\nAccount List info"
#define		PLUGIN_LIST_REPORT_TITLE			"\nPlugin List info"
#define		PLUGIN_PROPERTIES_LIST_REPORT_TITLE	"\nPlugin Properties"
#define		CALLING_REASON_REPORT_TITLE			"\Calling Reason"
#define		MINI_DUMP_FILE_NAME_REPORT_TITLE	"\Mini Dump File Name"
#define		MINI_DUMP_DATA_REPORT_TITLE			"\Mini Dump Data"


COpenBugReportCommand::COpenBugReportCommand(CWindow wnd) :
	m_wnd(wnd)
{
}


COpenBugReportCommand::COpenBugReportCommand(CWindow wnd, CString strDumpFile) :
		m_wnd(wnd), 
		m_strDumpFile(strDumpFile)
{
}


COpenBugReportCommand::~COpenBugReportCommand()
{
}


void COpenBugReportCommand::Execute()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	ATLASSERT(pUIManagerPtr);

	BOOL bIsCrashReport = !m_strDumpFile.IsEmpty();

	// Allow user show only one Bug Report Window each time, but Crash Report Window override it
	if ((FALSE == pUIManagerPtr->IsBugReportActive()) || bIsCrashReport)
	{
		pUIManagerPtr->SetBugReportActive(TRUE);
		CBugReportThread brt;

		// First we add all additional information to the list.
		//
		AddGlobalLogRecordsToMap();
		AddPurpleLogRecordsToMap();
		AddRegisteredLoggersToMap();
		AddCommonSettingsToMap();
		AddContactListSettingsToMap();
		AddEmoticonSettingsToMap();
		AddGeneralSettingsToMap();
		AddMessageSettingsToMap();
		AddNetworkSettingsToMap();
		AddProfileSettingsToMap();
		AddSoundSettingsToMap();
		AddStatusSettingsToMap();
		AddAccountListToMap();
		AddPluginListToMap();
		AddPluginPropertiesToMap();
		
		if (!bIsCrashReport)
		{
			m_reportInfoList.AddTail(CreateReportInfo(
					CALLING_REASON_REPORT_TITLE, "User Bug Report.\n\n"));
		}
		else
		{
			AddMiniDumpFileToMap();
			
			m_reportInfoList.AddTail(CreateReportInfo(
					CALLING_REASON_REPORT_TITLE, "Application Crash.\n\n"));
		}
		
		brt.SetHwnd(m_wnd);
		brt.AddReportInfoList(m_reportInfoList);
		brt.SetStackSize(2 * 1024 * 1024);
		brt.SetIsCrashReport(bIsCrashReport);
		brt.Start();
		brt.MessageLoopWait();
		brt.Stop();		// Necessario no caso do UOLMessenger ser finalizado 
						// com a tela do "Fale Conosco" aberta.
		pUIManagerPtr->SetBugReportActive(FALSE);
	}
}


void COpenBugReportCommand::AddGlobalLogRecordsToMap()
{
	// We won't use this for now.
	//
	CAtlList<CString> filterPatternsList;
	
	CString strLogRecords = static_cast<CUIMApplication*>(
			CUIMApplication::GetApplication())->GetLogger()->GetLogsFromLevel(
					CUIMGlobalLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING, 
					filterPatternsList);
	
	if (!strLogRecords.IsEmpty())
	{
		strLogRecords += "\n";
		
		m_reportInfoList.AddTail(CreateReportInfo(
				GLOBAL_LOG_RECORDS_REPORT_TITLE, strLogRecords));
	}
}


void COpenBugReportCommand::AddPurpleLogRecordsToMap()
{
	// We won't use this for now.
	//
	CAtlList<CString> filterPatternsList;
	
	CString strLogRecords = static_cast<CUIMApplication*>(
			CUIMApplication::GetApplication())->GetPurpleLogger()->GetLogsFromLevel(
					CUIMGlobalLogger::GLOBAL_LOGGER_LOG_LEVEL_INFO, 
					filterPatternsList);
	
	if (!strLogRecords.IsEmpty())
	{
		strLogRecords += "\n";
		
		m_reportInfoList.AddTail(CreateReportInfo(
				LIBPURPLE_LOG_RECORDS_REPORT_TITLE, strLogRecords));
	}
}


void COpenBugReportCommand::AddRegisteredLoggersToMap()
{
	IUOLMessengerFacade4* pFacade4 = 
			dynamic_cast<IUOLMessengerFacade4*>(
					CUIMApplication::GetApplication());
	
	CLoggingManagerPtr pLoggingManager;
	
	if (pFacade4 != NULL)
	{
		pLoggingManager = ap_dynamic_cast<CLoggingManagerPtr>(
				pFacade4->GetLoggingManager());
	}
	
	if (pLoggingManager != NULL)
	{
		CAtlList<CReportableLogInfoPtr> reportableLogsList;
		CReportableLogInfoPtr pReportableLog;
		CLoggerPtr pReportableLogger;
		CString strReportableLogs;
		CAtlList<CString> filterPatternsList;
		
		pLoggingManager->GetReportableLogsList(reportableLogsList);
		
		for (POSITION listPos = reportableLogsList.GetHeadPosition(); listPos; )
		{
			pReportableLog = reportableLogsList.GetNext(listPos);
			
			pReportableLogger = ap_dynamic_cast<CLoggerPtr>(
					ap_dynamic_cast<IUOLMessengerLoggingManagerPtr>(
							pLoggingManager)->GetLogger(pReportableLog->GetLogId()));
			
			if (pReportableLogger)
			{
				pReportableLog->GetFilterPatternsList(filterPatternsList);
				
				strReportableLogs = pReportableLogger->GetLogsFromLevel(
						pReportableLog->GetLogLevel(), filterPatternsList);
				
				strReportableLogs += "\n";
				
				m_reportInfoList.AddTail(CreateReportInfo(
						pReportableLog->GetReportTitle(), strReportableLogs));
			}
			
			filterPatternsList.RemoveAll();
		}
	}
}


void COpenBugReportCommand::AddCommonSettingsToMap()
{
	CString strCommonSettingsInfo;
	
	IUOLMessengerCommonSettingsPtr pCommonSettings = 
			CUIMApplication::GetApplication()->GetCommonSettings();
	
	if (pCommonSettings)
	{
		strCommonSettingsInfo.Format(
				"\tAppMode: %s\n"
				"\tDefaultTheme: %s\n", 
				pCommonSettings->GetAppMode(), 
				pCommonSettings->GetDefaultTheme());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				COMMON_SETTINGS_REPORT_TITLE, strCommonSettingsInfo));
	}
}


void COpenBugReportCommand::AddContactListSettingsToMap()
{
	CString strContactListSettingsInfo;
	IUOLMessengerContactListSettings2Ptr pContactListSettings2;
	
	if (CUIMApplication::GetApplication()->GetSettingsManager() != NULL)
	{
		pContactListSettings2 = ap_dynamic_cast<IUOLMessengerContactListSettings2Ptr>(
				CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings());
	}
	
	if (pContactListSettings2)
	{
		strContactListSettingsInfo.Format(
				"\tSortType: %d\n"
				"\tIsShowOfflineEnabled: %d\n"
				"\tIsShowGroupsEnabled: %d\n"
				"\tNotifyOnline: %d\n"
				"\tNotifyOffline: %d\n"
				"\tIsTopMost: %d\n"
				"\tSizeImage: %d\n"
				"\tSecurityLevel: %d\n"
				"\tContactNodeLayout: %s\n"
				"\tGroupByAccounts: %d\n", 
				pContactListSettings2->GetSortType(), 
				pContactListSettings2->IsShowOfflineEnabled(), 
				pContactListSettings2->IsShowGroupsEnabled(), 
				pContactListSettings2->GetNotifyOnline(), 
				pContactListSettings2->GetNotifyOffline(), 
				pContactListSettings2->IsTopMost(), 
				pContactListSettings2->GetSizeImage(), 
				pContactListSettings2->GetSecurityLevel(), 
				pContactListSettings2->GetContactNodeLayout(), 
				pContactListSettings2->GetGroupByAccounts());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				CONTACT_LIST_SETTINGS_REPORT_TITLE, strContactListSettingsInfo));
	}
}


void COpenBugReportCommand::AddEmoticonSettingsToMap()
{
	CString strEmoticonSettingsInfo;
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager());
	
	IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;
	
	if (pSettingsManager2)
	{
		pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
				pSettingsManager2->GetEmoticonSettings());
	}
	
	if (pEmoticonSettings3)
	{
		strEmoticonSettingsInfo.Format(
				"\tDefaultDirPath: %s\n"
				"\tFileVersion: %s\n"
				"\tGetUserSettingsFolderPath: %s\n", 
				pEmoticonSettings3->GetDefaultDirPath(), 
				pEmoticonSettings3->GetFileVersion(), 
				pEmoticonSettings3->GetUserSettingsFolderPath());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				EMOTICON_SETTINGS_REPORT_TITLE, strEmoticonSettingsInfo));
	}
}


void COpenBugReportCommand::AddGeneralSettingsToMap()
{
	CString strGeneralSettingsInfo;
	IUOLMessengerGeneralSettingsPtr pGeneralSettings;
	
	if (CUIMApplication::GetApplication()->GetSettingsManager() != NULL)
	{
		pGeneralSettings = CUIMApplication::GetApplication()->
				GetSettingsManager()->GetGeneralSettings();
	}
	
	if (pGeneralSettings)
	{
		strGeneralSettingsInfo.Format(
				"\tIsEnableSound: %d\n"
				"\tIsMuteWhileAwayEnabled: %d\n"
				"\tIsAutoStartupEnabled: %d\n"
				"\tIsOpenBrowserEnabled: %d\n"
				"\tCordX: %d\n"
				"\tCordY: %d\n"
				"\tSizeX: %d\n"
				"\tSizeY: %d\n"
				"\tListSplitterPosition: %d\n"
				"\tSelectedTheme: %s\n"
				"\tDefaultTheme: %s\n"
				"\tLastAvatarFolder: %s\n"
				"\tPluginLibFolder: %s\n"
				"\tDontShowHideAppWarning: %d\n"
				"\tLastSendFileFolder: %s\n"
				"\tLastReceiveFileFolder: %s\n", 
				pGeneralSettings->IsEnableSound(), 
				pGeneralSettings->IsMuteWhileAwayEnabled(), 
				pGeneralSettings->IsAutoStartupEnabled(), 
				pGeneralSettings->IsOpenBrowserEnabled(), 
				pGeneralSettings->GetCordX(), 
				pGeneralSettings->GetCordY(), 
				pGeneralSettings->GetSizeX(), 
				pGeneralSettings->GetSizeY(), 
				pGeneralSettings->GetListSplitterPosition(), 
				pGeneralSettings->GetSelectedTheme(), 
				pGeneralSettings->GetDefaultTheme(), 
				pGeneralSettings->GetLastAvatarFolder(), 
				pGeneralSettings->GetPluginLibFolder(), 
				pGeneralSettings->GetDontShowHideAppWarning(), 
				pGeneralSettings->GetLastSendFileFolder(), 
				pGeneralSettings->GetLastReceiveFileFolder());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				GENERAL_SETTINGS_REPORT_TITLE, strGeneralSettingsInfo));
	}
}


void COpenBugReportCommand::AddMessageSettingsToMap()
{
	CString strMessageSettingsInfo;
	IUOLMessengerMessageSettings3Ptr pMessageSettings3;
	
	if (CUIMApplication::GetApplication()->GetSettingsManager() != NULL)
	{
		pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(
				CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
	}
	
	if (pMessageSettings3)
	{
		strMessageSettingsInfo.Format(
				"\tFontFace: %s\n"
				"\tFontSize: %d\n"
				"\tFontColor: %d\n"
				"\tIsLogMessagesEnabled: %d\n"
				"\tIsOnNewMsgWindowEnabled: %d\n"
				"\tIsTrayNotificationEnabled: %d\n"
				"\tIsFontBoldEnabled: %d\n"
				"\tIsFontItalicEnabled: %d\n"
				"\tIsFontStrikeoutEnabled: %d\n"
				"\tIsFontUnderlineEnabled: %d\n"
				"\tIsShowUnreadMessagesAlertEnabled: %d\n"
				"\tCordX: %d\n"
				"\tCordY: %d\n"
				"\tSizeX: %d\n"
				"\tSizeY: %d\n"
				"\tConversationWindowVerticalSplitterPosition: %d\n"
				"\tConversationWindowHorizontalSplitterPosition: %d\n"
				"\tAllowNudges: %d\n"
				"\tAcceptCustomEmoticons: %d\n"
				"\tDefaultMessageStyle: %s\n"
				"\tFileTransferSaveFolderMode: %d\n"
				"\tDefaultFileTransferFolder: %s\n"
				"\tIsShowRecentHistoryEnabled: %d\n", 
				pMessageSettings3->GetFontFace(), 
				pMessageSettings3->GetFontSize(), 
				pMessageSettings3->GetFontColor(), 
				pMessageSettings3->IsLogMessagesEnabled(), 
				pMessageSettings3->IsOnNewMsgWindowEnabled(), 
				pMessageSettings3->IsTrayNotificationEnabled(), 
				pMessageSettings3->IsFontBoldEnabled(), 
				pMessageSettings3->IsFontItalicEnabled(), 
				pMessageSettings3->IsFontStrikeoutEnabled(), 
				pMessageSettings3->IsFontUnderlineEnabled(), 
				pMessageSettings3->IsShowUnreadMessagesAlertEnabled(), 
				pMessageSettings3->GetCordX(), 
				pMessageSettings3->GetCordY(), 
				pMessageSettings3->GetSizeX(), 
				pMessageSettings3->GetSizeY(), 
				pMessageSettings3->GetConversationWindowVerticalSplitterPosition(), 
				pMessageSettings3->GetConversationWindowHorizontalSplitterPosition(), 
				pMessageSettings3->GetAllowNudges(), 
				pMessageSettings3->GetAcceptCustomEmoticons(), 
				pMessageSettings3->GetDefaultMessageStyle(), 
				pMessageSettings3->GetFileTransferSaveFolderMode(), 
				pMessageSettings3->GetDefaultFileTransferFolder(), 
				pMessageSettings3->IsShowRecentHistoryEnabled());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				MESSAGE_SETTINGS_REPORT_TITLE, strMessageSettingsInfo));
	}
}


void COpenBugReportCommand::AddNetworkSettingsToMap()
{
	CString strNetworkSettingsInfo;
	IUOLMessengerNetworkSettingsPtr pNetworkSettings;
	
	if (CUIMApplication::GetApplication()->GetSettingsManager() != NULL)
	{
		pNetworkSettings = CUIMApplication::GetApplication()->
				GetSettingsManager()->GetNetworkSettings();
	}
	
	if (pNetworkSettings)
	{
		strNetworkSettingsInfo.Format(
				"\tProxyType: %d\n"
				"\tProxyHost: %s\n"
				"\tProxyPort: %d\n"
				"\tProxyUsername: %s\n", 
				pNetworkSettings->GetProxyType(), 
				pNetworkSettings->GetProxyHost(), 
				pNetworkSettings->GetProxyPort(), 
				pNetworkSettings->GetProxyUsername());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				NETWORK_SETTINGS_REPORT_TITLE, strNetworkSettingsInfo));
	}
}


void COpenBugReportCommand::AddProfileSettingsToMap()
{
	CString strProfileSettingsInfo;
	
	IUOLMessengerProfileManagerPtr pProfileManager = 
			CUIMApplication::GetApplication()->GetProfileManager();
	
	if (pProfileManager)
	{
		strProfileSettingsInfo.Format(
				"\tCurrentProfile: %s\n"
				"\tDefaultProfile: %s\n", 
				pProfileManager->GetCurrentProfile(), 
				pProfileManager->GetDefaultProfile());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				PROFILE_SETTINGS_REPORT_TITLE, strProfileSettingsInfo));
	}
}


void COpenBugReportCommand::AddSoundSettingsToMap()
{
	CString strSoundSettingsInfo;
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager());
	
	IUOLMessengerSoundSettings3Ptr pSoundSettings3;
	
	if (pSettingsManager2)
	{
		pSoundSettings3 = ap_dynamic_cast<IUOLMessengerSoundSettings3Ptr>(
				pSettingsManager2->GetSoundSettings());
	}
	
	if (pSoundSettings3)
	{
		strSoundSettingsInfo.Format(
				"\tDefaultDirPath: %s\n", 
				pSoundSettings3->GetDefaultDirPath());
		
		CAtlList<IUOLMessengerSoundEventPtr> listSoundEvents;
		pSoundSettings3->GetSoundEventList(listSoundEvents);
		
		if (listSoundEvents.GetCount() > 0)
		{
			strSoundSettingsInfo += "\tSoundEventsList: \n";
			
			IUOLMessengerSoundEventPtr pSoundEvent;
			CString strSoundEventInfo;
			
			for (POSITION listPos = listSoundEvents.GetHeadPosition(); listPos; )
			{
				pSoundEvent = listSoundEvents.GetNext(listPos);
				
				strSoundEventInfo.Format(
						"\t\tSoundEvent: EventType = [%s], Description = [%s], "
						"FilePath = [%s], IsEnabled = [%d].\n", 
						pSoundEvent->GetEventType(), 
						pSoundEvent->GetDescription(), 
						pSoundEvent->GetFilePath(), 
						pSoundEvent->IsEnabled());
				
				strSoundSettingsInfo += strSoundEventInfo;
			}
		}
		
		m_reportInfoList.AddTail(CreateReportInfo(
				SOUND_SETTINGS_REPORT_TITLE, strSoundSettingsInfo));
	}
}


void COpenBugReportCommand::AddStatusSettingsToMap()
{
	CString strStatusSettingsInfo;
	IUOLMessengerStatusSettingsPtr pStatusSettings;
	
	if (CUIMApplication::GetApplication()->GetSettingsManager() != NULL)
	{
		pStatusSettings = CUIMApplication::GetApplication()->
				GetSettingsManager()->GetStatusSettings();
	}
	
	if (pStatusSettings)
	{
		strStatusSettingsInfo.Format(
				"\tIsAutoAwayEnabled: %d\n"
				"\tGetTimeout: %d\n"
				"\tGetAwayMessage: %s\n", 
				pStatusSettings->IsAutoAwayEnabled(), 
				pStatusSettings->GetTimeout(), 
				pStatusSettings->GetAwayMessage());
		
		m_reportInfoList.AddTail(CreateReportInfo(
				STATUS_SETTINGS_REPORT_TITLE, strStatusSettingsInfo));
	}
}


void COpenBugReportCommand::AddAccountListToMap()
{
	CString strAccountListInfo;
	
	IUOLMessengerAccountManagerPtr pAccountManager = 
			CUIMApplication::GetApplication()->GetAccountManager();
	
	if (pAccountManager)
	{
		CAtlList<IUOLMessengerAccountPtr> accountList;
		pAccountManager->GetAccountList(accountList);
		
		if (accountList.GetCount() == 0)
		{
			strAccountListInfo = "No accounts created.\n";
		}
		else
		{
			strAccountListInfo.Format(
					"List of %d current created accounts:\n", 
					accountList.GetCount());
			
			IUOLMessengerAccountPtr pAccount;
			CString strAccountInfo;
			
			for (POSITION listPos = accountList.GetHeadPosition(); listPos; )
			{
				pAccount = accountList.GetNext(listPos);
				
				strAccountInfo.Format(
						"\tAccount: UserId = [%s], NickName = [%s], IsAutoConnectEnabled = [%d], "
						"IsSavePasswordEnabled = [%d], ProtocolId = [%s].\n",
						pAccount->GetUserId(), 
						pAccount->GetNickName(), 
						pAccount->IsAutoConnectEnabled(), 
						pAccount->IsSavePasswordEnabled(), 
						pAccount->GetProtocolId());
				
				CProtocolSettings3* pProtocolSettings3 = 
						dynamic_cast<CProtocolSettings3*>(pAccount->GetProtocolSettings().get());
				
				if (pProtocolSettings3)
				{
					CProtocolSettings3::CStringList settingList;
					CProtocolSettings3::CIntegerList integerValueList;
					CProtocolSettings3::CStringList stringValueList;
					CProtocolSettings3::CBooleanList booleanValueList;
					
					CString strProtocolSettingInfo;
					
					pProtocolSettings3->GetIntegerSettingsList(settingList, integerValueList);
					
					while (settingList.size() > 0)
					{
						strProtocolSettingInfo.Format(
								"\t\tProtocolSetting: Setting = [%s], Value = [%d].\n",
								settingList.front().c_str(), integerValueList.front());
						
						strAccountInfo += strProtocolSettingInfo;
						
						settingList.pop_front();
						integerValueList.pop_front();
					}
					
					pProtocolSettings3->GetStringSettingsList(settingList, stringValueList);
					
					while (settingList.size() > 0)
					{
						strProtocolSettingInfo.Format(
								"\t\tProtocolSetting: Setting = [%s], Value = [%s].\n",
								settingList.front().c_str(), stringValueList.front().c_str());
						
						strAccountInfo += strProtocolSettingInfo;
						
						settingList.pop_front();
						stringValueList.pop_front();
					}
					
					pProtocolSettings3->GetBooleanSettingsList(settingList, booleanValueList);
					
					while (settingList.size() > 0)
					{
						strProtocolSettingInfo.Format(
								"\t\tProtocolSetting: Setting = [%s], Value = [%d].\n",
								settingList.front().c_str(), booleanValueList.front());
						
						strAccountInfo += strProtocolSettingInfo;
						
						settingList.pop_front();
						booleanValueList.pop_front();
					}
				}
				
				strAccountListInfo += strAccountInfo;
			}
		}
		
		m_reportInfoList.AddTail(CreateReportInfo(
				ACCOUNT_LIST_REPORT_TITLE, strAccountListInfo));
	}
}


void COpenBugReportCommand::AddPluginListToMap()
{
	CString strPluginListInfo;
	
	CUOLMessengerPluginManager* pPluginManager = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();
	
	if (pPluginManager)
	{
		CAtlList<CPluginInfoPtr> pluginInfoList;
		pPluginManager->GetPluginInfoList(pluginInfoList);
		
		if (pluginInfoList.GetCount() == 0)
		{
			strPluginListInfo = "No plugins loaded.\n";
		}
		else
		{
			strPluginListInfo.Format(
					"List of %d plugins loaded:\n", 
					pluginInfoList.GetCount());
			
			CPluginInfoPtr pPluginInfo;
			CString strPluginInfo;
			
			for (POSITION listPos = pluginInfoList.GetHeadPosition(); listPos; )
			{
				pPluginInfo = pluginInfoList.GetNext(listPos);
				
				ULONGLONG ullPluginVersion = pPluginInfo->GetVersion();
				
				int iVersionItem1 = ullPluginVersion;
				int iVersionItem2 = ullPluginVersion;
				
				ullPluginVersion >>= 32;
				
				int iVersionItem3 = ullPluginVersion;
				int iVersionItem4 = ullPluginVersion;
				
				iVersionItem1 &= 0x00FF;
				iVersionItem2 >>= 16;
				iVersionItem3 &= 0x00FF;
				iVersionItem4 >>= 16;
				
				strPluginInfo.Format(
						"\tPlugin: InternalName = [%s], Version = [%d.%d.%d.%d], UpdateInfo = [%d].\n",
						pPluginInfo->GetInternalName(), 
						iVersionItem4, 
						iVersionItem3, 
						iVersionItem2, 
						iVersionItem1, 
						pPluginInfo->GetUpdateInfo());
				
				strPluginListInfo += strPluginInfo;
			}
		}
		
		strPluginListInfo += "\n";
		
		m_reportInfoList.AddTail(CreateReportInfo(
				PLUGIN_LIST_REPORT_TITLE, strPluginListInfo));
	}
}


void COpenBugReportCommand::AddPluginPropertiesToMap()
{
	IUOLMessengerFacade4* pFacade4 = 
			dynamic_cast<IUOLMessengerFacade4*>(
					CUIMApplication::GetApplication());
	
	IUOLMessengerPluginPropertiesRegisterPtr pPluginPropertiesRegister;
	
	if (pFacade4 != NULL)
	{
		pPluginPropertiesRegister = 
				pFacade4->GetPluginPropertiesRegister();
	}
	
	if (pPluginPropertiesRegister != NULL)
	{
		CAtlList<IUOLMessengerPluginPropertiesPtr> listPluginProperties;
		IUOLMessengerPluginPropertiesPtr pPluginProperties;
		CString strPluginsProperties;
		
		pPluginPropertiesRegister->GetPluginPropertiesList(listPluginProperties);
		
		for (POSITION listPos = listPluginProperties.GetHeadPosition(); listPos; )
		{
			pPluginProperties = listPluginProperties.GetNext(listPos);
			int iPluginPropertiesCount = pPluginProperties->GetPluginPropertiesCount();
			CString strPropertyName;
			
			strPluginsProperties += "Propriedades do plugin [";
			strPluginsProperties += pPluginProperties->GetPluginName();;
			strPluginsProperties += "]:\n";
			
			for (int index = 0; index < iPluginPropertiesCount; index++)
			{
				strPluginsProperties += "\t";
				strPropertyName = pPluginProperties->GetPluginPropertyName(index);
				strPluginsProperties += strPropertyName;
				strPluginsProperties += " = [";
				strPluginsProperties += pPluginProperties->
						GetPluginPropertyValue(strPropertyName);
				strPluginsProperties += "]\n";
			}
		}
		
		strPluginsProperties += "\n";
		
		m_reportInfoList.AddTail(CreateReportInfo(
				PLUGIN_PROPERTIES_LIST_REPORT_TITLE, strPluginsProperties));
	}
}


void COpenBugReportCommand::AddMiniDumpFileToMap()
{
	unsigned char* pFileData = NULL;
	long iFileDataLength = 0;
	
	CCryptography cryptography;
	CString strBase64File;
	CString strMiniDumpInfo;
	long iBase64Length = 0;
	
	if (CFileUtils::LoadFile(m_strDumpFile, pFileData, iFileDataLength) == TRUE)
	{
		if (cryptography.EncodeBase64(pFileData, 
				iFileDataLength, strBase64File, iBase64Length) == TRUE)
		{
			strMiniDumpInfo = m_strDumpFile;
			strMiniDumpInfo += "\n\n";
			
			m_reportInfoList.AddTail(CreateReportInfo(
					MINI_DUMP_FILE_NAME_REPORT_TITLE, strMiniDumpInfo));
			
			m_reportInfoList.AddTail(CreateReportInfo(
					MINI_DUMP_DATA_REPORT_TITLE, strBase64File + "\n\n"));
			
			// Just checking the dump data encoded is ok.
			//
			/*
			CString newFileName = m_strDumpFile;
			newFileName += "2";
			
			unsigned char* pFileData2 = NULL;
			long iFileDataLength2 = 0;
			
			if (cryptography.DecodeBase64(strBase64File, 
					iBase64Length, pFileData2, iFileDataLength2) == TRUE)
			{
				CFileUtils::StoreOnFile(newFileName, FALSE, pFileData2, iFileDataLength2);
				
				free(pFileData2);
			}
			*/
		}
		
		free(pFileData);
	}
}


COpenBugReportCommand::ReportInfoPtr COpenBugReportCommand::CreateReportInfo(
		CString strReportInfoTitle, CString strReportInfoData)
{
	ReportInfoPtr pReportInfo = new ReportInfo;
	
	if (pReportInfo)
	{
		pReportInfo->strReportInfoTitle = strReportInfoTitle;
		pReportInfo->strReportInfoData = strReportInfoData;
	}
	
	return pReportInfo;
}

