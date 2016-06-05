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

#ifndef OWAUTOMATICSETTINGS_H
#define OWAUTOMATICSETTINGS_H

#include <settings/Settings.h>

#include <util/String.h>
#include <util/StringList.h>

/**
 * High level component above Settings, tries to make it impossible to make a mistake.
 *
 * Implements a default value for keys.
 *
 * FIXME merge with settings? I don't remember why this code is not inside Settings
 * probably because of the default value implementation + the serialization inside Settings -> problem?
 *
 * @see Settings
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class AutomaticSettings : public Settings {
public:

	OWSETTINGS_API AutomaticSettings();

	OWSETTINGS_API ~AutomaticSettings();

	/**
	 * @see Settings::getAllKeys()
	 */
	OWSETTINGS_API StringList getAllKeys() const;

	/**
	 * @see Settings::getAny()
	 */
	OWSETTINGS_API boost::any getAny(const std::string & key) const;

	/**
	 * @see getAny()
	 */
	OWSETTINGS_API boost::any getDefaultValue(const std::string & key) const;


	/**
	 * Resets to key to its default value.
	 *
	 * @param key key to reset
	 */
	OWSETTINGS_API void resetToDefaultValue(const std::string & key);

	/**
	 * Makes it impossible to use set() with a wrong value type.
	 *
	 * @see Settings::set()
	 */
	OWSETTINGS_API void set(const std::string & key, const std::string & value);

	/**
	 * @see set()
	 */
	OWSETTINGS_API void set(const std::string & key, const StringList & value);

	/**
	 * @see set()
	 */
	OWSETTINGS_API void set(const std::string & key, bool value);

	/**
	 * @see set()
	 */
	OWSETTINGS_API void set(const std::string & key, int value);

protected:

	/**
	 * Makes it impossible to use get() directly.
	 *
	 * @see Settings::get()
	 */
	OWSETTINGS_API std::string get(const std::string &, const std::string &) const {
		return String::null;
	}

	/**
	 * @see get()
	 */
	OWSETTINGS_API StringList get(const std::string &, const StringList &) const {
		static const StringList empty;
		return empty;
	}

	/**
	 * @see get()
	 */
	OWSETTINGS_API bool get(const std::string &, bool) const {
		return false;
	}

	/**
	 * @see get()
	 */
	OWSETTINGS_API int get(const std::string &, int) const {
		return 0;
	}

	/**
	 * @see get()
	 */
	OWSETTINGS_API boost::any getAny(const std::string &, const boost::any &) const {
		static const boost::any empty;
		return empty;
	}

	/**
	 * @see get()
	 */
	OWSETTINGS_API bool getBooleanKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	OWSETTINGS_API int getIntegerKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	OWSETTINGS_API std::string getStringKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	OWSETTINGS_API StringList getStringListKeyValue(const std::string & key) const;

	/** Associates a key to a default value. */
	Keys _keyDefaultValueMap;
};

#endif	//OWAUTOMATICSETTINGS_H
