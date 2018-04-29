/******************************************************************************
 * \filename
 * \brief  Application global compile time configurations
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/
#pragma once

#include "config.h"
#include <QDir>
#include <QString>
#include <chrono>

namespace DigitalRooster {
/**
 * Name of configuration file
 */
const QString SYSTEM_CONFIG_PATH(
    QString(SETTINGS_PATH) + "/" + QString(SETTINGS_FILE_NAME));

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
 * Optional keyword for Alarm volume
 */
const QString KEY_VOLUME("volume");
/**
 * Default alarm volume
 */
const int default_alarm_volume = 30;

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
const QString KEY_UPDATE_INTERVAL("UpdateInterval");

/**
 * Keyword for alarms timeout
 */
const QString KEY_ALARM_TIMEOUT("AlarmTimeout");
/**
 * Default Alarm timeout
 */
const std::chrono::minutes default_alarm_timeout(30);

/**
 * Keyword configuration Sleep timer duration
 */
const QString KEY_SLEEP_TIMEOUT("SleepTimeout");

/**
 * Directory for all downloaded RSS Files
 */
const QString RSS_FILE_DIR(QDir::tempPath());


/**
 * Build directory (where testfiles are generated)
 */
const QString TEST_FILE_PATH(BUILD_DIR);


/*****************************************************************************/


/**
 * Runtime configuration object
 */
class AppConfig {
public:
    std::string mediaFile;
};

} // namespace DigitalRooster
