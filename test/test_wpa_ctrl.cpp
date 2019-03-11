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


#include "wpa_ctrl/wpa_ctrl.h"
#include "gtest/gtest.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono;


static const char* ctrl_iface_dir = "/var/run/wpa_supplicant/wlp2s0";

TEST(WPA, connectSocket) {
    auto ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    ASSERT_TRUE(ctrl_conn);
    wpa_ctrl_close(ctrl_conn);
}


void msg_cb(char* msg, size_t msg_len) {
    std::cout << __func__ << ": \n"
              << msg << "\nsize:" << msg_len << "\n--\n"
              << std::endl;
}

/**
 * int wpa_ctrl_request 	( 	struct wpa_ctrl *  	ctrl,
        const char *  	cmd,
        size_t  	cmd_len,
        char *  	reply,
        size_t *  	reply_len,
        void(*)(char *msg, size_t len)  	msg_cb
 */

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


TEST(WPA, cmdListAPs) {
    char buf[4096] = {};
    size_t buf_len = sizeof(buf) - 1;
    size_t orig_buf_len = sizeof(buf) - 1;


    auto ctrl_conn = wpa_ctrl_open(ctrl_iface_dir);
    ASSERT_TRUE(ctrl_conn);
    int res = wpa_ctrl_attach(ctrl_conn);
    ASSERT_EQ(res, 0);

    res = wpa_ctrl_request(
        ctrl_conn, "LEVEL 1", strlen("LEVEL 1"), buf, &buf_len, msg_cb);
    ASSERT_EQ(res, 0);
    std::cout << "LEVEL 1 " << buf << ":" << buf_len << std::endl;
    /**
     * SCAN
     */
    buf_len = orig_buf_len;
    res = wpa_ctrl_request(
        ctrl_conn, "SCAN", strlen("SCAN"), buf, &buf_len, msg_cb);
    ASSERT_EQ(res, 0);
    std::cout << "SCAN " << buf << ":" << buf_len << std::endl;
    std::this_thread::sleep_for(2s);
    while (wpa_ctrl_pending(ctrl_conn) > 0) {
        buf_len = sizeof(buf);
        wpa_ctrl_recv(ctrl_conn, buf, &buf_len);
        std::cout << "CTRL: " << buf << ":" << buf_len << std::endl;
    }

    /**
     * SCAN_RESULTS
     */
    buf_len = orig_buf_len;
    res = wpa_ctrl_request(ctrl_conn, "SCAN_RESULTS", strlen("SCAN_RESULTS"),
        buf, &buf_len, msg_cb);
    ASSERT_EQ(res, 0);
    std::cout << "SCAN_RESULTS " << buf << ":" << buf_len << std::endl;
    while (wpa_ctrl_pending(ctrl_conn) > 0) {
        buf_len = sizeof(buf);
        wpa_ctrl_recv(ctrl_conn, buf, &buf_len);
        std::cout << "CTRL: " << buf << ":" << buf_len << std::endl;
    }
    wpa_ctrl_detach(ctrl_conn);
    wpa_ctrl_close(ctrl_conn);
}
