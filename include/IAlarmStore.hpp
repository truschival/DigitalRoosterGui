/******************************************************************************
 * \filename
 * \brief	Interface for creating/update and deletion of alams
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef INCLUDE_IALARMSTORE_HPP_
#define INCLUDE_IALARMSTORE_HPP_

#include <QUuid>
#include <memory>
#include <vector>

namespace DigitalRooster {
class Alarm;

class IAlarmStore {
public:
    /**
     * Append new alarm to list
     * @param alarm
     */
    virtual void add_alarm(std::shared_ptr<Alarm> alarm) = 0;
    /**
     * Delete an alarm identified by ID from the list of alarms
     * @param id of alarm
     * @throws std::out_of_range if not found
     */
    virtual void delete_alarm(const QUuid& id) = 0;

    /**
     * Get a alarm identified by ID
     * @throws 	 std::out_of_range if not found
     * @param id unique ID of Alarm
     * @return Alarm
     */
    virtual const Alarm* get_alarm(const QUuid& id) const = 0;

    /**
     * get Alarm List
     */
    virtual const std::vector<std::shared_ptr<Alarm>>& get_alarms() const = 0;
    /**
     * virtual destructor
     */
    virtual ~IAlarmStore(){};
};


} // namespace DigitalRooster

#endif /* INCLUDE_IALARMSTORE_HPP_ */
