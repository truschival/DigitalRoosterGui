/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/
#ifndef _CONFIGURATION_MANAGER_H_
#define _CONFIGURATION_MANAGER_H_

#include <QMap>
#include <QSettings>
#include <QString>
#include <QVector>
#include <chrono>
#include <memory>

#include "PlayableItem.hpp"
#include "PodcastSource.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"

namespace DigitalRooster {

/**
 * Read JSON configuration
 */
class ConfigurationManager {
public:
    /**
     * Default constructor with path to ini file
     * @param filepath
     */
    ConfigurationManager(
        const QString& filepath = DigitalRooster::SYSTEM_CONFIG_PATH);

    virtual ~ConfigurationManager() = default;

    /**
     * get all radio stream sources
     */
    const QVector<std::shared_ptr<PlayableItem>>& get_stream_sources() {
        return stream_sources;
    }

    /**
     * get all podcast sources
     */
    const QVector<std::shared_ptr<PodcastSource>>& get_podcast_sources() {
        return podcast_sources;
    }

    /**
     * get all radio stream sources
     */
    const QVector<std::shared_ptr<Alarm>>& get_alarms() {
        return alarms;
    }

    /**
     * Access configuration when Alarm should stop automatically
     * @return minutes
     */
    std::chrono::minutes get_alarm_timeout() const {
        return alarmtimeout;
    }

    /**
     * Append the radio stream to list - duplicates will not be checked
     * @param src the new stream source - we take ownership
     */
    void add_radio_station(std::unique_ptr<PlayableItem> src);

    /**
     * Write memory config to file - will overwrite changes in file
     */
    void write_config_file();

private:
    /**
     * Path for configuration file
     */
    QString filepath;
    /**
     * JSon Object containing the configuration including podcasts, Internet
     * radio etc read form digitalrooster.json
     */
    QJsonObject appconfig;

    /**
     * Internet radio stream souces are directly read form INI file
     */
    QVector<std::shared_ptr<PlayableItem>> stream_sources;

    /**
     * Podcast sources (only pretty name and feed-url)
     */
    QVector<std::shared_ptr<PodcastSource>> podcast_sources;

    /**
     * All Alarm objects
     */
    QVector<std::shared_ptr<Alarm>> alarms;

    /**
     * Duration for alarm to stop automatically
     */
    std::chrono::minutes alarmtimeout;

    /**
     * read Json file and fill sources
     */
    void readJson();
    /**
     * Fills the vector stream_sources with entries form settings file
     */
    void read_radio_streams_from_file();

    /**
     * Read all podcast sources form configuration file
     */
    void read_podcasts_from_file();

    /**
     * Read Alarm objects
     */
    void read_alarms_from_file();
};

/**
 * Map a Alarm period literal to enum value
 * @param literal "daily", "once", "weekend" or "workdays"
 * @return enum value
 */
Alarm::Period json_string_to_alarm_period(const QString& literal);

} // namespace DigitalRooster
#endif // _SETTINGS_READER_HPP_
