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

#include <QLoggingCategory>
#include <QSaveFile>
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
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.ConfigurationManager");

/*****************************************************************************/
ConfigurationManager::ConfigurationManager(const QString& configdir)
    : alarmtimeout(DEFAULT_ALARM_TIMEOUT)
    , sleeptimeout(DEFAULT_SLEEP_TIMEOUT)
    , volume(DEFAULT_VOLUME)
    , brightness_sb(DEFAULT_BRIGHTNESS)
    , brightness_act(DEFAULT_BRIGHTNESS)
    , config_dir(configdir) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    writeTimer.setInterval(std::chrono::seconds(5));
    writeTimer.setSingleShot(true);
    connect(&writeTimer, SIGNAL(timeout()), this, SLOT(store_current_config()));
    // open or create configuration
    make_sure_config_path_exists();
    auto path = check_and_create_config();
    filewatcher.addPath(path);
    // store connection to disconnect during write_config_file
    fwConn = connect(&filewatcher, &QFileSystemWatcher::fileChanged, this,
        &ConfigurationManager::fileChanged);
};

/*****************************************************************************/
void ConfigurationManager::refresh_configuration() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    alarms.clear();
    podcast_sources.clear();
    stream_sources.clear();
    auto content = getJsonFromFile(get_configuration_path());
    parseJson(content.toUtf8());
    emit configuration_changed();
}

/*****************************************************************************/
QString ConfigurationManager::getJsonFromFile(const QString& path) {
    QString content;
    QFile file(path);
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCCritical(CLASS_LC) << file.errorString();
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
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
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
    auto bright_act = appconfig[KEY_BRIGHTNESS_ACT];
    if (!bright_act.isUndefined()) {
        set_active_brightness(bright_act.toInt(DEFAULT_BRIGHTNESS));
    }
    auto bright_sb = appconfig[KEY_BRIGHTNESS_SB];
    if (!bright_sb.isUndefined()) {
        set_standby_brightness(bright_sb.toInt(DEFAULT_BRIGHTNESS));
    }
    auto vol = appconfig[KEY_VOLUME];
    if (!vol.isUndefined()) {
        set_volume(vol.toInt(DEFAULT_VOLUME));
    }

    read_radio_streams_from_file(appconfig);
    read_podcasts_from_file(appconfig);
    read_alarms_from_file(appconfig);
    read_weather_from_file(appconfig);
}

/*****************************************************************************/
void ConfigurationManager::read_radio_streams_from_file(
    const QJsonObject& appconfig) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonArray irconfig =
        appconfig[DigitalRooster::KEY_GROUP_IRADIO_SOURCES].toArray();
    for (const auto ir : irconfig) {
        QString name(ir.toObject()[KEY_NAME].toString());
        QUrl url(ir.toObject()[KEY_URI].toString());
        if (!url.isValid()) {
            qCWarning(CLASS_LC)
                << "Invalid URI " << ir.toObject()[KEY_URI].toString();
            continue;
        }
        stream_sources.push_back(std::make_shared<PlayableItem>(name, url));
    }
    qCDebug(CLASS_LC) << "read" << stream_sources.size() << "streams";
}

/*****************************************************************************/
void ConfigurationManager::read_podcasts_from_file(
    const QJsonObject& appconfig) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonArray podcasts =
        appconfig[DigitalRooster::KEY_GROUP_PODCAST_SOURCES].toArray();
    for (const auto pc : podcasts) {
        auto jo = pc.toObject();

        QUrl url(jo[KEY_URI].toString());
        if (!url.isValid()) {
            qCWarning(CLASS_LC) << "Invalid URI " << jo[KEY_URI].toString();
            continue;
        }
        auto ps = std::make_shared<PodcastSource>(url);
        ps->set_update_task(std::make_unique<UpdateTask>(ps.get()));
        ps->set_update_interval(
            std::chrono::seconds(jo[KEY_UPDATE_INTERVAL].toInt(3600)));

        // Get notifications if name etc. changes
        connect(ps.get(), &PodcastSource::dataChanged, this,
            &ConfigurationManager::dataChanged);

        podcast_sources.push_back(ps);
    }
    qCDebug(CLASS_LC) << "read" << podcast_sources.size() << "podcasts";
}

/*****************************************************************************/
void ConfigurationManager::read_alarms_from_file(const QJsonObject& appconfig) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonArray alarm_config =
        appconfig[DigitalRooster::KEY_GROUP_ALARMS].toArray();
    for (const auto al : alarm_config) {
        auto json_alarm = al.toObject();
        QUrl url(json_alarm[KEY_URI].toString());
        if (!url.isValid()) {
            qCWarning(CLASS_LC)
                << "Invalid URI " << json_alarm[KEY_URI].toString();
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
        auto id = json_alarm[KEY_ID].toInt(QDateTime::currentMSecsSinceEpoch());
        alarm->set_id(id);
        /* if no specific alarm timeout is given take application default */
        auto timeout =
            json_alarm[KEY_ALARM_TIMEOUT].toInt(alarmtimeout.count());
        alarm->set_timeout(std::chrono::minutes(timeout));

        connect(alarm.get(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
        alarms.push_back(alarm);
    }
    qCDebug(CLASS_LC) << "read" << alarms.size() << "alarms";
}

/*****************************************************************************/
void ConfigurationManager::read_weather_from_file(
    const QJsonObject& appconfig) {
    if (appconfig[KEY_WEATHER].isNull()) {
        qCWarning(CLASS_LC) << "no weather configuration found!";
        return;
    }
    QJsonObject json_weather = appconfig[KEY_WEATHER].toObject();
    if (!json_weather[KEY_WEATHER_API_KEY].isNull()) {
        weather_cfg.apikey = json_weather[KEY_WEATHER_API_KEY].toString();
    } else {
        qCWarning(CLASS_LC) << "No openweathermaps API Key configured goto "
                               "https://openweathermap.org and get one!";
    }

    if (!json_weather[KEY_WEATHER_LOCATION_ID].isNull()) {
        weather_cfg.cityid = json_weather[KEY_WEATHER_LOCATION_ID].toString();
    } else {
        qCWarning(CLASS_LC) << "No weather location ID configured!";
    }
}

/*****************************************************************************/
void ConfigurationManager::add_radio_station(
    std::shared_ptr<PlayableItem> src) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->stream_sources.push_back(src);
}
/*****************************************************************************/
void ConfigurationManager::add_alarm(std::shared_ptr<Alarm> alm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->alarms.push_back(alm);
    dataChanged();
}

/*****************************************************************************/
void ConfigurationManager::dataChanged() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    writeTimer.start();
}

/*****************************************************************************/
void ConfigurationManager::fileChanged(const QString& path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    refresh_configuration();
}

/*****************************************************************************/
void ConfigurationManager::set_volume(int vol) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << vol;
    if (vol >= 0 && vol <= 100) {
        this->volume = vol;
        writeTimer.start();
    } else {
        qCWarning(CLASS_LC) << "invalid volume value: " << vol;
    }
}

/*****************************************************************************/
void ConfigurationManager::set_standby_brightness(int brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << brightness;
    if (brightness >= 0 && brightness <= 100) {
        this->brightness_sb = brightness;
        writeTimer.start();
    } else {
        qCWarning(CLASS_LC) << "invalid brightness value: " << brightness;
    }
}

/*****************************************************************************/
void ConfigurationManager::set_active_brightness(int brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << brightness;
    if (brightness >= 0 && brightness <= 100) {
        this->brightness_act = brightness;
        writeTimer.start();
    } else {
        qCWarning(CLASS_LC) << "invalid brightness value: " << brightness;
    }
}

/*****************************************************************************/
void ConfigurationManager::store_current_config() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject appconfig;

    QJsonArray podcasts;
    for (const auto& ps : podcast_sources) {
        QJsonObject psconfig;
        psconfig[KEY_NAME] = ps->get_title();
        psconfig[KEY_URI] = ps->get_url().toString();
        podcasts.append(psconfig);
    }
    appconfig[KEY_GROUP_PODCAST_SOURCES] = podcasts;

    QJsonArray iradios;
    for (const auto& iradiostream : stream_sources) {
        QJsonObject irconfig;
        irconfig[KEY_NAME] = iradiostream->get_display_name();
        irconfig[KEY_URI] = iradiostream->get_url().toString();
        iradios.append(irconfig);
    }
    appconfig[KEY_GROUP_IRADIO_SOURCES] = iradios;

    QJsonArray alarms_json;
    for (const auto& alarm : alarms) {
        QJsonObject alarmcfg;
        alarmcfg[KEY_ID] = alarm->get_id();
        alarmcfg[KEY_ALARM_PERIOD] =
            alarm_period_to_json_string(alarm->get_period());
        alarmcfg[KEY_TIME] = alarm->get_time().toString("hh:mm");
        alarmcfg[KEY_VOLUME] = alarm->get_volume();
        alarmcfg[KEY_URI] = alarm->get_media()->get_url().toString();
        alarmcfg[KEY_ENABLED] = alarm->is_enabled();
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
    appconfig[KEY_VOLUME] = volume;
    appconfig[KEY_BRIGHTNESS_SB] = brightness_sb;
    appconfig[KEY_BRIGHTNESS_ACT] = brightness_act;
    /* Static info - which version created the config file*/
    appconfig[KEY_VERSION] = PROJECT_VERSION;

    write_config_file(appconfig);
}

/*****************************************************************************/
void ConfigurationManager::write_config_file(const QJsonObject& appconfig) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto file_path = get_configuration_path();

    // Disconnect filewatcher while we are saving the file to avoid event loops
    if (!disconnect(fwConn)) {
        qCCritical(CLASS_LC) << "Disconnect failed!";
    }
    filewatcher.removePath(file_path);

    QSaveFile config_file(file_path);
    try {
        config_file.open(
            QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QJsonDocument doc(appconfig);
        config_file.write(doc.toJson());
        config_file.commit();
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
    // Reconnect filewatcher to be notified if someone else changes file
    fwConn = connect(&filewatcher, &QFileSystemWatcher::fileChanged, this,
        &ConfigurationManager::fileChanged);
    if (!fwConn) {
        qCCritical(CLASS_LC) << "reconnect failed!";
    }
    filewatcher.addPath(file_path);
}

/*****************************************************************************/
void ConfigurationManager::create_default_configuration() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::fromString("06:30", "hh:mm"));
    alm->set_period(Alarm::Workdays);
    alm->set_id(QDateTime::currentMSecsSinceEpoch());
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
QDir ConfigurationManager::make_sure_config_path_exists() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (!config_dir.mkpath(".")) {
        qCCritical(CLASS_LC) << "Cannot create configuration path!";
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory),
            "Cannot create configuration path!");
    }
    return config_dir;
}

/*****************************************************************************/

QString ConfigurationManager::get_configuration_path() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return config_dir.filePath(CONFIG_JSON_FILE_NAME);
}

/*****************************************************************************/

QString ConfigurationManager::check_and_create_config() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // check if file exists -> assume some default config and write file
    auto path = get_configuration_path();
    QFile config_file(path);
    if (!config_file.exists()) {
    	qCInfo(CLASS_LC) << "Creating default config";
        create_default_configuration();
    }
    return path;
}

/*****************************************************************************/
int ConfigurationManager::delete_alarm(qint64 id) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto old_end = alarms.end();
    alarms.erase(std::remove_if(
        alarms.begin(), alarms.end(), [&](const std::shared_ptr<Alarm> item) {
            return item->get_id() == id;
        }),alarms.end());
    if(old_end == alarms.end()){
    	return -1;
    }
    dataChanged();
    return 0;
};
/*****************************************************************************/
