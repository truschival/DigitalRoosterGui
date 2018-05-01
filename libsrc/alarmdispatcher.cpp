/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#include <QDebug>
#include <QMediaPlayer>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "configuration_manager.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

/***********************************************************************/

AlarmDispatcher::AlarmDispatcher(ConfigurationManager* confman, QObject* parent)
    : QObject(parent)
    , cm(confman) {
    interval_timer.setInterval(interval);
    interval_timer.setSingleShot(false);
    connect(&interval_timer, SIGNAL(timeout()), this, SLOT(check_alarms()));
    interval_timer.start();
}

/*****************************************************************************/
void AlarmDispatcher::check_alarms() {
    qDebug() << Q_FUNC_INFO << QDateTime::currentDateTime();
    auto now = QDateTime::currentDateTime();
    for (const auto& alarm : cm->get_alarms()) {
        auto delta = now.secsTo(alarm->get_next_trigger());
        if (alarm->is_enabled() && (delta * 1000) < interval + 1) {
            qDebug() << "Dispatching Alarm";
            emit alarm_triggered(alarm);
        }
    }
}