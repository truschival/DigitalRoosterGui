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

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLoggingCategory>

#include "appconstants.hpp"
#include "logger.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "util");
/*****************************************************************************/
namespace DigitalRooster {
/**
 * Global object
 */
static QCommandLineParser cmdline;

/*****************************************************************************/
const QCommandLineParser& get_commandline_options(
    const QCoreApplication& app, const QString& desc) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    QCommandLineOption logstdout({"s", CMD_ARG_LOG_STDOUT},
        QString("log to stdout") // description
    );

    QCommandLineOption logfile({"l", CMD_ARG_LOG_FILE},
        QString("application log <file>"), // description
        CMD_ARG_LOG_FILE,                  // value name
        DEFAULT_LOG_FILE);

    QCommandLineOption confpath({"c", CMD_ARG_CONFIG_FILE},
        QString("configuration file path default: ") + DEFAULT_CONFIG_FILE_PATH,
        CMD_ARG_CONFIG_FILE,
        DEFAULT_CONFIG_FILE_PATH // default
    );

    QCommandLineOption cachedir({"d", CMD_ARG_CACHE_DIR},
        QString("application cache <directory> default: ") +
            DEFAULT_CACHE_DIR_PATH,
        CMD_ARG_CACHE_DIR,     // value name
        DEFAULT_CACHE_DIR_PATH // default
    );

    cmdline.addOption(logstdout);
    cmdline.addOption(confpath);
    cmdline.addOption(logfile);
    cmdline.addOption(cachedir);
    cmdline.addHelpOption();
    cmdline.setApplicationDescription(desc);
    cmdline.addVersionOption();
    cmdline.process(app);
    return cmdline;
}

/*****************************************************************************/
void setup_log_facility(const QCommandLineParser& cmdline) {
    try {
        if (cmdline.isSet(CMD_ARG_LOG_STDOUT)) {
            setup_logger_stdout(); // Write log to stdout
        } else {
            // coverity[UNCAUGHT_EXCEPT]
            setup_logger_file(cmdline.value(CMD_ARG_LOG_FILE));
        }
    } catch (std::exception& exc) {
        // fallback behavior
        setup_logger_stdout();
    }
    qInfo() << "log to stdout: " << cmdline.isSet(CMD_ARG_LOG_STDOUT);
    qInfo() << "logfile: " << cmdline.value(CMD_ARG_LOG_FILE);
    qInfo() << "config: " << cmdline.value(CMD_ARG_CONFIG_FILE);
    qInfo() << "cachedir: " << cmdline.value(CMD_ARG_CACHE_DIR);
}

/*****************************************************************************/

} // namespace DigitalRooster
