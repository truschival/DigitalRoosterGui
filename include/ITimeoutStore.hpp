/******************************************************************************
 * \filename
 * \brief	Interface for creating/update and deletion Application Timeout
 *          Settings
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef INCLUDE_ITIMEOUTSTORE_HPP_
#define INCLUDE_ITIMEOUTSTORE_HPP_

#include <chrono>

namespace DigitalRooster {
/**
 * Interface for addition, access of Timeout Settings
 * actual storage \ref DigitalRooster::ConfigurationManager
 */
class ITimeOutStore {
public:
    /**
     * Access configuration when Alarm should stop automatically
     * @return default alarm timeout
     */
    virtual std::chrono::minutes get_alarm_timeout() const = 0;

    /**
     * Minutes after which DigitalRooster goes in standby
     * @return \ref sleep_timeout
     */
    virtual std::chrono::minutes get_sleep_timeout() const = 0;

    /**
     * Update sleep timeout Minutes after which DigitalRooster goes in standby
     * @param timeout \ref sleep_timeout
     */
    virtual void set_sleep_timeout(std::chrono::minutes timeout) = 0;
    /**
     * virtual destructor
     */
    virtual ~ITimeOutStore(){};
};
} // namespace DigitalRooster

#endif /* INCLUDE_ITIMEOUTSTORE_HPP_ */
