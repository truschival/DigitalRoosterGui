/******************************************************************************
 * \filename
 * \brief    Monitors the mediaplayer when an alarm has been triggered
 *
 * \details
 *
 * \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
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

namespace DigitalRooster {
class MediaPlayerProxy;

class AlarmMonitor : public QObject {
    Q_OBJECT
public:
    AlarmMonitor(MediaPlayerProxy* player, QObject* parent = nullptr);

public slots:
    void alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>);

private:
    /**
     * PlayerBackend that receives the Alarms
     */
    MediaPlayerProxy* mpp;
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
     * will trigger if player has not started playing in due time
     */
    void trigger_fallback_behavior();

    /**
     * Fallback Alarm
     */
    QMediaPlaylist fallback_alarm;
};
} // namespace DigitalRooster

#endif /* INCLUDE_ALARMMONITOR_HPP_ */
