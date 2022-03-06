// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QCoreApplication>
#include <QDebug>
#include <QObject>

#include <signal.h>

#include "RestApi.hpp"
#include "appconstants.hpp"
#include "configuration.hpp"
#include "testcommon.hpp"
#include "util.hpp"

using namespace DigitalRooster;

void term_handler(int sig) {
    exit(0);
}

/**
 * minimal program to start a REST server for API integration testing
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    const auto& cmdline = get_commandline_options(app);
    setup_log_facility(cmdline);

    setup_tests();
    qDebug() << argv[0];

    Configuration config(
        cmdline.value(CMD_ARG_CONFIG_FILE), cmdline.value(CMD_ARG_CACHE_DIR));
    RestApi restserver(config, config, config, config, config);
    config.update_configuration();

    /*
     * coverage data is only generated if program is not killed forcefully
     * -> capture the terminate signal and exit gracefully
     */
    signal(SIGTERM, term_handler);

    app.exec();
    return 0;
}
