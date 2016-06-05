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

#include <phapi/phapi.h>
#include <osipparser2/osip_port.h>
#include "interfaces/IUOLFoneClientNetworkConfig.h"
#include "interfaces/IUOLFoneClientPhoneCall.h"
#include "interfaces/IUOLFoneClientPhoneLineInfo.h"
#include "interfaces/IUOLFoneClientPhoneLineConfig.h"


#define			INVALID_PHONE_CALL_ID				-1


class CPhApiAdapter
{
public:
	CPhApiAdapter();
	virtual ~CPhApiAdapter();

	int Initialize(IUOLFoneClientPhoneLineConfig* pPhoneLineConfig, BSTR bstrCodecsPath);
	void Finalize();
	BOOL IsInitialized();
	BOOL IsFinalizeInProgress();

	int RegisterPhoneLine(IUOLFoneClientPhoneLineInfo* pPhoneLineInfo, IUOLFoneClientNetworkConfig* pNetworkConfig);
	int UnregisterPhoneLine(int nPhoneLineId);

	BOOL SetMediaNetworkConfig(IUOLFoneClientNetworkConfig* pNetworkConfig);

	int MakeCall(int nPhoneLineId, const CString& strSipAddress, BOOL bEnableVideo);
	int AcceptCall(int nPhoneCallId, BOOL bEnableVideo);
	int RejectCall(int nPhoneCallId, LONG lReason);
	int CloseCall(int nPhoneCallId);
	int HoldCall(int nPhoneCallId);
	int ResumeCall(int nPhoneCallId);
	int TransferCall(int nPhoneCallId, const CString& strPhoneUri);
	int SendRingingNotification(int nPhoneCallId);
	int ReconfigureCallMedia(int nPhoneCallId);
	//int JoinConferenceCall(int nPhoneCallId_01, int nPhoneCallId_02);
	//int SplitConferenceCall(int nPhoneCallId_01, int nPhoneCallId_02);
	int CreateConferenceCall(int nPhoneCallId);
	//int ConferenceInvite(int nPhoneLineId, int nConferenceCallId, const CString& strSipAddress, BOOL bEnableVideo);
	int ConferenceAddMember(int nConferenceCallId, int nPhoneCallId);
	int ConferenceRemoveMember(int nConferenceCallId, int nPhoneCallId);
	int DestroyConferenceCall(int nConferenceCallId);
	int SetLineCallForwardNumber(int nPhoneLineId, const CString& strCallForwardNumber);
	int SetLineBusyFlag(int nPhoneLineId, BOOL bIsBusy);
	
	int PlayTone(int nPhoneCallId, EnumTone tone);

	BOOL SetCallInputAudioDevice(const CString& strDeviceName);
	BOOL SetCallOutputAudioDevice(const CString& strDeviceName);

	void EnableAcousticEchoCancellation(BOOL bEnable);
	BOOL IsAcousticEchoCancellationEnabled();
	
	void EnableHalfDuplex(BOOL bEnable);
	BOOL IsHalfDuplexEnabled();

	void SetAudioCodecsOrder(const CString& strCodecs);
	CString GetAudioCodecsOrder();

	void SetVideoCodecsOrder(const CString& strCodecs);
	CString GetVideoCodecsOrder();

	CString GetSupportedAudioCodecs() const;
	CString GetSupportedVideoCodecs() const;

	int FindAvailableSipPort();

	BOOL SendInstantMessage(int nPhoneLineId, const CString& strMessage, const CString& strUserURI);
	
	BOOL SetMicMute(BOOL bMute);
	BOOL GetMicMute();
	
	void FreeVoipLines();

private:
	static void CallProgress(int nCallId, const phCallStateInfo_t* pCallStateInfo);
	static void TransferProgress(int nCallId, const phTransferStateInfo_t* pTransferStateinfo);
	static void ConferenceProgress(int nConferenceId, const phConfStateInfo_t* pConfStateinfo);
	static void RegistrationProgress(int nRegId, const phRegStateInfo_t* pRegStateInfo);
	static void MessageProgress(int nMessageId,  const phMsgStateInfo_t* pMsgStateinfo);
	static void OnNotify(const char* pszEvent, const char* pszFrom, const char* pszContent);
	static void SubscriptionProgress(int nSubscriptionId, const phSubscriptionStateInfo_t* pSubscriptionStateinfo);
	static void OnVideoFrameReady(int nCallId, phVideoFrameReceivedEvent_t* pVideoFrameReceivedEvent);
	static void ErrorNotify(enum phErrors error);
	static void DebugTrace(const char* pszMessage);
	//static void CallStreamError(int nCallId, const phCallStreamErrorInfo_t* pCallStreamErrorInfo);
	
	static void OsipDebugMethod(char *pszFileName, int iLineNumber, 
			osip_trace_level_t uiLevel, char *pszFormat, va_list vaList);
	
	void SetInitialized(BOOL bInitialized);
	void SetupNetwork(IUOLFoneClientNetworkConfig* pNetworkConfig, BOOL bMedia);
	TCHAR ToneToChar(EnumTone tone);
	int ChangeAudioDevices(const CString& strInputAudioDeviceId, const CString& strOutputAudioDeviceId);

	enum
	{
		DEFAULT_SIP_PORT = 5060
	};

private:
	static phCallbacks_t	ms_phCallbacks;
	static CString			ms_strDefaultAudioCodecs;
	static CString			ms_strDefaultVideoCodecs;
	static CString			ms_strSupportedAudioCodecs;
	static CString			ms_strSupportedVideoCodecs;
	
	BOOL		m_bInitialized;
	BOOL		m_bFinalizeInProgress;
	CString		m_strInputAudioDeviceId;
	CString		m_strOutputAudioDeviceId;
	CString		m_strAudioCodecsPrecedence;
	CString		m_strVideoCodecsPrecedence;
	
	int			m_iParticipantCall_01;
	int			m_iParticipantCall_02;
};