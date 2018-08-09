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
#include <QTime>
#include <QUrl>
#include <QSignalSpy>
#include <chrono>
#include <gtest/gtest.h>

#include "cm_mock.hpp" /* mock configuration manager */
#include "weather.hpp"

using namespace DigitalRooster;
using namespace std;
using namespace std::chrono;
using namespace ::testing;
using ::testing::AtLeast;

TEST(Weather, GetConfigForDownloadAfterTimerExpired) {
    auto cm = std::make_shared<CmMock>();
    EXPECT_CALL(*(cm.get()), get_weather_cfg())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->weather_cfg));
	
	Weather dut(cm);
    dut.set_update_interval(seconds(1));
  
	QSignalSpy spy(&dut, SIGNAL(weather_info_available(const QJsonObject&)));
    ASSERT_TRUE(spy.isValid());
    spy.wait(1500); // only to make sure time elapses during test
    // ASSERT_EQ(spy.count(), 1);
}