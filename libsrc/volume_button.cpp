// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>

#include "configuration_manager.hpp"
#include "volume_button.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.VolumeButton");

using namespace DigitalRooster;

/*****************************************************************************/
DigitalRooster::VolumeButton::VolumeButton(QObject* parent)
    : QObject(parent)
    , enable_volume_changes(true) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
VolumeButton::~VolumeButton() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::process_rotary_event(
    const Hal::InputEvent& evt) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (!enable_volume_changes) {
        qCDebug(CLASS_LC) << "ignoring volume change events";
        return;
    }
    // only react on -1 or 1 events
    if (evt.value < 0) {
        emit volume_incremented(-1);
    }
    if (evt.value > 0) {
        emit volume_incremented(1);
    }
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::monitor_rotary_button(bool active) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    enable_volume_changes = active;
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::process_key_event(
    const Hal::InputEvent& evt) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /*
     * Type=(0x01 EV_KEY), value=(1 pressed, 0 released), code=(102 KEY_HOME)
     * => only catch key events for key_home
     */
    if (evt.type != 0x01 || evt.code != 102) {
        qCDebug(CLASS_LC) << "uninteresting event type:" << evt.type
                          << " code: " << evt.code;
        return;
    }
    if (evt.value == 1) {
        qCDebug(CLASS_LC) << "button pressed";
        emit button_pressed();
    }
    if (evt.value == 0) {
        qCDebug(CLASS_LC) << "button released";
        emit button_released();
    }
}

/*****************************************************************************/
