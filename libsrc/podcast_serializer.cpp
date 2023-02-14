// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QFile>
#include <QImage>
#include <QLoggingCategory>
#include <stdexcept>

#include "PodcastSource.hpp"
#include "appconstants.hpp"
#include "podcast_serializer.hpp"
#include "timeprovider.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSerializer");

/*****************************************************************************/
PodcastSerializer::PodcastSerializer(const QDir& app_cache_dir,
    PodcastSource* source, std::chrono::milliseconds delay)
    : ps(source)
    , cache_dir(app_cache_dir) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // write timer will take care for delayed writes
    writeTimer.setInterval(delay);
    writeTimer.setSingleShot(true);
    connect(&writeTimer, &QTimer::timeout, this, &PodcastSerializer::write);
}

/*****************************************************************************/
PodcastSerializer::~PodcastSerializer() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    writeTimer.stop();
}

/*****************************************************************************/
void PodcastSerializer::restore_info() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ps != nullptr) {
        // no braces in the file path
        auto cache_file = cache_dir.filePath(ps->get_id_string());
        try {
            read_from_file(ps, cache_file);
        } catch (std::system_error&) {
            qCWarning(CLASS_LC) << "Cache file not found" << cache_file;
        } catch (PodcastSourceJSonCorrupted& jsexc) {
            qCWarning(CLASS_LC) << "corrupted JSON in file" << cache_file
                                << ": " << jsexc.what();
        }
    }
}

/*****************************************************************************/
void PodcastSerializer::set_podcast_source(PodcastSource* source) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    ps = source;
}

/*****************************************************************************/
void PodcastSerializer::write() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ps != nullptr) {
        write_cache();
    }
}

/*****************************************************************************/
void PodcastSerializer::write_cache() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto cache_file = cache_dir.filePath(ps->get_id_string());
    store_to_file(ps, cache_file);
}

/*****************************************************************************/
void PodcastSerializer::delete_cached_info() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ps != nullptr) {
        delete_cache();
    }
}

/*****************************************************************************/
void PodcastSerializer::delete_cache() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QFile cache_file(cache_dir.filePath(ps->get_id_string()));
    cache_file.remove();
}

/*****************************************************************************/
void PodcastSerializer::delayed_write() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (!writeTimer.isActive()) {
        writeTimer.start(); // start delayed write
    }
}

/*****************************************************************************/
void PodcastSerializer::store_image(QByteArray data) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    store_image_impl(data);
}

/*****************************************************************************/
void PodcastSerializer::store_image_impl(QByteArray& data) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /* Resize image and save file */
    auto image_data = QImage::fromData(data);
    auto small_image = image_data.scaled(DEFAULT_ICON_WIDTH, DEFAULT_ICON_WIDTH,
        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    /* Do not rely on file format info or name from URL, save it as PNG */
    auto image_cache_file_name =
        ps->create_image_file_name(ps->get_image_url());

    auto image_file_path = cache_dir.filePath(image_cache_file_name);
    if (small_image.save(image_file_path)) {
        // Store image path if successful
        ps->set_image_file_path(image_file_path);
    } else {
        ps->set_image_file_path("");
        qCCritical(CLASS_LC) << "save image failed" << image_file_path;
    }
}

/*****************************************************************************/
void DigitalRooster::store_to_file(
    PodcastSource* ps, const QString& file_path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    QJsonObject ps_obj = json_from_podcast_source(ps);
    QJsonArray episodes;
    for (const auto& episode : ps->get_episodes()) {
        episodes.append(episode->to_json_object());
    }
    ps_obj[KEY_EPISODES] = episodes;

    QSaveFile cache_file(file_path);
    try {
        cache_file.open(
            QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QJsonDocument doc(ps_obj);
        cache_file.write(doc.toJson());
        cache_file.commit();
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
}

/*****************************************************************************/
void DigitalRooster::read_from_file(
    PodcastSource* ps, const QString& file_path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCCritical(CLASS_LC) << file.errorString();
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory),
            file.errorString().toStdString());
    }
    auto json_doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject tl_obj = json_doc.object();
    if (!tl_obj.isEmpty()) {
        parse_podcast_source_from_json(tl_obj, ps);
        read_episodes_cache(tl_obj, ps);
    } else {
        throw PodcastSourceJSonCorrupted("Document empty!");
    }
}

/*****************************************************************************/
void DigitalRooster::read_episodes_cache(
    const QJsonObject& tl_obj, PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto episodes_json_array = tl_obj[KEY_EPISODES].toArray();
    if (episodes_json_array.isEmpty()) {
        qCWarning(CLASS_LC)
            << "JSON for PodcastSource does not contain episodes";
        return;
    }
    /*
     * in either case read individual podcast episiode settings to at least set
     * episode positions
     */
    for (const auto& ep : episodes_json_array) {
        auto ep_ptr = PodcastEpisode::from_json_object(ep.toObject());
        auto existing_ep = ps->get_episode_by_id(ep_ptr->get_guid());
        if (existing_ep) {
            // found -> update position
            existing_ep->set_position(ep_ptr->get_position());
        } else {
            // not found -> add
            ps->add_episode(ep_ptr);
        }
    }
}

/*****************************************************************************/
void DigitalRooster::parse_podcast_source_from_json(
    const QJsonObject& tl_obj, PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // read podcast source configuration properties
    auto ts_str = tl_obj[KEY_TIMESTAMP].toString();
    QDateTime timestamp;
    if (!ts_str.isEmpty()) {
        timestamp = QDateTime::fromString(ts_str, Qt::ISODate);
    }
    if (!timestamp.isValid()) {
        qCCritical(CLASS_LC)
            << ts_str << "invalid timestamp - will not restore data";
        throw PodcastSourceJSonCorrupted(
            ts_str.toStdString() + " - invalid timestamp");
    }
    if (ps->get_last_updated().isValid() &&
        ps->get_last_updated() > timestamp) {
        qCDebug(CLASS_LC) << "PodcastSource newer than cache";
    } else {
        // need to update podcast source data
        auto title = tl_obj[JSON_KEY_TITLE].toString();
        auto desc = tl_obj[KEY_DESCRIPTION].toString();
        auto img_url = tl_obj[KEY_ICON_URL].toString();
        auto img_cached = tl_obj[KEY_IMAGE_CACHE].toString();
        ps->set_title(title);
        ps->set_description(desc);
        /* First set cached image to avoid extra download */
        ps->set_image_file_path(img_cached);
        ps->set_image_url(img_url);
    }
}

/*****************************************************************************/
QJsonObject DigitalRooster::json_from_podcast_source(const PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject ps_obj = ps->to_json_object();
    ps_obj[KEY_TIMESTAMP] = wallclock->now().toString(Qt::ISODate);
    ps_obj[KEY_DESCRIPTION] = ps->get_description();
    ps_obj[KEY_ICON_URL] = ps->get_image_url().toString();
    ps_obj[KEY_IMAGE_CACHE] = ps->get_image_file_path();
    return ps_obj;
}

/*****************************************************************************/
