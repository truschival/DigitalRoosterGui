/******************************************************************************
 * \filename
 * \brief    Hardware interface
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2020 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#ifndef INCLUDE_IHARDWARE_HPP_
#define INCLUDE_IHARDWARE_HPP_

#include <QDebug>
#include <QObject>

namespace Hal {

/**
 * Simplified linux/input event to allow portable code
 */
struct InputEvent {
    /** Key code (if key-event) */
    int code;
    /** Value pressed/released increment etc. */
    int value;
    /** Type key event / mouse etc. ... (0x01 EV_KEY) */
    int type;
};

/**
 * Ambient Light Sensor Value
 */
struct AlsValue {
    /** Red Channel */
    uint16_t red;
    /** Green Channel */
    uint16_t green;
    /** Blue Channel */
    uint16_t blue;
    /** Clear Channel */
    uint16_t clear;
};

/**
 * Generic Hardware interface
 */
class IHardware : public QObject {
    Q_OBJECT
public:
    explicit IHardware(QObject* parent = nullptr)
        : QObject(parent){};
    /**
     * Check if we have a Ambient light sensor
     * @return
     */
    virtual bool als_sensor_available() const = 0;

    /**
     * Read value from ALS sensor
     * @return
     */
    virtual AlsValue read_als_sensor() = 0;

    /** need virtual destructor */
    virtual ~IHardware();

public slots:
    /**
     * Power management functions
     */
    virtual void system_reboot() = 0;
    virtual void system_poweroff() = 0;

    /**
     * Set backlight brightness of display on target
     * @param brightness 0..100 %
     */
    virtual void set_backlight(int brightness) = 0;

signals:
    /**
     * Button Event
     * @param evt
     */
    void button_event(const InputEvent evt);

    /**
     * Rotary Encoder event
     * @param evt
     */
    void rotary_event(const InputEvent evt);

    /**
     * New ambient light value available
     * @param als updated ambient light sensor reading
     */
    void als_value_changed(AlsValue als);
};
}; // namespace Hal


inline QDebug operator<<(QDebug& lhs, const Hal::AlsValue& rhs) {
    lhs << "r:" << rhs.red << "g:" << rhs.green << "b:" << rhs.blue
        << "c:" << rhs.clear;
    return lhs;
}

#endif /* INCLUDE_IHARDWARE_HPP_ */
