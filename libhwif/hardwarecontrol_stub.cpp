/******************************************************************************
 * \filename
 * \brief    stub hardware interface functions
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#include "hwif/hal.h"

int system_reboot() {
    return 0;
}

/*****************************************************************************/

int system_poweroff() {
    return 0;
}

/*****************************************************************************/
int set_brightness(int brightness) {
    return 0;
}

/*****************************************************************************/
int setup_hardware() {
    return 0;
}

/*****************************************************************************/

int get_push_button_handle() {
    return 0;
}

/*****************************************************************************/

int get_rotary_button_handle() {
    return 0;
}

/*****************************************************************************/
ScrollEvent get_scroll_event(int filedescriptor) {
    ScrollEvent evt;
    evt.dir = ScrollEvent::UP;
    return evt;
}

/*****************************************************************************/
int get_pushbutton_value(int filedescriptor) {
    return 0;
}
/*****************************************************************************/
