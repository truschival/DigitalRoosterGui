/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QDateTime>
#include <QDebug>
#include <QSignalSpy>
#include <QUrl>
#include <QVector>

#include <chrono>
#include <memory>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "cm_mock.hpp"
#include "player_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;
using ::testing::AtLeast;
using namespace std::chrono_literals;

/*****************************************************************************/
TEST(AlarmMonitor, playsAlarmFuture) {
    auto player = std::make_shared<PlayerMock>();
    AlarmMonitor mon(player);
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(3), Alarm::Daily);

    EXPECT_CALL(*(player.get()), do_play()).Times(1);
    EXPECT_CALL(*(player.get()), do_stop()).Times(1);

    EXPECT_CALL(*(player.get()), do_set_volume(_)).Times(1);
    EXPECT_CALL(*(player.get()), do_set_media(_)).Times(1);

    mon.alarm_triggered(alm);

    player->stop();
}

/*****************************************************************************/
TEST(AlarmMonitor, triggersFallbackForError) {
    auto player = std::make_shared<PlayerMock>();
    AlarmMonitor mon(player);
    auto timepoint = QDateTime::currentDateTimeUtc().addSecs(1);
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(3), Alarm::Daily);

    EXPECT_CALL(*(player.get()), do_play()).Times(2);
    EXPECT_CALL(*(player.get()), do_set_media(_)).Times(1);
    EXPECT_CALL(*(player.get()), do_set_volume(40)).Times(1);

    // Fallback behavior
    EXPECT_CALL(*(player.get()), do_set_volume(80)).Times(1);
    EXPECT_CALL(*(player.get()), do_set_playlist(_)).Times(1);

    mon.alarm_triggered(alm);
    player->emitError(QMediaPlayer::NetworkError);

    std::this_thread::sleep_for(2s);
}

/*****************************************************************************/


