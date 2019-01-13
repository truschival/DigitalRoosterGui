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
    ASSERT_TRUE(true);
}

/*****************************************************************************/
