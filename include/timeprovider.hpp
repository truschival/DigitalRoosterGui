/******************************************************************************
 * \filename
 * \brief abstraction interface for QDateTime and/or QTime to allow for mocking
 *        current time
 *
 * \details Periodically polls weather info
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _TIMEPROVIDER_HPP_
#define _TIMEPROVIDER_HPP_

#include <QDateTime>
#include <memory>

namespace DigitalRooster {

/**
 * Simple wrapper class around QDateTime to allow a fake wallclock during test
 */
class TimeProvider {
public:
    /**
     * Get current time and date
     * Public Non-Virtual Interface \ref get_time
     * @return current Date and time
     */
    QDateTime now() {
        return get_time();
    };

    virtual ~TimeProvider() = default;

private:
    /**
     * Implementation can be mocked
     */
    virtual QDateTime get_time() {
        return QDateTime::currentDateTime();
    }
};

/**
 * Global clock instance
 */
extern std::shared_ptr<TimeProvider> wallclock;

} // namespace DigitalRooster
#endif /* _TIMEPROVIDER_HPP_ */
