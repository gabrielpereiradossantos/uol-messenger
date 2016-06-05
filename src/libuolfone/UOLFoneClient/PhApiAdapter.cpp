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


#include "stdafx.h"
#include "PhApiAdapter.h"

#include "resource.h"
#include "SipWrapper.h"
#include "IPhApiAdapterCallback.h"
#include <sound/AudioDevice.h>
#include <sound/AudioDeviceManager.h>
#include <netlib.h>
#include <FileVersion.h>
#include "PhPhoneLineProgressInfo.h"
#include "PhPhoneCallProgressInfo.h"
#include "PhMessageProgressInfo.h"
#include "PhOsipDebugInfo.h"


phCallbacks_t CPhApiAdapter::ms_phCallbacks =
{
	CallProgress,
	TransferProgress,
	ConferenceProgress,
	RegistrationProgress,
	MessageProgress,
	OnNotify,
	SubscriptionProgress,
	OnVideoFrameReady,
	ErrorNotify,
	DebugTrace/*,
	CallStreamError*/
};

//CString CPhApiAdapter::ms_strDefaultAudioCodecs = _T("PCMA/8000,PCMU/8000");
CString CPhApiAdapter::ms_strDefaultAudioCodecs = _T("ILBC/8000,G729/8000,PCMA/8000,PCMU/8000,GSM/8000");
//CString CPhApiAdapter::ms_strDefaultAudioCodecs = _T("PCMU/8000,PCMA/8000,GSM/8000,ILBC/8000,SPEEX/16000,SPEEX/8000,AMR/8000,AMR-WB/16000");

CString CPhApiAdapter::ms_strDefaultVideoCodecs = _T("");

//CString	CPhApiAdapter::ms_strSupportedAudioCodecs = _T("PCMU/8000,PCMA/8000,GSM/8000,ILBC/8000,SPEEX/16000,SPEEX/8000,AMR/8000,AMR-WB/16000,G729/8000");
CString	CPhApiAdapter::ms_strSupportedAudioCodecs = _T("PCMU/8000,PCMA/8000,GSM/8000,ILBC/8000,SPEEX/16000,SPEEX/8000,G729/8000");

CString	CPhApiAdapter::ms_strSupportedVideoCodecs = _T("");



void CPhApiAdapter::CallProgress(int nCallId, const phCallStateInfo_t* pCallStateInfo)
{
	//...
	ATLTRACE(_T("%s - nCallId = %d, nCallStateInfo = %d\n"), __FUNCTION__, nCallId, pCallStateInfo->event);

	if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
	{
		return;
	}

	IPhApiAdapterCallback* pCallback = CSipWrapper::GetInstance()->GetCallback();

	if (pCallback != NULL)
	{
		CPhPhoneCallProgressInfo phoneCallProgressInfo(nCallId, pCallStateInfo);
		
		EnumPhoneCallState state = phoneCallProgressInfo.GetCallState();
		
		if (state != UFC_PHONE_CALL_STATE_UNKNOWN)
		{
			if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
			{
				return;
			}
			
			pCallback->HandleCallProgressEvent(&phoneCallProgressInfo);
		}
		else
		{
			ATLTRACE(_T("UFC_PHONE_CALL_STATE_UNKNOWN state\n"));
		}
	}
}


void CPhApiAdapter::TransferProgress(int nCallId, const phTransferStateInfo_t* pTransferStateinfo)
{
	ATLTRACE(_T("%s - call_id = %d\n"), __FUNCTION__, nCallId);
}


void CPhApiAdapter::ConferenceProgress(int nConferenceId, const phConfStateInfo_t* pConfStateinfo)
{
	ATLTRACE(_T("%s - conference_id = %d\n"), __FUNCTION__, nConferenceId);
}


void CPhApiAdapter::RegistrationProgress(int nRegId, const phRegStateInfo_t* pRegStateInfo)
{
	//...
	ATLTRACE(_T("%s - nRegid = %d, nRegStatus = %d\n"), __FUNCTION__, nRegId, pRegStateInfo->regStatus);

	if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
	{
		return;
	}

	IPhApiAdapterCallback* pCallback = CSipWrapper::GetInstance()->GetCallback();

	if (pCallback != NULL)
	{
		CPhPhoneLineProgressInfo phoneLineProgressInfo(pRegStateInfo);

		EnumPhoneLineState state = phoneLineProgressInfo.GetState();

		if (state != UFC_PHONE_LINE_STATE_UNKNOWN)
		{
			if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
			{
				return;
			}

			pCallback->HandlePhoneLineEvent(&phoneLineProgressInfo);
		}
	}
}


void CPhApiAdapter::MessageProgress(int nMessageId,  const phMsgStateInfo_t* pMsgStateInfo)
{
	if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
	{
		return;
	}

	IPhApiAdapterCallback* pCallback = CSipWrapper::GetInstance()->GetCallback();

	if (pCallback != NULL)
	{
		CPhMessageProgressInfo messageProgressInfo(pMsgStateInfo);

		pCallback->HandleMessageProgressEvent(&messageProgressInfo);
	}
}


void CPhApiAdapter::OnNotify(const char* pszEvent, const char* pszFrom, const char* pszContent)
{
	ATLTRACE(_T("%s - event = %s\n"), __FUNCTION__, pszEvent);
}


void CPhApiAdapter::SubscriptionProgress(int nSubscriptionId, const phSubscriptionStateInfo_t* pSubscriptionStateinfo)
{
	ATLTRACE(_T("%s - id = %d\n"), __FUNCTION__, nSubscriptionId);
}


void CPhApiAdapter::OnVideoFrameReady(int nCallId, phVideoFrameReceivedEvent_t* pVideoFrameReceivedEvent)
{
	ATLTRACE(_T("%s - call_id = %d\n"), __FUNCTION__, nCallId);
}


void CPhApiAdapter::ErrorNotify(enum phErrors error)
{
	ATLTRACE(_T("%s - error = %d\n"), __FUNCTION__, error);
}


void CPhApiAdapter::DebugTrace(const char* pszMessage)
{
	ATLTRACE(_T("%s - msg = %s\n"), __FUNCTION__, pszMessage);
}


/*
void CPhApiAdapter::CallStreamError(int nCallId, const phCallStreamErrorInfo_t* pCallStreamErrorInfo)
{
	ATLTRACE(_T("%s - call_id = %d\n"), __FUNCTION__, nCallId);

	if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
	{
		return;
	}

	if (pCallStreamErrorInfo)
	{
		IPhApiAdapterCallback* pCallback = CSipWrapper::GetInstance()->GetCallback();

		if (pCallback != NULL)
		{
			EnumPhoneCallStreamError error = UFC_PHONE_CALL_STREAM_AUDIO_RECEIVE_TIMEOUT;

			if (pCallStreamErrorInfo->error == phSTREAMVIDEORECEIVETIMEOUT)
			{
				error = UFC_PHONE_CALL_STREAM_VIDEO_RECEIVE_TIMEOUT;
			}

			pCallback->HandleCallStreamError(pCallStreamErrorInfo->vlid, nCallId, error);
		}
	}
}
*/


void CPhApiAdapter::OsipDebugMethod(char *pszFileName, int iLineNumber, 
		osip_trace_level_t uiLevel, char *pszFormat, va_list vaList)
{
	ATLTRACE(_T("%s - pszFileName = %s, iLineNumber = %d, uiLevel = %d, pszFormat = %s.\n"), 
			__FUNCTION__, pszFileName, iLineNumber, uiLevel, pszFormat);
	
	if (CSipWrapper::GetInstance()->IsFinalizeInProgress())
	{
		return;
	}
	
	IPhApiAdapterCallback* pCallback = 
			CSipWrapper::GetInstance()->GetCallback();
	
	if (pCallback != NULL)
	{
		int iLogMessageFinalSize = _vscprintf(pszFormat, vaList) + 1;
		char* pszLogMessage = (char*)malloc(iLogMessageFinalSize);
		
		if (pszLogMessage != NULL)
		{
			memset(pszLogMessage, 0, iLogMessageFinalSize);
			
			vsprintf(pszLogMessage, pszFormat, vaList);
			
			CPhOsipDebugInfo* pOsipDebugInfo = 
					new CPhOsipDebugInfo(pszFileName, 
							iLineNumber, uiLevel, pszLogMessage);
			
			pCallback->HandleOsipDebugEvent(pOsipDebugInfo);
			
			free(pszLogMessage);
		}
	}
}



CPhApiAdapter::CPhApiAdapter() : 
	m_bInitialized(FALSE),
	m_bFinalizeInProgress(FALSE),
	m_strInputAudioDeviceId("0"),
	m_strOutputAudioDeviceId("0"),
	m_iParticipantCall_01(INVALID_PHONE_CALL_ID),
	m_iParticipantCall_02(INVALID_PHONE_CALL_ID)
{
	m_strAudioCodecsPrecedence = ms_strDefaultAudioCodecs;
	m_strVideoCodecsPrecedence = ms_strDefaultVideoCodecs;
}


CPhApiAdapter::~CPhApiAdapter()
{
}


int CPhApiAdapter::Initialize(IUOLFoneClientPhoneLineConfig* pPhoneLineConfig, BSTR bstrCodecsPath)
{
	// Initialize WinSock...
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -2; 
	}

	// Get PhoneAPI config struct
	phConfig_t* pPhConfig = phGetConfig();

	if (!pPhConfig)
	{
		return -3;
	}

	::ZeroMemory(pPhConfig, sizeof(phConfig_t));

	// Plugin path...
	TCHAR pszModulePath[MAX_PATH + 1] = {0};

	if (0 == ::GetModuleFileName(_AtlBaseModule.GetModuleInstance(), pszModulePath, MAX_PATH))
	{
		return -4;
	}

	CString strCodecsPath(bstrCodecsPath);
	CString strModulePath = pszModulePath;
	
	int nFind = strModulePath.ReverseFind('\\');
	strModulePath = strModulePath.Left(nFind);

	strncpy(pPhConfig->plugin_path, (LPCTSTR) strCodecsPath, sizeof(pPhConfig->plugin_path));
	strncpy(pPhConfig->uol_library_search_path, (LPCTSTR) strModulePath, sizeof(pPhConfig->uol_library_search_path));

	// Sip config init...
	LONG lLocalPort = 0;

	if (pPhoneLineConfig)
	{
		HRESULT hr = pPhoneLineConfig->GetConnectionPort(&lLocalPort);

		CComBSTR bstrAudioCodecsPrecedence;
		hr = pPhoneLineConfig->GetAudioCodecsPrecedence(&bstrAudioCodecsPrecedence);

		if ((SUCCEEDED(hr)) && 
			(bstrAudioCodecsPrecedence.Length() > 0))
		{
			// TODO: validate codecs
			SetAudioCodecsOrder(CString(bstrAudioCodecsPrecedence));
		}

		CComBSTR bstrVideoCodecsPrecedence;
		hr = pPhoneLineConfig->GetVideoCodecsPrecedence(&bstrVideoCodecsPrecedence);

		if ((SUCCEEDED(hr)) &&
			(bstrVideoCodecsPrecedence.Length() > 0))
		{
			// TODO: validate codecs
			SetVideoCodecsOrder(CString(bstrVideoCodecsPrecedence));
		}

		CComPtr<IUOLFoneClientNetworkConfig> spServerNetworkConfig;
		hr = pPhoneLineConfig->GetServerNetworkConfig(&spServerNetworkConfig);

		if ((SUCCEEDED(hr)) && spServerNetworkConfig)
		{
			SetupNetwork(spServerNetworkConfig, FALSE);
			
			spServerNetworkConfig.Release();
		}

		CComPtr<IUOLFoneClientNetworkConfig> spMediaNetworkConfig;
		hr = pPhoneLineConfig->GetMediaNetworkConfig(&spMediaNetworkConfig);

		if ((SUCCEEDED(hr)) && spMediaNetworkConfig)
		{
			SetupNetwork(spMediaNetworkConfig, TRUE);
			
			spMediaNetworkConfig.Release();
		}
	}

	if (lLocalPort == 0)
	{
		lLocalPort = FindAvailableSipPort();
	}

	CString strSipPort;
	strSipPort.Format("%d", lLocalPort);

	strncpy(pPhConfig->proxy, "", sizeof(pPhConfig->proxy));
	strncpy(pPhConfig->sipport, strSipPort, sizeof(pPhConfig->sipport));
	strncpy(pPhConfig->local_rtp_port, "10600", sizeof(pPhConfig->local_rtp_port));
	strncpy(pPhConfig->local_video_rtp_port, "10700", sizeof(pPhConfig->local_video_rtp_port));

	// Codec list
	//CString tempCodecs("G726-64wb/16000,AMR-WB/16000,PCMU/8000,PCMA/8000,AMR/8000,GSM/8000");
	//CString tempCodecs("GSM/8000,ILBC/8000,G729/8000,PCMA/8000,PCMU/8000");
	//CString tempCodecs("ILBC/8000,G729/8000,PCMA/8000,PCMU/8000,GSM/8000"); // Original
	//strncpy(pPhConfig->audio_codecs, tempCodecs, tempCodecs.GetLength());
	strncpy(pPhConfig->audio_codecs, (LPCTSTR) m_strAudioCodecsPrecedence, sizeof(pPhConfig->audio_codecs));
	strncpy(pPhConfig->video_codecs, (LPCTSTR) m_strVideoCodecsPrecedence, sizeof(pPhConfig->video_codecs));

	// auto,none,fcone,rcone,prcone,sym
	strncpy(pPhConfig->nattype, "sym", sizeof(pPhConfig->nattype));
	//strncpy(pPhConfig->nattype, "auto", sizeof(pPhConfig->nattype));
	//strncpy(pPhConfig->stunserver, "200.221.0.130", sizeof(pPhConfig->stunserver));
	pPhConfig->nat_refresh_time = 25;

	CFileVersion fv(pszModulePath);
	ULONGLONG lVersion = fv.GetVersion();
	USHORT* pnVersion = reinterpret_cast<USHORT*>(&lVersion);

	CString strAppName;
	strAppName.LoadString(IDS_PROJNAME);

	CString strVersion;
	strVersion.Format(_T("%s(%d.%d.%d.%d)"), strAppName, pnVersion[3], pnVersion[2], pnVersion[1], pnVersion[0]);

	strncpy(pPhConfig->user_agent, (LPCTSTR) strVersion, sizeof(pPhConfig->user_agent));

	pPhConfig->rtp_receive_timeout = 10;

	//pPhConfig->use_tunnel = 1;

	// Set tunnel timeouts (in seconds)
	pPhConfig->tunnel_keep_alive_timeout = 60;
	pPhConfig->rtp_tunnel_connect_keep_alive_timeout = 30;
	pPhConfig->rtp_tunnel_accept_keep_alive_timeout = 5 * 60;
	pPhConfig->rtp_tunnel_connection_accept_timeout = 5 * 60;

	//EnableAcousticEchoCancellation(TRUE);

	m_bFinalizeInProgress = FALSE;

	// Use phAPI in asynchronous mode
	int nInit = phInit(&ms_phCallbacks, "", TRUE);

	int iLogLevel = -1;
	if ((iLogLevel = CSipWrapper::GetInstance()->GetLogLevel()) > 0)
	{
		// Increments the log level informed on method below by "1" 
		// because it make the activation considering only the levels 
		// that are lower. (i.e. the level informed is not considered 
		// during the activation).
		//
		osip_trace_initialize_func(
				(osip_trace_level_t)(iLogLevel + 1), 
				&CPhApiAdapter::OsipDebugMethod);
	}

	if (0 == nInit)
	{
		SetInitialized(TRUE);
	}

	return nInit;
}


void CPhApiAdapter::Finalize()
{
	m_bFinalizeInProgress = TRUE;

	phTerminate();

	if (CSipWrapper::GetInstance()->GetLogLevel() > 0)
	{
		// The method below considers only the levels below the 
		// informed one.
		// So passing "TRACE_LEVEL0" as the parameter, that is 
		// the lowest value, will result on logging for all 
		// levels being deactivated
		//
		osip_trace_initialize_func(TRACE_LEVEL0, NULL);
	}

	WSACleanup();

	SetInitialized(FALSE);
	
	m_bFinalizeInProgress = FALSE;
}


BOOL CPhApiAdapter::IsInitialized()
{
	return m_bInitialized;
}


BOOL CPhApiAdapter::IsFinalizeInProgress()
{
	return m_bFinalizeInProgress;
}


int CPhApiAdapter::RegisterPhoneLine(IUOLFoneClientPhoneLineInfo* pPhoneLineInfo, IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	int nVirtualLineId = -1;

	if ((pPhoneLineInfo != NULL) && (IsInitialized()))
	{
		CComPtr<IUOLFoneClientUserInfo> pUserInfo;

		HRESULT hr = pPhoneLineInfo->GetUserInfo(&pUserInfo);

		if (SUCCEEDED(hr) && pUserInfo)
		{
			CComPtr<IUOLFoneClientServerInfo> pServerInfo;

			hr = pPhoneLineInfo->GetServerInfo(&pServerInfo);

			if (SUCCEEDED(hr) && pServerInfo)
			{
				CComBSTR bstrDisplayName, bstrUsername;
				CComBSTR bstrIdentity, bstrPassword, bstrRealm;

				CComBSTR bstrServerHost;
				LONG lServerPort = 0;

				LONG lRegisterTimeout = 29 * 60;

				// Get user info...
				pUserInfo->GetDisplayName(&bstrDisplayName);
				pUserInfo->GetUsername(&bstrUsername);
				pUserInfo->GetIdentity(&bstrIdentity);
				pUserInfo->GetPassword(&bstrPassword);
				pUserInfo->GetRealm(&bstrRealm);

				// Get SIP register info...
				pServerInfo->GetServerHost(&bstrServerHost);
				pServerInfo->GetServerPort(&lServerPort);

				// Get register timeout
				pPhoneLineInfo->GetRegisterTimeout(&lRegisterTimeout);

				// Get SIP proxy info (by default, same as register server)
				CString strProxyAddress;
				strProxyAddress.Format(_T("%s:%d"), CString(bstrServerHost), lServerPort);

				CComPtr<IUOLFoneClientServerInfo> pProxyInfo;

				hr = pPhoneLineInfo->GetProxyInfo(&pProxyInfo);

				if (SUCCEEDED(hr) && pProxyInfo)
				{
					CComBSTR bstrProxyHost;
					LONG lProxyPort = 0;

					pProxyInfo->GetServerHost(&bstrProxyHost);
					pProxyInfo->GetServerPort(&lProxyPort);

					if ((bstrProxyHost.Length() > 0) && (lProxyPort > 0))
					{
						strProxyAddress.Format(_T("%s:%d"), CString(bstrProxyHost), lProxyPort);
					}
				}

				// Setup Http tunneling config...
				if (pNetworkConfig)
				{
					SetupNetwork(pNetworkConfig, FALSE);
					SetupNetwork(pNetworkConfig, TRUE);
				}

				if (lServerPort != 0)
				{
					CString strServerAddress;
					strServerAddress.Format(_T("%s:%d"), CString(bstrServerHost), lServerPort);

					CString strIdentity(bstrIdentity);
					CString strUsername(bstrUsername);
					CString strDisplayName(bstrDisplayName);

					if ((FALSE == strDisplayName.IsEmpty()) && (-1 == strDisplayName.Find(_T("\""))))
					{
						strDisplayName = _T("\"") + strDisplayName + _T("\"");
					}

					phAddAuthInfo(strUsername, strIdentity, CString(bstrPassword), "", CString(bstrRealm));

					phFreeVlines();
					
					nVirtualLineId = phAddVline2(strDisplayName, strUsername, strServerAddress, strProxyAddress, lRegisterTimeout);

					ATLTRACE(_T("%s - nVirtualLineId = [%d]\n"), __FUNCTION__, nVirtualLineId);

					//IPhApiAdapterCallback* pCallback = CSipWrapper::GetInstance()->GetCallback();

					//if (pCallback != NULL)
					//{
					//	pCallback->HandlePhoneLineEvent(nVirtualLineId, UFC_PHONE_LINE_STATE_PROGRESS);
					//}
				}
			}
		}
	}

	return nVirtualLineId;
}


int CPhApiAdapter::UnregisterPhoneLine(int nPhoneLineId)
{
	int nRet = -1;

	ATLTRACE(_T("%s - nPhoneLineId = [%d]\n"), __FUNCTION__, nPhoneLineId);

	if (IsInitialized())
	{
		nRet = phDelVline(nPhoneLineId, -1);

		//ATLASSERT(0 == nRet);
		if (0 != nRet)
		{
			//PH_BADVLID = -4
			//PH_VLBUSY = -11
			ATLTRACE(_T("%s - phDelVline FAILED [%d]\n"), __FUNCTION__, nRet);
		}

		//if ()
		//{
		//	IPhApiAdapterCallback* pCallback = GetCallback();

		//	if (pCallback != NULL)
		//	{
		//		pCallback->HandlePhoneLineEvent(nPhoneLineId, UFC_PHONE_LINE_STATE_UNREGISTERED);
		//	}
		//}
	}

	return nRet;
}


BOOL CPhApiAdapter::SetMediaNetworkConfig(IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	SetupNetwork(pNetworkConfig, TRUE);

	return TRUE;
}


int CPhApiAdapter::MakeCall(int nPhoneLineId, const CString& strSipAddress, BOOL bEnableVideo)
{
	int nCallId = -1;

	if (IsInitialized())
	{
		int nMediaStreams = PH_STREAM_AUDIO;

		if (bEnableVideo)
		{
			nMediaStreams |= (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX);
		}

		nCallId = phLinePlaceCall2(nPhoneLineId, (LPCTSTR) strSipAddress, NULL, 0, nMediaStreams);
	}

	return nCallId;
}


int CPhApiAdapter::AcceptCall(int nPhoneCallId, BOOL bEnableVideo)
{
	int nMediaStreams = PH_STREAM_AUDIO;

	if (bEnableVideo)
	{
		nMediaStreams |= (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX);
	}

	return phAcceptCall3(nPhoneCallId, NULL, nMediaStreams);
}


int CPhApiAdapter::RejectCall(int nPhoneCallId, LONG lReason)
{
	return phRejectCall(nPhoneCallId, lReason);
}


int CPhApiAdapter::CloseCall(int nPhoneCallId)
{
	int nRet = -1;

	if (IsInitialized())
	{
		nRet = phCloseCall(nPhoneCallId);
	}

	return nRet;
}


int CPhApiAdapter::HoldCall(int nPhoneCallId)
{
	int nRet = -1;

	if (IsInitialized())
	{
		nRet = phHoldCall(nPhoneCallId);
	}

	return nRet;
}


int CPhApiAdapter::ResumeCall(int nPhoneCallId)
{
	int nRet = -1;

	if (IsInitialized())
	{
		nRet = phResumeCall(nPhoneCallId);
	}

	return nRet;
}


int CPhApiAdapter::TransferCall(int nPhoneCallId, const CString& strPhoneUri)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		nRet = phBlindTransferCall(nPhoneCallId, strPhoneUri);
	}
	
	return nRet;
}



int CPhApiAdapter::SendRingingNotification(int nPhoneCallId)
{
	return phRingingCall(nPhoneCallId);
}


int CPhApiAdapter::ReconfigureCallMedia(int nPhoneCallId)
{
	return phRenegotiateCallMedia(nPhoneCallId);
}


/*
int CPhApiAdapter::JoinConferenceCall(int nPhoneCallId_01, int nPhoneCallId_02)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		if ((nRet = phConf(nPhoneCallId_01, nPhoneCallId_02)) == 0)
		{
			if ((nRet = phResumeCall(nPhoneCallId_01)) == 0)
			{
				nRet = phResumeCall(nPhoneCallId_02);
			}
		}
	}
	
	return nRet;
}


int CPhApiAdapter::SplitConferenceCall(int nPhoneCallId_01, int nPhoneCallId_02)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		nRet = phStopConf(nPhoneCallId_01, nPhoneCallId_02);
	}
	
	return nRet;
}
*/


/*
** Conference Algorithm **

int callId1 = placeCall();
//wait for CALLOK on callId1
phHold(callId1);
int callId2 = placeCall();
//wait for CALLOK on callId2
phHold(callId2);
phConf(callId1, callId2);
phResume(callId1);
phResume(callId2);
phStopConf(callId1, callId2);

*/

int CPhApiAdapter::CreateConferenceCall(int nPhoneCallId)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		//FIXME nRet = phConfCreate();
		
		m_iParticipantCall_01 = nPhoneCallId;
		m_iParticipantCall_02 = INVALID_PHONE_CALL_ID;
		
		nRet = 1;
	}
	
	return nRet;
}


int CPhApiAdapter::ConferenceAddMember(int nConferenceCallId, int nPhoneCallId)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		//FIXME phConfAddMember(nConferenceCallId, nPhoneCallId);
		
		m_iParticipantCall_02 = nPhoneCallId;
		
		if ( (m_iParticipantCall_01 != INVALID_PHONE_CALL_ID) && 
				(m_iParticipantCall_02 != INVALID_PHONE_CALL_ID) )
		{
			if ((nRet = phConf(m_iParticipantCall_01, m_iParticipantCall_02)) == 0)
			{
				if ((nRet = ResumeCall(m_iParticipantCall_01)) == 0)
				{
					nRet = ResumeCall(m_iParticipantCall_02);
				}
			}
		}
	}
	
	return nRet;
}


int CPhApiAdapter::ConferenceRemoveMember(int nConferenceCallId, int nPhoneCallId)
{
	int nRet = -1;
	BOOL bFound = FALSE;
	
	if (IsInitialized())
	{
		//FIXME phConfRemoveMember(nConferenceCallId, nPhoneCallId);
		//FIXME phConfClose(nConferenceCallId);
		
		if ( (m_iParticipantCall_01 == nPhoneCallId) || 
				(m_iParticipantCall_02 == nPhoneCallId) )
		{
			bFound = TRUE;
		}
		
		if ( (m_iParticipantCall_01 != INVALID_PHONE_CALL_ID) && 
				(m_iParticipantCall_02 != INVALID_PHONE_CALL_ID) && (bFound) )
		{
			nRet = phStopConf(m_iParticipantCall_01, m_iParticipantCall_02);
		}
		else if (bFound)
		{
			nRet = 0;
		}
		
		if (m_iParticipantCall_01 == nPhoneCallId)
		{
			m_iParticipantCall_01 = INVALID_PHONE_CALL_ID;
		}
		
		if (m_iParticipantCall_02 == nPhoneCallId)
		{
			m_iParticipantCall_02 = INVALID_PHONE_CALL_ID;
		}
	}
	
	return nRet;
}


int CPhApiAdapter::DestroyConferenceCall(int nConferenceCallId)
{
	int nRet = -1;
	BOOL bRemoveFailed = FALSE;
	
	if (IsInitialized())
	{
		if (m_iParticipantCall_01 != INVALID_PHONE_CALL_ID)
		{
			if ((nRet = ConferenceRemoveMember(nConferenceCallId, m_iParticipantCall_01)) != 0)
			{
				bRemoveFailed = TRUE;
			}
		}
		
		if ( (m_iParticipantCall_02 != INVALID_PHONE_CALL_ID) && (!bRemoveFailed) )
		{
			nRet = ConferenceRemoveMember(nConferenceCallId, m_iParticipantCall_01);
		}
	}
	
	return nRet;
}


int CPhApiAdapter::SetLineCallForwardNumber(int nPhoneLineId, const CString& strCallForwardNumber)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		nRet = phLineSetFollowMe(nPhoneLineId, strCallForwardNumber);
	}
	
	return nRet;
}


int CPhApiAdapter::SetLineBusyFlag(int nPhoneLineId, BOOL bIsBusy)
{
	int nRet = -1;
	
	if (IsInitialized())
	{
		nRet = phLineSetBusy(nPhoneLineId, bIsBusy);
	}
	
	return nRet;
}


int CPhApiAdapter::PlayTone(int nPhoneCallId, EnumTone tone)
{
	int nRet = -1;

	if (IsInitialized())
	{
		// PH_DTMF_MODE_INBAND - the DTMF signal is mixed in the outgoing
		// PH_DTMF_MODE_RTPPAYLOAD - the DTMF signal will be sent using telephone_event RTP payload

		TCHAR chDtmf = ToneToChar(tone);

		nRet = phSendDtmf(nPhoneCallId, chDtmf, PH_DTMF_MODE_INBAND | PH_DTMF_MODE_RTPPAYLOAD);
	}

	return nRet;
}


BOOL CPhApiAdapter::SetCallInputAudioDevice(const CString& strDeviceName)
{
	CString strDeviceId;

	// Get device Id...
	std::list<AudioDevice> listDevices = AudioDeviceManager::getInstance().getInputDeviceList();

	std::list<AudioDevice>::iterator itDevice;

	for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++)
	{
		AudioDevice audioDevice = *itDevice;
		
		if (0 == strDeviceName.CompareNoCase(audioDevice.getName().c_str()))
		{
			strDeviceId = (audioDevice.getData()[1]).c_str();
			break;
		}
	}

	// Update phAPI devices...
	if (FALSE == strDeviceId.IsEmpty())
	{
		m_strInputAudioDeviceId = strDeviceId;

		int nChanged = ChangeAudioDevices(m_strInputAudioDeviceId, m_strOutputAudioDeviceId);

		return (nChanged == 0);
	}

	return FALSE;
}


BOOL CPhApiAdapter::SetCallOutputAudioDevice(const CString& strDeviceName)
{
	CString strDeviceId;

	// Get device Id...
	std::list<AudioDevice> listDevices = AudioDeviceManager::getInstance().getOutputDeviceList();

	std::list<AudioDevice>::iterator itDevice;

	for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++)
	{
		AudioDevice audioDevice = *itDevice;
		
		if (0 == strDeviceName.CompareNoCase(audioDevice.getName().c_str()))
		{
			strDeviceId = (audioDevice.getData()[1]).c_str();
			break;
		}
	}

	// Update phAPI devices...
	if (FALSE == strDeviceId.IsEmpty())
	{
		m_strOutputAudioDeviceId = strDeviceId;

		int nChanged = ChangeAudioDevices(m_strInputAudioDeviceId, m_strOutputAudioDeviceId);

		return (nChanged == 0);
	}

	return FALSE;
}


void CPhApiAdapter::EnableAcousticEchoCancellation(BOOL bEnable)
{
	phConfig_t* pPhConfig = phGetConfig();

	if (pPhConfig)
	{
		pPhConfig->noaec = bEnable ? 1 : 0;
	}
}


BOOL CPhApiAdapter::IsAcousticEchoCancellationEnabled()
{
	phConfig_t* pPhConfig = phGetConfig();

	if (pPhConfig)
	{
		return (1 == pPhConfig->noaec);
	}

	return FALSE;
}


void CPhApiAdapter::EnableHalfDuplex(BOOL bEnable)
{
	phConfig_t* pPhConfig = phGetConfig();

	if (pPhConfig)
	{
		if (bEnable)
		{
			pPhConfig->hdxmode = PH_HDX_MODE_SPK;
		}
		else
		{
			pPhConfig->hdxmode = 0;
		}
	}
}


BOOL CPhApiAdapter::IsHalfDuplexEnabled()
{
	phConfig_t* pPhConfig = phGetConfig();

	if (pPhConfig)
	{
		return (PH_HDX_MODE_SPK == pPhConfig->hdxmode);
	}

	return FALSE;
}


void CPhApiAdapter::SetAudioCodecsOrder(const CString& strCodecs)
{
	m_strAudioCodecsPrecedence = strCodecs;
}


CString CPhApiAdapter::GetAudioCodecsOrder()
{
	return m_strAudioCodecsPrecedence;
}


void CPhApiAdapter::SetVideoCodecsOrder(const CString& strCodecs)
{
	m_strVideoCodecsPrecedence = strCodecs;
}


CString CPhApiAdapter::GetVideoCodecsOrder()
{
	return m_strVideoCodecsPrecedence;
}


CString CPhApiAdapter::GetSupportedAudioCodecs() const
{
	return ms_strSupportedAudioCodecs;
}


CString CPhApiAdapter::GetSupportedVideoCodecs() const
{
	return ms_strSupportedVideoCodecs;
}


int CPhApiAdapter::FindAvailableSipPort()
{
	int nLocalPort = DEFAULT_SIP_PORT;

	const int MAX_RETRIES = 5;
	int i = 0;
	for (i = 0; i < MAX_RETRIES; i++)
	{
		if (NETLIB_TRUE != is_local_udp_port_used(NULL, nLocalPort + i))
		{
			nLocalPort += i;
			break;
		}
	}

	if (i == MAX_RETRIES)
	{
		nLocalPort = get_local_free_udp_port(NULL);
	}
	
	return nLocalPort;
}


BOOL CPhApiAdapter::SendInstantMessage(int nPhoneLineId, const CString& strMessage, const CString& strUserURI)
{
	int nMessageId = phLineSendMessage(nPhoneLineId, strUserURI, (LPCTSTR) strMessage, "text/plain");

	return (nMessageId == 1);
}


BOOL CPhApiAdapter::SetMicMute(BOOL bMute)
{
	phSetUOLFoneMicMute(bMute);
	
	return TRUE;
}

BOOL CPhApiAdapter::GetMicMute()
{
	return phGetUOLFoneMicMute();
}

void CPhApiAdapter::FreeVoipLines()
{
	phFreeVlines();
}

void CPhApiAdapter::SetInitialized(BOOL bInitialized)
{
	m_bInitialized = bInitialized;
}

void CPhApiAdapter::SetupNetwork(IUOLFoneClientNetworkConfig* pNetworkConfig, BOOL bMedia)
{
	if (pNetworkConfig)
	{
		LONG lFlags = 0;

		HRESULT hr = pNetworkConfig->GetConnectionFlags(&lFlags);

		if (SUCCEEDED(hr))
		{
			BOOL bUseHttpTunnel = FALSE;
			CComBSTR bstrHttpTunnelHost;
			LONG lHttpTunnelPort = 0;

			if (lFlags & UFC_CONNECTION_FLAGS_USE_HTTP_TUNNEL)
			{
				CComPtr<IUOLFoneClientServerInfo> pHttpTunnelInfo;

				hr = pNetworkConfig->GetHttpTunnelInfo(&pHttpTunnelInfo);

				if (SUCCEEDED(hr) && pHttpTunnelInfo)
				{
					pHttpTunnelInfo->GetServerHost(&bstrHttpTunnelHost);
					pHttpTunnelInfo->GetServerPort(&lHttpTunnelPort);

					bUseHttpTunnel = ((bstrHttpTunnelHost.Length() > 0) && (lHttpTunnelPort > 0));
				}
			}

			BOOL bUseHttpProxy = FALSE;
			CComBSTR bstrHttpProxyHost;
			LONG lHttpProxyPort = 0;
			CComBSTR bstrHttpProxyUsername;
			CComBSTR bstrHttpProxyPassword;

			if ((bUseHttpTunnel) &&
				(lFlags & UFC_CONNECTION_FLAGS_USE_HTTP_PROXY))
			{
				CComPtr<IUOLFoneClientServerInfo> pHttpProxyInfo;

				hr = pNetworkConfig->GetHttpProxyInfo(&pHttpProxyInfo);

				if (SUCCEEDED(hr) && pHttpProxyInfo)
				{
					pHttpProxyInfo->GetServerHost(&bstrHttpProxyHost);
					pHttpProxyInfo->GetServerPort(&lHttpProxyPort);

					pHttpProxyInfo->GetUsername(&bstrHttpProxyUsername);
					pHttpProxyInfo->GetPassword(&bstrHttpProxyPassword);

					bUseHttpProxy = ((bstrHttpProxyHost.Length() > 0) && (lHttpProxyPort > 0));
				}
			}

			phConfig_t* pPhConfig = phGetConfig();

			if (FALSE == bMedia)
			{
				pPhConfig->use_tunnel = bUseHttpTunnel;
			}
			else
			{
				pPhConfig->rtp_use_tunnel = bUseHttpTunnel;
			}

			if (bUseHttpTunnel)
			{
				BOOL bUseSSL = (lFlags & UFC_CONNECTION_FLAGS_USE_SSL);

				if (FALSE == bMedia)
				{
					// SIP proxy HTTP tunnel config
					if (bUseHttpProxy)
					{
						phTunnelConfig(CString(bstrHttpProxyHost), lHttpProxyPort, 
								CString(bstrHttpTunnelHost), lHttpTunnelPort, 
								CString(bstrHttpProxyUsername), CString(bstrHttpProxyPassword), bUseSSL, FALSE);
					}
					else
					{
						phTunnelConfig(CString(""), 0, CString(bstrHttpTunnelHost), lHttpTunnelPort, CString(""), CString(""), bUseSSL, FALSE);
					}
				}
				else
				{
					// RTP HTTP tunnel config
					if (bUseHttpProxy)
					{
						phTunnelProxyConfig(CString(bstrHttpProxyHost), lHttpProxyPort, CString(bstrHttpProxyUsername), CString(bstrHttpProxyPassword));
					}

					phMediaTunnelConfig(CString(bstrHttpTunnelHost), lHttpTunnelPort, bUseSSL, FALSE);
				}
			}
		}
	}
}


TCHAR CPhApiAdapter::ToneToChar(EnumTone tone)
{
	TCHAR chDtmf = '\0';
	switch (tone)
	{
	case UFC_TONE_0:
		chDtmf = '0';
		break;
	case UFC_TONE_1:
		chDtmf = '1';
		break;
	case UFC_TONE_2:
		chDtmf = '2';
		break;
	case UFC_TONE_3:
		chDtmf = '3';
		break;
	case UFC_TONE_4:
		chDtmf = '4';
		break;
	case UFC_TONE_5:
		chDtmf = '5';
		break;
	case UFC_TONE_6:
		chDtmf = '6';
		break;
	case UFC_TONE_7:
		chDtmf = '7';
		break;
	case UFC_TONE_8:
		chDtmf = '8';
		break;
	case UFC_TONE_9:
		chDtmf = '9';
		break;
	case UFC_TONE_STAR:
		chDtmf = '*';
		break;
	case UFC_TONE_POUND:
		chDtmf = '#';
		break;
	default:
		chDtmf = '\0';
		break;
	}

	return chDtmf;
}


int CPhApiAdapter::ChangeAudioDevices(const CString& strInputAudioDeviceId, const CString& strOutputAudioDeviceId)
{
	CString strAudioDevice;
	strAudioDevice.Format(_T("pa:IN=%s OUT=%s"), strInputAudioDeviceId, strOutputAudioDeviceId);

	int nChange = phChangeAudioDevices(strAudioDevice);
	ATLASSERT(nChange == 0);

	return nChange;
}
