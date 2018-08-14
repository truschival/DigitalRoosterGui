/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QDebug>
#include <QSignalSpy>
#include <QTime>
#include <QUrl>
#include <chrono>
#include <gtest/gtest.h>

#include "cm_mock.hpp" /* mock configuration manager */
#include "weather.hpp"

using namespace DigitalRooster;
using namespace std;
using namespace std::chrono;
using namespace ::testing;
using ::testing::AtLeast;


class WeatherFile : public virtual ::testing::Test {
public:
    WeatherFile()
        : weatherFile(TEST_FILE_PATH + "/sample_weather.json") {
        if (!weatherFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << weatherFile.errorString();
            throw std::system_error(
                make_error_code(std::errc::no_such_file_or_directory),
                weatherFile.errorString().toStdString());
        }
    }

protected:
    QFile weatherFile;
};
/*****************************************************************************/
TEST(Weather, GetConfigForDownloadAfterTimerExpired) {
    auto cm = std::make_shared<CmMock>();
    EXPECT_CALL(*(cm.get()), get_weather_cfg())
        .Times(1)
        .WillRepeatedly(ReturnRef(cm->weather_cfg));

    Weather dut(cm);
    dut.set_update_interval(seconds(1));

    QSignalSpy spy(&dut, SIGNAL(weather_info_available(const QJsonObject&)));
    ASSERT_TRUE(spy.isValid());
    spy.wait(1500); // only to make sure time elapses during test
    // ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/
TEST_F(WeatherFile, ParseTemperatureFromFile) {
    auto cm = std::make_shared<CmMock>();
    Weather dut(cm);

    QSignalSpy spy(&dut, SIGNAL(temperature_changed(double)));
    dut.parse_response(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_FLOAT_EQ(dut.get_temperature(), 16);
}
/*****************************************************************************/
TEST_F(WeatherFile, GetCityFromFile) {
    auto cm = std::make_shared<CmMock>();
    Weather dut(cm);

    QSignalSpy spy(&dut, SIGNAL(city_updated(const QString&)));
    dut.parse_response(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_city(), QString("Porto Alegre"));
}

/*****************************************************************************/
TEST_F(WeatherFile, ParseConditionFromFile) {
    auto cm = std::make_shared<CmMock>();
    Weather dut(cm);

    QSignalSpy spy(&dut, SIGNAL(condition_changed(const QString&)));
    dut.parse_response(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_condition(), QString("few clouds"));
}
