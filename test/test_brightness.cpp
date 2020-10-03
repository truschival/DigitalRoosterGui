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

#include "IHardware.hpp"
#include "brightnesscontrol.hpp"
#include "cm_mock.hpp" /* mock configuration manager */

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;
using ::testing::AtLeast;

class HardwareMock : public Hal::IHardware {
public:
    MOCK_METHOD0(read_als_sensor, Hal::AlsValue());
    MOCK_CONST_METHOD0(als_sensor_available, bool());
    MOCK_METHOD0(system_reboot, void());
    MOCK_METHOD0(system_poweroff, void());
    MOCK_METHOD1(set_backlight, void(int));
};

/*****************************************************************************/
class BrightnessFixture : public ::testing::Test {
public:
    BrightnessFixture(){};

protected:
    CmMock cm;
    HardwareMock hwctrl;
};

/*****************************************************************************/
TEST_F(BrightnessFixture, HwProxyGetSensor) {
    EXPECT_CALL(hwctrl, als_sensor_available()).Times(1).WillOnce(Return(true));
    BrightnessControl dut(cm, &hwctrl);
    EXPECT_TRUE(dut.has_als_sensor());
}

/*****************************************************************************/
TEST_F(BrightnessFixture, AdaptiveModeEnabledNoSensor) {
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(hwctrl, als_sensor_available())
        .Times(1)
        .WillOnce(Return(false));

    BrightnessControl dut(cm, &hwctrl);
    EXPECT_FALSE(dut.adaptive_mode());
}

/*****************************************************************************/
TEST_F(BrightnessFixture, SetAdaptiveModeHasSensor) {
    // standby brightness is read in set_adaptive_mode
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(1)
        .WillRepeatedly(Return(20));

    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(3)
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    // Sensor is o.k.
    EXPECT_CALL(hwctrl, als_sensor_available())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));

    // When disabling we force setting backlight
    EXPECT_CALL(hwctrl, set_backlight(_)).Times(1);

    BrightnessControl dut(cm, &hwctrl);
    dut.set_adaptive_mode(true);
    dut.set_adaptive_mode(false);
    dut.set_adaptive_mode(true);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, RestoreActiveManual) {
    // standby brightness is read in set_adaptive_mode
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(1)
        .WillRepeatedly(Return(20));
    // Is read when switching to active
    EXPECT_CALL(cm, get_active_brightness())
        .Times(1)
        .WillRepeatedly(Return(42));

    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(2)
        .WillRepeatedly(Return(false));

    // 20% standby Brighness maps logarithmically to 5% PWM setting
    EXPECT_CALL(hwctrl, set_backlight(5)).Times(1);

    // 42% Brighness maps logarithmically to 12% PWM setting
    EXPECT_CALL(hwctrl, set_backlight(12)).Times(1);

    BrightnessControl dut(cm, &hwctrl);
    dut.set_adaptive_mode(false);
    dut.active(true);
    ASSERT_EQ(dut.get_brightness(), 12);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, RestoreStandbyManual) {
    // standby brightness is read in set_adaptive_mode and active()
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(2)
        .WillRepeatedly(Return(20));

    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(2)
        .WillRepeatedly(Return(false));

    // 12% perceived brightness at 5% PWM
    EXPECT_CALL(hwctrl, set_backlight(5)).Times(2);
    BrightnessControl dut(cm, &hwctrl);
    dut.set_adaptive_mode(false);
    dut.active(false);
    ASSERT_EQ(dut.get_brightness(), 5);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, AutoControl) {
    /* Default behavior of various adaptive_mode() calls */
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(hwctrl, als_sensor_available())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));

    // standby brightness is read in set_adaptive_mode
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(1)
        .WillRepeatedly(Return(20));
    // Is read when switching to active
    EXPECT_CALL(cm, get_active_brightness())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(35));

    // 5 times in adaptive_mode als_value_changed is called
    // in adaptive mode dut.active(false) should not call hardware
    EXPECT_CALL(hwctrl, set_backlight(_)).Times(5);

    BrightnessControl dut(cm, &hwctrl);
    dut.set_adaptive_mode(true);
    dut.active(true);

    dut.als_value_changed({20, 24, 28, 45});
    EXPECT_EQ(dut.get_brightness(), 19);
    dut.als_value_changed({20, 24, 32, 35});
    EXPECT_EQ(dut.get_brightness(), 26);
    dut.als_value_changed({20, 24, 32, 64});
    EXPECT_EQ(dut.get_brightness(), 32);

    // 4th call with same arguments should not change result
    dut.als_value_changed({20, 24, 32, 64});
    EXPECT_EQ(dut.get_brightness(), 32);

    // Switching to standby also does not change
    dut.active(false);
    EXPECT_EQ(dut.get_brightness(), 32);
    // next control cycle changes to current values + standby brightness
    dut.als_value_changed({20, 24, 32, 64});
    EXPECT_EQ(dut.get_brightness(), 21);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, AutoControlEmits) {
    /* Default behavior of various adaptive_mode() calls */
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(hwctrl, als_sensor_available())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));

    // standby brightness is read in set_adaptive_mode
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(1)
        .WillRepeatedly(Return(20));

    EXPECT_CALL(hwctrl, set_backlight(_)).Times(1);

    BrightnessControl dut(cm, &hwctrl);
    QSignalSpy spy(&dut, SIGNAL(brightness_changed(int)));
    ASSERT_TRUE(spy.isValid());

    dut.set_adaptive_mode(true);
    dut.als_value_changed({20, 24, 28, 45});
    EXPECT_GE(spy.count(), 1);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, CmProxySetActiveBrightnessInStandby) {
    /* Operate in ManualMode */
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    // called in set_active_brighness and in active
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(2)
        .WillRepeatedly(Return(20));

    EXPECT_CALL(cm, set_active_brightness(12)).Times(AtLeast(1));
    // switching active and set_active_brightness call hardware
    EXPECT_CALL(hwctrl, set_backlight(_)).Times(2);
    BrightnessControl dut(cm, &hwctrl);
    dut.active(false);
    dut.set_active_brightness(12);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, CmProxySetStandbyBrightnessInStandby) {
    /* Operate in ManualMode */
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    // called in set_active_brighness and in active
    EXPECT_CALL(cm, get_standby_brightness())
        .Times(2)
        .WillRepeatedly(Return(20));

    EXPECT_CALL(cm, set_standby_brightness(5)).Times(AtLeast(1));
    // switching active and set_active_brightness call hardware
    EXPECT_CALL(hwctrl, set_backlight(_)).Times(2);

    BrightnessControl dut(cm, &hwctrl);
    dut.active(false);
    dut.set_standby_brightness(5);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, CmProxySetActiveBrightnessActive) {
    /* Operate in ManualMode */
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    // Is read when switching to active
    EXPECT_CALL(cm, get_active_brightness())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(35));

    EXPECT_CALL(cm, set_active_brightness(12)).Times(AtLeast(1));
    // switching active and set_active_brightness call hardware
    EXPECT_CALL(hwctrl, set_backlight(_)).Times(2);
    BrightnessControl dut(cm, &hwctrl);
    dut.active(true);
    dut.set_active_brightness(12);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, CmProxySetStandbyBrightnessActive) {
    /* Operate in ManualMode */
    EXPECT_CALL(cm, backlight_control_enabled())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(false));

    // Is read when switching to active
    EXPECT_CALL(cm, get_active_brightness())
        .Times(AtLeast(1))
        .WillRepeatedly(Return(35));
    EXPECT_CALL(cm, set_standby_brightness(5)).Times(AtLeast(1));
    // switching active and set_active_brightness call hardware
    EXPECT_CALL(hwctrl, set_backlight(_)).Times(2);

    BrightnessControl dut(cm, &hwctrl);
    dut.active(true);
    dut.set_standby_brightness(5);
}

/*****************************************************************************/
TEST_F(BrightnessFixture, CmProxyGetBrightnessValues) {
    EXPECT_CALL(cm, get_active_brightness()).Times(1).WillOnce(Return(35));
    EXPECT_CALL(cm, get_standby_brightness()).Times(1).WillOnce(Return(20));
    BrightnessControl dut(cm, &hwctrl);
    EXPECT_EQ(dut.get_active_brightness(), 35);
    EXPECT_EQ(dut.get_standby_brightness(), 20);
}
