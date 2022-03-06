// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QString>
#include <QTime>

#include <memory>
#include <vector>

#include "appconstants.hpp"
#include "configuration.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class CmMock : public DigitalRooster::Configuration {
public:
    CmMock(const QString& configpath = QString(DigitalRooster::TEST_FILE_PATH +
               "/" + DigitalRooster::CONFIG_JSON_FILE_NAME),
        const QString& cachedir = QString(
            DigitalRooster::TEST_FILE_PATH + "/" + "cache"))
        : Configuration(configpath, cachedir) {
        qRegisterMetaType<std::shared_ptr<DigitalRooster::Alarm>>(
            "std::shared_ptr<DigitalRooster::Alarm>");
    };
    MOCK_CONST_METHOD0(
        get_alarms, std::vector<std::shared_ptr<DigitalRooster::Alarm>>&());

    MOCK_CONST_METHOD0(
        get_weather_config, const DigitalRooster::WeatherConfig&());

    MOCK_CONST_METHOD0(get_active_brightness, int());
    MOCK_CONST_METHOD0(get_standby_brightness, int());
    MOCK_CONST_METHOD0(backlight_control_enabled, bool());
    MOCK_CONST_METHOD0(do_get_volume, double());
    MOCK_METHOD1(set_active_brightness, void(int ab));
    MOCK_METHOD1(set_standby_brightness, void(int sb));
    MOCK_CONST_METHOD0(get_wpa_socket_name, QString());
    MOCK_CONST_METHOD0(get_alarm_timeout, std::chrono::minutes());
    MOCK_CONST_METHOD0(get_sleep_timeout, std::chrono::minutes());


    std::vector<std::shared_ptr<DigitalRooster::Alarm>> alarms;

    std::unique_ptr<DigitalRooster::WeatherConfig> weather_cfg;
};
