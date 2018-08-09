/******************************************************************************
 * \filename
 * \brief Download weather information form openweathermaps
 *
 * \details Periodically polls weather info
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _WEATHER_HPP_
#define _WEATHER_HPP_

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QtNetwork>
#include <chrono>

#include "DownloadManager.hpp"

namespace DigitalRooster {

class ConfigurationManager;
struct WeatherConfig;

/**
 * Periodically downloads weather info from Openweathermaps
 */
class Weather : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString city READ get_city)
    Q_PROPERTY(
        float temperature READ get_temperature NOTIFY temperature_changed)
    Q_PROPERTY(QUrl weatherIcon READ get_weather_icon_url NOTIFY icon_changed)

public:
    /**
     * Constructor for Weather provider
     * @param confman configuration
     * @param parent
     */
    Weather(std::shared_ptr<ConfigurationManager> confman,
        QObject* parent = nullptr);
    /**
     * Update Download interval
     * @param iv interval in seconds
     */
    void set_update_interval(std::chrono::seconds iv);
	std::chrono::seconds get_update_interval() const;
		
    /**
     * Access to temperature
     */
    float get_temperature() const {
        return temperature;
    }

    /**
     * Access to city name
     */
    QString get_city() const {
        return city_name;
    }

    /**
     * Construct a URL from icon_id
     */
    QUrl get_weather_icon_url() const {
        const QString base_uri("http://openweathermap.org/img/w/");
        return QUrl(base_uri + icon_id + ".png");
    }

public slots:

	/**
	 * Slot triggers refresh of weather data and starts a new download process
	 */
	void refresh();
    /**
     * Read Json from content bytearray and update internal fields
     * @oaram content  JSON as bytearray
     */
    void parse_response(QByteArray content);

signals:
    /**
     * Provide a full info as received from Openweathermaps
     */
    void weather_info_available(const QJsonObject& current);

    /**
     * Current Condition description
     * e.g. "light clouds", "light intensity drizzle"
     */
    void condition_changed(const QString& temp);

    /**
     * Current temperature
     */
    void temperature_changed(const float temperature);

    /**
     * Weather icon corresponding to current condition
     */
    void icon_changed(const QUrl& img_uri);

private:
    /**
     * Central configuration and data handler
     */
    std::shared_ptr<ConfigurationManager> cm;

    /**
     * Interval in ms for auto refresh of content
     * default to 1h
     */
    std::chrono::seconds update_interval{3600LL};

    /**
     * Current Temperature
     */
    float temperature = 0.0;

    /**
     * Name of location matching the ID, retured by weather-api
     */
    QString city_name;

    /**
     * Icon matching the current weather condtion returned by weather-api
     */
    QString icon_id;

    /**
     * QTimer for periodic updates
     */
    QTimer timer;
    
	/**
	 * HTTP handle to download JSONs
 	 */
    DownloadManager downloader;
};

/**
 * create a valid URL to download weather-json from openweathermaps
 * based on infromation in WeatherConfig
 * @param cfg configuration with location, units etc.
 * @return uri e.g. api.openweathermap.org/data/2.5/weather?zip=94040,us
 */
QUrl create_weather_uri(const WeatherConfig& cfg);


} // namespace DigitalRooster
#endif /* _WEATHER_HPP_ */