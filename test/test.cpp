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
#include <QObject>
#include <QStandardPaths>
#include <QTimer>

#include <gtest/gtest.h>
#include <memory>

#include "appconstants.hpp"
#include "logger.hpp"
#include "testcommon.hpp"

/**
 * see :
 * https://stackoverflow.com/questions/33829949/how-to-use-qtimers-in-googletest
 */

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    setup_test_logs();
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    QTimer exitTimer;
    QObject::connect(
        &exitTimer, &QTimer::timeout, &app, QCoreApplication::quit);
    exitTimer.start();
    app.exec();
    return ret;
}
/*****************************************************************************/
