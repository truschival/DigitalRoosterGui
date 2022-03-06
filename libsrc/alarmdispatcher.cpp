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
        emit upcoming_alarm_info_changed("");
        timer.stop();
    } else {
        auto alm_dt = get_next_instance(*upcoming_alarm);
        emit upcoming_alarm_info_changed(alm_dt.toString("ddd hh:mm"));
        auto delta =
            alm_dt.toMSecsSinceEpoch() - wallclock->now().toMSecsSinceEpoch();
        timer.start(delta);
    }
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
std::shared_ptr<Alarm> AlarmDispatcher::get_upcoming_alarm() {
    // Sort a copy of the alarms not to mess up Alarm list in QML
    auto v = std::vector(config.get_alarms());
    if (v.empty()) {
        return nullptr;
    }

    std::sort(v.begin(), v.end(),
        [](const std::shared_ptr<Alarm>& lhs,
            const std::shared_ptr<Alarm>& rhs) { return *lhs < *rhs; });
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
