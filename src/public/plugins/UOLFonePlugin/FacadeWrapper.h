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
#include <interfaces/IUOLMessengerImage.h>
#include <interfaces/IUOLMessengerUIManager.h>
#include <interfaces/IUOLMessengerProfileManager.h>
#include <interfaces/IUOLMessengerMainFrameFocusNotifier.h>
#include <interfaces/IUOLMessengerMainWindowEventsNotifier.h>
#include <interfaces/IUOLMessengerPropertyPageCommandFactory.h>
#include <interfaces/IUOLMessengerFrameWindow.h>
#include <interfaces/IUOLMessengerSoundSettings.h>
#include <interfaces/IUOLMessengerHistoryRecord.h>

#include "UOLFonePluginConfig.h"
#include "SoundEvent.h"
#include "core/FileWaveSound.h"
#include "core/UOLFoneWindow.h"
#include "core/UOLFoneChildWindowContainerView.h"
#include "core/UOLFoneKeyboardWindow.h"
#include "core/IUOLFoneControllerCallback.h"
#include "core/UOLFoneChildWindowInfo.h"
#include "core/UOLFoneRatesWindowFrame.h"
#include "core/XmlNodeWrapper.h"
#include "core/VoipMessagesHelper.h"
#include "TestSIP.h"



class CFacadeWrapper : 
		//public CUOLMessengerImageButtonEventListener2,
		public IUOLFoneControllerCallback,
		public CWindowImpl<CFacadeWrapper>, 
		public CUOLMessengerMainWindowEventsObserver, 
		public ITabChildWindowActionCallback,
		public CUOLMessengerSoundSettingsObserver
{
public:
	
	static const char* VOIP_LOGGER_MODULE_NAME;
	static const char* PLUGIN_LOGGER_MODULE_NAME;
	static const IUOLMessengerLogger::LogLevelType VOIP_LOGGER_LOG_LEVEL;
	static const IUOLMessengerLogger::LogLevelType VOIP_LOGGER_TO_REPORT_LEVEL;
	static const IUOLMessengerLogger::LogLevelType PLUGIN_LOGGER_LOG_LEVEL;
	static const IUOLMessengerLogger::LogLevelType PLUGIN_LOGGER_TO_REPORT_LEVEL;
	static const int VOIP_LOGGER_PURGE_INTERVAL;
	
public:
	DECLARE_WND_CLASS("UOLFoneFacadeWrapper")
	
	enum ActionCallbackIDs
	{
		ACTION_CALLBACK_CONNECT_UOL_ACCOUNT = 0x342, 
		ACTION_CALLBACK_CREATE_UOL_ACCOUNT, 
		ACTION_CALLBACK_CALL_STATE_CHANGE_TO_ERROR,
		ACTION_CALLBACK_ACCOUNT_STATE_CHANCE_TO_ERROR,
		ACTION_CALLBACK_CALL_ERROR_RECEIVED,
		ACTION_CALLBACK_RECONNECT_VOIP_ACCOUNT, //Reconnect on SIP Server
		ACTION_CALLBACK_RETURN,
		ACTION_CALLBACK_RESTART_APP,
		ACTION_CALLBACK_BUY_CREDITS
	};
	
	virtual ~CFacadeWrapper();
	
	static CFacadeWrapper* GetInstance();
	static IUOLMessengerFacade* GetFacadeInstance();
	
	void Init(IUOLMessengerFacade* pFacade, CUOLMessengerPlugin* pUOLMessengerPlugin);
	void Finalize();

	void PluginInit();
	void PluginFinalize();
	
	IUOLMessengerFacade* GetFacade();
	
	int AddTabButton();
	void RemoveTabButton();
	
	void SetUOLFoneWindowPtr(CUOLFoneWindow* uolFoneWndPtr);
	void SetChildContainerPtr(CUOLFoneChildWindowContainerView* childContainerPtr);
	
	void ShowHomeWindow();
	void ShowCallInPlaceWindow();
	void ShowAcceptCallWindow();
	void ShowCallErrorWindow(LPCTSTR strURL);
	void ShowKeyboardWindow();
	void ShowCallInProgressWindow();
	void ShowInitializingWindow();
	void ShowInformationWindow(LPCTSTR strURL);
	BOOL StartCallToPC(IUOLMessengerContactPtr pContact, BOOL bIsContactAddressTyped);
	BOOL StartCallToPhone();
	BOOL ChooseStartCallType();
	BOOL AcceptCall();
	void OnStopCall();
	void ShowPluginConfigDialog();
	void FormatPhoneToCall(CString& strPhoneNumber);
	void HangUpPhoneCall();
	void Redial();
	void TransferPhoneCall(CString& strPhoneNumber);
	void HoldPhoneCall();
	void ResumePhoneCall();
	void StartConferenceCall(CString& strPhoneNumber01, CString& strPhoneNumber02);
	void HangUpConferenceCall();

	void ShowHistoryWindow(const CString& strTypeFilter);
	void ShowVoipRates();
	void ShowSACButton();
	
	LPCTSTR GetVoIPCallText();
	IUOLMessengerImagePtr GetVoIPCallImage();
	void SetVoIPCallInfo(const CString& voIPCallText, IUOLMessengerImagePtr pVoIPCallImage);
	void SetVoIPCallInfo(const CString& voIPCallText, UINT nResId);
	void SetVoIPCallInfo(const CString& voIPCallText, IUOLMessengerImagePtr pVoIPCallImage, const CString& contactAccount);

	LPCTSTR GetLastCallErrorDescription();
	
	BOOL PlaySound(UINT nChar);
	
	BOOL PlayCallIncomingSound();
	BOOL PlayCallClosedSound();
	BOOL PlayCallCallingSound();
	BOOL StopCallSound();
	
	IUOLMessengerImageDrawStrategyPtr CreateDrawStrategy(
			IUOLMessengerUIManager::ImageDrawStrategy drawStrategy);

	IUOLMessengerImagePtr GetPC2PhoneDefaultImage();
	IUOLMessengerImagePtr GetPC2PhoneDefaultSmallImage();
	IUOLMessengerImagePtr GetPC2PCDefaultImage();
	IUOLMessengerImagePtr GetPC2PCDefaultSmallImage();
	
	BOOL GetCurrentUserProfilePath(CString& profilePath);
	
	BOOL LoadConfigFileStub(LPCTSTR resourceID, CString& configFileStub);
	
	void OpenBrowserWithURL(CString url, BOOL bAuthenticated = FALSE);
	
	BOOL GetConnectedUserAccounts(CAtlList<IUOLMessengerAccountPtr>& listAccounts);

	IUOLMessengerContactPtr FindContact(IUOLMessengerAccountPtr pIMAccount, const CString& strUser, BOOL bCreateWhenNotFound = FALSE);
	
	void RegisterAsFocusObserver(CUOLMessengerMainFrameFocusObserver* pFocusObserver);
	void UnregisterAsFocusObserver(CUOLMessengerMainFrameFocusObserver* pFocusObserver);
	
	IUOLFoneControllerCallback::InitStatusCode GetInitStatusCode();
	
	BOOL RegisterAsMainWindowEventsNotifier(CUOLMessengerMainWindowEventsObserver* pEventsObserver);
	BOOL UnregisterAsMainWindowEventsNotifier(CUOLMessengerMainWindowEventsObserver* pEventsObserver);
	
	void SetFocusToMainWindow();
	
	void	ExecuteInfomationButtonAction(UINT nActionID);
	DWORD	GetButtonImageResourceID(UINT nActionID);
	
	void ShowUOLAccountDisconnected();
	void ShowUOLAccountNotFound();
	void ShowBOLAccountWarning();
	void ShowUOLFoneSubscriberWarning();
	
	BOOL IsUOLMessengerAppMode();

	void LogBusyCall(const CString& phoneNumber);
	void LogBusyCall(IUOLMessengerContactPtr pContact);

	BOOL CallSAC();

	void DestroyRateWindow();
	CString GetpromoURL();
	CString GetpriceURL();
	CString GetSACnumber();
	CString GetBuyPlanURL();
	CString GetBuyCreditsURL();
	CString GetVoipOutPlanIDs();

	// Add/remove sound event to/from UOLMessenger
	void AddSoundEvents();
	void RemoveSoundEvents();

	void UpdateBalancePanel(CUOLFoneAccountPtr pAccount);

	//void StartMessageHook(CWindow* keyboardWindow);
	//void StopMessageHook();
	
	void CategorizeAndShow(LPCTSTR statusText, LPCTSTR description, 
			ITabChildWindowActionCallback* pActionCallback, UINT nActionId = 0);
	
	void LogOsipDebugEvent(const CString& strFileName, int iLineNumber, 
			int iLogLevel, const CString& strLogMessage);
	
	IUOLMessengerLoggingManagerPtr GetLoggingManager();
	
	CString GetLogsDefaultPath();
	
	int GetOsipLogCurrentLevel();

	IUOLMessengerDiagnoseManagerPtr GetDiagnoseManager();
	
protected:
	CFacadeWrapper();

	enum
	{
		TIMER_INITIALIZATION_PENDING = 0x100,
		SESSION_NOT_FOUND = 15,
		NOT_ENOUGH_CREDITS_ERROR_CODE = 18,
		MEDIA_TIMEOUT_ERROR_CODE = 20,
		SESSION_INVALID = 21
	};

	BEGIN_MSG_MAP(CFacadeWrapper)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

	void OnTimer(UINT nTimerId, TIMERPROC timerproc);
	
	// CUOLMessengerImageButtonEventListener2 overriden functions
	//virtual void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	
	IUOLMessengerUIManager2Ptr GetUIManager2();
	IUOLMessengerUIManager3Ptr GetUIManager3();
	IUOLMessengerUIManager4Ptr GetUIManager4();
	IUOLMessengerUIManager5Ptr GetUIManager5();

	void ShowChildWindow(int childWindowID, CUOLFoneChildWindowInfo* pChildWindowInfo);
	
	void ShowUOLFoneTab();

	IUOLMessengerImagePtr LoadImageFromResource(UINT nResId);

	void OnInitializationCompleted();
	void OnInitializationFailed(InitStatusCode initStatusCode, 
			CUOLFoneFailureInfoPtr pFailureInfo);
	void OnAccountConnectionStateChanged(CUOLFoneAccountPtr pAccount);
	void OnAccountInfoReceived(CUOLFoneAccountPtr pAccount);
	void OnAccountErrorReceived(CUOLFoneAccountPtr pAccount);
	void OnPhoneCallStateChanged(CPhoneCallPtr pPhoneCall);
	void OnPhoneCallInfoReceived(CPhoneCallPtr pPhoneCall);
	void OnPhoneCallErrorReceived(CPhoneCallPtr pPhoneCall, 
			CUOLFoneFailureInfoPtr pFailureInfo);
	void OnPhoneCallServerErrorReceived(CPhoneCallPtr pPhoneCall);
	void OnExecutionFailure(ExecStatusCode execStatusCode, 
			CUOLFoneFailureInfoPtr pFailureInfo);
	
	BOOL CallPC(IUOLMessengerContactPtr pContact);
	BOOL CallPhone(const CString& strPhoneNumber);

	//static LRESULT CALLBACK GetMsgHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	
	// CUOLMessengerMainWindowEventsObserver overriden functions
	virtual void OnMainWindowPosCreate();
	virtual void OnMainWindowPosShowWindow(BOOL bShow);
	
	CSoundEventPtr GetSoundEventByKey(UINT nChar);
	
	void ShowUserWarning(UINT nStrResourceID, int nActionID);
	
	BOOL IsBOLUser(CUOLFoneAccountPtr pAccount);
	
	void ResetLastCallErrorInfo();

	void LoadFoneInfo();

	// CUOLMessengerSoundSettingsObserver
	virtual void OnSoundSettingsChanged(IUOLMessengerSoundSettings3Ptr pSoundSettings);

	void LogFailureInfo(CString strFriendlyMessage);

	void ShowCallErrorWindow(LPCTSTR errorStatus, LPCTSTR errorDescription, 
			ITabChildWindowActionCallback* pActionCallback, UINT nActionId = 0);
	void ShowInformationWindow(LPCTSTR title, LPCTSTR message, 
			ITabChildWindowActionCallback* pActionCallback, UINT nActionId = 0);
	
private:
	
	IUOLMessengerFacade*	m_pFacade;
	CUOLMessengerPlugin*	m_pUOLMessengerPlugin;

	UINT					m_nInitializationTimer;
	IUOLFoneControllerCallback::InitStatusCode	m_initStatusCode;
	CUOLFoneFailureInfoPtr	m_pLastFailureInfo;
	
	IUOLMessengerImagePtr			m_pUOLFoneTabButtonImage;
	IUOLMessengerImageButtonPtr		m_pUOLFoneTabButton;
	
	BOOL	m_bIsToolbarButtonVisible;
	
	CUOLFoneWindow*						m_uolFoneWndPtr;
	CUOLFoneChildWindowContainerView*	m_childContainerPtr;
	
	int							m_callDuration;
	CComAutoCriticalSection		m_callDurationLock;
	
	CComAutoCriticalSection		m_showChildWindowLock;
	
	CString						m_voIPCallText;
	IUOLMessengerImagePtr		m_pVoIPCallImage;
	IUOLMessengerContactPtr		m_pCurContact;
	CString						m_strCurPhoneNumber;
	CString						m_contactAccount;

	CString						m_lastCallErrorDescription;
	CString						m_lastCallErrorUrl;
	UINT						m_lastCallErrorCode;
	CString						m_lastCallDisplayUrl;
	BOOL						m_callInPlace;
	BOOL						m_callIncoming;
	CComAutoCriticalSection		m_callInPlaceLock;
	
	IUOLMessengerImagePtr		m_pImagePC2Phone;
	IUOLMessengerImagePtr		m_pImagePC2PhoneSmall;
	IUOLMessengerImagePtr		m_pImagePC2PC;
	IUOLMessengerImagePtr		m_pImagePC2PCSmall;
	
	CComAutoCriticalSection		m_initTimerLock;
	
	BOOL						m_showingFirstTime;
	int							m_nTabButtonIndex;
	
	BOOL						m_bInitializationHasAlreadyFailed;
	BOOL						m_bInitWindowHasBeenShown;

	BOOL						m_bCallAlreadyLogged;
	IUOLMessengerFrameWindowPtr m_pFrameWindow;
	CUOLFoneRatesWindowFramePtr	m_pView;
	
	BOOL						m_bIsShowingError;
		
	CString						m_strSACnumber;
	CString						m_strpriceURL;
	CString						m_strpromoURL;
	
	CXmlDocumentWrapper			m_uolfoneInfoDoc;
	CXmlNodeWrapper*			m_SACnumberNode;
	CXmlNodeWrapper*			m_priceURLNode;
	CXmlNodeWrapper*			m_promoURLNode;
	CXmlNodeWrapper*			m_buyPlanURLNode;
	CXmlNodeWrapper*			m_buyCreditsURLNode;
	CXmlNodeWrapper*			m_VoipOutPlanIDsNode;

	BOOL						m_infoLoaded;

	CFileWaveSound				m_FileWaveSound;
	
	CString						m_errorsLogFilePath;
	BOOL						m_bMediaTimeoutError;
	
	CVoipMessagesHelper			m_messagesHelper;

	CComObject<CTestSIP>*		m_pTestSIP;
};

