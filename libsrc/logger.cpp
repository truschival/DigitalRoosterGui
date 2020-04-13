/******************************************************************************
 * \filename
 * \brief
 *
 * \details basic logging facility
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <memory>

#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

#include "logger.hpp"

namespace DigitalRooster {

static std::shared_ptr<QFile> logfile;

static void messageHandler(
    QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    static bool initialized = false;

    QTextStream out(logfile.get());

    if (!initialized) {
        out << "\n ======= "
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " =======" << endl;
        initialized = true;
    }
    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz ");

    switch (type) {
    case QtInfoMsg:
        out << "INF\t";
        break;
    case QtDebugMsg:
        out << "DBG\t";
        break;
    case QtWarningMsg:
        out << "WRN\t";
        break;
    case QtCriticalMsg:
        out << "CRT\t";
        break;
    case QtFatalMsg:
        out << "FAT\t";
        break;
    }
    out << context.category << "\t" << msg << endl;
    out.flush();
}

/*****************************************************************************/
void setup_logger_file(const QString& filename) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Logging to " << filename;
    logfile = std::make_shared<QFile>(filename);
    if(! logfile->open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text)){
        throw std::system_error(
            make_error_code(std::errc::bad_file_descriptor),
            logfile->errorString().toStdString());
    };
    qInstallMessageHandler(messageHandler);
}

/*****************************************************************************/
void setup_logger_stdout() {
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Logging to stdout";
    logfile = std::make_shared<QFile>();
    logfile->open(stdout, QIODevice::WriteOnly);
    qInstallMessageHandler(messageHandler);
}

/*****************************************************************************/


} // namespace
