// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

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

        EXPECT_CALL(cm, get_weather_config())
            .Times(AtLeast(1))
            .WillRepeatedly(ReturnRef(weather_cfg));
    };

protected:
    QFile weatherFile;
    QFile forecastFile;
    WeatherConfig weather_cfg;
    CmMock cm;
};

/*****************************************************************************/
TEST_F(WeatherFile, CheckSignals) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(weather_info_updated()));
    ASSERT_TRUE(spy.isValid());
    QSignalSpy spy2(&dut, SIGNAL(forecast_available()));
    ASSERT_TRUE(spy2.isValid());
    QSignalSpy spy3(&dut, SIGNAL(city_updated(const QString&)));
    ASSERT_TRUE(spy3.isValid());
    QSignalSpy spy4(&dut, SIGNAL(temperature_changed(double)));
    ASSERT_TRUE(spy4.isValid());
    QSignalSpy spy5(&dut, SIGNAL(icon_changed(const QUrl&)));
    ASSERT_TRUE(spy5.isValid());
}

/*****************************************************************************/
TEST_F(WeatherFile, GetConfigForDownloadAfterTimerExpired) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(weather_info_updated()));
    dut.set_update_interval(seconds(1));
    ASSERT_EQ(dut.get_update_interval(), std::chrono::seconds(1));
    spy.wait(500);  // first download
    spy.wait(1500); // only to make sure time elapses during test
}

/*****************************************************************************/
TEST_F(WeatherFile, parseWeatherTemperatures) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(temperature_changed(double)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    auto arguments = spy.takeFirst();
    ASSERT_DOUBLE_EQ(arguments.at(0).toDouble(), 16.7);

    ASSERT_FLOAT_EQ(dut.get_temperature(), 16.7);
    ASSERT_DOUBLE_EQ(dut.get_weather(0)->get_temperature(), 16.7);
    ASSERT_DOUBLE_EQ(dut.get_weather(0)->get_min_temperature(), 15.1);
    ASSERT_DOUBLE_EQ(dut.get_weather(0)->get_max_temperature(), 18.4);
}

/*****************************************************************************/
TEST_F(WeatherFile, parseWeatherCity) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(city_updated(const QString&)));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    auto arguments = spy.takeFirst();
    QString expected_city("Porto Alegre");
    ASSERT_EQ(arguments.at(0).toString(), expected_city);
    ASSERT_EQ(dut.get_city(), expected_city);
}

/*****************************************************************************/
TEST_F(WeatherFile, parseWeatherIconUrl) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(weather_info_updated()));
    dut.parse_weather(weatherFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(
        dut.get_weather_icon_url(), QUrl(WEATHER_ICON_BASE_URL + "02d.png"));
}

/*****************************************************************************/
TEST_F(WeatherFile, parseWeatherJsonInvalid) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(city_updated(const QString&)));

    ASSERT_TRUE(spy.isValid());
    auto crap = QByteArray::fromStdString(
        R"({This:"looks_like", "JSON":[], but:"is not!"})");
    dut.parse_weather(crap);
    spy.wait(10);
    EXPECT_EQ(spy.count(), 0);
}

/*****************************************************************************/
TEST_F(WeatherFile, parseWeatherNoTemperature) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(temperature_changed(double)));
    ASSERT_TRUE(spy.isValid());
    auto no_temp = QByteArray::fromStdString(R"({
                "main": {
                  "XXXXX": 16,
                  "temp_min": 16,
                  "temp_max": 16
                },
                "dt": 1534251600,
                "name": "Porto Alegre"
                })");
    dut.parse_weather(no_temp);
    // no signal should fire!
    spy.wait(10);
    EXPECT_EQ(spy.count(), 0);
    // should not have changed
    ASSERT_DOUBLE_EQ(dut.get_weather(0)->get_temperature(), 0);
}

/*****************************************************************************/
TEST_F(WeatherFile, parseWeatherNoCity) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(city_updated(const QString&)));
    ASSERT_TRUE(spy.isValid());
    // forecast does not have the correct format
    dut.parse_weather(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 0);
}


/*****************************************************************************/
TEST_F(WeatherFile, parseForecastsDoesNotTouchCurrent) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    dut.parse_forecast(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_DOUBLE_EQ(dut.get_weather(0)->get_temperature(), 0);
    ASSERT_DOUBLE_EQ(dut.get_weather(1)->get_temperature(), 25.06);
}

/*****************************************************************************/
TEST_F(WeatherFile, parseForecastBadJSON) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    ASSERT_TRUE(spy.isValid());
    auto crap = QByteArray::fromStdString(
        R"({This:"looks_like", "JSON":[], but:"is not!"})");
    dut.parse_forecast(crap);
    spy.wait(10);
    EXPECT_EQ(spy.count(), 0);

    auto empty_forecast =
        QByteArray::fromStdString(R"({  "main": {}, "list": [] })");
    dut.parse_forecast(empty_forecast);
    spy.wait(10);
    EXPECT_EQ(spy.count(), 0);
}

/*****************************************************************************/
TEST_F(WeatherFile, ForecastOutOfRange) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    dut.parse_forecast(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_FALSE(dut.get_weather(-1));
    ASSERT_TRUE(dut.get_weather(1));
    ASSERT_FALSE(dut.get_weather(100));
}

/*****************************************************************************/
TEST_F(WeatherFile, ForeCastOk) {
    Weather dut(cm);
    QSignalSpy spy(&dut, SIGNAL(forecast_available()));
    dut.parse_forecast(forecastFile.readAll());
    spy.wait(10);
    EXPECT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_weather(2)->get_weather_icon_url(),
        QUrl(WEATHER_ICON_BASE_URL + "10n.png"));

    ASSERT_DOUBLE_EQ(dut.get_weather(1)->get_temperature(), 25.06);
    ASSERT_DOUBLE_EQ(dut.get_weather(1)->get_min_temperature(), 25.06);
    ASSERT_DOUBLE_EQ(dut.get_weather(1)->get_max_temperature(), 26.16);

    ASSERT_EQ(
        dut.get_weather(2)->get_timestamp().toSecsSinceEpoch(), 1584813600);
}
/*****************************************************************************/
TEST(WeatherStatus, updateAcceptsBadJson) {
    WeatherStatus dut;
    ASSERT_NO_THROW(dut.update(QJsonObject()));

    auto empty_json = QByteArray::fromStdString(R"( { })");
    auto doc = QJsonDocument::fromJson(empty_json);
    ASSERT_NO_THROW(dut.update(doc.object()));
}

/*****************************************************************************/
TEST(WeatherStatus, validateBadJson) {
    auto crap = QByteArray::fromStdString(
        R"({This:"looks_like", "JSON":[], but:"is not!"})");
    QJsonDocument doc = QJsonDocument::fromJson(crap);
    EXPECT_THROW(
        WeatherStatus::validate_json(doc.object()), std::runtime_error);
}

/*****************************************************************************/
TEST(WeatherStatus, validateBadJsonNoMain) {
    auto crap_no_main = QByteArray::fromStdString(
        R"( { "dt": 1584792000, 
              "weather": [
                         {
                         "id": 500,
                         "main": "Rain",
                         "description": "light rain",
                         "icon": "10n"
                         }
                         ]
    })");
    auto doc = QJsonDocument::fromJson(crap_no_main);
    EXPECT_THROW(
        WeatherStatus::validate_json(doc.object()), std::runtime_error);
}

/*****************************************************************************/
TEST(WeatherStatus, validateBadJsonEmptyWeather) {
    auto crap_empty_weather = QByteArray::fromStdString(
        R"( { "dt": 1584792000,
              "main": {
                      "temp": 24.78,
                      "temp_min": 24.78,
                      "temp_max": 26.25
                      }, 
                  "weather": []
        })");
    QJsonParseError parse_err;
    auto doc = QJsonDocument::fromJson(crap_empty_weather, &parse_err);
    ASSERT_EQ(parse_err.error, QJsonParseError::NoError);
    EXPECT_THROW(
        WeatherStatus::validate_json(doc.object()), std::runtime_error);
}

/*****************************************************************************/
TEST(WeatherStatus, validateBadJsonNoTimestamp) {
    auto json_no_dt = QByteArray::fromStdString(
        R"( { "XX": 1584792000, 
              "main": {
                      "temp": 24.78,
                      "temp_min": 24.78,
                      "temp_max": 26.25
                      },
              "weather": [
                      {
                      "id": 500,
                      "main": "Rain",
                      "description": "light rain",
                      "icon": "10n"
                      }
                      ]
     })");
    QJsonParseError parse_err;
    auto doc = QJsonDocument::fromJson(json_no_dt, &parse_err);
    ASSERT_EQ(parse_err.error, QJsonParseError::NoError);
    EXPECT_THROW(
        WeatherStatus::validate_json(doc.object()), std::runtime_error);
}

/*****************************************************************************/
TEST(WeatherStatus, validateBadJsonTempNotDouble) {
    auto json_temp_not_double = QByteArray::fromStdString(
        R"( { "dt": 1584792000, 
              "main": {
                      "temp": "Hallo",
                      "temp_min": 24.78,
                      "temp_max": 26.25
                      },
              "weather": [
                      {
                      "id": 500,
                      "main": "Rain",
                      "description": "light rain",
                      "icon": "10n"
                      }
                      ]
     })");
    QJsonParseError parse_err;
    auto doc = QJsonDocument::fromJson(json_temp_not_double, &parse_err);
    ASSERT_EQ(parse_err.error, QJsonParseError::NoError);
    EXPECT_THROW(
        WeatherStatus::validate_json(doc.object()), std::runtime_error);
}
/*****************************************************************************/
TEST(WeatherStatus, validateBadJsonTempMinMissing) {
    auto json_temp_min = QByteArray::fromStdString(
        R"( { "dt": 1584792000, 
              "main": {
                      "temp": 15.9,
                      "temp_max": 26.25
                      },
              "weather": [
                      {
                      "id": 500,
                      "main": "Rain",
                      "description": "light rain",
                      "icon": "10n"
                      }
                      ]
     })");
    QJsonParseError parse_err;
    auto doc = QJsonDocument::fromJson(json_temp_min, &parse_err);
    ASSERT_EQ(parse_err.error, QJsonParseError::NoError);
    EXPECT_THROW(
        WeatherStatus::validate_json(doc.object()), std::runtime_error);
}

/*****************************************************************************/
TEST(WeatherCfg, fromJsonGood) {
    auto json_string = QString(R"(
        {
            "apiKey": "Secret",
            "locationId": "ABCD",
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
TEST(WeatherCfg, throwEmptyJson) {
    auto json_string = QString(R"(
        {}
        )");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());

    ASSERT_THROW(
        WeatherConfig::from_json_object(jdoc.object()), std::invalid_argument);
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
