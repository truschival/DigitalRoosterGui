// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>

#include "powercontrol.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PowerControl");

/*****************************************************************************/

void PowerControl::power_off() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    emit shutdown_request();
}

/*****************************************************************************/
void PowerControl::reboot() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    emit reboot_request();
}

/*****************************************************************************/
void PowerControl::toggle_power_state() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == Active) {
        state = Standby;
        emit going_in_standby();
        emit active(false);
    } else {
        state = Active;
        emit becoming_active();
        emit active(true);
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
    emit active(false);
}

/*****************************************************************************/
void PowerControl::activate() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (state == Standby) {
        state = Active;
        emit power_state_changed(state);
    }
    emit becoming_active();
    emit active(true);
}
/*****************************************************************************/
