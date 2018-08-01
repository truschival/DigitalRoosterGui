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

class AlarmMonitor : public QObject {
    Q_OBJECT
public:
    AlarmMonitor(std::shared_ptr<MediaPlayer> player, QObject* parent = nullptr);

public slots:
    void alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>);

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
    int fallback_timeout = 20000;

    /**
     * Internal state to check if alarm has been dispatched
     * and we are expecting player to get active
     */
    bool expecting_alarm_playing = false;
    /**
     * Fallback Alarm
     */
    QMediaPlaylist fallback_alarm;

    /**
     * Timer to stop alarm automatically if user has not stopped
     */
    QTimer alarm_auto_stop_timer;

    /**
     * Helper method to start player and alarm_auto_stop_timer
     */
    void start_playing();

    /**
     * Slot/Callback to stop running alarm after alarm_auto_stop_timer
     * has expired
     */
private slots:
    void stop_running_alarm();

    /**
     * will trigger if player has not started playing in due time
     */
    void trigger_fallback_behavior();
};
} // namespace DigitalRooster

#endif /* INCLUDE_ALARMMONITOR_HPP_ */
