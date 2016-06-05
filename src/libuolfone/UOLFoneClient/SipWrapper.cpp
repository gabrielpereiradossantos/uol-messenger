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
#include "SipWrapper.h"

#include "resource.h"
#include "DeviceInfo.h"
#include "DeviceInfoEnum.h"
#include "AudioDeviceManager.h"
#include "PhCodecInfoAdapter.h"
#include "MessageEvent.h"
#include "NetworkConnectionTester.h"
#include "ExtraResponseData.h"
#include "OsipDebugInfo.h"


CSipWrapper* CSipWrapper::m_pInstance = NULL;


typedef struct _PHONECALLEVENTPARAM
{
	LONG lPhoneLineId;
	LONG lPhoneCallId;
	EnumPhoneCallState state;
	CString strPhoneURI;

	_PHONECALLEVENTPARAM operator=(const _PHONECALLEVENTPARAM& pcep)
	{
		lPhoneLineId = pcep.lPhoneLineId;
		lPhoneCallId = pcep.lPhoneCallId;
		state = pcep.state;
		strPhoneURI = pcep.strPhoneURI;

		return *this;
	}

} PHONECALLEVENTPARAM, * LPPHONECALLEVENTPARAM;

typedef struct _PHONECALLASYNCEVENTPARAM
{
	LONG lPhoneCallId;
	EnumPhoneCallState state;
} PHONECALLASYNCEVENTPARAM, * LPPHONECALLASYNCEVENTPARAM;

typedef struct _PHONECALLSTREAMERRORPARAM
{
	LONG lPhoneLineId;
	LONG lPhoneCallId;
	EnumPhoneCallStreamError error;
} PHONECALLSTREAMERRORPARAM, * LPPHONECALLSTREAMERRORPARAM;




CSipWrapper::CSipWrapper() : 
		m_pConferenceCallObj(NULL), 
		m_lLogLevel(-1)
{
}


CSipWrapper::~CSipWrapper()
{
}


CSipWrapper* CSipWrapper::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CSipWrapper();
	}
	
	return m_pInstance;
}

void CSipWrapper::ReleaseInstance()
{
	delete m_pInstance;
	m_pInstance = NULL;
}



IPhApiAdapterCallback* CSipWrapper::GetCallback()
{
	return this;
}


BOOL CSipWrapper::IsFinalizeInProgress()
{
	return m_bFinalizeInProgress;
}


STDMETHODIMP CSipWrapper::Initialize(IUOLFoneClientCallback* pCallback, BSTR bstrCodecsPath)
{
	HRESULT hr = E_FAIL;

	if (pCallback != NULL)
	{
		m_bFinalizeInProgress = FALSE;

		// Create callback window
		HWND hWnd = Create(::GetDesktopWindow(), NULL, "SipWrapper", WS_POPUP);

		if (hWnd != NULL)
		{
			m_spCallback = pCallback;

			m_bstrCodecsPath = bstrCodecsPath;

			hr = S_OK;
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::Finalize(void)
{
	HRESULT hr = E_FAIL;

	if (m_spCallback != NULL)
	{
		m_spCallback.Release();

		m_bFinalizeInProgress = TRUE;

		// Destroy callback window
		if (IsWindow())
		{
			BOOL bDestroy = DestroyWindow();
			ATLASSERT(bDestroy);
		}

		FinalizePhoneLines();

		hr = S_OK;

		m_bFinalizeInProgress = FALSE;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::TestConnection(IUOLFoneClientConnectionTestParam* pConnectionTestParam, 
										 IUOLFoneClientConnectionTestResult** ppConnectionTestResult)
{
	HRESULT hr = E_FAIL;

	if ((pConnectionTestParam != NULL) && 
		(ppConnectionTestResult != NULL))
	{
		LONG lAvailablePort = m_phApiAdapter.FindAvailableSipPort();

		CNetworkConnectionTester tester;
		tester.SetLocalPort(lAvailablePort);

		hr = tester.TestConnection(pConnectionTestParam, ppConnectionTestResult);
	}

	return hr;
}


STDMETHODIMP CSipWrapper::EnumerateAudioCodecs(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum)
{
	HRESULT hr = E_FAIL;

	CString strAudioCodecs = m_phApiAdapter.GetSupportedAudioCodecs();
	
	hr = CPhCodecInfoAdapter::GetCodecInfoEnum(strAudioCodecs, ppCodecInfoEnum);

	return hr;
}


//STDMETHODIMP CSipWrapper::EnumerateVideoCodecs(IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum)
//{
//	return E_NOTIMPL;
//}


STDMETHODIMP CSipWrapper::GetAudioCodecsPrecedence(BSTR* pbstrAudioCodecs)
{
	HRESULT hr = E_POINTER;

	if (pbstrAudioCodecs)
	{
		*pbstrAudioCodecs = m_phApiAdapter.GetAudioCodecsOrder().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::GetVideoCodecsPrecedence(BSTR* pbstrVideoCodecs)
{
	HRESULT hr = E_POINTER;

	if (pbstrVideoCodecs)
	{
		*pbstrVideoCodecs = m_preferences.GetVideoCodecsPrecedence().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::InitializePhoneLines(IUOLFoneClientPhoneLineConfig* pPhoneLineConfig)
{
	HRESULT hr = E_FAIL;

	if (FALSE == m_phApiAdapter.IsInitialized())
	{
		ATLASSERT( m_bstrCodecsPath.Length() != 0 );

		// Initialize Phone API
		int nInitError = m_phApiAdapter.Initialize(pPhoneLineConfig, m_bstrCodecsPath);

		if (0 == nInitError)
		{
			// Set sound devices...
			CString strInputAudioDevice = CAudioDeviceManager::GetDefaultAudioInputDevice();
			CString strOutputAudioDevice = CAudioDeviceManager::GetDefaultAudioOutputDevice();

			m_preferences.SetCurrentAudioInputDevice(strInputAudioDevice);
			m_preferences.SetCurrentAudioOutputDevice(strOutputAudioDevice);

			m_phApiAdapter.SetCallInputAudioDevice(strInputAudioDevice);
			m_phApiAdapter.SetCallOutputAudioDevice(strOutputAudioDevice);

			hr = S_OK;
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::FinalizePhoneLines()
{
	HRESULT hr = E_FAIL;

	if (m_phApiAdapter.IsInitialized())
	{
		m_phApiAdapter.Finalize();

		// Clear phone calls
		m_phoneCallManager.RemoveAllPhoneCalls();
		m_phoneLineManager.RemoveAllPhoneLines();

		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::RegisterPhoneLine(IUOLFoneClientPhoneLineInfo* pPhoneLineInfo, IUOLFoneClientNetworkConfig* pNetworkConfig, IUOLFoneClientPhoneLine** ppPhoneLine)
{
	HRESULT hr = E_FAIL;

	if (m_phApiAdapter.IsInitialized())
	{
		if ((pPhoneLineInfo != NULL) && (ppPhoneLine != NULL))
		{
			*ppPhoneLine = NULL;

			//ATLASSERT(m_phoneLineManager.GetPhoneLineCount() == 0);

			LONG lPhoneLineId = m_phApiAdapter.RegisterPhoneLine(pPhoneLineInfo, pNetworkConfig);

			if (lPhoneLineId >= 0)
			{
				IUOLFoneClientPhoneLine* pNewPhoneLine = m_phoneLineManager.AddPhoneLine(lPhoneLineId, pPhoneLineInfo);

				if (pNewPhoneLine != NULL)
				{
					*ppPhoneLine = pNewPhoneLine;

					hr = S_OK;
				}
			}
		}
	}

	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);

	return hr;
}


STDMETHODIMP CSipWrapper::UnregisterPhoneLine(IUOLFoneClientPhoneLine* pPhoneLine)
{
	HRESULT hr = E_FAIL;

	if (m_phApiAdapter.IsInitialized())
	{
		if (pPhoneLine != NULL)
		{
			LONG lPhoneLineId = -1;

			pPhoneLine->GetPhoneLineId(&lPhoneLineId);

			IUOLFoneClientPhoneLine* pRegisteredPhoneLine = m_phoneLineManager.GetPhoneLine(lPhoneLineId);

			if (pRegisteredPhoneLine != NULL)
			{
				if (0 == m_phApiAdapter.UnregisterPhoneLine(lPhoneLineId))
				{
					hr = S_OK;
				}
				else
				{
					// User Internet connection may be down

					ATLTRACE(_T("%s - Can't unregister phone line!!!\n"), __FUNCTION__);
					m_phoneLineManager.SetPhoneLineState(lPhoneLineId, UFC_PHONE_LINE_STATE_CONNECTION_TIMEOUT);
					
					// Remove invalid phone line
					IUOLFoneClientPhoneLine* pRemovedPhoneLine = m_phoneLineManager.RemovePhoneLine(lPhoneLineId);
					
					if (pRemovedPhoneLine)
					{
						pRemovedPhoneLine->Release();
					}
				}

				pRegisteredPhoneLine->Release();
				pRegisteredPhoneLine = NULL;
			}
		}
	}

	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);

	return hr;
}


STDMETHODIMP CSipWrapper::MakePhoneCall(IUOLFoneClientPhoneLine* pPhoneLine, 
							 BSTR bstrPhoneURI,
							 BOOL bEnableVideo,
							 IUOLFoneClientPhoneCall** ppPhoneCall)
{
	HRESULT hr = E_FAIL;

	if (m_phApiAdapter.IsInitialized())
	{
		if ((pPhoneLine != NULL) && (ppPhoneCall != NULL))
		{
			*ppPhoneCall = NULL;

			LONG lPhoneLineId = -1;

			HRESULT hr2 = pPhoneLine->GetPhoneLineId(&lPhoneLineId);

			if (SUCCEEDED(hr2))
			{
				// Check if phone line exists
				IUOLFoneClientPhoneLine* pRegisteredPhoneLine = m_phoneLineManager.GetPhoneLine(lPhoneLineId);

				if (pRegisteredPhoneLine != NULL)
				{
					CString strPhoneURI(bstrPhoneURI);

					// Make call
					LONG lPhoneCallId = m_phApiAdapter.MakeCall(lPhoneLineId, strPhoneURI, /*bEnableVideo,*/FALSE);

					if (lPhoneCallId >= 0)
					{
						// Store call information
						IUOLFoneClientPhoneCall* pNewPhoneCall = m_phoneCallManager.AddPhoneCall(pRegisteredPhoneLine, strPhoneURI, /*bEnableVideo,*/ FALSE, lPhoneCallId);

						if (pNewPhoneCall != NULL)
						{
							*ppPhoneCall = pNewPhoneCall;

							hr = S_OK;
						}
					}

					pRegisteredPhoneLine->Release();
					pRegisteredPhoneLine = NULL;
				}
			}
		}
	}

	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);

	return hr;
}


STDMETHODIMP CSipWrapper::SetMediaNetworkConfig(IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	HRESULT hr = E_FAIL;

	if ((pNetworkConfig != NULL) && 
		(m_phApiAdapter.IsInitialized()) &&
		(0 == m_phoneCallManager.GetPhoneCallCount()))
	{
		if (m_phApiAdapter.SetMediaNetworkConfig(pNetworkConfig))
		{
			hr = S_OK;
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::AcceptPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall,	BOOL bEnableVideo)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				// Accept call
				// TODO: Support video
				int nError = m_phApiAdapter.AcceptCall(lPhoneCallId, FALSE);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);

	return hr;
}


STDMETHODIMP CSipWrapper::RejectPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall, EnumFailureCode rejectReason)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				// Hang-up call
				int nError = m_phApiAdapter.RejectCall(lPhoneCallId, rejectReason);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);

	return hr;
}


STDMETHODIMP CSipWrapper::ClosePhoneCall(IUOLFoneClientPhoneCall* pPhoneCall)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				// Hang-up call
				int nError = m_phApiAdapter.CloseCall(lPhoneCallId);

				if (0 == nError)
				{
					hr = S_OK;
				}
				else
				{
					// User Internet connection may be down

					ATLTRACE(_T("%s - Can't close phone call!!!\n"), __FUNCTION__);
					m_phoneCallManager.SetPhoneCallState(lPhoneCallId, UFC_PHONE_CALL_STATE_ERROR);
					
					// Remove invalid phone call
					IUOLFoneClientPhoneCall* pRemovedPhoneCall = m_phoneCallManager.RemovePhoneCall(lPhoneCallId);
					
					if (pRemovedPhoneCall)
					{
						pRemovedPhoneCall->Release();
					}
				}
			}
		}
	}

	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);

	return hr;
}


STDMETHODIMP CSipWrapper::HoldPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				int nError = m_phApiAdapter.HoldCall(lPhoneCallId);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::ResumePhoneCall(IUOLFoneClientPhoneCall* pPhoneCall)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				int nError = m_phApiAdapter.ResumeCall(lPhoneCallId);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::TransferPhoneCall(IUOLFoneClientPhoneCall* pPhoneCall, BSTR bstrPhoneURI)
{
	HRESULT hr = E_FAIL;
	
	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;
		
		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);
		
		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);
			
			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();
				
				CString strPhoneURI(bstrPhoneURI);
				
				int nError = m_phApiAdapter.TransferCall(lPhoneCallId, strPhoneURI);
				
				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}
	
	return hr;
}


STDMETHODIMP CSipWrapper::ReconfigureCallMedia(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				SetMediaNetworkConfig(pNetworkConfig);

				int nError = m_phApiAdapter.ReconfigureCallMedia(lPhoneCallId);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::PlayTone(IUOLFoneClientPhoneCall* pPhoneCall, EnumTone tone)
{
	HRESULT hr = E_FAIL;

	if ((pPhoneCall) && 
		(tone >= UFC_TONE_0) && (tone <= UFC_TONE_POUND))
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				int nError = m_phApiAdapter.PlayTone(lPhoneCallId, tone);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::GetAudioCodec(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo)
{
	return E_NOTIMPL;
}


STDMETHODIMP CSipWrapper::GetVideoCodec(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientCodecInfo** ppCodecInfo)
{
	return E_NOTIMPL;
}


STDMETHODIMP CSipWrapper::EnumerateAudioDevices(EnumAudioDeviceType audioDeviceType, IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum)
{
	HRESULT hr = E_FAIL;

	if (ppDeviceInfoEnum != NULL)
	{
		CAtlList<CString> listDevices;

		if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
		{
			CAudioDeviceManager::GetAudioInputDeviceList(listDevices);
		}
		else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
		{
			CAudioDeviceManager::GetAudioOutputDeviceList(listDevices);
		}

		CComObject<CDeviceInfoEnum>* pNewDeviceInfoEnum;

		hr = CComObject<CDeviceInfoEnum>::CreateInstance(&pNewDeviceInfoEnum);

		if (SUCCEEDED(hr))
		{
			for (POSITION pos = listDevices.GetHeadPosition(); pos; )
			{
				CString strDeviceName = listDevices.GetNext(pos);

				CComObject<CDeviceInfo>* pDeviceInfo;

				hr = CComObject<CDeviceInfo>::CreateInstance(&pDeviceInfo);

				if (SUCCEEDED(hr))
				{
					pDeviceInfo->SetName(strDeviceName);
					pNewDeviceInfoEnum->AddDeviceInfo(pDeviceInfo);
				}
				else
				{
					break;
				}
			}

			if (SUCCEEDED(hr))
			{
				pNewDeviceInfoEnum->AddRef();
				*ppDeviceInfoEnum = pNewDeviceInfoEnum;
			}
		}
	}
	else
	{
		hr = E_POINTER;
	}

	return hr;
}


//STDMETHODIMP CSipWrapper::EnumerateVideoDevices(IUOLFoneClientDeviceInfoEnum** ppDeviceInfoEnum)
//{
//	HRESULT hr = E_FAIL;
//
//	return hr;
//}



STDMETHODIMP CSipWrapper::SetAudioDevice(EnumAudioDeviceType audioDeviceType, BSTR bstrDeviceName)
{
	HRESULT hr = E_FAIL;
	
	CString strDeviceName(bstrDeviceName);

	if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
	{
		if (m_phApiAdapter.SetCallInputAudioDevice(strDeviceName))
		{
			m_preferences.SetCurrentAudioInputDevice(strDeviceName);
			hr = S_OK;
		}
	}
	else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
	{
		if (m_phApiAdapter.SetCallOutputAudioDevice(strDeviceName))
		{
			m_preferences.SetCurrentAudioOutputDevice(strDeviceName);
			hr = S_OK;
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::GetAudioDevice(EnumAudioDeviceType audioDeviceType, BSTR* pbstrDeviceName)
{
	HRESULT hr = E_FAIL;

	if (pbstrDeviceName != NULL)
	{
		CString strDeviceName;

		if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
		{
			strDeviceName = m_preferences.GetCurrentAudioInputDevice();
			hr = S_OK;
		}
		else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
		{
			strDeviceName = m_preferences.GetCurrentAudioOutputDevice();
			hr = S_OK;
		}

		*pbstrDeviceName = strDeviceName.AllocSysString();
	}

	return hr;
}


STDMETHODIMP CSipWrapper::EnableAcousticEchoCancellation(BOOL bEnable)
{
	m_phApiAdapter.EnableAcousticEchoCancellation(bEnable);

	return S_OK;
}


STDMETHODIMP CSipWrapper::IsAcousticEchoCancellationEnabled(BOOL* pbEnabled)
{
	HRESULT hr = E_FAIL;

	if (pbEnabled != NULL)
	{
		*pbEnabled = m_phApiAdapter.IsAcousticEchoCancellationEnabled();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::EnableHalfDuplex(BOOL bEnable)
{
	m_phApiAdapter.EnableHalfDuplex(bEnable);

	return S_OK;
}


STDMETHODIMP CSipWrapper::IsHalfDuplexEnabled(BOOL* pbEnabled)
{
	HRESULT hr = E_FAIL;

	if (pbEnabled != NULL)
	{
		*pbEnabled = m_phApiAdapter.IsHalfDuplexEnabled();
		hr = S_OK;
	}

	return hr;
}



STDMETHODIMP CSipWrapper::SetVolumeLevel(EnumAudioDeviceType audioDeviceType, LONG lLevel)
{
	HRESULT hr = E_FAIL;

	BOOL bResult = FALSE;

	if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
	{
		bResult = CAudioDeviceManager::SetAudioInputVolumeLevel(m_preferences.GetCurrentAudioInputDevice(), lLevel);
	}
	else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
	{
		bResult = CAudioDeviceManager::SetAudioOutputVolumeLevel(m_preferences.GetCurrentAudioOutputDevice(), lLevel);
	}

	if (bResult)
	{
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::GetVolumeLevel(EnumAudioDeviceType audioDeviceType, LONG* plLevel)
{
	HRESULT hr = E_FAIL;

	if (plLevel != NULL)
	{
		BOOL bResult = FALSE;
		int nVolumeLevel = -1;

		if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
		{
			bResult = CAudioDeviceManager::GetAudioInputVolumeLevel(m_preferences.GetCurrentAudioInputDevice(), nVolumeLevel);
		}
		else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
		{
			bResult = CAudioDeviceManager::GetAudioOutputVolumeLevel(m_preferences.GetCurrentAudioOutputDevice(), nVolumeLevel);
		}

		if (bResult)
		{
			*plLevel = nVolumeLevel;
			hr = S_OK;
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::SetMute(EnumAudioDeviceType audioDeviceType, BOOL bMute)
{
	HRESULT hr = E_FAIL;

	BOOL bResult = FALSE;

	if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
	{
		// TODO: Can't mute wave-in device (WinXP test)
		//bResult = CAudioDeviceManager::MuteAudioInput(m_preferences.GetCurrentAudioInputDevice(), bMute);
		bResult = m_phApiAdapter.SetMicMute(bMute);
	}
	else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
	{
		bResult = CAudioDeviceManager::MuteAudioOutput(m_preferences.GetCurrentAudioOutputDevice(), bMute);
	}

	if (bResult)
	{
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CSipWrapper::GetMute(EnumAudioDeviceType audioDeviceType, BOOL* pbMute)
{
	HRESULT hr = E_FAIL;

	if (pbMute != NULL)
	{
		*pbMute = FALSE;

		if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_INPUT)
		{
			//*pbMute = CAudioDeviceManager::IsAudioInputMute(m_preferences.GetCurrentAudioInputDevice());
			 *pbMute = m_phApiAdapter.GetMicMute();

			hr = S_OK;
		}
		else if (audioDeviceType == UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT)
		{
			*pbMute = CAudioDeviceManager::IsAudioOutputMute(m_preferences.GetCurrentAudioOutputDevice());
			hr = S_OK;
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::SendInstantMessage(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrMessage, BSTR bstrUserURI)
{
	HRESULT hr = E_FAIL;

	if (m_phApiAdapter.IsInitialized())
	{
		LONG lPhoneLineId = -1;

		HRESULT hr2 = pPhoneLine->GetPhoneLineId(&lPhoneLineId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone line exists
			IUOLFoneClientPhoneLine* pRegisteredPhoneLine = m_phoneLineManager.GetPhoneLine(lPhoneLineId);

			if (pRegisteredPhoneLine != NULL)
			{
				CString strMessage(bstrMessage);
				CString strUserURI(bstrUserURI);

				if (m_phApiAdapter.SendInstantMessage(lPhoneLineId, strMessage, strUserURI))
				{
					hr = S_OK;
				}

				pRegisteredPhoneLine->Release();
				pRegisteredPhoneLine = NULL;
			}
		}
	}

	return hr;
}


STDMETHODIMP CSipWrapper::SendRingingNotification(IUOLFoneClientPhoneCall* pPhoneCall)
{
	HRESULT hr = E_FAIL;

	if (pPhoneCall)
	{
		LONG lPhoneCallId = -1;

		HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);

		if (SUCCEEDED(hr2))
		{
			// Check if phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

			if (pPlacedPhoneCall != NULL)
			{
				pPlacedPhoneCall->Release();

				// Hang-up call
				int nError = m_phApiAdapter.SendRingingNotification(lPhoneCallId);

				if (0 == nError)
				{
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}


/*
STDMETHODIMP CSipWrapper::JoinConferenceCall(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02)
{
	HRESULT hr = E_FAIL;
	
	if ( (pPhoneCall_01) && (pPhoneCall_02) )
	{
		LONG lPhoneCallId_01 = -1;
		LONG lPhoneCallId_02 = -1;
		LONG lPhoneCallState_01 = UFC_PHONE_CALL_STATE_UNKNOWN;
		LONG lPhoneCallState_02 = UFC_PHONE_CALL_STATE_UNKNOWN;
		
		hr = pPhoneCall_01->GetPhoneCallId(&lPhoneCallId_01);
		
		if (SUCCEEDED(hr))
		{
			hr = pPhoneCall_02->GetPhoneCallId(&lPhoneCallId_02);
		}
		
		if (SUCCEEDED(hr))
		{
			// Check if both phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall_01 = m_phoneCallManager.GetPhoneCall(lPhoneCallId_01);
			IUOLFoneClientPhoneCall* pPlacedPhoneCall_02 = m_phoneCallManager.GetPhoneCall(lPhoneCallId_02);
			
			if ( (pPlacedPhoneCall_01 != NULL) && (pPlacedPhoneCall_02 != NULL) )
			{
				// Check if both phone call are on hold state
				hr = pPlacedPhoneCall_01->GetPhoneCallState(&lPhoneCallState_01);
				
				if (SUCCEEDED(hr))
				{
					hr = pPlacedPhoneCall_02->GetPhoneCallState(&lPhoneCallState_02);
				}
				
				if (pPlacedPhoneCall_01 != NULL)
				{
					pPlacedPhoneCall_01->Release();
				}
				
				if (pPlacedPhoneCall_02 != NULL)
				{
					pPlacedPhoneCall_02->Release();
				}
				
				if ( (lPhoneCallState_01 == UFC_PHONE_CALL_STATE_HOLD) && 
						(lPhoneCallState_02 == UFC_PHONE_CALL_STATE_HOLD) )
				{
					int nError = m_phApiAdapter.JoinConferenceCall(lPhoneCallId_01, lPhoneCallId_02);
					
					if (0 == nError)
					{
						hr = S_OK;
					}
				}
			}
		}
	}
	
	return hr;
}


STDMETHODIMP CSipWrapper::SplitConferenceCall(IUOLFoneClientPhoneCall* pPhoneCall_01, IUOLFoneClientPhoneCall* pPhoneCall_02)
{
	HRESULT hr = E_FAIL;
	
	if ( (pPhoneCall_01) && (pPhoneCall_01) )
	{
		LONG lPhoneCallId_01 = -1;
		LONG lPhoneCallId_02 = -1;
		LONG lPhoneCallState_01 = UFC_PHONE_CALL_STATE_UNKNOWN;
		LONG lPhoneCallState_02 = UFC_PHONE_CALL_STATE_UNKNOWN;
		
		hr = pPhoneCall_01->GetPhoneCallId(&lPhoneCallId_01);
		
		if (SUCCEEDED(hr))
		{
			hr = pPhoneCall_02->GetPhoneCallId(&lPhoneCallId_02);
		}
		
		if (SUCCEEDED(hr))
		{
			// Check if both phone call exists
			IUOLFoneClientPhoneCall* pPlacedPhoneCall_01 = m_phoneCallManager.GetPhoneCall(lPhoneCallId_01);
			IUOLFoneClientPhoneCall* pPlacedPhoneCall_02 = m_phoneCallManager.GetPhoneCall(lPhoneCallId_02);
			
			if ( (pPlacedPhoneCall_01 != NULL) && (pPlacedPhoneCall_02 != NULL) )
			{
				int nError = m_phApiAdapter.SplitConferenceCall(lPhoneCallId_01, lPhoneCallId_02);
				
				if (0 == nError)
				{
					hr = S_OK;
				}
			}
			
			if (pPlacedPhoneCall_01)
			{
				pPlacedPhoneCall_01->Release();
			}
			
			if (pPlacedPhoneCall_02)
			{
				pPlacedPhoneCall_02->Release();
			}
		}
	}
	
	return hr;
}
*/


STDMETHODIMP CSipWrapper::CreateConferenceCall(IUOLFoneClientPhoneCall* pPhoneCall, IUOLFoneClientConferenceCall** pConferenceCall)
{
	HRESULT hr = E_FAIL;
	
	if ( (m_phApiAdapter.IsInitialized()) && (m_pConferenceCallObj == NULL) )
	{
		if ( (pPhoneCall != NULL) && (pConferenceCall != NULL) )
		{
			*pConferenceCall = NULL;
			
			LONG lPhoneCallId = -1;
			
			HRESULT hr2 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);
			
			if (SUCCEEDED(hr2))
			{
				// Check if phone call exists.
				IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);
				
				if (pPlacedPhoneCall != NULL)
				{
					// Create the conference.
					LONG lConferenceId = m_phApiAdapter.CreateConferenceCall(lPhoneCallId);
					
					if (lConferenceId == UFC_CONF_CONST_DEFAULT_CONFERENCE_ID_NUMBER)
					{
						// Create and update the conference object.
						hr = CComObject<CConferenceCall>::CreateInstance(&m_pConferenceCallObj);
						
						if (SUCCEEDED(hr))
						{
							m_pConferenceCallObj->SetConferenceId(lConferenceId);
							m_pConferenceCallObj->AddParticipant(lPhoneCallId);
							m_pConferenceCallObj->AddRef();	// Local reference.
							
							*pConferenceCall = m_pConferenceCallObj;
							m_pConferenceCallObj->AddRef();	// Reference returned.
							
							hr = S_OK;
						}
					}
					
					pPlacedPhoneCall->Release();
					pPlacedPhoneCall = NULL;
				}
			}
		}
	}
	
	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);
	
	return hr;
}


STDMETHODIMP CSipWrapper::ConferenceAddMember(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall)
{
	HRESULT hr = E_FAIL;
	
	if ( (m_pConferenceCallObj != NULL) && 
			(m_pConferenceCallObj->GetNumberOfParticipants() < UFC_CONF_CONST_MAX_NUMBER_OF_PARTICIPANTS) )
	{
		if ((pConferenceCall != NULL) && (pPhoneCall != NULL))
		{
			LONG lConferenceId = -1;
			LONG lPhoneCallId = -1;
			
			HRESULT hr2 = pConferenceCall->GetConferenceId(&lConferenceId);
			HRESULT hr3 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);
			
			if ( (SUCCEEDED(hr2)) && (SUCCEEDED(hr3)) )
			{
				// Check if phone call exists.
				IUOLFoneClientPhoneCall* pPlacedPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);
				
				if (pPlacedPhoneCall != NULL)
				{
					// Add the member to the conference.
					int iResult = m_phApiAdapter.ConferenceAddMember(lConferenceId, lPhoneCallId);
					
					if (!iResult)
					{
						m_pConferenceCallObj->AddParticipant(lPhoneCallId);
						
						hr = S_OK;
					}
					
					pPlacedPhoneCall->Release();
					pPlacedPhoneCall = NULL;
				}
			}
		}
	}
	
	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);
	
	return hr;
}


STDMETHODIMP CSipWrapper::ConferenceRemoveMember(IUOLFoneClientConferenceCall* pConferenceCall, IUOLFoneClientPhoneCall* pPhoneCall)
{
	HRESULT hr = E_FAIL;
	
	if (m_pConferenceCallObj != NULL)
	{
		if ((pConferenceCall != NULL) && (pPhoneCall != NULL))
		{
			LONG lConferenceId = -1;
			LONG lPhoneCallId = -1;
			
			HRESULT hr2 = pConferenceCall->GetConferenceId(&lConferenceId);
			HRESULT hr3 = pPhoneCall->GetPhoneCallId(&lPhoneCallId);
			
			if ( (SUCCEEDED(hr2)) && (SUCCEEDED(hr3)) )
			{
				// Check if phone call is registered on the conference.
				if (m_pConferenceCallObj->FindParticipant(lPhoneCallId) != UFC_CONF_CONST_INVALID_PARTICIPANT_INDEX)
				{
					// Remove the member from the conference.
					int iResult = m_phApiAdapter.ConferenceRemoveMember(lConferenceId, lPhoneCallId);
					
					if (!iResult)
					{
						m_pConferenceCallObj->RemoveParticipant(lPhoneCallId);
						
						hr = S_OK;
					}
				}
			}
		}
	}
	
	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);
	
	return hr;
}


STDMETHODIMP CSipWrapper::DestroyConferenceCall(IUOLFoneClientConferenceCall* pConferenceCall)
{
	HRESULT hr = E_FAIL;
	BOOL bRemoveFailed = FALSE;
	
	if (m_pConferenceCallObj != NULL)
	{
		LONG lConferenceId = -1;
		
		HRESULT hr2 = pConferenceCall->GetConferenceId(&lConferenceId);
		
		if (SUCCEEDED(hr2))
		{
			// If the participants were not removed yet, we need to do so.
			//
			int iNumberOfParticipants = m_pConferenceCallObj->GetNumberOfParticipants();
			
			for (int index = 1; index <= iNumberOfParticipants; index ++)
			{
				// Remove the member from the conference.
				int iResult = m_phApiAdapter.ConferenceRemoveMember(lConferenceId, 
						m_pConferenceCallObj->GetParticipantCallId(index));
				
				if (!iResult)
				{
					bRemoveFailed = TRUE;
					
					break;
				}
			}
			
			if (!bRemoveFailed)
			{
				m_pConferenceCallObj->Release();
				m_pConferenceCallObj = NULL;
				
				hr = S_OK;
			}
		}
	}
	
	ATLTRACE(_T("%s - hr = 0x%x\n"), __FUNCTION__, hr);
	
	return hr;
}


STDMETHODIMP CSipWrapper::SetLineCallForwardNumber(IUOLFoneClientPhoneLine* pPhoneLine, BSTR bstrCallForwardNumber)
{
	HRESULT hr = E_FAIL;
	
	if ( (m_phApiAdapter.IsInitialized()) && (pPhoneLine != NULL) )
	{
		LONG lPhoneLineId = -1;
		
		hr = pPhoneLine->GetPhoneLineId(&lPhoneLineId);
		
		if (SUCCEEDED(hr))
		{
			// Check if phone line exists
			IUOLFoneClientPhoneLine* pRegisteredPhoneLine = m_phoneLineManager.GetPhoneLine(lPhoneLineId);
			
			if (pRegisteredPhoneLine != NULL)
			{
				CString strCallForwardNumber(bstrCallForwardNumber);
				
				if (m_phApiAdapter.SetLineCallForwardNumber(lPhoneLineId, strCallForwardNumber))
				{
					hr = S_OK;
				}
				
				pRegisteredPhoneLine->Release();
				pRegisteredPhoneLine = NULL;
			}
		}
	}
	
	return hr;
}


STDMETHODIMP CSipWrapper::SetLineBusyFlag(IUOLFoneClientPhoneLine* pPhoneLine, BOOL bIsBusy)
{
	HRESULT hr = E_FAIL;
	
	if ( (m_phApiAdapter.IsInitialized()) && (pPhoneLine != NULL) )
	{
		LONG lPhoneLineId = -1;
		
		hr = pPhoneLine->GetPhoneLineId(&lPhoneLineId);
		
		if (SUCCEEDED(hr))
		{
			// Check if phone line exists
			IUOLFoneClientPhoneLine* pRegisteredPhoneLine = m_phoneLineManager.GetPhoneLine(lPhoneLineId);
			
			if (pRegisteredPhoneLine != NULL)
			{
				if (m_phApiAdapter.SetLineBusyFlag(lPhoneLineId, bIsBusy))
				{
					hr = S_OK;
				}
				
				pRegisteredPhoneLine->Release();
				pRegisteredPhoneLine = NULL;
			}
		}
	}
	
	return hr;
}


STDMETHODIMP CSipWrapper::Initialize(IUOLFoneClientCallback2* pCallback2, 
					  BSTR bstrCodecsPath, LONG lLogLevel)
{
	HRESULT hr = Initialize(pCallback2, bstrCodecsPath);
	
	if (SUCCEEDED(hr))
	{
		m_lLogLevel = lLogLevel;
	}
	
	return hr;
}


LRESULT CSipWrapper::OnPhoneLineEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spCallback != NULL)
	{
		const IPhoneLineProgressInfo* pPhoneLineProgressInfo = (const IPhoneLineProgressInfo*) wParam;

		if (pPhoneLineProgressInfo)
		{
			LONG lPhoneLineId = pPhoneLineProgressInfo->GetLineId();
			EnumPhoneLineState state = pPhoneLineProgressInfo->GetState();

			m_phoneLineManager.SetPhoneLineState(lPhoneLineId, state);

			IUOLFoneClientPhoneLine* pPhoneLine = NULL;

			// Get phone line object
			if (state != UFC_PHONE_LINE_STATE_REGISTERED)
			{
				// Remove unregisted phone line
				pPhoneLine = m_phoneLineManager.RemovePhoneLine(lPhoneLineId);
				//ATLASSERT(pPhoneLine != NULL);

				if (state == UFC_PHONE_LINE_STATE_UNREGISTERED)
				{
					m_phApiAdapter.FreeVoipLines();
				}
			}
			else
			{
				pPhoneLine = m_phoneLineManager.GetPhoneLine(lPhoneLineId);
				ATLASSERT(pPhoneLine != NULL);
			}

			if (pPhoneLine)
			{
				if (pPhoneLineProgressInfo->HasExtraHeaders())
				{
					CComObject<CExtraResponseData>* pExtraResponseData = NULL;

					HRESULT hr = CComObject<CExtraResponseData>::CreateInstance(&pExtraResponseData);

					if (SUCCEEDED(hr))
					{
						pExtraResponseData->AddRef();
						pExtraResponseData->SetExtraHeaders(pPhoneLineProgressInfo);

						m_spCallback->PhoneLineStateChanged(pPhoneLine, state, pExtraResponseData);

						pExtraResponseData->Release();
					}
				}
				else
				{
					m_spCallback->PhoneLineStateChanged(pPhoneLine, state, NULL);
				}

				pPhoneLine->Release();
				pPhoneLine = NULL;
			}
		}
	}

	return 0L;
}


LRESULT CSipWrapper::OnPhoneCallEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spCallback != NULL)
	{
		const IPhoneCallProgressInfo* pPhoneCallProgressInfo = (const IPhoneCallProgressInfo*) wParam;
		
		if (NULL == pPhoneCallProgressInfo)
		{
			return 0L;
		}
		
		LONG lPhoneCallId = pPhoneCallProgressInfo->GetCallId();
		
		IUOLFoneClientPhoneCall* pPhoneCall = NULL;
		
		EnumPhoneCallState state = pPhoneCallProgressInfo->GetCallState();
		
		// The state must be updated before the RemovePhoneCall below, for 
		// calls that were closed.
		m_phoneCallManager.SetPhoneCallState(lPhoneCallId, state);
		
		// Get phone call object
		if ((state != UFC_PHONE_CALL_STATE_CLOSED) &&
			(state != UFC_PHONE_CALL_STATE_ERROR) &&
			(state != UFC_PHONE_CALL_STATE_NO_ANSWER) && 
			(state != UFC_PHONE_CALL_STATE_BUSY))
		{
			pPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);
		}
		else
		{
			// Remove unsucessfull call from manager.
			//
			pPhoneCall = m_phoneCallManager.RemovePhoneCall(lPhoneCallId);
			ATLASSERT(pPhoneCall != NULL);
			
			// Call must be closed or PhoneLine unregister will fail.
			//
			if (pPhoneCall != NULL)
			{
				int nError = m_phApiAdapter.CloseCall(lPhoneCallId);
				//ATLASSERT(nError == 0);
				//
				// We won't validate the return here because "CloseCall" method 
				// calling for failed calls won't return "0" as expected.
				// The return will be "-1" indicating that no "dialog" was 
				// created, but we need to call it so the call is released and 
				// unregister works.
			}
		}
		
		if (state == UFC_PHONE_CALL_STATE_INCOMING)
		{
			IUOLFoneClientPhoneLine* pPhoneLine = m_phoneLineManager.GetPhoneLine(pPhoneCallProgressInfo->GetLineId());
			
			if (pPhoneLine)
			{
				pPhoneCall = m_phoneCallManager.AddPhoneCall(pPhoneLine, pPhoneCallProgressInfo->GetPhoneURI(), FALSE, lPhoneCallId);
				
				pPhoneLine->Release();
				pPhoneLine = NULL;
			}
		}
		
		if (pPhoneCall)
		{	
			if (pPhoneCallProgressInfo->HasExtraInfo())
			{
				CComObject<CExtraResponseData>* pExtraResponseData = NULL;
				
				HRESULT hr = CComObject<CExtraResponseData>::CreateInstance(&pExtraResponseData);
				
				if (SUCCEEDED(hr))
				{
					pExtraResponseData->AddRef();
					pExtraResponseData->SetExtraHeaders(pPhoneCallProgressInfo);
					
					m_spCallback->PhoneCallStateChanged(pPhoneCall, state, pExtraResponseData);
					
					pExtraResponseData->Release();
				}
			}
			else
			{
				m_spCallback->PhoneCallStateChanged(pPhoneCall, state, NULL);
			}
			
			pPhoneCall->Release();
		}
	}
	
	return 0L;
}


LRESULT CSipWrapper::OnPhoneCallAsyncEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spCallback != NULL)
	{
		LPPHONECALLASYNCEVENTPARAM pPcaeParam = (LPPHONECALLASYNCEVENTPARAM) wParam;

		if (pPcaeParam != NULL)
		{
			IUOLFoneClientPhoneCall* pPhoneCall = NULL;

			pPhoneCall = m_phoneCallManager.GetPhoneCall(pPcaeParam->lPhoneCallId);

			if (pPhoneCall != NULL)
			{
				m_phoneCallManager.SetPhoneCallState(pPcaeParam->lPhoneCallId, pPcaeParam->state);

				m_spCallback->PhoneCallStateChanged(pPhoneCall, pPcaeParam->state, NULL);

				pPhoneCall->Release();
			}

			delete pPcaeParam;
		}
	}

	return 0L;
}


LRESULT CSipWrapper::OnMessageProgressEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spCallback != NULL)
	{
		const IMessageProgressInfo* pMessageProgressInfo = (const IMessageProgressInfo*) wParam;

		if (pMessageProgressInfo)
		{
			CComObject<CMessageEvent>* pMessageEventObject;

			HRESULT hr = CComObject<CMessageEvent>::CreateInstance(&pMessageEventObject);

			if (SUCCEEDED(hr))
			{
				pMessageEventObject->SetEventType(pMessageProgressInfo->GetProgressType());
				pMessageEventObject->SetFrom(pMessageProgressInfo->GetFrom());
				pMessageEventObject->SetTo(pMessageProgressInfo->GetTo());
				pMessageEventObject->SetContentType(pMessageProgressInfo->GetContentType());
				pMessageEventObject->SetContentSubtype(pMessageProgressInfo->GetSubtype());
				pMessageEventObject->SetContent(pMessageProgressInfo->GetContent());

				CComQIPtr<IUOLFoneClientMessageEvent> pMessageEvent = pMessageEventObject;

				if (pMessageEvent)
				{
					m_spCallback->MessageEvent(pMessageEvent, NULL);
				}
			}
		}
	}

	return 0L;
}


LRESULT CSipWrapper::OnPhoneCallStreamError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPPHONECALLSTREAMERRORPARAM pPcseParam = (LPPHONECALLSTREAMERRORPARAM) wParam;

	if (pPcseParam != NULL)
	{
		LONG lPhoneCallId = (int) pPcseParam->lPhoneCallId;

		IUOLFoneClientPhoneCall* pPhoneCall = NULL;
		
		EnumPhoneCallStreamError error = pPcseParam->error;

		// Get phone call object
		pPhoneCall = m_phoneCallManager.GetPhoneCall(lPhoneCallId);

		if (pPhoneCall)
		{
			m_spCallback->PhoneCallStreamError(pPhoneCall, error);

			pPhoneCall->Release();
		}

		delete pPcseParam;
	}

	return 0L;
}


LRESULT CSipWrapper::OnPhoneLineConnectionTimeout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLTRACE(_T("%s - Connection timeout!!!\n"), __FUNCTION__);

	if (m_spCallback)
	{
		m_spCallback->PhoneLineConnectionTimeout();
	}

	return 0L;
}


LRESULT CSipWrapper::OnOsipDebugEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spCallback != NULL)
	{
		const IOsipDebugInfo* pDebugInfo = (const IOsipDebugInfo*) wParam;
		
		if (pDebugInfo)
		{
			CComObject<COsipDebugInfo>* pOsipDebugInfoObject;
			
			HRESULT hr = CComObject<COsipDebugInfo>::CreateInstance(&pOsipDebugInfoObject);
			
			if (SUCCEEDED(hr))
			{
				pOsipDebugInfoObject->SetFileName(pDebugInfo->GetFileName());
				pOsipDebugInfoObject->SetLineNumber(pDebugInfo->GetLineNumber());
				pOsipDebugInfoObject->SetLogLevel(pDebugInfo->GetLogLevel());
				pOsipDebugInfoObject->SetLogMessage(pDebugInfo->GetLogMessage());
				
				CComQIPtr<IUOLFoneClientOsipDebugInfo> pOsipDebugInfo = pOsipDebugInfoObject;
				
				if (pOsipDebugInfo)
				{
					CComQIPtr<IUOLFoneClientCallback2> pCallback2 = m_spCallback;
					
					if (pCallback2)
					{
						pCallback2->OsipDebugEvent(pOsipDebugInfo);
					}
				}
			}
			
			delete pDebugInfo;
		}
	}
	
	return 0L;
}


void CSipWrapper::HandlePhoneLineEvent(const IPhoneLineProgressInfo* pPhoneLineProgressInfo)
{
	if (FALSE == m_phApiAdapter.IsFinalizeInProgress())
	{
		if (pPhoneLineProgressInfo)
		{
			if ((0 == pPhoneLineProgressInfo->GetLineId()) &&
				(pPhoneLineProgressInfo->GetState() == UFC_PHONE_LINE_STATE_CONNECTION_TIMEOUT))
			{
				ATLTRACE(_T("UFC_PHONE_LINE_STATE_CONNECTION_TIMEOUT\n"));

				CWindow::SendNotifyMessage(WM_USER_PHONE_LINE_CONNECTION_TIMEOUT);
			}
			else
			{
				CWindow::SendMessage(WM_USER_PHONE_LINE_EVENT, (WPARAM) pPhoneLineProgressInfo, 0);
			}
		}
	}
}


void CSipWrapper::HandleCallProgressEvent(const IPhoneCallProgressInfo* pPhoneCallProgressInfo)
{
	if (FALSE == m_phApiAdapter.IsFinalizeInProgress())
	{
		CWindow::SendMessage(WM_USER_PHONE_CALL_EVENT, (WPARAM) pPhoneCallProgressInfo, 0);
	}
}


void CSipWrapper::HandleMessageProgressEvent(const IMessageProgressInfo* pMessageProgressInfo)
{
	if (FALSE == m_phApiAdapter.IsFinalizeInProgress())
	{
		CWindow::SendMessage(WM_USER_MESSAGE_PROGRESS_EVENT, (WPARAM) pMessageProgressInfo, 0);
	}
}


void CSipWrapper::HandleCallStreamError(LONG lPhoneLineId, int nCallId, EnumPhoneCallStreamError error)
{
	if (FALSE == m_phApiAdapter.IsFinalizeInProgress())
	{
		LPPHONECALLSTREAMERRORPARAM pPcseParam = new PHONECALLSTREAMERRORPARAM;
		pPcseParam->lPhoneLineId = lPhoneLineId;
		pPcseParam->lPhoneCallId = nCallId;
		pPcseParam->error = error;

		CWindow::PostMessage(WM_USER_PHONE_CALL_STREAM_ERROR, (WPARAM) pPcseParam, 0);
	}
}


void CSipWrapper::HandleOsipDebugEvent(const IOsipDebugInfo* pOsipDebugInfo)
{
	if (FALSE == m_phApiAdapter.IsFinalizeInProgress())
	{
		CWindow::PostMessage(WM_USER_OSIP_DEBUG_EVENT, (WPARAM) pOsipDebugInfo, 0);
	}
}


LONG CSipWrapper::GetLogLevel()
{
	return m_lLogLevel;
}

