// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

/* only allow QML debugging for Debug builds */
#ifndef NDEBUG
#define QT_QML_DEBUG
#endif

#include <QCommandLineParser>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QtQuick>
// STD C++
#include <memory>

// hardware interface
#include "hardware_configuration.hpp"

#ifdef HARDWARE_STUB
#include "hardwarecontrol_stub.hpp"
#else
#include "hardwarecontrol_mk3.hpp"
#endif

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
#include "networkinfo.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"
#include "powercontrol.hpp"
#include "sleeptimer.hpp"
#include "timeprovider.hpp"
#include "util.hpp"
#include "volume_button.hpp"
#include "weather.hpp"
#include "wifi_control.hpp"
#include "wifilistmodel.hpp"

// REST interface - optional
#include "RestApi.hpp"

using namespace DigitalRooster;

Q_DECLARE_LOGGING_CATEGORY(MAIN)
Q_LOGGING_CATEGORY(MAIN, "qtgui")

/*****************************************************************************
 * Application constants
 ****************************************************************************/
/**
 * Log file path
 */
const QString DigitalRooster::DEFAULT_LOG_FILE(
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
        .filePath(APPLICATION_NAME.toLower()));

/**
 * Global wall clock
 */
std::shared_ptr<TimeProvider> DigitalRooster::wallclock =
    std::make_shared<TimeProvider>();

/*****************************************************************************/
int main(int argc, char* argv[]) {
    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    /* Force 96 DPI independent of display DPI by OS */
    // QCoreApplication::setAttribute(Qt::AA_Use96Dpi);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);
    QGuiApplication app(argc, argv);

    /*
     * Setup Commandline Parser
     */
    const auto& cmdline = get_commandline_options(app,
        "DigitalRooster - Your hackable alarm clock, podcast player and "
        "Internet radio"
        "\n\tCopyright (c) 2020 Thomas Ruschival");
    /*
     * Setup Logfacility
     */
    setup_log_facility(cmdline);

    double dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    qCInfo(MAIN) << "DPI (physical):"
                 << QGuiApplication::primaryScreen()->physicalDotsPerInch();
    qCInfo(MAIN) << "DPI (logical):" << dpi;
    qCInfo(MAIN) << "Screen Size:" << QGuiApplication::primaryScreen()->size();
    qCInfo(MAIN) << "DevicePixelRatio:"
                 << QGuiApplication::primaryScreen()->devicePixelRatio();
    qCInfo(MAIN) << APPLICATION_NAME << " - " << GIT_REVISION;
    qCDebug(MAIN) << "SSL Support: " << QSslSocket::supportsSsl()
                  << QSslSocket::sslLibraryVersionString();

/*
 *  Initialize Hardware (or call stubs)
 *  Could make a Factory Pattern - is it worth it?
 */
#ifdef HARDWARE_STUB
    Hal::HardwareControlStub hwctrl;
#else
    Hal::HardwareConfiguration hwcfg;
    Hal::HardwareControlMk3 hwctrl(hwcfg);
#endif
    /*
     * Read configuration
     */
    ConfigurationManager cm(
        cmdline.value(CMD_ARG_CONFIG_FILE), cmdline.value(CMD_ARG_CACHE_DIR));
    cm.update_configuration();

    // Initialize Player
    MediaPlayerProxy playerproxy;
    playerproxy.set_volume(cm.get_volume());

    AlarmDispatcher alarmdispatcher(cm);
    QObject::connect(&cm, &ConfigurationManager::alarms_changed,
        &alarmdispatcher, &AlarmDispatcher::check_alarms);
    AlarmMonitor alarmmonitor(playerproxy, std::chrono::seconds(20));
    QObject::connect(&alarmdispatcher, &AlarmDispatcher::alarm_triggered,
        &alarmmonitor, &AlarmMonitor::alarm_triggered);

    PodcastSourceModel psmodel(cm, playerproxy);
    AlarmListModel alarmlistmodel(cm);
    IRadioListModel iradiolistmodel(cm, playerproxy);
    WifiListModel wifilistmodel;

    Weather weather(cm);
    SleepTimer sleeptimer(cm);

    /* Brightness control sends pwm update requests to Hardware */
    BrightnessControl brightness(cm, &hwctrl);
    QObject::connect(&brightness, &BrightnessControl::brightness_changed,
        &hwctrl, &Hal::IHardware::set_backlight);
    QObject::connect(&hwctrl, &Hal::IHardware::als_value_changed, &brightness,
        &BrightnessControl::als_value_changed);

    PowerControl power;
    /* Power controls backlight */
    QObject::connect(
        &power, &PowerControl::active, &brightness, &BrightnessControl::active);
    /* Powercontrol standby stops player */
    QObject::connect(&power, &PowerControl::going_in_standby, &playerproxy,
        &MediaPlayer::stop);
    /* Wire shutdown and reboot requests to hardware */
    QObject::connect(&power, &PowerControl::reboot_request, &hwctrl,
        &Hal::IHardware::system_reboot);
    QObject::connect(&power, &PowerControl::shutdown_request, &hwctrl,
        &Hal::IHardware::system_poweroff);

    /* AlarmDispatcher activates system */
    QObject::connect(&alarmdispatcher, &AlarmDispatcher::alarm_triggered,
        &power, &PowerControl::activate);

    /* Sleeptimer sends system to standby */
    QObject::connect(&sleeptimer, &SleepTimer::sleep_timer_elapsed, &power,
        &PowerControl::standby);
    /* Sleeptimer resets when player changes state to play */
    QObject::connect(&playerproxy, &MediaPlayer::playback_state_changed,
        &sleeptimer, &SleepTimer::playback_state_changed);
    /* Sleeptimer also monitors alarms */
    QObject::connect(&alarmdispatcher, &AlarmDispatcher::alarm_triggered,
        &sleeptimer, &SleepTimer::alarm_triggered);

    /* Rotary encoder push button interface */
    VolumeButton volbtn;
    /* connect volume button to hardware interface */
    QObject::connect(&hwctrl, &Hal::IHardware::button_event, &volbtn,
        &VolumeButton::process_key_event);
    QObject::connect(&hwctrl, &Hal::IHardware::rotary_event, &volbtn,
        &VolumeButton::process_rotary_event);

    /* wire volume button to consumers */
    QObject::connect(&volbtn, &VolumeButton::button_released, &power,
        &PowerControl::toggle_power_state);
    QObject::connect(&volbtn, &VolumeButton::volume_incremented, &playerproxy,
        &MediaPlayer::increment_volume);

    /* Standby deactivates Volume button events */
    QObject::connect(&power, &PowerControl::active, &volbtn,
        &VolumeButton::monitor_rotary_button);
    QObject::connect(&playerproxy, &MediaPlayer::volume_changed, &cm,
        &ConfigurationManager::set_volume);


    /* we start in standby */
    power.standby();

#ifdef REST_API
    RestApi rest(cm, cm, cm, cm, cm);
#endif
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

    NetworkInfo netinfo(cm.get_net_dev_name());

    ctxt->setContextProperty("podcastmodel", &psmodel);
    ctxt->setContextProperty("playerProxy", &playerproxy);
    ctxt->setContextProperty("alarmlistmodel", &alarmlistmodel);
    ctxt->setContextProperty("iradiolistmodel", &iradiolistmodel);
    ctxt->setContextProperty("weather", &weather);
    ctxt->setContextProperty("config", &cm);
    ctxt->setContextProperty("alarmdispatcher", &alarmdispatcher);
    ctxt->setContextProperty("powerControl", &power);
    ctxt->setContextProperty("brightnessControl", &brightness);
    ctxt->setContextProperty("volumeButton", &volbtn);
    ctxt->setContextProperty("sleeptimer", &sleeptimer);
    ctxt->setContextProperty("netinfo", &netinfo);

    ctxt->setContextProperty(
        "DEFAULT_ICON_WIDTH", QVariant::fromValue(DEFAULT_ICON_WIDTH));
    ctxt->setContextProperty("FONT_SCALING", QVariant::fromValue(dpi));

    view.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
