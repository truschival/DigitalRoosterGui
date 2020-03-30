/******************************************************************************
 * \filename
 * \brief     Tests for RestAdapter
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QJsonArray>

#include <chrono>
#include <vector>

#include <gtest/gtest.h>
#include <pistache/http.h>

#include "REST/RestAdapter.hpp"
#include "cm_mock.hpp" /* mock configuration manager */

using namespace DigitalRooster;
using namespace std;
using namespace std::chrono;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/
TEST(RestAdapter, getUintNotAvailable) {
    auto query = Pistache::Optional<std::string>();
    ASSERT_EQ(get_uint_from_query_with_default(query, 2), 2);
}

/*****************************************************************************/
TEST(RestAdapter, getUintNegative) {
    Pistache::Optional<std::string> query = Pistache::Some<std::string>("-123");
    ASSERT_EQ(get_uint_from_query_with_default(query, 100), 100);
}

/*****************************************************************************/
TEST(RestAdapter, getUintUnConvertable) {
    Pistache::Optional<std::string> query = Pistache::Some<std::string>("FOO");
    ASSERT_EQ(get_uint_from_query_with_default(query, 100), 100);
}

/*****************************************************************************/
TEST(RestAdapter, cfr_LengthTooLarge) {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 2);
    auto r = DigitalRooster::container_from_range(v, 2, 10);
    ASSERT_EQ(r[0], 4);
    ASSERT_EQ(r.size(), 8);
}
/*****************************************************************************/


