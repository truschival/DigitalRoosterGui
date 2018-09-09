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

#ifndef INCLUDE_HWIF_POWERCONTROL_HPP_
#define INCLUDE_HWIF_POWERCONTROL_HPP_

#include <QObject>

namespace DigitalRooster {

/**
 * Controls power-off & reboot
 */
class PowerControl : public QObject {
    Q_OBJECT
public:
    PowerControl() = default;
    Q_INVOKABLE void power_off();
    Q_INVOKABLE void reboot();
};

}; // namespace DigitalRooster


#endif /* INCLUDE_HWIF_POWERCONTROL_HPP_ */
