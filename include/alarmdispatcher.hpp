/******************************************************************************
 * \filename
 * \brief  Dispatches Alarm to player when due
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#ifndef _ALARMDISPATCHER_HPP_
#define _ALARMDISPATCHER_HPP_

#include <QObject>
#include <QTimer>
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
    AlarmDispatcher(ConfigurationManager* confman, QObject* parent = nullptr);

signals:
    void alarm_triggered(std::shared_ptr<DigitalRooster::Alarm>);

private:
    /**
     * Central configuration and data handler
     */
    ConfigurationManager* cm;
    /**
     * Timer for periodic polling
     */
    QTimer interval_timer;
    /**
     * Time interval to check for alarm dispatching and if new alarms are
     * available in configuration
     */
    int interval = 30000;

public slots:
    /**
     * Will walk alarms and check if ready for dispatch
     * */
    void check_alarms();
};
} // namespace DigitalRooster

#endif /*_ALARMDISPATCHER_HPP_*/
