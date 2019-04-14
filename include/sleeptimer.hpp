/******************************************************************************
 * \filename
 * \brief Puts DigitalRooster in standby when timeout has elapsed
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef INCLUDE_SLEEPTIMER_HPP_
#define INCLUDE_SLEEPTIMER_HPP_

#include <QMediaPlayer>
#include <QObject>
#include <chrono>
#include <memory>

#include "alarm.hpp"
#include "configuration_manager.hpp"

namespace DigitalRooster {

/**
 * SleepTimer resets internal timer to \ref ConfigurationManager::sleep_timeout
 * on signal player_state_chaged() and puts DigitalRooster in standby when
 * sleep_timeout has elapsed
 */
class SleepTimer : public QObject {
    Q_OBJECT
    Q_PROPERTY(int time_remaining READ get_remaining_time NOTIFY
            remaining_time_changed)
    Q_PROPERTY(int sleep_timeout_min READ get_sleep_timeout_minutes_count WRITE
            set_sleep_timeout NOTIFY sleep_timeout_changed)
public:
    /**
     * Sleep timer constructor
     * @param cm configuration manager
     */
    SleepTimer(
        std::shared_ptr<ConfigurationManager> cm, QObject* parent = nullptr);

    /**
     * Access the remaining time until standby is triggered
     * @return time in minutes (as int)
     */
    int get_remaining_time();

    /**
     * Minutes after which DigitalRooster goes in standby
     * @return \ref sleep_timeout
     */
    std::chrono::minutes get_sleep_timeout() const;

    /**
     * Minutes after which DigitalRooster goes in standby
     * \ref get_sleep_timeout for clients that dont understand
     * std::chrono::minutes
     * @return \ref sleep_timeout
     */
    int get_sleep_timeout_minutes_count() const;

    /**
     * Update sleep timeout Minutes after which DigitalRooster goes in standby
     * for clients that dont understand std::chrono::minutes
     * @param timeout \ref sleep_timeout
     */
    void set_sleep_timeout(int timeout);

    /**
     * Update sleep timeout Minutes after which DigitalRooster goes in standby
     * @param timeout \ref sleep_timeout
     */
    void set_sleep_timeout(std::chrono::minutes timeout);
	/**
	 * Generic timer reset
	 */
	Q_INVOKABLE void reset_timer();

public slots:
    /**
     * Player state changed - resets timer if state is playing or paused
     * @param state current player state
     */
    void playback_state_changed(QMediaPlayer::State state);
    /**
     * Timout has been changed, reset timer
     * @param to new timeout value
     */
    void timeout_changed(std::chrono::minutes to);

    /**
     * Alarm has been triggered - set sleep timeout to \ref Alarm::timeout
     * @param  alarm that should be running
     */
    void alarm_triggered(std::shared_ptr<DigitalRooster::Alarm> alarm);

signals:
    /**
     * Timeout has elapsed
     */
    void sleep_timer_elapsed();

    /**
     * Inform subscribers about remaining time
     * @param time in minutes
     */
    void remaining_time_changed(int time);

    /**
     * Sleep timeout has been changed
     * for C++ clients that understand std::chrono::minutes
     * \param minutes
     */
    void sleep_timeout_changed(std::chrono::minutes minutes);

    /**
     * Sleep timeout has been changed
     * for clients that dont understand std::chrono::minutes
     * \param minutes \ref sleep_timeout
     */
    void sleep_timeout_changed(int minutes);

private:
    /**
     * local state 
     */
    enum TimerState {
        Idle = 0, //!< no running sleep_timeout
        Alarm,    //!< Alarm was triggged
        Playing	  //!< activation through user interaction
    };

    /**
     * Central configuration and data handler
     */
    std::shared_ptr<ConfigurationManager> cm;

    /**
     * Timer to trigger standby
     */
    QTimer sleep_timer;

    /**
     * Event Loop timer id from \ref QObject::startTimer()
     */
    int evt_timer_id;

    /**
     * Remaining time until sleep_timer_elapsed is emitted
     */
    std::chrono::minutes remaining_time;

    /**
     * local state to decide if alarm was triggered and events form QMediaPlayer
     * should be ignored (unil alarm is stopped)
     */
    TimerState activity;

    /**
     * React to QObject Timer events
     * @param evt
     */
    void timerEvent(QTimerEvent* evt) override;
};

};     // namespace DigitalRooster
#endif /* INCLUDE_SLEEPTIMER_HPP_ */
