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

#include <QFile>
#include <memory>
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
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
            << " ======= " << endl;
        initialized = true;
    }
    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz ");

    switch (type) {
    case QtInfoMsg:
        out << "INF ";
        break;
    case QtDebugMsg:
        out << "DBG ";
        break;
    case QtWarningMsg:
        out << "WRN ";
        break;
    case QtCriticalMsg:
        out << "CRT ";
        break;
    case QtFatalMsg:
        out << "! FATAL ! ";
        break;
    }
    out << context.category << ": " << msg << endl;
    out.flush();
}
/*****************************************************************************/

Logger::Logger(QString filename) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Logging to " << filename;
    logfile = std::make_shared<QFile>(filename);
    logfile->open(QIODevice::ReadWrite | QIODevice::Append| QIODevice::Text);
    qInstallMessageHandler(messageHandler);
}

/*****************************************************************************/

} // namespace