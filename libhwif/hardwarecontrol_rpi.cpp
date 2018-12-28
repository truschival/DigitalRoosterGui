/******************************************************************************
 * \filename
 * \brief    Raspberry PI specific hardware interface functions
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <linux/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <wiringPi.h>

#include "hwif/hal.h"
extern "C" {
static const int PWM_RANGE = 512; // 2 to 4095 (1024 default)
static const int CLOCK_DIV = 64;  // 1 to 4096
static const int BRIGHTNESS_PWM_PIN = 23;
/**
 * Brightness 0-100% between 1 and 256 for hardware PWM
 * hwpwm = 1+brightness[%] * (256-1)/100[%]
 */
static const double BRIGHTNESS_VAL_OFFSET_0 = 1;
static const double BRIGHTNESS_VAL_MAX = 256;
static const double BRIGHTNESS_SLOPE =
    (BRIGHTNESS_VAL_MAX - BRIGHTNESS_VAL_OFFSET_0) / 100.0;

/*****************************************************************************/
int system_reboot() {
    sync();
    return system("/sbin/reboot -d 2");
    // return reboot(LINUX_REBOOT_CMD_RESTART);
}

/*****************************************************************************/

int system_poweroff() {
    sync();
    // use init to shut down gracefully...
    return system("/sbin/poweroff -d 2");
    // if we get here we shut down hard
    //   return reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

/*****************************************************************************/
int set_brightness(int brightness) {
    int pwm_val = BRIGHTNESS_VAL_OFFSET_0 + brightness * BRIGHTNESS_SLOPE;
    pwmWrite(BRIGHTNESS_PWM_PIN, pwm_val);
    return 0;
}

/*****************************************************************************/
int setup_hardware() {
    wiringPiSetup();
    pinMode(BRIGHTNESS_PWM_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_BAL);
    pwmSetClock(CLOCK_DIV);
    pwmSetRange(PWM_RANGE);
    pwmWrite(BRIGHTNESS_PWM_PIN, 100);
    return 0;
};

/*****************************************************************************/
ScrollEvent get_scroll_event(int filedescriptor) {
    ScrollEvent evt;
    evt.dir = ScrollEvent::UP;
    return evt;
}
}