/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QTime>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "configuration_manager.hpp"

class CmMock : public DigitalRooster::ConfigurationManager {
public:
    CmMock() {
        qRegisterMetaType<std::shared_ptr<DigitalRooster::Alarm>>(
            "std::shared_ptr<DigitalRooster::Alarm>");

        weather_cfg.cityid = "2172797"; // Cairns, AU
        weather_cfg.language = "de";
        weather_cfg.units = "metric";

    };
    MOCK_METHOD0(
        get_alarm_list, QVector<std::shared_ptr<DigitalRooster::Alarm>>&());

    MOCK_METHOD0(get_weather_cfg, DigitalRooster::WeatherConfig&());

    MOCK_CONST_METHOD0(do_get_brightness_sb, int());
    MOCK_CONST_METHOD0(do_get_brightness_act, int());
    MOCK_CONST_METHOD0(do_get_volume, int());
    MOCK_METHOD1(do_set_brightness_act, void(int ab));
    MOCK_CONST_METHOD0(get_wpa_socket_name, QString());
    MOCK_CONST_METHOD0(get_alarm_timeout, std::chrono::minutes());
    MOCK_CONST_METHOD0(get_sleep_timeout, std::chrono::minutes());


    QVector<std::shared_ptr<DigitalRooster::Alarm>> alarms;

    DigitalRooster::WeatherConfig weather_cfg;
};
