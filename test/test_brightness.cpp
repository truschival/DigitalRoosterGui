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
const int log_ref[] = {0, 1, 2, 4, 5, 6, 8, 9, 11, 13, 15, 17, 20, 23, 26, 30,
    35, 41, 50, 65, 100};
/*****************************************************************************/
class BrightnessFixture : public ::testing::Test{
public:
	BrightnessFixture():dut(cm){};
protected:
	CmMock cm;
	BrightnessControl dut;
};

/*****************************************************************************/
TEST_F(BrightnessFixture, lin2log) {
    BrightnessControl dut(cm);
    for (int i = 0; i <= 100;) {
        // cout << dut.lin2log(i) << ", " << endl;
        EXPECT_EQ(dut.lin2log(i), log_ref[i / 5]);
        i += 5;
    }
}

/*****************************************************************************/
TEST_F(BrightnessFixture,RestoreActive) {
    // active brightness is read in constructor
    EXPECT_CALL(cm, do_get_brightness_act())
        .Times(1)
        .WillOnce(Return(42));

    dut.restore_active_brightness();
    ASSERT_EQ(dut.get_brightness(), 42);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, setBrightness) {
    // active brightness is read in constructor
    EXPECT_CALL(cm, do_set_brightness_act(25))
        .Times(1);

    dut.set_brightness(25);
    ASSERT_EQ(dut.get_brightness(), 25);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, RestoreStandby) {
    // active brightness is read in constructor
    EXPECT_CALL(cm, do_get_brightness_sb())
        .Times(1)
        .WillOnce(Return(10));

    dut.restore_standby_brightness();
    ASSERT_EQ(dut.get_brightness(), 10);
}

/*****************************************************************************/
