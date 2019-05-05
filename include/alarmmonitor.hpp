/******************************************************************************
 * \filename
 * \brief    Monitors the mediaplayer when an alarm has been triggered
 *
 * \details
 *
 * \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef INCLUDE_ALARMMONITOR_HPP_
#define INCLUDE_ALARMMONITOR_HPP_

#include <QMediaPlaylist>
#include <QObject>
#include <QTimer>
#include <chrono>
#include <memory>

#include "alarm.hpp"
#include "mediaplayer.hpp"

namespace DigitalRooster {

/**
 * Supervision of alarm behavior. Makes sure I wake up even if the original
 * media stream of the alarm is unavailable.
 */
class AlarmMonitor : public QObject {
    Q_OBJECT
public:
    /**
     * Monitor state
     */
    enum MonitorState {
        Idle,           //!< idle, no alarm playing
        ExpectingAlarm, //!< alarm should be playing soon
        AlarmActive,    //!< alarm is playing
        FallBackMode    //!< alarm failed to start, playing fall back
    };
    Q_ENUM(MonitorState)

    /**
     * Construct AlarmMonitor monitoring state changes of player
     * @param player media player
     * @param fallback_timeout grace period to wait until fallback is triggered
     * @param parent
     */
    AlarmMonitor(std::shared_ptr<MediaPlayer> player,
        std::chrono::milliseconds fallback_timeout = std::chrono::milliseconds(10000),
        QObject* parent = nullptr);

    /**
     * access current state
     * @return \ref AlarmMonitor::state
     */
    MonitorState get_state() const {
        return state;
    };

public slots:
    /**
     * Monitor the trigged alarm if it has started in due time
     * @param  alarm to monitor
     */
    void alarm_triggered(std::shared_ptr<DigitalRooster::Alarm> alarm);

signals:
    /**
     * internal state has changed
     * @param current state \ref AlarmMonitor::state
     */
    void state_changed(AlarmMonitor::MonitorState);

private:
    /**
     * PlayerBackend that receives the Alarms
     */
    std::shared_ptr<MediaPlayer> mpp;

    /**
     * Timer to trigger fallback behavior if player did not start to play
     * resource from alarm
     */
    QTimer fallback_alarm_timer;

    /**
     * Time to wait until fallback behavior is invoked
     */
    std::chrono::milliseconds timeout;

    /**
     * Fallback Alarm
     */
    QMediaPlaylist fallback_alarm;

    /**
     * Current state - check if alarm has been dispatched
     * and we are expecting player to get active
     */
    MonitorState state = Idle;

    /**
     * update \ref AlarmMonitor::state and emit state_changed
     * @param next_state next state
     */
    void set_state(MonitorState next_state);

private slots:

    /**
     * will trigger if player has not started playing in due time
     */
    void trigger_fallback_behavior();
};
} // namespace DigitalRooster

#endif /* INCLUDE_ALARMMONITOR_HPP_ */
