/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef _CONFIGURATION_MANAGER_H_
#define _CONFIGURATION_MANAGER_H_

#include <QFileSystemWatcher>
#include <QObject>
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
 * Simple POD for openweathermaps configuration
 * with sensible default values
 */
struct WeatherConfig {
    /* Base uri for OpenWeatherMaps API */
    QString base_uri{"http://api.openweathermap.org/data/2.5/weather?"};
    /**
     *  location id
     * from http://bulk.openweathermap.org/sample/city.list.json.gz
     * e.g. 'Esslingen,de' = id 2928751, Porto Alegre=3452925
     */
    QString cityid = {"2928751"};
    /** Openweathermap API Key */
    QString apikey = {"a904431b4e0eae431bcc1e075c761abb"};
    /** metric, imperial, */
    QString units = {"metric"};
    /* language for description 'en', 'de'...*/
    QString language = {"en"};
    /** Update Interval for wheather information */
    std::chrono::seconds update_interval{3600LL};
};

/**
 * Reads JSON configuration
 */
class ConfigurationManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString revision READ get_revision)
    Q_PROPERTY(QString buildtime READ get_build)
    Q_PROPERTY(int standbybrightness READ get_standby_brightness WRITE
            set_standby_brightness)
    Q_PROPERTY(int activebrightness READ get_active_brightness WRITE
            set_active_brightness)
    Q_PROPERTY(int defaultvolume READ get_volume WRITE set_volume)

public:
    /**
     * Default Constructor will use QT Standard paths for configuration
     */
    ConfigurationManager(
        const QString& configdir = QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation));

    virtual ~ConfigurationManager() = default;

    /**
     * return compile time version string
     */
    QString get_revision() const {
        return GIT_REVISION;
    }

    /**
     * return compile time
     */
    QString get_build() const {
        return PROJECT_BUILD_TIME;
    }

    /**
     * current linear volume
     * @return volume 0..100
     */
    int get_volume() const {
        return do_get_volume();
    }

    /**
     * User set and stored brightness for standby mode (form config file)
     * @return brightness
     */
    int get_standby_brightness() const {
        return do_get_brightness_sb();
    }

    /**
     * User set and stored brightness for standby mode (form config file)
     * @return brightness
     */
    int get_active_brightness() const {
        return do_get_brightness_act();
    }

    /**
     * get all radio stream sources
     */
    const QVector<std::shared_ptr<PlayableItem>>& get_stream_sources() {
        return get_iradio_list();
    }

    /**
     * get all podcast sources
     */
    const QVector<std::shared_ptr<PodcastSource>>& get_podcast_sources() {
        return get_podcast_list();
    }

    /**
     * Get a single podcast source identified by index
     * @throws 	 std::out_of_range if not found
     * @param index in vector
     * @return PodastSource
     */
    PodcastSource* get_podcast_source_by_index(int index) const;

    /**
     * Removes a podcast source entry form list
     * @throws 	 std::out_of_range if not found
     * @param index in vector
     */
    void remove_podcast_source_by_index(int index);

    /**
     * get all radio stream sources
     */
    const QVector<std::shared_ptr<Alarm>>& get_alarms() {
        return get_alarm_list();
    }

    /**
     * Weather configuration object
     */
    const WeatherConfig& get_weather_config() {
        return get_weather_cfg();
    }

    /**
     * Access configuration when Alarm should stop automatically
     * @return default alarm timeout
     */
    std::chrono::minutes get_alarm_timeout() const {
        return global_alarmtimeout;
    }

    /**
     * Path to wpa_supplicant control socket
     * @return "/var/wpa_supplicant/wlan0"
     */
    virtual QString get_wpa_socket_name() const;

    /**
     * Read full configuration file path
     * @return path to configuration file
     */
    QString get_configuration_path() const;
    /**
     * Append the radio stream to list - duplicates will not be checked
     * @param src the new stream source - we take ownership
     */
    void add_radio_station(std::shared_ptr<PlayableItem> src);

    /**
     * Append new alarm to list
     * @param alarm
     */
    void add_alarm(std::shared_ptr<Alarm> alarm);

    /**
     * Delete an alarm identified by ID from the list of alarms
     * @param id of alarm
     * @return 0 if alarm was deleted, -1 otherwise
     */
    int delete_alarm(const QUuid& id);

public slots:
    /**
     * Any Item (Alarm, PodcastSource...) changed
     * Trigger timer for writing configuration file
     */
    void dataChanged();

    /**
     * volume settings changed -> store
     * @param vol new volume settings (0..100)
     */
    void set_volume(int vol);

    /**
     * user changed standby brightness
     * @param brightness new volume settings (0..100)
     */
    void set_standby_brightness(int brightness);

    /**
     * user changed standby brightness
     * @param brightness new volume settings (0..100)
     */
    void set_active_brightness(int brightness);

    /**
     * Write memory config to file - will overwrite changes in file
     */
    void store_current_config();

    /**
     * The monitored file has changed -
     * adapter function since QFilesytemwatcher
     * emits fileChanged(const QString &path)
     */
    void fileChanged(const QString& path);

    void update_configuration() {
        refresh_configuration();
    }

signals:
    void configuration_changed();

private:
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
     * Weather configuration
     */
    WeatherConfig weather_cfg;

    /**
     * Duration for alarm to stop automatically
     */
    std::chrono::minutes global_alarmtimeout;

    /**
     * Stop playback automatically (globally)
     */
    std::chrono::minutes sleeptimeout;

    /**
     * Linear Volume in percent (stored in config file)
     */
    int volume;

    /**
     * display brightness (0..100%) in standby mode
     */
    int brightness_sb;
    /**
     * display brightness (0..100%) in active mode
     */
    int brightness_act;

    /**
     * File system monitor to get updated if someone changed the file
     */
    QFileSystemWatcher filewatcher;
    /**
     * file changed connection stored to disconnect & reconnect
     */
    QMetaObject::Connection fwConn;

    /**
     * Configuration directory, writable, created if it doesn't exist
     */
    QDir config_dir;

    /**
     * Timer tor write configuration to disk
     */
    QTimer writeTimer;

    /**
     * WPA control socket path /var/lib/wpa_supplicant/wlan0
     */
    QString wpa_socket_name;

    /**
     * Check if config directory exist, otherwise create it
     * @return directory that exist and is writable
     */

    QDir make_sure_config_path_exists() const;

    /**
     * Check if config and path exist, otherwise create default config file at
     * that location
     * @return full file path to configuration file
     */
    QString check_and_create_config();

    /**
     * Create "sensible" default entries for podcasts, alarms and internet radio
     * and store to default file
     */
    void create_default_configuration();

    /**
     * read file and return content as string
     */
    virtual QString get_json_from_file(const QString& path);

    /**
     * interpret json string
     */
    virtual void parse_json(const QByteArray& json);

    /**
     * Fills the vector stream_sources with entries form settings file
     */
    virtual void read_radio_streams_from_file(const QJsonObject& appconfig);

    /**
     * Read all podcast sources form configuration file
     */
    virtual void read_podcasts_from_file(const QJsonObject& appconfig);

    /**
     * Read Alarm objects
     */
    virtual void read_alarms_from_file(const QJsonObject& appconfig);

    /**
     * Read weatherconfig
     */
    virtual void read_weather_from_file(const QJsonObject& appconfig);

    /**
     * Store settings permanently to file
     */
    void write_config_file(const QJsonObject& appconfig);

    /**
     * Update all configuration items
     */
    void refresh_configuration();

    /**
     * get all radio stream sources
     */
    virtual QVector<std::shared_ptr<PlayableItem>>& get_iradio_list() {
        return stream_sources;
    }

    /**
     * get all podcast sources
     */
    virtual QVector<std::shared_ptr<PodcastSource>>& get_podcast_list() {
        return podcast_sources;
    }

    /**
     * get all radio stream sources
     */
    virtual QVector<std::shared_ptr<Alarm>>& get_alarm_list() {
        return alarms;
    }

    /**
     * Weather configuration object
     */
    virtual WeatherConfig& get_weather_cfg() {
        return weather_cfg;
    }

    /**
     * Private virtual interface for brightness settings
     */
    virtual int do_get_brightness_sb() const {
        return brightness_sb;
    }

    virtual int do_get_brightness_act() const {
        return brightness_act;
    }

    /**
     * Private virtual interface for volume settings
     */
    virtual int do_get_volume() const {
        return volume;
    };
};
} // namespace DigitalRooster
#endif // _SETTINGS_READER_HPP_
