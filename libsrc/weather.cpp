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

#include "configuration_manager.hpp"
#include "weather.hpp"
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
    //qDebug() << Q_FUNC_INFO;
    downloader.doDownload(create_weather_uri(cm->get_weather_config()));
}

/*****************************************************************************/
void Weather::parse_response(QByteArray content) {
}
/*****************************************************************************/

QUrl DigitalRooster::create_weather_uri(const WeatherConfig& cfg) {
    //qDebug() << Q_FUNC_INFO;
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
