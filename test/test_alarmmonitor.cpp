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
#include <QDebug>
#include <QSignalSpy>
#include <QUrl>
#include <QVector>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "alarm.hpp"
#include "alarmmonitor.hpp"
#include "cm_mock.hpp"
#include "player_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;
using ::testing::AtLeast;

TEST(AlarmMonitor, playsAlarm) {
    auto player = std::make_shared<PlayerMock>();
    AlarmMonitor mon(player);
    auto timepoint = QDateTime::currentDateTimeUtc().addSecs(3);
    auto alm = make_shared<DigitalRooster::Alarm>(
        QUrl("https://www.heise.de"), timepoint);

    EXPECT_CALL(*(player.get()), do_play()).Times(1);
    EXPECT_CALL(*(player.get()), do_set_volume(_)).Times(1);
    EXPECT_CALL(*(player.get()), do_set_media(_)).Times(1);

    mon.alarm_triggered(alm);
}
