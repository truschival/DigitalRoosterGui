// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>
#include <QString>
#include <QTimerEvent>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <memory>

#include <fcntl.h>
#include <hardwarecontrol_mk3.hpp>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <unistd.h>

#include "appconstants.hpp"
#include "hardware_configuration.hpp"

using namespace Hal;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HardwareControl");

/**
 * Brightness 0-100% between 6 and 256 for hardware PWM
 * hwpwm = 1+brightness[%] * (256-1)/100[%]
 */
static const double BRIGHTNESS_VAL_OFFSET_0 = 3; // Minimum barely visible
static const double BRIGHTNESS_VAL_MAX = 256;
static const double BRIGHTNESS_SLOPE =
    (BRIGHTNESS_VAL_MAX - BRIGHTNESS_VAL_OFFSET_0) / 100.0;

namespace Hal {

/*****************************************************************************/
static InputEvent read_event(int filedescriptor) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    InputEvent evt{0,0,0};
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
    }

    fh = open(path.toStdString().c_str(), O_RDONLY);
    if (fh < 0) {
        throw std::system_error(
            std::make_error_code(static_cast<std::errc>(errno)));
    }
    return fh;
}
} // namespace Hal
/*****************************************************************************/
HardwareControlMk3::HardwareControlMk3(
    const Hal::HardwareConfiguration& cfg, QObject* parent)
    : IHardware(parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    try {
        auto fh = Hal::open_file_handle(cfg.get_push_button_event_path());
        button_notifier =
            std::make_unique<QSocketNotifier>(fh, QSocketNotifier::Read);
        connect(button_notifier.get(), &QSocketNotifier::activated, this,
            &HardwareControlMk3::generate_button_event);
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
            &HardwareControlMk3::generate_rotary_event);
        rotary_notifier->setEnabled(true);

    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }

    backlight_pwm.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    try {
        backlight_pwm.open(cfg.get_backlight_path().toStdString());
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }

    als_red.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    als_green.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    als_blue.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    als_clear.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        als_red.open(cfg.get_als_path().toStdString() + "in_intensity_red_raw");
        als_green.open(
            cfg.get_als_path().toStdString() + "in_intensity_green_raw");
        als_blue.open(
            cfg.get_als_path().toStdString() + "in_intensity_blue_raw");
        als_clear.open(
            cfg.get_als_path().toStdString() + "in_intensity_clear_raw");

        als_sensor_ok = true;
        evt_timer_id = startTimer(DigitalRooster::ALS_SAMPLING_PERIOD);
    } catch (std::exception& exc) {
        als_sensor_ok = false;
        qCCritical(CLASS_LC) << exc.what();
    }

    set_backlight(30); // minimum brightness if something goes wrong
}

/*****************************************************************************/
bool HardwareControlMk3::als_sensor_available() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return als_sensor_ok;
}

/*****************************************************************************/
uint16_t read_sensor(std::ifstream& fs) {
    uint16_t val = 0;
    fs >> val;
    fs.seekg(0, std::ios_base::beg);
    return val;
}

/*****************************************************************************/
AlsValue HardwareControlMk3::read_als_sensor() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    AlsValue ret;
    try {
        als_sensor_ok = true; // reset, no error occurs, ok.
        ret.red = read_sensor(als_red);
        ret.green = read_sensor(als_green);
        ret.blue = read_sensor(als_blue);
        ret.clear = read_sensor(als_clear);
        als_value_changed(ret);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
        als_sensor_ok = false;
    }
    return ret;
}

/*****************************************************************************/
void HardwareControlMk3::timerEvent(QTimerEvent* evt) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (evt->timerId() == evt_timer_id) {
        read_als_sensor();
    } else {
        QObject::timerEvent(evt);
    }
}

/*****************************************************************************/
void HardwareControlMk3::generate_button_event(int file_handle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = read_event(file_handle);
    emit button_event(evt);
}

/*****************************************************************************/
void HardwareControlMk3::generate_rotary_event(int file_handle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = read_event(file_handle);
    emit rotary_event(evt);
}

/*****************************************************************************/
void HardwareControlMk3::system_reboot() {
    sync();
    set_backlight(0);
    system("/sbin/reboot -d 2");
    // return reboot(LINUX_REBOOT_CMD_RESTART);
}

/*****************************************************************************/

void HardwareControlMk3::system_poweroff() {
    sync();
    // use init to shut down gracefully...
    set_backlight(0);
    system("/sbin/poweroff -d 2");
    // if we get here we shut down hard
    //   return reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

/*****************************************************************************/
void HardwareControlMk3::set_backlight(int brightness_in) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << brightness_in;

    // Needs C++17
    int brightness = std::clamp(brightness_in, 1, 100);

    /* maximum brightness if PWM reg == 0, minimum at BRIGHTNESS_VAL_MAX */
    auto pwm_val =
        static_cast<int>(BRIGHTNESS_VAL_MAX - brightness * BRIGHTNESS_SLOPE);
    qCDebug(CLASS_LC) << "brightness:" << brightness << "pwm val:" << pwm_val;
    try {
        backlight_pwm << pwm_val << std::endl;
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << "cannot set brightness:" << exc.what();
    }
}

/*****************************************************************************/
Hal::IHardware::~IHardware() = default;
