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
#include <QDebug>
#include <algorithm>

using namespace DigitalRooster;

/*****************************************************************************/
PodcastSource::PodcastSource(const QUrl& url)
    : rss_feed_uri(url) {
}

/*****************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> newep) {
    if (episodes.size() < max_episodes) {
        auto ep = std::find_if(episodes.begin(), episodes.end(),
            [newep](std::shared_ptr<PodcastEpisode> episode) {
                return episode->get_guid() == newep->get_guid();
            });

        if (ep == episodes.end()) {
            episodes.push_back(newep);
            emit episodesChanged();
        }
    }
}
/*****************************************************************************/
void PodcastSource::set_update_interval(std::chrono::seconds interval) {
    update_interval = interval;
    if (updater != nullptr) {
        updater->set_update_interval(interval);
    }
}

/*****************************************************************************/
void PodcastSource::set_update_task(std::unique_ptr<UpdateTask>&& ut) {
    updater = std::move(ut);
    // make sure updater is not constructed with another podcastsource
    updater->set_podcast_source(this); 
    updater->set_update_interval(update_interval);
}

/*****************************************************************************/
QVector<QString> PodcastSource::get_episodes_names() {
    auto ret = QVector<QString>();
    for (const auto& e : episodes) {
        ret.push_back(e->get_display_name());
    }
    return ret;
}
