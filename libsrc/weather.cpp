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

#include "IWeatherConfigStore.hpp"
#include "weather.hpp"

using namespace DigitalRooster;
using namespace std;
using namespace std::chrono;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.Weather");

/*****************************************************************************/
Weather::Weather(const IWeatherConfigStore& store, QObject* parent)
    : cm(store) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // timer starts refresh, refresh calls downloader
    connect(&timer, &QTimer::timeout, this, &Weather::refresh);
    // downloader finished -> parse result
    connect(&weather_downloader, &HttpClient::dataAvailable, this,
        &Weather::parse_weather);
    connect(&forecast_downloader, &HttpClient::dataAvailable, this,
        &Weather::parse_forecast);

    timer.setInterval(duration_cast<milliseconds>(update_interval));
    timer.setSingleShot(false);
    timer.start();
    weather_downloader.doDownload(create_weather_url(cm.get_weather_config()));
    forecast_downloader.doDownload(
        create_forecast_url(cm.get_weather_config()));
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
    /* restart downloads */
    weather_downloader.doDownload(create_weather_url(cm.get_weather_config()));
    forecast_downloader.doDownload(
        create_forecast_url(cm.get_weather_config()));
}

/*****************************************************************************/
void Weather::parse_weather(const QByteArray& content) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(content, &perr);
    if (perr.error != QJsonParseError::NoError) {
        qCWarning(CLASS_LC) << "Parsing weather failed";
        return;
    }
    QJsonObject o = doc.object();
    parse_city(o);
    parse_temperature(o);
    parse_condition(o); // also extracts icon
    emit weather_info_updated();
}

/*****************************************************************************/
void Weather::parse_forecast(const QByteArray& content) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(content, &perr);
    if (perr.error != QJsonParseError::NoError) {
        qCWarning(CLASS_LC) << "Parsing forecast failed";
        return;
    }
    QJsonObject o = doc.object();
    auto fc_array = doc["list"].toArray();
    // TODO: probably need some mutex
    forecasts.clear();
    for (const auto fc_val : fc_array) {
    	forecasts.append(
            std::make_shared<DigitalRooster::Forecast>(fc_val.toObject()));
    }
    emit forecast_available();
}

/*****************************************************************************/
QUrl DigitalRooster::create_weather_url(const WeatherConfig& cfg) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString request_str(WEATHER_API_BASE_URL);
    request_str.reserve(512);
    request_str += "id=";
    request_str += cfg.get_location_id();
    request_str += "&units=metric";
    request_str += "&appid=";
    request_str += cfg.get_api_token();
    request_str += "&lang=en"; // default english
    return QUrl(request_str);
}

/*****************************************************************************/
QUrl DigitalRooster::create_forecast_url(const WeatherConfig& cfg) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString request_str(WEATHER_FORECASTS_API_BASE_URL);
    request_str.reserve(512);
    request_str += "id=";
    request_str += cfg.get_location_id();
    request_str += "&units=metric";
    request_str += "&appid=";
    request_str += cfg.get_api_token();
    request_str += "&lang=en"; // default english
    request_str += "&cnt=5";   // ~now, +3h, +6h, +9h, +12h
    return QUrl(request_str);
}

/*****************************************************************************/
void Weather::parse_city(const QJsonObject& o) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    city_name = o["name"].toString();
    qCInfo(CLASS_LC) << "Weather location:" << city_name;
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
        qCWarning(CLASS_LC) << "couldn't read weather JSON object";
        return;
    }
    condition = weather["description"].toString();
    icon_url =
        QUrl(WEATHER_ICON_BASE_URL + weather["icon"].toString() + ".png");
    emit condition_changed(condition);
    emit icon_changed(icon_url);
}

/*****************************************************************************/
WeatherConfig::WeatherConfig(const QString& token, const QString& location,
    const std::chrono::seconds& interval)
    : api_token(token)
    , location_id(location)
    , update_interval(interval) {
}

/*****************************************************************************/
QJsonObject WeatherConfig::to_json_object() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject j;
    j[KEY_UPDATE_INTERVAL] = static_cast<qint64>(update_interval.count());
    j[KEY_WEATHER_LOCATION_ID] = location_id;
    j[KEY_WEATHER_API_KEY] = api_token;
    return j;
}

/*****************************************************************************/
WeatherConfig WeatherConfig::from_json_object(const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (json[KEY_WEATHER_LOCATION_ID].toString().isEmpty()) {
        throw std::invalid_argument("Json Weather has no location id");
    }
    if (json[KEY_WEATHER_API_KEY].toString().isEmpty()) {
        throw std::invalid_argument("Json Weather has no API key !");
    }
    auto interval =
        std::chrono::seconds(json[KEY_UPDATE_INTERVAL].toInt(3600LL));

    return WeatherConfig(json[KEY_WEATHER_API_KEY].toString(),
        json[KEY_WEATHER_LOCATION_ID].toString(), interval);
}

/*****************************************************************************/
Forecast::Forecast(const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto main = json["main"].toObject();
    // for some reasons the weather is an array with 1 element
    auto weather_array = json["weather"].toArray();
    auto weather = weather_array.at(0).toObject();

    // According to api doc: "dt_txt": "2020-01-07 15:00:00"
    timestamp =
        QDateTime::fromSecsSinceEpoch(json["dt"].toInt(), QTimeZone::utc());

    if (!main.isEmpty()) {
        temperature = main["temp"].toDouble();
    }
    if (!weather.isEmpty()) {
        icon_url =
            QUrl(WEATHER_ICON_BASE_URL + weather["icon"].toString() + ".png");
    }

    qCDebug(CLASS_LC) << "forecast for" << timestamp << ":" << temperature
                      << "°C icon:" << icon_url;
}

/*****************************************************************************/
