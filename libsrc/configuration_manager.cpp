/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#include <QString>
#include <QTime>
#include <appconstants.hpp>
#include <iostream>
#include <stdexcept>

#include "UpdateTask.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

/*****************************************************************************/
ConfigurationManager::ConfigurationManager(const QString& filepath)
    : filepath(filepath) {
    readJson();
    read_radio_streams_from_file();
    read_podcasts_from_file();
    read_alarms_from_file();
};

/*****************************************************************************/
void ConfigurationManager::readJson() {
    QString val;
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory),
            file.errorString().toStdString());
    }

    val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    appconfig = doc.object();
}

/*****************************************************************************/
void ConfigurationManager::read_radio_streams_from_file() {
    QJsonArray irconfig =
        appconfig[DigitalRooster::KEY_GROUP_IRADIO_SOURCES].toArray();
    for (const auto ir : irconfig) {
        std::cout << ir.toString().toStdString() << std::endl;
        QString name(ir.toObject()[KEY_NAME].toString());
        QUrl url(ir.toObject()[KEY_URI].toString());
        if (url.isValid()) {
            stream_sources.push_back(std::make_shared<PlayableItem>(name, url));
        }
    }
}

/*****************************************************************************/
void ConfigurationManager::read_podcasts_from_file() {
    QJsonArray podcasts =
        appconfig[DigitalRooster::KEY_GROUP_PODCAST_SOURCES].toArray();
    for (const auto pc : podcasts) {
        auto jo = pc.toObject();

        QUrl url(jo[KEY_URI].toString());
        if (url.isValid()) {
            auto ps = std::make_shared<PodcastSource>(url);
            if (jo.contains(KEY_UPDATE_INTERVAL)) {
                ps->set_update_interval(
                    jo[KEY_UPDATE_INTERVAL].toInt(3600) * 1000);
            }
            podcast_sources.push_back(ps);
        }
    }
}

/*****************************************************************************/
void ConfigurationManager::read_alarms_from_file() {
    QJsonArray alarm_config =
        appconfig[DigitalRooster::KEY_GROUP_ALARMS].toArray();
    for (const auto al : alarm_config) {
        auto json_alarm = al.toObject();
        QUrl url(json_alarm[KEY_URI].toString());
        if (!url.isValid()) {
            qDebug() << "Invalid URI " << json_alarm[KEY_URI].toString();
            continue;
        }
        auto period = json_string_to_alarm_period(
            json_alarm[KEY_ALARM_PERIOD].toString(KEY_ALARM_DAILY));
        auto enabled = json_alarm[KEY_ENABLED].toBool(true);
        auto media = QUrl(json_alarm[KEY_URI].toString());

        auto timepoint =
            QTime::fromString(json_alarm[KEY_TIME].toString(), "hh:mm");
        auto alarm = std::make_shared<Alarm>(media, timepoint, period, enabled);
        auto volume = json_alarm.value(KEY_VOLUME).toDouble(50);
        alarm->set_volume(volume);

        alarms.push_back(alarm);
    }
}

/*****************************************************************************/
void ConfigurationManager::add_radio_station(
    std::unique_ptr<PlayableItem> src) {
    this->stream_sources.push_back(
        std::shared_ptr<PlayableItem>(std::move(src)));
}

/*****************************************************************************/
void ConfigurationManager::write_config_file() {
    /*clear previous doc */
    QJsonArray podcasts;
    for (const auto& ps : get_podcast_sources()) {
        QJsonObject psconfig;
        psconfig[KEY_NAME] = ps->get_title();
        psconfig[KEY_URI] = ps->get_url().toString();
        podcasts.append(psconfig);
    }

    appconfig[KEY_GROUP_PODCAST_SOURCES] = podcasts;

    QJsonArray iradios;
    for (const auto& iradiostream : get_stream_sources()) {
        QJsonObject irconfig;
        irconfig[KEY_NAME] = iradiostream->get_display_name();
        irconfig[KEY_URI] = iradiostream->get_url().toString();
        iradios.append(irconfig);
    }
    appconfig[KEY_GROUP_IRADIO_SOURCES] = iradios;

    QFile tf(filepath);
    tf.open(QIODevice::ReadWrite | QIODevice::Text);
    QJsonDocument doc(appconfig);
    tf.write(doc.toJson());
    tf.close();
}

/*****************************************************************************/

static const QMap<QString, Alarm::Period> period_literals = {
    {KEY_ALARM_DAILY, Alarm::Daily}, {KEY_ALARM_WORKDAYS, Alarm::Workdays},
    {KEY_ALARM_WEEKEND, Alarm::Weekend}, {KEY_ALARM_ONCE, Alarm::Once}};
/*****************************************************************************/

Alarm::Period DigitalRooster::json_string_to_alarm_period(
    const QString& literal) {
    if (period_literals.contains(literal))
        return period_literals[literal];
    throw(std::invalid_argument(literal.toStdString()));
}
