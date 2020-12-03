/******************************************************************************
 * \filename
 * \brief  Dispatches Alarm to player when due
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _ALARMDISPATCHER_HPP_
#define _ALARMDISPATCHER_HPP_

#include <QObject>
#include <QTimer>

#include <chrono>
#include <memory>

namespace DigitalRooster {
class Alarm;
class IAlarmStore;

/**
 * Monitors changes in alarm configuration and dispatches alarms when due
 */
class AlarmDispatcher : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString upcoming_alarm_info READ get_upcoming_alarm_info NOTIFY
            upcoming_alarm_info_changed)

public:
    /**
     * Constructor for AlarmDispatcher
     * @param store Interface to get/update/delete alarms
     * @param parent QObject hierarchy manage lifetime
     */
    explicit AlarmDispatcher(IAlarmStore& store, QObject* parent = nullptr);

    /**
     * get remaining time to alarm
     * @return interval in seconds
     */
    std::chrono::milliseconds get_remaining_time() const;

    /**
     * Timestamp for upcoming alarm
     * @return string of DateTime - like MON 07:45
     */
    QString get_upcoming_alarm_info() const;

    /**
     * Get Alarm instance that is triggered next
     */
    std::shared_ptr<DigitalRooster::Alarm> get_upcoming_alarm();

public slots:
    /**
     * Will update upcoming alarm and schedule a timer
     * */
    void check_alarms();

signals:
    /**
     * Signal receivers interested specific alarm content
     * @param alarm triggered alarm
     */
    void alarm_triggered(DigitalRooster::Alarm*  alarm);

    /**
     * Upcoming alarm has changed
     * @param info - new display string
     */
    void upcoming_alarm_info_changed(QString info);

private:
    /**
     * Central configuration and data handler
     */
    IAlarmStore& cm;

    /**
     * Next alarm to be dispatched, keep a copy
     */
    std::shared_ptr<DigitalRooster::Alarm> upcoming_alarm;

    /**
     * Trigger Timer for upcoming alarm
     */
    QTimer timer;

    /**
     * Convenience method to dispatch alarm to receivers
     * @param alarm to dispatch
     */
    void dispatch(const std::shared_ptr<DigitalRooster::Alarm>& alarm);

private slots:
    /**
     * Adapter slot to call dispatch(alarm) with upcoming_alarm
     */
    void trigger();
};
} // namespace DigitalRooster

#endif /*_ALARMDISPATCHER_HPP_*/
