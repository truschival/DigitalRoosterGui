// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>
#include <chrono>
#include <memory>

#include "IAlarmStore.hpp"
#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "timeprovider.hpp"

using namespace DigitalRooster;
using namespace std::chrono;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.AlarmDispatcher");

/*****************************************************************************/

AlarmDispatcher::AlarmDispatcher(IAlarmStore& store, QObject* parent)
    : QObject(parent)
    , config(store) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    timer.setSingleShot(true);
    /* Coarse timers with long intervals may fire a second early or late */
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer, &QTimer::timeout, this, &AlarmDispatcher::trigger);
    /* make sure alarms are updated and timer started */
    check_alarms();
}

/*****************************************************************************/
void AlarmDispatcher::check_alarms() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    upcoming_alarm = get_upcoming_alarm();
    // No alarms or all alarms are disabled
    if (!upcoming_alarm || !upcoming_alarm->is_enabled()) {
        scheduled_instance = QDateTime();
        emit upcoming_alarm_info_changed("");
        timer.stop();
    } else {
        scheduled_instance =
            get_next_instance(*upcoming_alarm, reference_time());
        emit upcoming_alarm_info_changed(
            scheduled_instance.toString("ddd hh:mm"));
        auto delta = wallclock->now().msecsTo(scheduled_instance);
        timer.start(std::chrono::milliseconds(delta));
    }
}

/*****************************************************************************/
void AlarmDispatcher::trigger() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (upcoming_alarm && upcoming_alarm->is_enabled()) {
        /* keep alarm alive, check_alarms() may replace upcoming_alarm */
        auto alarm = upcoming_alarm;
        last_dispatched = scheduled_instance;
        dispatch(alarm);
        /* a one-time alarm is done after it rang */
        if (alarm->get_period() == Alarm::Once) {
            alarm->enable(false);
        }
    }
    /* Check for next upcoming alarm */
    check_alarms();
}

/*****************************************************************************/
void AlarmDispatcher::dispatch(const std::shared_ptr<Alarm>& alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << alarm->get_id();
    emit alarm_triggered(alarm.get());
}
/*****************************************************************************/
std::chrono::milliseconds AlarmDispatcher::get_remaining_time() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto r = timer.remainingTimeAsDuration();
    return r;
}

/*****************************************************************************/
QDateTime AlarmDispatcher::reference_time() const {
    auto now = wallclock->now();
    if (last_dispatched.isValid() && now < last_dispatched) {
        return last_dispatched;
    }
    return now;
}

/*****************************************************************************/
std::shared_ptr<Alarm> AlarmDispatcher::get_upcoming_alarm() {
    // Same reference time for all alarms, otherwise the clock could advance
    // while comparing
    auto ref = reference_time();
    std::shared_ptr<Alarm> upcoming;
    QDateTime upcoming_dt;
    for (const auto& alarm : config.get_alarms()) {
        if (!alarm->is_enabled()) {
            continue;
        }
        auto next = get_next_instance(*alarm, ref);
        if (!upcoming || next < upcoming_dt) {
            upcoming = alarm;
            upcoming_dt = next;
        }
    }
    return upcoming;
}

/*****************************************************************************/
QString AlarmDispatcher::get_upcoming_alarm_info() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString ret;

    if (upcoming_alarm && upcoming_alarm->is_enabled()) {
        ret = scheduled_instance.toString("ddd hh:mm");
    }
    return ret;
}
