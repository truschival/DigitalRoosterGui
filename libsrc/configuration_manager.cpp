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
bool DigitalRooster::is_writable_directory(const QString& dirname) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QFileInfo file_info(dirname);
    if (file_info.isDir() && file_info.isWritable()) {
        return true;
    }
    return false;
}

/*****************************************************************************/
bool DigitalRooster::create_writable_directory(const QString& dirname) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QFileInfo file_info(dirname);
    auto path = QDir(file_info.dir());
    path.mkpath(dirname);
    return is_writable_directory(dirname);
}

/*****************************************************************************/
ConfigurationManager::ConfigurationManager(
    const QString& configpath, const QString& cachedir)
    : global_alarm_timeout(DEFAULT_ALARM_TIMEOUT)
    , sleep_timeout(DEFAULT_SLEEP_TIMEOUT)
    , volume(DEFAULT_VOLUME)
    , brightness_sb(DEFAULT_BRIGHTNESS)
    , brightness_act(DEFAULT_BRIGHTNESS)
    , config_file(configpath)
    , application_cache_dir(cachedir)
    , wpa_socket_name(WPA_CONTROL_SOCKET_PATH) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /*
     * check if the cache directory exists and is writable or if it can be
     * created
     */
    if (!is_writable_directory(cachedir) &&
        !create_writable_directory(cachedir)) {
        qCWarning(CLASS_LC)
            << "Failed using " << get_cache_dir_name()
            << "as cache using default:" << DEFAULT_CACHE_DIR_PATH;
        application_cache_dir.setPath(DEFAULT_CACHE_DIR_PATH);
        QDir().mkpath(DEFAULT_CACHE_DIR_PATH);
    }


    // Check or create config dir
    QFileInfo config_file_info(configpath);
    QString config_dir = config_file_info.dir().absolutePath();
    if (!is_writable_directory(config_dir) &&
        !create_writable_directory(config_dir)) {
        config_file = DEFAULT_CONFIG_FILE_PATH;
        qCWarning(CLASS_LC)
            << "directory of config file does not exist or is not writable"
            << "using default path" << config_file;
    }

    auto path = check_and_create_config();
    filewatcher.addPath(path);

    writeTimer.setInterval(std::chrono::seconds(5));
    writeTimer.setSingleShot(true);
    connect(&writeTimer, SIGNAL(timeout()), this, SLOT(store_current_config()));
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
    auto content = get_json_from_file(get_configuration_path());
    parse_json(content.toUtf8());
    emit configuration_changed();
}

/*****************************************************************************/
QString ConfigurationManager::get_json_from_file(const QString& path) {
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
void ConfigurationManager::parse_json(const QByteArray& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject appconfig = doc.object();
    /* get application config */
    auto at = appconfig[DigitalRooster::KEY_ALARM_TIMEOUT];
    if (!at.isUndefined()) {
        global_alarm_timeout =
            std::chrono::minutes(at.toInt(DEFAULT_ALARM_TIMEOUT.count()));
    } else {
        global_alarm_timeout =
            std::chrono::minutes(DEFAULT_ALARM_TIMEOUT.count());
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

    auto sleep_to = appconfig[KEY_SLEEP_TIMEOUT];
    if (!sleep_to.isUndefined()) {
        set_sleep_timeout(std::chrono::minutes(
            sleep_to.toInt(DEFAULT_SLEEP_TIMEOUT.count())));
    }

    auto wpa_sock = appconfig[KEY_WPA_SOCKET_NAME];
    wpa_socket_name = wpa_sock.toString(WPA_CONTROL_SOCKET_PATH);
    read_radio_streams_from_file(appconfig);
    read_podcasts_from_file(appconfig);
    read_alarms_from_file(appconfig);
    read_weather_from_file(appconfig);
}

/*****************************************************************************/
void ConfigurationManager::read_radio_streams_from_file(
    const QJsonObject& appconfig) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonArray stations =
        appconfig[DigitalRooster::KEY_GROUP_IRADIO_SOURCES].toArray();
    for (const auto json_station : stations) {
        auto station = json_station.toObject();
        QString name(station[KEY_NAME].toString());
        QUrl url(station[KEY_URI].toString());
        auto uid = QUuid::fromString(
            station[KEY_ID].toString(QUuid::createUuid().toString()));
        if (!url.isValid()) {
            qCWarning(CLASS_LC)
                << "Invalid URI " << station[KEY_URI].toString();
            continue;
        }
        stream_sources.push_back(
            std::make_shared<PlayableItem>(name, url, uid));
    }
    /* Sort alphabetically */
    std::sort(stream_sources.begin(), stream_sources.end(),
        [](const std::shared_ptr<PlayableItem>& lhs,
            const std::shared_ptr<PlayableItem>& rhs) {
            return lhs->get_display_name() < rhs->get_display_name();
        });
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
        auto uid = QUuid::fromString(
            jo[KEY_ID].toString(QUuid::createUuid().toString()));
        auto ps =
            std::make_shared<PodcastSource>(url, application_cache_dir, uid);
        auto title = jo[KEY_NAME].toString();
        ps->set_title(title);
        ps->set_update_interval(
            std::chrono::seconds(jo[KEY_UPDATE_INTERVAL].toInt(3600)));
        ps->set_update_task(std::make_unique<UpdateTask>(ps.get()));

        // Get notifications if name etc. changes
        connect(ps.get(), &PodcastSource::dataChanged, this,
            &ConfigurationManager::dataChanged);

        podcast_sources.push_back(ps);
    }
    std::sort(podcast_sources.begin(), podcast_sources.end(),
        [](const std::shared_ptr<PodcastSource>& lhs,
            const std::shared_ptr<PodcastSource>& rhs) {
            return lhs->get_title() < rhs->get_title();
        });
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
        // sane default for periodicity
        auto period = Alarm::Daily;
        try {
            period = json_string_to_alarm_period(
                json_alarm[KEY_ALARM_PERIOD].toString(KEY_ALARM_DAILY));
        } catch (std::invalid_argument& exc) {
            qCWarning(CLASS_LC) << "invalid periodicity entry! " << exc.what();
        }

        auto enabled = json_alarm[KEY_ENABLED].toBool(true);
        auto media = QUrl(json_alarm[KEY_URI].toString());

        auto timepoint =
            QTime::fromString(json_alarm[KEY_TIME].toString(), "hh:mm");
        auto id = QUuid::fromString(
            json_alarm[KEY_ID].toString(QUuid::createUuid().toString()));
        /*
         * Create alarm with essential information
         */
        auto alarm =
            std::make_shared<Alarm>(media, timepoint, period, enabled, id);

        auto volume = json_alarm[KEY_VOLUME].toInt(DEFAULT_ALARM_VOLUME);
        alarm->set_volume(volume);
        /* if no specific alarm timeout is given take application default */
        auto timeout =
            json_alarm[KEY_ALARM_TIMEOUT].toInt(global_alarm_timeout.count());
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
    dataChanged();
    emit stations_changed();
}

/*****************************************************************************/
const PlayableItem* ConfigurationManager::get_stream_source(
    const QUuid& id) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto item = std::find_if(stream_sources.begin(), stream_sources.end(),
        [&](const std::shared_ptr<PlayableItem> item) {
            return item->get_id() == id;
        });
    if (item == stream_sources.end()) {
    	throw std::out_of_range("");
    }
    return item->get();
}

/*****************************************************************************/
void ConfigurationManager::add_podcast_source(
    std::shared_ptr<PodcastSource> src) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->podcast_sources.push_back(src);
    dataChanged();
    emit podcast_sources_changed();
}

/*****************************************************************************/
const PodcastSource* ConfigurationManager::get_podcast_source(
    const QUuid& id) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto item = std::find_if(podcast_sources.begin(), podcast_sources.end(),
        [&](const std::shared_ptr<PodcastSource> item) {
            return item->get_id() == id;
        });
    if (item == podcast_sources.end()) {
        throw std::out_of_range("");
    }
    return item->get();
}

/*****************************************************************************/
void ConfigurationManager::add_alarm(std::shared_ptr<Alarm> alm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->alarms.push_back(alm);
    dataChanged();
    emit alarms_changed();
}

/*****************************************************************************/
const Alarm* ConfigurationManager::get_alarm(const QUuid& id) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto item = std::find_if(
        alarms.begin(), alarms.end(), [&](const std::shared_ptr<Alarm> item) {
            return item->get_id() == id;
        });
    if (item == alarms.end()) {
    	throw std::out_of_range("");
    }
    return item->get();
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
    return do_set_brightness_act(brightness);
}

/*****************************************************************************/
void ConfigurationManager::do_set_brightness_act(int brightness) {
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
        psconfig[KEY_ID] = ps->get_id().toString();
        podcasts.append(psconfig);
    }
    appconfig[KEY_GROUP_PODCAST_SOURCES] = podcasts;

    QJsonArray iradios;
    for (const auto& iradiostream : stream_sources) {
        QJsonObject irconfig;
        irconfig[KEY_NAME] = iradiostream->get_display_name();
        irconfig[KEY_URI] = iradiostream->get_url().toString();
        irconfig[KEY_ID] = iradiostream->get_id().toString();
        iradios.append(irconfig);
    }
    appconfig[KEY_GROUP_IRADIO_SOURCES] = iradios;

    QJsonArray alarms_json;
    for (const auto& alarm : alarms) {
        QJsonObject alarmcfg;
        alarmcfg[KEY_ID] = alarm->get_id().toString();
        try {
            alarmcfg[KEY_ALARM_PERIOD] =
                alarm_period_to_json_string(alarm->get_period());
        } catch (std::invalid_argument& exc) {
            qCCritical(CLASS_LC) << " invalid period " << alarm->get_period()
                                 << " using default " << KEY_ALARM_ONCE;
            alarmcfg[KEY_ALARM_PERIOD] = KEY_ALARM_ONCE;
        }
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
    appconfig[KEY_ALARM_TIMEOUT] =
        static_cast<qint64>(global_alarm_timeout.count());
    appconfig[KEY_SLEEP_TIMEOUT] = static_cast<qint64>(sleep_timeout.count());
    appconfig[KEY_WPA_SOCKET_NAME] = wpa_socket_name;
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
        QTime::fromString("06:30", "hh:mm"), Alarm::Workdays);
    alarms.push_back(alm);

    /* Podcasts */
    podcast_sources.push_back(std::make_shared<PodcastSource>(
        QUrl("http://armscontrolwonk.libsyn.com/rss"), application_cache_dir));

    podcast_sources.push_back(std::make_shared<PodcastSource>(
        QUrl("https://rss.acast.com/mydadwroteaporno"), application_cache_dir));

    podcast_sources.push_back(std::make_shared<PodcastSource>(
        QUrl("https://alternativlos.org/alternativlos.rss"),
        application_cache_dir));

    podcast_sources.push_back(std::make_shared<PodcastSource>(
        QUrl("http://www.podcastone.com/podcast?categoryID2=1225"),
        application_cache_dir));

    /* Radio Streams */
    stream_sources.push_back(std::make_shared<PlayableItem>("Deutschlandfunk",
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg")));

    stream_sources.push_back(
        std::make_shared<PlayableItem>("Deutschlandfunk Nova",
            QUrl("http://st03.dlf.de/dlf/03/104/ogg/stream.ogg")));

    stream_sources.push_back(std::make_shared<PlayableItem>("SWR2",
        QUrl("http://swr-swr2-live.cast.addradio.de/swr/swr2/live/mp3/256/"
             "stream.mp3")));

    stream_sources.push_back(std::make_shared<PlayableItem>(
        "FM4", QUrl("https://fm4shoutcast.sf.apa.at")));

    stream_sources.push_back(
        std::make_shared<PlayableItem>("BBC World Service News",
            QUrl("http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-einws")));

    stream_sources.push_back(std::make_shared<PlayableItem>("BBC World Service",
        QUrl("http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk")));

    store_current_config();
}

/*****************************************************************************/
QString ConfigurationManager::get_configuration_path() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return config_file;
}

/*****************************************************************************/
QString ConfigurationManager::get_wpa_socket_name() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return wpa_socket_name;
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
PodcastSource* ConfigurationManager::get_podcast_source_by_index(
    int index) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return podcast_sources.at(index).get();
}

/*****************************************************************************/
void ConfigurationManager::remove_podcast_source_by_index(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    podcast_sources.remove(index);
    writeTimer.start(); // start delayed write
}

/*****************************************************************************/
void ConfigurationManager::delete_alarm(const QUuid& id) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto old_end = alarms.end();
    alarms.erase(std::remove_if(alarms.begin(), alarms.end(),
                     [&](const std::shared_ptr<Alarm> item) {
                         return item->get_id() == id;
                     }),
        alarms.end());
    if (old_end == alarms.end()) {
    	throw std::out_of_range("");
    }
    dataChanged();
    emit alarms_changed();
};

/*****************************************************************************/
void ConfigurationManager::delete_podcast_source(const QUuid& id) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto old_end = podcast_sources.end();
    podcast_sources.erase(
        std::remove_if(podcast_sources.begin(), podcast_sources.end(),
            [&](const std::shared_ptr<PodcastSource> item) {
                return item->get_id() == id;
            }),
        podcast_sources.end());
    if (old_end == podcast_sources.end()) {
        throw std::out_of_range("");
    }
    dataChanged();
    emit podcast_sources_changed();
};

/*****************************************************************************/
void ConfigurationManager::delete_radio_station(const QUuid& id) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto old_end = stream_sources.end();
    stream_sources.erase(
        std::remove_if(stream_sources.begin(), stream_sources.end(),
            [&](const std::shared_ptr<PlayableItem> item) {
                return item->get_id() == id;
            }),
        stream_sources.end());
    if (old_end == stream_sources.end()) {
    	throw std::out_of_range("");
    }
    dataChanged();
    emit stations_changed();
};

/*****************************************************************************/
std::chrono::minutes ConfigurationManager::get_sleep_timeout() const {
    return sleep_timeout;
}

/*****************************************************************************/
void ConfigurationManager::set_sleep_timeout(std::chrono::minutes timeout) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    sleep_timeout = timeout;
    writeTimer.start();
}

/*****************************************************************************/
QString ConfigurationManager::get_cache_dir_name() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return application_cache_dir.path();
}

/*****************************************************************************/
