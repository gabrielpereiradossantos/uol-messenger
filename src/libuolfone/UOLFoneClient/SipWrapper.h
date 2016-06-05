/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#pragma once

#include "interfaces/IUOLFoneClient.h"
#include "IPhApiAdapterCallback.h"
#include "PhApiAdapter.h"
#include "PhoneLineManager.h"
#include "PhoneCallManager.h"
#include "Preferences.h"
#include "ConferenceCall.h"


class CSipWrapper :
	public IPhApiAdapterCallback,
	public CWindowImpl<CSipWrapper>
	// : public IUOLFoneClient
{
public:
	DECLARE_WND_CLASS("SipWrapper");

	virtual ~CSipWrapper();

	static CSipWrapper* GetInstance();
	static void ReleaseInstance();

	IPhApiAdapterCallback* GetCallback();
	BOOL IsFinalizeInProgress();

	// Interface IUOLFoneClient
	//
	STDMETHOD(Initialize)(IUOLFoneClientCallback* pCallback, BSTR bstrCodecsPath);
	STDMETHOD(Finalize)(void);
	STDMETHOD(TestConnection)(IUOLFoneClientConnectionTestParam* pConnectionTestParam, 
							  IUOLFoneClientConnectionTestResult** ppConnectionTestResult);
	STDMETHOD(EnumerateAudioCodecs)(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum);
	//STDMETHOD(EnumerateVideoCodecs)(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum);
	STDMETHOD(GetAudioCodecsPrecedence)(BSTR* pbstrAudioCodecs);
	STDMETHOD(GetVideoCodecsPrecedence)(BSTR* pbstrVideoCodecs);
	STDMETHOD(InitializePhoneLines)(IUOLFoneClientPhoneLineConfig* pPhoneLineConfig);
	STDMETHOD(FinalizePhoneLines)();

	STDMETHOD(RegisterPhoneLine)(IUOLFoneClientPhoneLineInfo* pPhoneLineInfo, IUOLFoneClientNetworkConfig* pNetworkConfig, IUOLFoneClientPhoneLine** ppPhoneLine);
	STDMETHOD(UnregisterPhoneLine)(IUOLFoneClientPhoneLine* pPhoneLine);
	STDMETHOD(MakePhoneCall)(IUOLFoneClientPhoneLine* pPhoneLine, 
							 BSTR bstrPhoneURI,
							 BOOL bEnableVideo,
							 IUOLFoneClientPhoneCall** ppPhoneCall);
	STDMETHOD(SetMediaNetworkConfig)(IUOLFoneClientNetworkConfig* pNetworkConfig);
	STDMETHOD(AcceptPhoneCall)(IUOLFoneClientPhoneCall* pPhoneCall,	BOOL bEnableVideo);
	STDMETHOD(RejectPhoneCall)(IUOLFoneClientPhoneCall* pPhoneCall, EnumFailureCode rejectReason);
	STDMETHOD(ClosePhoneCall)(IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(HoldPhoneCall)(IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(ResumePhoneCall)(IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(TransferPhoneCall)(IUOLFoneClientPhoneCall* pPhoneCall, BSTR bstrPhoneURI);
	STDMETHOD(ReconfigureCallMedia)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientNetworkConfig* pNetworkConfig);
	STDMETHOD(PlayTone)(IUOLFoneClientPhoneCall* pPhoneCall, EnumTone tone);
	//STDMETHOD(PlaySoundFile)(IUOLFoneClientPhoneCall* pPhoneCall, BSTR bstrSoundFilePath);
	STDMETHOD(GetAudioCodec)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo);
	STDMETHOD(GetVideoCodec)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo);
	STDMETHOD(EnumerateAudioDevices)(EnumAudioDeviceType audioDeviceType, IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum);
	//STDMETHOD(EnumerateVideoDevices)(IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum);
	STDMETHOD(SetAudioDevice)(EnumAudioDeviceType audioDeviceType, BSTR bstrDeviceName);
	STDMETHOD(GetAudioDevice)(EnumAudioDeviceType audioDeviceType, BSTR* pbstrDeviceName);
	STDMETHOD(EnableAcousticEchoCancellation)(BOOL bEnable);
	STDMETHOD(IsAcousticEchoCancellationEnabled)(BOOL* pbEnabled);
	STDMETHOD(EnableHalfDuplex)(BOOL bEnable);
	STDMETHOD(IsHalfDuplexEnabled)(BOOL* pbEnabled);
	STDMETHOD(SetVolumeLevel)(EnumAudioDeviceType audioDeviceType, LONG lLevel);
	STDMETHOD(GetVolumeLevel)(EnumAudioDeviceType audioDeviceType, LONG* plLevel);
	STDMETHOD(SetMute)(EnumAudioDeviceType audioDeviceType, BOOL bMute);
	STDMETHOD(GetMute)(EnumAudioDeviceType audioDeviceType, BOOL* pbMute);
	//STDMETHOD(SetVideoDevice)(BSTR bstrDeviceName);
	//STDMETHOD(GetVideoDevice)(BSTR* pbstrDeviceName);
	//STDMETHOD(SetVideoQuality)(LONG lVideoQuality);
	//STDMETHOD(GetVideoQuality)(LONG* plVideoQuality);
	STDMETHOD(SendInstantMessage)(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrMessage, BSTR bstrUserURI);
	STDMETHOD(SendRingingNotification)(IUOLFoneClientPhoneCall* pPhoneCall);
	//STDMETHOD(JoinConferenceCall)(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02);
	//STDMETHOD(SplitConferenceCall)(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02);
	STDMETHOD(CreateConferenceCall)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientConferenceCall** pConferenceCall);
	//STDMETHOD(ConferenceInvite)(IUOLFoneClientPhoneLine* pPhoneLine, 
	//							IUOLFoneClientConferenceCall* pConferenceCall,
	//							BSTR bstrPhoneURI,
	//							BOOL bEnableVideo,
	//							IUOLFoneClientPhoneCall** ppPhoneCall);
	STDMETHOD(ConferenceAddMember)(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(ConferenceRemoveMember)(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(DestroyConferenceCall)(IUOLFoneClientConferenceCall* pConferenceCall);
	STDMETHOD(SetLineCallForwardNumber)(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrCallForwardNumber);
	STDMETHOD(SetLineBusyFlag)(IUOLFoneClientPhoneLine* pPhoneLine, BOOL bIsBusy);

	// Interface IUOLFoneClient2
	//
	STDMETHOD(Initialize)(IUOLFoneClientCallback2* pCallback2, 
						  BSTR bstrCodecsPath, 
						  LONG lLogLevel);

protected:
	CSipWrapper();

	enum
	{
		WM_USER_PHONE_LINE_EVENT = WM_USER + 0x100,
		WM_USER_PHONE_CALL_EVENT,
		WM_USER_PHONE_CALL_ASYNC_EVENT,
		WM_USER_MESSAGE_PROGRESS_EVENT,
		WM_USER_PHONE_CALL_STREAM_ERROR,
		WM_USER_PHONE_LINE_CONNECTION_TIMEOUT,
		WM_USER_OSIP_DEBUG_EVENT
	};

	BEGIN_MSG_MAP(CSipWrapper)
		MESSAGE_HANDLER(WM_USER_PHONE_LINE_EVENT, OnPhoneLineEvent)
		MESSAGE_HANDLER(WM_USER_PHONE_CALL_EVENT, OnPhoneCallEvent)
		MESSAGE_HANDLER(WM_USER_PHONE_CALL_ASYNC_EVENT, OnPhoneCallAsyncEvent)
		MESSAGE_HANDLER(WM_USER_MESSAGE_PROGRESS_EVENT, OnMessageProgressEvent)
		MESSAGE_HANDLER(WM_USER_PHONE_CALL_STREAM_ERROR, OnPhoneCallStreamError)
		MESSAGE_HANDLER(WM_USER_PHONE_LINE_CONNECTION_TIMEOUT, OnPhoneLineConnectionTimeout)
		MESSAGE_HANDLER(WM_USER_OSIP_DEBUG_EVENT, OnOsipDebugEvent)
	END_MSG_MAP()

	LRESULT OnPhoneLineEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPhoneCallEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPhoneCallAsyncEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMessageProgressEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPhoneCallStreamError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPhoneLineConnectionTimeout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOsipDebugEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	virtual void HandlePhoneLineEvent(const IPhoneLineProgressInfo* pPhoneLineProgressInfo);
	virtual void HandleCallProgressEvent(const IPhoneCallProgressInfo* pPhoneCallProgressInfo);
	virtual void HandleMessageProgressEvent(const IMessageProgressInfo* pMessageProgressInfo);
	virtual void HandleCallStreamError(LONG lPhoneLineId, int nCallId, EnumPhoneCallStreamError error);
	virtual void HandleOsipDebugEvent(const IOsipDebugInfo* pOsipDebugInfo);

	IUOLFoneClientPhoneLine* GetPhoneLine(LONG nPhoneLineId);

	LONG GetLogLevel();

private:
	CComPtr<IUOLFoneClientCallback>		m_spCallback;

	BOOL				m_bFinalizeInProgress;

	CPhApiAdapter		m_phApiAdapter;
	CPhoneLineManager	m_phoneLineManager;
	CPhoneCallManager	m_phoneCallManager;
	CPreferences		m_preferences;
	CComBSTR			m_bstrCodecsPath;

	static CSipWrapper* m_pInstance;
	
	CComObject<CConferenceCall>* m_pConferenceCallObj;
	
	LONG				m_lLogLevel;
};
