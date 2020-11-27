// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QAudio>
#include <QLoggingCategory>
#include <QTimerEvent>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <numeric>

#include "IBrightnessStore.hpp"
#include "IHardware.hpp"
#include "brightnesscontrol.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.BrightnessControl");

static const double LOG_100 = 4.6052;

/*****************************************************************************/
BrightnessControl::BrightnessControl(
    IBrightnessStore& store, Hal::IHardware* hw)
    : cm(store)
    , hwctrl(hw) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void BrightnessControl::update_backlight(double bl_brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << bl_brightness;
    if (standby) {
        current_brightness =
            lin2log(cm.get_standby_brightness() + bl_brightness);
    } else {
        current_brightness =
            lin2log(cm.get_active_brightness() + bl_brightness);
    }
    hwctrl->set_backlight(current_brightness);
    emit brightness_changed(current_brightness);
}

/*****************************************************************************/
bool BrightnessControl::has_als_sensor() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return hwctrl->als_sensor_available();
}

/*****************************************************************************/
void BrightnessControl::subscribe_als_value_change() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (hwctrl->als_sensor_available()) {
        connect(hwctrl, &Hal::IHardware::als_value_changed, this,
            &BrightnessControl::als_value_changed);
    }
}

/*****************************************************************************/
double BrightnessControl::rgb_luma(const Hal::AlsValue& rgbc) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // Calculate relative perceived illuminance based on
    // https://en.wikipedia.org/wiki/Luma_(video)
    double ill = rgbc.red * luma_coeffs[0] + rgbc.green * luma_coeffs[1] +
        rgbc.blue * luma_coeffs[2];

    return ill;
}

/*****************************************************************************/
Hal::AlsValue BrightnessControl::filter_sensor(
    const Hal::AlsValue& brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << brightness;
    // rotate right, new value will enter at index 0 of each channel
    for (auto& e : als_readings) {
        std::rotate(e.rbegin(), e.rbegin() + 1, e.rend());
    }
    als_readings[0][0] = brightness.red;
    als_readings[1][0] = brightness.green;
    als_readings[2][0] = brightness.blue;
    als_readings[3][0] = brightness.clear;
    // Low pass filter
    Hal::AlsValue filtered;
    filtered.red = std::inner_product(als_readings[0].begin(),
        als_readings[0].end(), filter_coeffs.begin(), 0.0f);
    filtered.green = std::inner_product(als_readings[1].begin(),
        als_readings[1].end(), filter_coeffs.begin(), 0.0f);
    filtered.blue = std::inner_product(als_readings[2].begin(),
        als_readings[2].end(), filter_coeffs.begin(), 0.0f);
    filtered.clear = std::inner_product(als_readings[3].begin(),
        als_readings[3].end(), filter_coeffs.begin(), 0.0f);

    qCDebug(CLASS_LC) << "r" << filtered.red << "g" << filtered.green << "b"
                      << filtered.blue << "c" << filtered.clear;
    return filtered;
}

/*****************************************************************************/
void BrightnessControl::als_value_changed(Hal::AlsValue brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto filtered = filter_sensor(brightness);

    // Calculate relative perceived illuminance based on
    // https://en.wikipedia.org/wiki/Luma_(video)
    double ill = rgb_luma(filtered);
    qCDebug(CLASS_LC) << "Illuminance: " << ill;

    /**
     * Nice Writeup:
     * https://www.allaboutcircuits.com/technical-articles/measuring-and-calculating-lux-values-part-2/
     */
    /*
    // total readings r+b+g
    auto total_rgb = std::accumulate(filtered.begin(), filtered.end() - 1, 0);
    // Conversion factors:
    // IRtoILR=683×0.3=205,  IRtoILG=683×0.9=615,  IRtoILB=683×0.06=41
    const int IRtoILr=205;
    const int IRtoILg=615;
    const int IRtoILb=41;
    // The irradiance portion (IP) for each color
    auto IRr=filtered[0]/total_rgb* IRtoILr;
    auto IRg=filtered[1]/total_rgb* IRtoILg;
    auto IRb=filtered[2]/total_rgb* IRtoILb;
    auto IRtotal=IRr+IRg+IRb;

    const double clear_irradiance= 2600.0; // Counts/W/m²
    auto lux = filtered[3]/clear_irradiance * IRtotal;
    qCDebug(CLASS_LC) << "IRtotal" << IRtotal << "lux:" << lux ;
    */

    update_backlight(ill);
}

/*****************************************************************************/
void BrightnessControl::set_active_brightness(int brightness) {
    cm.set_active_brightness(brightness);
    if (!adaptive_mode()) {
        update_backlight();
    }
}

/*****************************************************************************/
void BrightnessControl::set_standby_brightness(int brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    cm.set_standby_brightness(brightness);
    if (!adaptive_mode()) {
        update_backlight();
    }
}

/*****************************************************************************/
int BrightnessControl::get_active_brightness() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return cm.get_active_brightness();
}

/*****************************************************************************/
int BrightnessControl::get_standby_brightness() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return cm.get_standby_brightness();
}

/*****************************************************************************/
int BrightnessControl::get_brightness() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return current_brightness;
}

/*****************************************************************************/
int BrightnessControl::lin2log(double lb) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // Clamped input to range 0.0 - 0.999 -- Needs C++17
    double lin = std::clamp(lb / 100.0, 0.0, 0.999);
    auto log_brightness = -std::log(1.0 - lin) / LOG_100;
    log_brightness = qRound(log_brightness * 100);

    qCDebug(CLASS_LC) << "  lin2log(" << lb << ")=" << log_brightness;
    return log_brightness;
}

/*****************************************************************************/
void BrightnessControl::set_adaptive_mode(bool ena) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (ena != cm.backlight_control_enabled()) {
        cm.enable_backlight_control(ena);
        emit adaptive_mode_changed(ena);
    }

    if (ena) {
        subscribe_als_value_change();
    } else {
        disconnect(hwctrl, &Hal::IHardware::als_value_changed, this,
            &BrightnessControl::als_value_changed);
        // Make sure display value gets updated to current 'manual' setting
        update_backlight();
    }
}

/*****************************************************************************/
void BrightnessControl::active(bool active) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    standby = !active;
    if (!adaptive_mode()) {
        update_backlight();
    }
}

/*****************************************************************************/
bool BrightnessControl::adaptive_mode() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return cm.backlight_control_enabled() && hwctrl->als_sensor_available();
}
/*****************************************************************************/
