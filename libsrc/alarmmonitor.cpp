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
AlarmMonitor::AlarmMonitor(std::shared_ptr<MediaPlayer> player,
    std::chrono::milliseconds fallback_timeout, QObject* parent)
    : QObject(parent)
    , mpp(player)
    , timeout(fallback_timeout) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* Receive errors from player */
    QObject::connect(mpp.get(),
        static_cast<void (MediaPlayer::*)(QMediaPlayer::Error)>(
            &MediaPlayer::error),
        [=](QMediaPlayer::Error error) {
            /* if any error occurs while we are expecting an alarm fallback! */
            if (error != QMediaPlayer::NoError && state == ExpectingAlarm) {
                qCWarning(CLASS_LC) << "player error occurred";
                trigger_fallback_behavior();
            }
        });

    /* listen to player state changes */
    QObject::connect(mpp.get(), &MediaPlayer::playback_state_changed,
        [=](QMediaPlayer::State player_state) {
            /* check if alarm was stopped without error (user stopped) */
            if (player_state == QMediaPlayer::StoppedState ||
                player_state == QMediaPlayer::PausedState) {
                if (mpp->error() == QMediaPlayer::NoError) {
                    fallback_alarm_timer.stop();
                    set_state(Idle);
                } else {
                    qCWarning(CLASS_LC) << "Player stopped with error!";
                }
            }
            if (player_state == QMediaPlayer::PlayingState) {
                set_state(AlarmActive);
            }
            qCDebug(CLASS_LC) << " AlarmMonitor received " << player_state
                              << " Player error: " << mpp->error();
        });

    /* setup fallback behavior */
    fallback_alarm.addMedia(QMediaContent(QUrl("qrc:/TempleBell.mp3")));
    fallback_alarm.setPlaybackMode(QMediaPlaylist::Loop);

    /**
     * Disarm the fallback behavior after some time has passed
     */
    fallback_alarm_timer.setSingleShot(true);
    fallback_alarm_timer.setInterval(timeout);
    QObject::connect(&fallback_alarm_timer, &QTimer::timeout,
        [=]() { trigger_fallback_behavior(); });
}

/*****************************************************************************/
void AlarmMonitor::set_state(MonitorState next_state) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    state = next_state;
    emit state_changed(state);
}

/*****************************************************************************/
void AlarmMonitor::alarm_triggered(
    std::shared_ptr<DigitalRooster::Alarm> alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    mpp->set_media(alarm->get_media());
    mpp->set_volume(alarm->get_volume());
    mpp->play();
    set_state(ExpectingAlarm);
    fallback_alarm_timer.start(timeout);
}

/*****************************************************************************/
void AlarmMonitor::trigger_fallback_behavior() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == ExpectingAlarm) {
        qCWarning(CLASS_LC) << "Player has not started in due time!";
        fallback_alarm.setCurrentIndex(0);
        set_state(FallBackMode);
        mpp->set_volume(80);
        mpp->set_playlist(&fallback_alarm);
        mpp->play();
    } else {
        qCWarning(CLASS_LC) << Q_FUNC_INFO << " while not expecting!";
    }
}

/*****************************************************************************/
