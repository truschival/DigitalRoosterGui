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
#include <QSignalSpy>
#include <gtest/gtest.h>

#include "brightnesscontrol.hpp"
#include "cm_mock.hpp" /* mock configuration manager */

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;
using ::testing::AtLeast;

/* log_100 values for 0:5:100 */
const int log_ref[] = {
    0,
    1,
    2,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    15,
    17,
    20,
    23,
    26,
    30,
    35,
    41,
    50,
    65,
	100
};

/*****************************************************************************/
TEST(Brightness, lin2log) {
    auto cm = std::make_shared<CmMock>();
	BrightnessControl dut(cm);
    for (int i = 0; i <= 100;) {
        //cout << dut.lin2log(i) << ", " << endl;
		EXPECT_EQ(dut.lin2log(i), log_ref[i / 5]);
        i = i += 5;
    }
}

/*****************************************************************************/
TEST(Brightness, RestoreActive) {
    auto cm = std::make_shared<CmMock>();
    // active brightness is read in constructor
    EXPECT_CALL(*cm.get(), do_get_brightness_act())
        .Times(1)
        .WillOnce(Return(42));
    BrightnessControl dut(cm);
    dut.restore_active_brightness();
    ASSERT_EQ(dut.get_brightness(), 42);
}

/*****************************************************************************/
TEST(Brightness, RestoreStandby) {
    auto cm = std::make_shared<CmMock>();
    // active brightness is read in constructor
    EXPECT_CALL(*cm.get(), do_get_brightness_sb())
        .Times(1)
        .WillOnce(Return(10));
    BrightnessControl dut(cm);
    dut.restore_standby_brightness();
    ASSERT_EQ(dut.get_brightness(), 10);
}

/*****************************************************************************/