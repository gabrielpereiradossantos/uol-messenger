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
#include "./AudioDeviceManager.h"

#pragma warning(disable:4290)
#include <sound/AudioDeviceManager.h>
#include <sound/IVolumeControl.h>
#include <sound/VolumeControl.h>


void CAudioDeviceManager::GetAudioInputDeviceList(CAtlList<CString>& listDevices)
{
	std::list<AudioDevice> inputDeviceList = AudioDeviceManager::getInstance().getInputDeviceList();

	CopyDeviceList(inputDeviceList, listDevices);
}


void CAudioDeviceManager::GetAudioOutputDeviceList(CAtlList<CString>& listDevices)
{
	std::list<AudioDevice> outputDeviceList = AudioDeviceManager::getInstance().getOutputDeviceList();

	CopyDeviceList(outputDeviceList, listDevices);
}


CString CAudioDeviceManager::GetDefaultAudioInputDevice()
{
	AudioDevice audioDevice	= AudioDeviceManager::getInstance().getDefaultInputDevice();

	CString strDeviceName = audioDevice.getName().c_str();

	return strDeviceName;
}


CString CAudioDeviceManager::GetDefaultAudioOutputDevice()
{
	AudioDevice audioDevice	= AudioDeviceManager::getInstance().getDefaultOutputDevice();

	CString strDeviceName = audioDevice.getName().c_str();

	return strDeviceName;
}


BOOL CAudioDeviceManager::SetAudioInputVolumeLevel(const CString& strDeviceName, int lLevel)
{
	if ((lLevel >= 0) && (lLevel <= 100))
	{
		AudioDevice audioDevice = GetAudioInputDevice(strDeviceName);

		return InternalSetAudioDeviceVolumeLevel(audioDevice, lLevel);
	}

	return FALSE;
}


BOOL CAudioDeviceManager::SetAudioOutputVolumeLevel(const CString& strDeviceName, int lLevel)
{
	if ((lLevel >= 0) && (lLevel <= 100))
	{
		AudioDevice audioDevice = GetAudioOutputDevice(strDeviceName);

		return InternalSetAudioDeviceVolumeLevel(audioDevice, lLevel);
	}

	return FALSE;
}


BOOL CAudioDeviceManager::GetAudioInputVolumeLevel(const CString& strDeviceName, int& lLevel)
{
	AudioDevice audioDevice = GetAudioInputDevice(strDeviceName);

	return InternalGetAudioDeviceVolumeLevel(audioDevice, lLevel);
}


BOOL CAudioDeviceManager::GetAudioOutputVolumeLevel(const CString& strDeviceName, int& lLevel)
{
	AudioDevice audioDevice = GetAudioOutputDevice(strDeviceName);

	return InternalGetAudioDeviceVolumeLevel(audioDevice, lLevel);
}


BOOL CAudioDeviceManager::MuteAudioInput(const CString& strDeviceName, BOOL bMute)
{
	AudioDevice audioDevice = GetAudioInputDevice(strDeviceName);

	return InternalMuteAudioDevice(audioDevice, bMute);
}


BOOL CAudioDeviceManager::MuteAudioOutput(const CString& strDeviceName, BOOL bMute)
{
	AudioDevice audioDevice = GetAudioOutputDevice(strDeviceName);

	return InternalMuteAudioDevice(audioDevice, bMute);
}


BOOL CAudioDeviceManager::IsAudioInputMute(const CString& strDeviceName)
{
	AudioDevice audioDevice = GetAudioInputDevice(strDeviceName);

	return InternalIsAudioDeviceMute(audioDevice);
}


BOOL CAudioDeviceManager::IsAudioOutputMute(const CString& strDeviceName)
{
	AudioDevice audioDevice = GetAudioOutputDevice(strDeviceName);

	return InternalIsAudioDeviceMute(audioDevice);
}


void CAudioDeviceManager::CopyDeviceList(std::list<AudioDevice>& listSource, CAtlList<CString>& listDestiny)
{
	listDestiny.RemoveAll();

	std::list<AudioDevice>::iterator it;

	for (it = listSource.begin(); it != listSource.end(); it++)
	{
		listDestiny.AddTail((*it).getName().c_str());
	}
}


AudioDevice CAudioDeviceManager::GetAudioInputDevice(const CString& strDeviceName)
{
	AudioDevice audioDevice = AudioDeviceManager::getInstance().getDefaultInputDevice();

	std::list<AudioDevice> listDevices = AudioDeviceManager::getInstance().getInputDeviceList();

	std::list<AudioDevice>::iterator itDevice;

	for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++)
	{
		AudioDevice tempAudioDevice = *itDevice;
		
		if (0 == strDeviceName.CompareNoCase(tempAudioDevice.getName().c_str()))
		{
			audioDevice = tempAudioDevice;
			break;		
		}
	}

	return audioDevice;
}


AudioDevice CAudioDeviceManager::GetAudioOutputDevice(const CString& strDeviceName)
{
	AudioDevice audioDevice = AudioDeviceManager::getInstance().getDefaultOutputDevice();

	std::list<AudioDevice> listDevices = AudioDeviceManager::getInstance().getOutputDeviceList();

	std::list<AudioDevice>::iterator itDevice;

	for (itDevice = listDevices.begin(); itDevice != listDevices.end(); itDevice++)
	{
		AudioDevice tempAudioDevice = *itDevice;
		
		if (0 == strDeviceName.CompareNoCase(tempAudioDevice.getName().c_str()))
		{
			audioDevice = tempAudioDevice;
			break;		
		}
	}

	return audioDevice;
}



BOOL CAudioDeviceManager::InternalSetAudioDeviceVolumeLevel(const AudioDevice& audioDevice, int lLevel)
{
	VolumeControl volumeControl(audioDevice);

	if (volumeControl.isSettable())
	{
		return volumeControl.setLevel(lLevel);
	}

	return FALSE;
}


BOOL CAudioDeviceManager::InternalGetAudioDeviceVolumeLevel(const AudioDevice& audioDevice, int& lLevel)
{
	VolumeControl volumeControl(audioDevice);

	lLevel = volumeControl.getLevel();

	return (lLevel != -1);
}


BOOL CAudioDeviceManager::InternalMuteAudioDevice(const AudioDevice& audioDevice, BOOL bMute)
{
	VolumeControl volumeControl(audioDevice);

	if (volumeControl.isSettable())
	{
		return volumeControl.setMute(bMute != FALSE);
	}

	return FALSE;
}


BOOL CAudioDeviceManager::InternalIsAudioDeviceMute(const AudioDevice& audioDevice)
{
	VolumeControl volumeControl(audioDevice);

	return volumeControl.isMuted();
}