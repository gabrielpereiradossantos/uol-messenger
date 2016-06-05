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

#include <settings/Settings.h>

#include <settings/StringListXMLSerializer.h>

#include <util/StringList.h>
#include <util/Logger.h>

using namespace std;

Settings::Settings() {
}

Settings::Settings(const Settings & settings) {
	copy(settings);
}

Settings & Settings::operator=(const Settings & settings) {
	copy(settings);
	return *this;
}

void Settings::copy(const Settings & settings) {
	_keyMap = settings._keyMap;
	//FIXME: should we copy the event? valueChangedEvent = settings.valueChangedEvent;
}

Settings::~Settings() {
}

StringList Settings::getAllKeys() const {
	StringList tmp;
	for (Keys::const_iterator it = _keyMap.begin(); it != _keyMap.end(); ++it) {
		tmp += it->first;
	}
	return tmp;
}

void Settings::remove(const std::string & key) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	Keys::iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		_keyMap.erase(it);
	} else {
		//Erasing a group of key

		//Looking for the first key
		Keys::iterator it = _keyMap.begin();
		while ((it != _keyMap.end())
			&& (!(String(it->first).beginsWith(key)))) {
			it++;
		}

		//Erasing keys
		while ((it != _keyMap.end()) && (String(it->first).beginsWith(key))) {
			_keyMap.erase(it);
			it++;
		}
	}
}

bool Settings::contains(const std::string & key) const {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		return true;
	}
	return false;
}

void Settings::set(const std::string & key, const std::string & value) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if ((_keyMap.find(key) == _keyMap.end()) ||
		((boost::any_cast<std::string>(_keyMap[key]) != value))) {
		_keyMap[key] = value;
		valueChangedEvent(*this, key);
	}
}

void Settings::set(const std::string & key, const StringList & value) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);
	
	if ((_keyMap.find(key) == _keyMap.end()) ||
		((boost::any_cast<StringList>(_keyMap[key]) != value))) {
		_keyMap[key] = value;
		valueChangedEvent(*this, key);
	}
}

void Settings::set(const std::string & key, bool value) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if ((_keyMap.find(key) == _keyMap.end()) ||
		((boost::any_cast<bool>(_keyMap[key]) != value))) {
		_keyMap[key] = value;
		valueChangedEvent(*this, key);
	}
}

void Settings::set(const std::string & key, int value) {
	RecursiveMutex::ScopedLock scopedLock(_mutex);

	if ((_keyMap.find(key) == _keyMap.end()) ||
		((boost::any_cast<int>(_keyMap[key]) != value))) {
		_keyMap[key] = value;
		valueChangedEvent(*this, key);
	}
}

void Settings::set(const std::string & key, boost::any value) {
	if (isBoolean(value)) {
		set(key, boost::any_cast<bool>(value));
	} else if (isInteger(value)) {
		set(key, boost::any_cast<int>(value));
	} else if (isString(value)) {
		set(key, boost::any_cast<std::string>(value));
	} else if (isStringList(value)) {
		set(key, boost::any_cast<StringList>(value));
	}
}

std::string Settings::get(const std::string & key, const std::string & defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isString(it->second)) {
			return boost::any_cast<std::string>(it->second);
		} else {
			LOG_FATAL("the key doesn't match a string value");
		}
	}

	return defaultValue;
}

StringList Settings::get(const std::string & key, const StringList & defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isStringList(it->second)) {
			return boost::any_cast<StringList>(it->second);
		} else {
			LOG_FATAL("the key doesn't match a StringList value");
		}
	}

	return defaultValue;
}

bool Settings::get(const std::string & key, bool defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isBoolean(it->second)) {
			return boost::any_cast<bool>(it->second);
		} else {
			LOG_FATAL("the key doesn't match a boolean value");
		}
	}

	return defaultValue;
}

int Settings::get(const std::string & key, int defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isInteger(it->second)) {
			return boost::any_cast<int>(it->second);
		} else {
			LOG_FATAL("the key doesn't match an integer value");
		}
	}

	return defaultValue;
}

boost::any Settings::getAny(const std::string & key, const boost::any & defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		return it->second;
	}

	return defaultValue;
}

bool Settings::isBoolean(const boost::any & value) {
	return (value.type() == typeid(bool));
}

bool Settings::isInteger(const boost::any & value) {
	return (value.type() == typeid(int));
}

bool Settings::isString(const boost::any & value) {
	return boost::any_cast<std::string>(&value);
}

bool Settings::isStringList(const boost::any & value) {
	return boost::any_cast<StringList>(&value);
}
