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

#include "volume_button.hpp"
#include "cm_mock.hpp" /* mock configuration manager */
using namespace DigitalRooster;
using namespace std;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/
TEST(VolumeButton, VolumeDefaultInitialized) {
    auto cm = std::make_shared<CmMock>();
    VolumeButton dut(cm.get());
	EXPECT_CALL(*(cm.get()), do_get_volume())
        .Times(1)
        .WillOnce(Return(25));
  
	EXPECT_EQ(dut.get_volume(), 25);
 }

/*****************************************************************************/
