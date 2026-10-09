// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <ApiHandler.hpp>
#include <QJsonArray>

#include <QCoreApplication>
#include <chrono>
#include <future>
#include <thread>
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

/*****************************************************************************/
template <typename T>
T wait_processing_events(std::future<T>& fut) {
    while (fut.wait_for(10ms) != std::future_status::ready) {
        QCoreApplication::processEvents();
    }
    return fut.get();
}

/*****************************************************************************/
TEST(RestAdapter, runInThreadOfExecutesInContextThread) {
    QObject ctx;
    std::thread::id exec_thread;
    auto fut = std::async(std::launch::async, [&]() {
        return run_in_thread_of(ctx, [&]() {
            exec_thread = std::this_thread::get_id();
            return 42;
        });
    });
    ASSERT_EQ(wait_processing_events(fut), 42);
    ASSERT_EQ(exec_thread, std::this_thread::get_id());
}

/*****************************************************************************/
TEST(RestAdapter, runInThreadOfPropagatesExceptions) {
    QObject ctx;
    auto fut = std::async(std::launch::async, [&]() {
        run_in_thread_of(ctx, []() { throw std::out_of_range("no item"); });
    });
    ASSERT_THROW(wait_processing_events(fut), std::out_of_range);
}

/*****************************************************************************/
TEST(RestAdapter, getValOverflowCapsAtLimits) {
    ASSERT_EQ(get_val_from_query_within_range(
                  std::optional<std::string>{"99999999999"}, 0, 50),
        50);
    ASSERT_EQ(get_val_from_query_within_range(
                  std::optional<std::string>{"-99999999999"}, 0, 50),
        0);
}

/*****************************************************************************/
TEST(RestAdapter, getValInvalidReturnsFallback) {
    auto query = std::optional<std::string>{"abc"};
    ASSERT_EQ(get_val_from_query_within_range(query, 1, 10, 10), 10);
}

/*****************************************************************************/
TEST(RestAdapter, errorJsonIsEscaped) {
    std::invalid_argument exc(R"(bad "quoted" input)");
    InternalErrorJson je(exc, 400);
    QJsonParseError perr;
    auto doc = QJsonDocument::fromJson(
        QByteArray::fromStdString(std::string(je)), &perr);
    ASSERT_EQ(perr.error, QJsonParseError::NoError);
    ASSERT_EQ(doc.object()["code"].toInt(), 400);
    ASSERT_EQ(doc.object()["message"].toString(),
        QString(R"(bad "quoted" input)"));
}

/*****************************************************************************/
TEST(RestAdapter, invalidJsonIsNotEchoed) {
    try {
        qjson_form_std_string("{ secret body");
        FAIL();
    } catch (std::invalid_argument& exc) {
        ASSERT_EQ(std::string(exc.what()).find("secret"), std::string::npos);
    }
}
