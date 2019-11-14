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

#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QString>

#include <fcntl.h>

#include <cstring> //std::strerror
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <linux/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wiringPi.h>

#include "hwif/hal.h"
#include "hwif/hardware_configuration.hpp"

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HAL");

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

static int push_button_filehandle = 0;
static int rotary_button_filehandle = 0;


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
    qCDebug(CLASS_LC) << Q_FUNC_INFO << "(real)";
    wiringPiSetup();
    pinMode(BRIGHTNESS_PWM_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_BAL);
    pwmSetClock(CLOCK_DIV);
    pwmSetRange(PWM_RANGE);
    pwmWrite(BRIGHTNESS_PWM_PIN, 100);

    Hal::HardwareConfiguration hwcfg;

    auto pb_event_file_name = hwcfg.get_push_button_event_path();
    if (pb_event_file_name.isEmpty()) {
        qCCritical(CLASS_LC) << "Pushbutton event interface not found";
    } else {
        push_button_filehandle = open(pb_event_file_name.toStdString().c_str(), O_RDONLY);
        if (push_button_filehandle < 0) {
            qCWarning(CLASS_LC) << " failed to open push-button device " << std::strerror(errno) << "(" <<errno << ")" ;
        }
    }

    auto rot_event_file_name = hwcfg.get_rotary_event_path();
    if (rot_event_file_name.isEmpty()) {
        qCCritical(CLASS_LC) << "Rotary event interface not found";
    } else {
        rotary_button_filehandle = open(rot_event_file_name.toStdString().c_str(), O_RDONLY);
        if (rotary_button_filehandle < 0) {
            qCWarning(CLASS_LC) << " failed to open rotary encoder device "<< errno;
        }
    }
    return 0;
}

/*****************************************************************************/

int get_push_button_handle() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return push_button_filehandle;
}

/*****************************************************************************/

int get_rotary_button_handle() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return rotary_button_filehandle;
}

/*****************************************************************************/
InputEvent get_input_event(int filedescriptor) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    InputEvent evt;
    struct input_event evt_raw;

    auto s = ::read(filedescriptor, &evt_raw, sizeof(evt_raw));
    if (s < 0) {
        qCCritical(CLASS_LC) << "ERROR ";
    }

    if (s > 0) {
        evt.code = evt_raw.code;
        evt.value = evt_raw.value;
        evt.type = evt_raw.type;
        qCDebug(CLASS_LC) << "T:" << evt.type << "V:" << evt.value
                          << "C:" << evt.code;
    }
    return evt;
}
}
