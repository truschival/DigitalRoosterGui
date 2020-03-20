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
        : weatherFile(TEST_FILE_PATH + "/sample_weather.json"){
        if (!weatherFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << weatherFile.errorString();
            throw std::system_error(
                make_error_code(std::errc::no_such_file_or_directory),
                weatherFile.errorString().toStdString());
        }
    };

protected:
    QFile weatherFile;
    CmMock cm;
};

/*****************************************************************************/
TEST_F(WeatherFile, RefreshEmitsSignal) {
    auto weather_cfg = DigitalRooster::WeatherConfig(
        QString("a904431b4e0eae431bcc1e075c761abb"), QString("2172797"));
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(weather_info_updated()));
    ASSERT_TRUE(spy.isValid());
    spy.wait(500);
    dut.refresh();
    spy.wait(1500); // make sure we have enough time to download info
    ASSERT_EQ(spy.count(), 2);
}

/*****************************************************************************/
TEST_F(WeatherFile, GetConfigForDownloadAfterTimerExpired) {
    auto weather_cfg = DigitalRooster::WeatherConfig(
        QString("a904431b4e0eae431bcc1e075c761abb"), QString("2172797"));
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(weather_info_updated()));
    ASSERT_TRUE(spy.isValid());
    dut.set_update_interval(seconds(1));
    ASSERT_EQ(dut.get_update_interval(), std::chrono::seconds(1));
    spy.wait(500);  // first download
    spy.wait(1500); // only to make sure time elapses during test
}

/*****************************************************************************/
TEST_F(WeatherFile, ParseTemperatureFromFile) {
    auto weather_cfg =
        DigitalRooster::WeatherConfig(QString("ABC"), QString("2172797"));
    EXPECT_CALL(cm, get_weather_config())
        .Times(1)
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(temperature_changed(double)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_FLOAT_EQ(dut.get_temperature(), 16);
}
/*****************************************************************************/
TEST_F(WeatherFile, GetCityFromFile) {
    auto weather_cfg =
        DigitalRooster::WeatherConfig(QString("ABC"), QString("2172797"));
    EXPECT_CALL(cm, get_weather_config())
        .Times(1)
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(city_updated(const QString&)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_city(), QString("Porto Alegre"));
}

/*****************************************************************************/
TEST_F(WeatherFile, ParseConditionFromFile) {
    auto weather_cfg =
        DigitalRooster::WeatherConfig(QString("ABC"), QString("2172797"));
    EXPECT_CALL(cm, get_weather_config())
        .Times(1)
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(condition_changed(const QString&)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_condition(), QString("few clouds"));
}

/*****************************************************************************/
TEST_F(WeatherFile, IconURI) {
    auto weather_cfg =
        DigitalRooster::WeatherConfig(QString("ABC"), QString("2172797"));
    EXPECT_CALL(cm, get_weather_config())
        .Times(1)
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(temperature_changed(double)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_weather_icon_url(),
        QUrl("http://openweathermap.org/img/w/02d.png"));
}

/*****************************************************************************/
TEST(WeatherCfg, fromJsonGood) {
    auto json_string = QString(R"(
	{
	    "API-Key": "Secret",
        "locationID": "ABCD",
		"updateInterval": 123
	}
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    auto dut = WeatherConfig::from_json_object(jdoc.object());
    ASSERT_EQ(dut.get_api_token(), QString("Secret"));
    ASSERT_EQ(dut.get_location_id(), QString("ABCD"));
    ASSERT_EQ(dut.get_update_interval(), std::chrono::seconds(123));
}

/*****************************************************************************/
TEST(WeatherCfg, throwEmptyLocation) {
    auto json_string = QString(R"(
	{
	    "API-Key": "Secret",
        "locationID": "",
		"updateInterval": 123
	}
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());

    ASSERT_THROW(
        WeatherConfig::from_json_object(jdoc.object()), std::invalid_argument);
}

/*****************************************************************************/
TEST(WeatherCfg, throwNoApiToken) {
    auto json_string = QString(R"(
	{
        "locationID": "ABCD",
		"updateInterval": 123
	}
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());

    ASSERT_THROW(
        WeatherConfig::from_json_object(jdoc.object()), std::invalid_argument);
}
/*****************************************************************************/
