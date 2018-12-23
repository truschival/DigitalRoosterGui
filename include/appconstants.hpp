/******************************************************************************
 * \filename
 * \brief  Application global compile time configurations
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef _APPCONSTANTS_HPP_
#define _APPCONSTANTS_HPP_

#include <QDir>
#include <QString>
#include <chrono>

#include "config.h"

#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)

namespace DigitalRooster {

/**
 * INI configuration Key for Sources
 */
const QString KEY_GROUP_SOURCES("Sources");

/**
 * Internet Radio entries are found here
 */
const QString KEY_GROUP_IRADIO_SOURCES("InternetRadio");

/**
 * Podcast feeds
 */
const QString KEY_GROUP_PODCAST_SOURCES("Podcasts");

/**
 * Alarm Array in JSON file
 */
const QString KEY_GROUP_ALARMS("Alarms");

/**
 * keyword for Alarm volume / stored volume
 */
const QString KEY_VOLUME("volume");

/**
 * keyword for backlight brightness (active mode)
 */
const QString KEY_BRIGHTNESS_ACT("brightnessActive");

/**
 * keyword for backlight brightness (standby mode)
 */
const QString KEY_BRIGHTNESS_SB("brightnessStandby");

/**
 * Keyword for all ids
 */
const QString KEY_ID("id");

/**
 * Key for all URIs
 */
const QString KEY_URI("uri");

/**
 * Key for all nice names
 */
const QString KEY_NAME("name");

/**
 * Key boolean enabled true/false
 */
const QString KEY_ENABLED("enabled");

/**
 * Keyword for alarms on weekends
 */
const QString KEY_TIME("time");
/**
 * Keyword for alarms on weekends
 */
const QString KEY_ALARM_PERIOD("period");

/**
 * Keyword for alarms on weekends
 */
const QString KEY_ALARM_WEEKEND("weekend");

/**
 * Keyword for alarms once
 */
const QString KEY_ALARM_ONCE("once");

/**
 * Keyword for alarms workdays
 */
const QString KEY_ALARM_WORKDAYS("workdays");

/**
 * Keyword for alarms daily
 */
const QString KEY_ALARM_DAILY("daily");

/**
 * Keyword for perdiodic refresh
 */
const QString KEY_UPDATE_INTERVAL("updateInterval");

/**
 * Keyword for alarms timeout
 */
const QString KEY_ALARM_TIMEOUT("alarmTimeout");

/**
 * Keyword configuration Sleep timer duration
 */
const QString KEY_SLEEP_TIMEOUT("sleepTimeout");

/**
 * Keyword to identify the creating project version of the config file
 */
const QString KEY_VERSION("version");

/**
 * Weather retrieval configuration
 */
const QString KEY_WEATHER("Weather");

/**
 * Location ID for openweathermaps
 */
const QString KEY_WEATHER_LOCATION_ID("locationID");

/**
 * API key to access openweathermaps
 */
const QString KEY_WEATHER_API_KEY("API-Key");


/**
 * Directory for all downloaded RSS Files
 */
const QString RSS_FILE_DIR(QDir::tempPath());

/**
 * Default alarm volume
 */
const int DEFAULT_ALARM_VOLUME = 30;

/**
 * Time after which an alarm should stop playing
 * (probably because nobody is in to switch it off manually)
 */
const std::chrono::minutes DEFAULT_ALARM_TIMEOUT(30);

/**
 * Time after which media should stop playing
 * (probably because I am already asleep)
 */
const std::chrono::minutes DEFAULT_SLEEP_TIMEOUT(60);

/**
 * Default output volume
 */
const int DEFAULT_VOLUME = 30;

/**
 * Default display brightness
 */
const int DEFAULT_BRIGHTNESS = 25;

/*****************************************************************************
 CMake build configurations from config.h
 *****************************************************************************/
/**
 * Build directory (where testfiles are generated)
 */
const QString TEST_FILE_PATH(STR_VALUE(CMAKE_BUILD_DIR));
/**
 * Application Name
 */
const QString APPLICATION_NAME(STR_VALUE(CMAKE_PROJECT_NAME));

/**
 * Project release version for configuration compatibility etc.
 */
const QString PROJECT_VERSION(STR_VALUE(CMAKE_PROJECT_VERSION));

/**
 * Git revision as QString
 */
const QString GIT_REVISION(STR_VALUE(GIT_DESCRIBE_REV));

/**
 * Project build time stamp
 */
const QString PROJECT_BUILD_TIME(STR_VALUE(CMAKE_BUILD_TIME_STAMP));

/**
 * Json file name for configuration
 */
const QString CONFIG_JSON_FILE_NAME(STR_VALUE(CMAKE_SETTINGS_FILE_NAME));

} // namespace DigitalRooster

#endif /* _APPCONSTANTS_HPP_ */
