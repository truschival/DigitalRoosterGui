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

#include <QObject>
#include <QString>
#include <QUrl>
#include <QJsonObject>
#include <QTimer>
#include <QtNetwork>
#include <chrono>

namespace DigitalRooster {

class ConfigurationManager;

/**
 * Simple POD for openweathermaps configuration
 * with sensible default values
 */
struct WeatherConfig {
    /**
     *  location id
     * from http://bulk.openweathermap.org/sample/city.list.json.gz
     * e.g. 'Esslingen,de' = id 4891, Porto Alegre=3452925
     */
    QString cityid = {"4891"};
    /** Openweathermap API Key */
    QString apikey = {"a904431b4e0eae431bcc1e075c761abb"};
    /** metric, imperial, */
    QString units = {"metric"};
    /* language for description 'en', 'de'...*/
    QString language = {"en"};
};


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

    /**
     * Current download interval
     * @return interval in seconds
     */
    std::chrono::seconds get_interval();

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
     * Read Json from content bytearray and update internal fields
     * @oaram content  JSON as bytearray
     */
    void parse_response(QByteArray& content);

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
     * Interval in ms for auto refresh of content
     * default to 1h
     */
    std::chrono::seconds update_interval;  

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
};
} // namespace DigitalRooster
#endif /* _WEATHER_HPP_ */