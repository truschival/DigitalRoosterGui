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
#include <QTime>
#include <QUrl>
#include <QUuid>
#include <QJsonDocument>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "alarm.hpp"
#include "appconstants.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;


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
    EXPECT_THROW(json_string_to_alarm_period("Foobar"), std::invalid_argument);
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

    QList<QVariant> arguments = spy_period_string.takeFirst();
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
