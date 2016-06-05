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

#include <settings/AutomaticSettings.h>

#include <util/Logger.h>

AutomaticSettings::AutomaticSettings() {
}

AutomaticSettings::~AutomaticSettings() {
}

void AutomaticSettings::set(const std::string & key, const std::string & value) {
	boost::any defaultValue = getDefaultValue(key);

	if (isString(defaultValue)) {
		Settings::set(key, value);
	} else {
		LOG_FATAL("the default value doesn't match a string value");
	}
}

void AutomaticSettings::set(const std::string & key, const StringList & value) {
	boost::any defaultValue = getDefaultValue(key);

	if (isStringList(defaultValue)) {
		Settings::set(key, value);
	} else {
		LOG_FATAL("the default value doesn't match a StringList value");
	}
}

void AutomaticSettings::set(const std::string & key, bool value) {
	boost::any defaultValue = getDefaultValue(key);

	if (isBoolean(defaultValue)) {
		Settings::set(key, value);
	} else {
		LOG_FATAL("the default value doesn't match a boolean value");
	}
}

void AutomaticSettings::set(const std::string & key, int value) {
	boost::any defaultValue = getDefaultValue(key);

	if (isInteger(defaultValue)) {
		Settings::set(key, value);
	} else {
		LOG_FATAL("the default value doesn't match an integer value");
	}
}


StringList AutomaticSettings::getAllKeys() const {
	StringList tmp;
	for (Keys::const_iterator it = _keyDefaultValueMap.begin(); it != _keyDefaultValueMap.end(); ++it) {
		tmp += it->first;
	}
	return tmp;
}

boost::any AutomaticSettings::getAny(const std::string & key) const {
	return Settings::getAny(key, getDefaultValue(key));
}

boost::any AutomaticSettings::getDefaultValue(const std::string & key) const {
	Keys::const_iterator it = _keyDefaultValueMap.find(key);
	if (it == _keyDefaultValueMap.end()) {
		LOG_FATAL("key=" + key + " not found, add it inside the AutomaticSettings constructor");
	}

	boost::any defaultValue = it->second;
	if (defaultValue.empty()) {
		LOG_FATAL("default value for key=" + key + " not defined, add it inside the AutomaticSettings constructor");
	}

	return defaultValue;
}

bool AutomaticSettings::getBooleanKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isBoolean(value)) {
		LOG_ERROR("value for key=" + key + " is not a boolean");
		return false;
	}

	return boost::any_cast<bool>(value);
}

int AutomaticSettings::getIntegerKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isInteger(value)) {
		LOG_ERROR("value for key=" + key + " is not an integer");
		return -1;
	}

	return boost::any_cast<int>(value);
}

std::string AutomaticSettings::getStringKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isString(value)) {
		LOG_ERROR("value for key=" + key + " is not a string");
		return String::null;
	}

	return boost::any_cast<std::string>(value);
}

StringList AutomaticSettings::getStringListKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isStringList(value)) {
		LOG_ERROR("value for key=" + key + " is not a string list");
		return StringList::null;
	}

	return boost::any_cast<StringList>(value);
}

void AutomaticSettings::resetToDefaultValue(const std::string & key) {
	Settings::set(key, getDefaultValue(key));
}
