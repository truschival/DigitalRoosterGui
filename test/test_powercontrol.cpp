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

#include "gtest/gtest.h"

#include "powercontrol.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;

/*****************************************************************************/
TEST(PowerControl, startInStandBy) {
    PowerControl pc;
    ASSERT_EQ(pc.get_power_state(), PowerControl::Standby);
}

/*****************************************************************************/
TEST(PowerControl, toggleTogglesAndEmits) {
    PowerControl pc;
    ASSERT_EQ(pc.get_power_state(), PowerControl::Standby);
    QSignalSpy spy_state_changed(
        &pc, SIGNAL(power_state_changed(PowerControl::PowerState)));
    QSignalSpy spy_active(&pc, SIGNAL(active(bool)));
    QSignalSpy spy_becoming_active(&pc, SIGNAL(becoming_active()));
    pc.toggle_power_state();

    ASSERT_EQ(spy_state_changed.count(), 1);
    ASSERT_EQ(spy_becoming_active.count(), 1);
    ASSERT_EQ(spy_active.count(), 1);

    auto args_active = spy_active.first();
    ASSERT_EQ(args_active.at(0).toBool(), true);

    auto args_sc = spy_state_changed.first();
    ASSERT_EQ(args_sc.at(0).toInt(), PowerControl::Active);

    ASSERT_EQ(pc.get_power_state(), PowerControl::Active);
}

/*****************************************************************************/
TEST(PowerControl, toggleChanges) {
    PowerControl pc;
    ASSERT_EQ(pc.get_power_state(), PowerControl::Standby);
    pc.toggle_power_state();
    ASSERT_EQ(pc.get_power_state(), PowerControl::Active);
    pc.toggle_power_state();
    ASSERT_EQ(pc.get_power_state(), PowerControl::Standby);
}

/*****************************************************************************/
TEST(PowerControl, doubleStandbyDoesNotEmitChange) {
    PowerControl pc;
    ASSERT_EQ(pc.get_power_state(), PowerControl::Standby);

    QSignalSpy spy_state_changed(
        &pc, SIGNAL(power_state_changed(PowerControl::PowerState)));
    QSignalSpy spy_active(&pc, SIGNAL(active(bool)));
    QSignalSpy spy_becoming_active(&pc, SIGNAL(becoming_active()));
    QSignalSpy spy_going_to_standby(&pc, SIGNAL(going_in_standby()));
    pc.standby();

    ASSERT_EQ(spy_state_changed.count(), 0);   // state was not changed!
    ASSERT_EQ(spy_becoming_active.count(), 0); // not active
    ASSERT_EQ(spy_active.count(), 1);          // called with argument=false
    ASSERT_EQ(spy_going_to_standby.count(), 1);

    auto args_active = spy_active.first();
    ASSERT_EQ(args_active.at(0).toBool(), false);
}

/*****************************************************************************/
TEST(PowerControl, firstActivateEmits) {
    PowerControl pc;
    ASSERT_EQ(pc.get_power_state(), PowerControl::Standby);

    QSignalSpy spy_state_changed(
        &pc, SIGNAL(power_state_changed(PowerControl::PowerState)));
    QSignalSpy spy_active(&pc, SIGNAL(active(bool)));
    QSignalSpy spy_becoming_active(&pc, SIGNAL(becoming_active()));
    QSignalSpy spy_going_to_standby(&pc, SIGNAL(going_in_standby()));

    pc.activate();
    ASSERT_EQ(spy_state_changed.count(), 1);   // state was changed!
    ASSERT_EQ(spy_becoming_active.count(), 1); // called
    ASSERT_EQ(spy_active.count(), 1);          // called with argument=true
    ASSERT_EQ(spy_going_to_standby.count(), 0);
    auto args_active = spy_active.first();
    ASSERT_EQ(args_active.at(0).toBool(), true);

    pc.activate();
    ASSERT_EQ(spy_state_changed.count(), 1);   // state was not changed!
    ASSERT_EQ(spy_becoming_active.count(), 2); // called again
    ASSERT_EQ(spy_active.count(), 2); // called again with argument=true
    ASSERT_EQ(spy_going_to_standby.count(), 0);
    args_active = spy_active.first();
    ASSERT_EQ(args_active.at(0).toBool(), true);
}
