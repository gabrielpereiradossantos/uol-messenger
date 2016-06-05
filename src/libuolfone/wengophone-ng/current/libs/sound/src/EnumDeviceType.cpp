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


#include <map>

typedef std::map<EnumDeviceType::DeviceType, std::string> DeviceTypeMap;
static DeviceTypeMap _deviceTypeMap;

static void init() {
	_deviceTypeMap[EnumDeviceType::DeviceTypeMasterVolume] = "DeviceTypeMasterVolume";
	_deviceTypeMap[EnumDeviceType::DeviceTypeWaveOut] = "DeviceTypeWaveOut";
	_deviceTypeMap[EnumDeviceType::DeviceTypeWaveIn] = "DeviceTypeWaveIn";
	_deviceTypeMap[EnumDeviceType::DeviceTypeCDOut] = "DeviceTypeCDOut";
	_deviceTypeMap[EnumDeviceType::DeviceTypeMicrophoneOut] = "DeviceTypeMicrophoneOut";
	_deviceTypeMap[EnumDeviceType::DeviceTypeMicrophoneIn] = "DeviceTypeMicrophoneIn";
}

std::string EnumDeviceType::toString(DeviceType deviceType) {
	init();
	std::string tmp = _deviceTypeMap[deviceType];
	if (tmp.empty()) {
		LOG_WARN("unknown DeviceType=" + String::fromNumber(deviceType));
	}
	return tmp;
}

EnumDeviceType::DeviceType EnumDeviceType::toDeviceType(const std::string & deviceType) {
	init();
	for (DeviceTypeMap::const_iterator it = _deviceTypeMap.begin();
		it != _deviceTypeMap.end();
		++it) {

		if ((*it).second == deviceType) {
			return (*it).first;
		}
	}

	LOG_WARN("unknown DeviceType=" + deviceType);
	return DeviceTypeMasterVolume;
}
