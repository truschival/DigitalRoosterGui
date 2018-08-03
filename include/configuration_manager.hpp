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

#include <QMap>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QFileSystemWatcher>

#include <chrono>
#include <memory>

#include "PlayableItem.hpp"
#include "PodcastSource.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"
#include "weather.hpp"

namespace DigitalRooster {

/**
 * Read JSON configuration
 */
class ConfigurationManager: public QObject {
Q_OBJECT
public:
	/**
	 * Default Constructor will use QT Standard paths for configuration
	 */
	ConfigurationManager();

	virtual ~ConfigurationManager() = default;

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
	 * get all radio stream sources
	 */
	const QVector<std::shared_ptr<Alarm>>& get_alarms() {
		return get_alarm_list();
	}

	/**
	 * Weather configuration object
	 */
    const WeatherConfig& get_weather_config(){
        return weather_cfg;
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
	void add_radio_station(std::shared_ptr<PlayableItem> src);

	/**
	 * Append new alarm to list
	 * @param alarm
	 */
	void add_alarm(std::shared_ptr<Alarm> alarm);

	/**
	 * Write memory config to file - will overwrite changes in file
	 */
	void store_current_config();

public slots:
	/**
	 * The monitored file has changed -
	 * adapter function since QFilesytemwatcher
	 * emits fileChanged(const QString &path)
	 */
	void fileChanged(const QString &path);

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
	std::chrono::minutes alarmtimeout;

	/**
	 * Stop playback automatically (globally)
	 */
	std::chrono::minutes sleeptimeout;

	/**
	 * File system monitor to get updated if someone changed the file
	 */
	QFileSystemWatcher filewatcher;

	/**
	 * Check if config path exist, otherwise create it
	 * @return directory that exist and is writable
	 */

	virtual QDir make_sure_config_path_exists();

	/**
	 * Check if config and path exist, otherwise create default config file at that location
	 * @return full file path to configuration file
	 */
	virtual QString check_and_create_config();

	/**
	 * Create "sensible" default entries for podcasts, alarms and internet radio
	 * and store to default file 
	 */
	virtual void create_default_configuration();

	/**
	 * read file and return content as string
	 */
	virtual QString getJsonFromFile(const QString& path);

	/**
	 * interpret json string
	 */
	virtual void parseJson(const QByteArray& json);

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
	virtual void write_config_file(const QJsonObject& appconfig);

	/**
	 * Update all configuration items
	 */
	virtual void refresh_configuration();

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
};
} // namespace DigitalRooster
#endif // _SETTINGS_READER_HPP_
