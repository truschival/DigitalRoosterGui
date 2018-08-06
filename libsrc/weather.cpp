/******************************************************************************
 * \filename
 * \brief  parse weather information form openweathermaps
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QDebug>
#include <QJsonObject>
#include <QString>
#include <appconstants.hpp>
#include <memory>
#include <stdexcept> // std::system_error

#include "weather.hpp"
using namespace DigitalRooster;

/*****************************************************************************/
Weather::Weather(
    std::shared_ptr<ConfigurationManager> confman, QObject* parent): 
	update_interval(60) {

}

/*****************************************************************************/
void Weather::set_update_interval(std::chrono::seconds interval) {
    update_interval = interval;
    if (interval.count() < timer.remainingTime())
        timer.start(update_interval);
}

/*****************************************************************************/