/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include "volume_button.hpp"
#include "configuration_manager.hpp"
#include "hwif/hal.h"
#include <QLoggingCategory>

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.VolumeButton");

using namespace DigitalRooster;

/*****************************************************************************/
DigitalRooster::VolumeButton::VolumeButton(QObject* parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* connect notifier and handler for  rotary encoder */
    rotary_notifier = std::make_unique<QSocketNotifier>(
        get_rotary_button_handle(), QSocketNotifier::Read);

    connect(rotary_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_rotary);

    /* connect notifier and handler for push button */
    button_notifier = std::make_unique<QSocketNotifier>(
        get_push_button_handle(), QSocketNotifier::Read);

    connect(button_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_button);

    rotary_notifier->setEnabled(true);
    button_notifier->setEnabled(true);
}

/*****************************************************************************/
VolumeButton::~VolumeButton() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::read_rotary(int filehandle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = get_input_event(filehandle);
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
    rotary_notifier->setEnabled(active);
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::read_button(int filehandle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = get_input_event(filehandle);
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
bool DigitalRooster::VolumeButton::get_button_state() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return button_state;
}

/*****************************************************************************/
