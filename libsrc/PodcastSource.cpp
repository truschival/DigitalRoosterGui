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

#include "PodcastSource.hpp"
#include "podcast_serializer.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSource");

/*****************************************************************************/
PodcastSource::PodcastSource(const QUrl& url, QUuid uid)
    : id(uid)
    , rss_feed_uri(url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // read initial settings from file if it exists
    restore();
    // write timer will take care for delayed writes
    writeTimer.setInterval(std::chrono::seconds(5));
    writeTimer.setSingleShot(true);
    connect(&writeTimer, SIGNAL(timeout()), this, SLOT(store()));
}

/*****************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> newep) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (episodes.size() >= max_episodes) {
        qInfo(CLASS_LC) << "max episodes reached: " << max_episodes;
        return;
    }
    auto ep = get_episode_by_id(newep->get_guid());
    /* add if not found */
    if (!ep) {
        episodes.push_back(newep);
        writeTimer.start(); // start delayed write
        emit episodes_count_changed(episodes.size());
        /* get notified if any data changes */
        connect(ep.get(), SIGNAL(data_changed()), this,
            SLOT(episode_info_changed()));
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
    writeTimer.start(); // start delayed write
}

/*****************************************************************************/
void PodcastSource::set_last_updated(QDateTime newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    last_updated = newVal;
    writeTimer.start(); // start delayed write
}

/*****************************************************************************/
void PodcastSource::set_link(QUrl newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    link = newVal;
    emit dataChanged();
    writeTimer.start(); // start delayed write
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
    writeTimer.start(); // start delayed write
}

/*****************************************************************************/
QString PodcastSource::get_cache_file_name() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return get_cache_file_impl();
}

/*****************************************************************************/
QString PodcastSource::get_cache_file_impl() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString res(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    return res + QDir::separator() + get_id().toString();
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
QVector<QString> PodcastSource::get_episodes_names() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto ret = QVector<QString>();
    for (const auto& e : episodes) {
        ret.push_back(e->get_display_name());
    }
    return ret;
}

/*****************************************************************************/
void DigitalRooster::PodcastSource::restore() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    PodcastSerializer serializer;
    try {
        serializer.read_from_file(this);
    } catch (std::exception& exc) {
        qCWarning(CLASS_LC) << "restore failed" << exc.what();
    }
}

/*****************************************************************************/
void DigitalRooster::PodcastSource::store() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    PodcastSerializer serializer;
    try {
        serializer.store_to_file(this);
    } catch (std::exception& exc) {
        qCWarning(CLASS_LC) << "store failed" << exc.what();
    }
}

/*****************************************************************************/
void PodcastSource::refresh() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    updater->start();
}

/*****************************************************************************/
void PodcastSource::episode_info_changed() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    writeTimer.start(); // start delayed write
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
        qCDebug(CLASS_LC) << " episode with id:" << id << "not in vector";
        return std::shared_ptr<PodcastEpisode>();
    }
    return *ep;
}

/*****************************************************************************/
const QVector<std::shared_ptr<PodcastEpisode>>&
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
void PodcastSource::set_image_uri(const QUrl& uri) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    image_uri = uri;
}

/*****************************************************************************/
