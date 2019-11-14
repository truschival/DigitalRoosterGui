/******************************************************************************
 * \filename
 * \brief    stub hardware interface functions
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#include <QLoggingCategory>
#include <QString>
#include <exception>
#include <memory>

#include "hwif/hardware_configuration.hpp"
#include "hwif/hardware_control.hpp"


using namespace Hal;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HAL");

/*****************************************************************************/
HardwareControl::HardwareControl(
    Hal::HardwareConfiguration& cfg, QObject* parent)
    : QObject(parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void HardwareControl::generate_button_event(int file_handle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void HardwareControl::generate_rotary_event(int file_handle) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void HardwareControl::system_reboot() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void  HardwareControl::system_poweroff() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
int HardwareControl::set_brightness(int brightness) {
    return 0;
}
/*****************************************************************************/
