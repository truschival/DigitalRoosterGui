/******************************************************************************
 * \filename
 * \brief	Interface for update/access of Brightness Settings
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef INCLUDE_IBRIGHTNESSTORE_HPP_
#define INCLUDE_IBRIGHTNESSTORE_HPP_

#include <chrono>

namespace DigitalRooster {
/**
 * Interface for addition, access of Brightness Settings
 * actual storage \ref DigitalRooster::ConfigurationManager
 */
class IBrightnessStore {
public:
    virtual int get_standby_brightness() const =0;
    virtual int get_active_brightness() const =0;
    /**
     * user changed standby brightness
     * @param brightness new volume settings (0..100)
     */
    virtual void set_standby_brightness(int brightness) =0;

    /**
     * user changed standby brightness
     * @param brightness new volume settings (0..100)
     */
    virtual void set_active_brightness(int brightness) =0;

    /**
     * virtual destructor
     */
    virtual ~IBrightnessStore(){};
};
} // namespace DigitalRooster

#endif /* INCLUDE_IBRIGHTNESSTORE_HPP_ */
