// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QDateTime>
#include <QDebug>
#include <QSignalSpy>
#include <QUrl>

#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "player_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;
using ::testing::AtLeast;
using ::testing::NiceMock;
using namespace std::chrono_literals;

/*****************************************************************************/
TEST(AlarmMonitor, playsAlarmFuture) {
    PlayerMock player;
    AlarmMonitor mon(player);
    ASSERT_EQ(mon.get_state(), AlarmMonitor::Idle);

    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(3), Alarm::Daily);

    EXPECT_CALL(player, do_play()).Times(1);
    EXPECT_CALL(player, do_stop()).Times(1);

    EXPECT_CALL(player, do_set_volume(_)).Times(1);
    EXPECT_CALL(player, do_set_media(_)).Times(1);

    mon.alarm_triggered(alm.get());
    ASSERT_EQ(mon.get_state(), AlarmMonitor::ExpectingAlarm);
    player.stop();
}

/*****************************************************************************/
TEST(AlarmMonitor, triggersFallbackForError) {
    PlayerMock player;
    AlarmMonitor mon(player);
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(1), Alarm::Daily);

    EXPECT_CALL(player, do_play()).Times(2);
    EXPECT_CALL(player, do_set_media(_)).Times(1);
    EXPECT_CALL(player, do_set_volume(DEFAULT_ALARM_VOLUME)).Times(1);

    // Fallback behavior
    EXPECT_CALL(player, do_set_volume(50)).Times(1);
    EXPECT_CALL(player, do_set_playlist(_)).Times(1);

    QSignalSpy spy(&mon, SIGNAL(state_changed(AlarmMonitor::MonitorState)));
    ASSERT_TRUE(spy.isValid());

    mon.alarm_triggered(alm.get());
    player.emitError(QMediaPlayer::NetworkError);

    ASSERT_EQ(spy.count(), 2); // ExpectingAlarm, FallBackMode
    ASSERT_EQ(mon.get_state(), AlarmMonitor::FallBackMode);
    std::this_thread::sleep_for(500ms);
}
/*****************************************************************************/
TEST(AlarmMonitor, triggersFallbackForTimeout) {
    // Nice mock - we don't care about calls to player
    NiceMock<PlayerMock> player;
    AlarmMonitor mon(player, 20ms);

    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(1), Alarm::Daily);

    QSignalSpy spy(&mon, SIGNAL(state_changed(AlarmMonitor::MonitorState)));
    ASSERT_TRUE(spy.isValid());

    mon.alarm_triggered(alm.get());
    spy.wait(50); // ExpectingAlarm
    spy.wait(50); // FallbackMode
    ASSERT_EQ(spy.count(), 2);
    ASSERT_EQ(mon.get_state(), AlarmMonitor::FallBackMode);
}

/*****************************************************************************/
TEST(AlarmMonitor, noFallBackIfStoppedNormally) {
    PlayerMock player;
    AlarmMonitor mon(player);

    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(1), Alarm::Daily);

    EXPECT_CALL(player, do_play()).Times(1);
    EXPECT_CALL(player, do_set_media(_)).Times(1);
    EXPECT_CALL(player, do_set_volume(DEFAULT_ALARM_VOLUME)).Times(1);
    EXPECT_CALL(player, do_error())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(QMediaPlayer::NoError));

    mon.alarm_triggered(alm.get());
    ASSERT_EQ(mon.get_state(), AlarmMonitor::ExpectingAlarm);
    player.playback_state_changed(QMediaPlayer::PlayingState);
    ASSERT_EQ(mon.get_state(), AlarmMonitor::AlarmActive);
    player.playback_state_changed(QMediaPlayer::StoppedState);
    ASSERT_EQ(mon.get_state(), AlarmMonitor::Idle);
}
