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

#include "PodcastSource.hpp"
#include <QDebug>
#include <algorithm>

using namespace DigitalRooster;

/*****************************************************************************/
PodcastSource::PodcastSource(const QUrl& url) :
		rss_feed_uri(url), updater(std::make_unique<UpdateTask>(*this)) {
	connect(updater.get(), SIGNAL(newDataAvailable()), this,
			SLOT(updateFinished()));
	connect(&timer, SIGNAL(timeout()), updater.get(), SLOT(start()));
	timer.start(update_interval);
	// initial download immediately
	updater->start();
}
;

/*****************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> newep) {
	if (episodes.size() < max_episodes) {
		auto ep = std::find_if(episodes.begin(), episodes.end(),
				[newep](std::shared_ptr<PodcastEpisode> episode) {
					// qDebug() << "Episode guid " << episode->get_guid() << "newep : "
					// <<newep->get_guid();
					return episode->get_guid() == newep->get_guid();
				});

		if (ep == episodes.end()) {
			episodes.push_back(newep);
			emit newDataAvailable();
		}
	}
}
/*****************************************************************************/
void PodcastSource::set_update_interval(int interval) {
	update_interval = interval;
	if (interval < timer.remainingTime())
		timer.start(update_interval);
}

/*****************************************************************************/
void PodcastSource::updateFinished() {
	emit newDataAvailable();
	timer.start(update_interval);
}

/*****************************************************************************/
QVector<QString> PodcastSource::get_episodes_names() {
	auto ret = QVector<QString>();
	for (const auto& e : episodes) {
		ret.push_back(e->get_display_name());
	}
	return ret;
}
