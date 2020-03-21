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
        : weatherFile(TEST_FILE_PATH + "/sample_weather.json")
        , forecastFile(TEST_FILE_PATH + "/sample_forecast.json")
        , weather_cfg(
              QString("a904431b4e0eae431bcc1e075c761abb"), QString("2172797")) {

        if (!weatherFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << weatherFile.errorString();
            throw std::system_error(
                make_error_code(std::errc::no_such_file_or_directory),
                weatherFile.errorString().toStdString());
        }

        if (!forecastFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << forecastFile.errorString();
            throw std::system_error(
                make_error_code(std::errc::no_such_file_or_directory),
                forecastFile.errorString().toStdString());
        }
    };

protected:
    QFile weatherFile;
    QFile forecastFile;
    WeatherConfig weather_cfg;
    CmMock cm;
};

/*****************************************************************************/
TEST_F(WeatherFile, RefreshEmitsSignal) {
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
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
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
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
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
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
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
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(temperature_changed(double)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(
        dut.get_weather_icon_url(), QUrl(WEATHER_ICON_BASE_URL + "02d.png"));
}


/*****************************************************************************/
TEST_F(WeatherFile, parseForecasts5) {
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    ASSERT_TRUE(spy.isValid());
    dut.parse_forecast(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_forecasts().size(), 5);
}

/*****************************************************************************/
TEST_F(WeatherFile, ForecastOutOfRange) {
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    ASSERT_TRUE(spy.isValid());
    dut.parse_forecast(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_TRUE(std::isnan(dut.get_forecast_temperature(-1)));
    ASSERT_TRUE(dut.get_forecast_icon_url(-1).isEmpty());
    ASSERT_FALSE(dut.get_forecast_timestamp(-1).isValid());

    ASSERT_TRUE(std::isnan(dut.get_forecast_temperature(6)));
    ASSERT_TRUE(dut.get_forecast_icon_url(6).isEmpty());
    ASSERT_FALSE(dut.get_forecast_timestamp(6).isValid());
}

/*****************************************************************************/
TEST_F(WeatherFile, ForeCastOk) {
    EXPECT_CALL(cm, get_weather_config())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(weather_cfg));

    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    ASSERT_TRUE(spy.isValid());
    dut.parse_forecast(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_forecasts().size(), 5);
    ASSERT_EQ(
        dut.get_forecast_icon_url(2), QUrl(WEATHER_ICON_BASE_URL + "10n.png"));
    ASSERT_DOUBLE_EQ(dut.get_forecast_temperature(2), 25.05);
    ASSERT_EQ(dut.get_forecast_timestamp(2).toSecsSinceEpoch(), 1584813600);

    ASSERT_EQ(
        dut.get_forecast_icon_url(4), QUrl(WEATHER_ICON_BASE_URL + "10d.png"));
    ASSERT_DOUBLE_EQ(dut.get_forecast_temperature(4), 26.51);
    ASSERT_EQ(dut.get_forecast_timestamp(4).toSecsSinceEpoch(), 1584835200);
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
