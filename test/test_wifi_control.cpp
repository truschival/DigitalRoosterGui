// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */


#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <QSignalSpy>
#include <QString>

#include "wifi_control.hpp"
#include "gtest/gtest.h"
#include "config_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/
TEST(WifiControl, startScan){
	CmMock config;
	// No real expectations but needed to return a network name.
    EXPECT_CALL(config, get_wpa_socket_name())
        .WillRepeatedly(Return(QString("/var/run/wpa_supplicant/wlp2s0")));

    auto dut = WifiControl::get_instance(&config);
    int scanstatus_typeid =
        qRegisterMetaType<WifiControl::ScanStatus>("WifiControl::ScanStatus");
    ASSERT_TRUE(QMetaType::isRegistered(scanstatus_typeid));

    QSignalSpy spy(dut, &WifiControl::scan_status_changed);
	ASSERT_TRUE(spy.isValid());
	dut->start_scan();
	spy.wait(300);
	spy.wait(3000);
	ASSERT_EQ(spy.count(),2);

	auto arguments = spy.takeFirst();
    EXPECT_EQ(arguments.at(0).toString(), QString("Scanning"));
	arguments = spy.takeFirst();
	EXPECT_EQ(arguments.at(0).toInt(), WifiControl::ScanOk);
}

/*****************************************************************************/


