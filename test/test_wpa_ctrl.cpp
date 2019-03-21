/******************************************************************************
 * \filename Test basic functionality for wpa_ctrl
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>


#include "wpa_ctrl/wpa_ctrl.h"
#include "gtest/gtest.h"


using namespace std::chrono;


static const char* ctrl_iface_dir = "/var/run/wpa_supplicant/wlp2s0";

TEST(WPA, connectSocket) {
    auto ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    ASSERT_TRUE(ctrl_conn);
    wpa_ctrl_close(ctrl_conn);
}


void msg_cb(char* msg, size_t msg_len) {
    std::cout << __func__ << ": \n"
              << msg << "[CALLBACK] \nsize:" << msg_len << "\n--\n"
              << std::endl;
}

TEST(WPA, cmdPingAccepted) {
    auto ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    char buf[2048] = {};
    size_t buf_len = 2048;
    ASSERT_TRUE(ctrl_conn);
    int res = wpa_ctrl_request(
        ctrl_conn, "PING", strlen("PING"), buf, &buf_len, msg_cb);
    ASSERT_EQ(res, 0);
    std::this_thread::sleep_for(100ms);
    ASSERT_STREQ(buf, "PONG\n");
    wpa_ctrl_close(ctrl_conn);
}
