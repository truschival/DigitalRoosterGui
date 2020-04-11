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
#include "util.hpp"

using namespace DigitalRooster;

void term_handler(int sig) {
    exit(0);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    const auto& cmdline = get_commandline_options(app);
    setup_log_facility(cmdline);

    setup_tests();
    qDebug() << argv[0];

    ConfigurationManager cm(
        cmdline.value(CMD_ARG_CONFIG_FILE), cmdline.value(CMD_ARG_CACHE_DIR));
    RestApi restserver(cm, cm, cm, cm, cm);
    cm.update_configuration();

    /*
     * coverage data is only generated if program is not killed forcefully
     * -> capture the terminate signal and exit gracefully
     */
    signal(SIGTERM, term_handler);

    app.exec();
    return 0;
}
