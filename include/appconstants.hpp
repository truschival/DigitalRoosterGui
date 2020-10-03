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
#include <QStandardPaths>
#include <QString>
#include <chrono>

#include "config.h"

#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)

namespace DigitalRooster {
/**
 * configuration Key for Sources Array
 */
const QString KEY_GROUP_SOURCES("Sources");

/**
 * Internet Radio entries are found here
 */
const QString KEY_GROUP_IRADIO_SOURCES("InternetRadio");

/**
 * Podcast feeds Object keyword
 */
const QString KEY_GROUP_PODCAST_SOURCES("Podcasts");

/**
 * key for array of individual Podcast Episode objects
 */
const QString KEY_EPISODES("Episodes");

/**
 * Alarm Array Object keyword in JSON file
 */
const QString KEY_GROUP_ALARMS("Alarms");

/**
 * keyword for Alarm volume / stored volume
 */
const QString KEY_VOLUME("volume");

/**
 * property keyword for backlight brightness (active mode)
 */
const QString KEY_BRIGHTNESS_ACT("brightnessActive");

/**
 * property keyword for backlight brightness (standby mode)
 */
const QString KEY_BRIGHTNESS_SB("brightnessStandby");

/**
 * property keyword for backlight control mode
 */
const QString KEY_BACKLIGHT_CONTROL("autoBacklight");

/**
 *  property keyword for all ids
 */
const QString KEY_ID("id");

/**
 *  property key for all URIs
 */
const QString KEY_URI("url");

/**
 * property key for all nice names
 */
const QString KEY_NAME("name");

/**
 * property key boolean enabled true/false
 */
const QString KEY_ENABLED("enabled");

/**
 *  property keyword for alarms on weekends
 */
const QString JSON_KEY_TIME("time");
/**
 * Keyword for alarms on weekends
 */
const QString KEY_ALARM_PERIOD("period");

/**
 *  property value for alarms on weekends
 */
const QString KEY_ALARM_WEEKEND("weekend");

/**
 *  property value for alarms once
 */
const QString KEY_ALARM_ONCE("once");

/**
 * property value for alarms workdays
 */
const QString KEY_ALARM_WORKDAYS("workdays");

/**
 * property value for alarms daily
 */
const QString KEY_ALARM_DAILY("daily");

/**
 * property keyword for perdiodic refresh
 */
const QString KEY_UPDATE_INTERVAL("updateInterval");

/**
 * property keyword for alarms timeout
 */
const QString KEY_ALARM_TIMEOUT("alarmTimeout");

/**
 * property keyword configuration Sleep timer duration
 */
const QString KEY_SLEEP_TIMEOUT("sleepTimeout");

/**
 * property keyword to identify the creating project version of the config file
 */
const QString KEY_VERSION("version");

/**
 * Weather Object keyword retrieval configuration
 */
const QString KEY_WEATHER("Weather");

/**
 * property keyword Location ID for openweathermaps property
 */
const QString KEY_WEATHER_LOCATION_ID("locationId");

/**
 * API key to access openweathermaps
 */
const QString KEY_WEATHER_API_KEY("apiKey");

/**
 * key for any free text description property
 */
const QString KEY_DESCRIPTION("description");

/**
 * key for author/publisher property
 */
const QString KEY_PUBLISHER("publisher");

/**
 * key for publication date property
 */
const QString KEY_PUBLISHED("publication_date");

/**
 * key for any file path+name property
 */
const QString KEY_FILE_PATH("path");

/**
 * key for any image path property
 */
const QString KEY_ICON_URL("icon");

/**
 * key for any image path property
 */
const QString KEY_IMAGE_CACHE("icon-cached");

/**
 * key for numeric position property
 */
const QString KEY_POSITION("position");

/**
 * key for numeric duration property
 */
const QString KEY_DURATION("duration");

/**
 * key for numeric duration property
 */
const QString JSON_KEY_TITLE("title");

/**
 * key for timestamp property
 */
const QString KEY_TIMESTAMP("timestamp");

/**
 * key for wpa control interface socket
 */
const QString KEY_WPA_SOCKET_NAME("wpa_ctrl");

/**
 * key for wpa control interface socket
 */
const QString KEY_WIFI_DEV_NAME("net_dev");

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
 * Sample interval for ambient light sensor
 */
const std::chrono::milliseconds ALS_SAMPLING_PERIOD(300);

/**
 * Default output volume
 */
const int DEFAULT_VOLUME = 30;

/**
 * Default display brightness
 */
const int DEFAULT_BRIGHTNESS = 25;

/**
 * Constant Podcast Icon size
 * Shared between C++ and QML
 */
const int DEFAULT_ICON_WIDTH = 88;

/**
 * number of forecasts to fetch
 * 8*3h =  24h
 */
const int WEATHER_FORECAST_COUNT = 8;

/**
 * Where to find icons for weather condition
 */
const QString WEATHER_ICON_BASE_URL("https://openweathermap.org/img/w/");

/**
 * API Base URL for weather
 */
const QString WEATHER_API_BASE_URL(
    "https://api.openweathermap.org/data/2.5/weather?");

/**
 * API Base URL for Forecasts
 */
const QString WEATHER_FORECASTS_API_BASE_URL(
    "https://api.openweathermap.org/data/2.5/forecast?");

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

/**
 * WIFI interface
 */
const QString WIFI_NET_DEV_NAME(STR_VALUE(WIFI_INTERFACE_NAME));

/**
 * WPA supplicant Control socket
 */
const QString WPA_CONTROL_SOCKET_PATH(STR_VALUE(WPA_SOCK));


/**
 * Port for REST API
 */
const int REST_API_PORT(REST_API_LISTEN_PORT);

/****************************************************************************/

/**
 * Command line option for logfile
 * -l  --logfile
 */
const QString CMD_ARG_LOG_FILE("logfile");

/**
 * Command line option logging to stdout (takes precedence over --logfile)
 * -s --stdout
 */
const QString CMD_ARG_LOG_STDOUT("stdout");

/**
 * Command line option for cache directory of podcasts
 * -c --cachedir
 */
const QString CMD_ARG_CACHE_DIR("cachedir");

/**
 * Command line option for configuration file digitalrooster.json
 * -c --config
 */
const QString CMD_ARG_CONFIG_FILE("config");


/****************************************************************************/

/**
 * Log file path
 * initialized in main.cpp or test.cpp
 */
extern const QString DEFAULT_LOG_FILE;

/**
 * Default configuration file path
 * initialized in main.cpp or test.cpp
 */
extern const QString DEFAULT_CONFIG_FILE_PATH;

/**
 * Cache directory
 * initialized in main.cpp or test.cpp
 */
extern const QString DEFAULT_CACHE_DIR_PATH;

/****************************************************************************/
} // namespace DigitalRooster

#endif /* _APPCONSTANTS_HPP_ */
