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
    DigitalRooster::ConfigurationManager* cm, QString rotary_encoder,
    QString button, QObject* parent)
    : rotary_file(rotary_encoder)
    , button_file(button) {

    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* connect notifier and handler for  rotary encoder */
    try {
        rotary_notifier = open_and_watch(rotary_file);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << " open file " << rotary_file << "failed! "
                             << rotary_file.errorString();
    }
    connect(rotary_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_rotary);

    /* connect notifier and handler for push button */
    try {
        // Trigger on edge, not data available to read
        button_notifier =
            open_and_watch(button_file, QSocketNotifier::Exception);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << " open file " << button_file << "failed!"
                             << button_file.errorString();
    }

    connect(button_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_button);
}

/*****************************************************************************/
std::unique_ptr<QSocketNotifier> DigitalRooster::VolumeButton::open_and_watch(
    QFile& file, QSocketNotifier::Type type) {
    if (file.open(QFile::ReadOnly)) {
        auto notifier = std::make_unique<QSocketNotifier>(file.handle(), type);
        notifier->setEnabled(true);
        return notifier;
    } else {
        throw std::runtime_error(file.errorString().toStdString());
    }
}

/*****************************************************************************/
VolumeButton::~VolumeButton() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (rotary_file.isOpen())
        rotary_file.close();

    if (button_file.isOpen())
        button_file.close();
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
