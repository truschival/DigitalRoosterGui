/******************************************************************************
 * \filename
 * \brief    DigitalRooster audio compatible board with backlight control
 * \details  This does not use SOC PWM channels and does not use wiringpi
 *
 * \author Thomas Ruschival
 * \copyright 2020 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <QLoggingCategory>
#include <QString>
#include <cstring>
#include <exception>
#include <fstream>
#include <memory>

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <linux/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hardware_configuration.hpp"
#include "hardware_control.hpp"

using namespace Hal;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HardwareControl");

/**
 * Brightness 0-100% between 1 and 256 for hardware PWM
 * hwpwm = 1+brightness[%] * (256-1)/100[%]
 */
static const double BRIGHTNESS_VAL_OFFSET_0 = 1;
static const double BRIGHTNESS_VAL_MAX = 256;
static const double BRIGHTNESS_SLOPE =
    (BRIGHTNESS_VAL_MAX - BRIGHTNESS_VAL_OFFSET_0) / 100.0;

namespace Hal {

/*****************************************************************************/
static InputEvent read_event(int filedescriptor) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    InputEvent evt;
    struct input_event evt_raw;

    auto s = ::read(filedescriptor, &evt_raw, sizeof(evt_raw));
    if (s < 0) {
        qCCritical(CLASS_LC) << std::strerror(errno);
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
static int open_file_handle(const QString& path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    int fh;
    if (path.isEmpty()) {
        throw std::runtime_error("file path empty");
    } else {
        fh = open(path.toStdString().c_str(), O_RDONLY);
        if (fh < 0) {
            throw std::system_error(
                std::make_error_code(static_cast<std::errc>(errno)));
        }
    }
    return fh;
}
} // namespace Hal
/*****************************************************************************/
HardwareControl::HardwareControl(
    const Hal::HardwareConfiguration& cfg, QObject* parent)
    : QObject(parent)
    , hwconf(cfg) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    try {
        auto fh = Hal::open_file_handle(cfg.get_push_button_event_path());
        button_notifier =
            std::make_unique<QSocketNotifier>(fh, QSocketNotifier::Read);
        connect(button_notifier.get(), &QSocketNotifier::activated, this,
            &HardwareControl::generate_button_event);
        button_notifier->setEnabled(true);

    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }

    try {
        auto fh = Hal::open_file_handle(cfg.get_rotary_event_path());
        /* connect notifier and handler for  rotary encoder */
        rotary_notifier =
            std::make_unique<QSocketNotifier>(fh, QSocketNotifier::Read);

        connect(rotary_notifier.get(), &QSocketNotifier::activated, this,
            &HardwareControl::generate_rotary_event);
        rotary_notifier->setEnabled(true);

    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
    set_brightness(10); // minimum brightness if something goes wrong
}

/*****************************************************************************/
void HardwareControl::generate_button_event(int file_handle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = read_event(file_handle);
    emit button_event(evt);
}

/*****************************************************************************/
void HardwareControl::generate_rotary_event(int file_handle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = read_event(file_handle);
    emit rotary_event(evt);
}

/*****************************************************************************/
void HardwareControl::system_reboot() {
    sync();
    set_brightness(0);
    system("/sbin/reboot -d 2");
    // return reboot(LINUX_REBOOT_CMD_RESTART);
}

/*****************************************************************************/

void HardwareControl::system_poweroff() {
    sync();
    // use init to shut down gracefully...
    set_brightness(0);
    system("/sbin/poweroff -d 2");
    // if we get here we shut down hard
    //   return reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

/*****************************************************************************/
int HardwareControl::set_brightness(int brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /* maximum brightness if PWM reg == 0, minimum at BRIGHTNESS_VAL_MAX */
    int pwm_val = BRIGHTNESS_VAL_MAX - brightness * BRIGHTNESS_SLOPE;
    qCDebug(CLASS_LC) << "pwm val:" << pwm_val;
    try {
        auto out = std::ofstream(hwconf.get_backlight_path().toStdString());
        out.exceptions(std::ofstream::badbit | std::ofstream::failbit);
        out << pwm_val;
        out.close();
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << "cannot set brightness:" << exc.what();
    }
    return 0;
}

/*****************************************************************************/
