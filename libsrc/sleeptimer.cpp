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

#include <QDebug>
#include <QLoggingCategory>
#include <QMediaPlayer>
#include <chrono>
#include <memory>

#include "sleeptimer.hpp"

using namespace DigitalRooster;
using namespace std::chrono;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.SleepTimer");

/*****************************************************************************/
SleepTimer::SleepTimer(
    std::shared_ptr<ConfigurationManager> cm, QObject* parent)
    : QObject(parent)
    , cm(cm)
    , evt_timer_id(0)
    , remaining_time(cm->get_sleep_timeout()) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    sleep_timer.setInterval(cm->get_sleep_timeout());
    sleep_timer.setSingleShot(true);

    connect(&sleep_timer, &QTimer::timeout, this,
        &SleepTimer::sleep_timer_elapsed);
    // Event loop timer every 30 seconds to update remaining time
    evt_timer_id = startTimer(seconds(30));
}

/*****************************************************************************/
void SleepTimer::playback_state_changed(QMediaPlayer::State state){
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == QMediaPlayer::PlayingState ||
        state == QMediaPlayer::PausedState) {
        qCDebug(CLASS_LC) << " restarting timer";
        sleep_timer.setInterval(cm->get_sleep_timeout());
        sleep_timer.start();
        emit remaining_time_changed(get_remaining_time());
    }
}

/*****************************************************************************/
void SleepTimer::timeout_changed(std::chrono::minutes to) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    sleep_timer.setInterval(cm->get_sleep_timeout());
    sleep_timer.start(to);
    emit remaining_time_changed(get_remaining_time());
}

/*****************************************************************************/
void SleepTimer::timerEvent(QTimerEvent* evt) {
	qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (evt->timerId() == evt_timer_id) {
        emit remaining_time_changed(get_remaining_time());
    } else {
        QObject::timerEvent(evt);
    }
}

/*****************************************************************************/
int SleepTimer::get_remaining_time() {
	qCDebug(CLASS_LC) << Q_FUNC_INFO;
    remaining_time =
    		std::chrono::duration_cast<minutes>(sleep_timer.remainingTimeAsDuration());
    qCDebug(CLASS_LC) << "remaining time: " << remaining_time.count();
    return remaining_time.count();
}

/*****************************************************************************/
std::chrono::minutes SleepTimer::get_sleep_timeout() const {
    return cm->get_sleep_timeout();
}

/*****************************************************************************/
void SleepTimer::set_sleep_timeout(std::chrono::minutes timeout) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    cm->set_sleep_timeout(timeout);
    emit sleep_timeout_changed(timeout);
    // also update running timeouts
    timeout_changed(timeout);
}

/*****************************************************************************/
void SleepTimer::set_sleep_timeout(int timeout) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    cm->set_sleep_timeout(std::chrono::minutes(timeout));
    emit sleep_timeout_changed(timeout);
    // also update running timeouts
    timeout_changed(std::chrono::minutes(timeout));
}

/*****************************************************************************/
int SleepTimer::get_sleep_timeout_minutes_count() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return cm->get_sleep_timeout().count();
}

/*****************************************************************************/

