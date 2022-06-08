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
                fallback_alarm_timer.isActive()) {
                qCWarning(CLASS_LC) << "player error occurred";
                trigger_fallback_behavior();
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
    QObject::connect(
        &fallback_alarm_timer, &QTimer::timeout, [&]() {
            qCDebug(CLASS_LC)
                << "fallback_alarm_timer elapsed without player error!";
            set_state(Idle);
        });
}

/*****************************************************************************/
void AlarmMonitor::stop() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    fallback_alarm_timer.stop();
    set_state(Idle);
}

/*****************************************************************************/
void AlarmMonitor::set_state(MonitorState next_state) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << state << "-->" << next_state;
    state = next_state;
}

/*****************************************************************************/
void AlarmMonitor::alarm_triggered(const DigitalRooster::Alarm* alarm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    set_state(Armed);
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
