/******************************************************************************
 * \filename
 * \brief		Hardware abstraction interface functions
 *
 * \details		implementation is target specific
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/


#ifndef _HARDWARE_CONTROLSTUB_HPP_
#define _HARDWARE_CONTROLSTUB_HPP_

#include "IHardware.hpp"
#include "hardware_configuration.hpp"
#include <QObject>

namespace Hal {

/**
 * Abstracts access to underlying files and hardware
 */
class HardwareControlStub : public IHardware {
public:
    /**
     * Empty default constructor of QObject
     */
    explicit HardwareControlStub(QObject* parent = nullptr)
        : IHardware(parent){};

    /**
     * destructor needs to close file descriptors!
     */
    virtual ~HardwareControlStub();

    /**
     * Check if we have a Ambient light sensor
     * @return
     */
    bool als_sensor_available() const override;

    /**
     * Read value from ALS sensor
     * @return
     */
    AlsValue read_als_sensor() override;
public slots:
    /**
     * Power management functions
     */
    virtual void system_reboot() override;
    virtual void system_poweroff() override;

    /**
     * Set brightness of display on target
     * @param brightness 0..100 %
     */
    virtual void set_backlight(int brightness) override;
};

};     // namespace Hal
#endif /* _HARDWARE_CONTROLSTUB_HPP_ */
