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

#include <QCryptographicHash>
#include <cstdio>
#include <stdexcept>

#include "DownloadManager.hpp"
#include "PodcastSource.hpp"
#include "PodcastSourceReader.hpp"
#include "UpdateTask.hpp"

using namespace DigitalRooster;

/*****************************************************************************/

UpdateTask::UpdateTask(PodcastSource* source)
    : ps(source) {

	connect(&dlm, SIGNAL(dataAvailable(const QByteArray)), this,
        SLOT(dataAvailable(const QByteArray)));
	
	// Start timer
	timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(start()));
    if (ps) {
        timer.start(source->get_update_interval()); 
	}
	// initial download immediately
    start();
}

/*****************************************************************************/

void UpdateTask::dataAvailable(const QByteArray& data) {
    update_podcast(*ps, data);
}


/*****************************************************************************/
void UpdateTask::set_podcast_source(PodcastSource* ps) {
    this->ps = ps;
}

/*****************************************************************************/

void UpdateTask::start() {
    if (ps) {
        dlm.doDownload(ps->get_url());
	}
}

/*****************************************************************************/
void UpdateTask::set_update_interval(std::chrono::seconds interval) {
	timer.start(interval);
}