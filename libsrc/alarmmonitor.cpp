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
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

/***********************************************************************/
AlarmMonitor::AlarmMonitor(MediaPlayerProxy* player, QObject* parent)
    : QObject(parent)
    , mpp(player) {

    /* Receive errors from player */
    QObject::connect(mpp,
        static_cast<void (MediaPlayerProxy::*)(QMediaPlayer::Error)>(
            &MediaPlayerProxy::error),
        [=](QMediaPlayer::Error error) {
            qDebug() << " Player error" << error;
            /* if any error occurs while we are expecting an alarm fallback! */
            if (error != QMediaPlayer::NoError && expecting_alarm_playing) {
                trigger_fallback_behavior();
            }
        });

    QObject::connect(mpp, &MediaPlayerProxy::playback_state_changed,
        [=](QMediaPlayer::State state) {
            qDebug() << " AlarmMonitor received " << state;
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
}
/*****************************************************************************/
void AlarmMonitor::alarm_triggered(
    std::shared_ptr<DigitalRooster::Alarm> alarm) {
    mpp->set_media(alarm->get_media());
    mpp->set_volume(alarm->get_volume());
    mpp->play();
    expecting_alarm_playing = true;
    fallback_alarm_timer.start(fallback_timeout);
}
/*****************************************************************************/
void AlarmMonitor::trigger_fallback_behavior() {
    if (expecting_alarm_playing) {
        qWarning() << "Player has not started in due time!";
        expecting_alarm_playing = false;
        fallback_alarm.setCurrentIndex(0);
        mpp->set_volume(80);
        mpp->set_playlist(&fallback_alarm);
        mpp->play();
    }
}
