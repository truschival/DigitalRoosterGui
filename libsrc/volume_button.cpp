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
    , button_file(button)
    , volume(cm->get_volume()) {

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
        button_notifier = open_and_watch(button_file);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << " open file " << button_file << "failed!"
                             << button_file.errorString();
    }

    connect(button_notifier.get(), &QSocketNotifier::activated, this,
        &VolumeButton::read_button);
}

/*****************************************************************************/
std::unique_ptr<QSocketNotifier> DigitalRooster::VolumeButton::open_and_watch(
    QFile& file) {
    if (file.open(QFile::ReadOnly)) {
        auto notifier = std::make_unique<QSocketNotifier>(
            file.handle(), QSocketNotifier::Exception);
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
    if (evt.value > 0 && volume <= 100) {
        volume += 1;
    }
    if (evt.value < 0 && volume > 0) {
        volume -= 1;
    }
    /* signal even if it didn't change -> inform subscribers about event */
    emit volume_changed(volume);
}

/*****************************************************************************/
void DigitalRooster::VolumeButton::read_button(int filehandle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << filehandle;
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
int DigitalRooster::VolumeButton::get_volume() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return volume;
}

/*****************************************************************************/
bool DigitalRooster::VolumeButton::get_button_state() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return button_state;
}

/*****************************************************************************/
