/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/


#include <QDateTime>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "configuration_manager.hpp"

class CmMock : public DigitalRooster::ConfigurationManager {
public:
    CmMock() {
        auto timepoint = QDateTime::currentDateTimeUtc().addSecs(3);
        auto alm = std::make_shared<DigitalRooster::Alarm>(
            QUrl("https://www.heise.de"), timepoint);
        alarms.push_back(alm);

        qRegisterMetaType<std::shared_ptr<DigitalRooster::Alarm>>(
            "std::shared_ptr<DigitalRooster::Alarm>");
    };
    MOCK_METHOD0(
        get_alarm_list, QVector<std::shared_ptr<DigitalRooster::Alarm>>&());

    QVector<std::shared_ptr<DigitalRooster::Alarm>> alarms;
};
