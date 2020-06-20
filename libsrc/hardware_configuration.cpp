/******************************************************************************
 * \filename
 * \brief    Configuration object for hardware interface
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QProcessEnvironment>

#include "hardware_configuration.hpp"

using namespace Hal;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HardwareControl");

/*****************************************************************************/
static bool override_if_env_var_exists(
    const QString& env_var_name, QString* var) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << env_var_name;
    auto env = QProcessEnvironment::systemEnvironment();
    if (env.contains(env_var_name) && !env.value(env_var_name).isEmpty()) {
        qCInfo(CLASS_LC) << env_var_name << "forces "
                         << env.value(env_var_name);
        *var = env.value(env_var_name);
        return true;
    }
    return false;
}

/*****************************************************************************/

HardwareConfiguration::HardwareConfiguration() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /*override Paths by environment variables */
    override_if_env_var_exists(
        PUSH_BUTTON_PATH_ENV_VAR_NAME, &dev_push_button_event_path);
    override_if_env_var_exists(
        ROTARY_PATH_ENV_VAR_NAME, &dev_rotary_event_path);
    override_if_env_var_exists(
        BACKLIGHT_PATH_ENV_VAR_NAME, &sys_backlight_path);
}
/*****************************************************************************/
