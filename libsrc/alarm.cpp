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

#include <QLoggingCategory>
#include <QUrl>

#include "PlayableItem.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"
#include "timeprovider.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.Alarm");

/*****************************************************************************/
// Alarm from time
Alarm::Alarm(const QUrl& media, const QTime& timepoint, Alarm::Period period,
    bool enabled, const QUuid& uid, QObject* parent)
    : QObject(parent)
    , id(uid)
    , media(std::make_shared<PlayableItem>("Alarm", media))
    , period(period)
    , alarm_time(timepoint)
    , enabled(enabled)
    , alarmtimeout(DEFAULT_ALARM_TIMEOUT) {

    qCDebug(CLASS_LC) << Q_FUNC_INFO << "trigger:" << alarm_time;
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
    return media->get_url();
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
