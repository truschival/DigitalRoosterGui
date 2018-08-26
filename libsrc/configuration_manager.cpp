/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QStandardPaths>
#include <QString>
#include <QTime>

#include <chrono>
#include <iostream>
#include <stdexcept>

#include "alarm.hpp"
#include "appconstants.hpp"

#include "UpdateTask.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

/*****************************************************************************/
ConfigurationManager::ConfigurationManager()
    : alarmtimeout(DEFAULT_ALARM_TIMEOUT)
    , sleeptimeout(DEFAULT_SLEEP_TIMEOUT) {

    connect(&filewatcher, SIGNAL(fileChanged(const QString&)), this,
        SLOT(fileChanged(const QString&)));
};

/*****************************************************************************/
void ConfigurationManager::refresh_configuration() {
    alarms.clear();
    podcast_sources.clear();
    stream_sources.clear();
    auto filepath = check_and_create_config();

    auto content = getJsonFromFile(filepath);
    parseJson(content.toUtf8());

    emit configuration_changed();
}

/*****************************************************************************/
QString ConfigurationManager::getJsonFromFile(const QString& path) {
    QString content;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory),
            file.errorString().toStdString());
    }

    content = file.readAll();
    file.close();

    return content;
}

/*****************************************************************************/
void ConfigurationManager::parseJson(const QByteArray& json) {
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject appconfig = doc.object();
    /* get application config */
    auto at = appconfig[DigitalRooster::KEY_ALARM_TIMEOUT];
    if (!at.isUndefined()) {
        alarmtimeout =
            std::chrono::minutes(at.toInt(DEFAULT_ALARM_TIMEOUT.count()));
    } else {
        alarmtimeout = std::chrono::minutes(DEFAULT_ALARM_TIMEOUT.count());
    }

    read_radio_streams_from_file(appconfig);
    read_podcasts_from_file(appconfig);
    read_alarms_from_file(appconfig);
    read_weather_from_file(appconfig);
}

/*****************************************************************************/
void ConfigurationManager::read_radio_streams_from_file(
    const QJsonObject& appconfig) {
    QJsonArray irconfig =
        appconfig[DigitalRooster::KEY_GROUP_IRADIO_SOURCES].toArray();
    for (const auto ir : irconfig) {
        QString name(ir.toObject()[KEY_NAME].toString());
        QUrl url(ir.toObject()[KEY_URI].toString());
        if (url.isValid()) {
            stream_sources.push_back(std::make_shared<PlayableItem>(name, url));
        }
    }
}

/*****************************************************************************/
void ConfigurationManager::read_podcasts_from_file(
    const QJsonObject& appconfig) {
    QJsonArray podcasts =
        appconfig[DigitalRooster::KEY_GROUP_PODCAST_SOURCES].toArray();
    for (const auto pc : podcasts) {
        auto jo = pc.toObject();

        QUrl url(jo[KEY_URI].toString());
        if (url.isValid()) {
            auto ps = std::make_shared<PodcastSource>(url);
            ps->set_update_interval(jo[KEY_UPDATE_INTERVAL].toInt(3600) * 1000);
            podcast_sources.push_back(ps);
        }
    }
}

/*****************************************************************************/
void ConfigurationManager::read_alarms_from_file(const QJsonObject& appconfig) {
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
        auto volume = json_alarm[KEY_VOLUME].toInt(DEFAULT_ALARM_VOLUME);
        alarm->set_volume(volume);
        /* if no specific alarm timeout is given take application default */
        auto timeout =
            json_alarm[KEY_ALARM_TIMEOUT].toInt(alarmtimeout.count());
        alarm->set_timeout(std::chrono::minutes(timeout));

        alarms.push_back(alarm);
    }
}


/*****************************************************************************/
void ConfigurationManager::read_weather_from_file(
    const QJsonObject& appconfig) {
    if (appconfig[KEY_WEATHER].isNull()) {
        qWarning() << "no weather configuration found!";
        return;
    }
    QJsonObject json_weather = appconfig[KEY_WEATHER].toObject();
    if (!json_weather[KEY_WEATHER_API_KEY].isNull()) {
        weather_cfg.apikey = json_weather[KEY_WEATHER_API_KEY].toString();
    } else {
        qWarning() << "No openweathermaps API Key configured goto "
                      "https://openweathermap.org and get one!";
    }

    if (!json_weather[KEY_WEATHER_LOCATION_ID].isNull()) {
        weather_cfg.cityid = json_weather[KEY_WEATHER_LOCATION_ID].toString();
    } else {
        qWarning() << "No weather location ID configured!";
    }
}

/*****************************************************************************/
void ConfigurationManager::add_radio_station(
    std::shared_ptr<PlayableItem> src) {
    this->stream_sources.push_back(src);
}
/*****************************************************************************/
void ConfigurationManager::add_alarm(std::shared_ptr<Alarm> alm) {
    this->alarms.push_back(alm);
}

/*****************************************************************************/
void ConfigurationManager::store_current_config() {
    QJsonObject appconfig;

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

    QJsonArray alarms_json;
    for (const auto& alarm : get_alarms()) {
        QJsonObject alarmcfg;
        alarmcfg[KEY_ALARM_PERIOD] =
            alarm_period_to_json_string(alarm->get_period());
        alarmcfg[KEY_TIME] = alarm->get_time().toString("hh:mm");
        alarmcfg[KEY_VOLUME] = alarm->get_volume();
        alarmcfg[KEY_URI] = alarm->get_media()->get_url().toString();
        alarms_json.append(alarmcfg);
    }
    appconfig[KEY_GROUP_ALARMS] = alarms_json;

    /* Store Weather information*/
    QJsonObject json_weather;
    json_weather[KEY_WEATHER_API_KEY] = weather_cfg.apikey;
    json_weather[KEY_WEATHER_LOCATION_ID] = weather_cfg.cityid;
    appconfig[KEY_WEATHER] = json_weather;

    /* global application configuration */
    appconfig[KEY_ALARM_TIMEOUT] = static_cast<qint64>(alarmtimeout.count());
    appconfig[KEY_SLEEP_TIMEOUT] = static_cast<qint64>(sleeptimeout.count());
    /* Static info - which version created the config file*/
    appconfig[KEY_VERSION] = PROJECT_VERSION;

    write_config_file(appconfig);
}

/*****************************************************************************/
void ConfigurationManager::write_config_file(const QJsonObject& appconfig) {
    auto config_dir = make_sure_config_path_exists();
    auto file_path = config_dir.filePath(CONFIG_JSON_FILE_NAME);

    QFile config_file(file_path);
    config_file.open(
        QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QJsonDocument doc(appconfig);
    config_file.write(doc.toJson());
    config_file.close();
}

/*****************************************************************************/
void ConfigurationManager::create_default_configuration() {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::fromString("06:30", "hh:mm"));
    alm->set_period(Alarm::Workdays);
    alarms.push_back(alm);

    auto acw = std::make_shared<DigitalRooster::PodcastSource>(
        QUrl("http://armscontrolwonk.libsyn.com/rss"));
    podcast_sources.push_back(acw);
    auto mdwap = std::make_shared<DigitalRooster::PodcastSource>(
        QUrl("https://rss.acast.com/mydadwroteaporno"));
    podcast_sources.push_back(mdwap);

    auto radio =
        std::make_shared<DigitalRooster::PlayableItem>("Deutschlandfunk (Ogg)",
            QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"));
    stream_sources.push_back(radio);

    store_current_config();
}

/*****************************************************************************/
QDir ConfigurationManager::make_sure_config_path_exists() {
    auto config_path =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir config_dir(config_path);
    if (!config_dir.mkpath(".")) {
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory),
            "Cannot create configuration path!");
    }
    return config_dir;
}
/*****************************************************************************/

void ConfigurationManager::fileChanged(const QString& path) {
    qDebug() << " Config changed, reloading";
    refresh_configuration();
}

/*****************************************************************************/

QString ConfigurationManager::check_and_create_config() {
    auto config_dir = make_sure_config_path_exists();
    // check if file exists -> assume some default config and write file
    auto file_path = config_dir.filePath(CONFIG_JSON_FILE_NAME);
    QFile config_file(file_path);
    if (!config_file.exists()) {
        create_default_configuration();
    }
    filewatcher.addPath(file_path);

    return file_path;
}

/*****************************************************************************/
