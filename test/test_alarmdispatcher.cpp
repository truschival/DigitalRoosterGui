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
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "cm_mock.hpp"
#include "mock_clock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/

// Fixture to inject fake clock as the global clock
class AlarmDispatcherFixture : public ::testing::Test {
public:
    // Make our own clock to be the wallclock
    void SetUp() {
        cm = std::make_shared<CmMock>();
        mc = std::make_shared<MockClock>();
        DigitalRooster::wallclock =
            std::static_pointer_cast<TimeProvider, MockClock>(mc);
    };

    // Restore original TimeProvider for other tests
    void TearDown() {
        DigitalRooster::wallclock = std::make_shared<TimeProvider>();
    };

protected:
    std::shared_ptr<MockClock> mc;
    std::shared_ptr<CmMock> cm;
};

/*****************************************************************************/

TEST_F(AlarmDispatcherFixture, callsGetAlarms) {
    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    // Friday 8:29:45, just before and just before and after the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate)));

    AlarmDispatcher a(cm);
    a.check_alarms();
}
/*****************************************************************************/

TEST_F(AlarmDispatcherFixture, dispatchSingleAlarm) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"), Alarm::Daily);
    cm->alarms.push_back(alm);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));

    // Friday 8:29:45, just before and just before and after the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate)));

    AlarmDispatcher a(cm);
    QSignalSpy spy1(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy1.isValid());

    QSignalSpy spy2(&a, SIGNAL(alarm_triggered()));
    ASSERT_TRUE(spy2.isValid());

    a.check_alarms();
    ASSERT_EQ(spy1.count(), 1);
    ASSERT_EQ(spy2.count(), 1);
}

/*****************************************************************************/

TEST_F(AlarmDispatcherFixture, disableSingleAlarmAfterDispatch) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"), Alarm::Once);
    cm->alarms.push_back(alm);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    // Friday 8:29:45, just before and just before and after the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate)));

    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.check_alarms();
    ASSERT_FALSE(alm->is_enabled());
    ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/

TEST_F(AlarmDispatcherFixture, DontDispatchDisabledAlarm) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"), Alarm::Daily, false);
    cm->alarms.push_back(alm);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));

    // just before and just after the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:30:02", Qt::ISODate)));

    AlarmDispatcher a(cm);

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.check_alarms();
    ASSERT_EQ(spy.count(), 0);
}
/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, DontDispatchPastAlarms) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:25:00", "hh:mm:ss"), Alarm::Daily, true);
    cm->alarms.push_back(alm);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    // 1 Minute after Alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:26:00", Qt::ISODate)));
    AlarmDispatcher a(cm);

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    ASSERT_TRUE(spy.isValid());
    a.check_alarms();
    ASSERT_EQ(spy.count(), 0);
}
/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, DispatchAlarmsWithSlightOffset) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"), Alarm::Workdays, true);
    cm->alarms.push_back(alm);

    // Friday 8:29:45, just before and just before and after the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(2))
        .WillOnce(
            Return(QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-28T08:30:20", Qt::ISODate)));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(2)
        .WillRepeatedly(ReturnRef(cm->alarms));
    AlarmDispatcher a(cm);

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());
    a.check_alarms();
    a.check_alarms();
    ASSERT_EQ(spy.count(), 1); // past alarm should not be played
}
/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, Workdays_Friday) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"), Alarm::Workdays, true);
    cm->alarms.push_back(alm);

    // Friday 8:29:45, just before the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate)));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    AlarmDispatcher a(cm);

    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    a.check_alarms();
    ASSERT_EQ(spy.count(), 1);
}
/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, Workdays_Monday) {
    // Monday 8:30
    auto timepoint = QTime::fromString("08:30:00", "hh:mm:ss");
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"), timepoint,
        Alarm::Workdays, true);
    cm->alarms.push_back(alm);

    // Monday 8:29
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-14T08:29:45", Qt::ISODate)));

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
TEST_F(AlarmDispatcherFixture, Weekends_Monday) {
    // Monday 8:30
    auto timepoint = QTime::fromString("08:30:00", "hh:mm:ss");
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"), timepoint,
        Alarm::Weekend, true);
    cm->alarms.push_back(alm);

    // Monday 8:29
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-14T08:29:45", Qt::ISODate)));


    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));

    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    a.check_alarms();
    ASSERT_EQ(spy.count(), 0);
}
/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, Weekends_Saturday) {
    auto timepoint = QTime::fromString("08:30:00", "hh:mm:ss");
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"), timepoint,
        Alarm::Weekend, true);
    cm->alarms.push_back(alm);

    // Saturday  8:29
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-29T08:29:45", Qt::ISODate)));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));

    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    a.check_alarms();
    ASSERT_EQ(spy.count(), 1);
}
/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, Workdays_Sunday) {
    // Monday 8:30
    auto timepoint = QTime::fromString("08:30:00", "hh:mm:ss");
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"), timepoint,
        Alarm::Weekend, true);
    cm->alarms.push_back(alm);

    // Friday 8:29:45, just before the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-28T08:29:45", Qt::ISODate)));

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));

    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    a.check_alarms();
    ASSERT_EQ(spy.count(), 0);
}


/*****************************************************************************/

TEST_F(AlarmDispatcherFixture, dispatch2DueAlarms) {
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:29:48", "hh:mm:ss"), Alarm::Once);
    cm->alarms.push_back(alm);
    auto alm2 = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:29:50", "hh:mm:ss"), Alarm::Daily);
    cm->alarms.push_back(alm2);

    ASSERT_EQ(cm->alarms.size(), 2);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->alarms));
    // Friday 8:29:45, just before the alarm time
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-28T08:29:45", Qt::ISODate)));

    AlarmDispatcher a(cm);
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    a.check_alarms();
    ASSERT_EQ(spy.count(), 2);
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, LoopTimerTriggers) {
    auto cm = std::make_shared<CmMock>();
    auto alm = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:29:40", "hh:mm:ss"), Alarm::Daily);
    cm->alarms.push_back(alm);

    EXPECT_CALL(*(cm.get()), get_alarm_list())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(cm->alarms));

    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-28T08:29:40", Qt::ISODate)));

    /* Interval 1s */
    AlarmDispatcher a(cm);
    a.set_interval(std::chrono::seconds(1));
    ASSERT_EQ(a.get_interval(), std::chrono::seconds(1));

    // Only observable signal, actually we want to test if it loops
    QSignalSpy spy(
        &a, SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));
    ASSERT_TRUE(spy.isValid());

    spy.wait(1100);
    spy.wait(1100);
    ASSERT_GE(spy.count(), 2);
}

/*****************************************************************************/
