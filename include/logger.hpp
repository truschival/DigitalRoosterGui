/******************************************************************************
 * \filename
 * \brief	Basic logging facility
 *
 * \details will install message handler to output to file
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <QStandardPaths>
#include <QString>

namespace DigitalRooster {


class Logger {

public:
    Logger(QString filename = QString(
               QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
               "/Digitalrooster.log"));
};
} // namespace DigitalRooster

#endif /*_LOGGER_HPP_ */
