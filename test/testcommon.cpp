/******************************************************************************
 * \filename
 * \brief	common test functions
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QDebug>
#include <QDir>
#include <QLoggingCategory>
#include <QString>

#include <exception>
#include <memory>

#include "appconstants.hpp"
#include "logger.hpp"
#include "testcommon.hpp"
#include "timeprovider.hpp"

/**
 * Log file path
 */
const QString DigitalRooster::DEFAULT_LOG_FILE(
    QDir(DigitalRooster::TEST_FILE_PATH).filePath(STR_VALUE(TEST_LOG_NAME)));
/**
 * Default configuration file path
 */
const QString DigitalRooster::DEFAULT_CONFIG_FILE_PATH(
    QDir(QDir(DigitalRooster::TEST_FILE_PATH).filePath("testconfig"))
        .filePath(DigitalRooster::APPLICATION_NAME + ".json"));
/**
 * Cache directory
 */
const QString DigitalRooster::DEFAULT_CACHE_DIR_PATH(
    QDir(DigitalRooster::TEST_FILE_PATH).filePath("testcache"));

/**
 * Default instance for clock
 * declared extern in timeprovider.hpp
 */
std::shared_ptr<DigitalRooster::TimeProvider> DigitalRooster::wallclock =
    std::make_shared<DigitalRooster::TimeProvider>();


/*****************************************************************************/
void setup_tests() {
    QDir(DigitalRooster::TEST_FILE_PATH).mkdir("testcache");
    QDir(DigitalRooster::TEST_FILE_PATH).mkdir("testconfig");
    QLoggingCategory::setFilterRules("*.debug=true");
}

/*****************************************************************************/
