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


// UOLFoneClientImpl.h : Declaration of the CUOLFoneClient

#pragma once

#include "resource.h"       // main symbols
#include "interfaces/IUOLFoneClient.h"
#include "SipWrapper.h"


// CUOLFoneClient

[
	coclass,
	threading("apartment"),
	vi_progid("UOLFoneClient.UOLFoneClient"),
	progid("UOLFoneClient.UOLFoneClient.1"),
	version(1.0),
	uuid("03824518-4A74-4594-947C-6C44FE01E133"),
	helpstring("UOLFoneClient Class")
]
class ATL_NO_VTABLE CUOLFoneClient : 
	public IUOLFoneClient2
{
public:
	CUOLFoneClient()
	{
	}

	virtual ~CUOLFoneClient()
	{
		ATLTRACE(_T("%s\n"), __FUNCTION__);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		//ms_nextId++;
		//m_nMyId = ms_nextId;

		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

//	ULONG InternalAddRef()
//	{
//		ULONG l = __super::InternalAddRef();
//
//#ifdef _DEBUG
//		CString str;
//		str.Format("%s %i\n", __FUNCTION__, l);
//
//		::OutputDebugString(str);
//#endif
//		return l;
//	}
//
//	ULONG InternalRelease()
//	{
//		ULONG l = __super::InternalRelease();
//
//#ifdef _DEBUG
//		CString str;
//		str.Format("%s %i\n", __FUNCTION__, l);
//
//		::OutputDebugString(str);
//#endif	
//
//		return l;
//	}

protected:
	
	// Interface IUOLFoneClient
	//
	STDMETHOD(Initialize)(IUOLFoneClientCallback* pCallback, BSTR bstrCodecsPath);
	STDMETHOD(Finalize)(void);
	STDMETHOD(TestConnection)(IUOLFoneClientConnectionTestParam* pConnectionTestParam, 
							  IUOLFoneClientConnectionTestResult** ppConnectionTestResult);
	STDMETHOD(EnumerateAudioCodecs)(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum);
	STDMETHOD(EnumerateVideoCodecs)(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum);
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
	STDMETHOD(PlaySoundFile)(IUOLFoneClientPhoneCall* pPhoneCall, BSTR bstrSoundFilePath);
	STDMETHOD(GetAudioCodec)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo);
	STDMETHOD(GetVideoCodec)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo);
	STDMETHOD(EnumerateAudioDevices)(EnumAudioDeviceType audioDeviceType, IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum);
	STDMETHOD(EnumerateVideoDevices)(IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum);
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
	STDMETHOD(SetVideoDevice)(BSTR bstrDeviceName);
	STDMETHOD(GetVideoDevice)(BSTR* pbstrDeviceName);
	STDMETHOD(SetVideoQuality)(LONG lVideoQuality);
	STDMETHOD(GetVideoQuality)(LONG* plVideoQuality);
	STDMETHOD(SendInstantMessage)(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrMessage, BSTR bstrUserURI);
	STDMETHOD(SendRingingNotification)(IUOLFoneClientPhoneCall* pPhoneCall);
	//STDMETHOD(JoinConferenceCall)(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02);
	//STDMETHOD(SplitConferenceCall)(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02);
	STDMETHOD(CreateConferenceCall)(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientConferenceCall** pConferenceCall);
	STDMETHOD(ConferenceInvite)(IUOLFoneClientPhoneLine* pPhoneLine, 
								IUOLFoneClientConferenceCall* pConferenceCall,
								BSTR bstrPhoneURI,
								BOOL bEnableVideo,
								IUOLFoneClientPhoneCall** ppPhoneCall);
	STDMETHOD(ConferenceAddMember)(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(ConferenceRemoveMember)(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall);
	STDMETHOD(DestroyConferenceCall)(IUOLFoneClientConferenceCall* pConferenceCall);
	STDMETHOD(SetLineCallForwardNumber)(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrCallForwardNumber);
	STDMETHOD(SetLineBusyFlag)(IUOLFoneClientPhoneLine* pPhoneLine, BOOL bIsBusy);
	
	// Interface IUOLFoneClient2
	//
	STDMETHOD(Initialize)(IUOLFoneClientCallback2* pCallback2, 
						  BSTR bstrCodecsPath, 
						  EnumOsipDebugLevel lLogLevel);

private:
	//static int ms_nextId;

	CComPtr<IUOLFoneClientCallback>		m_pCallback;
	//int m_nMyId;
};


