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
        QString rotary_encoder = QString("/dev/input/event1"),
        QString button = QString("/sys/class/gpio/gpio22"),
        QObject* parent = nullptr);
    /**
     * check if button is pressed
     * @return pressed/not pressed
     */
    bool get_button_state();

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
    void volume_changed(int increment);
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
    bool button_state = true;

    /**
     * Open the file and create a QSocketNotifier for it
     * @param file file to open and monitor
     * @param type event type to trigger QSocketNotifier::activate
     * @return
     */
    std::unique_ptr<QSocketNotifier> open_and_watch(
        QFile& file, QSocketNotifier::Type type = QSocketNotifier::Read);

private slots:
    /**
     * read event(s) from rotary file handle
     * update volume
     */
    void read_rotary(int filehandle);

    /**
     * read events from button file handle
     * update button_state
     */
    void read_button(int filehandle);
};
} // namespace DigitalRooster
#endif /* VOLUME_BUTTON_HPP */
