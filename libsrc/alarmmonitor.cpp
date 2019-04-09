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

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "mediaplayer.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.AlarmMonitor");

/*****************************************************************************/
AlarmMonitor::AlarmMonitor(std::shared_ptr<MediaPlayer> player, QObject* parent)
    : QObject(parent)
    , mpp(player) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* Receive errors from player */
    QObject::connect(mpp.get(),
        static_cast<void (MediaPlayer::*)(QMediaPlayer::Error)>(
            &MediaPlayer::error),
        [=](QMediaPlayer::Error error) {
            /* if any error occurs while we are expecting an alarm fallback! */
            if (error != QMediaPlayer::NoError && expecting_alarm_playing) {
                qCWarning(CLASS_LC) << "player error occurred";
                trigger_fallback_behavior();
            }
        });

    /* listen to player state changes */
    QObject::connect(mpp.get(), &MediaPlayer::playback_state_changed,
        [=](QMediaPlayer::State state) {
            /* check if alarm was stopped without error (user stopped) */
            if (state == QMediaPlayer::StoppedState ||
                state == QMediaPlayer::PausedState) {
                if (mpp->error() == QMediaPlayer::NoError) {
                    fallback_alarm_timer.stop();
                }
            }
            qCDebug(CLASS_LC) << " AlarmMonitor received " << state
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
}

/*****************************************************************************/
void AlarmMonitor::alarm_triggered(
    std::shared_ptr<DigitalRooster::Alarm> alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    mpp->set_media(alarm->get_media());
    mpp->set_volume(alarm->get_volume());
    mpp->play();
    expecting_alarm_playing = true;
    fallback_alarm_timer.start(fallback_timeout);
}

/*****************************************************************************/
void AlarmMonitor::trigger_fallback_behavior() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (expecting_alarm_playing) {
        qCWarning(CLASS_LC) << "Player has not started in due time!";
        expecting_alarm_playing = false;
        fallback_alarm.setCurrentIndex(0);
        mpp->set_volume(80);
        mpp->set_playlist(&fallback_alarm);
        mpp->play();
    }
}

/*****************************************************************************/
