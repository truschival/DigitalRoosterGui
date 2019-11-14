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
#include <memory>

#include "hardware_configuration.hpp"

namespace Hal {

/**
 * Simplified linux/input event to allow portable code
 */
struct InputEvent {
    int code;
    int value;
    int type;
};

const int EVENT_KEY = 0x01;
const int KEY_PRESSED = 0x01;
const int PUSHBUTTON_CODE = 102;


/**
 * Abstracts access to underlying files and hardware
 */
class HardwareControl : public QObject {
    Q_OBJECT;

public:
    /**
     * Construct with path to event interface for rotary encoder and
     * push button GPIO
     */
    explicit HardwareControl(
        Hal::HardwareConfiguration& cfg, QObject* parent = nullptr);

    /**
     * disable copy-constructor and copy assignment
     */
    HardwareControl(const HardwareControl& rhs) = delete;
    HardwareControl& operator=(const HardwareControl& rhs) = delete;

    /**
     * We could make it move-constructible but not needed now
     */
    HardwareControl(HardwareControl&& rhs) = delete;
    HardwareControl& operator=(HardwareControl&& rhs) = delete;

    /**
     * destructor needs to close file descriptors!
     */
    virtual ~HardwareControl() = default;

public slots:
    /**
     * Power management functions
     */
    virtual void system_reboot();
    virtual void system_poweroff();

    /**
     * Set brightness of display on target
     * @param brightness 0..100 %
     */
    virtual int set_brightness(int brightness);

signals:
    void button_event(const InputEvent evt);

    void rotary_event(const InputEvent evt);

private:

    /**
     * monitors changes on rotary encoder
     */
    std::unique_ptr<QSocketNotifier> rotary_notifier;

    /**
     * monitors changes on push button
     */
    std::unique_ptr<QSocketNotifier> button_notifier;

    /**
     * emits a button_event with info @ref button_notifier triggers
     * @param file_handle socket of button_notifier
     */
    void generate_button_event(int file_handle);
    /**
     * emits a rotary_event with info when @ref rotary_notifier triggers
     * @param file_handle socket fo rotary_notifier
     */
    void generate_rotary_event(int file_handle);

};

};     // namespace Hal
#endif /* _HARDWARE_CONTROL_HPP_ */
