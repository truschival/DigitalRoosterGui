/*************************************************************************************
 * \filename
 * \brief   Main entry point for QML Gui
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *************************************************************************************/

/* only allow QML debugging for Debug builds */
#ifndef NDEBUG
	#define QT_QML_DEBUG
#endif

#include <QDebug>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <memory>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "alarmlistmodel.hpp"
#include "alarmmonitor.hpp"
#include "configuration_manager.hpp"
#include "iradiolistmodel.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"

using namespace DigitalRooster;

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);

    // QLoggingCategory::setFilterRules("*.debug=true");
    qDebug() << "SSL Support: " << QSslSocket::supportsSsl()
             << QSslSocket::sslLibraryVersionString();

    app.setWindowIcon(QIcon("qrc:/ClockIcon48x48.png"));

    qmlRegisterType<PodcastEpisodeModel>(
        "ruschi.PodcastEpisodeModel", 1, 0, "PodcastEpisodeModel");
    qmlRegisterType<DigitalRooster::PodcastEpisode>(
        "ruschi.PodcastEpisode", 1, 0, "PodcastEpisode");
    qmlRegisterType<DigitalRooster::Alarm>("ruschi.Alarm", 1, 0, "Alarm");
    qmlRegisterType<DigitalRooster::IRadioListModel>(
        "ruschi.IRadioListModel", 1, 0, "IRadioListModel");
    qmlRegisterType<DigitalRooster::PlayableItem>(
        "ruschi.PlayableItem", 1, 0, "PlayableItem");

    /*Get available Podcasts */
    auto cm = std::make_shared<ConfigurationManager>();
    cm->update_configuration();
    auto playerproxy = std::make_shared<MediaPlayerProxy>();

    AlarmDispatcher alarmdispatcher(cm);
    AlarmMonitor wd(playerproxy);
    QObject::connect(&alarmdispatcher,
        SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)), &wd,
        SLOT(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    PodcastSourceModel psmodel(cm, playerproxy);
    IRadioListModel iradiolistmodel(cm, playerproxy);
    AlarmListModel alarmlistmodel(cm);

    QQmlApplicationEngine view;
    QQmlContext* ctxt = view.rootContext();
    ctxt->setContextProperty("podcastmodel", &psmodel);
    ctxt->setContextProperty("playerProxy", playerproxy.get());
    ctxt->setContextProperty("alarmlistmodel", &alarmlistmodel);
    ctxt->setContextProperty("iradiolistmodel", &iradiolistmodel);

    view.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
