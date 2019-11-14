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

#include <QCommandLineParser>
#include <QDebug>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QtQuick>
// STD C++
#include <iostream>
#include <memory>

// hardware interface
#include "hwif/hardware_configuration.hpp"
#include "hwif/hardware_control.hpp"

// Local classes
#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "alarmlistmodel.hpp"
#include "alarmmonitor.hpp"
#include "appconstants.hpp"
#include "brightnesscontrol.hpp"
#include "configuration_manager.hpp"
#include "iradiolistmodel.hpp"
#include "logger.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"
#include "powercontrol.hpp"
#include "sleeptimer.hpp"
#include "timeprovider.hpp"
#include "volume_button.hpp"
#include "weather.hpp"
#include "wifi_control.hpp"
#include "wifilistmodel.hpp"

using namespace DigitalRooster;

Q_DECLARE_LOGGING_CATEGORY(MAIN)
Q_LOGGING_CATEGORY(MAIN, "DigitalRooster.main")


/*****************************************************************************
 * Application constants
 ****************************************************************************/
/**
 * Log file path
 */
const QString DigitalRooster::DEFAULT_LOG_PATH(
    QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
        .filePath(APPLICATION_NAME + ".log"));

/**
 * Default configuration file path
 */
const QString DigitalRooster::DEFAULT_CONFIG_FILE_PATH(
    QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
        .filePath(CONFIG_JSON_FILE_NAME));

/**
 * Cache directory
 */
const QString DigitalRooster::DEFAULT_CACHE_DIR_PATH(
    QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation))
        .filePath(APPLICATION_NAME));

/**
 * Global wall clock
 */
std::shared_ptr<TimeProvider> DigitalRooster::wallclock =
    std::make_shared<TimeProvider>();

/*****************************************************************************/
int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);
    QGuiApplication app(argc, argv);

    /*
     * Setup Commandline Parser
     */
    QCommandLineParser cmdline;
    QCommandLineOption logstdout({"s", "stdout"}, QString("log to stdout"));
    QCommandLineOption logfile({"l", "logfile"},
        QString("application log <file>"), // description
        QString("logfile")                 // value name
    );
    QCommandLineOption confpath({"c", "confpath"},
        QString("configuration file path (default: ") +
            DEFAULT_CONFIG_FILE_PATH,
        QString("confpath"), DEFAULT_CONFIG_FILE_PATH);
    QCommandLineOption cachedir({"d", "cachedir"},
        QString("application cache <directory> (default: ") +
            DEFAULT_CACHE_DIR_PATH,
        QString("cachedir"), DEFAULT_CACHE_DIR_PATH);

    cmdline.addOption(logstdout);
    cmdline.addOption(confpath);
    cmdline.addOption(logfile);
    cmdline.addOption(cachedir);
    cmdline.addHelpOption();
    cmdline.addVersionOption();
    cmdline.process(app);
    /*
     * Setup Logfacility
     */
    if (cmdline.isSet(logstdout)) {
        setup_logger_stdout(); // Write log to stdout
    } else if (cmdline.isSet(logfile)) {
        setup_logger_file(cmdline.value(logfile));
    } else { // Default behavour as before
        setup_logger_file(DEFAULT_LOG_PATH);
    }
    qCInfo(MAIN) << "confpath: " << cmdline.value(confpath);
    qCInfo(MAIN) << "cachedir: " << cmdline.value(cachedir);
    qCInfo(MAIN) << APPLICATION_NAME << " - " << GIT_REVISION;
    qCDebug(MAIN) << "SSL Support: " << QSslSocket::supportsSsl()
                  << QSslSocket::sslLibraryVersionString();

    /*
     *  Initialize Hardware (or call stubs)
     */
    Hal::HardwareConfiguration hwcfg;
    Hal::HardwareControl hwctrl(hwcfg);

    /*
     * Read configuration
     */
    auto cm = std::make_shared<ConfigurationManager>(
        cmdline.value(confpath), cmdline.value(cachedir));
    cm->update_configuration();

    // Initialize Player
    auto playerproxy = std::make_shared<MediaPlayerProxy>();
    playerproxy->set_volume(cm->get_volume());

    AlarmDispatcher alarmdispatcher(cm);
    AlarmMonitor alarmmonitor(playerproxy, std::chrono::seconds(20));
    QObject::connect(&alarmdispatcher,
        SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)),
        &alarmmonitor,
        SLOT(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    PodcastSourceModel psmodel(cm, playerproxy);
    IRadioListModel iradiolistmodel(cm, playerproxy);
    AlarmListModel alarmlistmodel(cm);
    WifiListModel wifilistmodel;

    Weather weather(cm);
    SleepTimer sleeptimer(cm);

    /* Brightness control sends pwm update requests to Hardware */
    BrightnessControl brightness(cm);
    QObject::connect(&brightness, &BrightnessControl::brightness_pwm_change,
        &hwctrl, &Hal::HardwareControl::set_brightness);

    PowerControl power;
    /* PowerControl standby sets brightness */
    QObject::connect(&power, SIGNAL(going_in_standby()), &brightness,
        SLOT(restore_standby_brightness()));
    QObject::connect(&power, SIGNAL(becoming_active()), &brightness,
        SLOT(restore_active_brightness()));
    /* Powercontrol standby stops player */
    QObject::connect(
        &power, SIGNAL(going_in_standby()), playerproxy.get(), SLOT(stop()));
    /* Wire shutdown and reboot requests to hardware */
    QObject::connect(&power, &PowerControl::reboot_request, &hwctrl,
        &Hal::HardwareControl::system_reboot);
    QObject::connect(&power, &PowerControl::shutdown_request, &hwctrl,
        &Hal::HardwareControl::system_poweroff);

    /* AlarmDispatcher activates system */
    QObject::connect(
        &alarmdispatcher, SIGNAL(alarm_triggered()), &power, SLOT(activate()));

    /* Sleeptimer sends system to standby */
    QObject::connect(&sleeptimer, &SleepTimer::sleep_timer_elapsed, &power,
        &PowerControl::standby);
    /* Sleeptimer resets when player changes state to play */
    QObject::connect(playerproxy.get(), &MediaPlayer::playback_state_changed,
        &sleeptimer, &SleepTimer::playback_state_changed);
    /* Sleeptimer also monitors alarms */
    QObject::connect(&alarmdispatcher,
        SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)),
        &sleeptimer,
        SLOT(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    /* Rotary encoder push button interface */
    VolumeButton volbtn;
    /* connect volume button to hardware interface */
    QObject::connect(&hwctrl, &Hal::HardwareControl::button_event, &volbtn,
        &VolumeButton::process_key_event);
    QObject::connect(&hwctrl, &Hal::HardwareControl::rotary_event, &volbtn,
        &VolumeButton::process_rotary_event);

    /* wire volume button to consumers */
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

    /*
     * QML Setup
     */
    qmlRegisterType<PodcastEpisodeModel>(
        "ruschi.PodcastEpisodeModel", 1, 0, "PodcastEpisodeModel");
    qmlRegisterType<DigitalRooster::PodcastEpisode>(
        "ruschi.PodcastEpisode", 1, 0, "PodcastEpisode");
    qmlRegisterType<DigitalRooster::Alarm>("ruschi.Alarm", 1, 0, "Alarm");
    qmlRegisterType<DigitalRooster::IRadioListModel>(
        "ruschi.IRadioListModel", 1, 0, "IRadioListModel");
    qmlRegisterType<DigitalRooster::PlayableItem>(
        "ruschi.PlayableItem", 1, 0, "PlayableItem");
    qmlRegisterType<DigitalRooster::WifiListModel>(
        "ruschi.WifiListModel", 1, 0, "WifiListModel");

    QQmlApplicationEngine view;
    QQmlContext* ctxt = view.rootContext();

    WifiControl* wifictrl = WifiControl::get_instance(cm.get());
    ctxt->setContextProperty("wifictrl", wifictrl);
    ctxt->setContextProperty("wifilistmodel", &wifilistmodel);
    QObject::connect(wifictrl, &WifiControl::networks_found, &wifilistmodel,
        &WifiListModel::update_scan_results);

    ctxt->setContextProperty("podcastmodel", &psmodel);
    ctxt->setContextProperty("playerProxy", playerproxy.get());
    ctxt->setContextProperty("alarmlistmodel", &alarmlistmodel);
    ctxt->setContextProperty("iradiolistmodel", &iradiolistmodel);
    ctxt->setContextProperty("weather", &weather);
    ctxt->setContextProperty("config", cm.get());
    ctxt->setContextProperty("powerControl", &power);
    ctxt->setContextProperty("brightnessControl", &brightness);
    ctxt->setContextProperty("volumeButton", &volbtn);
    ctxt->setContextProperty("sleeptimer", &sleeptimer);
    ctxt->setContextProperty(
        "DEFAULT_ICON_WIDTH", QVariant::fromValue(DEFAULT_ICON_WIDTH));

    view.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
