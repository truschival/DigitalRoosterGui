/******************************************************************************
 * \filename
 * \brief    shutdown / Reboot
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

#include "hwif/hal.h"
#include "powercontrol.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PowerControl");

/*****************************************************************************/

void PowerControl::power_off() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    ::system_poweroff();
}

/*****************************************************************************/
void PowerControl::reboot() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    ::system_reboot();
}

/*****************************************************************************/
void PowerControl::toggle_power_state() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == Active) {
        state = Standby;
        emit going_in_standby();
    } else {
        state = Active;
        emit becoming_active();
    }
    emit power_state_changed(state);
}

/*****************************************************************************/
void PowerControl::standby() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == Active) {
        state = Standby;
        emit power_state_changed(state);
    }
    emit going_in_standby();
}

/*****************************************************************************/
void PowerControl::activate() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == Standby) {
        state = Active;
        emit power_state_changed(state);
    }
    emit becoming_active();
}
/*****************************************************************************/
