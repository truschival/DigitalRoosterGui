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
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QString>

#include "hardware_configuration.hpp"
#include "hardwarecontrol_stub.hpp"

using namespace Hal;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HardwareControl");



Hal::IHardware::~IHardware(){}

/*****************************************************************************/
HardwareControlStub::~HardwareControlStub() {
    qCInfo(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void HardwareControlStub::system_reboot() {
    qCInfo(CLASS_LC) << Q_FUNC_INFO;
    QCoreApplication::instance()->quit();
}

/*****************************************************************************/
void HardwareControlStub::system_poweroff() {
    qCInfo(CLASS_LC) << Q_FUNC_INFO;
    QCoreApplication::instance()->quit();
}

/*****************************************************************************/
bool HardwareControlStub::als_sensor_available() const {
    qCInfo(CLASS_LC) << Q_FUNC_INFO;
    return false;
};

/*****************************************************************************/
AlsValue HardwareControlStub::read_als_sensor() {
    qCInfo(CLASS_LC) << Q_FUNC_INFO;
    return AlsValue{0, 0, 0, 0};
}

/*****************************************************************************/
void HardwareControlStub::set_backlight(int brightness) {
	 qCInfo(CLASS_LC) << Q_FUNC_INFO;
}
/*****************************************************************************/
