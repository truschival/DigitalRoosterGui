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
    ASSERT_EQ(al.get_volume(), 40);
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
