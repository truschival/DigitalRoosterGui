/******************************************************************************
 * \filename
 * \brief		functions and classes interfacing hardware
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#pragma once

#include <QObject>

namespace DigitalRooster {

/**
 * Controls display brightness settings
 */
class BrightnessControl : public QObject {
    Q_OBJECT
public:
    BrightnessControl();

    /**
     * Get appoximate logarithmic perceived brightness
     * for linear input - this value is written to hardware
     * @return logarithmic brightness
     */
    int get_log_brightness();

public slots:
    /**
     * Change the brightness to new value
     * @param brightness 0..100 (linear)
     */
    void set_brightness(int brightness);

private:
    int linear_brightness = 0;
    int log_brightness;
};

} // namespace DigitalRooster


