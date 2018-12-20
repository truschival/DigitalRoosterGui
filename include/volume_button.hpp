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

namespace DigitalRooster {
class ConfigurationManager;

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
    VolumeButton(DigitalRooster::ConfigurationManager* cm,
        QString rotary_encoder = QString("/dev/input/event0"),
        QString button = QString("/sys/class/gpio/gpio01"),
        QObject* parent = nullptr);

    /**
     * check if button is pressed
     * @return pressed/not pressed
     */
    bool get_button_state();
    /**
     * read current volume
     * @return volume [0...100%]
     */
    int get_volume();

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

signals:
    void volume_changed(int absvolume);
    void button_pressed();
    void button_released();

private:
    QFile rotary_file;
    QFile button_file;
    /**
     * monitors changes on rotary encoder
     */
    std::unique_ptr<QSocketNotifier> rotary_notifier;

    /**
     * monitors changes on push button
     */
    std::unique_ptr<QSocketNotifier> button_notifier;

    /**
     * cached button state
     */
    bool button_state;

    /**
     * volume, incremented, decremented on each rotary event
     * value between 0..100
     */
    int volume = 0;

    int read_rotary();

    int read_button();
};
} // namespace DigitalRooster
#endif /* VOLUME_BUTTON_HPP */
