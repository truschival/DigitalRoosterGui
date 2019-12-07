/*******************************************************************************
 * \filename 
 * \brief	Allows mocking of the wallclock during test
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 ******************************************************************************/

#ifndef _INCLUDE_MOCK_CLOCK_
#define _INCLUDE_MOCK_CLOCK_
// Mock Wallclock to test weekends & workdays
#include "timeprovider.hpp"

class MockClock : public DigitalRooster::TimeProvider {
public:
    MOCK_METHOD0(get_time, QDateTime());
};
/*****************************************************************************/

#endif /*_INCLUDE_MOCK_CLOCK_*/
