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
#include <QSettings>
#include <QSignalSpy>
#include <QStandardPaths>

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "appconstants.hpp"
#include "config.h"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

class SettingsFixture : public virtual ::testing::Test {
public:
    SettingsFixture()
        : filename(TEST_FILE_PATH + "/" + CONFIG_JSON_FILE_NAME),
		  cm(TEST_FILE_PATH){
    }

    ~SettingsFixture() {
    }

    void SetUp() {
        add_podcast_sources(appconfig);
        add_internet_radio(appconfig);
        add_alarms(appconfig);
        add_weather_config(appconfig);
        QJsonDocument doc(appconfig);
        QFile tf(filename);
        tf.open(QIODevice::ReadWrite | QIODevice::Text);
        tf.write(doc.toJson());
        tf.close();
        cm.update_configuration();
    }

    void TearDown() {
        QFile file(filename);
        file.remove();
    }

protected:
    QString filename;
    QJsonObject appconfig;
    ConfigurationManager cm;

    void add_podcast_sources(QJsonObject& root) {
        QJsonArray radiosources;
        QJsonObject dradio;
        dradio[KEY_NAME] = "DRadio";
        dradio[KEY_URI] = "http://dradio.de";
        QJsonObject swr2;
        swr2[KEY_NAME] = "SWR2";
        swr2[KEY_URI] = "http://swr2.de";
        radiosources.append(dradio);
        radiosources.append(swr2);
        root[KEY_GROUP_IRADIO_SOURCES] = radiosources;
    }

    void add_internet_radio(QJsonObject& root) {
        QJsonArray pcsources;
        QJsonObject pc1;
        pc1[KEY_NAME] = "Alternativlos";
        pc1[KEY_URI] = "https://alternativlos.org/alternativlos.rss";
        QJsonObject pc2;
        pc2[KEY_NAME] = "Dradio Essay&Diskurs";
        pc2[KEY_URI] = "http://www.deutschlandfunk.de/"
                       "podcast-essay-und-diskurs.1185.de.podcast.xml";
        pcsources.append(pc1);
        pcsources.append(pc2);
        root[KEY_GROUP_PODCAST_SOURCES] = pcsources;
    }

    void add_alarms(QJsonObject& root) {
        QJsonArray alarms;
        QJsonObject al1;
        al1[KEY_TIME] = "10:00";
        al1[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al1[KEY_ALARM_PERIOD] = "daily";
        al1[KEY_ENABLED] = true;
        al1[KEY_ID] = 1;

        QJsonObject al2;
        al2[KEY_TIME] = "07:00";
        al2[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al2[KEY_ALARM_PERIOD] = "workdays";
        al2[KEY_ENABLED] = true;
        al2[KEY_ID] = 2;

        QJsonObject al3;
        al3[KEY_TIME] = "09:00";
        al3[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al3[KEY_ALARM_PERIOD] = "weekend";
        al3[KEY_ENABLED] = false;
        al3[KEY_ID] = 3;

        QJsonObject al4;
        al4[KEY_TIME] = "13:00";
        al4[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al4[KEY_ALARM_PERIOD] = "once";
        al4[KEY_ENABLED] = true;
        al4[KEY_ID] = 4;

        alarms.append(al1);
        alarms.append(al2);
        alarms.append(al3);
        alarms.append(al4);

        root[KEY_GROUP_ALARMS] = alarms;
    }

    void add_weather_config(QJsonObject& root) {
        /* Store Weather information*/
        QJsonObject json_weather;
        json_weather[KEY_WEATHER_API_KEY] =
            QString("d77bd1ca2fd77ce4e1cdcdd5f8b7206c");
        json_weather[KEY_WEATHER_LOCATION_ID] = QString("3452925");
        root[KEY_WEATHER] = json_weather;
    }
};

/*****************************************************************************/

TEST_F(SettingsFixture, read_radio_streams_two_streams) {
    auto& v = cm.get_stream_sources();
    ASSERT_EQ(2, v.size());
}
/*****************************************************************************/

TEST_F(SettingsFixture, addRadioStation_no_write) {
    cm.add_radio_station(
        std::make_shared<PlayableItem>("foo", QUrl("http://bar.baz")));
    cm.add_radio_station(
        std::make_shared<PlayableItem>("ref", QUrl("http://gmx.net")));
    auto& v = cm.get_stream_sources();
    ASSERT_EQ(4, v.size());
}
/*****************************************************************************/

TEST_F(SettingsFixture, addRadioStation_write) {
    {
        cm.add_radio_station(
            std::make_shared<PlayableItem>("foo", QUrl("http://bar.baz")));
        cm.add_radio_station(
            std::make_shared<PlayableItem>("ref", QUrl("http://gmx.net")));
        /* should write file in destructor */
        cm.store_current_config();
    }
    ConfigurationManager control(TEST_FILE_PATH);
    control.update_configuration();
    auto& v = control.get_stream_sources();
    ASSERT_EQ(4, v.size());

    auto stream = v[2];
    ASSERT_EQ(stream->get_display_name(), QString("foo"));
}
/*****************************************************************************/

TEST_F(SettingsFixture, read_2podcasts) {
    auto& v = cm.get_podcast_sources();
    ASSERT_EQ(2, v.size());
}
/*****************************************************************************/

TEST_F(SettingsFixture, read_PodcastUri) {
    auto& v = cm.get_podcast_sources();
    ASSERT_EQ(v[0]->get_url(),
        QString("https://alternativlos.org/alternativlos.rss"));
    ASSERT_EQ(v[1]->get_url(),
        QString("http://www.deutschlandfunk.de/"
                "podcast-essay-und-diskurs.1185.de.podcast.xml"));
}
/*****************************************************************************/

TEST_F(SettingsFixture, podcastSource_incomplete) {
    auto& v = cm.get_podcast_sources();
    ASSERT_EQ(v[0]->get_url(),
        QString("https://alternativlos.org/alternativlos.rss"));
    ASSERT_EQ(v[1]->get_description(), QString(""));
    ASSERT_EQ(v[1]->get_episodes_names().size(), 0);
}
/*****************************************************************************/

TEST(StringToPeriodEnum, mapping_good) {
    ASSERT_EQ(Alarm::Once, json_string_to_alarm_period(KEY_ALARM_ONCE));
    ASSERT_EQ(Alarm::Daily, json_string_to_alarm_period(KEY_ALARM_DAILY));
    ASSERT_EQ(Alarm::Weekend, json_string_to_alarm_period(KEY_ALARM_WEEKEND));
    ASSERT_EQ(Alarm::Workdays, json_string_to_alarm_period(KEY_ALARM_WORKDAYS));
}
/*****************************************************************************/

TEST(StringToPeriodEnum, mapping_bad) {
    EXPECT_THROW(json_string_to_alarm_period("Foobar"), std::exception);
}
/*****************************************************************************/
TEST_F(SettingsFixture, alarm_count) {
    auto& v = cm.get_alarms();
    ASSERT_EQ(v.size(), 4);
}
/*****************************************************************************/
TEST_F(SettingsFixture, alarm_id) {
    auto& v = cm.get_alarms();
    auto res = std::find_if(v.begin(), v.end(),
            [&](const std::shared_ptr<Alarm>& item) {
                return item->get_id() == 2;
            });
    ASSERT_NE(res,v.end());
    ASSERT_EQ( (*res)->get_period(), Alarm::Workdays);
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_daily) {
    auto& v = cm.get_alarms();
    ASSERT_EQ(v[0]->get_period(), Alarm::Daily);
    ASSERT_EQ(v[0]->get_time(), QTime::fromString("10:00", "hh:mm"));
    ASSERT_TRUE(v[0]->is_enabled());
}
/*****************************************************************************/

TEST_F(SettingsFixture, alarm_workdays) {
    auto& v = cm.get_alarms();

    ASSERT_EQ(v[1]->get_period(), Alarm::Workdays);
    ASSERT_EQ(v[1]->get_time(), QTime::fromString("07:00", "hh:mm"));
    ASSERT_TRUE(v[1]->is_enabled());
}
/*****************************************************************************/

TEST_F(SettingsFixture, alarm_weekends) {
    auto& v = cm.get_alarms();
    ASSERT_EQ(v[2]->get_period(), Alarm::Weekend);
    ASSERT_EQ(v[2]->get_time(), QTime::fromString("09:00", "hh:mm"));
    ASSERT_FALSE(v[2]->is_enabled());
}
/*****************************************************************************/

TEST_F(SettingsFixture, alarm_once) {
    auto& v = cm.get_alarms();
    ASSERT_EQ(v[3]->get_period(), Alarm::Once);
    ASSERT_EQ(v[3]->get_time(), QTime::fromString("13:00", "hh:mm"));
    ASSERT_TRUE(v[3]->is_enabled());
}
/*****************************************************************************/

TEST_F(SettingsFixture, emitConfigChanged) {
    auto number_of_alarms = cm.get_alarms().size();
    QSignalSpy spy(&cm, SIGNAL(configuration_changed()));
    ASSERT_TRUE(spy.isValid());
    cm.update_configuration();
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(cm.get_alarms().size(), number_of_alarms);
}

/*****************************************************************************/
TEST(ConfigManager, CreateDefaultConfigDir) {
    auto config_path =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir config_dir(config_path);
    config_dir.remove(".");

    ConfigurationManager cm;
    cm.update_configuration();
    ASSERT_TRUE(config_dir.exists());
}
/*****************************************************************************/

TEST(ConfigManager, CreateDefaultConfig) {
    auto config_path =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir config_dir(config_path);
    config_dir.removeRecursively();
    auto file_path = config_dir.filePath(CONFIG_JSON_FILE_NAME);
    QFile config_file(file_path);

    ConfigurationManager cm;
    cm.update_configuration();

    ASSERT_GT(config_file.size(), 0);
}

/*****************************************************************************/
TEST_F(SettingsFixture, GetweatherConfigApiToken) {
    auto cfg = cm.get_weather_config();
    ASSERT_EQ(cfg.apikey, QString("d77bd1ca2fd77ce4e1cdcdd5f8b7206c"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, GetweatherConfigCityId) {
    auto cfg = cm.get_weather_config();
    ASSERT_EQ(cfg.cityid, QString("3452925"));
}

/*****************************************************************************/
