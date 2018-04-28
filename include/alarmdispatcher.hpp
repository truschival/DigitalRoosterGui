/******************************************************************************
 * \filename
 * \brief  Monitors alarm list and dispatches Alarm to player when due
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#ifndef _ALARMDISPATCHER_HPP_
#define _ALARMDISPATCHER_HPP_

#include <QObject>
#include <QTimer>
#include <chrono>
#include <memory>

#include "alarm.hpp"
#include "mediaplayerproxy.hpp"

namespace DigitalRooster {
class ConfigurationManager;

/**
 * Monitors changes in alarm configuration and dispatches alarms when due
 */
class AlarmDispatcher : QObject {
    Q_OBJECT
public:
    AlarmDispatcher(MediaPlayerProxy* playerproxy,
        ConfigurationManager* confman, QObject* parent = nullptr);

signals:
    void alarm_triggered();

private:
    /**
     * PlayerBackend that receives the Alarms
     */
    MediaPlayerProxy* mpp;

    /**
     * Central configuration and data handler
     */
    ConfigurationManager* cm;
    /**
     * Timer for periodic polling
     */
    QTimer interval_timer;
    /**
     * Time interval to check for alarm dispatching and if new alarms are
     * available in configuration
     */
    int interval = 30000;
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
     * but player has not yet started playing
     */
    bool player_not_started = false;
public slots:
    /**
     * Will walk alarms and check if ready for dispatch
     * */
    void check_alarms();

    /**
     * will trigger if player has not started playing in due time
     */
    void fallback_triggered();
};
} // namespace DigitalRooster

#endif /*_ALARMDISPATCHER_HPP_*/
