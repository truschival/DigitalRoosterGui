// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QCoreApplication>
#include <QDebug>
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
    setup_tests();
    qDebug() << argv[0];
    DigitalRooster::setup_logger_file(DigitalRooster::DEFAULT_LOG_FILE);

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
