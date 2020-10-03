// SPDX-License-Identifier: GPL-3.0-or-later
/******************************************************************************
 * \author Thomas Ruschival
 * \copyright 2020 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#include <QSignalSpy>
#include <QString>
#include <gtest/gtest.h>

#include "networkinfo.hpp"
using namespace DigitalRooster;
using namespace std;

/*****************************************************************************/
TEST(NetworkInfo, AddrNoSuchItf) {
    NetworkInfo dut("enp0s31f6");
    ASSERT_EQ(dut.get_ip_addr(), QString("None"));
}

/*****************************************************************************/
TEST(NetworkInfo, LinkNoSuchItf) {
    NetworkInfo dut("enp0s31f6");
    ASSERT_FALSE(dut.get_link_status());
}

/*****************************************************************************/
TEST(NetworkInfo, AddrOk) {
    NetworkInfo dut("wlp2s0");
    ASSERT_EQ(dut.get_ip_addr(), QString("192.168.178.10"));
}

/*****************************************************************************/
TEST(NetworkInfo, LinkUp) {
    NetworkInfo dut("wlp2s0");
    ASSERT_TRUE(dut.get_link_status());
}

/*****************************************************************************/
TEST(NetworkInfo, CyclicPolling) {
    NetworkInfo dut("XXX"); // invalid
    QSignalSpy spy(&dut, SIGNAL(link_status_changed(bool)));
    ASSERT_TRUE(spy.isValid());
    spy.wait(2000); // should fire after 1 second
    ASSERT_EQ(spy.count(), 1);
}
