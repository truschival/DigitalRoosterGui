/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#include <QDebug>
#include <QMediaPlayer>

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "mediaplayer.hpp"

using namespace DigitalRooster;

/*****************************************************************************/
AlarmMonitor::AlarmMonitor(std::shared_ptr<MediaPlayer> player, QObject* parent)
    : QObject(parent)
    , mpp(player) {

    /* Receive errors from player */
    QObject::connect(mpp.get(),
        static_cast<void (MediaPlayer::*)(QMediaPlayer::Error)>(
            &MediaPlayer::error),
        [=](QMediaPlayer::Error error) {
            /* if any error occurs while we are expecting an alarm fallback! */
            if (error != QMediaPlayer::NoError && expecting_alarm_playing) {
                trigger_fallback_behavior();
            }
        });

    QObject::connect(mpp.get(), &MediaPlayer::playback_state_changed,
        [=](QMediaPlayer::State state) {
            /* only do something if we started the alarm */
            if (!alarm_auto_stop_timer.isActive())
                return;
            /* check if alarm was stopped without error (probably user
             * interaction) */
            if (state == QMediaPlayer::StoppedState ||
                state == QMediaPlayer::PausedState) {
                if (mpp->error() == QMediaPlayer::NoError) {
                    fallback_alarm_timer.stop();
                    alarm_auto_stop_timer.stop();
                }
            }
            qDebug() << " AlarmMonitor received " << state
                     << " Player error: " << mpp->error();
        });
    /* setup fallback behavior */
    fallback_alarm.addMedia(QMediaContent(QUrl("qrc:/TempleBell.mp3")));
    fallback_alarm.setPlaybackMode(QMediaPlaylist::Loop);

    /**
     * Disarm the fallback behavior after some time has passed
     */
    fallback_alarm_timer.setSingleShot(true);
    fallback_alarm_timer.setInterval(fallback_timeout);
    QObject::connect(&fallback_alarm_timer, &QTimer::timeout,
        [=]() { expecting_alarm_playing = false; });

    /**
     * Stop playing after timeout
     */
    QObject::connect(&alarm_auto_stop_timer, SIGNAL(timeout()), this,
        SLOT(stop_running_alarm()));
}

/*****************************************************************************/
void AlarmMonitor::alarm_triggered(
    std::shared_ptr<DigitalRooster::Alarm> alarm) {
    mpp->set_media(alarm->get_media());
    mpp->set_volume(alarm->get_volume());
    expecting_alarm_playing = true;
    fallback_alarm_timer.start(fallback_timeout);
    alarm_auto_stop_timer.setInterval(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            alarm->get_timeout()));
    alarm_auto_stop_timer.setSingleShot(true);
    start_playing();
}

/*****************************************************************************/
void AlarmMonitor::trigger_fallback_behavior() {
    if (expecting_alarm_playing) {
        qWarning() << "Player has not started in due time!";
        expecting_alarm_playing = false;
        fallback_alarm.setCurrentIndex(0);
        mpp->set_volume(80);
        mpp->set_playlist(&fallback_alarm);
        start_playing();
    }
}

/*****************************************************************************/
void AlarmMonitor::start_playing() {
    mpp->play();
    alarm_auto_stop_timer.start();
}
/*****************************************************************************/
void AlarmMonitor::stop_running_alarm() {
    qDebug() << Q_FUNC_INFO;
    mpp->stop();
}
