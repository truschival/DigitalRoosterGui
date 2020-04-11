/******************************************************************************
 * \filename
 * \brief		Util functions that are used globally and don't fit in
 *              somewhere else
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef INCLUDE_UTIL_HPP_
#define INCLUDE_UTIL_HPP_

#include <QCommandLineParser>
#include <QCoreApplication>

namespace DigitalRooster {

/**
 * Setup commandline parser options and parse application arguments
 * @param app started with app(argc, argv);
 * @param desc application description string
 * @return cmdline parser check option with cmdline.value(logfile)
 */
const QCommandLineParser& get_commandline_options(
    const QCoreApplication& app, const QString& desc = QString());

/**
 * Figure out where to send log messages to
 * @param cmdline arguments form cmdline
 */
void setup_log_facility(const QCommandLineParser& cmdline);


} // namespace DigitalRooster
#endif /* INCLUDE_UTIL_HPP_ */
