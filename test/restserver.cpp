/******************************************************************************
 * \filename
 * \brief	minimal program to start a REST server for API integration testen
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QCoreApplication>
#include <QDebug>
#include <QObject>

#include <chrono>
#include <signal.h>

#include "RestApi.hpp"
#include "appconstants.hpp"
#include "configuration_manager.hpp"
#include "testcommon.hpp"

using namespace DigitalRooster;

void term_handler(int sig) {
    exit(0);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    setup_tests();
    qDebug() << argv[0];

    ConfigurationManager cm(DEFAULT_CONFIG_FILE_PATH, DEFAULT_CACHE_DIR_PATH);
    RestApi restserver(cm, cm, cm, cm, cm);
    cm.update_configuration();

    /*
     * coverage data is only generated if program is not killed forcefully
     * -> capture the terminate signal and exit gracefully
     */
    signal(SIGTERM, term_handler);

    //    QTimer exitTimer;
    //    QObject::connect(
    //        &exitTimer, &QTimer::timeout, &app, QCoreApplication::quit);
    //    exitTimer.start(std::chrono::seconds(10));
    app.exec();
    return 0;
}
