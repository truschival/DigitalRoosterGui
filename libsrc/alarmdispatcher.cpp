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
#include <QVector>
#include <chrono>
#include <memory>

#include "IAlarmStore.hpp"
#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "mediaplayerproxy.hpp"
#include "timeprovider.hpp"

using namespace DigitalRooster;
using namespace std::chrono;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.AlarmDispatcher");

/*****************************************************************************/

AlarmDispatcher::AlarmDispatcher(IAlarmStore& store, QObject* parent)
    : QObject(parent)
    , cm(store)
    , upcoming_alarm(get_upcoming_alarm()) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, this, &AlarmDispatcher::trigger);
}

/*****************************************************************************/
void AlarmDispatcher::check_alarms() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto now = wallclock->now();
    upcoming_alarm = get_upcoming_alarm();

    // No alarms or all alarms are disabled
    if (!upcoming_alarm || !upcoming_alarm->is_enabled()) {
        timer.stop();
        emit upcoming_alarm_info_changed("");
        return;
    }

    // emit string
    auto alm_dt = get_next_instance(*upcoming_alarm);
    emit upcoming_alarm_info_changed(alm_dt.toString("ddd hh:mm"));

    auto delta = alm_dt.toMSecsSinceEpoch() - now.toMSecsSinceEpoch();
    timer.start(delta);
}

/*****************************************************************************/
void AlarmDispatcher::trigger() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (upcoming_alarm && upcoming_alarm->is_enabled()) {
        dispatch(upcoming_alarm);
    }

    /* Check for next upcoming alarm */
    check_alarms();
}

/*****************************************************************************/
void AlarmDispatcher::dispatch(std::shared_ptr<Alarm> alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << alarm->get_id();
    emit alarm_triggered(alarm);
    emit alarm_triggered();
}
/*****************************************************************************/
std::chrono::milliseconds AlarmDispatcher::get_remaining_time() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto r = timer.remainingTimeAsDuration();
    return r;
}

/*****************************************************************************/
std::shared_ptr<Alarm> AlarmDispatcher::get_upcoming_alarm() {
    // Sort a copy of the alarms not to mess up Alarm list in QML
    auto v = std::vector(cm.get_alarms());
    if (v.size() < 1)
        return nullptr;

    std::sort(v.begin(), v.end(),
        [](std::shared_ptr<Alarm> lhs, std::shared_ptr<Alarm> rhs) {
            return *lhs.get() < *rhs.get();
        });
    return v[0];
}

/*****************************************************************************/
QString AlarmDispatcher::get_upcoming_alarm_info() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString ret;

    if (upcoming_alarm && upcoming_alarm->is_enabled()) {
        auto alm_dt = get_next_instance(*upcoming_alarm);
        ret = alm_dt.toString("ddd hh:mm");
    }
    return ret;
}
