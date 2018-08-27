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
#include <QDebug>
#include <QSignalSpy>
#include <QUrl>
#include <QVector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"

#include "cm_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/

TEST(AlarmDispatcher, callsGetAlarms) {
    auto cm = std::make_shared<CmMock>();
    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    AlarmDispatcher a(cm);
    a.check_alarms();
}
/*****************************************************************************/

TEST(AlarmDispatcher, dispatchAlarm) {
    auto cm = std::make_shared<CmMock>();
    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.check_alarms();
    ASSERT_EQ(spy.count(), 1);
}
/*****************************************************************************/

TEST(AlarmDispatcher, DontdispatchDisabledAlarm) {
    auto cm = std::make_shared<CmMock>();
    cm->alarms[0]->enable(false);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    AlarmDispatcher a(cm);

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    ASSERT_TRUE(spy.isValid());
    a.check_alarms();
    ASSERT_EQ(spy.count(), 0);
}
/*****************************************************************************/

TEST(AlarmDispatcher, dispatch2DueAlarms) {
    auto cm = std::make_shared<CmMock>();
    auto timepoint = QDateTime::currentDateTimeUtc();
    cm->alarms.push_back(std::make_shared<Alarm>(
        QUrl("https://www.heise.de"), timepoint.addSecs(3)));

    ASSERT_EQ(cm->alarms.size(), 2);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.check_alarms();
    ASSERT_EQ(spy.count(), 2);
}
/*****************************************************************************/

TEST(AlarmDispatcher, dispatch1DueOf2Alarms) {
    auto cm = std::make_shared<CmMock>();
    AlarmDispatcher a(cm);
    a.set_interval(std::chrono::seconds(3));

    auto timepoint = QDateTime::currentDateTimeUtc();
    cm->alarms.push_back(std::make_shared<Alarm>(
        QUrl("https://www.heise.de"), timepoint.addSecs(5)));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(cm->alarms));

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.check_alarms();
    ASSERT_EQ(spy.count(), 1);
}
/*****************************************************************************/

TEST(AlarmDispatcher, willTriggerItself) {
    auto cm = std::make_shared<CmMock>();
    AlarmDispatcher a(cm);
    a.set_interval(std::chrono::seconds(1));
    ASSERT_EQ(a.get_interval(), std::chrono::seconds(1));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(cm->alarms));

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());
    spy.wait(3000);

    ASSERT_EQ(spy.count(), 1);
}
/*****************************************************************************/
TEST(AlarmDispatcher, LoopTimerTriggers) {
    auto cm = std::make_shared<CmMock>();
    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.set_interval(std::chrono::seconds(1));
    ASSERT_EQ(a.get_interval(), std::chrono::seconds(1));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(AtLeast(2))
        .WillRepeatedly(ReturnRef(cm->alarms));

    spy.wait(1200);
    spy.wait(1200);
    spy.wait(1200);
    ASSERT_GE(spy.count(), 2);
}

/*****************************************************************************/
