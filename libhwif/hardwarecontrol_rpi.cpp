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

#include <QLoggingCategory>
#include <QString>
#include <QDir>
#include <QFileInfo>

#include <fcntl.h>
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
static QString find_event_interface_by_name(const QString& device_name) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << device_name;
    QDir evt_dir = QDir("/dev/input/");
    for (auto& file_name : evt_dir.entryList(QDir::System)) {
        QFile f(file_name);
        if (!f.open(QFile::ReadWrite)) {
            qCCritical(CLASS_LC)
                << "Error: open file " << f.fileName() << f.errorString();
            continue;
        }
        char buf[256];
        int ret = ioctl(f.handle(), EVIOCGNAME(sizeof(buf)), buf);
        if (ret < 0) {
            qCCritical(CLASS_LC) << " ioctl failed " << ret ;
            continue;
        } else {
            // match name
            QString dev_name(buf);
            qCDebug(CLASS_LC) << "Checking " << dev_name
                              << "equals: " << (dev_name == device_name);
            if (dev_name == device_name) {
            	f.close();
                return QFileInfo(f).absoluteFilePath();
            }
        }
        f.close();
    }
    return QString();
}

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

    QString pb_event_file_name = find_event_interface_by_name("Rotary");
    if (pb_event_file_name.isEmpty()) {
        qCCritical(CLASS_LC) << "Pushbutton event interface not found";
    } else {
        push_button_filehandle = open("/dev/input/event1", O_RDONLY);
        if (push_button_filehandle < 0) {
            qCWarning(CLASS_LC) << " failed to open push-button GPIO";
        }
    rotary_button_filehandle = open("/dev/input/event1", O_RDONLY);
    if(rotary_button_filehandle < 0){
    	qCWarning(CLASS_LC) << " failed to open rotary-button event interface";
    }
    return 0;
};


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
ScrollEvent get_scroll_event(int filedescriptor) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    ScrollEvent evt;
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

/*****************************************************************************/
int get_pushbutton_value(int filedescriptor) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    char value = 0;
    lseek(filedescriptor, 0, SEEK_SET);
    auto s = ::read(filedescriptor, &value, sizeof(char));
    if (s < 0) {
        qCCritical(CLASS_LC) << "push_button read error";
    }
    if (s > 0) {
        qCDebug(CLASS_LC) << "push_button:" << value;
        return atoi(&value);
    }
    return value;
}
}
