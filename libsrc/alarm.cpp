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

static Q_LOGGING_CATEGORY(CLASS_LC, "Alarm");

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
                      << "period:" << alarm_period_to_json_string(period);
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

QString DigitalRooster::alarm_period_to_json_string(
    const Alarm::Period period) {
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
std::shared_ptr<Alarm> Alarm::from_json_object(const QJsonObject& json_alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (json_alarm.isEmpty()) {
        throw std::invalid_argument("Empty Alarm JSON object!");
    }
    auto urlstr = json_alarm[KEY_URI].toString();
    auto url = QUrl::fromEncoded(urlstr.toLocal8Bit(), QUrl::StrictMode);
    if (!url.isValid()) {
        qCWarning(CLASS_LC) << "Invalid URL" << json_alarm[KEY_URI].toString();
        throw std::invalid_argument("Alarm url invalid!");
    }

    auto period = json_string_to_alarm_period(
        json_alarm[KEY_ALARM_PERIOD].toString(KEY_ALARM_DAILY));

    auto timepoint =
        QTime::fromString(json_alarm[KEY_TIME].toString(), "hh:mm");
    if (!timepoint.isValid()) {
        qCWarning(CLASS_LC)
            << "Invalid Time " << json_alarm[KEY_TIME].toString();
        throw std::invalid_argument("Alarm time invalid!");
    }

    auto enabled = json_alarm[KEY_ENABLED].toBool(true);

    auto id = QUuid::fromString(
        json_alarm[KEY_ID].toString(QUuid::createUuid().toString()));
    if (id.isNull()) {
        throw std::invalid_argument("id cannot be converted into UUID!");
    }
    /*
     * Create alarm with essential information
     */
    auto alarm = std::make_shared<Alarm>(url, timepoint, period, enabled, id);

    /* Set volume if configured */
    if (json_alarm.contains(KEY_VOLUME)) {
        auto volume = json_alarm[KEY_VOLUME].toInt(DEFAULT_ALARM_VOLUME);
        alarm->set_volume(volume);
    }
    /* if no specific alarm timeout is given take application default */
    if (json_alarm.contains(KEY_ALARM_TIMEOUT)) {
        auto timeout =
            json_alarm[KEY_ALARM_TIMEOUT].toInt(DEFAULT_ALARM_TIMEOUT.count());
        alarm->set_timeout(std::chrono::minutes(timeout));
    }
    return alarm;
}

/*****************************************************************************/
QJsonObject Alarm::to_json_object() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject alarmcfg;
    alarmcfg[KEY_ID] = this->get_id().toString(QUuid::WithoutBraces);
    alarmcfg[KEY_ALARM_PERIOD] =
        alarm_period_to_json_string(this->get_period());
    alarmcfg[KEY_TIME] = this->get_time().toString("hh:mm");
    alarmcfg[KEY_VOLUME] = this->get_volume();
    alarmcfg[KEY_URI] = this->get_media_url().toString();
    alarmcfg[KEY_ENABLED] = this->is_enabled();
    return alarmcfg;
}

/*****************************************************************************/
