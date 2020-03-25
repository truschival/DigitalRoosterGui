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
    timer.start();
}

/*****************************************************************************/
void Weather::parse_weather(const QByteArray& content) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(content, &perr);
    if (perr.error != QJsonParseError::NoError) {
        qCWarning(CLASS_LC) << "Parsing weather report failed";
        return;
    }
    // input checking in WeatherStatus::validate() called by update();
    auto json = doc.object();
    // Weather 0 is current status
    if (weather[0].update(json)) {
        emit weather_info_updated();
        emit temperature_changed(weather[0].get_temperature());
        emit icon_changed(weather[0].get_weather_icon_url());
    }

    // City is in main.name - in forecast in city.name!
    if (json["name"].isString()) {
        city_name = json["name"].toString();
        qCInfo(CLASS_LC) << "Weather location:" << city_name;
        emit city_updated(city_name);
    } else {
        qCWarning(CLASS_LC) << "Could not read City name for location";
    }
}

/*****************************************************************************/
double Weather::get_temperature() const {
    return weather[0].get_temperature();
}

/*****************************************************************************/
QUrl Weather::get_weather_icon_url() const {
    return weather[0].get_weather_icon_url();
}

/*****************************************************************************/
WeatherStatus* Weather::get_weather(int idx) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        auto ret = new WeatherStatus(&weather.at(idx));
        return ret;
    } catch (std::out_of_range& exc) {
        qCCritical(CLASS_LC) << "Forecast index out of range!";
        return nullptr; // would be nice to throw exception to QML instead
    }
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

    // QJson is very forgiving if "list" does not exist or is not an array
    // default will be created
    auto fc_array = doc["list"].toArray();
    if (fc_array.size() <= 0) {
        qCWarning(CLASS_LC) << "no forecast 'list' found!";
        return;
    }

    auto max_idx = std::min(fc_array.size(), static_cast<int>(weather.size()));
    for (int i = 1; i < max_idx; i++) {
        weather[i].update(fc_array[i].toObject());
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
    qCDebug(CLASS_LC) << request_str;
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
    request_str += "&lang=en";                       // default english
    request_str += "&cnt=" + WEATHER_FORECAST_COUNT; //
    qCDebug(CLASS_LC) << request_str;
    return QUrl(request_str);
}

/*****************************************************************************/
WeatherConfig::WeatherConfig(const QString& token, const QString& location,
    const std::chrono::seconds& interval)
    : api_token(token)
    , location_id(location)
    , update_interval(interval) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
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
void WeatherStatus::validate_json(const QJsonObject& json) {
    if (json["dt"].isUndefined()) {
        throw std::runtime_error("no 'dt' in json object");
    }
    if (json["weather"].isUndefined() || json["weather"].toArray().isEmpty()) {
        throw std::runtime_error("no 'weather' in json object");
    }
    if (json["main"].isUndefined()) {
        throw std::runtime_error("no 'main' in json object");
    }

    auto main = json["main"].toObject();
    if (!main["temp"].isDouble()) {
        throw std::runtime_error("temperature in 'main' is NaN");
    }
    if (!main["temp_min"].isDouble()) {
        throw std::runtime_error("temp_min in 'main' is NaN");
    }
    if (!main["temp_max"].isDouble()) {
        throw std::runtime_error("temp_max in 'main' is NaN");
    }
}

/*****************************************************************************/
bool WeatherStatus::update(const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // try-catch here because we are called in signal-slot context
    try {
        validate_json(json);
    } catch (std::runtime_error& exc) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << exc.what();
        return false;
    }
    // extract temperatures
    parse_temperatures(json);

    // for some reasons the weather is an array with 1 element
    auto weather_array = json["weather"].toArray();
    auto weather = weather_array.at(0).toObject();

    // get Icon
    icon_url =
        QUrl(WEATHER_ICON_BASE_URL + weather["icon"].toString() + ".png");
    // get timestamp
    timestamp =
        QDateTime::fromSecsSinceEpoch(json["dt"].toInt(), QTimeZone::utc());

    qCDebug(CLASS_LC) << "forecast for" << timestamp << " temp:" << temperature
                      << "°C icon:" << icon_url;
    return true;
}

/*****************************************************************************/
void WeatherStatus::parse_temperatures(const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto main = json["main"].toObject();
    temperature = main["temp"].toDouble();
    temp_min = main["temp_min"].toDouble();
    temp_max = main["temp_max"].toDouble();
}

/*****************************************************************************/
