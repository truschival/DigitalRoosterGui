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

#include <QString>
namespace DigitalRooster {

/**
 * Logger to write to stdout
 */
void setup_logger_stdout();
/**
 * Logger to write to file
 * @param filename
 */
void setup_logger_file(const QString& filename);

} // namespace DigitalRooster

#endif /*_LOGGER_HPP_ */
