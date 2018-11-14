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

#include "PlayableItem.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"
#include <QLoggingCategory>
#include "timeprovider.hpp"
using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.Alarm");

/*****************************************************************************/
// Alarm from time
Alarm::Alarm(const QUrl& media, const QTime& timepoint, Alarm::Period period,
    bool enabled, QObject* parent)
    : QObject(parent)
    , media(std::make_shared<PlayableItem>("Alarm", media))
    , period(period)
    , trigger_instant(wallclock->now()) // use today's date, set time later
    , enabled(enabled)
    , alarmtimeout(DEFAULT_ALARM_TIMEOUT) {
	trigger_instant.setTime(timepoint);
	qCDebug(CLASS_LC) << Q_FUNC_INFO << "trigger:" << trigger_instant;
}

/*****************************************************************************/
// Alarm for exact date & time
Alarm::Alarm(const QUrl& media, const QDateTime& timepoint, Alarm::Period period,
    bool enabled, QObject* parent)
    : QObject(parent)
    , media(std::make_shared<PlayableItem>("Alarm", media))
    , period(period)
    , trigger_instant(timepoint) // use exact timepoint
    , enabled(enabled)
    , alarmtimeout(DEFAULT_ALARM_TIMEOUT) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << "trigger:" << trigger_instant;
}

/*****************************************************************************/
void Alarm::set_time(const QTime& timeofday) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    trigger_instant.setTime(timeofday);
    emit time_changed(trigger_instant.time());
}

/*****************************************************************************/
void Alarm::set_trigger(const QTime& timeofday, Alarm::Period period) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    trigger_instant.setTime(timeofday);
    set_period(period);
}

/*****************************************************************************/
void Alarm::set_trigger(const QDateTime& timeinstance) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    trigger_instant = timeinstance;
    set_period(Alarm::Once);
}

/*****************************************************************************/
void Alarm::update_trigger() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto now = wallclock->now();
    qCDebug(CLASS_LC) << now;
    auto day_of_week_today = now.date().dayOfWeek();

    // trigger instant is in the future - nothing to adjust for
    if (now < trigger_instant) {
        return;
    }

    // trigger in the past - we have to update
    switch (period) {
    case Alarm::Once:
        break;
    case Alarm::Weekend:
        /* Mon - Fri: schedule for next weekend*/
        if (day_of_week_today < Qt::Saturday) {
            trigger_instant =
                trigger_instant.addDays(Qt::Saturday - day_of_week_today);
        } /* Sunday: schedule to Saturday, Saturday schedule to Sunday*/
        else if (day_of_week_today == Qt::Sunday) {
            trigger_instant = trigger_instant.addDays(6);
        } else if (day_of_week_today == Qt::Saturday) {
            trigger_instant = trigger_instant.addDays(1);
        }
        break;
    case Alarm::Workdays:
        /* Mon - Thursday set next trigger for tomorrow */
        if (day_of_week_today <= Qt::Thursday) {
            trigger_instant = trigger_instant.addDays(1);
        } /* Friday and Saturday, Sunday set it for next monday */
        else {
            trigger_instant =
                trigger_instant.addDays(Qt::Sunday - day_of_week_today + 1);
        }
        break;
    case Alarm::Daily:
        trigger_instant = trigger_instant.addDays(1);
        break;
    }
    qCDebug(CLASS_LC) << "Next trigger_instant " << trigger_instant;
}

/*****************************************************************************/
const QDateTime& Alarm::get_next_trigger() {
    update_trigger();
    return trigger_instant;
}

/*****************************************************************************/
QString Alarm::get_period_string() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    switch (get_period()) {
    case Alarm::Once:
        return QString(tr("once"));
    case Alarm::Weekend:
        return QString(tr("weekend"));
    case Alarm::Workdays:
        return QString(tr("workdays"));
    case Alarm::Daily:
        return QString(tr("daily"));
    default:
        return QString(tr("ERROR"));
    }
}

/*****************************************************************************/

static const std::vector<std::pair<Alarm::Period, QString>> period_to_string = {
    {Alarm::Daily, KEY_ALARM_DAILY}, {Alarm::Workdays, KEY_ALARM_WORKDAYS},
    {Alarm::Weekend, KEY_ALARM_WEEKEND}, {Alarm::Once, KEY_ALARM_ONCE}};


/*****************************************************************************/

Alarm::Period DigitalRooster::json_string_to_alarm_period(
    const QString& literal) {
    auto res = std::find_if(period_to_string.begin(), period_to_string.end(),
        [&](const std::pair<Alarm::Period, QString>& item) {
            return item.second == literal;
        });
    if (res == period_to_string.end())
        throw(std::invalid_argument(std::string("can't resolve argument")));

    return res->first;
}
/*****************************************************************************/

QString DigitalRooster::alarm_period_to_json_string(
    const Alarm::Period period) {
    auto res = std::find_if(period_to_string.begin(), period_to_string.end(),
        [&](const std::pair<Alarm::Period, QString>& item) {
            return item.first == period;
        });
    if (res == period_to_string.end())
        throw(std::invalid_argument(std::string("can't resolve argument")));

    return res->second;
}
/*****************************************************************************/
