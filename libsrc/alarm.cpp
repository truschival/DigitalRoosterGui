// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>
#include <stdexcept>

#include "PlayableItem.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"
#include "timeprovider.hpp"
#include "util.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.Alarm");

/*****************************************************************************/
Alarm::Alarm()
    : id(QUuid::createUuid())
    , media(std::make_shared<PlayableItem>("Alarm", QUrl()))
    , period(Alarm::Daily)
    , enabled(true)
    , timeout(DEFAULT_ALARM_TIMEOUT) {
}

/*****************************************************************************/
// Alarm from time with media etc.
Alarm::Alarm(const QUrl& url, const QTime& timepoint, Alarm::Period period,
    bool enabled, const QUuid& uid, QObject* parent)
    : QObject(parent)
    , id(uid)
    , media(std::make_shared<PlayableItem>("Alarm", url))
    , period(period)
    , alarm_time(timepoint)
    , enabled(enabled)
    , timeout(DEFAULT_ALARM_TIMEOUT) {

    qCDebug(CLASS_LC) << Q_FUNC_INFO << "time:" << alarm_time << "URL:" << url
                      << "period:" << alarm_period_to_string(period);
}

/*****************************************************************************/
void Alarm::enable(bool state) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << state;
    enabled = state;
    emit dataChanged();
    emit enabled_changed(true);
}

/*****************************************************************************/
void Alarm::set_time(const QTime& timeofday) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << ":" << timeofday;
    alarm_time = timeofday;
    emit time_changed(alarm_time);
    emit dataChanged();
}

/*****************************************************************************/
QUrl Alarm::get_media_url() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return media->get_url();
}

/*****************************************************************************/
QString Alarm::get_period_string() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return alarm_period_to_string(this->period);
}

/*****************************************************************************/
void Alarm::set_period(Alarm::Period period) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->period = period;
    emit period_changed(period);
    emit period_changed(get_period_string());
}

/*****************************************************************************/
const QTime& Alarm::get_time() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << alarm_time;
    return alarm_time;
}

/*****************************************************************************/

static const std::vector<std::pair<Alarm::Period, QString>> period_to_string = {
    {Alarm::Daily, KEY_ALARM_DAILY}, {Alarm::Workdays, KEY_ALARM_WORKDAYS},
    {Alarm::Weekend, KEY_ALARM_WEEKEND}, {Alarm::Once, KEY_ALARM_ONCE}};

/*****************************************************************************/
void Alarm::update_media_url(const QUrl& url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    media->set_url(url);
    emit media_url_changed(url);
    emit dataChanged();
}

/*****************************************************************************/
Alarm::Period DigitalRooster::string_to_alarm_period(const QString& literal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto res = std::find_if(period_to_string.begin(), period_to_string.end(),
        [&](const std::pair<Alarm::Period, QString>& item) {
            return item.second == literal;
        });
    if (res == period_to_string.end()) {
        throw(std::invalid_argument(
            std::string("Invalid value for AlarmPeriod! :") +
            literal.toStdString()));
    }
    return res->first;
}
/*****************************************************************************/

QString DigitalRooster::alarm_period_to_string(const Alarm::Period period) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto res = std::find_if(period_to_string.begin(), period_to_string.end(),
        [&](const std::pair<Alarm::Period, QString>& item) {
            return item.first == period;
        });
    if (res == period_to_string.end()) {
        qCWarning(CLASS_LC) << "Invalid Period passed (" << period << ")"
                            << "This should never happen - aborting";
        abort();
    }
    return res->second;
}

/*****************************************************************************/
std::shared_ptr<Alarm> Alarm::from_json_object(const QJsonObject& json) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (json.isEmpty()) {
        throw std::invalid_argument("Empty Alarm JSON object!");
    }
    auto urlstr = json[KEY_URI].toString();
    auto url = valid_url_from_string(urlstr);

    auto period = string_to_alarm_period(
        json[KEY_ALARM_PERIOD].toString(KEY_ALARM_DAILY));

    auto timepoint = QTime::fromString(json[JSON_KEY_TIME].toString(), "hh:mm");
    if (!timepoint.isValid()) {
        qCWarning(CLASS_LC)
            << "Invalid Time " << json[JSON_KEY_TIME].toString();
        throw std::invalid_argument("Alarm time invalid!");
    }

    auto enabled = json[KEY_ENABLED].toBool(true);
    /* if json[KEY_ID] is empty create a new UUID */
    auto id = valid_uuid_from_String(
        json[KEY_ID].toString(QUuid::createUuid().toString()));
    /*
     * Create alarm with essential information
     */
    auto alarm = std::make_shared<Alarm>(url, timepoint, period, enabled, id);

    /* Set volume if configured */
    if (json.contains(KEY_VOLUME)) {
        auto volume = json[KEY_VOLUME].toInt(DEFAULT_ALARM_VOLUME);
        alarm->set_volume(volume);
    }
    /* if no specific alarm timeout is given take application default */
    if (json.contains(KEY_ALARM_TIMEOUT)) {
        auto timeout =
            json[KEY_ALARM_TIMEOUT].toInt(DEFAULT_ALARM_TIMEOUT.count());
        alarm->set_timeout(std::chrono::minutes(timeout));
    }
    return alarm;
}

/*****************************************************************************/
QJsonObject Alarm::to_json_object() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject alarmcfg;
    alarmcfg[KEY_ID] = this->get_id().toString(QUuid::WithoutBraces);
    alarmcfg[KEY_ALARM_PERIOD] = alarm_period_to_string(this->get_period());
    alarmcfg[JSON_KEY_TIME] = this->get_time().toString("hh:mm");
    alarmcfg[KEY_VOLUME] = this->get_volume();
    alarmcfg[KEY_URI] = this->get_media_url().toString();
    alarmcfg[KEY_ENABLED] = this->is_enabled();
    return alarmcfg;
}


/*****************************************************************************/
QDateTime DigitalRooster::get_next_instance(const Alarm& alm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QDateTime next;

    // If disabled return invalid datetime
    if (!alm.is_enabled()) {
        return next;
    }

    next.setTime(alm.get_time());
    // current DateTime instance
    auto now = wallclock->now();
    auto dow = now.date().dayOfWeek();
    // preliminary date, today
    next.setDate(now.date());

    switch (alm.get_period()) {
    // If enabled Once has the same behavior as daily: next possible instance
    // is today, or if passed, tomorrow at the same time
    case Alarm::Daily:
    case Alarm::Once:
        if (now.time() > alm.get_time()) {
            next = next.addDays(1);
        }
        break;
    case Alarm::Weekend:
        // any enabled weekend alarm has it's next instance on a Saturday or
        // Sunday If today is Saturday or Sunday and the time has passed -> add
        // one day.
        if (dow >= Qt::Saturday && now.time() > alm.get_time()) {
            next = next.addDays(1);
        }
        // If spilled over from Sunday to Monday or next is a workday, add
        // difference to Saturday
        if (next.date().dayOfWeek() < Qt::Saturday) {
            next = next.addDays(Qt::Saturday - next.date().dayOfWeek());
        }
        break;
    case Alarm::Workdays:
        // Monday to friday, add 1 day if time has passed
        if (dow < Qt::Saturday && now.time() > alm.get_time()) {
            next = next.addDays(1);
        }
        // Today is Weekend, schedule for monday
        if (dow >= Qt::Saturday) {
            next = next.addDays(Qt::Sunday - dow + 1);
        }

        // If spilled over from friday to saturday add difference
        if (next.date().dayOfWeek() >= Qt::Saturday) {
            next = next.addDays(Qt::Sunday - next.date().dayOfWeek() + 1);
        }
        break;
    }
    qCDebug(CLASS_LC) << "<  " << next;
    return next;
}

/*****************************************************************************/
/**
 * Comparison Operators to make Alarms comparable by their next execution
 * instance
 * TODO: C++20 make this a spaceship operator!
 */
bool DigitalRooster::operator<(const Alarm& lhs, const Alarm& rhs) {
    if (!lhs.is_enabled()) {
        return false;
    }
    if (!rhs.is_enabled()) {
        return true;
    }
    return get_next_instance(lhs) < get_next_instance(rhs);
};
