// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QDebug>
#include <QSignalSpy>
#include <gtest/gtest.h>

#include "appconstants.hpp"
#include "volume_button.hpp"

using namespace DigitalRooster;
using namespace std;
using namespace ::testing;

/*****************************************************************************/
TEST(VolumeButton, VolumeIncreasedDecreased) {
	VolumeButton vbtn;
    QSignalSpy spy(
        &vbtn, SIGNAL(volume_incremented(double)));
	ASSERT_TRUE(spy.isValid());
	Hal::InputEvent evt;
	evt.value = 1;
    vbtn.process_rotary_event(evt);
	evt.value = -1;
    vbtn.process_rotary_event(evt);

    ASSERT_EQ(spy.count(), 2);
    auto arguments = spy.takeFirst();
    EXPECT_DOUBLE_EQ(arguments.at(0).toDouble(), VOLUME_INCREMENT);
    arguments = spy.takeFirst();
    EXPECT_DOUBLE_EQ(arguments.at(0).toDouble(), -VOLUME_INCREMENT);
}

/*****************************************************************************/
TEST(VolumeButton, FilterRotaryEvents) {
	VolumeButton vbtn;
    QSignalSpy spy(
        &vbtn, SIGNAL(volume_incremented(double)));
	ASSERT_TRUE(spy.isValid());

	vbtn.monitor_rotary_button(false);
	Hal::InputEvent evt;
	evt.value = 1;
    vbtn.process_rotary_event(evt);
    ASSERT_EQ(spy.count(), 0);
}
/*****************************************************************************/
TEST(VolumeButton, ProcessKeyEvent) {
	VolumeButton vbtn;
    QSignalSpy spy_pressed(
        &vbtn, SIGNAL(button_pressed()));
    QSignalSpy spy_released(
        &vbtn, SIGNAL(button_released()));
	ASSERT_TRUE(spy_pressed.isValid());
	ASSERT_TRUE(spy_released.isValid());
	Hal::InputEvent evt;
	evt.type = 1;
	evt.code = 102;
	evt.value = 1;

    vbtn.process_key_event(evt); // press
	evt.value = 0;
	vbtn.process_key_event(evt); //release
    // uninteresting Event
    evt.code = 103;
    vbtn.process_key_event(evt);

    ASSERT_EQ(spy_pressed.count(), 1);
    ASSERT_EQ(spy_released.count(), 1);
}


