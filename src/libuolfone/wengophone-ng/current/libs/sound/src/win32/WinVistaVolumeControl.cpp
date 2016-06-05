/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "WinVistaVolumeControl.h"

#include "EnumDeviceType.h"

#include <util/Logger.h>


/* <UOLFONE> - disable logging*/
#if !LOG_ENABLE
#undef  LOG_DEBUG
#define LOG_DEBUG(m)
#undef  LOG_INFO
#define LOG_INFO(m)
#undef  LOG_WARN
#define LOG_WARN(m)
#undef  LOG_ERROR
#define LOG_ERROR(m)	
#undef  LOG_FATAL
#define LOG_FATAL(m)	
#endif
/* </UOLFONE> */


#define			DEVICE_NAME_LIMIT			512

static const unsigned MAXIMUM_VOLUME_LEVEL_DEFINED_BY_USER = 100;

static const float MAXIMUM_VOLUME_LEVEL = 1.0;
static const float MINIMUM_VOLUME_LEVEL = 0.0;


WinVistaVolumeControl::WinVistaVolumeControl(const AudioDevice & audioDevice)
{
	std::string deviceName = audioDevice.getData()[0];
	
	std::string defaultStr = "(default) ";
	std::string::size_type defaultStrPos = 0;
	
	// Eliminates the "(default) " string appended by PortAudio on default 
	// device (if present, of course).
	// 
	defaultStrPos = deviceName.find(defaultStr);
	if (defaultStrPos != std::string::npos)
	{
		deviceName.erase(0, defaultStr.length());
	}
	
	m_pVistaDevice = GetVistaDevice(deviceName);
	
	if (!m_pVistaDevice)
	{
		m_pVistaDevice = GetVistaDefaultDevice(IsInputDevice(audioDevice));
	}
	
	_isSettable = (m_pVistaDevice != NULL);
}


WinVistaVolumeControl::~WinVistaVolumeControl()
{
	if (m_pVistaDevice)
	{
		m_pVistaDevice->Release();
	}
}


int WinVistaVolumeControl::getLevel()
{
	HRESULT hResult = S_OK;
	IAudioEndpointVolume* spAudioEndpointVolume = NULL;
	float fLevel = 0.0;
	
	if (!_isSettable)
	{
		return 0;
	}
	
	if (m_pVistaDevice)
	{
		hResult = m_pVistaDevice->Activate(__uuidof(IAudioEndpointVolume), 
				CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, 
				NULL, (void**)&spAudioEndpointVolume);
	}
	
	if ( (SUCCEEDED(hResult)) && (spAudioEndpointVolume != NULL) )
	{
		hResult = spAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel);
	}
	
	if (spAudioEndpointVolume)
	{
		spAudioEndpointVolume->Release();
	}
	
	if (FAILED(hResult))
	{
		return 0;
	}
	
	return (int) (((float) ((fLevel - MINIMUM_VOLUME_LEVEL) * MAXIMUM_VOLUME_LEVEL_DEFINED_BY_USER) /
		(float) (MAXIMUM_VOLUME_LEVEL - MINIMUM_VOLUME_LEVEL)) + 0.5);
}


bool WinVistaVolumeControl::setLevel(unsigned level)
{
	HRESULT hResult = S_OK;
	IAudioEndpointVolume* spAudioEndpointVolume = NULL;
	float fLevel = level * (MAXIMUM_VOLUME_LEVEL - MINIMUM_VOLUME_LEVEL) / MAXIMUM_VOLUME_LEVEL_DEFINED_BY_USER;
	
	if (!_isSettable)
	{
		return false;
	}
	
	if (m_pVistaDevice)
	{
		hResult = m_pVistaDevice->Activate(__uuidof(IAudioEndpointVolume), 
				CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, 
				NULL, (void**)&spAudioEndpointVolume);
	}
	
	if ( (SUCCEEDED(hResult)) && (spAudioEndpointVolume != NULL) )
	{
		hResult = spAudioEndpointVolume->SetMasterVolumeLevelScalar(fLevel, NULL);
	}
	
	if (spAudioEndpointVolume)
	{
		spAudioEndpointVolume->Release();
	}
	
	if (FAILED(hResult))
	{
		return false;
	}
	
	return true;
}


bool WinVistaVolumeControl::setMute(bool mute)
{
	HRESULT hResult = S_OK;
	IAudioEndpointVolume* spAudioEndpointVolume = NULL;
	UINT bMute = (mute == true) ? 1 : 0;
	
	if (!_isSettable)
	{
		return false;
	}
	
	if (m_pVistaDevice)
	{
		hResult = m_pVistaDevice->Activate(__uuidof(IAudioEndpointVolume), 
				CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, 
				NULL, (void**)&spAudioEndpointVolume);
	}
	
	if ( (SUCCEEDED(hResult)) && (spAudioEndpointVolume != NULL) )
	{
		hResult = spAudioEndpointVolume->SetMute(bMute, NULL);
	}
	
	if (spAudioEndpointVolume)
	{
		spAudioEndpointVolume->Release();
	}
	
	if (FAILED(hResult))
	{
		return false;
	}
	
	return true;
}


bool WinVistaVolumeControl::isMuted()
{
	HRESULT hResult = S_OK;
	IAudioEndpointVolume* spAudioEndpointVolume = NULL;
	int bIsMute = 0;
	
	if (!_isSettable)
	{
		return false;
	}
	
	if (m_pVistaDevice)
	{
		hResult = m_pVistaDevice->Activate(__uuidof(IAudioEndpointVolume), 
				CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, 
				NULL, (void**)&spAudioEndpointVolume);
	}
	
	if ( (SUCCEEDED(hResult)) && (spAudioEndpointVolume != NULL) )
	{
		hResult = spAudioEndpointVolume->GetMute(&bIsMute);
	}
	
	if (spAudioEndpointVolume)
	{
		spAudioEndpointVolume->Release();
	}
	
	if (FAILED(hResult))
	{
		return false;
	}
	
	return (bIsMute != NULL);
}


IMMDevice* WinVistaVolumeControl::GetVistaDevice(const std::string & deviceName)
{
	HRESULT hResult = S_OK;
	IMMDeviceEnumerator* spDeviceEnumerator = NULL;
	IMMDeviceCollection* spEndpoints = 0;
	IMMDevice* spRetIMMDevice = NULL;
	IMMDevice* spLoopIMMDevice = NULL;
	UINT iDeviceCount = 0;
	DWORD dwDeviceState = 0;
	IPropertyStore* spProperties = NULL;
	PROPVARIANT devicePropValue;
	char searchingDeviceName[DEVICE_NAME_LIMIT];
	WCHAR* pszDeviceId = NULL;
	
	hResult = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IMMDeviceEnumerator),
			(void**)&spDeviceEnumerator);
	
	if ( (SUCCEEDED(hResult)) && (spDeviceEnumerator != NULL) )
	{
		hResult = spDeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &spEndpoints);
	}
	
	if ( (SUCCEEDED(hResult)) && (spEndpoints != NULL) )
	{
		hResult = spEndpoints->GetCount(&iDeviceCount);
	}
	
	if (SUCCEEDED(hResult))
	{
		for (UINT index = 0; index < iDeviceCount; index++)
		{
			hResult = spEndpoints->Item(index, &spLoopIMMDevice);
			
			if ( (SUCCEEDED(hResult)) && (spLoopIMMDevice != NULL) )
			{
				hResult = spLoopIMMDevice->GetState(&dwDeviceState);
			}
			
			if ( (SUCCEEDED(hResult)) && (dwDeviceState == DEVICE_STATE_ACTIVE) )
			{
				hResult = spLoopIMMDevice->OpenPropertyStore(STGM_READ, &spProperties);
			}
			
			if ( (SUCCEEDED(hResult)) && (spProperties != NULL) )
			{
				PropVariantInit(&devicePropValue);
				hResult = spProperties->GetValue(PKEY_Device_FriendlyName, &devicePropValue);
				
				if ( (SUCCEEDED(hResult)) && (devicePropValue.pwszVal) )
				{
					// We MUST consider here that we can receive a device name collected 
					// with diferents Windows API's, what can lead us to incorrect 
					// string values to compare.
					// For example: the WMME implementation used by PortAudio (one of 
					// them) captures only the first 31 characters of the device name, 
					// while the WASAPI (new Vista API) used here gets the full device 
					// friendly name.
					// So, we need to restrict the size of the string collected here 
					// (throughout WASAPI) and to be compared to the size of the 
					// device name passed as parameter.
					//
					memset(searchingDeviceName, 0, DEVICE_NAME_LIMIT);
					wcstombs(searchingDeviceName, devicePropValue.pwszVal, deviceName.length());
					
					if (lstrcmpi(searchingDeviceName, deviceName.c_str()) == 0)
					{
						spRetIMMDevice = spLoopIMMDevice;
						spRetIMMDevice->AddRef();
						
						break;
					}
				}
				
				PropVariantClear(&devicePropValue);
			}
		}
	}
	
	// Release temporary variables.
	//
	if (spDeviceEnumerator)
	{
		spDeviceEnumerator->Release();
	}
	
	if (spEndpoints)
	{
		spEndpoints->Release();
	}
	
	return spRetIMMDevice;
}


IMMDevice* WinVistaVolumeControl::GetVistaDefaultDevice(const bool bIsInputDevice)
{
	HRESULT hResult = S_OK;
	IMMDeviceEnumerator* spDeviceEnumerator = NULL;
	IMMDevice* spDefaultDevice = NULL;
	
	hResult = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IMMDeviceEnumerator),
			(void**)&spDeviceEnumerator);
	
	if ( (SUCCEEDED(hResult)) && (spDeviceEnumerator != NULL) )
	{
		hResult = spDeviceEnumerator->GetDefaultAudioEndpoint(
				(bIsInputDevice == true) ? eCapture : eRender, 
				eMultimedia, 
				&spDefaultDevice);
	}
	
	// Release temporary variables.
	//
	if (spDeviceEnumerator)
	{
		spDeviceEnumerator->Release();
	}
	
	return spDefaultDevice;
}


bool WinVistaVolumeControl::IsInputDevice(const AudioDevice & audioDevice)
{
	bool bIsInputDevice = false;
	
	EnumDeviceType::DeviceType deviceType = 
			EnumDeviceType::toDeviceType(audioDevice.getData()[2]);
	
	switch (deviceType)
	{
		// Devices listed by PortAudio don't consider these options.
		// So we don't too...
		//
		/*
		case EnumDeviceType::DeviceTypeWaveOut:
		case EnumDeviceType::DeviceTypeCDOut:
		case EnumDeviceType::DeviceTypeMicrophoneOut:
		case EnumDeviceType::DeviceTypeMicrophoneIn:
		*/
		
		case EnumDeviceType::DeviceTypeWaveIn:
			bIsInputDevice = true;
		break;
		
		case EnumDeviceType::DeviceTypeMasterVolume:
			bIsInputDevice = false;
		break;
	}
	
	return bIsInputDevice;
}

