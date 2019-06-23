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

#include "gtest/gtest.h"

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
        : filename(DEFAULT_CONFIG_FILE_PATH+"_SettingsFixture")
        , cache_dir(DEFAULT_CACHE_DIR_PATH) {
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
        QFileInfo file_info(tf);
        QDir().mkpath(file_info.path());
        tf.open(QIODevice::ReadWrite | QIODevice::Text);
        tf.write(doc.toJson());
        tf.close();

        cm = std::make_unique<ConfigurationManager>(filename, cache_dir);
        cm->update_configuration();
    }

    void TearDown() {
        QFileInfo file_info(filename);
        QDir(file_info.dir()).removeRecursively();

        QDir cd(cache_dir);
        cd.removeRecursively();
    }

protected:
    QString filename;
    QString cache_dir;
    QJsonObject appconfig;
    std::unique_ptr<ConfigurationManager> cm;

    void add_internet_radio(QJsonObject& root) {
        QJsonArray radiosources;
        QJsonObject dradio;
        dradio[KEY_NAME] = "DRadio";
        dradio[KEY_ID] = "a754affb-fd4b-4825-9eba-32b64fd7d50c";
        dradio[KEY_URI] = "http://dradio.de";
        QJsonObject swr2;
        swr2[KEY_NAME] = "SWR2";
        swr2[KEY_ID] = "c98a9c37-f990-47eb-ab73-21daf48d43c0";
        swr2[KEY_URI] = "http://swr2.de";
        radiosources.append(dradio);
        radiosources.append(swr2);
        root[KEY_GROUP_IRADIO_SOURCES] = radiosources;
    }

    void add_podcast_sources(QJsonObject& root) {
        QJsonArray pcsources;
        QJsonObject pc1;
        pc1[KEY_NAME] = "Alternativlos";
        pc1[KEY_ID] = "ab25693c-aae1-40dc-8946-8d2b670d4598";
        pc1[KEY_URI] = "https://alternativlos.org/alternativlos.rss";
        QJsonObject pc2;
        pc2[KEY_NAME] = "Dradio Essay&Diskurs";
        pc2[KEY_ID] = "a754affb-fd4b-4825-9eba-32b64fd7d50c";
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
        al1[KEY_ID] = "1a4bf6bd-7e67-4b40-80fd-b13e2524fc74";

        QJsonObject al2;
        al2[KEY_TIME] = "07:00";
        al2[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al2[KEY_ALARM_PERIOD] = "workdays";
        al2[KEY_ENABLED] = true;
        al2[KEY_ID] = "12eb4390-6abf-4626-be48-f11fe20f45cf";

        QJsonObject al3;
        al3[KEY_TIME] = "09:00";
        al3[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al3[KEY_ALARM_PERIOD] = "weekend";
        al3[KEY_ENABLED] = false;
        al3[KEY_ID] = "62ab05d7-d9ab-4254-8bfd-47bfdc74417a";

        QJsonObject al4;
        al4[KEY_TIME] = "13:00";
        al4[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al4[KEY_ALARM_PERIOD] = "once";
        al4[KEY_ENABLED] = true;
        al4[KEY_ID] = "fa3ce587-ab02-4328-9c68-4ee5e3626c86";

        QJsonObject al5;
        al5[KEY_TIME] = "17:00";
        al5[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al5[KEY_ALARM_PERIOD] = "Manchmal";
        al5[KEY_ENABLED] = true;
        al5[KEY_ID] = "694485e9-ac44-46f5-bc45-730a7a0ac387";

        alarms.append(al1);
        alarms.append(al2);
        alarms.append(al3);
        alarms.append(al4);
        alarms.append(al5);

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
    auto& v = cm->get_stream_sources();
    ASSERT_EQ(2, v.size());
}
/*****************************************************************************/

TEST_F(SettingsFixture, addRadioStation_no_write) {
    cm->add_radio_station(
        std::make_shared<PlayableItem>("foo", QUrl("http://bar.baz")));
    cm->add_radio_station(
        std::make_shared<PlayableItem>("ref", QUrl("http://gmx.net")));
    auto& v = cm->get_stream_sources();
    ASSERT_EQ(4, v.size());
}
/*****************************************************************************/

TEST_F(SettingsFixture, addRadioStation_write) {
    {
        cm->add_radio_station(
            std::make_shared<PlayableItem>("foo", QUrl("http://bar.baz")));
        cm->add_radio_station(
            std::make_shared<PlayableItem>("ref", QUrl("http://gmx.net")));
        /* should write file in destructor */
        cm->store_current_config();
    }
    ConfigurationManager control(filename, TEST_FILE_PATH);
    control.update_configuration();
    auto& v = control.get_stream_sources();
    ASSERT_EQ(4, v.size());

    auto stream = v[2];
    ASSERT_EQ(stream->get_display_name(), QString("foo"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, read_2podcasts) {
    auto& v = cm->get_podcast_sources();
    ASSERT_EQ(2, v.size());
}

/*****************************************************************************/
TEST_F(SettingsFixture, deletePodcastById) {
    auto& v = cm->get_podcast_sources();
    ASSERT_EQ(2, v.size());
    cm->remove_podcast_source_by_index(0);
    ASSERT_EQ(cm->get_podcast_sources().size(), 1);
}

/*****************************************************************************/
TEST_F(SettingsFixture, read_PodcastUri) {
    auto ps = cm->get_podcast_source_by_index(0);
    ASSERT_EQ(
        ps->get_url(), QString("https://alternativlos.org/alternativlos.rss"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, podcastSource_incomplete) {
    auto& v = cm->get_podcast_sources();
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
TEST_F(SettingsFixture, alarm_count) {
    auto& v = cm->get_alarms();
    ASSERT_EQ(v.size(), 5);
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_id) {
    auto& v = cm->get_alarms();
    auto res = std::find_if(
        v.begin(), v.end(), [&](const std::shared_ptr<Alarm>& item) {
            return item->get_id() ==
                QUuid("12eb4390-6abf-4626-be48-f11fe20f45cf");
        });
    ASSERT_NE(res, v.end());
    ASSERT_EQ((*res)->get_period(), Alarm::Workdays);
}

/*****************************************************************************/
TEST_F(SettingsFixture, podcastid) {
    auto& v = cm->get_podcast_sources();
    auto res = std::find_if(
        v.begin(), v.end(), [&](const std::shared_ptr<PodcastSource>& item) {
            return item->get_id() ==
                QUuid("a754affb-fd4b-4825-9eba-32b64fd7d50c");
        });
    ASSERT_NE(res, v.end());
    ASSERT_EQ((*res)->get_url(),
        QString("http://www.deutschlandfunk.de/"
                "podcast-essay-und-diskurs.1185.de.podcast.xml"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, streamsourceid) {
    auto& v = cm->get_stream_sources();
    auto res = std::find_if(
        v.begin(), v.end(), [&](const std::shared_ptr<PlayableItem>& item) {
            return item->get_id() ==
                QUuid("c98a9c37-f990-47eb-ab73-21daf48d43c0");
        });
    ASSERT_NE(res, v.end());
    ASSERT_EQ((*res)->get_url(), QString("http://swr2.de"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, addAlarm) {
    auto al = std::make_shared<Alarm>();
    auto size_before = cm->get_alarms().size();
    cm->add_alarm(al);
    ASSERT_EQ(cm->get_alarms().size(), size_before + 1);
}

/*****************************************************************************/
TEST_F(SettingsFixture, deleteAlarm) {
    auto al = std::make_shared<Alarm>();
    auto id = al->get_id();
    auto size_before = cm->get_alarms().size();
    cm->add_alarm(al);
    cm->delete_alarm(id);
    ASSERT_EQ(cm->get_alarms().size(), size_before);
}

/*****************************************************************************/
TEST_F(SettingsFixture, deleteAlarmNonExist) {
    auto size_before = cm->get_alarms().size();
    ASSERT_EQ(cm->delete_alarm(QUuid("XXX")), -1);
    ASSERT_EQ(cm->get_alarms().size(), size_before);
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_daily) {
    auto& v = cm->get_alarms();
    ASSERT_EQ(v[0]->get_period(), Alarm::Daily);
    ASSERT_EQ(v[0]->get_time(), QTime::fromString("10:00", "hh:mm"));
    ASSERT_TRUE(v[0]->is_enabled());
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_workdays) {
    auto& v = cm->get_alarms();

    ASSERT_EQ(v[1]->get_period(), Alarm::Workdays);
    ASSERT_EQ(v[1]->get_time(), QTime::fromString("07:00", "hh:mm"));
    ASSERT_TRUE(v[1]->is_enabled());
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_weekends) {
    auto& v = cm->get_alarms();
    ASSERT_EQ(v[2]->get_period(), Alarm::Weekend);
    ASSERT_EQ(v[2]->get_time(), QTime::fromString("09:00", "hh:mm"));
    ASSERT_FALSE(v[2]->is_enabled());
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_once) {
    auto& v = cm->get_alarms();
    ASSERT_EQ(v[3]->get_period(), Alarm::Once);
    ASSERT_EQ(v[3]->get_time(), QTime::fromString("13:00", "hh:mm"));
    ASSERT_TRUE(v[3]->is_enabled());
}

/*****************************************************************************/
TEST_F(SettingsFixture, alarm_once_default) {
    auto& v = cm->get_alarms();
    // Alarm 5 has an unknown peridicity string "Manchmal" it should default to
    // Daily
    ASSERT_EQ(v[4]->get_period(), Alarm::Daily);
}

/*****************************************************************************/
TEST_F(SettingsFixture, emitConfigChanged) {
    auto number_of_alarms = cm->get_alarms().size();
    QSignalSpy spy(cm.get(), SIGNAL(configuration_changed()));
    ASSERT_TRUE(spy.isValid());
    cm->update_configuration();
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(cm->get_alarms().size(), number_of_alarms);
}

/*****************************************************************************/
TEST(ConfigManager, CreateDefaultConfigDir) {
    auto config_path =
        QDir(QDir::tempPath()).filePath(QString("testdir_to_delete"));
    auto config_file_name = QDir(config_path).filePath(CONFIG_JSON_FILE_NAME);

    QDir config_dir(config_path);
    config_dir.removeRecursively();

    ConfigurationManager cm(config_file_name, DEFAULT_CACHE_DIR_PATH);
    cm.update_configuration();
    ASSERT_TRUE(QDir(config_path).exists());
}

/*****************************************************************************/
TEST(ConfigManager, CreateDefaultConfig) {
    auto config_path =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir config_dir(config_path);
    config_dir.removeRecursively();
    auto file_path = config_dir.filePath(CONFIG_JSON_FILE_NAME);

    ConfigurationManager cm(file_path, DEFAULT_CACHE_DIR_PATH);
    cm.update_configuration();

    QFile config_file(file_path);
    ASSERT_GT(config_file.size(), 0);
}

/*****************************************************************************/
TEST(ConfigManager, DefaultForNotWritableCache) {
    QDir default_cache_dir(DEFAULT_CACHE_DIR_PATH);
    default_cache_dir.removeRecursively();
    ASSERT_FALSE(default_cache_dir.exists());

    ConfigurationManager cm(DEFAULT_CONFIG_FILE_PATH, QString("/dev/"));

    ASSERT_TRUE(default_cache_dir.exists());
}

/*****************************************************************************/
TEST(ConfigManager, DefaultForNotWritableConfig) {
    QFile default_conf_file(DEFAULT_CONFIG_FILE_PATH);
    ASSERT_TRUE(default_conf_file.remove());
    ConfigurationManager cm(QString("/dev/foobar.json"),
        DEFAULT_CACHE_DIR_PATH);
    ASSERT_TRUE(default_conf_file.exists());
}

/*****************************************************************************/
TEST_F(SettingsFixture, GetweatherConfigApiToken) {
    auto cfg = cm->get_weather_config();
    ASSERT_EQ(cfg.apikey, QString("d77bd1ca2fd77ce4e1cdcdd5f8b7206c"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, GetweatherConfigCityId) {
    auto cfg = cm->get_weather_config();
    ASSERT_EQ(cfg.cityid, QString("3452925"));
}

/*****************************************************************************/
TEST_F(SettingsFixture, changeSleepTimeoutMinutes) {
    auto new_to = std::chrono::minutes(5);
    cm->set_sleep_timeout(new_to);
    ASSERT_EQ(cm->get_sleep_timeout(), new_to);
}

/*****************************************************************************/
