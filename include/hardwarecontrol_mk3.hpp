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


#ifndef _HARDWARE_CONTROL_HPP_
#define _HARDWARE_CONTROL_HPP_

#include <QFile>
#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <fstream>
#include <memory>

#include "IHardware.hpp"
#include "hardware_configuration.hpp"

namespace Hal{

/**
 * Abstracts access to underlying files and hardware
 */
class HardwareControlMk3 : public IHardware{
public:
    /**
     * Construct with path to event interface for rotary encoder and
     * push button GPIO
     */
    explicit HardwareControlMk3(
        const Hal::HardwareConfiguration& cfg, QObject* parent = nullptr);

    /**
     * disable copy-constructor and copy assignment
     */
    HardwareControlMk3(const HardwareControlMk3& rhs) = delete;
    HardwareControlMk3& operator=(const HardwareControlMk3& rhs) = delete;

    /**
     * We could make it move-constructible but not needed now
     */
    HardwareControlMk3(HardwareControlMk3&& rhs) = delete;
    HardwareControlMk3& operator=(HardwareControlMk3&& rhs) = delete;

    /**
     * destructor needs to close file descriptors!
     */
    virtual ~HardwareControlMk3() = default;

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
    void system_reboot() override;
    void system_poweroff() override;

    /**
     * Set brightness of display on target
     * @param brightness 0..100 %
     */
    void set_backlight(int brightness) override;

private:
    /**
     * ALS Sensor available
     */
    bool als_sensor_ok;

    /**
     * stream reading sensor value
     */
    std::ifstream als_clear;
    /**
     * stream reading sensor value
     */
    std::ifstream als_red;
    /**
     * stream reading sensor value
     */
    std::ifstream als_green;
    /**
     * stream reading sensor value
     */
    std::ifstream als_blue;

    /**
     * Stream writing pwm values
     */
    std::ofstream backlight_pwm;

    /**
     * monitors changes on rotary encoder
     */
    std::unique_ptr<QSocketNotifier> rotary_notifier;

    /**
     * monitors changes on push button
     */
    std::unique_ptr<QSocketNotifier> button_notifier;

    /**
     * Timer Event (QObject timer) for cyclic polling ALS
     */
    int evt_timer_id = -1;

    /**
     * emits a button_event with info @ref button_notifier triggers
     * @param file_handle socket of button_notifier
     */
    void generate_button_event(int file_handle);

    /**
     * emits a rotary_event with info when @ref rotary_notifier triggers
     * @param file_handle socket of \ref rotary_notifier
     */
    void generate_rotary_event(int file_handle);

private slots:
    /**
     * React to QObject Timer events
     * @param evt
     */
    void timerEvent(QTimerEvent* evt) override;
};

};     // namespace Hal
#endif /* _HARDWARE_CONTROL_HPP_ */
