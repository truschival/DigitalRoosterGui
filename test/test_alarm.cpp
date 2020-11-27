// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */


#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTime>
#include <QUrl>
#include <QUuid>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "alarm.hpp"
#include "appconstants.hpp"
#include "mock_clock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/

// Fixture to inject fake clock as the global clock
class AlarmNextInstance : public ::testing::Test {
public:
    AlarmNextInstance()
        : mc(new MockClock)
        , al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
              QTime::fromString("08:45:00", "hh:mm:ss")){};

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
    std::shared_ptr<MockClock> mc;
    Alarm al;
};

/*****************************************************************************/
TEST(Alarm, defaultVolume) {
    Alarm al;
    ASSERT_EQ(al.get_volume(), DEFAULT_ALARM_VOLUME);
}

/*****************************************************************************/
TEST(Alarm, mediaUrlChanged) {
    auto newUrl = QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3");
    DigitalRooster::Alarm al(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"));

    QSignalSpy spy_mc(&al, SIGNAL(media_url_changed(QUrl)));
    ASSERT_TRUE(spy_mc.isValid());
    QSignalSpy spy_dc(&al, SIGNAL(dataChanged()));
    ASSERT_TRUE(spy_dc.isValid());
    al.update_media_url(newUrl);
    ASSERT_EQ(spy_mc.count(), 1);
    ASSERT_EQ(spy_dc.count(), 1);
    ASSERT_EQ(al.get_media_url(), newUrl);
}

/*****************************************************************************/
TEST(Alarm, timeChanged) {
    auto newTime = QTime::fromString("09:45:00", "hh:mm:ss");
    DigitalRooster::Alarm al(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"));

    QSignalSpy spy_tc(&al, SIGNAL(time_changed(QTime)));
    ASSERT_TRUE(spy_tc.isValid());
    QSignalSpy spy_dc(&al, SIGNAL(dataChanged()));
    ASSERT_TRUE(spy_dc.isValid());
    al.set_time(newTime);
    ASSERT_EQ(spy_tc.count(), 1);
    ASSERT_EQ(spy_dc.count(), 1);
    ASSERT_EQ(al.get_time(), newTime);
}


/*****************************************************************************/
TEST(Alarm, fullConstructorEnabled) {
    Alarm al;
    Alarm al2(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime().addSecs(3600));
    ASSERT_TRUE(al.is_enabled());
    ASSERT_TRUE(al2.is_enabled());
}

/*****************************************************************************/
TEST(StringToPeriodEnum, mapping_bad) {
    EXPECT_THROW(string_to_alarm_period("Foobar"), std::invalid_argument);
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
    ASSERT_EQ(al.get_timeout(), DEFAULT_ALARM_TIMEOUT);
    al.set_timeout(std::chrono::minutes(5));
    ASSERT_EQ(al.get_timeout().count(), 5);
}

/*****************************************************************************/
TEST(Alarm, periodChangeEmits) {
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"),
        QTime::currentTime().addSecs(600));

    QSignalSpy spy_period_string(&al, SIGNAL(period_changed(QString)));
    ASSERT_TRUE(spy_period_string.isValid());
    QSignalSpy spy_period_enum(&al, SIGNAL(period_changed(Alarm::Period)));
    ASSERT_TRUE(spy_period_enum.isValid());

    al.set_period(Alarm::Daily);
    ASSERT_EQ(spy_period_string.count(), 1);
    ASSERT_EQ(spy_period_enum.count(), 1);

    auto arguments = spy_period_string.takeFirst();
    ASSERT_EQ(arguments.at(0).toString(), QString("daily"));
}
/*****************************************************************************/
TEST(Alarm, construct_from_json) {
    QString json_string(R"(
        {
	        "id": "{247c4f9d-9626-4061-a8cc-1bd2249a0a20}",
            "period": "workdays",
            "time": "06:30",
            "enabled": false,
            "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3",
            "volume": 40,
	        "alarmTimeout": 45
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    auto alarm = Alarm::from_json_object(jdoc.object());

    ASSERT_EQ(alarm->get_id().toString(),
        QString("{247c4f9d-9626-4061-a8cc-1bd2249a0a20}"));
    ASSERT_EQ(alarm->get_period(), Alarm::Workdays);
    ASSERT_EQ(alarm->get_time(), QTime::fromString("06:30", "hh:mm"));
    ASSERT_FALSE(alarm->is_enabled());
    ASSERT_EQ(alarm->get_media_url(),
        QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"));
    ASSERT_EQ(alarm->get_timeout(), std::chrono::minutes(45));
    ASSERT_EQ(alarm->get_volume(), 40);
}

/*****************************************************************************/
TEST(Alarm, from_json_ivalid) {
    QString json_string(R"(
        {
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    ASSERT_THROW(Alarm::from_json_object(jdoc.object()), std::invalid_argument);
}

/*****************************************************************************/
TEST(Alarm, from_json_invalid_uuid) {
    QString json_string(R"(
        {
	        "id": "123-432-ABCD",
            "period": "workdays",
            "time": "06:30",
            "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"
        }
	)");
    QJsonParseError perr;
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8(), &perr);
    ASSERT_EQ(perr.error, QJsonParseError::NoError);
    ASSERT_THROW(Alarm::from_json_object(jdoc.object()), std::invalid_argument);
}

/*****************************************************************************/
TEST(Alarm, from_json_invalid_url) {
    QString json_string(R"(
        {
	        "id": "247c4f9d-9626-4061-a8cc-1bd2249a0a20",
            "period": "once",
            "time": "06:30",
            "url": "something definitely not an url"
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    ASSERT_THROW(Alarm::from_json_object(jdoc.object()), std::invalid_argument);
}

/*****************************************************************************/
TEST(Alarm, from_json_invalid_time) {
    QString json_string(R"(
        {
	        "id": "247c4f9d-9626-4061-a8cc-1bd2249a0a20",
            "period": "daily",
            "time": "25:30",
            "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    ASSERT_THROW(Alarm::from_json_object(jdoc.object()), std::invalid_argument);
}


/*****************************************************************************/
TEST(Alarm, comparisionOperators) {
    DigitalRooster::Alarm al1(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"));

    DigitalRooster::Alarm al2(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:45:00", "hh:mm:ss"));

    DigitalRooster::Alarm al3(
        QUrl("http://st01.dlf.de/dlf/01/104/ogg/stream.ogg"),
        QTime::fromString("08:30:00", "hh:mm:ss"));

    EXPECT_TRUE(al1 < al2);
    al3.enable(false);
    EXPECT_FALSE(al3 < al1);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, checkQDateTimeDefaultBehavior) {
    auto invalid_nothing = QDateTime();
    ASSERT_FALSE(invalid_nothing.isValid());

    auto invalid_no_date = QDateTime();
    invalid_no_date.setTime(QTime::currentTime());
    ASSERT_FALSE(invalid_no_date.isValid());

    auto invalid_no_time = QDateTime();
    invalid_no_time.setDate(QDate::currentDate());
    ASSERT_FALSE(invalid_no_date.isValid());

    auto valid_have_both = QDateTime();
    valid_have_both.setTime(QTime::currentTime());
    valid_have_both.setDate(QDate::currentDate());
    ASSERT_TRUE(valid_have_both.isValid());
}


/*****************************************************************************/
TEST_F(AlarmNextInstance, Disabled) {
    al.enable(false);
    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-18T08:46:20", Qt::ISODate)));

    ASSERT_FALSE(DigitalRooster::get_next_instance(al).isValid());
}


/*****************************************************************************/
TEST_F(AlarmNextInstance, DailyTodayEnabled) {
    al.set_period(Alarm::Daily);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-18T08:30:20", Qt::ISODate)));

    auto expected = QDateTime::fromString("2020-11-18T08:45:00", Qt::ISODate);
    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, DailyTodayPast) {
    al.set_period(Alarm::Daily);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-18T08:46:20", Qt::ISODate)));

    auto expected = QDateTime::fromString("2020-11-19T08:45:00", Qt::ISODate);


    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, OnceToday) {
    al.set_period(Alarm::Once);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-18T07:46:20", Qt::ISODate)));

    auto expected = QDateTime::fromString("2020-11-18T08:45:00", Qt::ISODate);

    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, OnceTodayPast) {
    al.set_period(Alarm::Once);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(
            Return(QDateTime::fromString("2020-11-18T08:46:20", Qt::ISODate)));

    auto expected = QDateTime::fromString("2020-11-19T08:45:00", Qt::ISODate);

    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WeekendFriday) {
    al.set_period(Alarm::Weekend);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Friday Nov 20 2020 */
            Return(QDateTime::fromString("2020-11-20T07:00:00", Qt::ISODate)));

    // Expect saturday 8:45
    auto expected = QDateTime::fromString("2020-11-21T08:45:00", Qt::ISODate);
    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WeekendSaturday) {
    al.set_period(Alarm::Weekend);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Saturday Nov 21 2020 */
            Return(QDateTime::fromString("2020-11-21T08:46:20", Qt::ISODate)));

    // Expect Sunday 8:45
    auto expected = QDateTime::fromString("2020-11-22T08:45:00", Qt::ISODate);

    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WeekendSunday) {
    al.set_period(Alarm::Weekend);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Sunday Nov 22 2020 */
            Return(QDateTime::fromString("2020-11-22T08:30:00", Qt::ISODate)));

    // Expect Sunday 8:45
    auto expected = QDateTime::fromString("2020-11-22T08:45:00", Qt::ISODate);

    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WeekendSundayPast) {
    al.set_period(Alarm::Weekend);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Sunday Nov 22 2020, 9:00 */
            Return(QDateTime::fromString("2020-11-22T09:00:00", Qt::ISODate)));

    // Expect next saturday 8:45
    auto expected = QDateTime::fromString("2020-11-28T08:45:00", Qt::ISODate);

    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WorkdaysFriday) {
    al.set_period(Alarm::Workdays);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Friday Nov 20 2020 */
            Return(QDateTime::fromString("2020-11-20T07:00:00", Qt::ISODate)));

    // Expect saturday 8:45
    auto expected = QDateTime::fromString("2020-11-20T08:45:00", Qt::ISODate);
    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WorkdaysFridayPast) {
    al.set_period(Alarm::Workdays);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Friday Nov 20 2020 */
            Return(QDateTime::fromString("2020-11-20T09:00:00", Qt::ISODate)));

    // Expect Monday 8:45
    auto expected = QDateTime::fromString("2020-11-23T08:45:00", Qt::ISODate);
    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WorkdaysSaturday) {
    al.set_period(Alarm::Workdays);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Saturday Nov 21 2020 */
            Return(QDateTime::fromString("2020-11-21T07:30:20", Qt::ISODate)));

    // Expect Monday 8:45
    auto expected = QDateTime::fromString("2020-11-23T08:45:00", Qt::ISODate);
    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}

/*****************************************************************************/
TEST_F(AlarmNextInstance, WorkdaysWednesdayPast) {
    al.set_period(Alarm::Workdays);

    EXPECT_CALL(*(mc.get()), get_time())
        .WillRepeatedly(/* Wednesday Nov. 18 2020 */
            Return(QDateTime::fromString("2020-11-18T09:30:20", Qt::ISODate)));

    // Expect Thursday 8:45
    auto expected = QDateTime::fromString("2020-11-19T08:45:00", Qt::ISODate);
    ASSERT_EQ(DigitalRooster::get_next_instance(al), expected);
}
