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


#include "../VerifyDllsThread.h"
#include "IUOLFoneControllerCallback.h"
#include "UOLFoneAccountManager.h"
#include "IUOLFoneAccountManagerObserver.h"
#include "ServerConfig.h"
#include "AccountStateChangeHandler.h"
#include "PhoneCallStateChangeHandler.h"
#include "ServerMessageHandler.h"
#include "CallContactParser.h"

#include "com/IDownloadCallbackHandler.h"
#include "com/DownloadCallback.h"
#include "com/ICallbackHandler.h"
#include "com/UOLFoneClientCallback.h"
#include "com/ICodecsConfigFileUpdaterCallbackHandler.h"
#include "com/CodecsConfigFileUpdaterCallback.h"
#include "com/ICodecsUpdaterCallbackHandler.h"
#include "com/CodecsUpdaterCallback.h"
#include "com/PhoneLineInfo.h"
#include "com/UserInfo.h"
#include "com/NetworkConfig.h"
#include "com/PhoneLineConfig.h"
#include "UOLFoneFailureInfo.h"



class CUOLFoneController : 
	public CWindowImpl<CUOLFoneController>,
	public ICallbackHandler,
	public IDownloadCallbackHandler,
	public ICodecsConfigFileUpdaterCallbackHandler,
	public ICodecsUpdaterCallbackHandler,
	public IVerifyDllsThreadCallback,
	public IUOLFoneAccountManagerObserver
{
public:
	virtual ~CUOLFoneController();

	static CUOLFoneController* GetInstance();

	BOOL Initialize(const CString& strLibDir, IUOLFoneControllerCallback* pControllerCallback);
	void Finalize();

	CUOLFoneAccountManagerPtr GetUOLFoneAccountManager();

	void GetOutputDevices(CAtlList<CString>& listDevices);
	void GetInputDevices(CAtlList<CString>& listDevices);

	CString GetCurrentOutputDevice() const;
	void SetCurrentOutputDevice(const CString& strOutputDevice);

	LONG GetCurrentOutputDeviceVolume();
	void SetCurrentOutputDeviceVolume(LONG lLevel);

	BOOL GetCurrentOutputDeviceMute() const;
	void SetCurrentOutputDeviceMute(BOOL bMute);

	CString GetCurrentInputDevice() const;
	void SetCurrentInputDevice(const CString& strInputDevice);

	LONG GetCurrentInputDeviceVolume();
	void SetCurrentInputDeviceVolume(LONG lLevel);

	BOOL GetCurrentInputDeviceMute() const;
	void SetCurrentInputDeviceMute(BOOL bMute);

	BOOL Connect();
	BOOL Disconnect();

	BOOL IsConnected();

	BOOL IsValidPhoneNumber(const CString& strPhoneNumber);

	BOOL MakePhoneCall(IUOLMessengerContactPtr pContact);
	BOOL MakePhoneCall(const CString& strPhoneNumber);

	BOOL HangUpPhoneCall();
	
	BOOL TransferPhoneCall(CString& strPhoneNumber);
	BOOL HoldPhoneCall();
	BOOL ResumePhoneCall();
	BOOL MakeConferenceCall(CString& strPhoneNumber01, CString& strPhoneNumber02);
	BOOL HangUpConferenceCall();

	BOOL AcceptIncomingCall();
	BOOL RejectIncomingCall();

	BOOL IsCallInProgress();
	
	BOOL PlayTone(UINT nChar);
	
	CUOLFoneAccountPtr GetCurrentAccount();
	
	void UpdateNetworkConfig();
	
	BOOL IsIMAccountConnected();
	
	CUOLFoneFailureInfoPtr GetLastFailureInfo();
	
	void PublishCapabilities(IUOLMessengerAccountPtr pAccount);
	void RevokeCapabilities(IUOLMessengerAccountPtr pAccount);
	
	BOOL HasPendingCallToProceed();
	BOOL TestNetworkConnection();

	CString GetUOLFoneClientDllName() const;
	CString GetUOLFoneCryptDllName(BOOL bLocal = FALSE) const;
	CString GetUOLFoneServerConfigProviderDllName() const;
	
protected:
	virtual void PhoneCallStateChanged(IUOLFoneClientPhoneCall* pPhoneCall, EnumPhoneCallState phoneCallState, IUOLFoneClientExtraResponseData2* pExtraResponseData);
	virtual void PhoneLineStateChanged(IUOLFoneClientPhoneLine* pPhoneLine, EnumPhoneLineState phoneLineState, IUOLFoneClientExtraResponseData2* pExtraResponseData);
	virtual void MessageEvent(IUOLFoneClientMessageEvent* pMessageEvent, IUOLFoneClientExtraResponseData2* pExtraResponseData);
	virtual void PhoneCallStreamError(IUOLFoneClientPhoneCall* pPhoneCall, EnumPhoneCallStreamError phoneCallStreamError);
	virtual void PhoneLineConnectionTimeout();
	virtual void OsipDebugEvent(IUOLFoneClientOsipDebugInfo* pOsipDebugInfo);

	virtual void HandleDownloadComplete(IUOLFoneDownload* pDownload);
	virtual void HandleUpdateConfigComplete(LONG lResult);
	virtual void HandleUpdateCodecFileBegin(BSTR codecName, BSTR fileName);
	virtual void HandleUpdateCodecFileProgressChange(BSTR codecName, BSTR fileName, ULONG nProgress, ULONG nProgressMax);
	virtual void HandleUpdateCodecFileComplete(BSTR codecName, BSTR fileName);
	virtual void HandleUpdateCodecComplete(BSTR codecName);
	virtual void HandleUpdateCodecsFinished(LONG lResult);

	virtual void OnUOLFoneAccountChanged(CUOLFoneAccountPtr pOldAccount);

	virtual void OnDownloadBegin(DWORD dwIndex);
	virtual void OnProgressChange(DWORD dwIndex, ULONG nProgress, ULONG nProgressMax);
	virtual void OnDownloadComplete(DWORD dwIndex, DWORD dwError);
	virtual void OnVerifyDllsFinished(HRESULT hr);

	enum
	{
		WM_USER_LIBS_DOWNLOAD_COMPLETE = WM_USER + 0x130,
		WM_USER_INITIALIZE_LIBRARIES_TASK,
		WM_USER_INITIALIZE_PHONE_LINES_TASK,
		WM_USER_CHECK_CODECS_UPDATES_TASK,
		WM_USER_UPDATE_CODECS_TASK,
		WM_USER_REINITIALIZE_PHONELINES_TASK,
		WM_USER_CONNECT_ACCOUNT,
		WM_USER_DISCONNECT_ACCOUNT,
		WM_USER_CHECK_PHONE_NUMBER,
		WM_USER_CALL_PHONE_NUMBER,
		WM_USER_CALL_CONTACT,
		WM_USER_HANG_UP_CALL,
		WM_USER_ACCEPT_INCOMING_CALL,
		WM_USER_REJECT_INCOMING_CALL,
		WM_USER_TRANSFER_CALL,
		WM_USER_HOLD_UP_CALL,
		WM_USER_RESUME_UP_CALL,
		WM_USER_MAKE_CONFERENCE_CALL,
		WM_USER_HANG_UP_CONFERENCE_CALL
	};

	BEGIN_MSG_MAP(CUOLFoneController)
		MESSAGE_HANDLER_EX(WM_USER_INITIALIZE_LIBRARIES_TASK, OnInitializeLibrariesTask)
		MESSAGE_HANDLER_EX(WM_USER_INITIALIZE_PHONE_LINES_TASK, OnInitializePhoneLinesTask)
		MESSAGE_HANDLER_EX(WM_USER_CHECK_CODECS_UPDATES_TASK, OnCheckCodecsUpdateTask)
		//MESSAGE_HANDLER_EX(WM_USER_UPDATE_CODECS_TASK, OnUpdateCodecsTask)
		MESSAGE_HANDLER_EX(WM_USER_REINITIALIZE_PHONELINES_TASK, OnReinitializePhonelinesTask)
		MESSAGE_HANDLER_EX(WM_USER_CONNECT_ACCOUNT, OnConnectAccount)
		MESSAGE_HANDLER_EX(WM_USER_DISCONNECT_ACCOUNT, OnDisconnectAccount)
		MESSAGE_HANDLER_EX(WM_USER_CHECK_PHONE_NUMBER, OnCheckPhoneNumber)
		MESSAGE_HANDLER_EX(WM_USER_CALL_PHONE_NUMBER, OnCallPhoneNumber)
		MESSAGE_HANDLER_EX(WM_USER_CALL_CONTACT, OnCallContact)
		MESSAGE_HANDLER_EX(WM_USER_HANG_UP_CALL, OnHangUpCall)
		MESSAGE_HANDLER_EX(WM_USER_ACCEPT_INCOMING_CALL, OnAcceptIncomingCall)
		MESSAGE_HANDLER_EX(WM_USER_REJECT_INCOMING_CALL, OnRejectIncomingCall)
		MESSAGE_HANDLER_EX(WM_USER_TRANSFER_CALL, OnTransferCall)
		MESSAGE_HANDLER_EX(WM_USER_HOLD_UP_CALL, OnHoldUpCall)
		MESSAGE_HANDLER_EX(WM_USER_RESUME_UP_CALL, OnResumeUpCall)
		MESSAGE_HANDLER_EX(WM_USER_MAKE_CONFERENCE_CALL, OnMakeConferenceCall)
		MESSAGE_HANDLER_EX(WM_USER_HANG_UP_CONFERENCE_CALL, OnHangUpConferenceCall)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

	LRESULT OnInitializeLibrariesTask(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnInitializePhoneLinesTask(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCheckCodecsUpdateTask(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//LRESULT OnUpdateCodecsTask(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnReinitializePhonelinesTask(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnConnectAccount(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDisconnectAccount(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCheckPhoneNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCallPhoneNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCallContact(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHangUpCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnAcceptIncomingCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRejectIncomingCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTransferCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHoldUpCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnResumeUpCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMakeConferenceCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHangUpConferenceCall(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnTimer(UINT nTimerId, TIMERPROC timerproc);
	void OnDestroy();

	enum
	{
		TIMER_SEND_RINGING_NOTIFICATION = 0x150,
		MAX_PHONE_RINGING_COUNT = 5
	};

private:
	CUOLFoneController();

	enum InternalStatus
	{
		InternalStatusUnitialized						= 0,
		InternalStatusInitializing						= 1 << 0,
		InternalStatusFinalizing						= 1 << 1,
		InternalStatusReady								= 1 << 2,
		InternalStatusInitializationError				= 1 << 3,
		InternalStatusDownloadingLibraries				= 1 << 4,
		InternalStatusDownloadingCodecsList				= 1 << 5,
		InternalStatusUpdatingCodecs					= 1 << 6,
		InternalStatusDownloadingPhoneValidationRules	= 1 << 7,
	};

	/// Initialization steps:
	/// 0. Initialize()
	///    0.1 Create UOLFoneController window
	///    0.2 Initialize UOLFoneAccountManager
	///    0.3 Prepare codecs directory
	///    0.4 Start libraries download (async)
	///
	/// [Wait for libraries download]
	///
    /// 1. Initialize libraries
	///    1.1 Initialize UOLFoneServerConfigProvider API
	///    1.2 Initialize UOLFoneClient API
	///    1.3 Start phone validation rules download (async)
	///    1.4 Start codecs list download (async)
	///
	/// [Wait for codecs list download]
	///
	/// 2. Initialize phone lines
	///    2.1 Test network connection
	///    2.2 Initialize/setup phone lines
	///
	/// [Wait for user account registration]
	///
	/// 3. Check for codecs updates
	///    (if there is no updates skip steps 4 and 5)
	///    3.1 Unregister current account (async)
	///
	/// [Wait account unregistration]
	///
	/// 4. Update codecs
	///    4.1 Finalize phone lines
	///    4.2 Start codecs update (async)
	///
	/// [Wait for codecs update process]
	///
	/// 5. Reinitialize phone lines
	///    5.1 Initialize phone lines
	///    5.2 Register user account (async)
	///
	/// [Wait for user account registration]
	///
	/// [Ready]


	BOOL IsInitialized();
	void SetInternalStatus(int nStatus);
	int GetInternalStatus();
	
	//BOOL IsCodecsUpdateInProgress();

	CString GetDllName(UINT nResDllName, UINT nResDllVersion) const;
#ifdef _DEBUG
	CString GetDebugDllName(UINT nResDllName, UINT nResDllVersion) const;
#endif
	CString GetCodecsPath();

	BOOL ScheduleTask(UINT nMessageId);

	BOOL InitializeComLibs();
	void FinalizeComLibs();

	BOOL InitializeUOLFoneClientApi();
	void FinalizeUOLFoneClientApi();

	BOOL InitializeUOLFoneServerConfigProviderApi();
	void FinalizeUOLFoneServerConfigProviderApi();

	BOOL InitializePhoneLineConfig();
	void FinalizePhoneLineConfig();

	BOOL InitializeServerConfig();
	void FinalizeServerConfig();

	BOOL InitializeNetworkConfig();
	void FinalizeNetworkConfig();

	BOOL InitializePhoneLines();
	BOOL FinalizePhoneLines();

	BOOL InitializeUserInfo();
	void FinalizeUserInfo();

	BOOL InitializePhoneLineInfo();
	void FinalizePhoneLineInfo();

	CString GetAudioCodecsPrecedence(BOOL bWithSubscriberCodec = FALSE);

	BOOL DownloadPhoneValidationRules();
	BOOL DownloadCodecsList();

	void FillAccountInscriptions(CUOLFoneAccountPtr pAccount, IUOLFoneClientExtraResponseData2* pExtraResponseData);

	BOOL Connect(CUOLFoneAccountPtr pAccount);
	BOOL Disconnect(CUOLFoneAccountPtr pAccount);
	BOOL IsConnected(CUOLFoneAccountPtr pAccount);

	CString FormatUsername(const CString& strIMAccountUsername);
	CString FormatURI(const CString& strUsername);
	CString FormatDisplayName(const CCallContactParser& callContactParser);

	BOOL CheckIdentity(CUOLFoneAccountPtr pSourceAccount, IUOLFoneClientPhoneLine* pTargerPhoneLine);
	BOOL CheckIdentity(CPhoneCallPtr pSourcePhoneCall, IUOLFoneClientPhoneCall* pTargetPhoneCall);

	BOOL ConnectAccountImpl(CUOLFoneAccountPtr pAccount);
	BOOL DisconnectAccountImpl(CUOLFoneAccountPtr pAccount);
	BOOL IsValidPhoneNumberImpl(const CString& strPhoneNumber);
	BOOL IsCallInProgress(CUOLFoneAccountPtr pAccount);
	BOOL MakePhoneCallImpl(IUOLMessengerContactPtr pContact);
	BOOL MakePhoneCallImpl(const CString& strPhoneNumber);
	BOOL HangUpPhoneCallImpl(CUOLFoneAccountPtr pAccount);
	BOOL AcceptIncomingCallImpl(CUOLFoneAccountPtr pAccount);
	BOOL RejectIncomingCallImpl(CUOLFoneAccountPtr pAccount);
	BOOL TransferCallImpl(const CString& strPhoneNumber);
	BOOL HoldUpPhoneCallImpl(CUOLFoneAccountPtr pAccount);
	BOOL ResumeUpPhoneCallImpl(CUOLFoneAccountPtr pAccount);
	BOOL MakeConferenceCallImpl(const CString& strPhoneNumber01, const CString& strPhoneNumber02);
	BOOL HangUpConferenceCallImpl(CUOLFoneAccountPtr pAccount);

	void StopSendRingingNotificationTimer();
	
	void UpdateServerConfig();

	void GetDevices(EnumAudioDeviceType audioDeviceType, CAtlList<CString>& listDevices);

	void NotifyPhoneLineRegistered(CUOLFoneAccountPtr pAccount);

	void NotifyInitializationCompleted();
	void NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCode initStatusCode);
	void NotifyAccountConnectionStateChanged(CUOLFoneAccountPtr pAccount);
	void NotifyAccountInfoReceived(CUOLFoneAccountPtr pAccount);
	void NotifyAccountErrorReceived(CUOLFoneAccountPtr pAccount);
	void NotifyPhoneCallStateChanged(CPhoneCallPtr pPhoneCall);
	void NotifyPhoneCallInfoReceived(CPhoneCallPtr pPhoneCall);
	void NotifyPhoneCallErrorReceived(CPhoneCallPtr pPhoneCall);
	void NotifyPhoneCallServerErrorReceived(CPhoneCallPtr pPhoneCall);
	void NotifyExecutionFailure(IUOLFoneControllerCallback::ExecStatusCode execStatusCode);

	CString GetInstallCookie();
	
	HRESULT SetupNetworkConfig();
	
	void ExtractAdditionalErrorInfo(IUOLFoneClientExtraResponseData2* pExtraResponseData);
	
	/*
	 * OsipToUIMLogLevel
	 * 
	 *		Translates the log id received from Wengo libraries to 
	 *		a id recognizable by UIM log infraestructure.
	 * 
	 * @param	lOsipLogLevel Log level id from Wengo libraries.
	 *
	 * @return	Log level id recognized by UIM log infraestructure.
	 */
	int OsipToUIMLogLevel(LONG lOsipLogLevel);
	
	/*
	 * UIMToOsipLogLevel
	 * 
	 *		Translates the log id received from UIM log infraestructure 
	 *		to a id recognizable by Wengo libraries.
	 * 
	 * @param	iUIMLogLevel Log level id from UIM log infraestructure.
	 *
	 * @return	Log level id recognized by Wengo libraries.
	 */
	int UIMToOsipLogLevel(int iUIMLogLevel);
	
private:
	int			m_nInternalStatus;

	BOOL		m_bCheckCodecUpdates;
	int			m_nUpdatedCodecsCount;

	CString		m_strUIMLibDir;
	IUOLFoneControllerCallback*		m_pControllerCallback;

	CUOLFoneAccountManagerPtr	m_pUOLFoneAccountManager;
	CUOLFoneAccountPtr			m_pConnectionPendingAccount;
	CUOLFoneAccountPtr			m_pDisconnectionPendingAccount;
	CServerConfigPtr			m_pServerConfig;
	UINT						m_nTimerSendRingingNotification;
	UINT						m_nPhoneRingingCount;

	CComPtr<IUOLFoneServerConfigProvider>	m_spUOLFoneServerConfigProvider;
	CComPtr<IUOLFoneClient>					m_spUOLFoneClient;

	CComObject<CUOLFoneClientCallback>*				m_pCallback;
	CComObject<CDownloadCallback>*					m_pDownloadCallback;
	CComObject<CCodecsConfigFileUpdaterCallback>*	m_pCodecsConfigFileUpdaterCallback;
	CComObject<CCodecsUpdaterCallback>*				m_pCodecsUpdaterCallback;

	CComPtr<IUOLFoneDownload>					m_spPhoneRulesDownload;
	CComPtr<IUOLFonePhoneValidator>				m_spPhoneValidator;
	CComPtr<IUOLFoneCodecsConfigFileUpdater3>	m_spCodecsConfigFileUpdater;
	CComPtr<IUOLFoneCodecsUpdater3>				m_spCodecsUpdater;

	CComPtr<IUOLFoneClientConnectionTestResult>		m_spConnectionTestResult;
	CComObject<CPhoneLineConfig>*					m_pPhoneLineConfig;

	CComObject<CPhoneLineInfo>*				m_pPhoneLineInfo;
	CComObject<CUserInfo>*					m_pUserInfo;

	CComObject<CNetworkConfig>*				m_pNetworkConfig;
	CComObject<CNetworkConfig>*				m_pMediaNetworkConfig;

	CComPtr<IUOLFoneClientPhoneLine>		m_spPhoneLine;
	CComPtr<IUOLFoneClientPhoneCall>		m_spPhoneCall;

	CVerifyDllsThread						m_VerifyDllsThread;

	CAccountStateChangeHandler				m_accountStateChangeHandler;
	CPhoneCallStateChangeHandler			m_phoneCallStateChangeHandler;
	CServerMessageHandler					m_serverMessageHandler;
	
	CString					m_pPendingCallNumber;
	BOOL					m_bCapabilitiesPublished;
	BOOL					m_bIsCallPresenceDependent;
	
	CUOLFoneFailureInfoPtr					m_pLastFailureInfo;
	
	IUOLMessengerContactPtr					m_pPendingCallContact;
};