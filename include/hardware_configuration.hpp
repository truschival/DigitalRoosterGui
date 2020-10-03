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
 * Name of environment variable for backlight device
 */
const QString BACKLIGHT_PATH_ENV_VAR_NAME{"BACKLIGHT_PATH"};

/**
 * Name of environment variable for ALS device
 */
const QString ALS_PATH_ENV_VAR_NAME{"ALS_PATH"};

/**
 * Name of environment variable to set push button event path
 */
const QString PUSH_BUTTON_PATH_ENV_VAR_NAME{"DR_PUSH_EVENT_PATH"};

/**
 * Name of environment variable to set rotary encoder event path
 */
const QString ROTARY_PATH_ENV_VAR_NAME{"DR_ROTARY_EVENT_PATH"};

/**
 * Helper class to resolve input event devices according to environment
 * variables <br> If existing following environment variables take priority:
 * <br>
 * DR_PUSH_EVENT_PATH= \ref PUSH_BUTTON_PATH_ENV_VAR_NAME <br>
 * DR_ROTARY_EVENT_PATH= \ref ROTARY_PATH_ENV_VAR_NAME <br>
 * ALS_PATH = \ref ALS_PATH_ENV_VAR_NAME <br>
 * BACKLIGHT_PATH = \ref BACKLIGHT_PATH_ENV_VAR_NAME <br>
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
     * Path to /sys/class/leds/.... that controls the backlight
     * @return \ref sys_backlight_path
     */
    QString get_backlight_path() const {
        return sys_backlight_path;
    };

    /**
     * Path to rotary encoder event input device
     * @return \ref dev_rotary_event_path
     */
    QString get_rotary_event_path() const {
        return dev_rotary_event_path;
    };

    /**
     * Path to ambient light sensor directory in /sys/
     * must append the sensor value files e.g. 'in_intensity_clear_raw'
     * @return \ref sys_als_path
     */
    QString get_als_path() const {
        return sys_als_path;
    };

    /**
     * Path to push button event input device /dev/input/eventXXX
     * @return \ref dev_push_button_event_path
     */
    QString get_push_button_event_path() const {
        return dev_push_button_event_path;
    };

private:
    /**
     * Path in /sys for backlight control
     */
    QString sys_backlight_path{"/sys/class/leds/bl/brightness"};

    /**
     * Path in /sys for ambient light
     */
    QString sys_als_path{"/sys/bus/iio/devices/iio:device0/"};

    /**
     * Dev-File path for rotary events
     */
    QString dev_rotary_event_path;

    /**
     * Dev-File path for push button events
     */
    QString dev_push_button_event_path;
};

} /* namespace Hal */

#endif /* _HARDWARE_CONFIGURATION_HPP_ */
