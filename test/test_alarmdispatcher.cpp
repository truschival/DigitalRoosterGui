// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

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
#include "alarmdispatcher.hpp"
#include "cm_mock.hpp"
#include "mock_clock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;
using ::testing::NiceMock;

/*****************************************************************************/
// Fixture to inject fake clock as the global clock
class AlarmDispatcherFixture : public ::testing::Test {
public:
    AlarmDispatcherFixture()
        : mc(new NiceMock<MockClock>) {
        ON_CALL(cm, get_alarms()).WillByDefault(ReturnRef(cm.alarms));

        alm1 = std::make_shared<DigitalRooster::Alarm>(
            QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
            QTime::fromString("08:30:00", "hh:mm:ss"), Alarm::Daily);
        cm.alarms.push_back(alm1);

        // need to make a pointer because AlarmDispatcher inherits QObject which
        // is not copyable
        dut = std::make_unique<DigitalRooster::AlarmDispatcher>(cm);
        // QSignalSpy and spy->wait needed for Qt Eventloop processing,
        // otherwise timers are not started correctly
        spy = std::make_unique<QSignalSpy>(
            dut.get(), SIGNAL(upcoming_alarm_info_changed(QString)));
    };

    // Make our own clock to be the wallclock
    void SetUp() {
        DigitalRooster::wallclock =
            std::static_pointer_cast<TimeProvider, MockClock>(mc);
    };

    // Restore original TimeProvider for other tests
    void TearDown() {
        DigitalRooster::wallclock = std::make_shared<TimeProvider>();
    };

protected:
    std::shared_ptr<NiceMock<MockClock>> mc;
    NiceMock<CmMock> cm;
    std::shared_ptr<DigitalRooster::Alarm> alm1;
    std::unique_ptr<DigitalRooster::AlarmDispatcher> dut;
    std::unique_ptr<QSignalSpy> spy;
};

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, CheckFixture) {
    ASSERT_EQ(cm.get_alarms().size(), 1);
    ASSERT_TRUE(spy->isValid());
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, InfoEmptyAlarms) {
    ON_CALL(*(mc.get()), get_time())
        .WillByDefault(
            Return(QDateTime::fromString("2020-11-22T19:00:00", Qt::ISODate)));
    cm.alarms.clear();
    ASSERT_EQ(cm.get_alarms().size(), 0);

    dut->check_alarms();
    auto result = dut->get_upcoming_alarm();
    ASSERT_EQ(result, nullptr);
    ASSERT_EQ(dut->get_upcoming_alarm_info(), QString(""));
    dut->check_alarms();

    spy->wait(100);
    auto delta = dut->get_remaining_time();
    ASSERT_EQ(delta.count(), -1);
    ASSERT_EQ(dut->get_upcoming_alarm_info(), QString(""));
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, InfoDisabledAlarms) {
    ON_CALL(*(mc.get()), get_time())
        .WillByDefault(
            Return(QDateTime::fromString("2020-11-22T19:00:00", Qt::ISODate)));

    alm1->enable(false);
    dut->check_alarms();
    auto result = dut->get_upcoming_alarm();
    ASSERT_EQ(dut->get_upcoming_alarm_info(), QString(""));
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, SortAlarms) {
    auto alm2 = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:00:00", "hh:mm:ss"), Alarm::Daily);
    alm2->enable(false);
    cm.alarms.push_back(alm2);

    auto alm3 = std::make_shared<DigitalRooster::Alarm>(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:29:40", "hh:mm:ss"), Alarm::Daily);
    cm.alarms.push_back(alm3);

    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AnyNumber())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-22T19:00:00", Qt::ISODate)));

    ASSERT_TRUE(*alm1.get() < *alm2.get());

    auto result = dut->get_upcoming_alarm();
    ASSERT_EQ(result, alm3);
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, RemainingTime) {
    /* 10 seconds to dispatch */
    ON_CALL(*(mc.get()), get_time())
        .WillByDefault(
            Return(QDateTime::fromString("2020-11-22T08:29:50", Qt::ISODate)));
    dut->check_alarms();
    spy->wait(200);
    auto delta = dut->get_remaining_time();
    ASSERT_LE(delta.count(), 10500);
    ASSERT_GE(delta.count(), 9000);
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, DisablingAlarmStopsTimer) {
    /* 10 seconds to dispatch */
    ON_CALL(*(mc.get()), get_time())
        .WillByDefault(
            Return(QDateTime::fromString("2020-11-22T08:29:50", Qt::ISODate)));
    dut->check_alarms();
    spy->wait(200);
    auto delta = dut->get_remaining_time();
    ASSERT_LT(delta.count(), 10500);

    alm1->enable(false);
    dut->check_alarms();
    spy->wait(100);
    auto next_instance = dut->get_upcoming_alarm_info();
    ASSERT_EQ(next_instance, QString(""));
    auto delta2 = dut->get_remaining_time();
    ASSERT_EQ(delta2.count(), -1);
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, AlarmTriggers) {
    /* 1 seconds to dispatch */
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AnyNumber())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-22T08:29:59", Qt::ISODate)));

    QSignalSpy spy2(dut.get(), SIGNAL(alarm_triggered(DigitalRooster::Alarm*)));
    ASSERT_TRUE(spy2.isValid());

    dut->check_alarms();
    spy->wait(1100);
    ASSERT_EQ(spy->count(), 2);
    ASSERT_EQ(spy2.count(), 1);
}

/*****************************************************************************/
TEST_F(AlarmDispatcherFixture, AlarmTriggersReschedule) {
    /* 1 seconds to dispatch */
    EXPECT_CALL(*(mc.get()), get_time())
        .WillOnce(/* Sun 8:29 */
            Return(QDateTime::fromString("2020-11-22T08:29:59", Qt::ISODate)))
        .WillOnce(
            Return(QDateTime::fromString("2020-11-22T08:29:59", Qt::ISODate)))
        .WillOnce(
            Return(QDateTime::fromString("2020-11-22T08:29:59", Qt::ISODate)))
        .WillRepeatedly(/* Sun 8:35*/
            Return(QDateTime::fromString("2020-11-22T08:35:59", Qt::ISODate)));

    QSignalSpy spy2(dut.get(), SIGNAL(alarm_triggered(DigitalRooster::Alarm*)));
    ASSERT_TRUE(spy2.isValid());

    dut->check_alarms();
    auto next_instance = dut->get_upcoming_alarm_info();
    ASSERT_EQ(next_instance, QString("Sun 08:30"));
    spy->wait(1100);
    ASSERT_EQ(spy->count(), 2);
    ASSERT_EQ(spy2.count(), 1);
    spy->wait(100); // Let event loop run

    next_instance = dut->get_upcoming_alarm_info();
    ASSERT_EQ(next_instance, QString("Mon 08:30"));
    auto next_delta = dut->get_remaining_time();

    // We have a running timer with trigger time more than 23hrs in the future
    ASSERT_GT(next_delta.count(), 23 * 3600 * 1000);
    ASSERT_LT(next_delta.count(), 24 * 3600 * 1000);
}
