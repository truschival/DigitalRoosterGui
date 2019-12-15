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
#include "weather.hpp"

namespace DigitalRooster {


/**
 * Reads JSON configuration
 */
class ConfigurationManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString revision READ get_revision CONSTANT)
    Q_PROPERTY(QString buildtime READ get_build CONSTANT)
    Q_PROPERTY(int standbybrightness READ get_standby_brightness WRITE
            set_standby_brightness)
    Q_PROPERTY(int activebrightness READ get_active_brightness WRITE
            set_active_brightness)
    Q_PROPERTY(int defaultvolume READ get_volume WRITE set_volume)

public:
    /**
     * Default constructor will use QT Standard paths for configuration
     * @param configpath path to application configuration
     * @param cachedir directory to cache data (podcastlist etc)
     */
    ConfigurationManager(const QString& configpath, const QString& cachedir);

    virtual ~ConfigurationManager() = default;

    /**
     * return compile time version string
     */
    QString get_revision() const {
        if (GIT_REVISION.isEmpty()) {
            return PROJECT_VERSION;
        }
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
     * Get a internet radio station identified by ID
     * @throws 	 std::out_of_range if not found
     * @param id unique ID of podcast
     * @return station
     */
    const PlayableItem* get_stream_source(const QUuid& id) const;

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
     * Get a single podcast source identified by ID
     * @throws 	 std::out_of_range if not found
     * @param id unique ID of podcast
     * @return source
     */
    const PodcastSource* get_podcast_source(const QUuid& id) const;

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
     * Get a alarm identified by ID
     * @throws 	 std::out_of_range if not found
     * @param id unique ID of podcast
     * @return station
     */
    const Alarm* get_alarm(const QUuid& id) const;

    /**
     * Weather configuration object
     */
    const WeatherConfig* get_weather_config() {
        return get_weather_cfg();
    }

    /**
     * Access configuration when Alarm should stop automatically
     * @return default alarm timeout
     */
    virtual std::chrono::minutes get_alarm_timeout() const {
        return global_alarm_timeout;
    }

    /**
     * Minutes after which DigitalRooster goes in standby
     * @return \ref sleep_timeout
     */
    virtual std::chrono::minutes get_sleep_timeout() const;

    /**
     * Update sleep timeout Minutes after which DigitalRooster goes in standby
     * @param timeout \ref sleep_timeout
     */
    void set_sleep_timeout(std::chrono::minutes timeout);

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
     * Where to store cache files
     * @return application_cache_dir.dirName()
     */
    QString get_cache_path() {
        return get_cache_dir_name();
    };

    /**
     * Append the radio stream to list - duplicates will not be checked
     * @param src the new stream source - we take ownership
     */
    void add_radio_station(std::shared_ptr<PlayableItem> src);

    /**
     * Append new PodcastSource to list
     * @param podcast source
     */
    void add_podcast_source(std::shared_ptr<PodcastSource> podcast);

    /**
     * Append new alarm to list
     * @param alarm
     */
    void add_alarm(std::shared_ptr<Alarm> alarm);

    /**
     * Delete an alarm identified by ID from the list of alarms
     * @param id of alarm
     * @throws 	 std::out_of_range if not found
     */
    void delete_alarm(const QUuid& id);

    /**
     * Delete a internet radio station identified by id form the list
     * @param id unique id of radio station
     * @throws 	 std::out_of_range if not found
     */
    void delete_radio_station(const QUuid& id);

    /**
     * Delete a podcast source identified by id form the list of sources
     * @param id unique id of podcast source
     * @throws 	 std::out_of_range if not found
     */
    void delete_podcast_source(const QUuid& id);

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
    /**
     * Any configuration item changed
     */
    void configuration_changed();
    /**
     * podcast list was changed (added/deleted items)
     */
    void podcast_sources_changed();
    /**
     * alarm list was changed (added/deleted items)
     */
    void alarms_changed();
    /**
     * radio list was changed (added/deleted items)
     */
    void stations_changed();

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
     * Duration for alarm to stop automatically
     */
    std::chrono::minutes global_alarm_timeout;

    /**
     * Stop playback automatically (globally)
     */
    std::chrono::minutes sleep_timeout;

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
     * Weather configuration
     */
    std::unique_ptr<WeatherConfig> weather_cfg;

    /**
     * Configuration directory, writable, created if it doesn't exist
     */
    QString config_file;

    /**
     * Directory for storing cache files
     */
    QDir application_cache_dir;

    /**
     * Timer tor write configuration to disk
     */
    QTimer writeTimer;

    /**
     * WPA control socket path /var/lib/wpa_supplicant/wlan0
     */
    QString wpa_socket_name;

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
     * Application wide cache dir, set by command line parameter
     * defaults to
     * QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
     * @return application_cache_dir.dirName()
     */
    virtual QString get_cache_dir_name();

    /**
     * interpret json string
     */
    virtual void parse_json(const QByteArray& json);

    /**
     * Fills the vector stream_sources
     */
    virtual void read_radio_streams(const QJsonObject& appconfig);

    /**
     * Read all podcast sources form configuration file
     */
    virtual void read_podcasts(const QJsonObject& appconfig);

    /**
     * Read Alarm objects
     */
    virtual void read_alarms(const QJsonObject& appconfig);

    /**
     * Read weatherconfig
     */
    virtual void read_weather(const QJsonObject& appconfig);

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
    virtual const WeatherConfig* get_weather_cfg() {
        return weather_cfg.get();
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
     * actually set active brightness
     * @param brightness - new actual brightness
     */
    virtual void do_set_brightness_act(int brightness);

    /**
     * Private virtual interface for volume settings
     */
    virtual int do_get_volume() const {
        return volume;
    };
};

/**
 * Check if path is a directory and is writable
 * @param dirname
 * @return true for success, false otherwise
 */
bool is_writable_directory(const QString& dirname);

/**
 * try to create a directory for the files
 * @param dirname
 * @return true for success, false otherwise
 */
bool create_writable_directory(const QString& dirname);


} // namespace DigitalRooster
#endif // _SETTINGS_READER_HPP_
