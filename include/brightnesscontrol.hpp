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

#ifndef _BRIGHTNESSCONTROL_HPP_
#define _BRIGHTNESSCONTROL_HPP_

#include <QObject>
#include <array>
#include <memory>

#include "IHardware.hpp"

namespace DigitalRooster {

class IBrightnessStore;


/**
 * RBG intensity to illuminance coefficients
 * https://en.wikipedia.org/wiki/Luma_(video)
 */
constexpr std::array<double, 3> luma_coeffs = {0.213, 0.715, 0.073};

/**
 * Controls display brightness settings
 * mainly translates linear 0-100% brightness in logarithmic pwm value
 * and stores current values in configuration manager
 */
class BrightnessControl : public QObject {
    Q_OBJECT
    Q_PROPERTY(int brightness READ get_brightness NOTIFY brightness_changed)
    Q_PROPERTY(int active_brightness READ get_active_brightness WRITE
            set_active_brightness)
    Q_PROPERTY(int standby_brightness READ get_standby_brightness WRITE
            set_standby_brightness)
    Q_PROPERTY(bool has_sensor READ has_als_sensor)
    Q_PROPERTY(bool feedback READ adaptive_mode WRITE set_adaptive_mode NOTIFY
            adaptive_mode_changed)
public:
    /**
     * Filter coefficients
     * 2nd order lowpass FIR for values x(n), x(n-1), x(n-2)
     */
    static constexpr std::array<double, 3> filter_coeffs = {1.0, 0.5, 0.3};

    /**
     * Constructor
     * @param store configuration storage - \ref
     * DigitalRooster::ConfigurationManager
     */
    explicit BrightnessControl(IBrightnessStore& store, Hal::IHardware* hw);
    ~BrightnessControl() = default;

    /**
     * Updates the PWM settings with value depending on standby/active state
     * @param bl_brightness backlight brightness (0..100%)
     */
    void update_backlight(double bl_brightness = 0);

    /**
     * Get current backlight brightness value non-linear in range (0..100%)
     * @return current_brightness
     */
    int get_brightness() const;

    /**
     * Get linear (0..100%) brightness
     * @return brightness
     */
    int get_active_brightness() const;

    /**
     * Get linear (0..100%) brightness
     * @return brightness
     */
    int get_standby_brightness() const;

    /**
     * Uses auto backlight control based on sensor value
     * @return enable true/false
     */
    bool adaptive_mode() const;

    /**
     * Check if Ambient Light Sensor available
     * @return enable true/false
     */
    bool has_als_sensor();

public slots:
    /**
     * Use auto backlight control based on sensor value
     * (if sensor exists)
     * @param enable true/false
     */
    void set_adaptive_mode(bool enable);

    /**
     * Change the brightness for active mode
     * @param brightness 0..100 (linear)
     */
    void set_active_brightness(int brightness);

    /**
     * Change the brightness for standby mode
     * @param brightness 0..100 (linear)
     */
    void set_standby_brightness(int brightness);

    /**
     * Set Backlight brightness to active/standby
     * @param active
     */
    void active(bool active);

    /**
     * Ambient Light sensor data available
     * @param brightness rgbc values
     */
    void als_value_changed(Hal::AlsValue brightness);

signals:
    /**
     * auto control has been switched off/on
     */
    void adaptive_mode_changed(bool enabled);

    /**
     * Change of current_brightness (non linear)
     * @param perc value for dutycycle (0-100%)
     */
    void brightness_changed(int perc);

private:
    /**
     * configuration and data handler
     */
    IBrightnessStore& cm;

    /**
     * Hardware controller
     */
    Hal::IHardware* hwctrl;

    /**
     * Current backlight brightness 0..100% setting for hardware control
     * Logarithmic measure
     */
    int current_brightness = 0;

    /**
     * Backlight bright (active) or low (standby)
     */
    bool standby = true;

    /**
     * Last 3 Ambient Light Sensor values for red,green,blue and clear
     */
    std::array<std::array<double, 3>, 4> als_readings = {};

    /**
     * Setup signal-slot connection if feedback-control is enabled
     */
    void subscribe_als_value_change();

    /**
     * Put new value in filter array, and apply filter coefficients for each
     * channel
     * @param brightness new input value
     * @return filtered avarage of previous readings
     */
    Hal::AlsValue filter_sensor(const Hal::AlsValue& brightness);
};

/**
 * Convert RGB to relative perceived illuminance based on
 * https://en.wikipedia.org/wiki/Luma_(video)
 * @param rgbc
 * @return illuminance
 */
double rgb_luma(const Hal::AlsValue& rgbc);

/**
 * Linear brightness [0...100%] to
 * logarithmic (perceived) brightness
 * @param lb linear brightness
 * @return logarithmic brightness as int [0..100]
 */
int lin2log(double lb);


} // namespace DigitalRooster
#endif /* _BRIGHTNESSCONTROL_HPP_ */
