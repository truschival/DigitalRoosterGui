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
#include <QTime>
#include <QUrl>
#include <QUuid>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "alarm.hpp"
#include "appconstants.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

// Mock Wallclock to test weekends & workdays
#include "timeprovider.hpp"

class MockClock : public DigitalRooster::TimeProvider {
public:
    MOCK_METHOD0(get_time, QDateTime());
};

// Fixture to inject fake clock as the global clock
class AlarmFakeTime : public ::testing::Test {
public:
    // Make our own clock to be the wallclock
    void SetUp() {
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
};


/*****************************************************************************/
TEST(Alarm, defaultVolume) {
    Alarm al;
    ASSERT_EQ(al.get_volume(), 40);
}

/*****************************************************************************/
TEST(Alarm, fullConstructorEnabled) {
    Alarm al;
    Alarm al2(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QDateTime::currentDateTime().addSecs(3600));
    ASSERT_TRUE(al.is_enabled());
    ASSERT_TRUE(al2.is_enabled());
}
/*****************************************************************************/

TEST(Alarm, OnceTodayFuture) {
    auto timepoint = QDateTime::currentDateTime().addSecs(3600);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Once);

    ASSERT_EQ(al.get_period(), Alarm::Once);
    ASSERT_EQ(al.get_next_trigger(), timepoint);
}
/*****************************************************************************/
TEST(Alarm, OnceTodayPast) {
    auto timepoint = QDateTime::currentDateTime().addSecs(-3600); // in one hour

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Once);

    ASSERT_EQ(al.get_period(), Alarm::Once);
    ASSERT_EQ(al.get_next_trigger(), timepoint);
}
/*****************************************************************************/

TEST(Alarm, DailyPast) {
    auto timepoint = QTime::currentTime().addSecs(-3600);
    auto expected_trigger =
        QDateTime::currentDateTime().addDays(1).addSecs(-3600);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Daily);

    ASSERT_EQ(al.get_period(), Alarm::Daily);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, Once) {
    auto timepoint = QDateTime::fromString("2018-09-26T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Once, true);
    ASSERT_EQ(al.get_period(), Alarm::Once);

    // We run Friday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-26T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:35:00", Qt::ISODate)));

    // Call today in one two minutes
    auto expected_trigger = timepoint;
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    // ... and it should not change
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}
/*****************************************************************************/

TEST_F(AlarmFakeTime, Daily) {

    auto timepoint = QDateTime::fromString("2018-09-22T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Daily, true);
    ASSERT_EQ(al.get_period(), Alarm::Daily);

    // We run Friday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-26T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:31:00", Qt::ISODate)));

    // Call today in one two minutes
    auto expected_trigger =
        QDateTime::fromString("2018-09-26T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    // Tomorrow morning
    expected_trigger =
        QDateTime::fromString("2018-09-27T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, Workdays) {
    auto timepoint = QDateTime::fromString("2018-09-26T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Workdays, true);
    ASSERT_EQ(al.get_period(), Alarm::Workdays);

    // We run Friday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-26T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-26T08:31:00", Qt::ISODate)));

    // Call today in one two minutes
    auto expected_trigger =
        QDateTime::fromString("2018-09-26T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    // Tomorrow morning
    expected_trigger =
        QDateTime::fromString("2018-09-27T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, WorkdaysRunFriday) {
    auto timepoint = QDateTime::fromString("2018-09-28T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Workdays, true);
    ASSERT_EQ(al.get_period(), Alarm::Workdays);

    // We run Friday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-28T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-28T08:31:00", Qt::ISODate)));

    // Today right now (we called at 8:28)
    auto expected_trigger = timepoint;
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    // Expect to be called on next monday morning
    expected_trigger =
        QDateTime::fromString("2018-10-01T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, WorkdaysRunSaturday) {
    auto timepoint = QDateTime::fromString("2018-09-27T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Workdays, true);
    ASSERT_EQ(al.get_period(), Alarm::Workdays);

    // We run Friday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-29T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-29T08:31:00", Qt::ISODate)));

    // In either case expect to be called on next monday morning
    auto expected_trigger =
        QDateTime::fromString("2018-10-01T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, WorkdaysRunSunday) {
    auto timepoint = QDateTime::fromString("2018-09-27T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Workdays, true);
    ASSERT_EQ(al.get_period(), Alarm::Workdays);

    // We run Friday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-30T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-30T08:31:00", Qt::ISODate)));

    // In either case expect to be called on next monday morning
    auto expected_trigger =
        QDateTime::fromString("2018-10-01T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}


/*****************************************************************************/
TEST_F(AlarmFakeTime, WeekendsRunMonday) {
    auto timepoint = QDateTime::fromString("2018-09-27T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Weekend, true);
    ASSERT_EQ(al.get_period(), Alarm::Weekend);

    // We run Monday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-24T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-24T08:31:00", Qt::ISODate)));

    // Expect to be called on Saturday morning
    auto expected_trigger =
        QDateTime::fromString("2018-10-29T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, WeekendsRunFriday) {
    auto timepoint = QDateTime::fromString("2018-09-27T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Weekend, true);
    ASSERT_EQ(al.get_period(), Alarm::Weekend);

    // We run Sunday at 8:29 (before the expected alarm)
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(AtLeast(1))

        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-28T08:29:00", Qt::ISODate)));

    // Expect to be called on Saturday Morning
    auto expected_trigger =
        QDateTime::fromString("2018-10-01T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, WeekendsConstructFromTimeRunSaturday) {
    // First call from constructor, e.g. alarm was generated thursday 27/09
    // Then call update  trigger friday afternoon
    // We run saturday at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(4)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-27T15:10:00", Qt::ISODate)))
        .WillOnce(
            Return(QDateTime::fromString("2018-09-27T15:15:00", Qt::ISODate)))
        .WillOnce(
            Return(QDateTime::fromString("2018-09-29T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-29T08:31:00", Qt::ISODate)));

    auto alarmtime = QTime::fromString("08:30:00", "hh:mm:ss");

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), alarmtime,
        Alarm::Weekend, true);
    ASSERT_EQ(al.get_period(), Alarm::Weekend);

    // Expect to be called on same day, in a minute
    auto expected_trigger =
        QDateTime::fromString("2018-09-29T08:30:00", Qt::ISODate);
    // call on friday afternoon
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    // call on Saturday morning afternoon
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
    // and on sunday morning
    expected_trigger =
        QDateTime::fromString("2018-09-30T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}


/*****************************************************************************/
TEST_F(AlarmFakeTime, WeekendsRunSaturday) {
    auto timepoint = QDateTime::fromString("2018-09-28T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Weekend, true);
    ASSERT_EQ(al.get_period(), Alarm::Weekend);

    // We run saturday at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-29T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-29T08:31:00", Qt::ISODate)));

    // Expect to be called on same day, in a minute
    auto expected_trigger =
        QDateTime::fromString("2018-09-29T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);

    // and on sunday morning
    expected_trigger =
        QDateTime::fromString("2018-09-30T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST_F(AlarmFakeTime, WeekendsRunSunday) {
    auto timepoint = QDateTime::fromString("2018-09-30T08:30:00", Qt::ISODate);
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Weekend, true);
    ASSERT_EQ(al.get_period(), Alarm::Weekend);

    // We run Sunday at at 8:28 and 8:31 just after the timepoint
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(2)
        .WillOnce(
            Return(QDateTime::fromString("2018-09-30T08:28:00", Qt::ISODate)))
        .WillRepeatedly(
            Return(QDateTime::fromString("2018-09-30T08:31:00", Qt::ISODate)));

    // In one minute when invoked first
    auto expected_trigger = timepoint;
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);

    // Expect to be called on next saturday morning
    expected_trigger =
        QDateTime::fromString("2018-10-06T08:30:00", Qt::ISODate);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}


/*****************************************************************************/
TEST(Alarm, updateTiggerSetsPeriodtoOnce) {
    auto expected_trigger = QDateTime::currentDateTime().addSecs(3600);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime(), Alarm::Daily);

    al.set_trigger(expected_trigger);

    ASSERT_EQ(al.get_period(), Alarm::Once);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST(Alarm, updateTiggerSetsPeriodtoWeekend) {
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime(), Alarm::Daily);

    al.set_trigger(QTime::currentTime().addSecs(3600), Alarm::Weekend);

    auto expected_trigger = QDateTime::currentDateTime().addSecs(3600);
    auto dow_today = expected_trigger.date().dayOfWeek();

    if (dow_today != Qt::Saturday && dow_today != Qt::Sunday)
        expected_trigger = expected_trigger.addDays(Qt::Sunday - dow_today);
    if (dow_today == Qt::Saturday)
        expected_trigger = expected_trigger.addDays(1);
    if (dow_today == Qt::Sunday)
        expected_trigger = expected_trigger.addDays(6);

    ASSERT_EQ(al.get_period(), Alarm::Weekend);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST(Alarm, updatePeriodSetsNextTrigger) {
    auto expected_trigger = QDateTime::currentDateTime().addSecs(-3600);
    auto dow_today = expected_trigger.date().dayOfWeek();

    if (dow_today != Qt::Saturday && dow_today != Qt::Sunday)
        expected_trigger = expected_trigger.addDays(Qt::Sunday - dow_today);
    if (dow_today == Qt::Saturday)
        expected_trigger = expected_trigger.addDays(1);
    if (dow_today == Qt::Sunday)
        expected_trigger = expected_trigger.addDays(6);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime().addSecs(-3600));
    ASSERT_EQ(al.get_period(), Alarm::Daily); // Default from constructor
    al.set_period(Alarm::Weekend);

    ASSERT_EQ(al.get_period(), Alarm::Weekend);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST(StringToPeriodEnum, mapping_bad) {
    EXPECT_THROW(json_string_to_alarm_period("Foobar"), std::exception);
}

/*****************************************************************************/
TEST(Alarm, defaultTimeout) {
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime().addSecs(-3600));
    ASSERT_EQ(al.get_timeout().count(), DEFAULT_ALARM_TIMEOUT.count());
}
/*****************************************************************************/
TEST(Alarm, updatedTimeout) {
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime().addSecs(-3600));
    al.set_timeout(std::chrono::minutes(5));
    ASSERT_EQ(al.get_timeout().count(), 5);
}
