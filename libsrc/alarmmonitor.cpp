// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */


#include <QLoggingCategory>

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "appconstants.hpp"
#include "mediaplayer.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.AlarmMonitor");

/*****************************************************************************/
AlarmMonitor::AlarmMonitor(MediaPlayer& player,
    std::chrono::milliseconds fallback_timeout, QObject* parent)
    : QObject(parent)
    , mpp(player)
    , timeout(fallback_timeout) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* Receive errors from player */
    QObject::connect(&mpp,
        static_cast<void (MediaPlayer::*)(QMediaPlayer::Error)>(
            &MediaPlayer::error),
        [&](QMediaPlayer::Error error) {
            /* if any error occurs while we are expecting or playing an alarm */
            if (error != QMediaPlayer::NoError &&
                (state == ExpectingAlarm || state == AlarmActive)) {
                qCWarning(CLASS_LC) << "player error occurred";
                trigger_fallback_behavior();
            }
        });

    /* listen to player state changes */
    QObject::connect(&mpp, &MediaPlayer::playback_state_changed,
        [&](QMediaPlayer::State player_state) {
            qCDebug(CLASS_LC) << "AlarmMonitor PlayerState:" << player_state
                              << "PlayerError:" << mpp.error();
            /* check if alarm was stopped without error (user stopped) */
            if (player_state == QMediaPlayer::StoppedState ||
                player_state == QMediaPlayer::PausedState) {
                if (mpp.error() == QMediaPlayer::NoError) {
                    qCDebug(CLASS_LC) << " Stopped player without error";
                    set_state(Idle);
                } else {
                    qCWarning(CLASS_LC) << "Player stopped with error!";
                }
            }
            if (player_state == QMediaPlayer::PlayingState) {
                set_state(AlarmActive);
            }
        });

    /* setup fallback behavior */
    fallback_alarm.addMedia(QMediaContent(QUrl("qrc:/TempleBell.mp3")));
    fallback_alarm.setPlaybackMode(QMediaPlaylist::Loop);

    /**
     * Disarm the fallback behavior after some time has passed
     */
    fallback_alarm_timer.setSingleShot(true);
    fallback_alarm_timer.setInterval(timeout);
    QObject::connect(&fallback_alarm_timer, &QTimer::timeout, [&]() {
        qCWarning(CLASS_LC) << "fallback_alarm_timer.timeout()";
        if (state == ExpectingAlarm) {
            trigger_fallback_behavior();
        }
    });
}

/*****************************************************************************/
void AlarmMonitor::set_state(MonitorState next_state) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << state << "-->" << next_state;
    state = next_state;
    emit state_changed(state);
}

/*****************************************************************************/
void AlarmMonitor::alarm_triggered(const DigitalRooster::Alarm* alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    set_state(ExpectingAlarm);
    mpp.set_media(alarm->get_media());
    mpp.set_volume(alarm->get_volume());
    mpp.play();
    fallback_alarm_timer.start(timeout);
}

/*****************************************************************************/
void AlarmMonitor::trigger_fallback_behavior() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    fallback_alarm.setCurrentIndex(0);
    set_state(FallBackMode);
    mpp.set_volume(DEFAULT_FALLBACK_VOLUME);
    mpp.set_playlist(&fallback_alarm);
    mpp.play();
}

/*****************************************************************************/
