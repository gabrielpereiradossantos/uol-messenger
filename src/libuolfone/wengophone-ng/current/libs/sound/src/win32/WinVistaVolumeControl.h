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

#ifndef OWWINVISTAVOLUMECONTROL_H
#define OWWINVISTAVOLUMECONTROL_H


#include <sound/IVolumeControl.h>
#include <sound/AudioDevice.h>

#include "../EnumDeviceType.h"


#include <windows.h>
#include <MMReg.h>  //must be before other Wasapi headers
#include <strsafe.h>
#include <mmdeviceapi.h>
#include <Avrt.h>
#include <audioclient.h>
#include <KsMedia.h>
#include <Endpointvolume.h>
#include <initguid.h>
#include <functiondiscoverykeys.h>  // PKEY_Device_FriendlyName


/**
 * Gets and changes the volume of a Windows Vista sound mixer.
 *
 * @author Mauro Sergio Ferreira Brasil
 */
class WinVistaVolumeControl : public IVolumeControl {
public:
	
	/**
	 * Constructs a WinVistaVolumeControl object.
	 *
	 * @param audioDevice Windows audio device
	 */
	WinVistaVolumeControl(const AudioDevice & audioDevice);
	
	~WinVistaVolumeControl();
	
	bool setLevel(unsigned level);
	
	int getLevel();
	
	bool setMute(bool mute);
	
	bool isMuted();
	
	bool isSettable() const
	{
		return _isSettable;
	};
	
private:
	
	enum
	{
		SET_LEVEL_OPERATION,
		GET_LEVEL_OPERATION,
		SET_MUTE_OPERATION,
		GET_MUTE_OPERATION
	};
	
private:
	
	IMMDevice* GetVistaDevice(const std::string & deviceName);
	
	IMMDevice* GetVistaDefaultDevice(const bool bIsInputDevice);
	
	bool IsInputDevice(const AudioDevice & audioDevice);
	
	/**
	 * Inits the ISimpleAudioVolume interface pointer.
	 *
	 * @param IMMDevice interface.
	 * 
	 * @return success boolean
	 */
	//bool initVolumeControl(IMMDevice* pIMMDevice);
	
	IMMDevice*				m_pVistaDevice;
	
	bool _isSettable;
};


#endif	//OWWINVISTAVOLUMECONTROL_H
