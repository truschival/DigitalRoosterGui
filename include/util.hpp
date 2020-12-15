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
#include <QUrl>
#include <QUuid>

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

/**
 * Parse string into QUrl and validate
 * @throws invalid_argument if urlstr cannnot be parsed into valid url
 * @param urlstr string representing the URL
 * @return url
 */
QUrl valid_url_from_string(const QString& urlstr);

/**
 * Parse string into QUuid and validate
 * @throws invalid_argument if uuidstr cannnot be parsed into valid UUID
 * @param uuidstr string e.g. 247c4f9d-9626-4061-a8cc-1bd2249a0a20
 * @return UUID
 */
QUuid valid_uuid_from_String(const QString& uuidstr);

/**
 * Simple check if value is in range 0-100 used for positive 
 * Integer range like volume percentage, brightness percentage....
 * @param value to check
 * @return value>=0 && value <=100
 */
bool value_in_0_100(int value);

} // namespace DigitalRooster
#endif /* INCLUDE_UTIL_HPP_ */
