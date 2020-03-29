/******************************************************************************
 * \filename
 * \brief   Download weather information form openweathermaps
 *
 * \details Periodically polls weather info
 *
 * \copyright (c) 2019  Thomas Ruschival <thomas@ruschival.de>
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

#include <mutex>

#include "IWeatherConfigStore.hpp"

namespace DigitalRooster {

class Weather;
class ConfigurationManager;

/**
 * Object for Forecast data
 * Still a QObject, would be nice if I figure out how to pass a list
 * of Forecasts to QML (and keep the ressources managed and thread-safe)
 */
class WeatherStatus : public QObject {
    Q_OBJECT
    Q_PROPERTY(QDateTime timestamp READ get_timestamp)
    Q_PROPERTY(double temp READ get_temperature)
    Q_PROPERTY(double temp_min READ get_max_temperature)
    Q_PROPERTY(double temp_max READ get_min_temperature)
    Q_PROPERTY(QUrl icon_url READ get_weather_icon_url)
public:
    WeatherStatus() = default;

    /**
     * Cloning constructor - Q_OBJECT does not allow copy construction
     * @param other where to take the fields from
     */
    explicit WeatherStatus(const WeatherStatus* other)
        : temperature(other->temperature)
        , temp_min(other->temp_min)
        , temp_max(other->temp_max)
        , icon_url(other->icon_url)
        , timestamp(other->timestamp) {
    }

    /**
     * Update status with data from JSON object
     * @return true if update worked, false otherwise
     * @param json a 'weather' object:
     *     {
     *     "dt": 1584792000,
     *     "main": {
     *        "temp": 24.78,
     *        "temp_min": 24.78,
     *        "temp_max": 26.25,
     *      },
     *     "weather": [
     *          {
     *          "id": 500,
     *          "icon": "10n"
     *          }
     *        ]
     *     }
     */
    bool update(const QJsonObject& json);

    QDateTime get_timestamp() const {
        return timestamp;
    };

    QUrl get_weather_icon_url() const {
        return icon_url;
    };

    double get_temperature() const {
        return temperature;
    };

    double get_min_temperature() const {
        return temp_min;
    };

    double get_max_temperature() const {
        return temp_max;
    }
    /**
     * Basic validation of json from API will throw if a elementary object e.g.
     * 'main' or 'weather' is missing
     * @param json
     */
    static void validate_json(const QJsonObject& json);

private:
    /**
     * temperature at timepoint
     */
    double temperature = 0.0;
    /**
     * measured/expected temperature at timepoint
     */
    double temp_min = 0.0;
    /**
     * maximum measured/expected temperature at timepoint
     */
    double temp_max = 0.0;

    /**
     * Weather Icon URL
     */
    QUrl icon_url;

    /**
     * Timepoint for this forecast
     */
    QDateTime timestamp;

    /**
     * update min/max current temperature
     * @param json object that contains a "main" object of forecast or current
     * weather: "main": { "temp": 16, "pressure": 1018, "humidity": 93,
     *      "temp_min": 16,
     *      "temp_max": 16
     *   }
     */
    void parse_temperatures(const QJsonObject& json);
};


/**
 * Periodically downloads weather info from Openweathermaps
 * weather information is parsed into a WeatherStatus objects and
 * can be accessed with get_weather()
 */
class Weather : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString city READ get_city NOTIFY city_updated)
    Q_PROPERTY(
        float temp READ get_temperature NOTIFY temperature_changed)
    Q_PROPERTY(QUrl weatherIcon READ get_weather_icon_url NOTIFY icon_changed)
public:
    /**
     * Constructor for Weather provider
     * @param store access to current weather configuration
     * @param parent
     */
    explicit Weather(
        const IWeatherConfigStore& store, QObject* parent = nullptr);
    /**
     * Update Download interval
     * @param iv interval in seconds
     */
    void set_update_interval(std::chrono::seconds iv);
    std::chrono::seconds get_update_interval() const;

    /**
     * Access to current temperature
     * @deprecated but available for compatibility
     */
    double get_temperature() const;
    /**
     * Icon url for current condition
     * @deprecated but available for compatibility
     */
    QUrl get_weather_icon_url() const;

    /**
     * Access to city name
     */
    QString get_city() const {
        return city_name;
    }

    /**
     * Ugly Interface to pass the information to QML
     * I would like to keep a list of Forecasts in C++ and pass it to QML
     * however I only can pass a list of raw pointers to QML (QObject is not
     * copyable) and I would like to avoid raw pointers in the list so I can
     * manage resources.
     * The initial idea was a method like:
     * Q_INVOKABLE QList<Forecast> get_forecasts() const;
     * A QList<Forecast*> (raw pointers) would work for this case, however there
     * is no way to assert that QML does not hold one of these Forecast pointers
     * while the forecast is updated, the Object may be deleted and we have a
     * dangling pointer.
     * TODO: I have not figured out how to return a
     * QList<std::shared_ptr<Forecast>>
     *
     * Instead I opted for a static array \ref weather where 0 is the current
     * weather
     */

    /**
     * Access the weather status at given time in the future or now (idx=0
     *
     * @param idx 0=current condition forecast for now+idx*3h (more or less)
     * @return Wheater status object
     */
    Q_INVOKABLE DigitalRooster::WeatherStatus* get_weather(int idx) const;

public slots:

    /**
     * Slot triggers refresh of weather data and starts a new download process
     */
    Q_INVOKABLE void refresh();

    /**
     * Read Current weather status as JSON and update member fields
     * @param content  JSON as bytearray
     */
    void parse_weather(const QByteArray& content);

    /**
     * Read Current weather status as JSON and update member fields
     * @param content  weather forecast JSON as bytearray
     */
    void parse_forecast(const QByteArray& content);

signals:
    /**
     * Flag new weather information received
     */
    void weather_info_updated();

    /**
     * City name available
     */
    void city_updated(const QString& cityname);

    /**
     * Current temperature
     */
    void temperature_changed(double temperature);

    /**
     * Weather icon corresponding to current condition
     */
    void icon_changed(const QUrl& img_uri);

    /**
     * Forecast available, emitted after successfully parsing new forecasts
     */
    void forecast_available();

private:
    /**
     * Central configuration and data handler
     */
    const IWeatherConfigStore& cm;

    /**
     * Interval in ms for auto refresh of content
     * default to 1h
     */
    std::chrono::seconds update_interval{3600LL};

    /**
     * Name of location matching the ID, retured by weather-api
     */
    QString city_name;

    /**
     * QTimer for periodic updates
     */
    QTimer timer;

    /**
     * HTTP handle to download JSONs
     */
    HttpClient weather_downloader;
    HttpClient forecast_downloader;

    /**
     * Mutex to lock list of forecasts
     */
    mutable std::mutex forecast_mtx;

    /**
     * Static array of Weatherstatus
     * now + WEATHER_FORECAST_COUNT*3h forecasts should be enough
     */
    std::array<DigitalRooster::WeatherStatus, 1 + WEATHER_FORECAST_COUNT>
        weather;
};

/**
 * create a valid URL to download weather-json from openweathermaps
 * based on information in WeatherConfig
 * @param cfg configuration with location, units etc.
 * @return uri e.g. api.openweathermap.org/data/2.5/weather?zip=94040,us
 */
QUrl create_weather_url(const WeatherConfig& cfg);
/**
 * create a valid URL to download weather-json from openweathermaps
 * based on information in WeatherConfig
 * @param cfg configuration with location, units etc.
 * @return uri e.g. api.openweathermap.org/data/2.5/weather?zip=94040,us
 */
QUrl create_forecast_url(const WeatherConfig& cfg);


} // namespace DigitalRooster
#endif /* _WEATHER_HPP_ */
