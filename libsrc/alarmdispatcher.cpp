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
#include <QLoggingCategory>
#include <QMediaPlayer>
#include <chrono>
#include <memory>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "configuration_manager.hpp"
#include "mediaplayerproxy.hpp"
#include "timeprovider.hpp"

using namespace DigitalRooster;
using namespace std::chrono;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.AlarmDispatcher");

/*****************************************************************************/

AlarmDispatcher::AlarmDispatcher(
    std::shared_ptr<ConfigurationManager> confman, QObject* parent)
    : QObject(parent)
    , cm(confman)
    , interval(std::chrono::seconds(30)) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    interval_timer.setInterval(duration_cast<milliseconds>(interval));
    interval_timer.setSingleShot(false);
    connect(&interval_timer, SIGNAL(timeout()), this, SLOT(check_alarms()));
    interval_timer.start();
}

/*****************************************************************************/
void AlarmDispatcher::check_alarms() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto now = wallclock->now();
    auto dow = now.date().dayOfWeek();
    for (const auto& alarm : cm->get_alarms()) {
        /* only check period if enabled and due within the interval */
        if (!alarm->is_enabled() ||
            std::abs(now.time().secsTo(alarm->get_time())) > interval.count())
            continue;

        /* Check if today is the day */
        switch (alarm->get_period()) {
        case Alarm::Once:
            emit alarm_triggered(alarm);
            /* disable the next time*/
            alarm->enable(false);
            break;
        case Alarm::Daily:
            /* always trigger */
            emit alarm_triggered(alarm);
            break;
        case Alarm::Weekend:
            if (dow > Qt::Friday) {
                emit alarm_triggered(alarm);
            }
            break;
        case Alarm::Workdays:
            if (dow < Qt::Saturday) {
                emit alarm_triggered(alarm);
            }
        }
    }
}

/*****************************************************************************/
void AlarmDispatcher::set_interval(std::chrono::seconds iv) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    interval = iv;
    interval_timer.setInterval(
        std::chrono::duration_cast<std::chrono::milliseconds>(iv));
}

/*****************************************************************************/
std::chrono::seconds AlarmDispatcher::get_interval() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return interval;
}
