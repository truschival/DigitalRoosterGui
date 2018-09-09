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
#include <memory>
namespace DigitalRooster {

class ConfigurationManager;

/**
 * Controls display brightness settings
 */
class BrightnessControl : public QObject {
    Q_OBJECT
    Q_PROPERTY(int brightness READ get_brightness WRITE set_brightness)
public:
    /**
     * Constructor
     * @param confman configuration
     */
    BrightnessControl(std::shared_ptr<ConfigurationManager> confman);
    ~BrightnessControl() = default;

    /**
     * Get appoximate logarithmic perceived brightness
     * for linear input - this value is written to hardware
     * @return logarithmic brightness
     */
    int get_log_brightness();

    /**
     * Get linear (0..100%) brightness
     * @return brightness
     */
    int get_brightness();

public slots:
    /**
     * Change the brightness to new value
     * @param brightness 0..100 (linear)
     */
    void set_brightness(int brightness);

signals:
    /**
     * Some hardware caused brightness change
     * @param brightness 0..100 (linear)
     */
    void brightness_changed(int brightness);

private:
    /**
     * Central configuration and data handler
     */
    std::shared_ptr<ConfigurationManager> cm;

    int linear_brightness = 0;
    int log_brightness;
};

} // namespace DigitalRooster
