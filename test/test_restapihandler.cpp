// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <ApiHandler.hpp>
#include <QJsonArray>

#include <chrono>
#include <vector>
#include <optional>
#include <gtest/gtest.h>
#include <pistache/http.h>

#include "common.hpp"
#include "config_mock.hpp" /* mock configuration manager */

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

using namespace std;
using namespace std::chrono;
using namespace ::testing;
using ::testing::AtLeast;

/*****************************************************************************/
TEST(RestAdapter, getUintNotAvailableDefaultToMin) {
    auto query = std::optional<std::string>{};
    ASSERT_EQ(get_val_from_query_within_range(query, 2, 5), 2);
}

/*****************************************************************************/
TEST(RestAdapter, getUintNegativeDefaultToMin) {
    auto query = std::optional<std::string>{"-123"};
    ASSERT_EQ(get_val_from_query_within_range(query, 5, 100), 5);
}

/*****************************************************************************/
TEST(RestAdapter, getUintUnConvertableDefaultToMin) {
    auto query = std::optional<std::string>{"FOO"};
    ASSERT_EQ(get_val_from_query_within_range(query, 5, 100), 5);
}

/*****************************************************************************/
TEST(RestAdapter, getUintOutOfRangeCapMax) {
    auto query = std::optional<std::string>{"100"};
    ASSERT_EQ(get_val_from_query_within_range(query, 5, 50), 50);
}

/*****************************************************************************/
TEST(RestAdapter, cfrGood) {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 2);
    auto r = container_from_range(v, 2, 10);
    ASSERT_EQ(r[0], 4);
    ASSERT_EQ(r.size(), 8);
}

/*****************************************************************************/
TEST(RestAdapter, cfrOffsetAtEnd) {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 2);
    auto r = container_from_range(v, 10, 0);
    ASSERT_EQ(r.size(), 0);
}

/*****************************************************************************/
TEST(RestAdapter, cfrFull) {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 2);
    auto r = container_from_range(v, 0, 10);
    ASSERT_EQ(r.size(), 10);
}

/*****************************************************************************/
TEST(RestAdapter, cfrOutOfBounds) {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 2);
    auto r = container_from_range(v, 8, 5);
    ASSERT_EQ(r.size(), 2);
}

/*****************************************************************************/
TEST(RestAdapter, cfrEndAndOutOfBounds) {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 2);
    auto r = container_from_range(v, 10, 5);
    ASSERT_EQ(r.size(), 0);
}

/*****************************************************************************/
