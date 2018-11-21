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

#include "PodcastSource.hpp"
#include <QDateTime>
#include <QLoggingCategory>
#include <QMediaPlayer>
#include <algorithm>
using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSource");

/*****************************************************************************/
PodcastSource::PodcastSource(const QUrl& url, qint64 uid)
    : id(uid)
    , rss_feed_uri(url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> newep) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (episodes.size() < max_episodes) {
        auto ep = std::find_if(episodes.begin(), episodes.end(),
            [newep](std::shared_ptr<PodcastEpisode> episode) {
                return episode->get_guid() == newep->get_guid();
            });

        if (ep == episodes.end()) {
            episodes.push_back(newep);
            emit episodesChanged();
        }
    } else {
        qInfo(CLASS_LC) << "max episodes reached: " << max_episodes;
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
