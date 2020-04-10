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
#include <QObject>

#include <chrono>

#include <signal.h>

#include "appconstants.hpp"
#include "testcommon.hpp"

#include "RestApi.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

void term_handler(int sig){
	exit(0);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    setup_test_logs();

    ConfigurationManager cm(DEFAULT_CONFIG_FILE_PATH, DEFAULT_CACHE_DIR_PATH);
    RestApi restserver(cm, cm, cm, cm, cm);
    cm.update_configuration();

    signal(SIGTERM,term_handler);

//    QTimer exitTimer;
//    QObject::connect(
//        &exitTimer, &QTimer::timeout, &app, QCoreApplication::quit);
//    exitTimer.start(std::chrono::seconds(10));
    app.exec();
    return 0;
}
