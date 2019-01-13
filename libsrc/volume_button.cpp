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
DigitalRooster::VolumeButton::VolumeButton(
    DigitalRooster::ConfigurationManager* cm, QObject* parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* connect notifier and handler for  rotary encoder */
    rotary_notifier = std::make_unique<QSocketNotifier>(
        get_rotary_button_handle(), QSocketNotifier::Read);

    connect(rotary_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_rotary);

    /* connect notifier and handler for push button */
    button_notifier = std::make_unique<QSocketNotifier>(
        get_push_button_handle(), QSocketNotifier::Exception);

    connect(button_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_button);
}

/*****************************************************************************/
VolumeButton::~VolumeButton() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::read_rotary(int filehandle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto evt = get_scroll_event(filehandle);
    // only react on -1 or 1 events
    if (evt.value < 0) {
        emit volume_changed(-1);
    }
    if (evt.value > 0) {
        emit volume_changed(1);
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
    /*
     * TODO: maybe we don't even need to read the actual value. Triggered
     * on edges Could be enough.
     */

    // disable during read, otherwise QSocketNotifier is triggered again
    button_notifier->setEnabled(false);
    auto status = get_pushbutton_value(filehandle);
    if (status > 0 && !button_state) {
        qCDebug(CLASS_LC) << "button_pressed";
        emit button_pressed();
    }
    if (status == 0 && button_state) {
        qCDebug(CLASS_LC) << "button_released";
        emit button_released();
    }
    button_state = status > 0;
    button_notifier->setEnabled(true);
}

/*****************************************************************************/
bool DigitalRooster::VolumeButton::get_button_state() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return button_state;
}

/*****************************************************************************/
