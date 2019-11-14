/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _POWERCONTROL_HPP_
#define _POWERCONTROL_HPP_

#include <QObject>

namespace DigitalRooster {

/**
 * Thin abstraction layer between QML and hardware control
 */
class PowerControl : public QObject {
    Q_OBJECT
public:
    /**
     * PowerState
     */
    enum PowerState {
        Active, //!< running
        Standby //!< standby
    };
    Q_ENUM(PowerState)

    PowerControl(QObject* parent = nullptr) : QObject(parent){};
    Q_INVOKABLE void power_off();
    Q_INVOKABLE void reboot();

    PowerControl::PowerState get_power_state() {
        return state;
    }

public slots:
    Q_INVOKABLE void toggle_power_state();
    Q_INVOKABLE void activate();
    Q_INVOKABLE void standby();
signals:
    void power_state_changed(PowerControl::PowerState);
    void becoming_active();
    void going_in_standby();
    void active(bool);

    void shutdown_request();
    void reboot_request();

private:
    /** Default state in standby */
    PowerState state = Standby;
};

}; // namespace DigitalRooster


#endif /* _POWERCONTROL_HPP_ */
