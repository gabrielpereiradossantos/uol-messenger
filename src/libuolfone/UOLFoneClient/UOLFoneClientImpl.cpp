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


// UOLFoneClientImpl.cpp : Implementation of CUOLFoneClient

#include "stdafx.h"
#include "UOLFoneClientImpl.h"


//int CUOLFoneClient::ms_nextId = 0;

// CUOLFoneClient

STDMETHODIMP CUOLFoneClient::Initialize(IUOLFoneClientCallback* pCallback, BSTR bstrCodecsPath)
{
	return CSipWrapper::GetInstance()->Initialize(pCallback, bstrCodecsPath);
}


STDMETHODIMP CUOLFoneClient::Finalize(void)
{
	HRESULT hr = CSipWrapper::GetInstance()->Finalize();

	CSipWrapper::ReleaseInstance();
	
	return hr;
}


STDMETHODIMP CUOLFoneClient::TestConnection(IUOLFoneClientConnectionTestParam* pConnectionTestParam, 
											IUOLFoneClientConnectionTestResult** ppConnectionTestResult)
{
	return CSipWrapper::GetInstance()->TestConnection(pConnectionTestParam, ppConnectionTestResult);
}


STDMETHODIMP CUOLFoneClient::EnumerateAudioCodecs(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum)
{
	return CSipWrapper::GetInstance()->EnumerateAudioCodecs(ppCodecInfoEnum);
}


STDMETHODIMP CUOLFoneClient::EnumerateVideoCodecs(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum)
{
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::GetAudioCodecsPrecedence(BSTR* pbstrAudioCodecs)
{
	return CSipWrapper::GetInstance()->GetAudioCodecsPrecedence(pbstrAudioCodecs);
}


STDMETHODIMP CUOLFoneClient::GetVideoCodecsPrecedence(BSTR* pbstrVideoCodecs)
{
	return CSipWrapper::GetInstance()->GetVideoCodecsPrecedence(pbstrVideoCodecs);
}


STDMETHODIMP CUOLFoneClient::InitializePhoneLines(IUOLFoneClientPhoneLineConfig* pPhoneLineConfig)
{
	return CSipWrapper::GetInstance()->InitializePhoneLines(pPhoneLineConfig);
}


STDMETHODIMP CUOLFoneClient::FinalizePhoneLines()
{
	return CSipWrapper::GetInstance()->FinalizePhoneLines();
}


STDMETHODIMP CUOLFoneClient::RegisterPhoneLine(IUOLFoneClientPhoneLineInfo* pPhoneLineInfo, IUOLFoneClientNetworkConfig* pNetworkConfig, IUOLFoneClientPhoneLine** ppPhoneLine)
{
	return CSipWrapper::GetInstance()->RegisterPhoneLine(pPhoneLineInfo, pNetworkConfig, ppPhoneLine);
}


STDMETHODIMP CUOLFoneClient::UnregisterPhoneLine(IUOLFoneClientPhoneLine* pPhoneLine)
{
	return CSipWrapper::GetInstance()->UnregisterPhoneLine(pPhoneLine);
}


STDMETHODIMP CUOLFoneClient::MakePhoneCall(IUOLFoneClientPhoneLine* pPhoneLine, 
										   BSTR bstrPhoneURI,
										   BOOL bEnableVideo,
										   IUOLFoneClientPhoneCall** pPhoneCall)
{
	return CSipWrapper::GetInstance()->MakePhoneCall(pPhoneLine, bstrPhoneURI, bEnableVideo, pPhoneCall);
}


STDMETHODIMP CUOLFoneClient::SetMediaNetworkConfig(IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	return CSipWrapper::GetInstance()->SetMediaNetworkConfig(pNetworkConfig);
}


STDMETHODIMP CUOLFoneClient::AcceptPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall, BOOL bEnableVideo)
{
	return CSipWrapper::GetInstance()->AcceptPhoneCall(pPhoneCall, bEnableVideo);
}


STDMETHODIMP CUOLFoneClient::RejectPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall, EnumFailureCode rejectReason)
{
	return CSipWrapper::GetInstance()->RejectPhoneCall(pPhoneCall, rejectReason);
}


STDMETHODIMP CUOLFoneClient::ClosePhoneCall(IUOLFoneClientPhoneCall* pPhoneCall)
{
	return CSipWrapper::GetInstance()->ClosePhoneCall(pPhoneCall);
}


STDMETHODIMP CUOLFoneClient::HoldPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall)
{
	return CSipWrapper::GetInstance()->HoldPhoneCall(pPhoneCall);
}


STDMETHODIMP CUOLFoneClient::ResumePhoneCall(IUOLFoneClientPhoneCall* pPhoneCall)
{
	return CSipWrapper::GetInstance()->ResumePhoneCall(pPhoneCall);
}


STDMETHODIMP CUOLFoneClient::TransferPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall, BSTR bstrPhoneURI)
{
	return CSipWrapper::GetInstance()->TransferPhoneCall(pPhoneCall, bstrPhoneURI);
}


STDMETHODIMP CUOLFoneClient::ReconfigureCallMedia(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	return CSipWrapper::GetInstance()->ReconfigureCallMedia(pPhoneCall, pNetworkConfig);
}


STDMETHODIMP CUOLFoneClient::PlayTone(IUOLFoneClientPhoneCall* pPhoneCall, EnumTone tone)
{
	return CSipWrapper::GetInstance()->PlayTone(pPhoneCall, tone);
}


STDMETHODIMP CUOLFoneClient::PlaySoundFile(IUOLFoneClientPhoneCall* pPhoneCall, BSTR bstrSoundFilePath)
{
	return S_OK;
}


STDMETHODIMP CUOLFoneClient::GetAudioCodec(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo)
{
	return CSipWrapper::GetInstance()->GetAudioCodec(pPhoneCall, ppCodecInfo);
}


STDMETHODIMP CUOLFoneClient::GetVideoCodec(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo)
{
	return CSipWrapper::GetInstance()->GetVideoCodec(pPhoneCall, ppCodecInfo);
}


STDMETHODIMP CUOLFoneClient::EnumerateAudioDevices(EnumAudioDeviceType audioDeviceType, IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum)
{
	return CSipWrapper::GetInstance()->EnumerateAudioDevices(audioDeviceType, ppDeviceInfoEnum);
}


STDMETHODIMP CUOLFoneClient::EnumerateVideoDevices(IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum)
{
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::SetAudioDevice(EnumAudioDeviceType audioDeviceType, BSTR bstrDeviceName)
{
	return CSipWrapper::GetInstance()->SetAudioDevice(audioDeviceType, bstrDeviceName);
}


STDMETHODIMP CUOLFoneClient::GetAudioDevice(EnumAudioDeviceType audioDeviceType, BSTR* pbstrDeviceName)
{
	return CSipWrapper::GetInstance()->GetAudioDevice(audioDeviceType, pbstrDeviceName);
}


STDMETHODIMP CUOLFoneClient::EnableAcousticEchoCancellation(BOOL bEnable)
{
	return CSipWrapper::GetInstance()->EnableAcousticEchoCancellation(bEnable);
}


STDMETHODIMP CUOLFoneClient::IsAcousticEchoCancellationEnabled(BOOL* pbEnabled)
{
	return CSipWrapper::GetInstance()->IsAcousticEchoCancellationEnabled(pbEnabled);
}


STDMETHODIMP CUOLFoneClient::EnableHalfDuplex(BOOL bEnable)
{
	return CSipWrapper::GetInstance()->EnableHalfDuplex(bEnable);
}


STDMETHODIMP CUOLFoneClient::IsHalfDuplexEnabled(BOOL* pbEnabled)
{
	return CSipWrapper::GetInstance()->IsHalfDuplexEnabled(pbEnabled);
}


STDMETHODIMP CUOLFoneClient::SetVolumeLevel(EnumAudioDeviceType audioDeviceType, LONG lLevel)
{
	return CSipWrapper::GetInstance()->SetVolumeLevel(audioDeviceType, lLevel);
}


STDMETHODIMP CUOLFoneClient::GetVolumeLevel(EnumAudioDeviceType audioDeviceType, LONG* plLevel)
{
	return CSipWrapper::GetInstance()->GetVolumeLevel(audioDeviceType, plLevel);
}


STDMETHODIMP CUOLFoneClient::SetMute(EnumAudioDeviceType audioDeviceType, BOOL bMute)
{
	return CSipWrapper::GetInstance()->SetMute(audioDeviceType, bMute);
}


STDMETHODIMP CUOLFoneClient::GetMute(EnumAudioDeviceType audioDeviceType, BOOL* pbMute)
{
	return CSipWrapper::GetInstance()->GetMute(audioDeviceType, pbMute);
}


STDMETHODIMP CUOLFoneClient::SetVideoDevice(BSTR bstrDeviceName)
{
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::GetVideoDevice(BSTR* pbstrDeviceName)
{
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::SetVideoQuality(LONG lVideoQuality)
{
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::GetVideoQuality(LONG* plVideoQuality)
{
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::SendInstantMessage(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrMessage, BSTR bstrUserURI)
{
	return CSipWrapper::GetInstance()->SendInstantMessage(pPhoneLine, bstrMessage, bstrUserURI);
}


STDMETHODIMP CUOLFoneClient::SendRingingNotification(IUOLFoneClientPhoneCall* pPhoneCall)
{
	return CSipWrapper::GetInstance()->SendRingingNotification(pPhoneCall);
}


/*
STDMETHODIMP CUOLFoneClient::JoinConferenceCall(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02)
{
	return CSipWrapper::GetInstance()->JoinConferenceCall(pPhoneCall_01, pPhoneCall_02);
}


STDMETHODIMP CUOLFoneClient::SplitConferenceCall(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02)
{
	return CSipWrapper::GetInstance()->SplitConferenceCall(pPhoneCall_01, pPhoneCall_02);
}
*/


STDMETHODIMP CUOLFoneClient::CreateConferenceCall(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientConferenceCall** pConferenceCall)
{
	return CSipWrapper::GetInstance()->CreateConferenceCall(pPhoneCall, pConferenceCall);
}


STDMETHODIMP CUOLFoneClient::ConferenceInvite(IUOLFoneClientPhoneLine* pPhoneLine, 
							IUOLFoneClientConferenceCall* pConferenceCall,
							BSTR bstrPhoneURI,
							BOOL bEnableVideo,
							IUOLFoneClientPhoneCall** ppPhoneCall)
{
	// This call is more or less equivalent to phLinePlaceCall followed by phConfAddMember.
	// (MakePhoneCall e ConferenceAddMember).
	//
	
	return E_NOTIMPL;
}


STDMETHODIMP CUOLFoneClient::ConferenceAddMember(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall)
{
	return CSipWrapper::GetInstance()->ConferenceAddMember(pConferenceCall, pPhoneCall);
}


STDMETHODIMP CUOLFoneClient::ConferenceRemoveMember(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall)
{
	return CSipWrapper::GetInstance()->ConferenceRemoveMember(pConferenceCall, pPhoneCall);
}


STDMETHODIMP CUOLFoneClient::DestroyConferenceCall(IUOLFoneClientConferenceCall* pConferenceCall)
{
	return CSipWrapper::GetInstance()->DestroyConferenceCall(pConferenceCall);
}


STDMETHODIMP CUOLFoneClient::SetLineCallForwardNumber(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrCallForwardNumber)
{
	return CSipWrapper::GetInstance()->SetLineCallForwardNumber(pPhoneLine, bstrCallForwardNumber);
}


STDMETHODIMP CUOLFoneClient::SetLineBusyFlag(IUOLFoneClientPhoneLine* pPhoneLine, BOOL bIsBusy)
{
	return CSipWrapper::GetInstance()->SetLineBusyFlag(pPhoneLine, bIsBusy);
}


STDMETHODIMP CUOLFoneClient::Initialize(IUOLFoneClientCallback2* pCallback2, 
					  BSTR bstrCodecsPath, EnumOsipDebugLevel lLogLevel)
{
	return CSipWrapper::GetInstance()->Initialize(pCallback2, bstrCodecsPath, lLogLevel);
}

