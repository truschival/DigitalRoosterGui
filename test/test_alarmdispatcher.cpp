/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/
#include <QDebug>
#include <QSignalSpy>
#include <QUrl>
#include <QVector>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

class CmMock : public ConfigurationManager {
public:
    CmMock() {
        auto timepoint = QDateTime::currentDateTimeUtc().addSecs(3);
        auto alm =
            std::make_shared<Alarm>(QUrl("https://www.heise.de"), timepoint);
        alarms.push_back(alm);

        qRegisterMetaType<std::shared_ptr<DigitalRooster::Alarm>>(
            "std::shared_ptr<DigitalRooster::Alarm>");
    };
    MOCK_METHOD0(get_alarm_list, QVector<std::shared_ptr<Alarm>>&());
    QVector<std::shared_ptr<Alarm>> alarms;
};
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
    a.set_interval(std::chrono::seconds(1));
    ASSERT_EQ(a.get_interval(), std::chrono::seconds(1));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(2)
        .WillRepeatedly(ReturnRef(cm->alarms));

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    spy.wait(5000);
    ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/
