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

#include "weather.hpp"
#include "configuration_manager.hpp"
#include <QDebug>
#include <QJsonObject>
#include <QString>
#include <appconstants.hpp>
#include <memory>
#include <stdexcept> // std::system_error

using namespace DigitalRooster;
using namespace std;
using namespace std::chrono;

/*****************************************************************************/
Weather::Weather(std::shared_ptr<ConfigurationManager> confman, QObject* parent)
    : cm(confman) {
    // timer starts refresh, refresh calls downloader
    connect(&timer, SIGNAL(timeout()), this, SLOT(refresh()));
    // downloader finished -> parse result
    connect(&downloader, SIGNAL(dataAvailable(QByteArray)), this,
        SLOT(parse_response(QByteArray)));

    timer.setInterval(duration_cast<milliseconds>(update_interval));
    timer.setSingleShot(false);
    timer.start();
}

/*****************************************************************************/
void Weather::set_update_interval(std::chrono::seconds interval) {
    update_interval = interval;
    timer.setInterval(duration_cast<milliseconds>(update_interval));
    timer.start(duration_cast<milliseconds>(update_interval));
}
/*****************************************************************************/

std::chrono::seconds Weather::get_update_interval() const {
    return duration_cast<seconds>(timer.intervalAsDuration());
}

/*****************************************************************************/
void Weather::refresh() {
    // qDebug() << Q_FUNC_INFO;
    downloader.doDownload(create_weather_uri(cm->get_weather_config()));
}

/*****************************************************************************/
void Weather::parse_response(QByteArray content) {
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(content, &perr);
    if (perr.error != QJsonParseError::NoError) {
        qWarning() << "Parsing Failed";
        return;
    }
    QJsonObject o = doc.object();
    parse_city(o);
    parse_temperature(o);
    parse_condition(o); // also extracts icon
    emit weather_info_available(o);
}
/*****************************************************************************/

QUrl DigitalRooster::create_weather_uri(const WeatherConfig& cfg) {
    // qDebug() << Q_FUNC_INFO;
    QString uri_str(cfg.base_uri);
    uri_str += "id=";
    uri_str += cfg.cityid;
    uri_str += "&units=";
    uri_str += cfg.units;
    uri_str += "&appid=";
    uri_str += cfg.apikey;
    qDebug() << uri_str;
    return QUrl(uri_str);
}
/*****************************************************************************/
void Weather::parse_city(const QJsonObject& o) {
    qDebug() << Q_FUNC_INFO;
    city_name = o["name"].toString();
    emit city_updated(city_name);
}
/*****************************************************************************/
void Weather::parse_temperature(const QJsonObject& o) {
    qDebug() << Q_FUNC_INFO;
    auto main = o["main"].toObject();
    if (!main.isEmpty()) {
        temperature = main["temp"].toDouble();
        emit temperature_changed(temperature);
    }
}

/*****************************************************************************/
void Weather::parse_condition(const QJsonObject& o) {
    qDebug() << Q_FUNC_INFO;
    // Weather is an array
    auto weather_arr = o["weather"].toArray();
    auto weather = weather_arr.at(0);
    if (weather.isUndefined()) {
        qWarning() << " couldn't read weather JSON object";
        return;
    }
    condition = weather["description"].toString();
    icon_id = weather["icon"].toString();
    emit condition_changed(condition);
    emit icon_changed(icon_id);
}
/*****************************************************************************/
