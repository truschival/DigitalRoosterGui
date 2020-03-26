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

#ifndef _HARDWARE_CONFIGURATION_HPP_
#define _HARDWARE_CONFIGURATION_HPP_

#include <QString>

namespace Hal {

/**
 * Name of environment variable to set push button event path
 */
const QString PUSH_BUTTON_PATH_ENV_VAR_NAME{"DR_PUSH_EVENT_PATH"};

/**
 * Name of environment variable to set rotary encoder event path
 */
const QString ROTARY_PATH_ENV_VAR_NAME{"DR_ROTARY_EVENT_PATH"};

/**
 * Name of environment variable to set push button event path
 */
const QString PUSH_BUTTON_EVENT_DEV_ENV_VAR_NAME{"DR_PUSH_EVENT_DEV"};

/**
 * Name of environment variable to set rotary encoder event path
 */
const QString ROTARY_EVENT_DEV_ENV_VAR_NAME{"DR_ROTARY_EVENT_DEV"};


/**
 * Helper class to resolve input event devices according to environment variables
 * <br>
 * If existing following environment variables take priority: <br>
 * DR_PUSH_EVENT_PATH=   \ref PUSH_BUTTON_PATH_ENV_VAR_NAME <br>
 * DR_ROTARY_EVENT_PATH= \ref ROTARY_PATH_ENV_VAR_NAME <br>
 *
 * The class tries to look up event interface by the device name (as specified in DTS)
 * using these name variables: <br>
 * DR_ROTARY_EVENT_NAME="rotary_abs"
 *  \ref ROTARY_EVENT_DEV_ENV_VAR_NAME = \ref dev_rotary_event_name <br>
 * DR_PUSH_EVENT_NAME="gpiokeys"
 *  \ref PUSH_BUTTON_EVENT_DEV_ENV_VAR_NAME = \ref dev_push_button_event_name
 */
class HardwareConfiguration {
public:
    /**
     * Default constructor reads environment variables and sets
     * @ref dev_rotary_event_path and
     * @ref dev_push_button_event_path accordingly
     */
    HardwareConfiguration();

    /**
     * Path to rotary encoder event input device
     * @return \ref dev_rotary_event_path
     */
    QString get_rotary_event_path() const{
    	return dev_rotary_event_path;
    };

    /**
     * Path to push button event input device /dev/input/eventXXX
     * @return \ref dev_push_button_event_path
     */
    QString get_push_button_event_path() const{
    	return dev_push_button_event_path;
    };

private:
    /**
     * Iterates /dev/input/event to find a device with for given dev_name
     * @param dev_name name as for ioctl EVIOCGNAME
     * @return absolute path
     */
    QString resolve_name_to_path(const QString& dev_name);

    /**
     * Dev-File path for rotary events
     */
    QString dev_rotary_event_path;

    /**
     * Dev-File path for push button events
     */
    QString dev_push_button_event_path;

    /**
     * Input device name of rotary button (must match Device Tree)
     * Default = "rotary_abs"
     */
    QString dev_rotary_event_name{"rotary_abs"};

    /**
     * Input device name of rotary button (must match Device Tree)
     * Default = "gpio_keys"
     */
    QString dev_push_button_event_name{"gpio_keys"};
};

} /* namespace Hal */

#endif /* _HARDWARE_CONFIGURATION_HPP_ */
