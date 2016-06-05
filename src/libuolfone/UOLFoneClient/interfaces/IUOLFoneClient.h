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

#include "IUOLFoneClientCallback.h"
#include "IUOLFoneClientNetworkConfig.h"
#include "IUOLFoneClientPhoneLineConfig.h"
#include "IUOLFoneClientPhoneLineInfo.h"
#include "IUOLFoneClientPhoneLine.h"
#include "IUOLFoneClientPhoneCall.h"
#include "IUOLFoneClientCodecInfo.h"
#include "IUOLFoneClientCodecInfoEnum.h"
#include "IUOLFoneClientDeviceInfo.h"
#include "IUOLFoneClientDeviceInfoEnum.h"
#include "IUOLFoneClientConnectionTestParam.h"
#include "IUOLFoneClientConnectionTestResult.h"
#include "IUOLFoneClientConferenceCall.h"


[
	export
]
enum EnumAudioDeviceType
{
	UFC_AUDIO_DEVICE_TYPE_CALL_INPUT,
	UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT//,
	//UFC_AUDIO_DEVICE_TYPE_RINGING
};


[
	export
]
enum EnumVideoQuality
{
	UFC_VIDEO_QUALITY_NORMAL,		// Down=0-512kbit/s up=0-128kbit/s.
	UFC_VIDEO_QUALITY_GOOD,			// Down=512-2048kbit/s up=128-256kbit/s.
	UFC_VIDEO_QUALITY_VERY_GOOD,	// Down=+2048kbit/s up=+256kbit/s.
	UFC_VIDEO_QUALITY_EXCELLENT		// Down=+8192kbit/s up=+1024kbit/s.
};


// IUOLFoneClient
[
	object,
	uuid("1B96FC0D-CA4D-4ABC-BF64-6985E97E0717"),
	dual,	helpstring("IUOLFoneClient Interface")//,
	//pointer_default(unique)
]
__interface IUOLFoneClient : IDispatch
{
	[id(1)] HRESULT Initialize([in] IUOLFoneClientCallback* pCallback, [in] BSTR bstrCodecsPath);
	[id(2)] HRESULT Finalize();

	// Network testing
	[id(3)] HRESULT TestConnection([in] IUOLFoneClientConnectionTestParam* pConnectionTestParam, 
								   [out] IUOLFoneClientConnectionTestResult** ppConnectionTestResult);

	// Codecs enumeration
	[id(4)] HRESULT EnumerateAudioCodecs([out] IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum);
	[id(5)] HRESULT EnumerateVideoCodecs([out] IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum);

	// Codecs configuration
	[id(6)] HRESULT GetAudioCodecsPrecedence([out] BSTR* pbstrAudioCodecs);
	[id(7)] HRESULT GetVideoCodecsPrecedence([out] BSTR* pbstrVideoCodecs);

	// Phone line 
	[id(8)] HRESULT InitializePhoneLines([in] IUOLFoneClientPhoneLineConfig* pPhoneLineConfig);
	[id(9)] HRESULT FinalizePhoneLines();

	// REGISTER
	[id(10)] HRESULT RegisterPhoneLine([in] IUOLFoneClientPhoneLineInfo* pPhoneLineInfo,
									   [in] IUOLFoneClientNetworkConfig* pNetworkConfig,
									   [out] IUOLFoneClientPhoneLine** ppPhoneLine);
	[id(11)] HRESULT UnregisterPhoneLine([in] IUOLFoneClientPhoneLine* pPhoneLine);

	// INVITE
	[id(12)] HRESULT MakePhoneCall([in] IUOLFoneClientPhoneLine* pPhoneLine, 
								   [in] BSTR bstrPhoneURI,
								   [in] BOOL bEnableVideo,
								   [out] IUOLFoneClientPhoneCall** ppPhoneCall);

	[id(13)] HRESULT SetMediaNetworkConfig([in] IUOLFoneClientNetworkConfig* pNetworkConfig);

	// INVITE reply/deny
	[id(14)] HRESULT AcceptPhoneCall([in] IUOLFoneClientPhoneCall* pPhoneCall,
								 	 [in] BOOL bEnableVideo);
	[id(15)] HRESULT RejectPhoneCall([in] IUOLFoneClientPhoneCall* pPhoneCall, [in] EnumFailureCode rejectReason);

	// BYE/CANCEL
	[id(16)] HRESULT ClosePhoneCall([in] IUOLFoneClientPhoneCall* pPhoneCall);

	[id(17)] HRESULT HoldPhoneCall([in] IUOLFoneClientPhoneCall* pPhoneCall);
	[id(18)] HRESULT ResumePhoneCall([in] IUOLFoneClientPhoneCall* pPhoneCall);
	[id(19)] HRESULT TransferPhoneCall([in] IUOLFoneClientPhoneCall* pPhoneCall, [in] BSTR bstrPhoneURI);
	[id(20)] HRESULT ReconfigureCallMedia([in] IUOLFoneClientPhoneCall* pPhoneCall, [in] IUOLFoneClientNetworkConfig* pNetworkConfig);

	// DTMF sending
	[id(21)] HRESULT PlayTone([in] IUOLFoneClientPhoneCall* pPhoneCall, [in] EnumTone tone);
	
	[id(22)] HRESULT PlaySoundFile([in] IUOLFoneClientPhoneCall* pPhoneCall, [in] BSTR bstrSoundFilePath);

	// Call codec
	[id(23)] HRESULT GetAudioCodec([in] IUOLFoneClientPhoneCall* pPhoneCall, [out] IUOLFoneClientCodecInfo** pCodecInfo);
	[id(24)] HRESULT GetVideoCodec([in] IUOLFoneClientPhoneCall* pPhoneCall, [out] IUOLFoneClientCodecInfo** pCodecInfo);

	// Devices enumeration
	[id(25)] HRESULT EnumerateAudioDevices([in] EnumAudioDeviceType audioDeviceType, IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum);
	[id(26)] HRESULT EnumerateVideoDevices([out] IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum);

	// Audio device config
	[id(27)] HRESULT SetAudioDevice([in] EnumAudioDeviceType audioDeviceType, [in] BSTR bstrDeviceName);
	[id(28)] HRESULT GetAudioDevice([in] EnumAudioDeviceType audioDeviceType, [out] BSTR* pbstrDeviceName);
	[id(29)] HRESULT EnableAcousticEchoCancellation([in] BOOL bEnable);
	[id(30)] HRESULT IsAcousticEchoCancellationEnabled([out] BOOL* pbEnabled);
	[id(31)] HRESULT EnableHalfDuplex([in] BOOL bEnable);
	[id(32)] HRESULT IsHalfDuplexEnabled([in] BOOL* pbEnabled);
	[id(33)] HRESULT SetVolumeLevel([in] EnumAudioDeviceType audioDeviceType, [in] LONG lLevel);
	[id(34)] HRESULT GetVolumeLevel([in] EnumAudioDeviceType audioDeviceType, [out] LONG* plLevel);
	[id(35)] HRESULT SetMute([in] EnumAudioDeviceType audioDeviceType, [in] BOOL bMute);
	[id(36)] HRESULT GetMute([in] EnumAudioDeviceType audioDeviceType, [out] BOOL* pbMute);

	// Video device config
	[id(37)] HRESULT SetVideoDevice([in] BSTR bstrDeviceName);
	[id(38)] HRESULT GetVideoDevice([out] BSTR* pbstrDeviceName);
	[id(39)] HRESULT SetVideoQuality([in] LONG lVideoQuality);
	[id(40)] HRESULT GetVideoQuality([out] LONG* plVideoQuality);

	// Message send
	[id(41)] HRESULT SendInstantMessage([in] IUOLFoneClientPhoneLine* pPhoneLine, [in] BSTR bstrMessage, [in] BSTR bstrUserURI);

	[id(42)] HRESULT SendRingingNotification([in] IUOLFoneClientPhoneCall* pPhoneCall);
	
	// Conference Call
	//[id(43)] HRESULT JoinConferenceCall([in] IUOLFoneClientPhoneCall* pPhoneCall_01, [in] IUOLFoneClientPhoneCall* pPhoneCall_02);
	//[id(44)] HRESULT SplitConferenceCall([in] IUOLFoneClientPhoneCall* pPhoneCall_01, [in] IUOLFoneClientPhoneCall* pPhoneCall_02);
	[id(43)] HRESULT CreateConferenceCall([in] IUOLFoneClientPhoneCall* pPhoneCall, [out] IUOLFoneClientConferenceCall** pConferenceCall);
	[id(44)] HRESULT ConferenceInvite([in] IUOLFoneClientPhoneLine* pPhoneLine, 
									  [in] IUOLFoneClientConferenceCall* pConferenceCall,
									  [in] BSTR bstrPhoneURI,
									  [in] BOOL bEnableVideo,
									  [out] IUOLFoneClientPhoneCall** ppPhoneCall);
	[id(45)] HRESULT ConferenceAddMember([in] IUOLFoneClientConferenceCall* pConferenceCall, [in] IUOLFoneClientPhoneCall* pPhoneCall);
	[id(46)] HRESULT ConferenceRemoveMember([in] IUOLFoneClientConferenceCall* pConferenceCall, [in] IUOLFoneClientPhoneCall* pPhoneCall);
	[id(47)] HRESULT DestroyConferenceCall([in] IUOLFoneClientConferenceCall* pConferenceCall);
	
	// Call forward (Follow me)
	[id(48)] HRESULT SetLineCallForwardNumber([in] IUOLFoneClientPhoneLine* pPhoneLine, 
											  [in] BSTR bstrCallForwardNumber);
	
	// Busy flag (dont disturb)
	[id(49)] HRESULT SetLineBusyFlag([in] IUOLFoneClientPhoneLine* pPhoneLine, 
									 [in] BOOL bIsBusy);
};


// IUOLFoneClient2
[
	object,
	uuid("FBAEDE5C-6971-4c74-9B8E-11B407566B40"),
	dual,	helpstring("IUOLFoneClient2 Interface")//,
	//pointer_default(unique)
]
__interface IUOLFoneClient2 : IUOLFoneClient
{
	[id(50)] HRESULT Initialize([in] IUOLFoneClientCallback2* pCallback2, 
							   [in] BSTR bstrCodecsPath, 
							   [in] EnumOsipDebugLevel lLogLevel);
};

