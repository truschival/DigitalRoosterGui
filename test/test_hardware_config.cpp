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

#include <cstdlib>
#include <QString>

#include "gtest/gtest.h"
#include "hardware_configuration.hpp"

using namespace Hal;
using namespace ::testing;

/*****************************************************************************/
TEST(HardwareConfig, envSetsPushButtonPath) {
    QString expected_path{"/dev/input/event27"};
    ASSERT_EQ(setenv(Hal::PUSH_BUTTON_PATH_ENV_VAR_NAME.toStdString().c_str(),
                  expected_path.toStdString().c_str(), 1),
        0);
    HardwareConfiguration hc;
    ASSERT_EQ(hc.get_push_button_event_path(), expected_path);
}
/*****************************************************************************/
TEST(HardwareConfig, envSetsRotaryPath) {
    QString expected_path{"/dev/input/event37"};
    ASSERT_EQ(setenv(Hal::ROTARY_PATH_ENV_VAR_NAME.toStdString().c_str(),
                  expected_path.toStdString().c_str(), 1),
        0);
    HardwareConfiguration hc;
    ASSERT_EQ(hc.get_rotary_event_path(), expected_path);
}
