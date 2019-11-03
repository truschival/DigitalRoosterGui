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

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "hwif/hardware_configuration.hpp"

using namespace Hal;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HAL");


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
    auto env = QProcessEnvironment::systemEnvironment();

    /* set push button device name if specified*/
    override_if_env_var_exists(
        PUSH_BUTTON_EVENT_DEV_ENV_VAR_NAME, &dev_push_button_event_name);
    /* Find push_button event path by device name */
    dev_push_button_event_path =
        resolve_name_to_path(dev_push_button_event_name);

    override_if_env_var_exists(
        ROTARY_EVENT_DEV_ENV_VAR_NAME, &dev_rotary_event_name);
    /* Find rotary event path by device name */
    dev_rotary_event_path = resolve_name_to_path(dev_rotary_event_name);

    /*override Paths by environment variables */
    override_if_env_var_exists(
        PUSH_BUTTON_PATH_ENV_VAR_NAME, &dev_push_button_event_path);
    override_if_env_var_exists(
        ROTARY_PATH_ENV_VAR_NAME, &dev_rotary_event_path);
}
/*****************************************************************************/

QString HardwareConfiguration::resolve_name_to_path(const QString& device_name) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << device_name;
    QDir evt_dir = QDir("/dev/input/");
    for (auto& file_name : evt_dir.entryList(QDir::System)) {
        QFile f(file_name);
        qCDebug(CLASS_LC) << "trying " << file_name;
        if (!f.open(QFile::ReadWrite)) {
            qCCritical(CLASS_LC)
                << "Error: open file " << f.fileName() << f.errorString();
            continue;
        }

        char buf[256];
        int ret = ioctl(f.handle(), EVIOCGNAME(sizeof(buf)), buf);
        if (ret < 0) {
            qCCritical(CLASS_LC) << " ioctl failed " << ret;
            continue;
        } else {
            // match name
            QString dev_name(buf);
            qCDebug(CLASS_LC) << "Checking " << dev_name
                              << "equals: " << (dev_name == device_name);
            if (dev_name == device_name) {
                f.close();
                return QFileInfo(f).absoluteFilePath();
            }
        }
        f.close();
    }
    return QString();
}

/*****************************************************************************/


