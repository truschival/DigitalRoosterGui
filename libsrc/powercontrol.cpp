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

void PowerControl::power_off()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    ::system_poweroff();
}

/*****************************************************************************/
void PowerControl::reboot(){
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    ::system_reboot();
}
/*****************************************************************************/