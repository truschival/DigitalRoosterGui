/******************************************************************************
 * \filename
 * \brief   Main entry point for QML Gui
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

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

#include <iostream>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "alarmlistmodel.hpp"
#include "alarmmonitor.hpp"
#include "appconstants.hpp"
#include "brightnesscontrol.hpp"
#include "configuration_manager.hpp"
#include "hwif/hal.h"
#include "iradiolistmodel.hpp"
#include "logger.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"
#include "powercontrol.hpp"
#include "timeprovider.hpp"
#include "volume_button.hpp"
#include "weather.hpp"

using namespace DigitalRooster;

Q_DECLARE_LOGGING_CATEGORY(MAIN)
Q_LOGGING_CATEGORY(MAIN, "DigitalRooster.main")

/**
 * Global wall clock
 */
std::shared_ptr<TimeProvider> DigitalRooster::wallclock =
    std::make_shared<TimeProvider>();

/*****************************************************************************/
int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);

    std::cout << APPLICATION_NAME.toStdString().c_str() << " - "
              << GIT_REVISION.toStdString().c_str() << "\n logging to: ";
    std::cout << QString(QStandardPaths::writableLocation(
                             QStandardPaths::TempLocation) +
                     "/Digitalrooster.log")
                     .toStdString()
                     .c_str()
              << std::endl;

    Logger logfacility;
    qCDebug(MAIN) << "SSL Support: " << QSslSocket::supportsSsl()
                  << QSslSocket::sslLibraryVersionString();

    // Initialize Hardware (or call stubs)
    ::setup_hardware();

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
    playerproxy->set_volume(cm->get_volume());

    AlarmDispatcher alarmdispatcher(cm);
    AlarmMonitor alarmmonitor(playerproxy);
    QObject::connect(&alarmdispatcher,
        SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)),
        &alarmmonitor,
        SLOT(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    PodcastSourceModel psmodel(cm, playerproxy);
    IRadioListModel iradiolistmodel(cm, playerproxy);
    AlarmListModel alarmlistmodel(cm);
    Weather weather(cm);
    PowerControl power;
    BrightnessControl brightness(cm);
    /* PowerControl standby sets brightness */
    QObject::connect(&power, SIGNAL(going_in_standby()), &brightness,
        SLOT(restore_standby_brightness()));
    QObject::connect(&power, SIGNAL(becoming_active()), &brightness,
        SLOT(restore_active_brightness()));
    /* Powercontrol standby stops player */
    QObject::connect(
        &power, SIGNAL(going_in_standby()), playerproxy.get(), SLOT(stop()));
    /* AlarmDispatcher sets activates system */
    QObject::connect(
        &alarmdispatcher, SIGNAL(alarm_triggered()), &power, SLOT(activate()));
    /* Alarm Monitor alarm timeouts deactivates the system */
    QObject::connect(
        &alarmmonitor, SLOT(alarm_timeout_occurred), &power, SLOT(standby));
    /* Rotary encoder interface */
    VolumeButton volbtn(cm.get());
    QObject::connect(
        &volbtn, SIGNAL(button_released()), &power, SLOT(toggle_power_state()));
    QObject::connect(&volbtn, SIGNAL(volume_incremented(int)),
        playerproxy.get(), SLOT(increment_volume(int)));
    /* Standby deactivates Volume button events */
    QObject::connect(&power, SIGNAL(active(bool)), &volbtn,
        SLOT(monitor_rotary_button(bool)));
    QObject::connect(playerproxy.get(), SIGNAL(volume_changed(int)), cm.get(),
        SLOT(set_volume(int)));

    /* we start in standby */
    power.standby();

    QQmlApplicationEngine view;
    QQmlContext* ctxt = view.rootContext();
    ctxt->setContextProperty("podcastmodel", &psmodel);
    ctxt->setContextProperty("playerProxy", playerproxy.get());
    ctxt->setContextProperty("alarmlistmodel", &alarmlistmodel);
    ctxt->setContextProperty("iradiolistmodel", &iradiolistmodel);
    ctxt->setContextProperty("weather", &weather);
    // TODO remove next line - only for testing of settingspage!
    ctxt->setContextProperty("config", cm.get());
    ctxt->setContextProperty("powerControl", &power);
    ctxt->setContextProperty("brightnessControl", &brightness);
    ctxt->setContextProperty("volumeButton", &volbtn);

    view.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
