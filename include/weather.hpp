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
#include <httpclient.hpp>


namespace DigitalRooster {

class ConfigurationManager;
/**
 * Configuration of weather source as found in application config file
 * read by ConfigurationManager \ref ConfigurationManager::get_weather_cfg
 */
class WeatherConfig {
public:
    /**
     * Constructor with 'relevant' information
     * @param token
     * @param cityid
     * @param interval 1hour
     */
    WeatherConfig(
        const QString& token =QString() ,
        const QString& cityid =QString() ,
        const std::chrono::seconds& interval = std::chrono::seconds(3600));

    /**
     * create Weatherconfig form Json configuration
     * @param json jobject
     * @return
     */
    static std::unique_ptr<WeatherConfig> from_json_object(const QJsonObject& json);
    /**
     * Serialize as JSON Object - only contains information that is not
     * updated through RSS feed and cached.
     * @return QJsonObject
     */
    QJsonObject to_json_object() const;

    /**
     * Openweather City id / location id
     * @return \ref location_id
     */
    const QString& get_location_id() const {
        return location_id;
    }
    /**
     * 'Secret' api-token for openweather api
     * @return token string
     */
    const QString& get_api_token() const{
        return api_token;
    }
    /**
     * update interval to poll openweather api
     * @return seconds
     */
    const std::chrono::seconds get_update_interval() {
        return update_interval;
    }
private:
    /** Openweathermap API Key */
    QString api_token;
    /**
     *  location id
     * from http://bulk.openweathermap.org/sample/city.list.json.gz
     * e.g. 'Esslingen,de' = id 2928751, Porto Alegre=3452925
     */
    QString location_id;
    /** Update Interval for wheather information */
    std::chrono::seconds update_interval;
};


/**
 * Periodically downloads weather info from Openweathermaps
 */
class Weather : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString city READ get_city NOTIFY city_updated)
    Q_PROPERTY(QString condition READ get_condition NOTIFY condition_changed)
    Q_PROPERTY(
        float temperature READ get_temperature NOTIFY temperature_changed)
    Q_PROPERTY(QUrl weatherIcon READ get_weather_icon_url NOTIFY icon_changed)

public:
    /**
     * Constructor for Weather provider
     * @param confman configuration
     * @param parent
     */
    explicit Weather(std::shared_ptr<ConfigurationManager> confman,
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
    double get_temperature() const {
        return temperature;
    }

    /**
     * Current wheather condition description (localized string)
     * e.g. "light clouds", "light intensity drizzle"
     */
    QString get_condition() const {
        return condition;
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
     * @param content  JSON as bytearray
     */
    void parse_response(QByteArray content);

signals:
    /**
     * Flag new weather information received
     */
    void weather_info_updated();

    /**
     * Current Condition description
     */
    void condition_changed(const QString& temp);

    /**
     * City name available
     */
    void city_updated(const QString& cityname);

    /**
     * Current temperature
     */
    void temperature_changed(const double temperature);

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
    double temperature = 0.0;

    /**
     * Name of location matching the ID, retured by weather-api
     */
    QString city_name;

    /**
     * Localized weathercondition string
     */
    QString condition;

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
    HttpClient downloader;


    void parse_city(const QJsonObject& o);
    void parse_temperature(const QJsonObject& o);
    void parse_condition(const QJsonObject& o);
};

/**
 * create a valid URL to download weather-json from openweathermaps
 * based on infromation in WeatherConfig
 * @param cfg configuration with location, units etc.
 * @return uri e.g. api.openweathermap.org/data/2.5/weather?zip=94040,us
 */
QUrl create_weather_uri(const WeatherConfig* cfg);


} // namespace DigitalRooster
#endif /* _WEATHER_HPP_ */
