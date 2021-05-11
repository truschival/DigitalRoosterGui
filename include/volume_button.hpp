/******************************************************************************
 * \filename
 * \brief	Interaction with hardware volume (rotary encoder with push button)
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _VOLUME_BUTTON_HPP_
#define _VOLUME_BUTTON_HPP_

#include <QFile>
#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <memory>

#include "IHardware.hpp"

namespace DigitalRooster {
/**
 * Abstracts access to rotary button event interfaces and notifies updated
 * absolute volume
 */
class VolumeButton : public QObject {
    Q_OBJECT;

public:
    /**
     * Construct with path to event interface for rotary encoder and
     * push button GPIO
     */
    explicit VolumeButton(QObject* parent = nullptr);

    /**
     * disable copy-constructor and copy assignment
     */
    VolumeButton(const VolumeButton& rhs) = delete;
    VolumeButton& operator=(const VolumeButton& rhs) = delete;

    /**
     * We could make it move-constructible but not needed now
     */
    VolumeButton(VolumeButton&& rhs) = delete;
    VolumeButton& operator=(VolumeButton&& rhs) = delete;

    /**
     * constructor needs to close file descriptors!
     */
    virtual ~VolumeButton();

public slots:
    /**
     * Enable/Disable generation of rotary button events
     * @sa volume_incremented
     * @param active state
     */
    void monitor_rotary_button(bool active);

    /**
     * process rotary events
     * @param evt event that occurred
     */
    void process_rotary_event(const Hal::InputEvent& evt);

    /**
     * process Push-button events
     * @param evt event that occurred
     */
    void process_key_event(const Hal::InputEvent& evt);

signals:
    /**
     * volume was incremented/decremented by increment
     */
    void volume_incremented(double increment);
    /**
     * Push button was/is pressed
     */
    void button_pressed();

    /**
     * Push button was/is released
     */
    void button_released();

private:
    /**
     * if false no volume_increment events will be generated
     */
    bool enable_volume_changes;
};
} // namespace DigitalRooster
#endif /* VOLUME_BUTTON_HPP */
