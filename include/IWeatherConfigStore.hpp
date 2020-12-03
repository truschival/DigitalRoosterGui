/******************************************************************************
 * \filename
 * \brief	Interface for access to WeatherConfig
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef INCLUDE_IWEATHERCONFIGSTORE_HPP_
#define INCLUDE_IWEATHERCONFIGSTORE_HPP_

#include <QString>

#include <QJsonObject>
#include <chrono>
#include <memory>

namespace DigitalRooster {
/**
 * Configuration of weather source as found in application config file
 * read by ConfigurationManager \ref ConfigurationManager::get_weather_config
 */
class WeatherConfig {
public:
    /**
     * Constructor with 'relevant' information
     * @param token
     * @param cityid
     * @param interval 1hour
     */
    WeatherConfig(QString token = QString(),
        QString cityid = QString(),
        std::chrono::seconds interval = std::chrono::seconds(3600));

    /**
     * create Weatherconfig form Json configuration
     * @param json jobject
     * @return
     */
    static WeatherConfig from_json_object(const QJsonObject& json);

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
    const QString& get_api_token() const {
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
 * Interface for addition, access of Weather Settings
 * actual storage \ref DigitalRooster::ConfigurationManager
 */
class IWeatherConfigStore {
public:
    /**
     * Read current WeatherConfig
     * @return weatherconfig
     */
    virtual const WeatherConfig& get_weather_config() const = 0;

    /**
     * virtual destructor
     */
    virtual ~IWeatherConfigStore(){};
};
} // namespace DigitalRooster

#endif /* INCLUDE_IWEATHERCONFIGSTORE_HPP_ */
