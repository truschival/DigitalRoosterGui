// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QtGlobal>
#include <QString>

#include "gtest/gtest.h"
#include "hardware_configuration.hpp"

using namespace Hal;
using namespace ::testing;

/*****************************************************************************/
TEST(HardwareConfig, envSetsPushButtonPath) {
    QString expected_path{"/dev/input/event27"};
    ASSERT_TRUE(qputenv(Hal::PUSH_BUTTON_PATH_ENV_VAR_NAME.toStdString().c_str(),
            expected_path.toUtf8()));
    HardwareConfiguration hc;
    ASSERT_EQ(hc.get_push_button_event_path(), expected_path);
}

/*****************************************************************************/
TEST(HardwareConfig, envSetsRotaryPath) {
    QString expected_path{"/dev/input/event37"};
    ASSERT_TRUE(qputenv(Hal::ROTARY_PATH_ENV_VAR_NAME.toStdString().c_str(),
                  expected_path.toUtf8()));
    HardwareConfiguration hc;
    ASSERT_EQ(hc.get_rotary_event_path(), expected_path);
}
