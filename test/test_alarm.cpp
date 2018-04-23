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
#include <QDateTime>
#include <QTime>
#include <QUrl>
#include <QDebug>

#include <gtest/gtest.h>

#include "alarm.hpp"
using namespace DigitalRooster;


TEST(Alarm, OnceTodayFuture) {
    auto timepoint = QDateTime::currentDateTime().addSecs(3600);

    Alarm al(
        QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint);

    ASSERT_EQ(al.get_period(), Alarm::Once);
    ASSERT_EQ(al.get_next_trigger(), timepoint);
}
/*****************************************************************************/
TEST(Alarm, OnceTodayPast) {
    auto timepoint = QDateTime::currentDateTime().addSecs(-3600); // in one hour

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint);

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
TEST(Alarm, DailyFuture) {
    auto timepoint = QTime::currentTime().addSecs(3600); // in one hour
    auto expected_trigger = QDateTime::currentDateTime().addSecs(3600);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Daily);

    ASSERT_EQ(al.get_period(), Alarm::Daily);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST(Alarm, WorkdaysFuture) {
    auto timepoint = QTime::currentTime().addSecs(3600); // in one hour
    auto expected_trigger = QDateTime::currentDateTime().addSecs(3600);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Workdays);

    ASSERT_EQ(al.get_period(), Alarm::Workdays);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()),1);
}
/*****************************************************************************/
TEST(Alarm, WorkdaysPast) {
    auto timepoint = QTime::currentTime().addSecs(-3600); 
	// Tomorrow 1 hr ago

    auto expected_trigger = QDateTime::currentDateTime().addSecs(-3600);
	auto dow_today = QDateTime::currentDateTime().date().dayOfWeek();

	if (dow_today != Qt::Friday && dow_today != Qt::Saturday)
		expected_trigger = expected_trigger.addDays(1);
	if (dow_today == Qt::Friday)
		expected_trigger = expected_trigger.addDays(2);
	if (dow_today == Qt::Saturday)
		expected_trigger = expected_trigger.addDays(1);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Workdays);

    ASSERT_EQ(al.get_period(), Alarm::Workdays);
	ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}
/*****************************************************************************/
TEST(Alarm, WeekendsDateTimeFuture) {
    auto timepoint = QTime::currentTime().addSecs(3600);

    auto expected_trigger = QDateTime::currentDateTime().addSecs(3600);
    auto dow_today = expected_trigger.date().dayOfWeek();
    if (dow_today != Qt::Saturday && dow_today != Qt::Sunday)
        expected_trigger = expected_trigger.addDays(Qt::Sunday - dow_today);

    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Weekend);

    ASSERT_EQ(al.get_period(), Alarm::Weekend);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}

/*****************************************************************************/
TEST(Alarm, WeekendsPast) {
    auto timepoint = QTime::currentTime().addSecs(-3600);

    auto expected_trigger = QDateTime::currentDateTime().addSecs(-3600);
    auto dow_today = expected_trigger.date().dayOfWeek();

	if (dow_today != Qt::Saturday && dow_today != Qt::Sunday) {
		expected_trigger = expected_trigger.addDays(Qt::Sunday - dow_today);
	}
	if (dow_today == Qt::Saturday) {
		expected_trigger = expected_trigger.addDays(1);
	}
	if (dow_today == Qt::Sunday) {
		expected_trigger = expected_trigger.addDays(6);
	}
    Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), timepoint,
        Alarm::Weekend);

    ASSERT_EQ(al.get_period(), Alarm::Weekend);
    ASSERT_LE(expected_trigger.secsTo(al.get_next_trigger()), 1);
}



/*****************************************************************************/
TEST(Alarm, updateTiggerSetsPeriodtoOnce) {
	auto expected_trigger = QDateTime::currentDateTime().addSecs(3600);

	Alarm al(QUrl("http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"), QTime::currentTime(),
		Alarm::Daily);

	al.set_trigger(expected_trigger);

	ASSERT_EQ(al.get_period(), Alarm::Once);
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
