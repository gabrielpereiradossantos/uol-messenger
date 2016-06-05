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

#include <sound/VolumeControl.h>

#include <cutil/global.h>

#if defined(OS_WINDOWS)
	#include "win32/Win32VolumeControl.h"
	/* <UOLFONE> */
#if _MSC_VER >= 1400
	#include "win32/WinVistaVolumeControl.h"
#endif
	/* </UOLFONE> */
#elif defined(OS_MACOSX)
	#include "mac/MacVolumeControl.h"
#elif defined(OS_LINUX)
	#if defined(OWSOUND_PORTAUDIO_SUPPORT)
		#include "unix/UnixVolumeControl.h"
	#else
		#include "linux/LinuxVolumeControl.h"
	#endif
#else
	#error This OS has not been tested
#endif

VolumeControl::VolumeControl(const AudioDevice & audioDevice) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

#if defined(OS_WINDOWS)
	/* <UOLFONE> */
#if _MSC_VER >= 1400
	OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof( osvi );
	GetVersionEx( &osvi );
	
    // Check for Windows Vista - So we can use the apropriate implementation.
    if ( (osvi.dwMajorVersion == 6) && (osvi.dwPlatformId == 2) )
    {
		_iVolumeControlPrivate = new WinVistaVolumeControl(audioDevice);
	}
	else
	{
		_iVolumeControlPrivate = new Win32VolumeControl(audioDevice);
	}
#else
	_iVolumeControlPrivate = new Win32VolumeControl(audioDevice);
#endif
	/* </UOLFONE> */
#elif defined(OS_MACOSX)
	_iVolumeControlPrivate = new MacVolumeControl(audioDevice);
#elif defined(OS_LINUX)
	#if defined(OWSOUND_PORTAUDIO_SUPPORT)
		_iVolumeControlPrivate = new UnixVolumeControl(audioDevice);
	#else
		_iVolumeControlPrivate = new LinuxVolumeControl(audioDevice);
	#endif
#endif
}

VolumeControl::~VolumeControl() {
	delete _iVolumeControlPrivate;
}

bool VolumeControl::setLevel(unsigned level) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->setLevel(level);
	} else {
		return false;
	}
}

int VolumeControl::getLevel() {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->getLevel();
	} else {
		return 0;
	}
}

bool VolumeControl::setMute(bool mute) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->setMute(mute);
	} else {
		return false;
	}
}

bool VolumeControl::isMuted() {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->isMuted();
	} else {
		return true;
	}
}

bool VolumeControl::isSettable() const {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->isSettable();
	} else {
		return false;
	}
}
