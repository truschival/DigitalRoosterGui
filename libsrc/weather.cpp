/******************************************************************************
 * \filename
 * \brief  parse weather information form openweathermaps
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QString>
#include <appconstants.hpp>
#include <memory>
#include <stdexcept> // std::system_error

#include "configuration_manager.hpp"
#include "weather.hpp"

using namespace DigitalRooster;
using namespace std;
using namespace std::chrono;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.Weather");

/*****************************************************************************/
Weather::Weather(std::shared_ptr<ConfigurationManager> confman, QObject* parent)
    : cm(confman) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // timer starts refresh, refresh calls downloader
    connect(&timer, SIGNAL(timeout()), this, SLOT(refresh()));
    // downloader finished -> parse result
    connect(&downloader, SIGNAL(dataAvailable(QByteArray)), this,
        SLOT(parse_response(QByteArray)));

    timer.setInterval(duration_cast<milliseconds>(update_interval));
    timer.setSingleShot(false);
    timer.start();
    downloader.doDownload(create_weather_uri(cm->get_weather_config()));
}

/*****************************************************************************/
void Weather::set_update_interval(std::chrono::seconds interval) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    update_interval = interval;
    timer.setInterval(duration_cast<milliseconds>(update_interval));
    timer.start(duration_cast<milliseconds>(update_interval));
}
/*****************************************************************************/

std::chrono::seconds Weather::get_update_interval() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return duration_cast<seconds>(timer.intervalAsDuration());
}

/*****************************************************************************/
void Weather::refresh() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    downloader.doDownload(create_weather_uri(cm->get_weather_config()));
}

/*****************************************************************************/
void Weather::parse_response(QByteArray content) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(content, &perr);
    if (perr.error != QJsonParseError::NoError) {
        qCWarning(CLASS_LC) << "Parsing Failed";
        return;
    }
    QJsonObject o = doc.object();
    parse_city(o);
    parse_temperature(o);
    parse_condition(o); // also extracts icon
    emit weather_info_updated();
}
/*****************************************************************************/

QUrl DigitalRooster::create_weather_uri(const WeatherConfig* cfg) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString request_str({"http://api.openweathermap.org/data/2.5/weather?"});
    request_str.reserve(512);
    request_str += "id=";
    request_str += cfg->get_location_id();
    request_str += "&units=metric";
    request_str += "&appid=";
    request_str += cfg->get_api_token();
    return QUrl(request_str);
}
/*****************************************************************************/
void Weather::parse_city(const QJsonObject& o) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    city_name = o["name"].toString();
    emit city_updated(city_name);
}
/*****************************************************************************/
void Weather::parse_temperature(const QJsonObject& o) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto main = o["main"].toObject();
    if (!main.isEmpty()) {
        temperature = main["temp"].toDouble();
        emit temperature_changed(temperature);
    }
}

/*****************************************************************************/
void Weather::parse_condition(const QJsonObject& o) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // Weather is an array
    auto weather_arr = o["weather"].toArray();
    auto weather = weather_arr.at(0);
    if (weather.isUndefined()) {
        qCWarning(CLASS_LC) << " couldn't read weather JSON object";
        return;
    }
    condition = weather["description"].toString();
    icon_id = weather["icon"].toString();
    emit condition_changed(condition);
    emit icon_changed(icon_id);
}

/*****************************************************************************/
WeatherConfig::WeatherConfig(const QString& token, const QString& location,
    const std::chrono::seconds& interval)
    : api_token(token)
    , location_id(location)
    , update_interval(interval) {
}

/*****************************************************************************/
QJsonObject WeatherConfig::to_json_object() const{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject j;
    j[KEY_UPDATE_INTERVAL] = static_cast<qint64>(update_interval.count());
    j[KEY_WEATHER_LOCATION_ID] = location_id;
    j[KEY_WEATHER_API_KEY] = api_token;
    return j;
}

/*****************************************************************************/
std::unique_ptr<WeatherConfig> WeatherConfig::from_json_object(const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (json[KEY_WEATHER_LOCATION_ID].toString().isEmpty()) {
        throw std::invalid_argument("Json Weather has no location id");
    }
    if (json[KEY_WEATHER_API_KEY].toString().isEmpty()) {
        throw std::invalid_argument("Json Weather has no API key !");
    }
    auto interval =
        std::chrono::seconds(json[KEY_UPDATE_INTERVAL].toInt(3600LL));

    return std::make_unique<WeatherConfig>(json[KEY_WEATHER_API_KEY].toString(),
        json[KEY_WEATHER_LOCATION_ID].toString(), interval);
}
/*****************************************************************************/
