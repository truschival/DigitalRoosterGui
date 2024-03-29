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
#include <QTimer>

#include <atomic>
#include <chrono>
#include <memory>
#include <vector>

#include "appconstants.hpp"
/* Implemented Interfaces */
#include "IAlarmStore.hpp"
#include "IBrightnessStore.hpp"
#include "IPodcastStore.hpp"
#include "IStationStore.hpp"
#include "ITimeoutStore.hpp"
#include "IWeatherConfigStore.hpp"

namespace DigitalRooster {
// forward decl
class PlayableItem;
class PodcastSource;
class Alarm;

/**
 * Reads JSON configuration and provides API to configuration objects
 */
class Configuration : public QObject,
                             public IAlarmStore,
                             public IStationStore,
                             public IPodcastStore,
                             public ITimeOutStore,
                             public IBrightnessStore,
                             public IWeatherConfigStore {
    Q_OBJECT
    Q_PROPERTY(QString revision READ get_revision CONSTANT)
    Q_PROPERTY(QString buildtime READ get_build CONSTANT)
    Q_PROPERTY(int standbybrightness READ get_standby_brightness WRITE
            set_standby_brightness)
    Q_PROPERTY(int activebrightness READ get_active_brightness WRITE
            set_active_brightness)
    Q_PROPERTY(double defaultvolume READ get_volume WRITE set_volume)

public:
    /**
     * Default constructor will use QT Standard paths for configuration
     * @param configpath path to application configuration
     * @param cachedir directory to cache data (podcastlist etc)
     */
    Configuration(const QString& configpath, const QString& cachedir);

    virtual ~Configuration() = default;

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
    double get_volume() const {
        return do_get_volume();
    }

    /*
     * Implementation of IAlarmStore
     */
    void add_alarm(std::shared_ptr<Alarm> alarm) override;
    void delete_alarm(const QUuid& id) override;
    const Alarm* get_alarm(const QUuid& id) const override;
    const std::vector<std::shared_ptr<Alarm>>& get_alarms() const override;

    /*
     * Implementation of IStationStore
     */
    virtual void add_radio_station(std::shared_ptr<PlayableItem> src) override;
    virtual void delete_radio_station(const QUuid& id) override;
    const PlayableItem* get_station(const QUuid& id) const override;
    virtual const std::vector<std::shared_ptr<PlayableItem>>&
    get_stations() const override;

    /*
     * Implementation of IPodcastStore
     */
    virtual void add_podcast_source(
        std::shared_ptr<PodcastSource> podcast) override;
    virtual void delete_podcast_source(const QUuid& id) override;
    virtual const PodcastSource* get_podcast_source(
        const QUuid& id) const override;
    virtual const std::vector<std::shared_ptr<PodcastSource>>&
    get_podcast_sources() const override;
    virtual PodcastSource* get_podcast_source_by_index(
        int index) const override;
    virtual void remove_podcast_source_by_index(int index) override;

    /*
     * Implementation of ITimeoutStore
     */
    virtual std::chrono::minutes get_alarm_timeout() const override;
    virtual std::chrono::minutes get_sleep_timeout() const override;
    virtual void set_sleep_timeout(std::chrono::minutes timeout) override;

    /*
     * Implementation of IBrightnessStore
     */
    virtual int get_standby_brightness() const override;
    virtual int get_active_brightness() const override;

    /*
     * Implementation of IWeatherConfigStore
     */
    virtual const WeatherConfig& get_weather_config() const override;

    /**
     * Path to wpa_supplicant control socket
     * @return "/var/wpa_supplicant/wlan0"
     */
    virtual QString get_wpa_socket_name() const;

    /**
     * Network interface name
     * @return "wlan0"
     */
    virtual QString get_net_dev_name() const;

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
    void set_volume(double vol);

    /**
     * user changed standby brightness
     * @param brightness new volume settings (0..100)
     */
    void set_standby_brightness(int brightness) override;

    /**
     * user changed standby brightness
     * @param brightness new volume settings (0..100)
     */
    void set_active_brightness(int brightness) override;

    /**
     * Enable/disable manual brightness control
     * @return true if auto control is enabled
     */
    virtual void enable_backlight_control(bool enable) override;
    virtual bool backlight_control_enabled() const override;

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
    std::vector<std::shared_ptr<PlayableItem>> stream_sources;

    /**
     * Podcast sources (only pretty name and feed-url)
     */
    std::vector<std::shared_ptr<PodcastSource>> podcast_sources;

    /**
     * All Alarm objects
     */
    std::vector<std::shared_ptr<Alarm>> alarms;

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
    double volume;

    /**
     * display brightness (0..100%) in standby mode
     */
    int brightness_sb;
    /**
     * display brightness (0..100%) in active mode
     */
    int brightness_act;
    /**
     * Backlight control in auto mode
     */
    bool backlight_control_act;

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
    WeatherConfig weather_cfg;

    /**
     * Configuration directory, writable, created if it doesn't exist
     */
    QString config_file;

    /**
     * Directory for storing cache files
     */
    QDir application_cache_dir;

    /**
     * Timer Id for writing data
     * assigned by \ref QObject::startTimer()
     * started in ctor
     */
    int evt_timer_id = -1;

    /**
     * Write operations set this flag. event timer expires
     * configuration is written to disk
     */
    std::atomic<bool> dirty{false};

    /**
     * WPA control socket path /var/lib/wpa_supplicant/wlan0
     */
    QString wpa_socket_name;

    /**
     * Wifi Network device name
     */
    QString net_dev_name;

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
     * actually set active brightness
     * @param brightness - new actual brightness
     */
    virtual void do_set_brightness_act(int brightness);

    /**
     * Private virtual interface for volume settings
     */
    virtual double do_get_volume() const {
        return volume;
    };

    /**
     * React to QObject Timer events
     * check if config needs to be written to disk
     * @param evt timer event
     */
    virtual void timerEvent(QTimerEvent* evt) override;

private slots:
    /**
     * catch all slot if any data of an alarm has changed
     */
    void alarm_data_changed();
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
