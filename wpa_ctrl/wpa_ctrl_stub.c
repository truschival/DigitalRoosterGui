/******************************************************************************
 * \filename
 * \brief	 Empty definitions for wpa_supplicant control functions for windows
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "wpa_ctrl/wpa_ctrl.h"

/*****************************************************************************/
int wpa_ctrl_attach(struct wpa_ctrl* ctrl) {
    return 0;
}

/*****************************************************************************/
int wpa_ctrl_detach(struct wpa_ctrl* ctrl) {
    return 0;
}

/*****************************************************************************/
int wpa_ctrl_recv(struct wpa_ctrl* ctrl, char* reply, size_t* reply_len) {
    return 0;
}

/*****************************************************************************/
int wpa_ctrl_pending(struct wpa_ctrl* ctrl) {
    return 0;
}

/*****************************************************************************/
int wpa_ctrl_get_fd(struct wpa_ctrl* ctrl) {
    return 0;
}

/*****************************************************************************/
struct wpa_ctrl* wpa_ctrl_open(const char* ctrl_path) {
    // return a nullptr to make sure it is not used
    return NULL;
}

/*****************************************************************************/
void wpa_ctrl_close(struct wpa_ctrl* ctrl) {
    return;
}

/*****************************************************************************/
int wpa_ctrl_request(struct wpa_ctrl* ctrl, const char* cmd, size_t cmd_len,
    char* reply, size_t* reply_len, void (*msg_cb)(char* msg, size_t len)) {
    return 0;
}
