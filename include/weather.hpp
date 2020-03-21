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

#include <mutex>

#include "IWeatherConfigStore.hpp"

namespace DigitalRooster {

class ConfigurationManager;

/**
 * Object for Forecast data
 * Still a QObject, would be nice if I figure out how to pass a list
 * of Forecasts to QML (and keep the ressources managed and thread-safe)
 */
class Forecast : public QObject {
    Q_OBJECT
    Q_PROPERTY(QDateTime timestamp READ get_timestamp)
    Q_PROPERTY(double temperature READ get_temperature)
    Q_PROPERTY(QUrl weatherIcon READ get_weather_icon_url)
public:
    Forecast() = default;
    /**
     * Create a new forecast  from JSON object
     * @param json
     * @param parent
     */
    explicit Forecast(const QJsonObject& json);

    QDateTime get_timestamp() const {
        return timestamp;
    };

    QUrl get_weather_icon_url() const {
        return icon_url;
    };

    double get_temperature() const {
        return temperature;
    };

private:
    /**
     * expected temperature at timepoint
     */
    double temperature = 0.0;

    /**
     * Weather Icon URL
     */
    QUrl icon_url;

    /**
     * Timepoint for this forecast
     */
    QDateTime timestamp;
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
        return icon_url;
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
     */
    /**
     * Get a copy of Forecast list
     * @return
     */
    const QList<std::shared_ptr<Forecast>> get_forecasts() const;

    /**
     * reach into list of forecasts and get temperature
     * @param fc_idx index in list
     * @return tempertature
     */
    Q_INVOKABLE double get_forecast_temperature(int fc_idx) const;
    /**
     * reach into list of forecasts and get timestamp
     * @param fc_idx index in list
     * @return tempertature
     */
    Q_INVOKABLE QDateTime get_forecast_timestamp(int fc_idx) const;
    /**
     * reach into list of forecasts and get icon url
     * @param fc_idx index in list
     * @return tempertature
     */
    Q_INVOKABLE QUrl get_forecast_icon_url(int fc_idx) const;

public slots:

    /**
     * Slot triggers refresh of weather data and starts a new download process
     */
    void refresh();

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
     * Current Temperature
     */
    double temperature = 0.0;

    /**
     * Name of location matching the ID, retured by weather-api
     */
    QString city_name;

    /**
     * Localized weather condition string
     */
    QString condition;

    /**
     * Icon matching the current weather condition returned by weather-api
     */
    QUrl icon_url;

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
     * list of forecasts
     */
    QList<std::shared_ptr<DigitalRooster::Forecast>> forecasts;

    void parse_city(const QJsonObject& o);
    void parse_temperature(const QJsonObject& o);
    void parse_condition(const QJsonObject& o);
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
