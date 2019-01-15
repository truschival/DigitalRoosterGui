/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _ALARM_HPP_
#define _ALARM_HPP_

#include <QDateTime>
#include <QObject>
#include <QUrl>
#include <QUuid>
#include <chrono>
#include <memory>

namespace DigitalRooster {

class PlayableItem;
class TimeProvider;
/**
 * A single alarm
 */
class Alarm : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ is_enabled WRITE enable NOTIFY enabled_changed)
    Q_PROPERTY(QUuid id READ get_id)
    Q_PROPERTY(QTime time READ get_time WRITE set_time NOTIFY time_changed)
    Q_PROPERTY(QString periodicity READ get_period_string NOTIFY period_changed)
    Q_PROPERTY(DigitalRooster::Alarm::Period period_id READ get_period WRITE
            set_period NOTIFY period_changed)
    Q_PROPERTY(QUrl url READ get_media_url WRITE update_media_url NOTIFY
            media_url_changed)

public:
    /**
     * Alarm periodicity
     */
    enum Period {
        Once = 0, //!< next time the time of day occurs (within 24 hrs)
        Daily,    //!< every day
        Weekend,  //!< Saturdays & Sundays
        Workdays  //!< Monday through Friday
    };
    Q_ENUM(Period)

    /**
     * Construct an alarm for given time
     * @param media what to play
     * @param timepoint time of day - any time of day
     * @param period periodicity
     * @param enabled activated/deactivated
     * @param parent obligatory QObject parent
     */
    Alarm(const QUrl& media, const QTime& timepoint,
        Alarm::Period period = Alarm::Daily, bool enabled = true,
        const QUuid& uid = QUuid::createUuid(), QObject* parent = nullptr);

    /**
     * Need Default constructor to register with QML
     */
    Alarm()
        : id(QUuid::createUuid())
        , media(nullptr)
        , period(Alarm::Daily)
        , enabled(true){};

    /**
     * unique id for alarm
     * @return
     */
    QUuid get_id() const {
        return id;
    }

    /**
     * Trigger time
     * @return time of day when alarm is due
     */
    const QTime& get_time() const;
    void set_time(const QTime& timeofday);

    /**
     * Periodicity of Alarm
     * @return period
     */
    Alarm::Period get_period() const {
        return period;
    }

    /**
     * Convert the periodicity into a human readable string
     * @return string representation for enum
     */
    QString get_period_string() const;

    /**
     * Change periodicity
     */
    void set_period(Alarm::Period period) {
        this->period = period;
        emit period_changed(period);
        emit period_changed(get_period_string());
    };

    /**
     * Volume for this Alarm
     * @return
     */
    int get_volume() {
        return volume;
    }
    /**
     * Update Volume for this Alarm
     * @param volume
     */
    void set_volume(int volume) {
        this->volume = volume;
    }

    /**
     * Duration for alarm to stop automatically
     * @return time in minutes
     */
    std::chrono::minutes get_timeout() {
        return alarmtimeout;
    }
    /**
     * Duration for alarm to stop automatically
     * @param  timeout in minutes
     */
    void set_timeout(std::chrono::minutes timeout) {
        alarmtimeout = timeout;
    }

    /**
     * Alarm media
     */
    std::shared_ptr<PlayableItem> get_media() const {
        return media;
    }
    void set_media(std::shared_ptr<PlayableItem> new_media) {
        media = new_media;
    }

    /**
     * is this alarm set
     * @return state
     */
    bool is_enabled() const {
        return enabled;
    }

    /**
     * update media form QML
     * @param url new media url
     */
    void update_media_url(const QUrl& url);
    QUrl get_media_url() const;

public slots:
    /**
     * enable alarm to play next time
     * @param state
     */

    void enable(bool state) {
        enabled = state;
        emit dataChanged();
        emit enabled_changed(true);
    }

signals:
    void enabled_changed(bool state);

    void period_changed(Alarm::Period period);

    void period_changed(QString period);

    void time_changed(QTime time);

    void media_url_changed(QUrl url);
    /**
     * Generic event, some data of this object changed
     */
    void dataChanged();

private:
    /**
     * 'unique' id for this alarm
     */
    const QUuid id;

    /**
     * What to play when alarm triggers
     */
    std::shared_ptr<PlayableItem> media;

    /**
     * when alarm is repeated
     */
    Alarm::Period period;

    /**
     * Time of day when Alarm is due
     */
    QTime alarm_time;

    /**
     * Will it trigger?
     */
    bool enabled;
    /**
     * Duration for alarm to stop automatically
     */
    std::chrono::minutes alarmtimeout;

    /**
     * Default volume for alarm
     */
    int volume = 40;
};

/**
 * Map a Alarm period literal to enum value
 * @param literal "daily", "once", "weekend" or "workdays"
 * @return enum value
 */
Alarm::Period json_string_to_alarm_period(const QString& literal);

/**
 * Get String for period
 * @param period Alarm::Once "1".... etc
 * @return QString "Once"
 */
QString alarm_period_to_json_string(const Alarm::Period period);

};     // namespace DigitalRooster
#endif // _ALARM_HPP_
