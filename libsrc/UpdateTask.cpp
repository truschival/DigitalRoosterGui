// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>

#include "PodcastSource.hpp"
#include "UpdateTask.hpp"
#include "rss2podcastsource.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.UpdateTask");

/*****************************************************************************/

UpdateTask::UpdateTask(PodcastSource* source)
    : ps(source) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    connect(&dlm, &HttpClient::dataAvailable, this, &UpdateTask::dataAvailable);

    // Start timer
    timer.setSingleShot(false);
    connect(&timer, &QTimer::timeout, this, &UpdateTask::start);
    if (ps) {
        timer.start(source->get_update_interval());
    }
    // initial download immediately
    start();
}

/*****************************************************************************/

void UpdateTask::dataAvailable(const QByteArray& data) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    update_podcast(*ps, data);
}


/*****************************************************************************/
void UpdateTask::set_podcast_source(PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->ps = ps;
}

/*****************************************************************************/

void UpdateTask::start() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ps) {
        dlm.doDownload(ps->get_url());
    }
}

/*****************************************************************************/
void UpdateTask::set_update_interval(std::chrono::seconds interval) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    timer.start(interval);
}
