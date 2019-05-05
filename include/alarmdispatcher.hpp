/******************************************************************************
 * \filename
 * \brief  Dispatches Alarm to player when due
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _ALARMDISPATCHER_HPP_
#define _ALARMDISPATCHER_HPP_

#include <QObject>
#include <QTimer>
#include <chrono>
#include <memory>

#include "alarm.hpp"
#include "mediaplayerproxy.hpp"

namespace DigitalRooster {
class ConfigurationManager;

/**
 * Monitors changes in alarm configuration and dispatches alarms when due
 */
class AlarmDispatcher : public QObject {
    Q_OBJECT
public:
    /**
     * Constructor for AlarmDispatcher
     * @param confman
     * @param parent
     */
    AlarmDispatcher(std::shared_ptr<ConfigurationManager> confman,
        QObject* parent = nullptr);

    /**
     * Update alarm check interval
     * @param iv interval in seconds
     */
    void set_interval(std::chrono::seconds iv);

    /**
     * Current alarm check interval
     * @return interval in seconds
     */
    std::chrono::seconds get_interval();

public slots:
    /**
     * Will walk alarms and check if ready for dispatch
     * */
    void check_alarms();

signals:
    /**
     * Signal receivers interested specific alarm content
     * @param alarm triggered alarm
     */
    void alarm_triggered(std::shared_ptr<DigitalRooster::Alarm> alarm);
   
	/**
     * Signal generic receivers if any alarm is triggered
     */
    void alarm_triggered();

private:
    /**
     * Central configuration and data handler
     */
    std::shared_ptr<ConfigurationManager> cm;
    /**
     * Timer for periodic polling
     */
    QTimer interval_timer;
    /**
     * Time interval to check for alarm dispatching and if new alarms are
     * available in configuration
     */
    std::chrono::seconds interval;

    /**
     * Convenience helper method
     * @param alarm to dispatch
     */
    void dispatch(std::shared_ptr<DigitalRooster::Alarm> alarm);
};
} // namespace DigitalRooster

#endif /*_ALARMDISPATCHER_HPP_*/
