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
#include "PlayableItem.hpp" // to register type
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
        .filePath(CONFIG_JSON_FILE_NAME.toLower()));

/**
 * Cache directory
 */
const QString DigitalRooster::DEFAULT_CACHE_DIR_PATH(
    QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation))
        .filePath(APPLICATION_NAME.toLower()));

/**
 * Global wall clock
 */
std::shared_ptr<TimeProvider> DigitalRooster::wallclock =
    std::make_shared<TimeProvider>();

/*****************************************************************************/
int main(int argc, char* argv[]) {
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    /* Force 96 DPI independent of display DPI by OS */
    // QCoreApplication::setAttribute(Qt::AA_Use96Dpi);
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
        try {
            setup_logger_file(cmdline.value(logfile));
        } catch (std::system_error& exc) {
            setup_logger_stdout(); // Write log to stdout
        }
    } else { // Default behavour as before
        setup_logger_file(DEFAULT_LOG_PATH);
    }

    double dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    qCInfo(MAIN) << "DPI (physical):" << QGuiApplication::primaryScreen()->physicalDotsPerInch();
    qCInfo(MAIN) << "DPI (logical):" << dpi;
    qCInfo(MAIN) << "Screen Size:" << QGuiApplication::primaryScreen()->size();
    qCInfo(MAIN) << "DevicePixelRatio:" << QGuiApplication::primaryScreen()->devicePixelRatio();

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
    ConfigurationManager cm(cmdline.value(confpath), cmdline.value(cachedir));
    cm.update_configuration();

    // Initialize Player
    MediaPlayerProxy playerproxy;
    playerproxy.set_volume(cm.get_volume());

    AlarmDispatcher alarmdispatcher(cm);
    AlarmMonitor alarmmonitor(playerproxy, std::chrono::seconds(20));
    QObject::connect(&alarmdispatcher,
        SIGNAL(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)),
        &alarmmonitor,
        SLOT(alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>)));

    PodcastSourceModel psmodel(cm, playerproxy);
    AlarmListModel alarmlistmodel(cm);
    IRadioListModel iradiolistmodel(cm, playerproxy);
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
    QObject::connect(&power, &PowerControl::going_in_standby, &playerproxy,
        &MediaPlayer::stop);
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
    QObject::connect(&playerproxy, &MediaPlayer::playback_state_changed,
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
    QObject::connect(&volbtn, &VolumeButton::button_released, &power,
        &PowerControl::toggle_power_state);
    QObject::connect(&volbtn, &VolumeButton::volume_incremented, &playerproxy,
        &MediaPlayer::increment_volume);

    /* Standby deactivates Volume button events */
    QObject::connect(&power, SIGNAL(active(bool)), &volbtn,
        SLOT(monitor_rotary_button(bool)));
    QObject::connect(&playerproxy, &MediaPlayer::volume_changed, &cm,
        &ConfigurationManager::set_volume);

    /* we start in standby */
    power.standby();

    /*
     * QML Setup dynamically createable types
     * All Elements/Lists are created in C++
     */
    qmlRegisterUncreatableType<DigitalRooster::PodcastEpisodeModel>(
        "ruschi.PodcastEpisodeModel", 1, 0, "PodcastEpisodeModel",
        "QML must not instantiate PodcastEpisodeModel!");
    qmlRegisterUncreatableType<DigitalRooster::PodcastEpisode>(
        "ruschi.PodcastEpisode", 1, 0, "PodcastEpisode",
        "QML must not instantiate PodcastEpisode!");
    qmlRegisterUncreatableType<DigitalRooster::Alarm>(
        "ruschi.Alarm", 1, 0, "Alarm", "QML must not instantiate Alarm!");
    qmlRegisterUncreatableType<DigitalRooster::PlayableItem>(
        "ruschi.PlayableItem", 1, 0, "PlayableItem",
        "QML must not instantiate PlayableItem!");
    qmlRegisterUncreatableType<DigitalRooster::WifiListModel>(
        "ruschi.WifiListModel", 1, 0, "WifiListModel",
        "QML must not instantiate WifiListModel!");
    qmlRegisterUncreatableType<DigitalRooster::WeatherStatus>(
        "ruschi.WeatherStatus", 1, 0, "WeatherStatus",
        "QML must not instantiate WeatherStatus!");

    QQmlApplicationEngine view;
    QQmlContext* ctxt = view.rootContext();

    WifiControl* wifictrl = WifiControl::get_instance(&cm);
    ctxt->setContextProperty("wifictrl", wifictrl);
    ctxt->setContextProperty("wifilistmodel", &wifilistmodel);
    QObject::connect(wifictrl, &WifiControl::networks_found, &wifilistmodel,
        &WifiListModel::update_scan_results);

    ctxt->setContextProperty("podcastmodel", &psmodel);
    ctxt->setContextProperty("playerProxy", &playerproxy);
    ctxt->setContextProperty("alarmlistmodel", &alarmlistmodel);
    ctxt->setContextProperty("iradiolistmodel", &iradiolistmodel);
    ctxt->setContextProperty("weather", &weather);
    ctxt->setContextProperty("config", &cm);
    ctxt->setContextProperty("powerControl", &power);
    ctxt->setContextProperty("brightnessControl", &brightness);
    ctxt->setContextProperty("volumeButton", &volbtn);
    ctxt->setContextProperty("sleeptimer", &sleeptimer);

    ctxt->setContextProperty(
        "DEFAULT_ICON_WIDTH", QVariant::fromValue(DEFAULT_ICON_WIDTH));
    ctxt->setContextProperty(
            "FONT_SCALING", QVariant::fromValue(dpi));

    view.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
