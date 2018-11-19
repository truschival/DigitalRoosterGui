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
#include <memory>
namespace DigitalRooster {

class ConfigurationManager;

/**
 * Controls display brightness settings
 */
class BrightnessControl : public QObject {
    Q_OBJECT
    Q_PROPERTY(int brightness READ get_brightness WRITE set_brightness NOTIFY
            brightness_changed)
public:
    /**
     * Constructor
     * @param confman configuration
     */
    BrightnessControl(std::shared_ptr<ConfigurationManager> confman);
    ~BrightnessControl() = default;

    /**
     * Get linear (0..100%) brightness
     * @return brightness
     */
    int get_brightness();
   
	/**
     * Linear brightness [0...100%] to
     * logarithmic (perceived) brightness
     * @param lb linear brightness
     * @return logarithmic brightness as int [0..100] 
     */
    int lin2log(int lb);

public slots:
    /**
     * Change the brightness to new value and store it as 
     * active brightness value
     * @param brightness 0..100 (linear)
     */
    void set_brightness(int brightness);

	/**
     * Set brightness to poweron brightness
     */
    void restore_active_brightness();

	/**
     * Set brightness to sleep brightness
     */
    void restore_standby_brightness();

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
	
	/**
	 * Current brightness setting (linear)
	 */
	int linear_brightness;
};

} // namespace DigitalRooster
#endif /* _BRIGHTNESSCONTROL_HPP_ */

