/******************************************************************************
 * \filename
 * \brief    controls brightness
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#include <QAudio>
#include <QLoggingCategory>
#include <memory>
#include <cmath>

#include "IBrightnessStore.hpp"
#include "brightnesscontrol.hpp"
#include "powercontrol.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.BrightnessControl");
static const double LOG_100 = 4.6052;

/*****************************************************************************/
BrightnessControl::BrightnessControl(IBrightnessStore& store)
    : cm(store)
    , linear_brightness(0) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void BrightnessControl::set_brightness(int brightness) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << "Linear: " << brightness;
    linear_brightness = brightness;
    emit brightness_pwm_change(lin2log(linear_brightness));
    cm.set_active_brightness(brightness);
}

/*****************************************************************************/
int BrightnessControl::get_brightness() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return linear_brightness;
}

/*****************************************************************************/
int BrightnessControl::lin2log(int lb) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    double lin = (double)lb / 100.0;
    double log_brightness = 1.0;
    if (lin < 0.99) {
        log_brightness = -std::log(1 - lin) / LOG_100;
    }
    return qRound(log_brightness * 100);
}

/*****************************************************************************/
void BrightnessControl::restore_active_brightness() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    linear_brightness = cm.get_active_brightness();
    emit brightness_pwm_change(lin2log(linear_brightness));
}

/*****************************************************************************/
void BrightnessControl::restore_standby_brightness() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    linear_brightness = cm.get_standby_brightness();
    emit brightness_pwm_change(lin2log(linear_brightness));
}

/*****************************************************************************/
