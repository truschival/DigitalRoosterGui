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

#include "PlayableItem.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"
using namespace DigitalRooster;

/*****************************************************************************/

// Alarm from time
Alarm::Alarm(const QUrl& media, const QTime& timepoint, Alarm::Period period,
    bool enabled, QObject* parent)
    : QObject(parent)
    , media(std::make_shared<PlayableItem>("Alarm", media))
    , period(period)
    , trigger_instant(QDateTime::currentDateTime()) // today, set time later
    , enabled(enabled)
    , alarmtimeout(default_alarm_timeout) {

    trigger_instant.setTime(timepoint);
    // qDebug() << __FUNCTION__ << "timepoint" << trigger_instant;
}
/*****************************************************************************/

Alarm::Alarm(const QUrl& media, const QDateTime& timepoint, bool enabled,
    QObject* parent)
    : QObject(parent)
    , media(std::make_shared<PlayableItem>("Alarm", media))
    , period(Alarm::Once)
    , trigger_instant(timepoint)
    , enabled(enabled)
    , alarmtimeout(default_alarm_timeout) {
    // qDebug() << __FUNCTION__ << "timepoint" << trigger_instant;
}

/*****************************************************************************/
void Alarm::set_trigger(const QTime& timeofday, Alarm::Period period) {
    trigger_instant.setTime(timeofday);
    set_period(period);
}

/*****************************************************************************/
void Alarm::set_trigger(const QDateTime& timeinstance) {
    trigger_instant = timeinstance;
    set_period(Alarm::Once);
}

/*****************************************************************************/
void Alarm::update_trigger() {
    auto now = QDateTime::currentDateTime();
    auto day_of_week_today = now.date().dayOfWeek();

    // qDebug() << "Old trigger_instant " << trigger_instant;

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
    // qDebug() << "Next trigger_instant " << trigger_instant;
}

/*****************************************************************************/
const QDateTime& Alarm::get_next_trigger() {
    update_trigger();
    return trigger_instant;
}

/*****************************************************************************/
QString Alarm::get_period_string() const{ 
	switch (get_period()) {
    case Alarm::Once:
        return QString(tr("Once"));
    case Alarm::Weekend:
        return QString(tr("Weekend")); 
    case Alarm::Workdays:
        return QString(tr("Workdays")); 
    case Alarm::Daily:
        return QString(tr("Daily")); 
    default:
        return QString(tr("Error")); 
    }
}

/*****************************************************************************/
