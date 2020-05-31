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

#include <QDateTime>
#include <QLoggingCategory>
#include <QMediaPlayer>
#include <QStandardPaths>

#include <algorithm>
#include <memory>

#include "PodcastSource.hpp"
#include "appconstants.hpp"
#include "httpclient.hpp"
#include "podcast_serializer.hpp"
#include "util.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSource");

/*****************************************************************************/
PodcastSource::PodcastSource(const QUrl& url, QUuid uid)
    : id(uid)
    , rss_feed_uri(url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
PodcastSource::~PodcastSource() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (download_cnx)
        disconnect(download_cnx);

    if (icon_downloader) {
        icon_downloader.get()->deleteLater();
        icon_downloader.release();
    }
}

/*****************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> newep) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (episodes.size() >= max_episodes) {
        qInfo(CLASS_LC) << " > max episodes reached: " << max_episodes;
        return;
    }
    auto ep = get_episode_by_id(newep->get_guid());
    /* add if not found */
    if (!ep) {
        qCDebug(CLASS_LC) << " > new Episode :" << newep->get_guid();
        // insert sorted by publication date
        auto iterator =
            std::lower_bound(episodes.begin(), episodes.end(), newep,
                [](const std::shared_ptr<PodcastEpisode>& lhs,
                    const std::shared_ptr<PodcastEpisode>& rhs) {
                    return lhs->get_publication_date() >
                        rhs->get_publication_date();
                });
        episodes.insert(iterator, newep);
        /* get notified if any data changes */
        connect(newep.get(), SIGNAL(data_changed()), this,
            SLOT(episode_info_changed()));
        emit episodes_count_changed(episodes.size());
    } else {
        qCDebug(CLASS_LC) << " > " << newep->get_guid() << "already in list";
    }
}

/*****************************************************************************/
void PodcastSource::set_update_interval(std::chrono::seconds interval) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    update_interval = interval;
    if (updater != nullptr) {
        updater->set_update_interval(interval);
    }
}

/*****************************************************************************/
void PodcastSource::set_description(QString newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    description = newVal;
    emit descriptionChanged();
    emit dataChanged();
}

/*****************************************************************************/
void PodcastSource::set_last_updated(QDateTime newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    last_updated = newVal;
}

/*****************************************************************************/
void PodcastSource::set_link(QUrl newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    link = newVal;
    emit dataChanged();
}

/*****************************************************************************/
void PodcastSource::set_max_episodes(int max) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (max >= 0)
        max_episodes = max;
}

/*****************************************************************************/
void PodcastSource::set_title(QString newTitle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    title = newTitle;
    emit titleChanged();
    emit dataChanged();
}

/*****************************************************************************/
QString PodcastSource::get_cache_file_name() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return get_cache_file_impl();
}

/*****************************************************************************/
QString PodcastSource::get_cache_file_impl() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return get_id().toString();
}

/*****************************************************************************/
void PodcastSource::set_update_task(std::unique_ptr<UpdateTask>&& ut) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    updater = std::move(ut);
    // make sure updater is not constructed with another podcastsource
    updater->set_podcast_source(this);
    updater->set_update_interval(update_interval);
}

/*****************************************************************************/
void PodcastSource::set_serializer(std::unique_ptr<PodcastSerializer>&& pser) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // We just take ownership
    serializer = std::move(pser);
    serializer->set_podcast_source(this);
    // Setup connections
    connect(this, &PodcastSource::dataChanged, serializer.get(),
        &PodcastSerializer::delayed_write);
}

/*****************************************************************************/
std::vector<QString> PodcastSource::get_episodes_names() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto ret = std::vector<QString>();
    for (const auto& e : episodes) {
        ret.push_back(e->get_display_name());
    }
    return ret;
}

/*****************************************************************************/
void PodcastSource::refresh() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    updater->start();
}

/*****************************************************************************/
void PodcastSource::purge_icon_cache() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /* remove local image cache */
    if (image_file_path.isEmpty()) {
        return;
    }
    QFile img_file(image_file_path);
    if (!img_file.remove()) {
        qCWarning(CLASS_LC) << " removing image cache failed "
                            << image_file_path << ":" << img_file.errorString();
    }
    image_file_path.clear(); // reset image file
}

/*****************************************************************************/
void PodcastSource::purge_episodes() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    episodes.clear();
    emit episodes_count_changed(episodes.size());
}

/*****************************************************************************/
void PodcastSource::purge() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    purge_episodes();
    purge_icon_cache();
    // Remove cache file
    if (serializer) {
        serializer->delete_cached_info();
    }
}

/*****************************************************************************/
void PodcastSource::episode_info_changed() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    emit dataChanged();
}

/*****************************************************************************/
std::shared_ptr<PodcastEpisode> PodcastSource::get_episode_by_id(
    const QString& id) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return get_episode_by_id_impl(id);
}

/*****************************************************************************/
std::shared_ptr<PodcastEpisode> PodcastSource::get_episode_by_id_impl(
    const QString& id) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto ep = std::find_if(episodes.begin(), episodes.end(),
        [id](std::shared_ptr<PodcastEpisode> episode) {
            return episode->get_guid() == id;
        });

    if (ep == episodes.end()) {
        qCDebug(CLASS_LC) << id << "not in vector";
        return std::shared_ptr<PodcastEpisode>();
    }
    return *ep;
}

/*****************************************************************************/
const std::vector<std::shared_ptr<PodcastEpisode>>&
PodcastSource::get_episodes_impl() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return episodes;
}

/*****************************************************************************/
int DigitalRooster::PodcastSource::get_episode_count_impl() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return get_episodes().size();
}

/*****************************************************************************/
void PodcastSource::set_image_url(const QUrl& uri) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (uri != icon_url) {
        icon_url = uri;
        emit icon_changed();
        emit dataChanged();
    }
    /* if we don't have a local copy or the file name changed  - download it*/
    if (!QFile(image_file_path).exists() ||
        QUrl::fromLocalFile(image_file_path).fileName() != uri.fileName()) {
        trigger_image_download();
    }
}
/*****************************************************************************/
void PodcastSource::set_image_file_path(const QString& path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /* clean previous cache file */
    QFile oldfile(image_file_path);
    if(oldfile.exists()){
    	oldfile.remove();
    }

    image_file_path = path;
    emit icon_changed();
    emit dataChanged();
}

/*****************************************************************************/
void PodcastSource::trigger_image_download() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    icon_downloader = std::make_unique<HttpClient>();
    download_cnx = connect(icon_downloader.get(), &HttpClient::dataAvailable,
        serializer.get(), &PodcastSerializer::store_image);
    icon_downloader->doDownload(icon_url);
}

/*****************************************************************************/
QUrl PodcastSource::get_icon_impl() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /* If we find a cache file, return it, otherwise the url must do */
    if (QFile(image_file_path).exists()) {
        qCDebug(CLASS_LC) << "found cached icon:" << image_file_path;
        return QUrl::fromLocalFile(image_file_path);
    } else {
        qCDebug(CLASS_LC) << "start download for icon cache:";
        /* Download for next time */
        trigger_image_download();
        return icon_url;
    }
}

/*****************************************************************************/
std::shared_ptr<PodcastSource> PodcastSource::from_json_object(
    const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (json.isEmpty()) {
        throw std::invalid_argument("Empty PodcastSource JSON object!");
    }

    auto id = valid_uuid_from_String(
        json[KEY_ID].toString(QUuid::createUuid().toString()));

    auto url = valid_url_from_string(json[KEY_URI].toString());
    auto ps = std::make_shared<PodcastSource>(url, id);

    auto title = json[KEY_TITLE].toString();
    auto desc = json[KEY_DESCRIPTION].toString();
    auto img_url = json[KEY_ICON_URL].toString();
    auto img_cached = json[KEY_IMAGE_CACHE].toString();

    ps->set_title(title);
    ps->set_description(desc);
    ps->set_image_url(img_url);
    ps->set_image_file_path(img_cached);
    ps->set_update_interval(
        std::chrono::seconds(json[KEY_UPDATE_INTERVAL].toInt(3600)));
    ps->set_update_task(std::make_unique<UpdateTask>(ps.get()));
    return ps;
}

/*****************************************************************************/
QJsonObject PodcastSource::to_json_object() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject json;
    json[KEY_ID] = get_id_string();
    json[KEY_URI] = get_url().toString();
    json[KEY_TITLE] = get_title();
    json[KEY_UPDATE_INTERVAL] =
        static_cast<qint64>(get_update_interval().count());
    return json;
}

/*****************************************************************************/
