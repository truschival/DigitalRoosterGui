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
#include <QSignalSpy>
#include <QUrl>

#include <chrono>
#include <memory>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "sleeptimer.hpp"

#include "cm_mock.hpp"
#include "player_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using ::testing::AtLeast;

/*****************************************************************************/
TEST(SleepTimer, stopsRunningAlarm) {
    auto cm = std::make_shared<CmMock>();
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::currentTime().addSecs(3), Alarm::Daily);

    /* sleep timeout 500 ms */
    EXPECT_CALL(*(cm.get()), get_sleep_timeout())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(duration_cast<minutes>(500ms)));

    SleepTimer dut(cm);
    alm->set_timeout(duration_cast<minutes>(500ms));
    QSignalSpy spy(&dut, SIGNAL(sleep_timer_elapsed()));
    ASSERT_TRUE(spy.isValid());
    // dispatch alarm
    dut.alarm_triggered(alm);

    spy.wait(1000);
    ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/
TEST(SleepTimer, stopsPlayer) {
    auto cm = std::make_shared<CmMock>();
    /* sleep timeout 500 ms */
    EXPECT_CALL(*(cm.get()), get_sleep_timeout())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(duration_cast<minutes>(500ms)));

    SleepTimer dut(cm);
    QSignalSpy spy(&dut, SIGNAL(sleep_timer_elapsed()));
    ASSERT_TRUE(spy.isValid());
    // start player
    dut.playback_state_changed(QMediaPlayer::PlayingState);

    spy.wait(1000);
    ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/
TEST(SleepTimer, PlayerStoppedDoesNotEmitSignal) {
    auto cm = std::make_shared<CmMock>();
    /* sleep timeout 500 ms */
    EXPECT_CALL(*(cm.get()), get_sleep_timeout())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(duration_cast<minutes>(500ms)));

    SleepTimer dut(cm);
    QSignalSpy spy(&dut, SIGNAL(sleep_timer_elapsed()));
    ASSERT_TRUE(spy.isValid());
    // start Player
    dut.playback_state_changed(QMediaPlayer::PlayingState);
    dut.playback_state_changed(QMediaPlayer::StoppedState);
    spy.wait(1000);
    ASSERT_EQ(spy.count(), 0); // should never be called
}

/*****************************************************************************/

TEST(SleepTimer, TimeoutChangeEmitsSignal) {
    auto cm = std::make_shared<CmMock>();
    /* sleep timeout 500 ms */
    EXPECT_CALL(*(cm.get()), get_sleep_timeout())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(duration_cast<minutes>(500ms)));

    SleepTimer dut(cm);
    QSignalSpy spy(&dut, SIGNAL(sleep_timeout_changed(int)));
    ASSERT_TRUE(spy.isValid());
    // dispatch alarm
    dut.set_sleep_timeout(5); // use int signature
    ASSERT_EQ(spy.count(), 1); // expect 1 timeout change
    auto args = spy.first();
    bool valid = true;
    auto to = args.at(0).toInt(&valid);
    ASSERT_TRUE(valid);
    ASSERT_EQ(to,5);
}

/*****************************************************************************/
