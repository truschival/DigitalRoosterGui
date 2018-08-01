/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QDebug>
#include <QMediaPlayer>
#include <chrono>
#include <memory>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "configuration_manager.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;
using namespace std::chrono;

/*****************************************************************************/

AlarmDispatcher::AlarmDispatcher(
    std::shared_ptr<ConfigurationManager> confman, QObject* parent)
    : QObject(parent)
    , cm(confman)
    , interval(std::chrono::seconds(30)) {
    interval_timer.setInterval(duration_cast<milliseconds>(interval));
    interval_timer.setSingleShot(false);
    connect(&interval_timer, SIGNAL(timeout()), this, SLOT(check_alarms()));
    interval_timer.start();
}

/*****************************************************************************/
void AlarmDispatcher::check_alarms() {
    // qDebug() << Q_FUNC_INFO << QDateTime::currentDateTime();
    auto now = QDateTime::currentDateTime();
    for (const auto& alarm : cm->get_alarms()) {
        auto delta = seconds(now.secsTo(alarm->get_next_trigger()));
        if (alarm->is_enabled() && delta <= interval) {
            // qDebug() << "Dispatching Alarm";
            emit alarm_triggered(alarm);
        }
    }
}

/*****************************************************************************/
void AlarmDispatcher::set_interval(std::chrono::seconds iv) {
    interval = iv;
    interval_timer.setInterval(
        std::chrono::duration_cast<std::chrono::milliseconds>(iv));
}

/*****************************************************************************/
std::chrono::seconds AlarmDispatcher::get_interval() {
    return interval;
}
