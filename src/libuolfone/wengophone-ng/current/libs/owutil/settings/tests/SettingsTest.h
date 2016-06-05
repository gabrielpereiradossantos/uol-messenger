/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef SETTINGSTEST_H
#define SETTINGSTEST_H

#include <Settings.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

class SettingsTest {
public:

	SettingsTest() {
	}

	void testSettings() {
		Settings settings;
		const std::string key1 = "audio.speakers.left.volume";
		const int value1 = 15;
		const std::string key2 = "video.card";
		const std::string value2 = "PrettyPixel";
		const std::string value3 = "not found";
		
		settings.set(key1, value1);
		settings.set(key2, value2);
		
		int ret1 = settings.get(key1, 50);
		BOOST_CHECK(ret1 == value1);
		
		string ret2 = settings.get(key2, value3);
		BOOST_CHECK(ret2 == value2);
		
		string ret3 = settings.get("invalid key", value3);
		BOOST_CHECK(ret3 == value3);
	}
	
	void testAllKeys() {
		StringList keyList, keysFromSettings;
		Settings settings;

		keyList += "audio.test";
		keyList += "audio.speakers.front.left.name";
		keyList += "video.format.bpp";
		keyList += "video.card";
		keyList += "audio.speakers.rear.right.volume";
		keyList += "video.format.height";
		keyList += "audio.main.noise";
		keyList += "video.format.width";
		
		for (register unsigned i = 0 ; i < keyList.size() ; i++) {
			settings.set(keyList[i], string("key test"));
		}

		//Sorting keyList
		keyList.sort();

		//All key must be present and sorted
		keysFromSettings = settings.getAllKeys();

		BOOST_CHECK(keyList.size() == keysFromSettings.size());
		for (register unsigned i = 0 ; i <  keyList.size() ; i++) {
			BOOST_CHECK(keyList[i] == keysFromSettings[i]);
		}
	}
	
	testRemove() {
		StringList keyList, keysFromSettings;
		Settings settings;

		keyList += "audio.test";
		keyList += "audio.speakers.front.left.name";
		keyList += "video.format.bpp";
		keyList += "video.card";
		keyList += "audio.speakers.rear.right.volume";
		keyList += "video.format.height";
		keyList += "audio.main.noise";
		keyList += "video.format.width";
		
		for (register unsigned i = 0 ; i < keyList.size() ; i++) {
			settings.set(keyList[i], string("key test"));
		}

		unsigned size = settings.size();
		
		settings.remove("audio.test");
		BOOST_CHECK(settings.size() == (size - 1));
		
		settings.remove("video");
		BOOST_CHECK(settings.size() == (size - 5));
	}
};

class SettingsTestSuite : public test_suite {
public:

	SettingsTestSuite() : test_suite("SettingsTestSuite") {
		boost::shared_ptr<SettingsTest> instance(new SettingsTest());

		test_case * testSettings = BOOST_CLASS_TEST_CASE(&SettingsTest::testSettings, instance);
		test_case * testAllKeys = BOOST_CLASS_TEST_CASE(&SettingsTest::testAllKeys, instance);

		add(testSettings);
		add(testAllKeys);
	}
};

#endif	//SETTINGSTEST_H
