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

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QObject>
#include <QStandardPaths>
#include <QTimer>
#include <gtest/gtest.h>

#include "appconstants.hpp"
#include "logger.hpp"
#include "timeprovider.hpp"
#include <memory>

std::shared_ptr<DigitalRooster::TimeProvider> DigitalRooster::wallclock =
    std::make_shared<DigitalRooster::TimeProvider>();


/**
 * Log file path
 */
const QString DigitalRooster::DEFAULT_LOG_PATH(
    QDir(TEST_FILE_PATH).filePath("Digitalrooster_tests.log"));

/**
 * Default configuration file path
 */
const QString DigitalRooster::DEFAULT_CONFIG_FILE_PATH(
    QDir(QDir(TEST_FILE_PATH).filePath("testconfig"))
        .filePath(APPLICATION_NAME + ".json"));

/**
 * Cache directory
 */
const QString DigitalRooster::DEFAULT_CACHE_DIR_PATH(
    QDir(TEST_FILE_PATH).filePath("testcache"));

/**
 * see :
 * https://stackoverflow.com/questions/33829949/how-to-use-qtimers-in-googletest
 */

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    try {
        DigitalRooster::setup_logger_file(DigitalRooster::DEFAULT_LOG_PATH);
    } catch (std::system_error& exc) {
        DigitalRooster::setup_logger_stdout(); // Write log to stdout
    }

    QDir(DigitalRooster::TEST_FILE_PATH).mkdir("testcache");
    QDir(DigitalRooster::TEST_FILE_PATH).mkdir("testconfig");

    QLoggingCategory::setFilterRules("*.debug=true");

    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    QTimer exitTimer;
    QObject::connect(
        &exitTimer, &QTimer::timeout, &app, QCoreApplication::quit);
    exitTimer.start();
    app.exec();
    return ret;
}
