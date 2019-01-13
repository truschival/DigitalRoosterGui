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
#include <QSignalSpy>
#include <gtest/gtest.h>

#include "cm_mock.hpp" /* mock configuration manager */
#include "volume_button.hpp"
using namespace DigitalRooster;
using namespace std;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/

TEST(VolumeButton, VolumeChangedTriggered) {
    auto cm = std::make_shared<CmMock>();
    QString rotary_filename("/dev/input/mouse0");
    QString button_filename("/dev/input/mouse0");

    EXPECT_CALL(*(cm.get()), do_get_volume()).Times(1).WillOnce(Return(25));

    VolumeButton dut(cm.get(), rotary_filename, button_filename);
    QSignalSpy spy(&dut, SIGNAL(volume_changed(int)));
    spy.wait(10000);
    EXPECT_EQ(spy.count(), 1);
}

/*****************************************************************************/
